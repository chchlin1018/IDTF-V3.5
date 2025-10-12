# NDH 系統後續工作建議與實施路線圖

## 目錄

1. [執行摘要](#執行摘要)
2. [短期目標 (1-2 個月)](#短期目標-1-2-個月)
3. [中期目標 (3-4 個月)](#中期目標-3-4-個月)
4. [長期目標 (5-6 個月)](#長期目標-5-6-個月)
5. [詳細實施計畫](#詳細實施計畫)
6. [風險評估與應對](#風險評估與應對)
7. [成功指標](#成功指標)

## 執行摘要

NDH (Neutral Data Hub) 系統已經建立了堅實的基礎架構,包括 CORBA 啟發的分散式物件架構、Kafka 事件流整合,以及 AVEVA PI System 和 Historian 的整合方案。為了將系統推向生產就緒狀態,我們建議按照以下路線圖進行開發。

### 關鍵里程碑

| 里程碑 | 時間 | 主要交付物 |
|--------|------|-----------|
| **M1: 核心重構** | 第 1 個月 | 統一連接器基礎類別、錯誤處理框架 |
| **M2: 功能增強** | 第 2 個月 | OPC UA 訂閱、資料緩衝、監控系統 |
| **M3: 生產準備** | 第 3 個月 | 部署指南、安全加固、效能優化 |
| **M4: 擴展整合** | 第 4 個月 | 告警管理、插件機制、更多系統整合 |
| **M5: 企業級功能** | 第 5-6 個月 | 高可用性、災難恢復、進階分析 |

### 資源需求總覽

- **開發人員**: 2-3 人 (全職)
- **測試人員**: 1 人 (全職)
- **DevOps 工程師**: 1 人 (50% 時間)
- **技術文件撰寫**: 1 人 (25% 時間)
- **專案經理**: 1 人 (25% 時間)

## 短期目標 (1-2 個月)

### 第 1 個月: 核心重構與穩定性

#### 週 1-2: 程式碼重構

**任務 1.1: 建立統一的連接器基礎類別**

```python
# 目標: 減少 60% 的重複程式碼

class IndustrialSystemConnector(Connector):
    """工業系統連接器基礎類別"""
    
    def __init__(self, connector_id: str, system_type: str):
        super().__init__(connector_id, system_type)
        self.mapping_manager = TagMappingManager()
        self.asset_servants: Dict[str, AssetServant] = {}
        self.connection_pool = ConnectionPool()
    
    # 抽象方法 - 子類別必須實作
    @abstractmethod
    async def _read_current_value_impl(self, tag_id: str) -> Dict:
        """讀取當前值的實際實作"""
        pass
    
    @abstractmethod
    async def _read_historical_values_impl(
        self, tag_id: str, start: datetime, end: datetime
    ) -> List[Dict]:
        """讀取歷史資料的實際實作"""
        pass
    
    @abstractmethod
    async def _write_value_impl(self, tag_id: str, value: Any) -> bool:
        """寫入值的實際實作"""
        pass
    
    # 共同邏輯 - 所有子類別共用
    def register_asset(
        self,
        servant: AssetServant,
        mappings: Dict[str, TagMapping]
    ):
        """註冊資產和映射 - 共同實作"""
        asset_id = servant.asset_id
        self.asset_servants[asset_id] = servant
        
        for attr_name, mapping in mappings.items():
            self.mapping_manager.add_mapping(asset_id, attr_name, mapping)
        
        logger.info(f"Registered asset {asset_id} with {len(mappings)} mappings")
    
    async def start_sync(self, asset_id: str):
        """開始同步資產資料 - 共同實作"""
        servant = self.asset_servants.get(asset_id)
        if not servant:
            raise ValueError(f"Asset {asset_id} not registered")
        
        mappings = self.mapping_manager.get_all_mappings(asset_id)
        
        for attr_name, mapping in mappings.items():
            await self._subscribe_to_tag(asset_id, attr_name, mapping)
    
    async def read_current_values(self, asset_id: str) -> Dict[str, Any]:
        """批次讀取當前值 - 共同實作"""
        mappings = self.mapping_manager.get_all_mappings(asset_id)
        
        # 建立批次請求
        tag_ids = [mapping.tag_id for mapping in mappings.values()]
        
        # 批次讀取 (使用連接池)
        async with self.connection_pool.get_connection() as conn:
            results = await self._batch_read_impl(conn, tag_ids)
        
        # 轉換為 NDH 格式
        ndh_values = {}
        for attr_name, mapping in mappings.items():
            if mapping.tag_id in results:
                raw_value = results[mapping.tag_id]["value"]
                ndh_values[attr_name] = mapping.convert_from_source(raw_value)
        
        return ndh_values
    
    async def _subscribe_to_tag(
        self,
        asset_id: str,
        attr_name: str,
        mapping: TagMapping
    ):
        """訂閱 Tag - 共同邏輯 + 子類別實作"""
        async def callback(tag_id: str, value_dict: Dict):
            await self._handle_tag_update(asset_id, attr_name, mapping, value_dict)
        
        await self._subscribe_impl(mapping.tag_id, callback)
    
    async def _handle_tag_update(
        self,
        asset_id: str,
        attr_name: str,
        mapping: TagMapping,
        value_dict: Dict
    ):
        """處理 Tag 更新 - 共同實作"""
        servant = self.asset_servants.get(asset_id)
        if not servant:
            return
        
        # 資料品質檢查
        if not self._validate_quality(value_dict):
            logger.warning(
                f"Poor quality data for {asset_id}.{attr_name}: "
                f"{value_dict.get('quality')}"
            )
            return
        
        # 轉換值
        raw_value = value_dict["value"]
        ndh_value = mapping.convert_from_source(raw_value)
        
        # 更新 Servant
        await servant.set_attribute(attr_name, ndh_value)
        
        # 發布事件
        await servant.publish_event(
            "attribute_changed",
            {
                "attribute": attr_name,
                "value": ndh_value,
                "source_value": raw_value,
                "timestamp": value_dict["timestamp"],
                "quality": value_dict.get("quality")
            }
        )
    
    def _validate_quality(self, value_dict: Dict) -> bool:
        """驗證資料品質 - 共同實作"""
        quality = value_dict.get("quality", 0)
        return quality >= 192  # OPC Good quality
```

**交付物**:
- `ndh/connectors/base_industrial.py`: 基礎類別
- `ndh/connectors/pi_system.py`: 重構後的 PI System 連接器
- `ndh/connectors/historian.py`: 重構後的 Historian 連接器
- 單元測試覆蓋率 > 80%

**週 3: 錯誤處理框架**

**任務 1.2: 實作完整的錯誤處理機制**

```python
# ndh/core/exceptions.py

class NDHException(Exception):
    """NDH 基礎異常類別"""
    def __init__(self, message: str, error_code: str, details: Dict = None):
        super().__init__(message)
        self.error_code = error_code
        self.details = details or {}
        self.timestamp = datetime.now()

class ConnectionError(NDHException):
    """連接錯誤"""
    pass

class AuthenticationError(NDHException):
    """認證錯誤"""
    pass

class DataQualityError(NDHException):
    """資料品質錯誤"""
    pass

class MappingError(NDHException):
    """映射錯誤"""
    pass

# ndh/core/error_handler.py

class ErrorHandler:
    """錯誤處理器"""
    
    def __init__(self):
        self.error_callbacks: Dict[Type[Exception], List[Callable]] = {}
        self.error_log: List[Dict] = []
    
    def register_handler(
        self,
        exception_type: Type[Exception],
        callback: Callable
    ):
        """註冊錯誤處理器"""
        if exception_type not in self.error_callbacks:
            self.error_callbacks[exception_type] = []
        self.error_callbacks[exception_type].append(callback)
    
    async def handle_error(
        self,
        exception: Exception,
        context: Dict = None
    ):
        """處理錯誤"""
        # 記錄錯誤
        error_record = {
            "timestamp": datetime.now(),
            "type": type(exception).__name__,
            "message": str(exception),
            "context": context or {}
        }
        self.error_log.append(error_record)
        
        # 呼叫註冊的處理器
        for exc_type, callbacks in self.error_callbacks.items():
            if isinstance(exception, exc_type):
                for callback in callbacks:
                    try:
                        await callback(exception, context)
                    except Exception as e:
                        logger.error(f"Error in error handler: {e}")
    
    def get_error_statistics(self) -> Dict:
        """取得錯誤統計"""
        stats = {}
        for record in self.error_log:
            error_type = record["type"]
            stats[error_type] = stats.get(error_type, 0) + 1
        return stats

# 使用範例
error_handler = ErrorHandler()

# 註冊處理器
async def handle_connection_error(exc: ConnectionError, context: Dict):
    logger.error(f"Connection error: {exc.message}")
    # 嘗試重新連接
    await reconnect(context.get("connector_id"))

error_handler.register_handler(ConnectionError, handle_connection_error)

# 在程式碼中使用
try:
    await connector.connect()
except ConnectionError as e:
    await error_handler.handle_error(e, {"connector_id": connector.id})
```

**交付物**:
- `ndh/core/exceptions.py`: 異常類別定義
- `ndh/core/error_handler.py`: 錯誤處理器
- 錯誤處理最佳實踐文件
- 所有連接器加入錯誤處理

**週 4: 文件整理**

**任務 1.3: 建立文件索引和快速開始指南**

```markdown
# NDH 文件索引 (README.md)

## 快速開始
- [5 分鐘快速開始](docs/QUICK_START.md)
- [安裝指南](docs/INSTALLATION.md)
- [基本概念](docs/CONCEPTS.md)

## 核心架構
- [CORBA 啟發的架構](docs/CORBA_INSPIRED_ARCHITECTURE.md)
- [物件唯一性](docs/OBJECT_UNIQUENESS.md)
- [生命週期管理](docs/LIFECYCLE.md)

## 整合方案
- [Kafka 整合](docs/KAFKA_INTEGRATION.md)
- [AVEVA PI System 整合](docs/PI_SYSTEM_INTEGRATION.md)
- [AVEVA Historian 整合](docs/HISTORIAN_INTEGRATION.md)
- [OPC UA 整合](docs/OPC_UA_INTEGRATION.md)

## 開發指南
- [連接器開發指南](docs/CONNECTOR_DEVELOPMENT.md)
- [插件開發指南](docs/PLUGIN_DEVELOPMENT.md)
- [API 參考](docs/API_REFERENCE.md)

## 部署和運維
- [部署指南](docs/DEPLOYMENT_GUIDE.md)
- [Docker 部署](docs/DOCKER_DEPLOYMENT.md)
- [Kubernetes 部署](docs/K8S_DEPLOYMENT.md)
- [監控和日誌](docs/MONITORING.md)
- [故障排除](docs/TROUBLESHOOTING.md)

## 最佳實踐
- [效能優化](docs/PERFORMANCE.md)
- [安全性](docs/SECURITY.md)
- [測試策略](docs/TESTING.md)
```

**交付物**:
- `README.md`: 文件入口
- `docs/QUICK_START.md`: 快速開始指南
- `docs/INSTALLATION.md`: 安裝指南
- `docs/CONCEPTS.md`: 基本概念

#### 第 1 個月總結

**完成項目**:
- ✓ 統一連接器基礎類別
- ✓ 完整的錯誤處理框架
- ✓ 文件索引和快速開始指南
- ✓ 程式碼重構,減少 60% 重複

**指標**:
- 程式碼覆蓋率: > 80%
- 文件完整度: > 70%
- 技術債務減少: 60%

### 第 2 個月: 功能增強與監控

#### 週 5-6: OPC UA 直接訂閱

**任務 2.1: 實作 OPC UA 訂閱機制**

```python
# ndh/connectors/opcua_subscription.py

from asyncua import Client, Node
from asyncua.common.subscription import SubHandler

class OPCUASubscriptionHandler(SubHandler):
    """OPC UA 訂閱處理器"""
    
    def __init__(self, callback: Callable):
        self.callback = callback
    
    def datachange_notification(self, node: Node, val, data):
        """資料變化通知"""
        asyncio.create_task(
            self.callback(
                node_id=str(node.nodeid),
                value=val,
                timestamp=data.monitored_item.Value.SourceTimestamp,
                quality=data.monitored_item.Value.StatusCode.value
            )
        )

class OPCUASubscriptionManager:
    """OPC UA 訂閱管理器"""
    
    def __init__(self):
        self.clients: Dict[str, Client] = {}
        self.subscriptions: Dict[str, Any] = {}
        self.handlers: Dict[str, OPCUASubscriptionHandler] = {}
    
    async def connect(self, server_url: str) -> bool:
        """連接到 OPC UA 伺服器"""
        if server_url in self.clients:
            return True
        
        try:
            client = Client(server_url)
            await client.connect()
            self.clients[server_url] = client
            logger.info(f"Connected to OPC UA server: {server_url}")
            return True
        except Exception as e:
            logger.error(f"Failed to connect to {server_url}: {e}")
            return False
    
    async def subscribe_to_node(
        self,
        server_url: str,
        node_id: str,
        callback: Callable,
        sampling_interval: float = 1000.0
    ):
        """訂閱 OPC UA 節點"""
        if server_url not in self.clients:
            await self.connect(server_url)
        
        client = self.clients[server_url]
        
        # 建立訂閱
        subscription_key = f"{server_url}:{node_id}"
        if subscription_key not in self.subscriptions:
            handler = OPCUASubscriptionHandler(callback)
            subscription = await client.create_subscription(
                sampling_interval,
                handler
            )
            
            # 監控節點
            node = client.get_node(node_id)
            await subscription.subscribe_data_change(node)
            
            self.subscriptions[subscription_key] = subscription
            self.handlers[subscription_key] = handler
            
            logger.info(f"Subscribed to {subscription_key}")
    
    async def unsubscribe(self, server_url: str, node_id: str):
        """取消訂閱"""
        subscription_key = f"{server_url}:{node_id}"
        if subscription_key in self.subscriptions:
            subscription = self.subscriptions[subscription_key]
            await subscription.delete()
            del self.subscriptions[subscription_key]
            del self.handlers[subscription_key]
    
    async def disconnect_all(self):
        """斷開所有連接"""
        for client in self.clients.values():
            await client.disconnect()
        self.clients.clear()
        self.subscriptions.clear()
        self.handlers.clear()

# 整合到 Historian 連接器
class HistorianConnector(IndustrialSystemConnector):
    
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.opcua_manager = OPCUASubscriptionManager()
    
    async def start_sync(self, asset_id: str):
        """使用 OPC UA 訂閱而非輪詢"""
        mappings = self.mapping_manager.get_all_mappings(asset_id)
        
        for attr_name, mapping in mappings.items():
            if mapping.opc_server and mapping.opc_node_id:
                # 使用 OPC UA 訂閱
                await self.opcua_manager.subscribe_to_node(
                    mapping.opc_server,
                    mapping.opc_node_id,
                    lambda node_id, value, timestamp, quality:
                        self._handle_opcua_update(
                            asset_id, attr_name, mapping,
                            value, timestamp, quality
                        ),
                    sampling_interval=mapping.scan_rate
                )
            else:
                # 回退到輪詢
                await super().start_sync(asset_id)
```

**交付物**:
- `ndh/connectors/opcua_subscription.py`: OPC UA 訂閱管理器
- 整合到 Historian 和 PI System 連接器
- OPC UA 整合文件
- 效能測試報告 (對比輪詢方式)

#### 週 7: 資料緩衝和重試機制

**任務 2.2: 實作資料緩衝器**

```python
# ndh/core/data_buffer.py

class DataBuffer:
    """資料緩衝器 - 確保資料不遺失"""
    
    def __init__(
        self,
        max_size: int = 10000,
        flush_interval: float = 5.0,
        persistence_path: str = "/var/ndh/buffer"
    ):
        self.buffer: List[Dict] = []
        self.max_size = max_size
        self.flush_interval = flush_interval
        self.persistence_path = persistence_path
        self.flush_task: Optional[asyncio.Task] = None
        
        # 從磁碟恢復緩衝
        self._load_from_disk()
    
    async def buffer_write(
        self,
        connector_id: str,
        tag_id: str,
        value: Any,
        timestamp: datetime = None
    ):
        """緩衝寫入請求"""
        item = {
            "connector_id": connector_id,
            "tag_id": tag_id,
            "value": value,
            "timestamp": timestamp or datetime.now(),
            "retry_count": 0,
            "max_retries": 3
        }
        
        self.buffer.append(item)
        
        # 如果緩衝區滿了,立即刷新
        if len(self.buffer) >= self.max_size:
            await self.flush_buffer()
    
    async def start_auto_flush(self):
        """啟動自動刷新"""
        async def flush_loop():
            while True:
                await asyncio.sleep(self.flush_interval)
                await self.flush_buffer()
        
        self.flush_task = asyncio.create_task(flush_loop())
    
    async def flush_buffer(self):
        """刷新緩衝區"""
        if not self.buffer:
            return
        
        items_to_write = self.buffer.copy()
        self.buffer.clear()
        
        failed_items = []
        
        for item in items_to_write:
            try:
                connector = get_connector(item["connector_id"])
                success = await connector.write_value(
                    item["tag_id"],
                    item["value"],
                    item["timestamp"]
                )
                
                if not success:
                    item["retry_count"] += 1
                    if item["retry_count"] < item["max_retries"]:
                        failed_items.append(item)
                    else:
                        logger.error(
                            f"Failed to write {item['tag_id']} "
                            f"after {item['max_retries']} retries"
                        )
            
            except Exception as e:
                logger.error(f"Error writing buffered data: {e}")
                item["retry_count"] += 1
                if item["retry_count"] < item["max_retries"]:
                    failed_items.append(item)
        
        # 將失敗的項目放回緩衝區
        self.buffer.extend(failed_items)
        
        # 持久化到磁碟
        self._save_to_disk()
    
    def _save_to_disk(self):
        """持久化緩衝區到磁碟"""
        if not self.buffer:
            return
        
        os.makedirs(self.persistence_path, exist_ok=True)
        buffer_file = os.path.join(self.persistence_path, "buffer.json")
        
        with open(buffer_file, 'w') as f:
            json.dump(self.buffer, f, default=str)
    
    def _load_from_disk(self):
        """從磁碟恢復緩衝區"""
        buffer_file = os.path.join(self.persistence_path, "buffer.json")
        
        if os.path.exists(buffer_file):
            with open(buffer_file, 'r') as f:
                self.buffer = json.load(f)
            logger.info(f"Loaded {len(self.buffer)} items from disk")
```

**交付物**:
- `ndh/core/data_buffer.py`: 資料緩衝器
- 整合到所有連接器
- 持久化機制測試
- 重試策略文件

#### 週 8: 監控和日誌系統

**任務 2.3: 整合 Prometheus 和結構化日誌**

```python
# ndh/monitoring/metrics.py

from prometheus_client import Counter, Histogram, Gauge, start_http_server

class MetricsCollector:
    """指標收集器"""
    
    def __init__(self):
        # 連接器指標
        self.connection_status = Gauge(
            'ndh_connector_connection_status',
            'Connector connection status (1=connected, 0=disconnected)',
            ['connector_id', 'system_type']
        )
        
        self.read_operations = Counter(
            'ndh_read_operations_total',
            'Total number of read operations',
            ['connector_id', 'operation_type']
        )
        
        self.read_latency = Histogram(
            'ndh_read_latency_seconds',
            'Read operation latency',
            ['connector_id']
        )
        
        self.write_operations = Counter(
            'ndh_write_operations_total',
            'Total number of write operations',
            ['connector_id', 'status']
        )
        
        self.data_quality_issues = Counter(
            'ndh_data_quality_issues_total',
            'Total number of data quality issues',
            ['connector_id', 'asset_id']
        )
        
        # 事件指標
        self.events_published = Counter(
            'ndh_events_published_total',
            'Total number of events published',
            ['event_type']
        )
        
        # 緩衝區指標
        self.buffer_size = Gauge(
            'ndh_buffer_size',
            'Current buffer size',
            ['connector_id']
        )
        
        self.buffer_flushes = Counter(
            'ndh_buffer_flushes_total',
            'Total number of buffer flushes',
            ['connector_id']
        )
    
    def start_server(self, port: int = 9090):
        """啟動 Prometheus 指標伺服器"""
        start_http_server(port)
        logger.info(f"Metrics server started on port {port}")

# ndh/monitoring/logging.py

import structlog

def setup_logging():
    """設定結構化日誌"""
    structlog.configure(
        processors=[
            structlog.stdlib.filter_by_level,
            structlog.stdlib.add_logger_name,
            structlog.stdlib.add_log_level,
            structlog.stdlib.PositionalArgumentsFormatter(),
            structlog.processors.TimeStamper(fmt="iso"),
            structlog.processors.StackInfoRenderer(),
            structlog.processors.format_exc_info,
            structlog.processors.UnicodeDecoder(),
            structlog.processors.JSONRenderer()
        ],
        context_class=dict,
        logger_factory=structlog.stdlib.LoggerFactory(),
        cache_logger_on_first_use=True,
    )

logger = structlog.get_logger()

# 使用範例
logger.info(
    "connector_connected",
    connector_id="HISTORIAN-01",
    system_type="AVEVAHistorian",
    server_url="http://historian.company.com"
)
```

**交付物**:
- `ndh/monitoring/metrics.py`: Prometheus 指標收集
- `ndh/monitoring/logging.py`: 結構化日誌
- Grafana 儀表板配置
- 監控文件

#### 第 2 個月總結

**完成項目**:
- ✓ OPC UA 直接訂閱
- ✓ 資料緩衝和重試機制
- ✓ Prometheus 監控
- ✓ 結構化日誌

**指標**:
- 訂閱延遲: < 100ms (相比輪詢改善 90%)
- 資料遺失率: < 0.01%
- 監控覆蓋率: 100%

## 中期目標 (3-4 個月)

### 第 3 個月: 生產準備

#### 週 9-10: 部署指南和容器化

**任務 3.1: Docker 容器化**

```dockerfile
# Dockerfile

FROM python:3.11-slim

WORKDIR /app

# 安裝系統依賴
RUN apt-get update && apt-get install -y \
    build-essential \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# 複製依賴文件
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# 複製應用程式
COPY ndh/ ./ndh/
COPY config/ ./config/

# 暴露端口
EXPOSE 8000 9090

# 健康檢查
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8000/health || exit 1

# 啟動應用
CMD ["python", "-m", "ndh.main"]
```

```yaml
# docker-compose.yml

version: '3.8'

services:
  ndh-broker:
    build: .
    ports:
      - "8000:8000"
      - "9090:9090"
    environment:
      - NDH_ORB_ID=NDH-ORB-1
      - KAFKA_BOOTSTRAP_SERVERS=kafka:9092
      - REDIS_HOST=redis
      - REDIS_PORT=6379
    depends_on:
      - kafka
      - redis
    volumes:
      - ./config:/app/config
      - ndh-data:/var/ndh
    restart: unless-stopped
  
  kafka:
    image: confluentinc/cp-kafka:7.5.0
    ports:
      - "9092:9092"
    environment:
      KAFKA_BROKER_ID: 1
      KAFKA_ZOOKEEPER_CONNECT: zookeeper:2181
      KAFKA_ADVERTISED_LISTENERS: PLAINTEXT://kafka:9092
    depends_on:
      - zookeeper
  
  zookeeper:
    image: confluentinc/cp-zookeeper:7.5.0
    environment:
      ZOOKEEPER_CLIENT_PORT: 2181
  
  redis:
    image: redis:7-alpine
    ports:
      - "6379:6379"
  
  prometheus:
    image: prom/prometheus:latest
    ports:
      - "9091:9090"
    volumes:
      - ./monitoring/prometheus.yml:/etc/prometheus/prometheus.yml
      - prometheus-data:/prometheus
  
  grafana:
    image: grafana/grafana:latest
    ports:
      - "3000:3000"
    volumes:
      - grafana-data:/var/lib/grafana
      - ./monitoring/grafana/dashboards:/etc/grafana/provisioning/dashboards
    environment:
      - GF_SECURITY_ADMIN_PASSWORD=admin

volumes:
  ndh-data:
  prometheus-data:
  grafana-data:
```

**交付物**:
- Dockerfile 和 docker-compose.yml
- Docker 部署文件
- 健康檢查端點
- 容器化測試

#### 週 11: 安全加固

**任務 3.2: 實作 RBAC 和加密**

```python
# ndh/security/rbac.py

from enum import Enum

class Permission(Enum):
    """權限枚舉"""
    READ_ASSET = "read:asset"
    WRITE_ASSET = "write:asset"
    CREATE_ASSET = "create:asset"
    DELETE_ASSET = "delete:asset"
    READ_CONFIG = "read:config"
    WRITE_CONFIG = "write:config"
    ADMIN = "admin"

class Role:
    """角色"""
    def __init__(self, name: str, permissions: List[Permission]):
        self.name = name
        self.permissions = set(permissions)
    
    def has_permission(self, permission: Permission) -> bool:
        """檢查是否有權限"""
        return permission in self.permissions or Permission.ADMIN in self.permissions

# 預定義角色
ROLES = {
    "admin": Role("admin", [Permission.ADMIN]),
    "operator": Role("operator", [
        Permission.READ_ASSET,
        Permission.WRITE_ASSET
    ]),
    "viewer": Role("viewer", [
        Permission.READ_ASSET
    ]),
    "engineer": Role("engineer", [
        Permission.READ_ASSET,
        Permission.WRITE_ASSET,
        Permission.CREATE_ASSET,
        Permission.READ_CONFIG
    ])
}

class RBACManager:
    """RBAC 管理器"""
    
    def __init__(self):
        self.user_roles: Dict[str, List[str]] = {}
    
    def assign_role(self, user_id: str, role_name: str):
        """分配角色"""
        if user_id not in self.user_roles:
            self.user_roles[user_id] = []
        if role_name not in self.user_roles[user_id]:
            self.user_roles[user_id].append(role_name)
    
    def check_permission(
        self,
        user_id: str,
        permission: Permission
    ) -> bool:
        """檢查權限"""
        user_role_names = self.user_roles.get(user_id, [])
        for role_name in user_role_names:
            role = ROLES.get(role_name)
            if role and role.has_permission(permission):
                return True
        return False
    
    def require_permission(self, permission: Permission):
        """裝飾器: 要求權限"""
        def decorator(func):
            @wraps(func)
            async def wrapper(*args, **kwargs):
                # 從請求中取得使用者 ID
                user_id = kwargs.get("user_id") or get_current_user_id()
                
                if not self.check_permission(user_id, permission):
                    raise PermissionDeniedError(
                        f"User {user_id} does not have permission {permission.value}"
                    )
                
                return await func(*args, **kwargs)
            return wrapper
        return decorator

# 使用範例
rbac = RBACManager()

@rbac.require_permission(Permission.WRITE_ASSET)
async def update_asset_attribute(asset_id: str, attr_name: str, value: Any):
    """更新資產屬性 - 需要寫入權限"""
    # ...
```

**交付物**:
- RBAC 實作
- JWT 認證
- TLS/SSL 配置
- 安全性文件

#### 週 12: 效能優化

**任務 3.3: 效能測試和優化**

```python
# tests/performance/test_throughput.py

import asyncio
import time
from typing import List

class PerformanceTest:
    """效能測試"""
    
    async def test_read_throughput(
        self,
        connector: IndustrialSystemConnector,
        tag_ids: List[str],
        duration: int = 60
    ):
        """測試讀取吞吐量"""
        start_time = time.time()
        read_count = 0
        
        while time.time() - start_time < duration:
            tasks = [
                connector.read_current_value(tag_id)
                for tag_id in tag_ids
            ]
            await asyncio.gather(*tasks)
            read_count += len(tag_ids)
        
        elapsed = time.time() - start_time
        throughput = read_count / elapsed
        
        print(f"Read throughput: {throughput:.2f} reads/second")
        return throughput
    
    async def test_write_latency(
        self,
        connector: IndustrialSystemConnector,
        tag_id: str,
        iterations: int = 1000
    ):
        """測試寫入延遲"""
        latencies = []
        
        for i in range(iterations):
            start = time.time()
            await connector.write_value(tag_id, i)
            latency = time.time() - start
            latencies.append(latency)
        
        avg_latency = sum(latencies) / len(latencies)
        p95_latency = sorted(latencies)[int(len(latencies) * 0.95)]
        p99_latency = sorted(latencies)[int(len(latencies) * 0.99)]
        
        print(f"Average latency: {avg_latency*1000:.2f}ms")
        print(f"P95 latency: {p95_latency*1000:.2f}ms")
        print(f"P99 latency: {p99_latency*1000:.2f}ms")
        
        return {
            "avg": avg_latency,
            "p95": p95_latency,
            "p99": p99_latency
        }
```

**交付物**:
- 效能測試套件
- 效能基準報告
- 優化建議文件
- 連接池優化

### 第 4 個月: 擴展整合

#### 週 13-14: 告警管理

**任務 4.1: 實作告警管理模組**

```python
# ndh/alarms/manager.py

from enum import Enum

class AlarmSeverity(Enum):
    """告警嚴重程度"""
    CRITICAL = "critical"
    HIGH = "high"
    MEDIUM = "medium"
    LOW = "low"
    INFO = "info"

class AlarmState(Enum):
    """告警狀態"""
    ACTIVE = "active"
    ACKNOWLEDGED = "acknowledged"
    CLEARED = "cleared"

@dataclass
class Alarm:
    """告警"""
    alarm_id: str
    asset_id: str
    attribute: str
    severity: AlarmSeverity
    state: AlarmState
    message: str
    timestamp: datetime
    acknowledged_by: Optional[str] = None
    acknowledged_at: Optional[datetime] = None
    cleared_at: Optional[datetime] = None

class AlarmManager:
    """告警管理器"""
    
    def __init__(self):
        self.active_alarms: Dict[str, Alarm] = {}
        self.alarm_history: List[Alarm] = []
        self.alarm_rules: Dict[str, AlarmRule] = {}
        self.notification_handlers: List[Callable] = []
    
    def add_alarm_rule(
        self,
        rule_id: str,
        asset_id: str,
        attribute: str,
        condition: Callable[[Any], bool],
        severity: AlarmSeverity,
        message_template: str
    ):
        """新增告警規則"""
        rule = AlarmRule(
            rule_id=rule_id,
            asset_id=asset_id,
            attribute=attribute,
            condition=condition,
            severity=severity,
            message_template=message_template
        )
        self.alarm_rules[rule_id] = rule
    
    async def check_value(
        self,
        asset_id: str,
        attribute: str,
        value: Any
    ):
        """檢查值是否觸發告警"""
        for rule in self.alarm_rules.values():
            if rule.asset_id == asset_id and rule.attribute == attribute:
                if rule.condition(value):
                    await self._trigger_alarm(rule, value)
    
    async def _trigger_alarm(self, rule: AlarmRule, value: Any):
        """觸發告警"""
        alarm_id = f"{rule.asset_id}:{rule.attribute}:{rule.rule_id}"
        
        if alarm_id not in self.active_alarms:
            alarm = Alarm(
                alarm_id=alarm_id,
                asset_id=rule.asset_id,
                attribute=rule.attribute,
                severity=rule.severity,
                state=AlarmState.ACTIVE,
                message=rule.message_template.format(value=value),
                timestamp=datetime.now()
            )
            
            self.active_alarms[alarm_id] = alarm
            self.alarm_history.append(alarm)
            
            # 發送通知
            await self._send_notifications(alarm)
    
    async def acknowledge_alarm(self, alarm_id: str, user_id: str):
        """確認告警"""
        if alarm_id in self.active_alarms:
            alarm = self.active_alarms[alarm_id]
            alarm.state = AlarmState.ACKNOWLEDGED
            alarm.acknowledged_by = user_id
            alarm.acknowledged_at = datetime.now()
    
    async def clear_alarm(self, alarm_id: str):
        """清除告警"""
        if alarm_id in self.active_alarms:
            alarm = self.active_alarms[alarm_id]
            alarm.state = AlarmState.CLEARED
            alarm.cleared_at = datetime.now()
            del self.active_alarms[alarm_id]
    
    def get_active_alarms(
        self,
        severity: Optional[AlarmSeverity] = None
    ) -> List[Alarm]:
        """取得活動告警"""
        alarms = list(self.active_alarms.values())
        if severity:
            alarms = [a for a in alarms if a.severity == severity]
        return alarms
    
    async def _send_notifications(self, alarm: Alarm):
        """發送通知"""
        for handler in self.notification_handlers:
            try:
                await handler(alarm)
            except Exception as e:
                logger.error(f"Error sending alarm notification: {e}")

# 使用範例
alarm_manager = AlarmManager()

# 新增告警規則
alarm_manager.add_alarm_rule(
    rule_id="PUMP_HIGH_TEMP",
    asset_id="PUMP_001",
    attribute="temperature",
    condition=lambda temp: temp > 80,
    severity=AlarmSeverity.HIGH,
    message_template="Pump temperature too high: {value}°C"
)

# 註冊通知處理器
async def send_email_notification(alarm: Alarm):
    # 發送郵件
    pass

alarm_manager.notification_handlers.append(send_email_notification)
```

**交付物**:
- 告警管理模組
- 告警規則引擎
- 通知系統
- 告警儀表板

#### 週 15-16: 連接器插件機制

**任務 4.2: 實作插件架構**

```python
# ndh/plugins/interface.py

class ConnectorPlugin(ABC):
    """連接器插件介面"""
    
    @property
    @abstractmethod
    def name(self) -> str:
        """插件名稱"""
        pass
    
    @property
    @abstractmethod
    def version(self) -> str:
        """插件版本"""
        pass
    
    @abstractmethod
    def create_connector(self, config: Dict) -> Connector:
        """建立連接器實例"""
        pass
    
    @abstractmethod
    def validate_config(self, config: Dict) -> bool:
        """驗證配置"""
        pass

# ndh/plugins/registry.py

class PluginRegistry:
    """插件註冊表"""
    
    def __init__(self):
        self.plugins: Dict[str, ConnectorPlugin] = {}
    
    def register(self, plugin: ConnectorPlugin):
        """註冊插件"""
        self.plugins[plugin.name] = plugin
        logger.info(f"Registered plugin: {plugin.name} v{plugin.version}")
    
    def get_plugin(self, name: str) -> Optional[ConnectorPlugin]:
        """取得插件"""
        return self.plugins.get(name)
    
    def list_plugins(self) -> List[str]:
        """列出所有插件"""
        return list(self.plugins.keys())
    
    def create_connector(
        self,
        plugin_name: str,
        config: Dict
    ) -> Connector:
        """建立連接器"""
        plugin = self.get_plugin(plugin_name)
        if not plugin:
            raise ValueError(f"Plugin {plugin_name} not found")
        
        if not plugin.validate_config(config):
            raise ValueError(f"Invalid config for plugin {plugin_name}")
        
        return plugin.create_connector(config)

# 插件範例
class HistorianPlugin(ConnectorPlugin):
    """Historian 插件"""
    
    @property
    def name(self) -> str:
        return "AVEVAHistorian"
    
    @property
    def version(self) -> str:
        return "1.0.0"
    
    def create_connector(self, config: Dict) -> Connector:
        return HistorianConnector(
            connector_id=config["connector_id"],
            historian_rest_url=config["rest_url"],
            username=config["username"],
            password=config["password"]
        )
    
    def validate_config(self, config: Dict) -> bool:
        required_keys = ["connector_id", "rest_url", "username", "password"]
        return all(key in config for key in required_keys)

# 使用
registry = PluginRegistry()
registry.register(HistorianPlugin())
registry.register(PISystemPlugin())

# 從配置建立連接器
connector = registry.create_connector("AVEVAHistorian", {
    "connector_id": "HIST-01",
    "rest_url": "http://historian.company.com",
    "username": "user",
    "password": "pass"
})
```

**交付物**:
- 插件介面定義
- 插件註冊表
- 插件開發指南
- 範例插件

## 長期目標 (5-6 個月)

### 第 5-6 個月: 企業級功能

#### 高可用性

- ORB 叢集
- 自動故障轉移
- 負載平衡

#### 災難恢復

- 資料備份策略
- 恢復程序
- 災難恢復演練

#### 進階分析

- 資料分析引擎
- 機器學習整合
- 預測性維護

## 風險評估與應對

### 高風險項目

| 風險 | 影響 | 機率 | 應對策略 |
|------|------|------|---------|
| OPC UA 整合複雜度 | 高 | 中 | 預留額外時間,尋求專家協助 |
| 效能不達標 | 高 | 低 | 早期進行效能測試,及時優化 |
| 安全性漏洞 | 高 | 中 | 進行安全審計,使用成熟的安全庫 |
| 第三方系統相容性 | 中 | 高 | 充分測試,建立相容性矩陣 |

## 成功指標

### 技術指標

- 程式碼覆蓋率 > 80%
- API 回應時間 < 100ms (P95)
- 系統可用性 > 99.9%
- 資料遺失率 < 0.01%

### 業務指標

- 支援 5+ 種工業系統
- 處理 10,000+ 資產
- 100,000+ Tags 同時訂閱
- 1,000,000+ 事件/天

---

**作者**: 林志錚 (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**最後更新**: 2025-01-10

