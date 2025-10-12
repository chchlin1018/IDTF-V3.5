"""
NDH Omniverse Connector

將 NDH 的工業資料同步到 NVIDIA Omniverse 平台,
實現即時的工業數位分身視覺化和多人協作。
"""

import asyncio
import logging
from typing import Dict, List, Any, Optional, Callable
from datetime import datetime
from dataclasses import dataclass
from enum import Enum

# Kafka
from kafka import KafkaConsumer
from kafka.errors import KafkaError

# OpenUSD (需要安裝 usd-core)
try:
    from pxr import Usd, UsdGeom, Sdf, Gf
    USD_AVAILABLE = True
except ImportError:
    USD_AVAILABLE = False
    logging.warning("OpenUSD (pxr) not available. Install with: pip install usd-core")

# Omniverse Client Library (需要安裝 omni.client)
try:
    import omni.client
    OMNI_CLIENT_AVAILABLE = True
except ImportError:
    OMNI_CLIENT_AVAILABLE = False
    logging.warning("Omniverse Client Library not available")


logger = logging.getLogger(__name__)


class ConnectionState(Enum):
    """連接狀態"""
    DISCONNECTED = "disconnected"
    CONNECTING = "connecting"
    CONNECTED = "connected"
    ERROR = "error"


@dataclass
class OmniverseConnectorConfig:
    """Omniverse 連接器配置"""
    # Kafka 配置
    kafka_bootstrap_servers: str = "localhost:9092"
    kafka_topics: List[str] = None
    kafka_group_id: str = "omniverse-connector"
    
    # Nucleus 配置
    nucleus_url: str = "omniverse://localhost"
    nucleus_username: Optional[str] = None
    nucleus_api_token: Optional[str] = None
    
    # Stage 配置
    stage_url: str = "omniverse://localhost/Projects/Factory/DigitalTwin.usd"
    update_rate: float = 10.0  # Hz
    
    # 批次更新配置
    batch_size: int = 100
    flush_interval: float = 0.1  # seconds
    
    # 資料過濾配置
    enable_filtering: bool = True
    min_update_interval: float = 0.1  # seconds
    value_change_threshold: float = 0.01  # 1%
    
    def __post_init__(self):
        if self.kafka_topics is None:
            self.kafka_topics = ["asset.telemetry", "asset.events", "asset.alarms"]


@dataclass
class USDUpdate:
    """USD 更新"""
    prim_path: str
    attr_name: str
    value: Any
    timestamp: Optional[datetime] = None


class IADLToUSDTransformer:
    """
    IADL 到 OpenUSD 轉換器
    """
    
    def __init__(self, mapping_config: Optional[Dict] = None):
        self.mapping = mapping_config or {}
        
    def transform(self, iadl_data: Dict[str, Any]) -> List[USDUpdate]:
        """
        將 IADL 資料轉換為 USD 更新列表
        
        Args:
            iadl_data: IADL 格式的資料
            
        Returns:
            USD 更新列表
        """
        updates = []
        
        try:
            # 提取資產 ID
            asset_id = iadl_data.get("asset_id") or iadl_data.get("id")
            if not asset_id:
                logger.warning("No asset_id found in IADL data")
                return updates
                
            # 映射到 USD Prim 路徑
            prim_path = self._map_asset_to_prim_path(asset_id)
            
            # 轉換屬性
            properties = iadl_data.get("properties", {})
            for prop_name, prop_value in properties.items():
                usd_attr = self._map_property_to_attribute(prop_name)
                converted_value = self._convert_value(prop_value, prop_name)
                
                updates.append(USDUpdate(
                    prim_path=prim_path,
                    attr_name=usd_attr,
                    value=converted_value,
                    timestamp=iadl_data.get("timestamp")
                ))
                
            # 處理狀態變化
            if "state" in iadl_data:
                visual_updates = self._generate_visual_updates(
                    prim_path,
                    iadl_data["state"]
                )
                updates.extend(visual_updates)
                
        except Exception as e:
            logger.error(f"Error transforming IADL data: {e}", exc_info=True)
            
        return updates
        
    def _map_asset_to_prim_path(self, asset_id: str) -> str:
        """將資產 ID 映射到 USD Prim 路徑"""
        # 簡單映射: /Factory/Line1/Motor01 -> /World/Factory/Line1/Motor01
        # 可以根據需要自定義映射規則
        if asset_id.startswith("/"):
            return f"/World{asset_id}"
        else:
            return f"/World/{asset_id}"
            
    def _map_property_to_attribute(self, property_name: str) -> str:
        """將屬性名稱映射到 USD 屬性名稱"""
        # 檢查映射配置
        if property_name in self.mapping.get("property_to_attribute", {}):
            return self.mapping["property_to_attribute"][property_name]
        # 默認使用 custom: 前綴
        return f"custom:{property_name}"
        
    def _convert_value(self, value: Any, property_name: str) -> Any:
        """轉換值的單位或格式"""
        # 可以根據需要添加單位轉換邏輯
        # 例如: 攝氏度 -> 開氏度
        return value
        
    def _generate_visual_updates(self, prim_path: str, state: str) -> List[USDUpdate]:
        """根據狀態生成視覺更新"""
        updates = []
        
        # 狀態到顏色的映射
        state_colors = {
            "ALARM": (1.0, 0.0, 0.0),      # 紅色
            "WARNING": (1.0, 1.0, 0.0),    # 黃色
            "RUNNING": (0.0, 1.0, 0.0),    # 綠色
            "IDLE": (0.5, 0.5, 0.5),       # 灰色
            "STOPPED": (0.3, 0.3, 0.3)     # 深灰色
        }
        
        if state in state_colors:
            color = state_colors[state]
            if USD_AVAILABLE:
                updates.append(USDUpdate(
                    prim_path=prim_path,
                    attr_name="primvars:displayColor",
                    value=Gf.Vec3f(*color)
                ))
            else:
                updates.append(USDUpdate(
                    prim_path=prim_path,
                    attr_name="primvars:displayColor",
                    value=color
                ))
                
        return updates


