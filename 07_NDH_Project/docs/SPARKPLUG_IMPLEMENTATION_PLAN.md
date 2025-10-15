# NDH Sparkplug 整合實作方案

**版本**: 1.0  
**日期**: 2025-10-10  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)

---

## 目錄

1. [**實作概述**](#1-實作概述)
2. [**技術架構設計**](#2-技術架構設計)
3. [**連接器設計**](#3-連接器設計)
4. [**資料映射**](#4-資料映射)
5. [**實作計畫**](#5-實作計畫)
6. [**程式碼範例**](#6-程式碼範例)
7. [**測試策略**](#7-測試策略)
8. [**部署指南**](#8-部署指南)

---

## 1. 實作概述

### 1.1. 目標

實作一個完整的 Sparkplug 連接器,使 NDH 能夠:

1. 訂閱 Sparkplug MQTT 主題
2. 解析 Sparkplug B 有效負載
3. 將 Sparkplug 設備映射為 NDH 資產
4. 將 Sparkplug 資料發布為 NDH 事件
5. (可選) 發送命令到 Sparkplug 設備

### 1.2. 實作範圍

**Phase 1: MVP (最小可行產品)**

- ✅ Sparkplug 訂閱者連接器
- ✅ 解析 DBIRTH, DDATA, DDEATH 訊息
- ✅ 自動設備發現
- ✅ 資料映射到 NDH 事件
- ✅ 基本錯誤處理

**Phase 2: 完整功能 (可選)**

- ⏳ Sparkplug 發布者 (作為 Edge Node)
- ⏳ 命令支援 (DCMD, NCMD)
- ⏳ 完整狀態管理
- ⏳ Kafka 橋接

### 1.3. 技術選型

| 元件 | 選擇 | 理由 |
|------|------|------|
| **MQTT 客戶端** | Eclipse Paho MQTT | 成熟、穩定、廣泛使用 |
| **Sparkplug 庫** | pysparkplug | 功能完整、維護活躍 |
| **Protobuf** | protobuf (內建於 pysparkplug) | Sparkplug B 標準格式 |
| **MQTT Broker** | Eclipse Mosquitto / HiveMQ CE | 開源、完全相容 MQTT 3.1.1 |

---

## 2. 技術架構設計

### 2.1. 整體架構

```
┌─────────────────────────────────────────────────────────┐
│                    Sparkplug 設備                        │
│                  (Edge Node + Devices)                   │
└────────────────────┬────────────────────────────────────┘
                     │ Publish Sparkplug messages
                     │ (BIRTH, DATA, DEATH)
┌────────────────────▼────────────────────────────────────┐
│                    MQTT Broker                           │
│              (Mosquitto / HiveMQ CE)                     │
└────────────────────┬────────────────────────────────────┘
                     │ Subscribe to spBv1.0/#
                     │
┌────────────────────▼────────────────────────────────────┐
│                NDH Sparkplug Connector                   │
│  ┌────────────────────────────────────────────────┐    │
│  │  MQTT Client                                   │    │
│  │  - Connect to Broker                           │    │
│  │  - Subscribe to topics                         │    │
│  └────────────┬───────────────────────────────────┘    │
│               │                                          │
│  ┌────────────▼───────────────────────────────────┐    │
│  │  Sparkplug Parser                              │    │
│  │  - Parse Protobuf payload                      │    │
│  │  - Handle BIRTH/DATA/DEATH                     │    │
│  └────────────┬───────────────────────────────────┘    │
│               │                                          │
│  ┌────────────▼───────────────────────────────────┐    │
│  │  Data Mapper                                   │    │
│  │  - Map Edge Nodes to Assets                    │    │
│  │  - Map Devices to Assets                       │    │
│  │  - Map Metrics to Properties                   │    │
│  └────────────┬───────────────────────────────────┘    │
│               │                                          │
│  ┌────────────▼───────────────────────────────────┐    │
│  │  Event Publisher                               │    │
│  │  - Publish to NDH Event Service                │    │
│  └────────────────────────────────────────────────┘    │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                NDH Event Service (Kafka)                 │
│  - asset.created                                         │
│  - asset.deleted                                         │
│  - asset.data_changed                                    │
└─────────────────────────────────────────────────────────┘
```

### 2.2. 資料流

**1. 設備上線流程**:
```
Sparkplug Device
  │
  │ 1. Publish DBIRTH
  ▼
MQTT Broker
  │
  │ 2. Forward to Connector
  ▼
Sparkplug Connector
  │
  │ 3. Parse DBIRTH
  │ 4. Extract device info and metrics
  │ 5. Create NDH asset
  ▼
NDH Event Service
  │
  │ 6. Publish asset.created event
  ▼
Kafka
```

**2. 資料更新流程**:
```
Sparkplug Device
  │
  │ 1. Publish DDATA (on value change)
  ▼
MQTT Broker
  │
  │ 2. Forward to Connector
  ▼
Sparkplug Connector
  │
  │ 3. Parse DDATA
  │ 4. Extract metric values
  │ 5. Update NDH asset properties
  ▼
NDH Event Service
  │
  │ 6. Publish asset.data_changed event
  ▼
Kafka
```

**3. 設備離線流程**:
```
Sparkplug Device
  │
  │ 1. Publish DDEATH (or LWT)
  ▼
MQTT Broker
  │
  │ 2. Forward to Connector
  ▼
Sparkplug Connector
  │
  │ 3. Parse DDEATH
  │ 4. Mark asset as offline
  ▼
NDH Event Service
  │
  │ 5. Publish asset.status_changed event
  ▼
Kafka
```

---

## 3. 連接器設計

### 3.1. 類別結構

```python
class SparkplugConnector(BaseConnector):
    """Sparkplug MQTT Connector"""
    
    def __init__(self, connector_id: str, config: Dict[str, Any]):
        super().__init__(connector_id, config)
        self.mqtt_client: Optional[mqtt.Client] = None
        self.sparkplug_client: Optional[SparkplugClient] = None
        self.edge_nodes: Dict[str, EdgeNodeInfo] = {}
        self.devices: Dict[str, DeviceInfo] = {}
        self.callbacks: List[Callable] = []
    
    async def connect(self) -> bool:
        """連接到 MQTT Broker"""
    
    async def disconnect(self) -> bool:
        """斷開連接"""
    
    async def subscribe(self, group_id: str = "#") -> bool:
        """訂閱 Sparkplug 主題"""
    
    def _on_message(self, client, userdata, message):
        """處理 MQTT 訊息"""
    
    def _handle_nbirth(self, group_id: str, edge_node_id: str, payload):
        """處理 Edge Node BIRTH"""
    
    def _handle_dbirth(self, group_id: str, edge_node_id: str, device_id: str, payload):
        """處理 Device BIRTH"""
    
    def _handle_ndata(self, group_id: str, edge_node_id: str, payload):
        """處理 Edge Node DATA"""
    
    def _handle_ddata(self, group_id: str, edge_node_id: str, device_id: str, payload):
        """處理 Device DATA"""
    
    def _handle_ndeath(self, group_id: str, edge_node_id: str):
        """處理 Edge Node DEATH"""
    
    def _handle_ddeath(self, group_id: str, edge_node_id: str, device_id: str):
        """處理 Device DEATH"""
    
    async def publish_event(self, event: Dict[str, Any]):
        """發布事件到 NDH"""
```

### 3.2. 配置參數

```python
config = {
    # MQTT Broker 配置
    "broker_host": "localhost",
    "broker_port": 1883,
    "username": "sparkplug_user",  # 可選
    "password": "password",        # 可選
    
    # Sparkplug 配置
    "group_id": "#",  # 訂閱所有 Group,或指定特定 Group
    
    # NDH 配置
    "ndh_broker_url": "http://localhost:8000",
    "kafka_bootstrap_servers": "localhost:9092",
    
    # 進階配置
    "qos": 1,  # MQTT QoS
    "clean_session": False,
    "keep_alive": 60,
}
```

### 3.3. 主題訂閱模式

Sparkplug 主題格式:
```
spBv1.0/{group_id}/{message_type}/{edge_node_id}/{device_id}
```

**訂閱策略**:

1. **訂閱所有 Group**:
   ```python
   mqtt_client.subscribe("spBv1.0/#", qos=1)
   ```

2. **訂閱特定 Group**:
   ```python
   mqtt_client.subscribe("spBv1.0/Factory1/#", qos=1)
   ```

3. **訂閱特定訊息類型**:
   ```python
   mqtt_client.subscribe("spBv1.0/+/DBIRTH/#", qos=1)
   mqtt_client.subscribe("spBv1.0/+/DDATA/#", qos=1)
   mqtt_client.subscribe("spBv1.0/+/DDEATH/#", qos=1)
   ```

---

## 4. 資料映射

### 4.1. Sparkplug 到 NDH 的映射

| Sparkplug 概念 | NDH 概念 | 映射規則 |
|---------------|----------|---------|
| **Group** | Domain | `domain = f"sparkplug_{group_id}"` |
| **Edge Node** | Asset (Gateway) | `asset_id = f"{group_id}_{edge_node_id}"` |
| **Device** | Asset (Device) | `asset_id = f"{group_id}_{edge_node_id}_{device_id}"` |
| **Metric** | Property | `property_name = metric.name` |
| **Metric Value** | Property Value | `property_value = metric.value` |

### 4.2. 訊息類型映射

| Sparkplug 訊息 | NDH 事件 | 事件資料 |
|---------------|----------|---------|
| **NBIRTH** | `asset.created` | Edge Node 資訊和 Metrics |
| **DBIRTH** | `asset.created` | Device 資訊和 Metrics |
| **NDATA** | `asset.data_changed` | Edge Node Metric 值變更 |
| **DDATA** | `asset.data_changed` | Device Metric 值變更 |
| **NDEATH** | `asset.status_changed` | Edge Node 離線 |
| **DDEATH** | `asset.status_changed` | Device 離線 |

### 4.3. Metric 類型映射

| Sparkplug 資料類型 | Python 類型 | NDH 屬性類型 |
|-------------------|-------------|-------------|
| **Int8, Int16, Int32, Int64** | `int` | `integer` |
| **UInt8, UInt16, UInt32, UInt64** | `int` | `integer` |
| **Float** | `float` | `float` |
| **Double** | `float` | `float` |
| **Boolean** | `bool` | `boolean` |
| **String** | `str` | `string` |
| **DateTime** | `datetime` | `timestamp` |
| **Bytes** | `bytes` | `binary` |

---

## 5. 實作計畫

### 5.1. Phase 1: MVP (2 週)

#### Week 1: 核心開發

**Day 1-2: 環境設定和基礎架構**
- 安裝依賴 (pysparkplug, paho-mqtt)
- 建立 SparkplugConnector 基礎類別
- 實作 MQTT 連接邏輯

**Day 3-4: 訊息解析**
- 實作 DBIRTH 解析
- 實作 DDATA 解析
- 實作 DDEATH 解析

**Day 5: NDH 整合**
- 實作資料映射邏輯
- 實作事件發布到 Kafka

#### Week 2: 測試和文件

**Day 6-7: 測試**
- 單元測試
- 整合測試
- 使用 Mosquitto 和模擬設備測試

**Day 8-9: 文件**
- 撰寫技術文件
- 建立使用範例
- 撰寫 README

**Day 10: 審查和發布**
- 程式碼審查
- 整合到 NDH 主分支
- 發布到 GitHub

### 5.2. Phase 2: 完整功能 (1 週,可選)

**Day 11-12: 命令支援**
- 實作 DCMD 發布
- 實作 NCMD 發布

**Day 13-14: 完整狀態管理**
- 實作 NBIRTH/NDEATH 處理
- 實作 LWT 處理

**Day 15: Kafka 橋接**
- 實作 Sparkplug ↔ Kafka 橋接
- 測試和文件

---

## 6. 程式碼範例

### 6.1. 基本連接器實作

```python
"""
NDH Sparkplug Connector
"""

import asyncio
from typing import Dict, Any, Optional, List, Callable
from datetime import datetime

import paho.mqtt.client as mqtt
from pysparkplug import SparkplugClient, EdgeNode, Device
from loguru import logger

from .base import BaseConnector, ConnectorStatus


class SparkplugConnector(BaseConnector):
    """
    Sparkplug MQTT Connector for NDH
    
    Features:
    - Subscribe to Sparkplug topics
    - Parse Sparkplug B payloads
    - Map Sparkplug devices to NDH assets
    - Publish NDH events
    """
    
    def __init__(self, connector_id: str, config: Dict[str, Any]):
        super().__init__(connector_id, config)
        
        self.mqtt_client: Optional[mqtt.Client] = None
        self.edge_nodes: Dict[str, Dict] = {}
        self.devices: Dict[str, Dict] = {}
        self.callbacks: List[Callable] = []
    
    def get_required_config_fields(self) -> List[str]:
        return ["broker_host", "broker_port"]
    
    async def connect(self) -> bool:
        """連接到 MQTT Broker"""
        try:
            self._set_status(ConnectorStatus.CONNECTING)
            
            broker_host = self.config["broker_host"]
            broker_port = self.config.get("broker_port", 1883)
            
            logger.info(f"Connecting to MQTT Broker: {broker_host}:{broker_port}")
            
            # 建立 MQTT 客戶端
            self.mqtt_client = mqtt.Client(
                client_id=f"ndh_sparkplug_{self.connector_id}",
                clean_session=self.config.get("clean_session", False)
            )
            
            # 設定回調
            self.mqtt_client.on_connect = self._on_connect
            self.mqtt_client.on_message = self._on_message
            self.mqtt_client.on_disconnect = self._on_disconnect
            
            # 設定認證
            if "username" in self.config and "password" in self.config:
                self.mqtt_client.username_pw_set(
                    self.config["username"],
                    self.config["password"]
                )
            
            # 連接
            self.mqtt_client.connect(
                broker_host,
                broker_port,
                keepalive=self.config.get("keep_alive", 60)
            )
            
            # 啟動網路循環
            self.mqtt_client.loop_start()
            
            # 等待連接完成
            await asyncio.sleep(1)
            
            if self.status == ConnectorStatus.CONNECTED:
                logger.success(f"Sparkplug connector {self.connector_id} connected")
                return True
            else:
                return False
            
        except Exception as e:
            error_msg = f"Failed to connect to MQTT Broker: {str(e)}"
            logger.error(error_msg)
            self._set_status(ConnectorStatus.ERROR, error_msg)
            return False
    
    async def disconnect(self) -> bool:
        """斷開連接"""
        try:
            if self.mqtt_client:
                self.mqtt_client.loop_stop()
                self.mqtt_client.disconnect()
                self.mqtt_client = None
            
            self._set_status(ConnectorStatus.DISCONNECTED)
            logger.info(f"Sparkplug connector {self.connector_id} disconnected")
            
            return True
            
        except Exception as e:
            error_msg = f"Failed to disconnect: {str(e)}"
            logger.error(error_msg)
            return False
    
    async def subscribe(self, group_id: str = "#") -> bool:
        """訂閱 Sparkplug 主題"""
        if not self.mqtt_client:
            raise RuntimeError("Not connected to MQTT Broker")
        
        try:
            topic = f"spBv1.0/{group_id}/#"
            qos = self.config.get("qos", 1)
            
            self.mqtt_client.subscribe(topic, qos=qos)
            logger.info(f"Subscribed to Sparkplug topic: {topic}")
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to subscribe: {str(e)}")
            return False
    
    def _on_connect(self, client, userdata, flags, rc):
        """MQTT 連接回調"""
        if rc == 0:
            self._set_status(ConnectorStatus.CONNECTED)
            logger.info("MQTT connected successfully")
            
            # 自動訂閱
            group_id = self.config.get("group_id", "#")
            asyncio.create_task(self.subscribe(group_id))
        else:
            error_msg = f"MQTT connection failed with code: {rc}"
            logger.error(error_msg)
            self._set_status(ConnectorStatus.ERROR, error_msg)
    
    def _on_disconnect(self, client, userdata, rc):
        """MQTT 斷開回調"""
        if rc != 0:
            logger.warning(f"Unexpected MQTT disconnection: {rc}")
            self._set_status(ConnectorStatus.ERROR, "Unexpected disconnection")
    
    def _on_message(self, client, userdata, message):
        """處理 MQTT 訊息"""
        try:
            # 解析主題
            topic_parts = message.topic.split("/")
            
            if len(topic_parts) < 4:
                logger.warning(f"Invalid Sparkplug topic: {message.topic}")
                return
            
            namespace = topic_parts[0]  # spBv1.0
            group_id = topic_parts[1]
            message_type = topic_parts[2]
            edge_node_id = topic_parts[3]
            device_id = topic_parts[4] if len(topic_parts) > 4 else None
            
            # 解析 Payload (使用 pysparkplug)
            # 這裡需要使用 pysparkplug 的解析功能
            # payload = parse_sparkplug_payload(message.payload)
            
            # 根據訊息類型處理
            if message_type == "NBIRTH":
                self._handle_nbirth(group_id, edge_node_id, message.payload)
            elif message_type == "DBIRTH":
                self._handle_dbirth(group_id, edge_node_id, device_id, message.payload)
            elif message_type == "NDATA":
                self._handle_ndata(group_id, edge_node_id, message.payload)
            elif message_type == "DDATA":
                self._handle_ddata(group_id, edge_node_id, device_id, message.payload)
            elif message_type == "NDEATH":
                self._handle_ndeath(group_id, edge_node_id)
            elif message_type == "DDEATH":
                self._handle_ddeath(group_id, edge_node_id, device_id)
            
            self.data_count += 1
            
        except Exception as e:
            logger.error(f"Error processing Sparkplug message: {str(e)}")
    
    def _handle_dbirth(self, group_id: str, edge_node_id: str, device_id: str, payload):
        """處理 Device BIRTH"""
        try:
            # 建立 NDH 資產 ID
            asset_id = f"{group_id}_{edge_node_id}_{device_id}"
            
            # 儲存設備資訊
            self.devices[asset_id] = {
                "group_id": group_id,
                "edge_node_id": edge_node_id,
                "device_id": device_id,
                "metrics": {},  # 從 payload 解析
                "status": "online",
                "last_seen": datetime.now()
            }
            
            # 發布 NDH 事件
            event = {
                "event_type": "asset.created",
                "timestamp": datetime.now().isoformat(),
                "source": f"sparkplug://{group_id}/{edge_node_id}/{device_id}",
                "data": {
                    "asset_id": asset_id,
                    "asset_type": "sparkplug_device",
                    "group_id": group_id,
                    "edge_node_id": edge_node_id,
                    "device_id": device_id,
                    # "metrics": metrics  # 從 payload 解析
                }
            }
            
            asyncio.create_task(self.publish_event(event))
            
            logger.info(f"Device BIRTH: {asset_id}")
            
        except Exception as e:
            logger.error(f"Error handling DBIRTH: {str(e)}")
    
    def _handle_ddata(self, group_id: str, edge_node_id: str, device_id: str, payload):
        """處理 Device DATA"""
        try:
            asset_id = f"{group_id}_{edge_node_id}_{device_id}"
            
            # 更新設備資訊
            if asset_id in self.devices:
                self.devices[asset_id]["last_seen"] = datetime.now()
                # 更新 metrics
            
            # 發布 NDH 事件
            event = {
                "event_type": "asset.data_changed",
                "timestamp": datetime.now().isoformat(),
                "source": f"sparkplug://{group_id}/{edge_node_id}/{device_id}",
                "data": {
                    "asset_id": asset_id,
                    # "metrics": changed_metrics  # 從 payload 解析
                }
            }
            
            asyncio.create_task(self.publish_event(event))
            
        except Exception as e:
            logger.error(f"Error handling DDATA: {str(e)}")
    
    def _handle_ddeath(self, group_id: str, edge_node_id: str, device_id: str):
        """處理 Device DEATH"""
        try:
            asset_id = f"{group_id}_{edge_node_id}_{device_id}"
            
            # 更新設備狀態
            if asset_id in self.devices:
                self.devices[asset_id]["status"] = "offline"
            
            # 發布 NDH 事件
            event = {
                "event_type": "asset.status_changed",
                "timestamp": datetime.now().isoformat(),
                "source": f"sparkplug://{group_id}/{edge_node_id}/{device_id}",
                "data": {
                    "asset_id": asset_id,
                    "status": "offline"
                }
            }
            
            asyncio.create_task(self.publish_event(event))
            
            logger.info(f"Device DEATH: {asset_id}")
            
        except Exception as e:
            logger.error(f"Error handling DDEATH: {str(e)}")
    
    async def publish_event(self, event: Dict[str, Any]):
        """發布事件到 NDH (Kafka)"""
        # 這裡需要實作發布到 Kafka 的邏輯
        logger.debug(f"Publishing event: {event['event_type']}")
        
        # 呼叫回調
        for callback in self.callbacks:
            try:
                if asyncio.iscoroutinefunction(callback):
                    await callback(event)
                else:
                    callback(event)
            except Exception as e:
                logger.error(f"Error in event callback: {str(e)}")
    
    # 其他訊息處理方法的實作...
    def _handle_nbirth(self, group_id: str, edge_node_id: str, payload):
        pass
    
    def _handle_ndata(self, group_id: str, edge_node_id: str, payload):
        pass
    
    def _handle_ndeath(self, group_id: str, edge_node_id: str):
        pass
    
    # BaseConnector 抽象方法的實作
    async def read(self, tags: Optional[List[str]] = None) -> Dict[str, Any]:
        """讀取設備資料 (返回快取的資料)"""
        return {
            "edge_nodes": self.edge_nodes,
            "devices": self.devices
        }
    
    async def write(self, data: Dict[str, Any]) -> bool:
        """寫入資料 (發送命令)"""
        # Phase 2 實作
        raise NotImplementedError("Command support not yet implemented")
```

### 6.2. 使用範例

```python
"""
Sparkplug Connector 使用範例
"""

import asyncio
from ndh.connectors.sparkplug_connector import SparkplugConnector


async def on_event(event):
    """事件回調"""
    print(f"Event: {event['event_type']}")
    print(f"  Asset: {event['data'].get('asset_id')}")
    print(f"  Timestamp: {event['timestamp']}")


async def main():
    # 配置
    config = {
        "broker_host": "localhost",
        "broker_port": 1883,
        "group_id": "#",  # 訂閱所有 Group
    }
    
    # 建立連接器
    connector = SparkplugConnector(
        connector_id="sparkplug_001",
        config=config
    )
    
    # 註冊事件回調
    connector.callbacks.append(on_event)
    
    try:
        # 連接
        print("Connecting to MQTT Broker...")
        await connector.connect()
        
        print("Connected! Waiting for Sparkplug messages...")
        print("Press Ctrl+C to stop")
        
        # 持續運行
        while True:
            await asyncio.sleep(1)
    
    except KeyboardInterrupt:
        print("\nStopping...")
    
    finally:
        # 斷開連接
        await connector.disconnect()
        print("Disconnected")


if __name__ == "__main__":
    asyncio.run(main())
```

---

## 7. 測試策略

### 7.1. 單元測試

**測試範圍**:
- 連接/斷開邏輯
- 訊息解析
- 資料映射
- 事件發布

**工具**: pytest, pytest-asyncio

### 7.2. 整合測試

**測試環境**:
- MQTT Broker: Eclipse Mosquitto (Docker)
- 模擬設備: 使用 pysparkplug 建立模擬 Edge Node 和 Device
- NDH: 本地開發環境

**測試場景**:
1. 設備上線 (DBIRTH)
2. 資料更新 (DDATA)
3. 設備離線 (DDEATH)
4. 網路中斷恢復
5. 大量設備同時上線

### 7.3. 效能測試

**測試指標**:
- 訊息處理延遲
- 吞吐量 (messages/s)
- 記憶體使用
- CPU 使用

**目標**:
- 延遲 < 100ms
- 吞吐量 > 1,000 messages/s
- 記憶體 < 200MB (1,000 設備)

---

## 8. 部署指南

### 8.1. 依賴安裝

```bash
# 安裝 Python 依賴
pip install paho-mqtt pysparkplug protobuf

# 或使用 requirements.txt
pip install -r requirements.txt
```

### 8.2. MQTT Broker 部署

**使用 Docker 部署 Mosquitto**:

```bash
# 拉取映像
docker pull eclipse-mosquitto

# 建立配置檔
cat > mosquitto.conf << EOF
listener 1883
allow_anonymous true
EOF

# 啟動 Broker
docker run -d \
  --name mosquitto \
  -p 1883:1883 \
  -v $(pwd)/mosquitto.conf:/mosquitto/config/mosquitto.conf \
  eclipse-mosquitto
```

### 8.3. 連接器部署

**作為 NDH 的一部分**:

```bash
# 啟動 NDH (包含 Sparkplug 連接器)
python -m ndh.main
```

**獨立部署**:

```bash
# 啟動 Sparkplug 連接器
python -m ndh.connectors.sparkplug_connector
```

### 8.4. 配置範例

```yaml
# config.yaml
sparkplug:
  connector_id: sparkplug_main
  broker_host: mqtt.example.com
  broker_port: 1883
  username: ndh_user
  password: secret
  group_id: "#"
  qos: 1
  clean_session: false
  keep_alive: 60

ndh:
  broker_url: http://localhost:8000
  kafka_bootstrap_servers: localhost:9092
```

---

## 總結

本實作方案提供了一個完整的 Sparkplug 整合路線圖,從架構設計、連接器實作、資料映射到測試和部署,涵蓋了所有關鍵環節。

**關鍵要點**:

1. **使用成熟的庫**: pysparkplug 和 paho-mqtt
2. **清晰的資料映射**: Sparkplug 概念到 NDH 概念的一對一映射
3. **事件驅動**: 所有 Sparkplug 訊息都映射為 NDH 事件
4. **可擴展**: 架構支援未來的功能擴展 (如命令支援)
5. **易於測試**: 使用 Docker 和模擬設備進行測試

通過實施本方案,NDH 將能夠在 2-3 週內完成 Sparkplug 的整合,為客戶提供一個輕量級、標準化的 IIoT 整合選項。

---

**文件版本**: 1.0  
**最後更新**: 2025-10-10  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)

