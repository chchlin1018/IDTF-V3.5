# NDH 與 AVEVA Historian 整合

## 目錄

1. [整合架構概述](#整合架構概述)
2. [Historian vs PI System 比較](#historian-vs-pi-system-比較)
3. [物件與 Tag 的映射關係](#物件與-tag-的映射關係)
4. [Historian 連接器實作](#historian-連接器實作)
5. [即時資料訂閱](#即時資料訂閱)
6. [歷史資料查詢](#歷史資料查詢)
7. [資料寫入 Historian](#資料寫入-historian)
8. [完整使用範例](#完整使用範例)
9. [效能優化](#效能優化)

## 整合架構概述

### 整體架構圖

```
┌─────────────────────────────────────────────────────────────┐
│                    IADL Editor                               │
│  定義資產模型和 Historian Tag 映射                             │
└────────────────────────┬────────────────────────────────────┘
                         │ IADL 定義
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    NDH Broker (ORB)                          │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │            Asset Servants (物件實例)                   │  │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐     │  │
│  │  │  PUMP_001  │  │ MOTOR_001  │  │ SENSOR_001 │     │  │
│  │  │            │  │            │  │            │     │  │
│  │  │ flow_rate  │  │   speed    │  │    temp    │     │  │
│  │  │ pressure   │  │   current  │  │  humidity  │     │  │
│  │  └─────┬──────┘  └─────┬──────┘  └─────┬──────┘     │  │
│  └────────┼───────────────┼───────────────┼────────────┘  │
│           │               │               │                │
│  ┌────────▼───────────────▼───────────────▼────────────┐  │
│  │         AVEVA Historian Connector                    │  │
│  │  - Tag 映射管理                                        │  │
│  │  - 即時資料訂閱 (OPC UA/DA)                           │  │
│  │  - 歷史資料查詢 (REST API/AASQL)                      │  │
│  │  - 資料寫入                                            │  │
│  └──────────────────────┬───────────────────────────────┘  │
└─────────────────────────┼───────────────────────────────────┘
                          │ REST API / OPC UA / AASQL
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                  AVEVA Historian                             │
│                                                              │
│  ┌─────────────────┐      ┌─────────────────┐             │
│  │  Historian Server│      │   System Platform│            │
│  │                 │      │   Integration    │            │
│  │  Tags:          │      │                 │            │
│  │  - PUMP001.PV   │      │  Templates:     │            │
│  │  - PUMP001.SP   │      │  - PumpTemplate │            │
│  │  - MTR001.SPEED │      │  - MotorTemplate│            │
│  │  - SENS001.TEMP │      │                 │            │
│  └─────────────────┘      └─────────────────┘             │
└─────────────────────────────────────────────────────────────┘
```

## Historian vs PI System 比較

### 核心差異

| 特性 | AVEVA PI System | AVEVA Historian |
|------|----------------|-----------------|
| **主要用途** | 企業級歷史資料庫 | 製程級歷史資料庫 |
| **資料模型** | Tag-based | Tag-based + System Platform 整合 |
| **主要 API** | PI Web API | REST API + AASQL |
| **即時資料** | PI Data Archive | Historian Server |
| **資產框架** | PI AF (獨立) | System Platform (整合) |
| **查詢語言** | PI AF SDK | AASQL (SQL-like) |
| **OPC 支援** | OPC DA/UA 介面 | 原生 OPC DA/UA 支援 |
| **典型部署** | 企業級 | 工廠級 |
| **授權模式** | Tag-based | Server-based |

### 相似之處

- 都是時間序列資料庫
- 都支援 OPC DA/UA
- 都提供 REST API
- 都支援資料壓縮和歸檔
- 都有資產模型功能

### 選擇建議

**選擇 PI System**:
- 需要企業級整合
- 多工廠、跨地域部署
- 需要 PI Vision 視覺化
- 已有 PI 基礎設施

**選擇 Historian**:
- 與 System Platform 深度整合
- 工廠級部署
- 需要 SCADA 和歷史資料統一管理
- 預算考量

## 物件與 Tag 的映射關係

### IADL 中定義映射

```iadl
// IADL 定義資產和 Historian Tag 映射
Asset PUMP_001 {
    type: Equipment;
    name: "主要冷卻水幫浦";
    location: "FACTORY_A/BUILDING_1";
    
    // 屬性定義
    properties {
        flow_rate: Float {
            unit: "m³/h";
            range: [0, 200];
            // Historian Tag 映射
            historian_tag: "PUMP001.FLOW.PV";
            historian_server: "HISTORIAN-01";
            historian_node: "SCADA_NODE_1";
        };
        
        pressure: Float {
            unit: "bar";
            range: [0, 10];
            historian_tag: "PUMP001.PRESS.PV";
            historian_server: "HISTORIAN-01";
            historian_node: "SCADA_NODE_1";
        };
        
        temperature: Float {
            unit: "°C";
            range: [0, 100];
            historian_tag: "PUMP001.TEMP.PV";
            historian_server: "HISTORIAN-01";
            historian_node: "SCADA_NODE_1";
        };
        
        status: String {
            historian_tag: "PUMP001.STATUS";
            historian_server: "HISTORIAN-01";
            historian_node: "SCADA_NODE_1";
        };
    }
    
    // 控制點映射
    control_points {
        start_command: {
            historian_tag: "PUMP001.START.CMD";
            type: "Discrete";
        };
        
        stop_command: {
            historian_tag: "PUMP001.STOP.CMD";
            type: "Discrete";
        };
        
        speed_setpoint: {
            historian_tag: "PUMP001.SPEED.SP";
            type: "Analog";
        };
    }
    
    // System Platform 整合
    system_platform: {
        galaxy: "FACTORY_A_GALAXY";
        area: "BUILDING_1";
        object_name: "PUMP_001";
        template: "PumpTemplate";
    };
}
```

### 映射配置類別

```python
"""
AVEVA Historian Tag 映射配置
"""

from dataclasses import dataclass
from typing import Optional, Dict, Any
from enum import Enum

class HistorianTagType(Enum):
    """Historian Tag 類型"""
    ANALOG = "Analog"           # 類比量
    DISCRETE = "Discrete"       # 離散量
    STRING = "String"           # 字串
    INT32 = "Int32"            # 32位整數
    FLOAT = "Float"            # 浮點數

class HistorianDataType(Enum):
    """Historian 資料類型"""
    SINGLE = "Single"          # 單精度浮點
    DOUBLE = "Double"          # 雙精度浮點
    INT16 = "Int16"            # 16位整數
    INT32 = "Int32"            # 32位整數
    BOOLEAN = "Boolean"        # 布林值
    STRING = "String"          # 字串

class HistorianStorageMode(Enum):
    """儲存模式"""
    DELTA = "Delta"            # 變化儲存
    CYCLIC = "Cyclic"          # 週期儲存
    SWINGING_DOOR = "SwingingDoor"  # 擺動門壓縮

@dataclass
class HistorianTagMapping:
    """Historian Tag 映射配置"""
    
    # NDH 物件資訊
    asset_id: str                    # 資產 ID
    attribute_name: str              # 屬性名稱
    
    # Historian 資訊
    historian_server: str            # Historian Server 名稱
    historian_node: str              # Historian Node 名稱
    historian_tag: str               # Tag 名稱
    tag_type: HistorianTagType       # Tag 類型
    data_type: HistorianDataType     # 資料類型
    
    # 映射選項
    read_only: bool = False          # 唯讀
    scan_rate: float = 1000.0        # 掃描速率(毫秒)
    deadband: Optional[float] = None # 死區
    storage_mode: HistorianStorageMode = HistorianStorageMode.DELTA
    
    # 單位轉換
    engineering_units: Optional[str] = None
    conversion_factor: float = 1.0
    conversion_offset: float = 0.0
    
    # 品質管理
    use_historian_quality: bool = True
    quality_threshold: int = 192      # Good quality threshold
    
    # OPC 資訊(如果透過 OPC 訂閱)
    opc_server: Optional[str] = None
    opc_item_path: Optional[str] = None
    
    def convert_from_historian(self, value: float) -> float:
        """從 Historian 值轉換到 NDH 值"""
        return value * self.conversion_factor + self.conversion_offset
    
    def convert_to_historian(self, value: float) -> float:
        """從 NDH 值轉換到 Historian 值"""
        return (value - self.conversion_offset) / self.conversion_factor

@dataclass
class SystemPlatformMapping:
    """System Platform 物件映射"""
    
    asset_id: str                    # 資產 ID
    galaxy: str                      # Galaxy 名稱
    area: str                        # Area 名稱
    object_name: str                 # 物件名稱
    template: Optional[str] = None   # Template 名稱
    
    # 屬性映射
    attribute_mappings: Dict[str, str] = None  # NDH 屬性 -> System Platform 屬性

class HistorianTagMappingManager:
    """Historian Tag 映射管理器"""
    
    def __init__(self):
        self.mappings: Dict[str, Dict[str, HistorianTagMapping]] = {}
        self.sp_mappings: Dict[str, SystemPlatformMapping] = {}
    
    def add_tag_mapping(
        self,
        asset_id: str,
        attribute_name: str,
        mapping: HistorianTagMapping
    ):
        """新增 Tag 映射"""
        if asset_id not in self.mappings:
            self.mappings[asset_id] = {}
        self.mappings[asset_id][attribute_name] = mapping
    
    def get_tag_mapping(
        self,
        asset_id: str,
        attribute_name: str
    ) -> Optional[HistorianTagMapping]:
        """取得 Tag 映射"""
        return self.mappings.get(asset_id, {}).get(attribute_name)
    
    def get_all_mappings_for_asset(
        self,
        asset_id: str
    ) -> Dict[str, HistorianTagMapping]:
        """取得資產的所有映射"""
        return self.mappings.get(asset_id, {})
    
    def add_sp_mapping(
        self,
        asset_id: str,
        mapping: SystemPlatformMapping
    ):
        """新增 System Platform 映射"""
        self.sp_mappings[asset_id] = mapping
    
    def get_sp_mapping(
        self,
        asset_id: str
    ) -> Optional[SystemPlatformMapping]:
        """取得 System Platform 映射"""
        return self.sp_mappings.get(asset_id)
    
    def load_from_iadl(self, iadl_definition: dict):
        """從 IADL 定義載入映射"""
        asset_id = iadl_definition.get("asset_id")
        
        # 載入屬性映射
        properties = iadl_definition.get("properties", {})
        for attr_name, attr_def in properties.items():
            if "historian_tag" in attr_def:
                mapping = HistorianTagMapping(
                    asset_id=asset_id,
                    attribute_name=attr_name,
                    historian_server=attr_def.get("historian_server"),
                    historian_node=attr_def.get("historian_node"),
                    historian_tag=attr_def["historian_tag"],
                    tag_type=HistorianTagType(attr_def.get("tag_type", "Analog")),
                    data_type=HistorianDataType(attr_def.get("data_type", "Single")),
                    engineering_units=attr_def.get("unit")
                )
                self.add_tag_mapping(asset_id, attr_name, mapping)
        
        # 載入 System Platform 映射
        if "system_platform" in iadl_definition:
            sp_def = iadl_definition["system_platform"]
            sp_mapping = SystemPlatformMapping(
                asset_id=asset_id,
                galaxy=sp_def.get("galaxy"),
                area=sp_def.get("area"),
                object_name=sp_def.get("object_name"),
                template=sp_def.get("template")
            )
            self.add_sp_mapping(asset_id, sp_mapping)
```

## Historian 連接器實作

### Historian REST API 連接器

```python
"""
AVEVA Historian Connector for NDH
使用 REST API 和 AASQL 進行連接
"""

import aiohttp
import asyncio
from typing import Dict, List, Optional, Any, Callable
from datetime import datetime, timedelta
import json

class HistorianRESTConnector:
    """
    AVEVA Historian REST API 連接器
    支援即時資料讀取、歷史資料查詢和資料寫入
    """
    
    def __init__(
        self,
        base_url: str,
        username: str,
        password: str,
        verify_ssl: bool = True
    ):
        self.base_url = base_url.rstrip('/')
        self.username = username
        self.password = password
        self.verify_ssl = verify_ssl
        
        # Session
        self.session: Optional[aiohttp.ClientSession] = None
        
        # 認證 token
        self.auth_token: Optional[str] = None
        
        # Tag 快取
        self.tag_cache: Dict[str, Dict] = {}
        
        # 訂閱管理
        self.subscriptions: Dict[str, Dict] = {}
        self.subscription_tasks: List[asyncio.Task] = []
    
    async def connect(self) -> bool:
        """建立連接並認證"""
        self.session = aiohttp.ClientSession(
            connector=aiohttp.TCPConnector(ssl=self.verify_ssl)
        )
        
        try:
            # 認證
            auth_url = f"{self.base_url}/api/authentication/login"
            auth_data = {
                "username": self.username,
                "password": self.password
            }
            
            async with self.session.post(auth_url, json=auth_data) as response:
                if response.status == 200:
                    data = await response.json()
                    self.auth_token = data.get("token")
                    print("Connected to AVEVA Historian successfully")
                    return True
                else:
                    print(f"Failed to authenticate: {response.status}")
                    return False
        
        except Exception as e:
            print(f"Connection error: {e}")
            return False
    
    async def disconnect(self):
        """斷開連接"""
        # 停止所有訂閱
        for task in self.subscription_tasks:
            task.cancel()
        
        if self.session:
            await self.session.close()
    
    def _get_headers(self) -> Dict[str, str]:
        """取得請求 headers"""
        return {
            "Authorization": f"Bearer {self.auth_token}",
            "Content-Type": "application/json"
        }
    
    async def get_tag_info(
        self,
        historian_server: str,
        historian_node: str,
        tag_name: str
    ) -> Optional[Dict[str, Any]]:
        """
        取得 Tag 資訊
        使用快取提高效能
        """
        cache_key = f"{historian_server}:{historian_node}:{tag_name}"
        
        if cache_key in self.tag_cache:
            return self.tag_cache[cache_key]
        
        try:
            url = f"{self.base_url}/api/tags"
            params = {
                "server": historian_server,
                "node": historian_node,
                "tagName": tag_name
            }
            
            async with self.session.get(
                url,
                params=params,
                headers=self._get_headers()
            ) as response:
                if response.status == 200:
                    data = await response.json()
                    if data.get("tags"):
                        tag_info = data["tags"][0]
                        self.tag_cache[cache_key] = tag_info
                        return tag_info
        
        except Exception as e:
            print(f"Error getting tag info: {e}")
        
        return None
    
    async def read_current_value(
        self,
        historian_server: str,
        historian_node: str,
        tag_name: str
    ) -> Optional[Dict[str, Any]]:
        """
        讀取當前值
        
        Returns:
            {
                "value": 125.5,
                "timestamp": "2025-01-09T10:30:15Z",
                "quality": 192,
                "quality_string": "Good"
            }
        """
        try:
            url = f"{self.base_url}/api/data/current"
            params = {
                "server": historian_server,
                "node": historian_node,
                "tagName": tag_name
            }
            
            async with self.session.get(
                url,
                params=params,
                headers=self._get_headers()
            ) as response:
                if response.status == 200:
                    data = await response.json()
                    if data.get("values"):
                        value_data = data["values"][0]
                        return {
                            "value": value_data.get("value"),
                            "timestamp": value_data.get("timestamp"),
                            "quality": value_data.get("quality"),
                            "quality_string": self._quality_to_string(
                                value_data.get("quality")
                            )
                        }
        
        except Exception as e:
            print(f"Error reading current value: {e}")
        
        return None
    
    def _quality_to_string(self, quality: int) -> str:
        """將品質代碼轉換為字串"""
        if quality >= 192:
            return "Good"
        elif quality >= 128:
            return "Uncertain"
        else:
            return "Bad"
    
    async def read_multiple_current_values(
        self,
        tags: List[tuple]  # [(server, node, tag_name), ...]
    ) -> Dict[str, Dict[str, Any]]:
        """
        批次讀取多個 Tag 的當前值
        """
        results = {}
        
        try:
            url = f"{self.base_url}/api/data/current/batch"
            
            # 建立批次請求
            tag_list = [
                {
                    "server": server,
                    "node": node,
                    "tagName": tag_name
                }
                for server, node, tag_name in tags
            ]
            
            payload = {"tags": tag_list}
            
            async with self.session.post(
                url,
                json=payload,
                headers=self._get_headers()
            ) as response:
                if response.status == 200:
                    data = await response.json()
                    
                    for item in data.get("values", []):
                        tag_name = item.get("tagName")
                        results[tag_name] = {
                            "value": item.get("value"),
                            "timestamp": item.get("timestamp"),
                            "quality": item.get("quality"),
                            "quality_string": self._quality_to_string(
                                item.get("quality")
                            )
                        }
        
        except Exception as e:
            print(f"Error reading multiple values: {e}")
        
        return results
    
    async def read_historical_values(
        self,
        historian_server: str,
        historian_node: str,
        tag_name: str,
        start_time: datetime,
        end_time: datetime,
        interval: Optional[int] = None,  # 間隔(秒)
        max_count: int = 1000
    ) -> List[Dict[str, Any]]:
        """
        讀取歷史資料
        
        Args:
            historian_server: Historian Server 名稱
            historian_node: Historian Node 名稱
            tag_name: Tag 名稱
            start_time: 開始時間
            end_time: 結束時間
            interval: 取樣間隔(秒) (None 表示取得所有記錄值)
            max_count: 最大筆數
        
        Returns:
            [
                {"value": 125.5, "timestamp": "...", "quality": 192},
                ...
            ]
        """
        try:
            if interval:
                # 插值資料
                url = f"{self.base_url}/api/data/interpolated"
            else:
                # 記錄值
                url = f"{self.base_url}/api/data/raw"
            
            params = {
                "server": historian_server,
                "node": historian_node,
                "tagName": tag_name,
                "startTime": start_time.isoformat(),
                "endTime": end_time.isoformat(),
                "maxCount": max_count
            }
            
            if interval:
                params["interval"] = interval
            
            async with self.session.get(
                url,
                params=params,
                headers=self._get_headers()
            ) as response:
                if response.status == 200:
                    data = await response.json()
                    
                    results = []
                    for item in data.get("values", []):
                        results.append({
                            "value": item.get("value"),
                            "timestamp": item.get("timestamp"),
                            "quality": item.get("quality"),
                            "quality_string": self._quality_to_string(
                                item.get("quality")
                            )
                        })
                    
                    return results
        
        except Exception as e:
            print(f"Error reading historical values: {e}")
        
        return []
    
    async def read_historical_values_aasql(
        self,
        query: str
    ) -> List[Dict[str, Any]]:
        """
        使用 AASQL 查詢歷史資料
        
        Example:
            SELECT TagName, DateTime, Value, wwQualityRule
            FROM History
            WHERE TagName = 'PUMP001.FLOW.PV'
            AND DateTime BETWEEN '2025-01-08' AND '2025-01-09'
        """
        try:
            url = f"{self.base_url}/api/query/aasql"
            payload = {"query": query}
            
            async with self.session.post(
                url,
                json=payload,
                headers=self._get_headers()
            ) as response:
                if response.status == 200:
                    data = await response.json()
                    return data.get("results", [])
        
        except Exception as e:
            print(f"Error executing AASQL query: {e}")
        
        return []
    
    async def write_value(
        self,
        historian_server: str,
        historian_node: str,
        tag_name: str,
        value: Any,
        timestamp: Optional[datetime] = None,
        quality: int = 192  # Good quality
    ) -> bool:
        """
        寫入值到 Historian Tag
        
        Args:
            historian_server: Historian Server 名稱
            historian_node: Historian Node 名稱
            tag_name: Tag 名稱
            value: 要寫入的值
            timestamp: 時間戳 (None 表示使用當前時間)
            quality: 品質代碼 (預設 192 = Good)
        
        Returns:
            成功返回 True
        """
        try:
            url = f"{self.base_url}/api/data/write"
            
            payload = {
                "server": historian_server,
                "node": historian_node,
                "tagName": tag_name,
                "value": value,
                "quality": quality
            }
            
            if timestamp:
                payload["timestamp"] = timestamp.isoformat()
            
            async with self.session.post(
                url,
                json=payload,
                headers=self._get_headers()
            ) as response:
                return response.status in [200, 201, 204]
        
        except Exception as e:
            print(f"Error writing value: {e}")
            return False
    
    async def subscribe_to_tag(
        self,
        historian_server: str,
        historian_node: str,
        tag_name: str,
        callback: Callable,
        interval: float = 1.0
    ):
        """
        訂閱 Tag 的即時資料
        
        Args:
            historian_server: Historian Server 名稱
            historian_node: Historian Node 名稱
            tag_name: Tag 名稱
            callback: 回調函數 async def callback(tag_name, value_dict)
            interval: 輪詢間隔(秒)
        """
        subscription_key = f"{historian_server}:{historian_node}:{tag_name}"
        
        if subscription_key in self.subscriptions:
            print(f"Already subscribed to {subscription_key}")
            return
        
        self.subscriptions[subscription_key] = {
            "historian_server": historian_server,
            "historian_node": historian_node,
            "tag_name": tag_name,
            "callback": callback,
            "interval": interval,
            "last_value": None,
            "last_timestamp": None
        }
        
        # 啟動訂閱任務
        task = asyncio.create_task(
            self._subscription_loop(subscription_key)
        )
        self.subscription_tasks.append(task)
    
    async def _subscription_loop(self, subscription_key: str):
        """訂閱循環"""
        sub = self.subscriptions[subscription_key]
        
        while subscription_key in self.subscriptions:
            try:
                # 讀取當前值
                value_dict = await self.read_current_value(
                    sub["historian_server"],
                    sub["historian_node"],
                    sub["tag_name"]
                )
                
                if value_dict:
                    # 檢查是否有變化
                    if (value_dict["value"] != sub["last_value"] or
                        value_dict["timestamp"] != sub["last_timestamp"]):
                        
                        # 更新快取
                        sub["last_value"] = value_dict["value"]
                        sub["last_timestamp"] = value_dict["timestamp"]
                        
                        # 呼叫回調
                        await sub["callback"](sub["tag_name"], value_dict)
                
                # 等待間隔
                await asyncio.sleep(sub["interval"])
            
            except Exception as e:
                print(f"Error in subscription loop: {e}")
                await asyncio.sleep(sub["interval"])
    
    async def unsubscribe_from_tag(
        self,
        historian_server: str,
        historian_node: str,
        tag_name: str
    ):
        """取消訂閱"""
        subscription_key = f"{historian_server}:{historian_node}:{tag_name}"
        if subscription_key in self.subscriptions:
            del self.subscriptions[subscription_key]
```

### Historian 連接器 (整合到 NDH)

```python
"""
AVEVA Historian Connector 整合到 NDH ORB
"""

from ndh.connectors.base import Connector

class HistorianConnector(Connector):
    """
    AVEVA Historian 連接器
    連接 NDH 物件與 Historian Tags
    """
    
    def __init__(
        self,
        connector_id: str,
        historian_rest_url: str,
        username: str,
        password: str
    ):
        super().__init__(connector_id, "AVEVAHistorian")
        
        self.historian_client = HistorianRESTConnector(
            historian_rest_url,
            username,
            password
        )
        
        self.mapping_manager = HistorianTagMappingManager()
        
        # 資產到 Servant 的映射
        self.asset_servants: Dict[str, AssetServant] = {}
    
    async def connect(self) -> bool:
        """建立連接"""
        success = await self.historian_client.connect()
        if success:
            self.state = ConnectorState.CONNECTED
            print(f"Historian Connector {self.connector_id} connected")
        return success
    
    async def disconnect(self):
        """斷開連接"""
        await self.historian_client.disconnect()
        self.state = ConnectorState.DISCONNECTED
    
    def register_asset(
        self,
        servant: AssetServant,
        mappings: Dict[str, HistorianTagMapping]
    ):
        """
        註冊資產和 Historian Tag 映射
        
        Args:
            servant: Asset Servant 物件
            mappings: 屬性名稱 -> HistorianTagMapping
        """
        asset_id = servant.asset_id
        self.asset_servants[asset_id] = servant
        
        # 註冊映射
        for attr_name, mapping in mappings.items():
            self.mapping_manager.add_tag_mapping(
                asset_id,
                attr_name,
                mapping
            )
    
    async def start_sync(self, asset_id: str):
        """
        開始同步資產的資料
        訂閱所有映射的 Historian Tags
        """
        servant = self.asset_servants.get(asset_id)
        if not servant:
            print(f"Asset {asset_id} not registered")
            return
        
        mappings = self.mapping_manager.get_all_mappings_for_asset(asset_id)
        
        for attr_name, mapping in mappings.items():
            # 訂閱 Historian Tag
            await self.historian_client.subscribe_to_tag(
                mapping.historian_server,
                mapping.historian_node,
                mapping.historian_tag,
                lambda tag, value: self._handle_tag_update(
                    asset_id,
                    attr_name,
                    mapping,
                    value
                ),
                interval=mapping.scan_rate / 1000.0  # 轉換為秒
            )
    
    async def _handle_tag_update(
        self,
        asset_id: str,
        attr_name: str,
        mapping: HistorianTagMapping,
        value_dict: Dict[str, Any]
    ):
        """處理 Historian Tag 更新"""
        servant = self.asset_servants.get(asset_id)
        if not servant:
            return
        
        # 檢查品質
        if value_dict["quality"] < mapping.quality_threshold:
            print(f"Poor quality data for {asset_id}.{attr_name}: {value_dict['quality']}")
            return
        
        # 轉換值
        historian_value = value_dict["value"]
        ndh_value = mapping.convert_from_historian(historian_value)
        
        # 更新 Servant 屬性
        await servant.set_attribute(attr_name, ndh_value)
        
        # 發布事件
        await servant.publish_event(
            "attribute_changed",
            {
                "attribute": attr_name,
                "value": ndh_value,
                "historian_value": historian_value,
                "timestamp": value_dict["timestamp"],
                "quality": value_dict["quality"],
                "quality_string": value_dict["quality_string"]
            }
        )
    
    async def read_current_values(self, asset_id: str) -> Dict[str, Any]:
        """讀取資產的所有當前值"""
        mappings = self.mapping_manager.get_all_mappings_for_asset(asset_id)
        
        # 建立批次請求
        tags = [
            (mapping.historian_server, mapping.historian_node, mapping.historian_tag)
            for mapping in mappings.values()
        ]
        
        # 批次讀取
        results = await self.historian_client.read_multiple_current_values(tags)
        
        # 轉換為 NDH 格式
        ndh_values = {}
        for attr_name, mapping in mappings.items():
            if mapping.historian_tag in results:
                historian_value = results[mapping.historian_tag]["value"]
                ndh_values[attr_name] = mapping.convert_from_historian(historian_value)
        
        return ndh_values
    
    async def write_attribute(
        self,
        asset_id: str,
        attr_name: str,
        value: Any
    ) -> bool:
        """寫入屬性值到 Historian"""
        mapping = self.mapping_manager.get_tag_mapping(asset_id, attr_name)
        if not mapping or mapping.read_only:
            return False
        
        # 轉換值
        historian_value = mapping.convert_to_historian(value)
        
        # 寫入 Historian
        return await self.historian_client.write_value(
            mapping.historian_server,
            mapping.historian_node,
            mapping.historian_tag,
            historian_value
        )
    
    async def get_historical_data(
        self,
        asset_id: str,
        attr_name: str,
        start_time: datetime,
        end_time: datetime,
        interval: Optional[int] = None
    ) -> List[Dict[str, Any]]:
        """取得歷史資料"""
        mapping = self.mapping_manager.get_tag_mapping(asset_id, attr_name)
        if not mapping:
            return []
        
        # 從 Historian 讀取歷史資料
        historian_data = await self.historian_client.read_historical_values(
            mapping.historian_server,
            mapping.historian_node,
            mapping.historian_tag,
            start_time,
            end_time,
            interval
        )
        
        # 轉換為 NDH 格式
        ndh_data = []
        for item in historian_data:
            ndh_data.append({
                "value": mapping.convert_from_historian(item["value"]),
                "timestamp": item["timestamp"],
                "quality": item["quality"],
                "quality_string": item["quality_string"]
            }
        
        return ndh_data
    
    async def query_aasql(self, query: str) -> List[Dict[str, Any]]:
        """執行 AASQL 查詢"""
        return await self.historian_client.read_historical_values_aasql(query)
```

## 完整使用範例

### 端到端整合範例

```python
"""
完整的 NDH + AVEVA Historian 整合範例
"""

async def main():
    """主程式"""
    
    # ===== 1. 初始化 NDH ORB =====
    orb = NDH_ORB_Kafka(
        orb_id="NDH-ORB-1",
        endpoint="http://localhost:8000",
        kafka_bootstrap_servers=["localhost:9092"]
    )
    await orb.init()
    
    # ===== 2. 建立 Historian 連接器 =====
    historian_connector = HistorianConnector(
        connector_id="HISTORIAN-CONNECTOR-1",
        historian_rest_url="http://historian-server.company.com:8080",
        username="historian_user",
        password="historian_password"
    )
    await historian_connector.connect()
    
    # ===== 3. 從 IADL 載入資產定義 =====
    iadl_definition = {
        "asset_id": "PUMP_001",
        "type": "Equipment",
        "location": "FACTORY_A/BUILDING_1",
        "properties": {
            "flow_rate": {
                "type": "Float",
                "unit": "m³/h",
                "historian_tag": "PUMP001.FLOW.PV",
                "historian_server": "HISTORIAN-01",
                "historian_node": "SCADA_NODE_1"
            },
            "pressure": {
                "type": "Float",
                "unit": "bar",
                "historian_tag": "PUMP001.PRESS.PV",
                "historian_server": "HISTORIAN-01",
                "historian_node": "SCADA_NODE_1"
            }
        },
        "system_platform": {
            "galaxy": "FACTORY_A_GALAXY",
            "area": "BUILDING_1",
            "object_name": "PUMP_001",
            "template": "PumpTemplate"
        }
    }
    
    # 載入映射
    historian_connector.mapping_manager.load_from_iadl(iadl_definition)
    
    # ===== 4. 建立 Asset Servant =====
    pump = PumpServant("PUMP_001", "FACTORY_A")
    adapter = orb.get_adapter("RootPOA")
    await adapter.activate_object(pump)
    
    # ===== 5. 註冊到 Historian 連接器 =====
    mappings = historian_connector.mapping_manager.get_all_mappings_for_asset("PUMP_001")
    historian_connector.register_asset(pump, mappings)
    
    # ===== 6. 開始即時同步 =====
    await historian_connector.start_sync("PUMP_001")
    
    # ===== 7. 訂閱事件並發布到 Kafka =====
    async def handle_attribute_changed(event):
        # 發布到 Kafka
        await orb.publish_asset_event(
            "attribute_changed",
            pump,
            event.event_data
        )
    
    await pump.subscribe_event("attribute_changed", handle_attribute_changed)
    
    # ===== 8. 提供 REST API =====
    from fastapi import FastAPI
    app = FastAPI()
    
    @app.get("/api/assets/{asset_id}/current")
    async def get_current_values(asset_id: str):
        """取得當前值"""
        values = await historian_connector.read_current_values(asset_id)
        return {"asset_id": asset_id, "values": values}
    
    @app.get("/api/assets/{asset_id}/history")
    async def get_history(
        asset_id: str,
        attribute: str,
        hours: int = 24,
        interval: Optional[int] = None
    ):
        """取得歷史資料"""
        history = await historian_connector.get_historical_data(
            asset_id,
            attribute,
            datetime.now() - timedelta(hours=hours),
            datetime.now(),
            interval
        )
        return {"asset_id": asset_id, "attribute": attribute, "data": history}
    
    @app.post("/api/assets/{asset_id}/control")
    async def control_asset(
        asset_id: str,
        attribute: str,
        value: float
    ):
        """控制資產"""
        success = await historian_connector.write_attribute(asset_id, attribute, value)
        return {"success": success}
    
    @app.post("/api/query/aasql")
    async def execute_aasql(query: str):
        """執行 AASQL 查詢"""
        results = await historian_connector.query_aasql(query)
        return {"results": results}
    
    # ===== 9. 啟動 API 伺服器 =====
    import uvicorn
    config = uvicorn.Config(app, host="0.0.0.0", port=8000)
    server = uvicorn.Server(config)
    
    # 在背景執行
    asyncio.create_task(server.serve())
    
    # ===== 10. 運行 =====
    print("NDH + AVEVA Historian integration running...")
    print("API available at http://localhost:8000")
    
    try:
        await asyncio.Event().wait()  # 永久運行
    except KeyboardInterrupt:
        print("Shutting down...")
    
    # ===== 11. 清理 =====
    await historian_connector.disconnect()
    await orb.shutdown()

if __name__ == "__main__":
    asyncio.run(main())
```

### AASQL 查詢範例

```python
async def aasql_examples():
    """AASQL 查詢範例"""
    
    # 範例 1: 查詢單一 Tag 的歷史資料
    query1 = """
        SELECT TagName, DateTime, Value, wwQualityRule
        FROM History
        WHERE TagName = 'PUMP001.FLOW.PV'
        AND DateTime BETWEEN '2025-01-08' AND '2025-01-09'
        ORDER BY DateTime
    """
    results1 = await historian_connector.query_aasql(query1)
    
    # 範例 2: 查詢多個 Tags
    query2 = """
        SELECT TagName, DateTime, Value
        FROM History
        WHERE TagName IN ('PUMP001.FLOW.PV', 'PUMP001.PRESS.PV')
        AND DateTime > '2025-01-09 00:00:00'
    """
    results2 = await historian_connector.query_aasql(query2)
    
    # 範例 3: 聚合查詢
    query3 = """
        SELECT 
            TagName,
            AVG(Value) as AvgValue,
            MAX(Value) as MaxValue,
            MIN(Value) as MinValue
        FROM History
        WHERE TagName = 'PUMP001.FLOW.PV'
        AND DateTime BETWEEN '2025-01-08' AND '2025-01-09'
        GROUP BY TagName
    """
    results3 = await historian_connector.query_aasql(query3)
    
    # 範例 4: 時間區間聚合
    query4 = """
        SELECT 
            DATEPART(hour, DateTime) as Hour,
            AVG(Value) as AvgFlow
        FROM History
        WHERE TagName = 'PUMP001.FLOW.PV'
        AND DateTime > '2025-01-08'
        GROUP BY DATEPART(hour, DateTime)
        ORDER BY Hour
    """
    results4 = await historian_connector.query_aasql(query4)
```

## 效能優化

### 批次讀取

```python
# 不好的做法: 逐個讀取
for attr in attributes:
    value = await historian_client.read_current_value(server, node, attr)

# 好的做法: 批次讀取
tags = [(server, node, attr) for attr in attributes]
values = await historian_client.read_multiple_current_values(tags)
```

### Tag 資訊快取

```python
# 快取 Tag 資訊避免重複查詢
self.tag_cache[cache_key] = tag_info
```

### 智慧訂閱

```python
# 根據變化率調整掃描間隔
if change_rate > threshold:
    mapping.scan_rate = 500  # 快速掃描 (500ms)
else:
    mapping.scan_rate = 5000  # 慢速掃描 (5s)
```

### 使用 AASQL 進行複雜查詢

```python
# 使用 AASQL 一次查詢多個 Tags 的聚合資料
query = """
    SELECT TagName, AVG(Value) as AvgValue
    FROM History
    WHERE TagName IN ('PUMP001.FLOW.PV', 'PUMP002.FLOW.PV')
    AND DateTime > '2025-01-08'
    GROUP BY TagName
"""
results = await historian_connector.query_aasql(query)
```

## 總結

這個整合方案提供了:

1. **完整的物件映射**: IADL 定義 -> NDH 物件 -> Historian Tags
2. **即時資料同步**: Historian Tags 的變化自動反映到 NDH 物件
3. **歷史資料查詢**: 透過 NDH API 或 AASQL 查詢歷史資料
4. **雙向控制**: 可以從 NDH 寫入控制命令到 Historian
5. **事件驅動**: 整合 Kafka 實現事件溯源
6. **高效能**: 批次讀取、快取、智慧訂閱
7. **可擴展**: 支援多個 Historian Server 和大量 Tags
8. **System Platform 整合**: 支援與 System Platform 的深度整合

與 PI System 相比,Historian 整合提供了:
- 更好的 System Platform 整合
- AASQL 查詢能力
- 更靈活的部署選項
- 更適合工廠級應用

---

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**最後更新**: 2025-01-09