class BatchUpdateManager:
    """
    批次更新管理器
    減少 Nucleus 的寫入次數,提高效能
    """
    
    def __init__(self, batch_size: int = 100, flush_interval: float = 0.1):
        self.batch_size = batch_size
        self.flush_interval = flush_interval
        self.pending_updates: List[USDUpdate] = []
        self.last_flush_time = asyncio.get_event_loop().time()
        self.flush_callback: Optional[Callable] = None
        
    def set_flush_callback(self, callback: Callable):
        """設置刷新回調函數"""
        self.flush_callback = callback
        
    async def add_update(self, update: USDUpdate):
        """添加更新到批次"""
        self.pending_updates.append(update)
        
        # 檢查是否需要刷新
        current_time = asyncio.get_event_loop().time()
        should_flush = (
            len(self.pending_updates) >= self.batch_size or
            current_time - self.last_flush_time >= self.flush_interval
        )
        
        if should_flush:
            await self.flush()
            
    async def flush(self):
        """刷新所有待處理的更新"""
        if not self.pending_updates:
            return
            
        if self.flush_callback:
            try:
                await self.flush_callback(self.pending_updates)
            except Exception as e:
                logger.error(f"Error flushing updates: {e}", exc_info=True)
                
        # 清空批次
        self.pending_updates.clear()
        self.last_flush_time = asyncio.get_event_loop().time()


class DataFilter:
    """
    資料過濾器
    減少不必要的更新,提高效能
    """
    
    def __init__(self, min_update_interval: float = 0.1,
                 value_change_threshold: float = 0.01):
        self.min_update_interval = min_update_interval
        self.value_change_threshold = value_change_threshold
        self.last_values: Dict[str, Any] = {}
        self.last_update_time: Dict[str, float] = {}
        
    def should_update(self, key: str, new_value: Any) -> bool:
        """
        判斷是否需要更新
        
        Args:
            key: 唯一鍵 (例如: prim_path.attr_name)
            new_value: 新值
            
        Returns:
            是否應該更新
        """
        current_time = asyncio.get_event_loop().time()
        
        # 檢查時間間隔
        if key in self.last_update_time:
            elapsed = current_time - self.last_update_time[key]
            if elapsed < self.min_update_interval:
                return False
                
        # 檢查值變化
        if key in self.last_values:
            old_value = self.last_values[key]
            
            # 數值類型: 檢查變化百分比
            if isinstance(new_value, (int, float)) and isinstance(old_value, (int, float)):
                if old_value != 0:
                    change_percent = abs((new_value - old_value) / old_value)
                    if change_percent < self.value_change_threshold:
                        return False
            # 其他類型: 檢查是否相等
            elif new_value == old_value:
                return False
                
        # 更新記錄
        self.last_values[key] = new_value
        self.last_update_time[key] = current_time
        
        return True


