# NDH 與 AVEVA PI System 整合

## 目錄

1. [整合架構概述](#整合架構概述)
2. [物件與 Tag 的映射關係](#物件與-tag-的映射關係)
3. [PI System 連接器實作](#pi-system-連接器實作)
4. [即時資料訂閱](#即時資料訂閱)
5. [歷史資料查詢](#歷史資料查詢)
6. [資料寫入 PI System](#資料寫入-pi-system)
7. [完整使用範例](#完整使用範例)
8. [效能優化](#效能優化)

## 整合架構概述

### 整體架構圖

```
┌─────────────────────────────────────────────────────────────┐
│                    IADL Editor                               │
│  定義資產模型和 PI Tag 映射                                    │
└────────────────────────┬────────────────────────────────────┘
                         │ IADL 定義
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    NDH Broker (ORB)                          │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │            Asset Tag Instances (物件實例)                   │  │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐     │  │
│  │  │  PUMP_001  │  │ MOTOR_001  │  │ SENSOR_001 │     │  │
│  │  │            │  │            │  │            │     │  │
│  │  │ flow_rate  │  │   speed    │  │    temp    │     │  │
│  │  │ pressure   │  │   current  │  │  humidity  │     │  │
│  │  └─────┬──────┘  └─────┬──────┘  └─────┬──────┘     │  │
│  └────────┼───────────────┼───────────────┼────────────┘  │
│           │               │               │                │
│  ┌────────▼───────────────▼───────────────▼────────────┐  │
│  │         PI System Connector                          │  │
│  │  - Tag 映射管理                                        │  │
│  │  - 即時資料訂閱                                        │  │
│  │  - 歷史資料查詢                                        │  │
│  │  - 資料寫入                                            │  │
│  └──────────────────────┬───────────────────────────────┘  │
└─────────────────────────┼───────────────────────────────────┘
                          │ PI Web API / AF SDK
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                  AVEVA PI System                             │
│                                                              │
│  ┌─────────────────┐      ┌─────────────────┐             │
│  │  PI Data Archive│      │   PI AF Server  │             │
│  │                 │      │   (Asset Framework)            │
│  │  Tags:          │      │                 │             │
│  │  - PUMP001.PV   │      │  Templates:     │             │
│  │  - PUMP001.SP   │      │  - PumpTemplate │             │
│  │  - MTR001.SPEED │      │  - MotorTemplate│             │
│  │  - SENS001.TEMP │      │                 │             │
│  └─────────────────┘      └─────────────────┘             │
└─────────────────────────────────────────────────────────────┘
```

### 核心概念

#### NDH 物件 (Asset Tag Instance)
```python
class PumpServant:
    asset_id = "PUMP_001"
    attributes = {
        "flow_rate": 0.0,
        "pressure": 0.0,
        "temperature": 0.0,
        "status": "stopped"
    }
```

#### PI System Tag
```
Tag Name: PUMP001.FLOW.PV
Description: Pump 001 Flow Rate Process Value
Engineering Units: m³/h
Point Type: Float32
```

#### 映射關係
```
NDH Object Attribute  <-->  PI Tag
PUMP_001.flow_rate    <-->  PUMP001.FLOW.PV
PUMP_001.pressure     <-->  PUMP001.PRESS.PV
PUMP_001.temperature  <-->  PUMP001.TEMP.PV
```

## 物件與 Tag 的映射關係

### IADL 中定義映射

```iadl
// IADL 定義資產和 PI Tag 映射
Asset PUMP_001 {
    type: Equipment;
    name: "主要冷卻水幫浦";
    location: "FACTORY_A/BUILDING_1";
    
    // 屬性定義
    properties {
        flow_rate: Float {
            unit: "m³/h";
            range: [0, 200];
            // PI Tag 映射
            pi_tag: "PUMP001.FLOW.PV";
            pi_server: "PI-SERVER-01";
        };
        
        pressure: Float {
            unit: "bar";
            range: [0, 10];
            pi_tag: "PUMP001.PRESS.PV";
            pi_server: "PI-SERVER-01";
        };
        
        temperature: Float {
            unit: "°C";
            range: [0, 100];
            pi_tag: "PUMP001.TEMP.PV";
            pi_server: "PI-SERVER-01";
        };
        
        status: String {
            pi_tag: "PUMP001.STATUS";
            pi_server: "PI-SERVER-01";
        };
    }
    
    // 控制點映射
    control_points {
        start_command: {
            pi_tag: "PUMP001.START.CMD";
            type: "Digital";
        };
        
        stop_command: {
            pi_tag: "PUMP001.STOP.CMD";
            type: "Digital";
        };
        
        speed_setpoint: {
            pi_tag: "PUMP001.SPEED.SP";
            type: "Float32";
        };
    }
    
    // PI AF 元素映射
    pi_af_element: "\\\\PI-AF-SERVER\\FACTORY_A\\BUILDING_1\\PUMP_001";
}
```

### 映射配置類別

```python
"""
PI Tag 映射配置
"""

from dataclasses import dataclass
from typing import Optional, Dict, Any
from enum import Enum

class PIPointType(Enum):
    """PI Point 類型"""
    FLOAT32 = "Float32"
    FLOAT64 = "Float64"
    INT16 = "Int16"
    INT32 = "Int32"
    DIGITAL = "Digital"
    STRING = "String"
    TIMESTAMP = "Timestamp"

class PIUpdateMode(Enum):
    """PI 更新模式"""
    SCAN = "scan"           # 定期掃描
    EXCEPTION = "exception" # 例外報告
    UNSOLICITED = "unsolicited"  # 非請求更新

@dataclass
class PITagMapping:
    """PI Tag 映射配置"""
    
    # NDH 物件資訊
    asset_id: str                    # 資產 ID
    attribute_name: str              # 屬性名稱
    
    # PI System 資訊
    pi_server: str                   # PI Server 名稱
    pi_tag: str                      # PI Tag 名稱
    pi_point_type: PIPointType       # Point 類型
    
    # 映射選項
    read_only: bool = False          # 唯讀
    update_mode: PIUpdateMode = PIUpdateMode.SCAN
    scan_interval: float = 1.0       # 掃描間隔(秒)
    deadband: Optional[float] = None # 死區
    
    # 單位轉換
    engineering_units: Optional[str] = None
    conversion_factor: float = 1.0
    conversion_offset: float = 0.0
    
    # 品質管理
    use_pi_quality: bool = True      # 使用 PI 品質
    quality_threshold: int = 0       # 品質閾值
    
    def convert_from_pi(self, value: float) -> float:
        """從 PI 值轉換到 NDH 值"""
        return value * self.conversion_factor + self.conversion_offset
    
    def convert_to_pi(self, value: float) -> float:
        """從 NDH 值轉換到 PI 值"""
        return (value - self.conversion_offset) / self.conversion_factor

@dataclass
class PIAFElementMapping:
    """PI AF 元素映射"""
    
    asset_id: str                    # 資產 ID
    af_server: str                   # AF Server
    af_database: str                 # AF Database
    af_element_path: str             # AF Element 路徑
    af_template: Optional[str] = None  # AF Template
    
    # 屬性映射
    attribute_mappings: Dict[str, str] = None  # NDH 屬性 -> AF 屬性

class PITagMappingManager:
    """PI Tag 映射管理器"""
    
    def __init__(self):
        self.mappings: Dict[str, Dict[str, PITagMapping]] = {}
        self.af_mappings: Dict[str, PIAFElementMapping] = {}
    
    def add_tag_mapping(
        self,
        asset_id: str,
        attribute_name: str,
        mapping: PITagMapping
    ):
        """新增 Tag 映射"""
        if asset_id not in self.mappings:
            self.mappings[asset_id] = {}
        self.mappings[asset_id][attribute_name] = mapping
    
    def get_tag_mapping(
        self,
        asset_id: str,
        attribute_name: str
    ) -> Optional[PITagMapping]:
        """取得 Tag 映射"""
        return self.mappings.get(asset_id, {}).get(attribute_name)
    
    def get_all_mappings_for_asset(
        self,
        asset_id: str
    ) -> Dict[str, PITagMapping]:
        """取得資產的所有映射"""
        return self.mappings.get(asset_id, {})
    
    def add_af_mapping(
        self,
        asset_id: str,
        mapping: PIAFElementMapping
    ):
        """新增 AF 元素映射"""
        self.af_mappings[asset_id] = mapping
    
    def get_af_mapping(
        self,
        asset_id: str
    ) -> Optional[PIAFElementMapping]:
        """取得 AF 元素映射"""
        return self.af_mappings.get(asset_id)
    
    def load_from_iadl(self, iadl_definition: dict):
        """從 IADL 定義載入映射"""
        asset_id = iadl_definition.get("asset_id")
        
        # 載入屬性映射
        properties = iadl_definition.get("properties", {})
        for attr_name, attr_def in properties.items():
            if "pi_tag" in attr_def:
                mapping = PITagMapping(
                    asset_id=asset_id,
                    attribute_name=attr_name,
                    pi_server=attr_def.get("pi_server"),
                    pi_tag=attr_def["pi_tag"],
                    pi_point_type=PIPointType(attr_def.get("point_type", "Float32")),
                    engineering_units=attr_def.get("unit")
                )
                self.add_tag_mapping(asset_id, attr_name, mapping)
        
        # 載入 AF 映射
        if "pi_af_element" in iadl_definition:
            af_path = iadl_definition["pi_af_element"]
            parts = af_path.split("\\\\")
            af_server = parts[1] if len(parts) > 1 else None
            
            af_mapping = PIAFElementMapping(
                asset_id=asset_id,
                af_server=af_server,
                af_database=parts[2] if len(parts) > 2 else None,
                af_element_path=af_path
            )
            self.add_af_mapping(asset_id, af_mapping)
```

## PI System 連接器實作

### PI Web API 連接器

```python
"""
AVEVA PI System Connector for NDH
使用 PI Web API 進行連接
"""

import aiohttp
import asyncio
from typing import Dict, List, Optional, Any, Callable
from datetime import datetime, timedelta
import base64
import json

class PIWebAPIConnector:
    """
    PI Web API 連接器
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
        
        # 建立認證 header
        credentials = f"{username}:{password}"
        encoded = base64.b64encode(credentials.encode()).decode()
        self.headers = {
            "Authorization": f"Basic {encoded}",
            "Content-Type": "application/json"
        }
        
        # Session
        self.session: Optional[aiohttp.ClientSession] = None
        
        # WebID 快取
        self.webid_cache: Dict[str, str] = {}
        
        # 訂閱管理
        self.subscriptions: Dict[str, Dict] = {}
        self.subscription_tasks: List[asyncio.Task] = []
    
    async def connect(self):
        """建立連接"""
        self.session = aiohttp.ClientSession(
            headers=self.headers,
            connector=aiohttp.TCPConnector(ssl=self.verify_ssl)
        )
        
        # 測試連接
        try:
            async with self.session.get(f"{self.base_url}/system") as response:
                if response.status == 200:
                    print("Connected to PI Web API successfully")
                    return True
                else:
                    print(f"Failed to connect: {response.status}")
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
    
    async def get_point_webid(self, pi_server: str, tag_name: str) -> Optional[str]:
        """
        取得 PI Point 的 WebID
        使用快取提高效能
        """
        cache_key = f"{pi_server}:{tag_name}"
        
        if cache_key in self.webid_cache:
            return self.webid_cache[cache_key]
        
        try:
            # 查詢 WebID
            url = f"{self.base_url}/dataservers"
            params = {"name": pi_server}
            
            async with self.session.get(url, params=params) as response:
                if response.status == 200:
                    data = await response.json()
                    server_webid = data["Items"][0]["WebId"]
                    
                    # 取得 Point WebID
                    point_url = f"{self.base_url}/dataservers/{server_webid}/points"
                    point_params = {"nameFilter": tag_name}
                    
                    async with self.session.get(point_url, params=point_params) as point_response:
                        if point_response.status == 200:
                            point_data = await point_response.json()
                            if point_data["Items"]:
                                webid = point_data["Items"][0]["WebId"]
                                self.webid_cache[cache_key] = webid
                                return webid
        
        except Exception as e:
            print(f"Error getting WebID: {e}")
        
        return None
    
    async def read_current_value(
        self,
        pi_server: str,
        tag_name: str
    ) -> Optional[Dict[str, Any]]:
        """
        讀取當前值
        
        Returns:
            {
                "value": 125.5,
                "timestamp": "2025-01-09T10:30:15Z",
                "quality": "Good",
                "units": "m³/h"
            }
        """
        webid = await self.get_point_webid(pi_server, tag_name)
        if not webid:
            return None
        
        try:
            url = f"{self.base_url}/streams/{webid}/value"
            
            async with self.session.get(url) as response:
                if response.status == 200:
                    data = await response.json()
                    return {
                        "value": data.get("Value"),
                        "timestamp": data.get("Timestamp"),
                        "quality": data.get("Good", True),
                        "units": data.get("UnitsAbbreviation")
                    }
        
        except Exception as e:
            print(f"Error reading current value: {e}")
        
        return None
    
    async def read_multiple_current_values(
        self,
        tags: List[tuple]  # [(pi_server, tag_name), ...]
    ) -> Dict[str, Dict[str, Any]]:
        """
        批次讀取多個 Tag 的當前值
        使用 PI Web API 的批次端點提高效能
        """
        results = {}
        
        # 建立批次請求
        batch_request = {}
        for i, (pi_server, tag_name) in enumerate(tags):
            webid = await self.get_point_webid(pi_server, tag_name)
            if webid:
                batch_request[f"req{i}"] = {
                    "Method": "GET",
                    "Resource": f"{self.base_url}/streams/{webid}/value"
                }
        
        if not batch_request:
            return results
        
        try:
            url = f"{self.base_url}/batch"
            
            async with self.session.post(url, json=batch_request) as response:
                if response.status == 207:  # Multi-Status
                    data = await response.json()
                    
                    for i, (pi_server, tag_name) in enumerate(tags):
                        key = f"req{i}"
                        if key in data and data[key]["Status"] == 200:
                            content = data[key]["Content"]
                            results[tag_name] = {
                                "value": content.get("Value"),
                                "timestamp": content.get("Timestamp"),
                                "quality": content.get("Good", True),
                                "units": content.get("UnitsAbbreviation")
                            }
        
        except Exception as e:
            print(f"Error reading multiple values: {e}")
        
        return results
    
    async def read_historical_values(
        self,
        pi_server: str,
        tag_name: str,
        start_time: datetime,
        end_time: datetime,
        interval: Optional[str] = None,  # e.g., "1m", "5m", "1h"
        max_count: int = 1000
    ) -> List[Dict[str, Any]]:
        """
        讀取歷史資料
        
        Args:
            pi_server: PI Server 名稱
            tag_name: Tag 名稱
            start_time: 開始時間
            end_time: 結束時間
            interval: 取樣間隔 (None 表示取得所有記錄值)
            max_count: 最大筆數
        
        Returns:
            [
                {"value": 125.5, "timestamp": "...", "quality": "Good"},
                ...
            ]
        """
        webid = await self.get_point_webid(pi_server, tag_name)
        if not webid:
            return []
        
        try:
            if interval:
                # 插值或取樣資料
                url = f"{self.base_url}/streams/{webid}/interpolated"
            else:
                # 記錄值
                url = f"{self.base_url}/streams/{webid}/recorded"
            
            params = {
                "startTime": start_time.isoformat(),
                "endTime": end_time.isoformat(),
                "maxCount": max_count
            }
            
            if interval:
                params["interval"] = interval
            
            async with self.session.get(url, params=params) as response:
                if response.status == 200:
                    data = await response.json()
                    
                    results = []
                    for item in data.get("Items", []):
                        results.append({
                            "value": item.get("Value"),
                            "timestamp": item.get("Timestamp"),
                            "quality": item.get("Good", True),
                            "units": item.get("UnitsAbbreviation")
                        })
                    
                    return results
        
        except Exception as e:
            print(f"Error reading historical values: {e}")
        
        return []
    
    async def write_value(
        self,
        pi_server: str,
        tag_name: str,
        value: Any,
        timestamp: Optional[datetime] = None
    ) -> bool:
        """
        寫入值到 PI Tag
        
        Args:
            pi_server: PI Server 名稱
            tag_name: Tag 名稱
            value: 要寫入的值
            timestamp: 時間戳 (None 表示使用當前時間)
        
        Returns:
            成功返回 True
        """
        webid = await self.get_point_webid(pi_server, tag_name)
        if not webid:
            return False
        
        try:
            url = f"{self.base_url}/streams/{webid}/value"
            
            payload = {
                "Value": value
            }
            
            if timestamp:
                payload["Timestamp"] = timestamp.isoformat()
            
            async with self.session.post(url, json=payload) as response:
                return response.status in [200, 202, 204]
        
        except Exception as e:
            print(f"Error writing value: {e}")
            return False
    
    async def subscribe_to_tag(
        self,
        pi_server: str,
        tag_name: str,
        callback: Callable,
        interval: float = 1.0
    ):
        """
        訂閱 Tag 的即時資料
        
        Args:
            pi_server: PI Server 名稱
            tag_name: Tag 名稱
            callback: 回調函數 async def callback(tag_name, value_dict)
            interval: 輪詢間隔(秒)
        """
        subscription_key = f"{pi_server}:{tag_name}"
        
        if subscription_key in self.subscriptions:
            print(f"Already subscribed to {subscription_key}")
            return
        
        self.subscriptions[subscription_key] = {
            "pi_server": pi_server,
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
                    sub["pi_server"],
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
        pi_server: str,
        tag_name: str
    ):
        """取消訂閱"""
        subscription_key = f"{pi_server}:{tag_name}"
        if subscription_key in self.subscriptions:
            del self.subscriptions[subscription_key]
```

### PI System 連接器 (整合到 NDH)

```python
"""
PI System Connector 整合到 NDH ORB
"""

from ndh.connectors.base import Connector

class PISystemConnector(Connector):
    """
    PI System 連接器
    連接 NDH 物件與 PI System Tags
    """
    
    def __init__(
        self,
        connector_id: str,
        pi_web_api_url: str,
        username: str,
        password: str
    ):
        super().__init__(connector_id, "PISystem")
        
        self.pi_client = PIWebAPIConnector(
            pi_web_api_url,
            username,
            password
        )
        
        self.mapping_manager = PITagMappingManager()
        
        # 資產到 Servant 的映射
        self.asset_servants: Dict[str, AssetServant] = {}
    
    async def connect(self) -> bool:
        """建立連接"""
        success = await self.pi_client.connect()
        if success:
            self.state = ConnectorState.CONNECTED
            print(f"PI System Connector {self.connector_id} connected")
        return success
    
    async def disconnect(self):
        """斷開連接"""
        await self.pi_client.disconnect()
        self.state = ConnectorState.DISCONNECTED
    
    def register_asset(
        self,
        servant: AssetServant,
        mappings: Dict[str, PITagMapping]
    ):
        """
        註冊資產和 PI Tag 映射
        
        Args:
            servant: Asset Tag Instance 物件
            mappings: 屬性名稱 -> PITagMapping
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
        訂閱所有映射的 PI Tags
        """
        servant = self.asset_servants.get(asset_id)
        if not servant:
            print(f"Asset {asset_id} not registered")
            return
        
        mappings = self.mapping_manager.get_all_mappings_for_asset(asset_id)
        
        for attr_name, mapping in mappings.items():
            # 訂閱 PI Tag
            await self.pi_client.subscribe_to_tag(
                mapping.pi_server,
                mapping.pi_tag,
                lambda tag, value: self._handle_tag_update(
                    asset_id,
                    attr_name,
                    mapping,
                    value
                ),
                interval=mapping.scan_interval
            )
    
    async def _handle_tag_update(
        self,
        asset_id: str,
        attr_name: str,
        mapping: PITagMapping,
        value_dict: Dict[str, Any]
    ):
        """處理 PI Tag 更新"""
        servant = self.asset_servants.get(asset_id)
        if not servant:
            return
        
        # 轉換值
        pi_value = value_dict["value"]
        ndh_value = mapping.convert_from_pi(pi_value)
        
        # 更新 Servant 屬性
        await servant.set_attribute(attr_name, ndh_value)
        
        # 發布事件
        await servant.publish_event(
            "attribute_changed",
            {
                "attribute": attr_name,
                "value": ndh_value,
                "pi_value": pi_value,
                "timestamp": value_dict["timestamp"],
                "quality": value_dict["quality"]
            }
        )
    
    async def read_current_values(self, asset_id: str) -> Dict[str, Any]:
        """讀取資產的所有當前值"""
        mappings = self.mapping_manager.get_all_mappings_for_asset(asset_id)
        
        # 建立批次請求
        tags = [
            (mapping.pi_server, mapping.pi_tag)
            for mapping in mappings.values()
        ]
        
        # 批次讀取
        results = await self.pi_client.read_multiple_current_values(tags)
        
        # 轉換為 NDH 格式
        ndh_values = {}
        for attr_name, mapping in mappings.items():
            if mapping.pi_tag in results:
                pi_value = results[mapping.pi_tag]["value"]
                ndh_values[attr_name] = mapping.convert_from_pi(pi_value)
        
        return ndh_values
    
    async def write_attribute(
        self,
        asset_id: str,
        attr_name: str,
        value: Any
    ) -> bool:
        """寫入屬性值到 PI System"""
        mapping = self.mapping_manager.get_tag_mapping(asset_id, attr_name)
        if not mapping or mapping.read_only:
            return False
        
        # 轉換值
        pi_value = mapping.convert_to_pi(value)
        
        # 寫入 PI
        return await self.pi_client.write_value(
            mapping.pi_server,
            mapping.pi_tag,
            pi_value
        )
    
    async def get_historical_data(
        self,
        asset_id: str,
        attr_name: str,
        start_time: datetime,
        end_time: datetime,
        interval: Optional[str] = None
    ) -> List[Dict[str, Any]]:
        """取得歷史資料"""
        mapping = self.mapping_manager.get_tag_mapping(asset_id, attr_name)
        if not mapping:
            return []
        
        # 從 PI 讀取歷史資料
        pi_data = await self.pi_client.read_historical_values(
            mapping.pi_server,
            mapping.pi_tag,
            start_time,
            end_time,
            interval
        )
        
        # 轉換為 NDH 格式
        ndh_data = []
        for item in pi_data:
            ndh_data.append({
                "value": mapping.convert_from_pi(item["value"]),
                "timestamp": item["timestamp"],
                "quality": item["quality"]
            })
        
        return ndh_data
```

## 即時資料訂閱

### 完整訂閱流程

```python
async def setup_pump_with_pi_integration():
    """
    設定幫浦與 PI System 的整合
    """
    
    # 1. 建立 NDH ORB
    orb = NDH_ORB("NDH-ORB-1", "http://localhost:8000")
    await orb.init()
    
    # 2. 建立 PI System 連接器
    pi_connector = PISystemConnector(
        connector_id="PI-CONNECTOR-1",
        pi_web_api_url="https://pi-server.company.com/piwebapi",
        username="pi_user",
        password="pi_password"
    )
    
    await pi_connector.connect()
    
    # 3. 建立 Asset Tag Instance
    pump = PumpServant("PUMP_001", "FACTORY_A")
    adapter = orb.get_adapter("RootPOA")
    await adapter.activate_object(pump)
    
    # 4. 定義 PI Tag 映射
    mappings = {
        "flow_rate": PITagMapping(
            asset_id="PUMP_001",
            attribute_name="flow_rate",
            pi_server="PI-SERVER-01",
            pi_tag="PUMP001.FLOW.PV",
            pi_point_type=PIPointType.FLOAT32,
            engineering_units="m³/h",
            scan_interval=1.0
        ),
        "pressure": PITagMapping(
            asset_id="PUMP_001",
            attribute_name="pressure",
            pi_server="PI-SERVER-01",
            pi_tag="PUMP001.PRESS.PV",
            pi_point_type=PIPointType.FLOAT32,
            engineering_units="bar",
            scan_interval=1.0
        ),
        "temperature": PITagMapping(
            asset_id="PUMP_001",
            attribute_name="temperature",
            pi_server="PI-SERVER-01",
            pi_tag="PUMP001.TEMP.PV",
            pi_point_type=PIPointType.FLOAT32,
            engineering_units="°C",
            scan_interval=5.0
        )
    }
    
    # 5. 註冊資產和映射
    pi_connector.register_asset(pump, mappings)
    
    # 6. 開始同步
    await pi_connector.start_sync("PUMP_001")
    
    # 7. 訂閱資產事件
    async def handle_attribute_changed(event):
        print(f"Attribute changed: {event.event_data}")
    
    await pump.subscribe_event("attribute_changed", handle_attribute_changed)
    
    # 8. 運行
    print("PI System integration running...")
    await asyncio.sleep(3600)  # 運行 1 小時
    
    # 9. 清理
    await pi_connector.disconnect()
    await orb.shutdown()
```

## 歷史資料查詢

### 查詢範例

```python
async def query_pump_history():
    """查詢幫浦歷史資料"""
    
    # 取得過去 24 小時的流量資料
    history = await pi_connector.get_historical_data(
        asset_id="PUMP_001",
        attr_name="flow_rate",
        start_time=datetime.now() - timedelta(hours=24),
        end_time=datetime.now(),
        interval="5m"  # 每 5 分鐘一筆
    )
    
    # 分析資料
    values = [item["value"] for item in history]
    avg_flow = sum(values) / len(values)
    max_flow = max(values)
    min_flow = min(values)
    
    print(f"Average flow: {avg_flow:.2f} m³/h")
    print(f"Max flow: {max_flow:.2f} m³/h")
    print(f"Min flow: {min_flow:.2f} m³/h")
    
    return history
```

## 資料寫入 PI System

### 寫入控制命令

```python
async def control_pump():
    """控制幫浦"""
    
    # 設定速度設定點
    success = await pi_connector.write_attribute(
        asset_id="PUMP_001",
        attr_name="speed_setpoint",
        value=75.0  # 75%
    )
    
    if success:
        print("Speed setpoint written to PI")
    
    # 發送啟動命令
    success = await pi_connector.write_attribute(
        asset_id="PUMP_001",
        attr_name="start_command",
        value=1  # Digital: 1 = Start
    )
    
    if success:
        print("Start command sent to PI")
```

## 完整使用範例

### 端到端整合範例

```python
"""
完整的 NDH + PI System 整合範例
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
    
    # ===== 2. 建立 PI System 連接器 =====
    pi_connector = PISystemConnector(
        connector_id="PI-CONNECTOR-1",
        pi_web_api_url="https://pi-server.company.com/piwebapi",
        username="pi_user",
        password="pi_password"
    )
    await pi_connector.connect()
    
    # ===== 3. 從 IADL 載入資產定義 =====
    iadl_definition = {
        "asset_id": "PUMP_001",
        "type": "Equipment",
        "location": "FACTORY_A/BUILDING_1",
        "properties": {
            "flow_rate": {
                "type": "Float",
                "unit": "m³/h",
                "pi_tag": "PUMP001.FLOW.PV",
                "pi_server": "PI-SERVER-01"
            },
            "pressure": {
                "type": "Float",
                "unit": "bar",
                "pi_tag": "PUMP001.PRESS.PV",
                "pi_server": "PI-SERVER-01"
            }
        }
    }
    
    # 載入映射
    pi_connector.mapping_manager.load_from_iadl(iadl_definition)
    
    # ===== 4. 建立 Asset Tag Instance =====
    pump = PumpServant("PUMP_001", "FACTORY_A")
    adapter = orb.get_adapter("RootPOA")
    await adapter.activate_object(pump)
    
    # ===== 5. 註冊到 PI 連接器 =====
    mappings = pi_connector.mapping_manager.get_all_mappings_for_asset("PUMP_001")
    pi_connector.register_asset(pump, mappings)
    
    # ===== 6. 開始即時同步 =====
    await pi_connector.start_sync("PUMP_001")
    
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
        values = await pi_connector.read_current_values(asset_id)
        return {"asset_id": asset_id, "values": values}
    
    @app.get("/api/assets/{asset_id}/history")
    async def get_history(
        asset_id: str,
        attribute: str,
        hours: int = 24
    ):
        """取得歷史資料"""
        history = await pi_connector.get_historical_data(
            asset_id,
            attribute,
            datetime.now() - timedelta(hours=hours),
            datetime.now(),
            interval="5m"
        )
        return {"asset_id": asset_id, "attribute": attribute, "data": history}
    
    @app.post("/api/assets/{asset_id}/control")
    async def control_asset(
        asset_id: str,
        attribute: str,
        value: float
    ):
        """控制資產"""
        success = await pi_connector.write_attribute(asset_id, attribute, value)
        return {"success": success}
    
    # ===== 9. 啟動 API 伺服器 =====
    import uvicorn
    config = uvicorn.Config(app, host="0.0.0.0", port=8000)
    server = uvicorn.Server(config)
    
    # 在背景執行
    asyncio.create_task(server.serve())
    
    # ===== 10. 運行 =====
    print("NDH + PI System integration running...")
    print("API available at http://localhost:8000")
    
    try:
        await asyncio.Event().wait()  # 永久運行
    except KeyboardInterrupt:
        print("Shutting down...")
    
    # ===== 11. 清理 =====
    await pi_connector.disconnect()
    await orb.shutdown()

if __name__ == "__main__":
    asyncio.run(main())
```

## 效能優化

### 批次讀取

```python
# 不好的做法: 逐個讀取
for attr in attributes:
    value = await pi_client.read_current_value(server, attr)

# 好的做法: 批次讀取
tags = [(server, attr) for attr in attributes]
values = await pi_client.read_multiple_current_values(tags)
```

### WebID 快取

```python
# 快取 WebID 避免重複查詢
self.webid_cache[cache_key] = webid
```

### 智慧訂閱

```python
# 根據變化率調整掃描間隔
if change_rate > threshold:
    mapping.scan_interval = 0.5  # 快速掃描
else:
    mapping.scan_interval = 5.0  # 慢速掃描
```

### 死區過濾

```python
# 只在值變化超過死區時更新
if abs(new_value - last_value) > mapping.deadband:
    await update_attribute(new_value)
```

## 總結

這個整合方案提供了:

1. **完整的物件映射**: IADL 定義 -> NDH 物件 -> PI Tags
2. **即時資料同步**: PI Tags 的變化自動反映到 NDH 物件
3. **歷史資料查詢**: 透過 NDH API 查詢 PI 歷史資料
4. **雙向控制**: 可以從 NDH 寫入控制命令到 PI
5. **事件驅動**: 整合 Kafka 實現事件溯源
6. **高效能**: 批次讀取、快取、智慧訂閱
7. **可擴展**: 支援多個 PI Server 和大量 Tags

---

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**最後更新**: 2025-01-09

