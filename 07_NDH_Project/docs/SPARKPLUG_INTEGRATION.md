# NDH Sparkplug 整合文件

**版本**: 1.0  
**日期**: 2025-10-10  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)

---

## 目錄

1. [簡介](#1-簡介)
2. [Sparkplug 概述](#2-sparkplug-概述)
3. [NDH Sparkplug 連接器](#3-ndh-sparkplug-連接器)
4. [安裝與配置](#4-安裝與配置)
5. [使用範例](#5-使用範例)
6. [資料映射](#6-資料映射)
7. [與 Kafka 整合](#7-與-kafka-整合)
8. [效能與最佳實踐](#8-效能與最佳實踐)
9. [故障排除](#9-故障排除)
10. [參考資料](#10-參考資料)

---

## 1. 簡介

### 1.1. 目的

本文件說明如何使用 NDH 的 Sparkplug 連接器整合 Sparkplug 設備和系統。

### 1.2. 適用對象

- 系統整合工程師
- 工業自動化工程師
- NDH 開發人員
- IIoT 架構師

### 1.3. 主要功能

NDH Sparkplug 連接器提供以下功能:

- ✅ 訂閱 Sparkplug MQTT 主題
- ✅ 解析 Sparkplug B 有效負載
- ✅ 自動設備發現 (透過 BIRTH 訊息)
- ✅ 狀態管理 (透過 DEATH 訊息)
- ✅ 映射 Sparkplug 設備到 NDH 資產
- ✅ 發布 NDH 事件到 Kafka
- ✅ 統計和監控

---

## 2. Sparkplug 概述

### 2.1. 什麼是 Sparkplug?

Sparkplug 是一個基於 MQTT 的開源規範,專為工業物聯網 (IIoT) 設計。它由 Eclipse Foundation 維護,定義了:

1. **MQTT 主題命名空間**: 標準化的主題結構
2. **MQTT 狀態管理**: BIRTH/DEATH 訊息機制
3. **MQTT 有效負載**: Protocol Buffers (Protobuf) 格式

### 2.2. 主題結構

Sparkplug 主題格式:

```
spBv1.0/{group_id}/{message_type}/{edge_node_id}/{device_id}
```

**範例**:
```
spBv1.0/Factory1/DBIRTH/Gateway1/Sensor001
spBv1.0/Factory1/DDATA/Gateway1/Sensor001
spBv1.0/Factory1/DDEATH/Gateway1/Sensor001
```

### 2.3. 訊息類型

| 訊息類型 | 說明 | 方向 |
|---------|------|------|
| **NBIRTH** | Edge Node 上線 | Device → Broker |
| **NDEATH** | Edge Node 離線 | Device → Broker |
| **DBIRTH** | Device 上線 | Device → Broker |
| **DDEATH** | Device 離線 | Device → Broker |
| **NDATA** | Edge Node 資料 | Device → Broker |
| **DDATA** | Device 資料 | Device → Broker |
| **NCMD** | Edge Node 命令 | Broker → Device |
| **DCMD** | Device 命令 | Broker → Device |

### 2.4. 有效負載格式

Sparkplug B 使用 Protocol Buffers (Protobuf) 進行資料序列化:

```protobuf
message Payload {
    uint64 timestamp = 1;
    repeated Metric metrics = 2;
    uint64 seq = 3;
    string uuid = 4;
}

message Metric {
    string name = 1;
    uint64 alias = 2;
    uint64 timestamp = 3;
    uint32 datatype = 4;
    // ... value fields
}
```

---

## 3. NDH Sparkplug 連接器

### 3.1. 架構

```
┌─────────────────────────────────────────────────────────┐
│              Sparkplug 設備 (Edge Node + Devices)        │
└────────────────────┬────────────────────────────────────┘
                     │ Publish BIRTH/DATA/DEATH
┌────────────────────▼────────────────────────────────────┐
│                    MQTT Broker                           │
│              (Mosquitto / HiveMQ CE)                     │
└────────────────────┬────────────────────────────────────┘
                     │ Subscribe to spBv1.0/#
┌────────────────────▼────────────────────────────────────┐
│                NDH Sparkplug Connector                   │
│  ┌────────────────────────────────────────────────┐    │
│  │  MQTT Client (paho-mqtt)                       │    │
│  └────────────┬───────────────────────────────────┘    │
│               │                                          │
│  ┌────────────▼───────────────────────────────────┐    │
│  │  Sparkplug Parser                              │    │
│  │  - Parse topics                                │    │
│  │  - Parse payloads                              │    │
│  │  - Handle BIRTH/DATA/DEATH                     │    │
│  └────────────┬───────────────────────────────────┘    │
│               │                                          │
│  ┌────────────▼───────────────────────────────────┐    │
│  │  Data Mapper                                   │    │
│  │  - Map to NDH assets                           │    │
│  │  - Map to NDH events                           │    │
│  └────────────┬───────────────────────────────────┘    │
│               │                                          │
│  ┌────────────▼───────────────────────────────────┐    │
│  │  Event Publisher                               │    │
│  │  - Publish to Kafka                            │    │
│  │  - Call callbacks                              │    │
│  └────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
```

### 3.2. 核心類別

#### SparkplugConnector

主要連接器類別,繼承自 `BaseConnector`:

```python
class SparkplugConnector(BaseConnector):
    """Sparkplug MQTT Connector for NDH"""
    
    def __init__(self, connector_id: str, config: Dict[str, Any])
    async def connect(self) -> bool
    async def disconnect(self) -> bool
    async def subscribe(self, group_id: str = "#") -> bool
    async def publish_event(self, event: Dict[str, Any])
    def add_callback(self, callback: Callable)
    def get_statistics(self) -> Dict[str, Any]
```

### 3.3. 配置參數

| 參數 | 類型 | 必需 | 預設值 | 說明 |
|------|------|------|--------|------|
| `broker_host` | str | ✅ | - | MQTT Broker 主機 |
| `broker_port` | int | ✅ | 1883 | MQTT Broker 埠號 |
| `username` | str | ❌ | None | MQTT 使用者名稱 |
| `password` | str | ❌ | None | MQTT 密碼 |
| `group_id` | str | ❌ | "#" | 訂閱的 Group ID |
| `qos` | int | ❌ | 1 | MQTT QoS 等級 |
| `clean_session` | bool | ❌ | False | MQTT Clean Session |
| `keep_alive` | int | ❌ | 60 | MQTT Keep Alive (秒) |

---

## 4. 安裝與配置

### 4.1. 安裝依賴

```bash
# 基本依賴
pip install paho-mqtt loguru

# 可選: Kafka 整合
pip install aiokafka

# 可選: 完整的 Sparkplug B 支援 (Protobuf)
pip install pysparkplug
```

### 4.2. MQTT Broker 設定

#### 使用 Docker 部署 Mosquitto

```bash
# 拉取映像
docker pull eclipse-mosquitto

# 啟動 Broker
docker run -d \
  --name mosquitto \
  -p 1883:1883 \
  eclipse-mosquitto \
  mosquitto -c /mosquitto-no-auth.conf
```

#### 使用 Docker 部署 HiveMQ CE

```bash
# 拉取映像
docker pull hivemq/hivemq-ce

# 啟動 Broker
docker run -d \
  --name hivemq \
  -p 1883:1883 \
  hivemq/hivemq-ce
```

### 4.3. 基本配置

```python
config = {
    "broker_host": "localhost",
    "broker_port": 1883,
    "group_id": "#",  # 訂閱所有 Group
    "qos": 1,
}
```

### 4.4. 認證配置

```python
config = {
    "broker_host": "mqtt.example.com",
    "broker_port": 1883,
    "username": "ndh_user",
    "password": "secret_password",
    "group_id": "Factory1",
    "qos": 1,
}
```

---

## 5. 使用範例

### 5.1. 基本訂閱

```python
import asyncio
from ndh.connectors.sparkplug_connector import SparkplugConnector

async def on_event(event):
    """事件回調"""
    print(f"Event: {event['event_type']}")
    print(f"Asset: {event['data']['asset_id']}")

async def main():
    # 配置
    config = {
        "broker_host": "localhost",
        "broker_port": 1883,
        "group_id": "#",
    }
    
    # 建立連接器
    connector = SparkplugConnector("sparkplug_001", config)
    
    # 註冊回調
    connector.add_callback(on_event)
    
    # 連接
    await connector.connect()
    
    # 持續運行
    while True:
        await asyncio.sleep(1)

if __name__ == "__main__":
    asyncio.run(main())
```

### 5.2. 訂閱特定 Group

```python
config = {
    "broker_host": "localhost",
    "broker_port": 1883,
    "group_id": "Factory1",  # 只訂閱 Factory1
}
```

### 5.3. 獲取統計資訊

```python
# 獲取統計
stats = connector.get_statistics()

print(f"Messages: {stats['message_count']}")
print(f"Births: {stats['birth_count']}")
print(f"Deaths: {stats['death_count']}")
print(f"Data: {stats['data_count']}")
print(f"Edge Nodes: {stats['edge_node_count']}")
print(f"Devices: {stats['device_count']}")
```

---

## 6. 資料映射

### 6.1. Sparkplug 到 NDH 的映射

| Sparkplug 概念 | NDH 概念 | 映射規則 |
|---------------|----------|---------|
| **Group** | Domain | `domain = f"sparkplug_{group_id}"` |
| **Edge Node** | Asset (Gateway) | `asset_id = f"{group_id}_{edge_node_id}"` |
| **Device** | Asset (Device) | `asset_id = f"{group_id}_{edge_node_id}_{device_id}"` |
| **Metric** | Property | `property_name = metric.name` |

### 6.2. 訊息類型映射

| Sparkplug 訊息 | NDH 事件 | 說明 |
|---------------|----------|------|
| **NBIRTH** | `asset.created` | Edge Node 上線 |
| **DBIRTH** | `asset.created` | Device 上線 |
| **NDATA** | `asset.data_changed` | Edge Node 資料變更 |
| **DDATA** | `asset.data_changed` | Device 資料變更 |
| **NDEATH** | `asset.status_changed` | Edge Node 離線 |
| **DDEATH** | `asset.status_changed` | Device 離線 |

### 6.3. 事件結構

#### asset.created (DBIRTH)

```json
{
  "event_type": "asset.created",
  "timestamp": "2025-10-10T12:34:56.789",
  "source": "sparkplug://Factory1/Gateway1/Sensor001",
  "data": {
    "asset_id": "Factory1_Gateway1_Sensor001",
    "asset_type": "sparkplug_device",
    "group_id": "Factory1",
    "edge_node_id": "Gateway1",
    "device_id": "Sensor001",
    "metrics": {
      "temperature": 25.5,
      "humidity": 60.2,
      "pressure": 1013.25
    }
  }
}
```

#### asset.data_changed (DDATA)

```json
{
  "event_type": "asset.data_changed",
  "timestamp": "2025-10-10T12:35:01.234",
  "source": "sparkplug://Factory1/Gateway1/Sensor001",
  "data": {
    "asset_id": "Factory1_Gateway1_Sensor001",
    "metrics": {
      "temperature": 26.1,
      "humidity": 59.8
    }
  }
}
```

#### asset.status_changed (DDEATH)

```json
{
  "event_type": "asset.status_changed",
  "timestamp": "2025-10-10T12:40:00.000",
  "source": "sparkplug://Factory1/Gateway1/Sensor001",
  "data": {
    "asset_id": "Factory1_Gateway1_Sensor001",
    "status": "offline"
  }
}
```

---

## 7. 與 Kafka 整合

### 7.1. Kafka 主題映射

| NDH 事件 | Kafka 主題 |
|---------|-----------|
| `asset.created` | `ndh.assets.created` |
| `asset.data_changed` | `ndh.assets.data_changed` |
| `asset.status_changed` | `ndh.assets.status_changed` |

### 7.2. Sparkplug to Kafka 橋接

```python
from ndh.connectors.sparkplug_connector import SparkplugConnector
from aiokafka import AIOKafkaProducer
import json

class SparkplugToKafkaBridge:
    def __init__(self, sparkplug_config, kafka_servers):
        self.sparkplug = SparkplugConnector("bridge", sparkplug_config)
        self.kafka = AIOKafkaProducer(
            bootstrap_servers=kafka_servers,
            value_serializer=lambda v: json.dumps(v).encode('utf-8')
        )
        self.sparkplug.add_callback(self.on_event)
    
    async def start(self):
        await self.sparkplug.connect()
        await self.kafka.start()
    
    async def on_event(self, event):
        topic = f"ndh.assets.{event['event_type'].split('.')[1]}"
        await self.kafka.send_and_wait(topic, value=event)
```

---

## 8. 效能與最佳實踐

### 8.1. 效能指標

| 指標 | 目標值 |
|------|--------|
| **訊息延遲** | < 100ms |
| **吞吐量** | > 1,000 messages/s |
| **記憶體使用** | < 200MB (1,000 設備) |
| **CPU 使用** | < 10% (1,000 設備) |

### 8.2. 最佳實踐

#### 1. 使用適當的 QoS

- **QoS 0**: 最快,但可能丟失訊息
- **QoS 1**: 推薦,至少一次傳遞
- **QoS 2**: 最慢,但保證只傳遞一次

#### 2. 訂閱策略

```python
# 訂閱所有 Group (開發/測試)
config = {"group_id": "#"}

# 訂閱特定 Group (生產環境)
config = {"group_id": "Factory1"}

# 訂閱多個 Group (需要多個連接器)
```

#### 3. 錯誤處理

```python
try:
    await connector.connect()
except Exception as e:
    logger.error(f"Connection failed: {e}")
    # 實作重試邏輯
```

#### 4. 監控和日誌

```python
# 定期輸出統計
async def monitor():
    while True:
        stats = connector.get_statistics()
        logger.info(f"Stats: {stats}")
        await asyncio.sleep(60)
```

---

## 9. 故障排除

### 9.1. 連接失敗

**問題**: `Failed to connect to MQTT Broker`

**解決方案**:
1. 檢查 Broker 是否運行: `docker ps`
2. 檢查埠號是否可訪問: `telnet localhost 1883`
3. 檢查認證資訊是否正確
4. 檢查防火牆設定

### 9.2. 沒有收到訊息

**問題**: 連接成功但沒有收到訊息

**解決方案**:
1. 檢查設備是否發布訊息
2. 使用 MQTT 客戶端工具訂閱 `spBv1.0/#` 確認訊息
3. 檢查 `group_id` 配置是否正確
4. 檢查 QoS 設定

### 9.3. 記憶體洩漏

**問題**: 記憶體使用持續增長

**解決方案**:
1. 定期清理離線設備資料
2. 限制快取的設備數量
3. 使用 LRU 快取策略

---

## 10. 參考資料

### 10.1. Sparkplug 規範

- [Sparkplug Specification](https://sparkplug.eclipse.org/specification/)
- [Eclipse Tahu (Reference Implementation)](https://github.com/eclipse-tahu/tahu)

### 10.2. MQTT 資源

- [Eclipse Paho MQTT Python Client](https://github.com/eclipse/paho.mqtt.python)
- [MQTT 3.1.1 Specification](https://docs.oasis-open.org/mqtt/mqtt/v3.1.1/mqtt-v3.1.1.html)

### 10.3. NDH 文件

- [Sparkplug 分析與建議](SPARKPLUG_ANALYSIS_AND_RECOMMENDATION.md)
- [Sparkplug 實作方案](SPARKPLUG_IMPLEMENTATION_PLAN.md)
- [Kafka 整合](KAFKA_INTEGRATION.md)

---

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**最後更新**: 2025-10-10  
**版本**: 1.0