class OmniverseConnector:
    """
    NDH Omniverse 連接器
    
    將 NDH 的工業資料同步到 Omniverse 平台
    """
    
    def __init__(self, config: OmniverseConnectorConfig):
        self.config = config
        self.state = ConnectionState.DISCONNECTED
        
        # Kafka Consumer
        self.kafka_consumer: Optional[KafkaConsumer] = None
        
        # USD Stage
        self.usd_stage: Optional[Usd.Stage] = None
        
        # 轉換器
        self.transformer = IADLToUSDTransformer()
        
        # 批次更新管理器
        self.batch_manager = BatchUpdateManager(
            batch_size=config.batch_size,
            flush_interval=config.flush_interval
        )
        self.batch_manager.set_flush_callback(self._apply_batch_updates)
        
        # 資料過濾器
        self.data_filter = None
        if config.enable_filtering:
            self.data_filter = DataFilter(
                min_update_interval=config.min_update_interval,
                value_change_threshold=config.value_change_threshold
            )
            
        # 統計
        self.stats = {
            "messages_received": 0,
            "updates_applied": 0,
            "updates_filtered": 0,
            "errors": 0
        }
        
        logger.info(f"OmniverseConnector initialized with config: {config}")
        
    async def connect(self):
        """連接到 Kafka 和 Nucleus"""
        self.state = ConnectionState.CONNECTING
        
        try:
            # 連接到 Kafka
            await self._connect_kafka()
            
            # 連接到 Nucleus
            await self._connect_nucleus()
            
            # 打開 USD Stage
            await self._open_stage()
            
            self.state = ConnectionState.CONNECTED
            logger.info("Successfully connected to Kafka and Nucleus")
            
        except Exception as e:
            self.state = ConnectionState.ERROR
            logger.error(f"Failed to connect: {e}", exc_info=True)
            raise
            
    async def _connect_kafka(self):
        """連接到 Kafka"""
        logger.info(f"Connecting to Kafka: {self.config.kafka_bootstrap_servers}")
        
        self.kafka_consumer = KafkaConsumer(
            *self.config.kafka_topics,
            bootstrap_servers=self.config.kafka_bootstrap_servers,
            group_id=self.config.kafka_group_id,
            value_deserializer=lambda m: m.decode('utf-8'),
            auto_offset_reset='latest',
            enable_auto_commit=True
        )
        
        logger.info(f"Connected to Kafka, subscribed to topics: {self.config.kafka_topics}")
        
    async def _connect_nucleus(self):
        """連接到 Nucleus"""
        if not OMNI_CLIENT_AVAILABLE:
            logger.warning("Omniverse Client Library not available, skipping Nucleus connection")
            return
            
        logger.info(f"Connecting to Nucleus: {self.config.nucleus_url}")
        
        # 設置認證
        if self.config.nucleus_username and self.config.nucleus_api_token:
            omni.client.set_authentication_message_box_enabled(False)
            # Note: 實際的認證方式可能需要根據 Omniverse 版本調整
            
        # 初始化 Omniverse Client
        result = omni.client.initialize()
        if result != omni.client.Result.OK:
            raise ConnectionError(f"Failed to initialize Omniverse Client: {result}")
            
        logger.info("Connected to Nucleus")
        
    async def _open_stage(self):
        """打開或創建 USD Stage"""
        if not USD_AVAILABLE:
            logger.warning("OpenUSD not available, skipping stage opening")
            return
            
        logger.info(f"Opening USD Stage: {self.config.stage_url}")
        
        try:
            # 嘗試打開現有 Stage
            self.usd_stage = Usd.Stage.Open(self.config.stage_url)
            logger.info("Opened existing USD Stage")
        except Exception as e:
            logger.warning(f"Failed to open existing stage: {e}")
            logger.info("Creating new USD Stage")
            # 創建新 Stage
            self.usd_stage = Usd.Stage.CreateNew(self.config.stage_url)
            
    async def start(self):
        """開始監聽和處理事件"""
        if self.state != ConnectionState.CONNECTED:
            raise RuntimeError("Connector not connected. Call connect() first.")
            
        logger.info("Starting event processing loop")
        
        try:
            # 使用 asyncio 運行 Kafka 消費者
            loop = asyncio.get_event_loop()
            
            while True:
                # 非阻塞地獲取訊息
                message_batch = await loop.run_in_executor(
                    None,
                    lambda: self.kafka_consumer.poll(timeout_ms=100, max_records=10)
                )
                
                for topic_partition, messages in message_batch.items():
                    for message in messages:
                        await self._process_message(message)
                        
                # 定期刷新批次更新
                await self.batch_manager.flush()
                
                # 短暫休眠
                await asyncio.sleep(0.01)
                
        except KeyboardInterrupt:
            logger.info("Received keyboard interrupt, stopping...")
        except Exception as e:
            logger.error(f"Error in event processing loop: {e}", exc_info=True)
            self.stats["errors"] += 1
        finally:
            await self.disconnect()
            
    async def _process_message(self, message):
        """處理單個 Kafka 訊息"""
        try:
            self.stats["messages_received"] += 1
            
            # 解析訊息
            import json
            iadl_data = json.loads(message.value)
            
            # 轉換為 USD 更新
            usd_updates = self.transformer.transform(iadl_data)
            
            # 過濾和批次處理更新
            for update in usd_updates:
                # 資料過濾
                if self.data_filter:
                    key = f"{update.prim_path}.{update.attr_name}"
                    if not self.data_filter.should_update(key, update.value):
                        self.stats["updates_filtered"] += 1
                        continue
                        
                # 添加到批次
                await self.batch_manager.add_update(update)
                
        except Exception as e:
            logger.error(f"Error processing message: {e}", exc_info=True)
            self.stats["errors"] += 1
            
    async def _apply_batch_updates(self, updates: List[USDUpdate]):
        """應用批次更新到 USD Stage"""
        if not self.usd_stage:
            logger.warning("USD Stage not available, skipping updates")
            return
            
        try:
            for update in updates:
                # 獲取 Prim
                prim = self.usd_stage.GetPrimAtPath(update.prim_path)
                
                if not prim:
                    # Prim 不存在,創建它
                    prim = self.usd_stage.DefinePrim(update.prim_path)
                    logger.debug(f"Created new prim: {update.prim_path}")
                    
                # 獲取或創建屬性
                attr = prim.GetAttribute(update.attr_name)
                if not attr:
                    # 根據值類型推斷屬性類型
                    if isinstance(update.value, bool):
                        attr = prim.CreateAttribute(update.attr_name, Sdf.ValueTypeNames.Bool)
                    elif isinstance(update.value, int):
                        attr = prim.CreateAttribute(update.attr_name, Sdf.ValueTypeNames.Int)
                    elif isinstance(update.value, float):
                        attr = prim.CreateAttribute(update.attr_name, Sdf.ValueTypeNames.Float)
                    elif isinstance(update.value, str):
                        attr = prim.CreateAttribute(update.attr_name, Sdf.ValueTypeNames.String)
                    else:
                        attr = prim.CreateAttribute(update.attr_name, Sdf.ValueTypeNames.Token)
                        
                # 設置值
                attr.Set(update.value)
                
            # 保存 Stage (觸發協作同步)
            self.usd_stage.Save()
            
            self.stats["updates_applied"] += len(updates)
            logger.debug(f"Applied {len(updates)} updates to USD Stage")
            
        except Exception as e:
            logger.error(f"Error applying batch updates: {e}", exc_info=True)
            self.stats["errors"] += 1
            
    async def disconnect(self):
        """斷開連接"""
        logger.info("Disconnecting...")
        
        # 刷新剩餘的更新
        await self.batch_manager.flush()
        
        # 關閉 Kafka Consumer
        if self.kafka_consumer:
            self.kafka_consumer.close()
            logger.info("Kafka consumer closed")
            
        # 關閉 USD Stage
        if self.usd_stage:
            self.usd_stage.Save()
            logger.info("USD Stage saved")
            
        # 關閉 Omniverse Client
        if OMNI_CLIENT_AVAILABLE:
            omni.client.shutdown()
            logger.info("Omniverse Client shutdown")
            
        self.state = ConnectionState.DISCONNECTED
        logger.info("Disconnected")
        
    def get_stats(self) -> Dict[str, int]:
        """獲取統計資訊"""
        return self.stats.copy()


# 便利函數
async def create_and_run_connector(config: OmniverseConnectorConfig):
    """
    創建並運行 Omniverse 連接器
    
    Args:
        config: 連接器配置
    """
    connector = OmniverseConnector(config)
    
    try:
        await connector.connect()
        await connector.start()
    except KeyboardInterrupt:
        logger.info("Received keyboard interrupt")
    finally:
        await connector.disconnect()
        
        # 打印統計資訊
        stats = connector.get_stats()
        logger.info("Final statistics:")
        for key, value in stats.items():
            logger.info(f"  {key}: {value}")

