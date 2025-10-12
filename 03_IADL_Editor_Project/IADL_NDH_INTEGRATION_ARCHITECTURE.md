# IADL Editor 與 Neutral Data Hub 整合架構

## 文件資訊

**文件名稱**: IADL Editor 與 NDH 整合架構  
**版本**: 1.1  
**日期**: 2024-10-09  
**作者**: Manus AI Assistant  
**狀態**: 規劃階段

---

## 目錄

1. [執行摘要](#執行摘要)
2. [專案定位](#專案定位)
3. [整合架構概述](#整合架構概述)
4. [NDH 專案架構](#ndh-專案架構)
5. [訊息佇列選擇: Redis vs Kafka](#訊息佇列選擇-redis-vs-kafka)
6. [IADL Editor 與 NDH 整合](#iadl-editor-與-ndh-整合)
7. [資料流程](#資料流程)
8. [API 設計](#api-設計)
9. [實作計畫](#實作計畫)
10. [測試策略](#測試策略)
11. [部署架構](#部署架構)

---

## 執行摘要

本文件描述了 IADL Editor 和 Neutral Data Hub (NDH) 兩個獨立專案的整合架構。IADL Editor 作為離線的資產建模和配置工具,NDH 作為即時的資料交換中間件服務,兩者透過標準化的 API 和資料格式進行整合,形成完整的工業資產管理和資料整合解決方案。

### 核心價值

**IADL Editor 提供**:
- 資產模型定義和管理
- 多系統資料格式轉換
- NDH 配置生成和管理
- 離線資料分析和視覺化

**NDH 提供**:
- 即時資料交換和路由
- 多系統連接和整合
- 資料緩衝和佇列管理
- 事件驅動的資料處理

**整合後的優勢**:
- 統一的資料模型 (IADL)
- 設計時和執行時的完整支援
- 降低系統整合複雜度
- 提高資料品質和一致性

---

## 專案定位

### IADL Editor

**類型**: 桌面應用程式 (離線工具)

**主要功能**:
- 工業資產描述語言 (IADL) 編輯器
- 資產模型建立和管理
- 多系統資料格式匯入/匯出
- 資料驗證和轉換
- 配置管理

**技術棧**:
- C++17
- Qt 6 (跨平台 GUI 框架)
- qmake (建置系統)
- 插件架構

**使用場景**:
- 系統設計階段的資產建模
- 資料格式轉換和遷移
- 配置檔案生成
- 離線資料分析

**目標使用者**:
- 系統工程師
- 自動化工程師
- 系統整合商
- 專案經理

### Neutral Data Hub (NDH)

**類型**: 伺服器端服務 (執行時中間件)

**主要功能**:
- 即時資料交換和路由
- 多系統連接管理
- 資料轉換和映射
- 訊息佇列和緩衝
- 事件驅動處理
- 資料持久化

**技術棧** (建議):
- Python 3.11+ 或 Node.js
- FastAPI/Flask 或 Express.js
- Redis (訊息佇列和快取)
- PostgreSQL (資料持久化)
- Docker (容器化部署)
- Kubernetes (編排,可選)

**使用場景**:
- 生產環境的即時資料整合
- 多系統資料同步
- 事件驅動的自動化
- 資料收集和分發

**目標使用者**:
- 系統管理員
- DevOps 工程師
- 資料工程師
- 營運團隊

### 專案關係

```
設計階段                     執行階段
┌─────────────────┐         ┌─────────────────┐
│  IADL Editor    │         │      NDH        │
│  (離線工具)      │ ──配置→ │  (執行時服務)    │
│                 │         │                 │
│ • 資產建模       │         │ • 即時資料交換   │
│ • 格式轉換       │ ←─資料─ │ • 系統連接       │
│ • 配置生成       │         │ • 事件處理       │
└─────────────────┘         └─────────────────┘
        ↓                           ↓
   IADL 模型                   執行時資料
```

---

## 整合架構概述

### 整合層級

#### 1. 資料模型層 (Data Model Layer)

**IADL 作為共同語言**:
- IADL Editor 定義和編輯 IADL 模型
- NDH 使用 IADL 模型作為資料結構標準
- 兩者共享相同的資料模型定義

**資料模型內容**:
- 資產定義 (Asset Definitions)
- 屬性結構 (Property Schemas)
- 關係定義 (Relationships)
- 類型系統 (Type System)

#### 2. 配置層 (Configuration Layer)

**IADL Editor 生成 NDH 配置**:
- 連接器配置 (Connector Configurations)
- 路由規則 (Routing Rules)
- 資料映射 (Data Mappings)
- 轉換邏輯 (Transformation Logic)

**配置格式**:
- JSON/YAML 格式的配置檔案
- 基於 IADL 模型的結構化配置
- 版本控制和驗證

#### 3. API 層 (API Layer)

**IADL Editor 呼叫 NDH API**:
- 推送配置到 NDH
- 查詢執行時資料
- 監控 NDH 狀態
- 管理連接器

**NDH 提供 REST API**:
- 配置管理 API
- 資料查詢 API
- 監控和管理 API
- WebSocket (即時資料)

#### 4. 資料交換層 (Data Exchange Layer)

**雙向資料流**:
- IADL Editor → NDH: 配置、模型定義
- NDH → IADL Editor: 執行時資料、統計資訊
- 使用標準化的 IADL 格式

### 整合架構圖

```
┌─────────────────────────────────────────────────────────────┐
│                        IADL Editor                          │
│  ┌────────────┐  ┌────────────┐  ┌────────────┐           │
│  │ 資產建模器  │  │ 配置生成器  │  │ NDH 客戶端 │           │
│  └────────────┘  └────────────┘  └────────────┘           │
│         │              │                │                   │
│         └──────────────┴────────────────┘                   │
│                        │                                     │
│                   IADL 模型                                  │
└────────────────────────┼────────────────────────────────────┘
                         │
                    REST API / WebSocket
                         │
┌────────────────────────┼────────────────────────────────────┐
│                        │          NDH                        │
│                   ┌────┴────┐                               │
│                   │ API 層  │                               │
│                   └────┬────┘                               │
│         ┌──────────────┼──────────────┐                    │
│         │              │              │                     │
│    ┌────┴────┐   ┌────┴────┐   ┌────┴────┐               │
│    │路由引擎 │   │轉換引擎 │   │佇列管理 │               │
│    └────┬────┘   └────┬────┘   └────┬────┘               │
│         │              │              │                     │
│         └──────────────┴──────────────┘                    │
│                        │                                     │
│                   ┌────┴────┐                               │
│                   │連接器層 │                               │
│                   └────┬────┘                               │
└────────────────────────┼────────────────────────────────────┘
                         │
        ┌────────────────┼────────────────┐
        │                │                │
   ┌────┴────┐     ┌────┴────┐     ┌────┴────┐
   │ SCADA   │     │  DCS    │     │  MES    │
   │ 系統    │     │  系統   │     │  系統   │
   └─────────┘     └─────────┘     └─────────┘
```

---

## NDH 專案架構

### 核心元件

#### 1. API 層 (API Layer)

**REST API 伺服器**:
```python
# 使用 FastAPI 範例
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

app = FastAPI(title="Neutral Data Hub API")

# 配置管理
@app.post("/api/v1/config")
async def upload_config(config: dict):
    """上傳 IADL 配置"""
    pass

@app.get("/api/v1/config/{config_id}")
async def get_config(config_id: str):
    """取得配置"""
    pass

# 資料查詢
@app.get("/api/v1/assets")
async def list_assets():
    """列出所有資產"""
    pass

@app.get("/api/v1/assets/{asset_id}")
async def get_asset(asset_id: str):
    """取得資產資料"""
    pass

# 即時資料
@app.websocket("/ws/realtime")
async def websocket_endpoint(websocket: WebSocket):
    """WebSocket 即時資料推送"""
    pass
```

**API 端點分類**:

1. **配置管理 API**
   - `POST /api/v1/config` - 上傳配置
   - `GET /api/v1/config/{id}` - 取得配置
   - `PUT /api/v1/config/{id}` - 更新配置
   - `DELETE /api/v1/config/{id}` - 刪除配置
   - `GET /api/v1/config` - 列出所有配置

2. **資產管理 API**
   - `GET /api/v1/assets` - 列出資產
   - `GET /api/v1/assets/{id}` - 取得資產詳情
   - `POST /api/v1/assets` - 建立資產
   - `PUT /api/v1/assets/{id}` - 更新資產
   - `DELETE /api/v1/assets/{id}` - 刪除資產

3. **資料查詢 API**
   - `GET /api/v1/data/current` - 取得當前值
   - `GET /api/v1/data/history` - 取得歷史資料
   - `POST /api/v1/data/query` - 複雜查詢

4. **連接器管理 API**
   - `GET /api/v1/connectors` - 列出連接器
   - `POST /api/v1/connectors` - 建立連接器
   - `GET /api/v1/connectors/{id}/status` - 連接器狀態
   - `POST /api/v1/connectors/{id}/start` - 啟動連接器
   - `POST /api/v1/connectors/{id}/stop` - 停止連接器

5. **監控 API**
   - `GET /api/v1/health` - 健康檢查
   - `GET /api/v1/metrics` - 系統指標
   - `GET /api/v1/logs` - 日誌查詢

#### 2. 路由引擎 (Routing Engine)

**功能**:
- 根據規則路由資料
- 支援條件路由
- 負載平衡
- 錯誤處理和重試

**路由規則範例**:
```json
{
  "routes": [
    {
      "id": "route_001",
      "name": "Temperature Data Route",
      "source": {
        "connector": "scada_connector_1",
        "filter": {
          "assetType": "Sensor",
          "properties.sensorType": "Temperature"
        }
      },
      "destinations": [
        {
          "connector": "historian_connector_1",
          "transform": "temperature_transform"
        },
        {
          "connector": "mes_connector_1",
          "condition": "value > 80"
        }
      ]
    }
  ]
}
```

#### 3. 轉換引擎 (Transformation Engine)

**功能**:
- 資料格式轉換
- 單位轉換
- 資料映射
- 計算和聚合

**轉換規則範例**:
```json
{
  "transforms": [
    {
      "id": "temperature_transform",
      "name": "Temperature Unit Conversion",
      "type": "unit_conversion",
      "config": {
        "from_unit": "celsius",
        "to_unit": "fahrenheit",
        "formula": "value * 9/5 + 32"
      }
    },
    {
      "id": "asset_mapping",
      "name": "SCADA to MES Asset Mapping",
      "type": "field_mapping",
      "config": {
        "mappings": [
          {"source": "TagName", "target": "EquipmentID"},
          {"source": "Value", "target": "CurrentValue"},
          {"source": "Quality", "target": "DataQuality"}
        ]
      }
    }
  ]
}
```

#### 4. 連接器層 (Connector Layer)

**連接器架構**:
```python
from abc import ABC, abstractmethod

class Connector(ABC):
    """連接器基礎類別"""
    
    def __init__(self, config: dict):
        self.config = config
        self.is_connected = False
    
    @abstractmethod
    async def connect(self):
        """建立連接"""
        pass
    
    @abstractmethod
    async def disconnect(self):
        """斷開連接"""
        pass
    
    @abstractmethod
    async def read(self, asset_id: str):
        """讀取資料"""
        pass
    
    @abstractmethod
    async def write(self, asset_id: str, value: any):
        """寫入資料"""
        pass
    
    @abstractmethod
    async def subscribe(self, asset_ids: list, callback):
        """訂閱資料變化"""
        pass

class OPCUAConnector(Connector):
    """OPC UA 連接器實作"""
    
    async def connect(self):
        # OPC UA 連接邏輯
        pass
    
    async def read(self, asset_id: str):
        # 讀取 OPC UA 節點
        pass

class ModbusConnector(Connector):
    """Modbus 連接器實作"""
    
    async def connect(self):
        # Modbus 連接邏輯
        pass
    
    async def read(self, asset_id: str):
        # 讀取 Modbus 暫存器
        pass
```

**支援的連接器類型**:
- OPC UA Connector
- Modbus TCP/RTU Connector
- MQTT Connector
- REST API Connector
- Database Connector (SQL)
- File System Connector
- Kafka Connector

#### 5. 訊息佇列管理 (Message Queue Manager)

**功能**:
- 資料緩衝
- 非同步處理
- 負載平衡
- 可靠傳輸

**使用 Redis 實作**:
```python
import redis
import json

class MessageQueue:
    def __init__(self, redis_host='localhost', redis_port=6379):
        self.redis_client = redis.Redis(
            host=redis_host, 
            port=redis_port, 
            decode_responses=True
        )
    
    async def enqueue(self, queue_name: str, message: dict):
        """加入訊息到佇列"""
        self.redis_client.lpush(
            queue_name, 
            json.dumps(message)
        )
    
    async def dequeue(self, queue_name: str):
        """從佇列取出訊息"""
        message = self.redis_client.brpop(queue_name, timeout=1)
        if message:
            return json.loads(message[1])
        return None
    
    async def get_queue_length(self, queue_name: str):
        """取得佇列長度"""
        return self.redis_client.llen(queue_name)
```

#### 6. 資料持久化層 (Data Persistence Layer)

**功能**:
- 配置儲存
- 歷史資料儲存
- 審計日誌
- 快取管理

**資料庫設計**:
```sql
-- 資產表
CREATE TABLE assets (
    id VARCHAR(255) PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    type VARCHAR(100) NOT NULL,
    properties JSONB,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 資產資料表 (時間序列)
CREATE TABLE asset_data (
    id SERIAL PRIMARY KEY,
    asset_id VARCHAR(255) REFERENCES assets(id),
    timestamp TIMESTAMP NOT NULL,
    value JSONB NOT NULL,
    quality VARCHAR(50),
    source VARCHAR(255)
);

-- 配置表
CREATE TABLE configurations (
    id VARCHAR(255) PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    type VARCHAR(100) NOT NULL,
    config JSONB NOT NULL,
    version INTEGER DEFAULT 1,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 連接器表
CREATE TABLE connectors (
    id VARCHAR(255) PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    type VARCHAR(100) NOT NULL,
    config JSONB NOT NULL,
    status VARCHAR(50) DEFAULT 'stopped',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 審計日誌表
CREATE TABLE audit_logs (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    user_id VARCHAR(255),
    action VARCHAR(100) NOT NULL,
    resource_type VARCHAR(100),
    resource_id VARCHAR(255),
    details JSONB
);

-- 索引
CREATE INDEX idx_asset_data_asset_id ON asset_data(asset_id);
CREATE INDEX idx_asset_data_timestamp ON asset_data(timestamp);
CREATE INDEX idx_audit_logs_timestamp ON audit_logs(timestamp);
```

### NDH 專案結構

```
ndh/
├── api/                          # API 層
│   ├── __init__.py
│   ├── main.py                   # FastAPI 主程式
│   ├── routes/                   # API 路由
│   │   ├── __init__.py
│   │   ├── config.py             # 配置管理 API
│   │   ├── assets.py             # 資產管理 API
│   │   ├── data.py               # 資料查詢 API
│   │   ├── connectors.py         # 連接器管理 API
│   │   └── monitoring.py         # 監控 API
│   └── models/                   # API 資料模型
│       ├── __init__.py
│       ├── asset.py
│       ├── config.py
│       └── connector.py
├── core/                         # 核心引擎
│   ├── __init__.py
│   ├── routing_engine.py         # 路由引擎
│   ├── transformation_engine.py  # 轉換引擎
│   ├── message_queue.py          # 訊息佇列
│   └── event_processor.py        # 事件處理器
├── connectors/                   # 連接器
│   ├── __init__.py
│   ├── base.py                   # 連接器基礎類別
│   ├── opcua.py                  # OPC UA 連接器
│   ├── modbus.py                 # Modbus 連接器
│   ├── mqtt.py                   # MQTT 連接器
│   ├── rest.py                   # REST API 連接器
│   └── database.py               # 資料庫連接器
├── persistence/                  # 資料持久化
│   ├── __init__.py
│   ├── database.py               # 資料庫管理
│   ├── models.py                 # ORM 模型
│   └── migrations/               # 資料庫遷移
├── iadl/                         # IADL 支援
│   ├── __init__.py
│   ├── parser.py                 # IADL 解析器
│   ├── validator.py              # IADL 驗證器
│   └── schema.py                 # IADL 結構定義
├── utils/                        # 工具函數
│   ├── __init__.py
│   ├── logger.py                 # 日誌工具
│   ├── config.py                 # 配置管理
│   └── security.py               # 安全工具
├── tests/                        # 測試
│   ├── __init__.py
│   ├── test_api.py
│   ├── test_routing.py
│   ├── test_connectors.py
│   └── fixtures/                 # 測試資料
├── docker/                       # Docker 配置
│   ├── Dockerfile
│   ├── docker-compose.yml
│   └── .dockerignore
├── docs/                         # 文件
│   ├── API.md
│   ├── ARCHITECTURE.md
│   ├── DEPLOYMENT.md
│   └── DEVELOPMENT.md
├── requirements.txt              # Python 依賴
├── setup.py                      # 安裝腳本
├── README.md                     # 專案說明
└── .env.example                  # 環境變數範例
```

---

## 訊息佇列選擇: Redis vs Kafka

### 概述

NDH 的核心功能之一是處理大量的工業資料流,這需要一個強大且可靠的訊息佇列系統。本章節詳細比較 Redis 和 Kafka 兩種主流解決方案,並提供選擇標準和實作建議。

### 為什麼需要訊息佇列?

在工業環境中,訊息佇列扮演關鍵角色:

**解耦系統**:
- 資料生產者和消費者獨立運作
- 系統可以獨立升級和維護
- 降低系統間的相依性

**緩衝資料流**:
- 處理突發的資料流量
- 平滑資料處理速度
- 防止系統過載

**可靠傳輸**:
- 確保資料不遺失
- 支援重試機制
- 提供傳輸確認

**非同步處理**:
- 提高系統回應速度
- 支援背景任務
- 最佳化資源使用

### Redis 深入分析

#### Redis 架構

```
Redis Server
├── 記憶體儲存 (主要)
│   ├── String
│   ├── List (用於佇列)
│   ├── Set
│   ├── Hash
│   └── Sorted Set
├── 持久化 (可選)
│   ├── RDB (快照)
│   └── AOF (日誌)
└── Pub/Sub (發布訂閱)
```

#### Redis 作為訊息佇列

**List 資料結構**:
```python
import redis

r = redis.Redis(host='localhost', port=6379)

# 生產者: 推送訊息到佇列
r.lpush('sensor_data_queue', json.dumps({
    'asset_id': 'sensor_001',
    'value': 25.5,
    'timestamp': '2024-10-09T10:30:00Z'
}))

# 消費者: 從佇列取出訊息 (阻塞式)
while True:
    message = r.brpop('sensor_data_queue', timeout=1)
    if message:
        data = json.loads(message[1])
        process_data(data)
```

**Pub/Sub 模式**:
```python
# 發布者
r.publish('temperature_updates', json.dumps({
    'sensor_id': 'temp_001',
    'value': 25.5
}))

# 訂閱者
pubsub = r.pubsub()
pubsub.subscribe('temperature_updates')

for message in pubsub.listen():
    if message['type'] == 'message':
        data = json.loads(message['data'])
        handle_update(data)
```

**Streams (Redis 5.0+)**:
```python
# 更進階的訊息佇列功能
# 支援消費者群組、訊息確認、持久化

# 生產者
r.xadd('sensor_stream', {
    'asset_id': 'sensor_001',
    'value': '25.5',
    'timestamp': '2024-10-09T10:30:00Z'
})

# 消費者群組
r.xgroup_create('sensor_stream', 'processing_group', id='0')
messages = r.xreadgroup('processing_group', 'consumer1', 
                        {'sensor_stream': '>'}, count=10)

for stream, message_list in messages:
    for message_id, data in message_list:
        process_data(data)
        r.xack('sensor_stream', 'processing_group', message_id)
```

#### Redis 優勢

**極高的效能**:
- 記憶體操作,延遲 < 1ms
- 單執行緒模型,避免鎖競爭
- 每秒可處理 100,000+ 操作

**簡單易用**:
- 安裝和配置簡單
- API 直觀易懂
- 豐富的客戶端函式庫

**多功能**:
- 不僅是訊息佇列
- 可作為快取
- 可作為 Session 儲存
- 支援多種資料結構

**輕量級**:
- 記憶體佔用小
- 資源需求低
- 適合小型部署

#### Redis 限制

**持久化較弱**:
- 主要依賴記憶體
- RDB 快照有資料遺失風險
- AOF 會影響效能

**可擴展性受限**:
- 單機效能受記憶體限制
- Redis Cluster 配置複雜
- 不適合超大規模資料

**訊息保留有限**:
- 沒有內建的長期訊息保留
- 訊息被消費後即刪除
- 不支援時間旅行(重播舊資料)

**Pub/Sub 可靠性**:
- 訊息不持久化
- 訂閱者離線時會遺失訊息
- 沒有訊息確認機制

### Kafka 深入分析

#### Kafka 架構

```
Kafka Cluster
├── Broker 1
│   ├── Topic A - Partition 0 (Leader)
│   ├── Topic A - Partition 1 (Follower)
│   └── Topic B - Partition 0 (Follower)
├── Broker 2
│   ├── Topic A - Partition 1 (Leader)
│   ├── Topic B - Partition 0 (Leader)
│   └── Topic B - Partition 1 (Follower)
└── Broker 3
    ├── Topic A - Partition 0 (Follower)
    ├── Topic B - Partition 1 (Leader)
    └── Topic C - Partition 0 (Leader)

ZooKeeper / KRaft
└── 管理叢集元資料和協調
```

#### Kafka 核心概念

**Topic (主題)**:
- 訊息的邏輯分類
- 例如: `temperature_sensors`, `pressure_sensors`

**Partition (分區)**:
- Topic 的物理分割
- 實現平行處理和負載平衡
- 每個 Partition 是有序的訊息序列

**Producer (生產者)**:
- 發送訊息到 Topic
- 可指定 Partition 或由 Kafka 自動分配

**Consumer (消費者)**:
- 從 Topic 讀取訊息
- 可組成 Consumer Group 實現負載平衡

**Offset (偏移量)**:
- 訊息在 Partition 中的位置
- 消費者追蹤已讀取的位置
- 支援重播和恢復

#### Kafka 作為訊息佇列

**生產者範例**:
```python
from kafka import KafkaProducer
import json

producer = KafkaProducer(
    bootstrap_servers=['localhost:9092'],
    value_serializer=lambda v: json.dumps(v).encode('utf-8')
)

# 發送訊息
producer.send('sensor_data', {
    'asset_id': 'sensor_001',
    'value': 25.5,
    'timestamp': '2024-10-09T10:30:00Z'
})

producer.flush()
```

**消費者範例**:
```python
from kafka import KafkaConsumer
import json

consumer = KafkaConsumer(
    'sensor_data',
    bootstrap_servers=['localhost:9092'],
    group_id='ndh_processors',
    value_deserializer=lambda m: json.loads(m.decode('utf-8')),
    auto_offset_reset='earliest',  # 從最早的訊息開始
    enable_auto_commit=True
)

for message in consumer:
    data = message.value
    process_data(data)
```

**進階功能 - 事務**:
```python
from kafka import KafkaProducer

producer = KafkaProducer(
    bootstrap_servers=['localhost:9092'],
    transactional_id='ndh_producer_1'
)

producer.init_transactions()

try:
    producer.begin_transaction()
    
    # 發送多個訊息作為一個事務
    producer.send('topic1', b'message1')
    producer.send('topic2', b'message2')
    
    producer.commit_transaction()
except Exception as e:
    producer.abort_transaction()
```

#### Kafka 優勢

**高吞吐量**:
- 每秒可處理數百萬訊息
- 批次處理最佳化
- 零拷貝技術

**持久化和可靠性**:
- 所有訊息寫入磁碟
- 可配置的複製因子(通常 3)
- 保證資料不遺失

**可擴展性**:
- 水平擴展,增加 Broker
- Partition 實現平行處理
- 支援數千個 Consumer

**資料保留**:
- 可配置保留時間(天、週、月)
- 支援基於大小的保留
- 訊息可重複消費

**順序保證**:
- 同一 Partition 內訊息有序
- 支援 Key-based Partitioning

**生態系統**:
- Kafka Connect (連接器)
- Kafka Streams (流處理)
- KSQL (SQL 查詢)
- 豐富的第三方整合

#### Kafka 限制

**複雜性**:
- 安裝和配置複雜
- 需要 ZooKeeper (或 KRaft)
- 學習曲線陡峭

**資源需求**:
- 需要較多記憶體和磁碟
- 至少需要 3 個 Broker (生產環境)
- 維護成本較高

**延遲**:
- 相比 Redis 延遲較高(毫秒級 vs 微秒級)
- 批次處理導致的延遲
- 不適合極低延遲場景

**過度設計風險**:
- 小型應用可能不需要 Kafka 的複雜性
- 維護成本可能超過收益

### 詳細比較

#### 效能比較

| 指標 | Redis | Kafka |
|------|-------|-------|
| **延遲** | < 1ms (微秒級) | 2-10ms (毫秒級) |
| **吞吐量** | 100K ops/sec (單機) | 1M+ msgs/sec (叢集) |
| **記憶體使用** | 高 (記憶體為主) | 中 (頁面快取) |
| **磁碟使用** | 低 | 高 (持久化) |
| **CPU 使用** | 低 (單執行緒) | 中到高 |
| **網路頻寬** | 中 | 高 (複製) |

#### 可靠性比較

| 特性 | Redis | Kafka |
|------|-------|-------|
| **持久化** | RDB/AOF (可選) | 預設持久化 |
| **複製** | Master-Slave | Leader-Follower (多副本) |
| **資料遺失風險** | 中到高 | 低 |
| **故障恢復** | 較慢 | 快速 (自動) |
| **訊息確認** | 有限 (Streams) | 完整支援 |
| **事務支援** | 有限 | 完整支援 |

#### 可擴展性比較

| 特性 | Redis | Kafka |
|------|-------|-------|
| **水平擴展** | 困難 (Cluster) | 容易 (增加 Broker) |
| **垂直擴展** | 受記憶體限制 | 受磁碟限制 |
| **分區/分片** | 手動配置 | 自動管理 |
| **最大規模** | GB 級 | PB 級 |
| **Consumer 數量** | 有限 | 數千個 |

#### 功能比較

| 功能 | Redis | Kafka |
|------|-------|-------|
| **訊息保留** | 短期 | 長期 (可配置) |
| **訊息重播** | 有限 (Streams) | 完整支援 |
| **順序保證** | 有限 | 強保證 (Partition 內) |
| **消費者群組** | 有 (Streams) | 完整支援 |
| **訊息過濾** | 應用層 | 應用層 |
| **壓縮** | 無 | 支援 (gzip, snappy, lz4) |

### 選擇標準

#### 使用 Redis 的場景

**小型部署** (< 1000 個資產):
```
特徵:
- 資料量: < 10 GB/天
- 訊息速率: < 10,000 msgs/sec
- 保留時間: < 24 小時
- 消費者: < 10 個

優勢:
- 快速部署
- 低維護成本
- 簡單易用
```

**即時性要求極高**:
```
場景:
- 即時警報系統
- 高頻交易
- 即時儀表板更新

要求:
- 延遲 < 1ms
- 快速讀寫
- 簡單的訊息流
```

**多功能需求**:
```
場景:
- 同時需要快取和佇列
- Session 管理
- 計數器和排行榜

優勢:
- 一個系統多種用途
- 降低系統複雜度
```

**原型和開發**:
```
場景:
- 快速原型開發
- 概念驗證 (PoC)
- 開發和測試環境

優勢:
- 快速啟動
- 易於除錯
- 低學習成本
```

#### 使用 Kafka 的場景

**大型部署** (> 10,000 個資產):
```
特徵:
- 資料量: > 100 GB/天
- 訊息速率: > 100,000 msgs/sec
- 保留時間: 天到月
- 消費者: 數十到數百個

優勢:
- 高吞吐量
- 水平擴展
- 可靠性高
```

**資料持久化需求**:
```
場景:
- 歷史資料分析
- 審計和合規
- 資料重播需求

要求:
- 長期保留
- 資料不可遺失
- 支援時間旅行
```

**複雜的資料流**:
```
場景:
- 多個資料來源和目標
- 複雜的路由規則
- 資料轉換管道

優勢:
- Topic 隔離
- 平行處理
- 流處理整合
```

**企業級可靠性**:
```
場景:
- 關鍵業務系統
- 24/7 運作
- 高可用性要求

優勢:
- 自動故障轉移
- 資料複製
- 強一致性保證
```

**與現有 Kafka 生態系統整合**:
```
場景:
- 組織已使用 Kafka
- 需要與其他 Kafka 系統整合
- 使用 Kafka Connect/Streams

優勢:
- 統一的資料平台
- 共享基礎設施
- 降低整合成本
```

### 混合架構方案

在實際部署中,Redis 和 Kafka 可以共存,各自發揮優勢:

#### 架構 1: Redis 作為快取,Kafka 作為主佇列

```
資料流:
生產者 → Kafka (持久化佇列) → 消費者
                ↓
            Redis (快取最新值)
                ↓
            即時查詢 API
```

**實作範例**:
```python
# 生產者: 同時寫入 Kafka 和 Redis
def publish_sensor_data(asset_id, value, timestamp):
    # 寫入 Kafka (持久化)
    kafka_producer.send('sensor_data', {
        'asset_id': asset_id,
        'value': value,
        'timestamp': timestamp
    })
    
    # 更新 Redis 快取 (最新值)
    redis_client.hset(f'asset:{asset_id}', mapping={
        'value': value,
        'timestamp': timestamp
    })
    redis_client.expire(f'asset:{asset_id}', 3600)  # 1小時過期

# 即時查詢: 從 Redis 讀取
def get_current_value(asset_id):
    data = redis_client.hgetall(f'asset:{asset_id}')
    if data:
        return data
    else:
        # Redis 沒有,從 Kafka 或資料庫查詢
        return query_from_database(asset_id)

# 歷史查詢: 從 Kafka 或資料庫
def get_history(asset_id, start_time, end_time):
    return query_kafka_or_database(asset_id, start_time, end_time)
```

#### 架構 2: 分層訊息佇列

```
即時層 (Redis)
- 延遲 < 1ms
- 保留 1 小時
- 用於即時警報和儀表板

持久層 (Kafka)
- 延遲 < 10ms
- 保留 30 天
- 用於歷史分析和審計
```

**實作範例**:
```python
# 雙層寫入
def process_sensor_data(data):
    # 1. 寫入 Redis (即時層)
    redis_client.lpush('realtime_queue', json.dumps(data))
    redis_client.ltrim('realtime_queue', 0, 999)  # 保留最新1000條
    
    # 2. 寫入 Kafka (持久層)
    kafka_producer.send('sensor_data_persistent', data)
    
    # 3. 檢查即時警報
    if data['value'] > threshold:
        redis_client.publish('alerts', json.dumps({
            'type': 'threshold_exceeded',
            'asset_id': data['asset_id'],
            'value': data['value']
        }))

# 即時消費者 (從 Redis)
def realtime_consumer():
    pubsub = redis_client.pubsub()
    pubsub.subscribe('alerts')
    for message in pubsub.listen():
        handle_alert(message)

# 批次消費者 (從 Kafka)
def batch_consumer():
    consumer = KafkaConsumer('sensor_data_persistent')
    for message in consumer:
        store_to_database(message.value)
```

#### 架構 3: 按資料類型分離

```
高頻資料 (Redis)
- 感測器即時讀數
- 狀態變化事件
- 短期快取

低頻資料 (Kafka)
- 配置變更
- 警報記錄
- 操作日誌
```

### NDH 中的實作建議

#### 階段 1: MVP (最小可行產品)

**使用 Redis**:
```python
# ndh/core/message_queue.py
import redis
import json

class MessageQueue:
    def __init__(self):
        self.redis_client = redis.Redis(
            host='localhost',
            port=6379,
            decode_responses=True
        )
    
    async def enqueue(self, queue_name: str, message: dict):
        """加入訊息到佇列"""
        self.redis_client.lpush(
            queue_name,
            json.dumps(message)
        )
    
    async def dequeue(self, queue_name: str):
        """從佇列取出訊息"""
        message = self.redis_client.brpop(queue_name, timeout=1)
        if message:
            return json.loads(message[1])
        return None
    
    async def publish(self, channel: str, message: dict):
        """發布訊息"""
        self.redis_client.publish(channel, json.dumps(message))
    
    async def subscribe(self, channels: list, callback):
        """訂閱頻道"""
        pubsub = self.redis_client.pubsub()
        pubsub.subscribe(*channels)
        
        for message in pubsub.listen():
            if message['type'] == 'message':
                data = json.loads(message['data'])
                await callback(data)
```

**配置**:
```yaml
# config/message_queue.yml
message_queue:
  type: redis
  redis:
    host: localhost
    port: 6379
    db: 0
    password: null
    max_connections: 50
```

#### 階段 2: 擴展 (加入 Kafka 支援)

**抽象介面**:
```python
# ndh/core/message_queue_interface.py
from abc import ABC, abstractmethod

class MessageQueueInterface(ABC):
    @abstractmethod
    async def enqueue(self, queue_name: str, message: dict):
        pass
    
    @abstractmethod
    async def dequeue(self, queue_name: str):
        pass
    
    @abstractmethod
    async def publish(self, topic: str, message: dict):
        pass
    
    @abstractmethod
    async def subscribe(self, topics: list, callback):
        pass
```

**Redis 實作**:
```python
# ndh/core/redis_queue.py
class RedisMessageQueue(MessageQueueInterface):
    # ... (如上所示)
    pass
```

**Kafka 實作**:
```python
# ndh/core/kafka_queue.py
from kafka import KafkaProducer, KafkaConsumer
import json

class KafkaMessageQueue(MessageQueueInterface):
    def __init__(self, bootstrap_servers):
        self.producer = KafkaProducer(
            bootstrap_servers=bootstrap_servers,
            value_serializer=lambda v: json.dumps(v).encode('utf-8')
        )
        self.consumers = {}
    
    async def enqueue(self, queue_name: str, message: dict):
        """發送到 Kafka Topic"""
        self.producer.send(queue_name, message)
        self.producer.flush()
    
    async def dequeue(self, queue_name: str):
        """從 Kafka Topic 消費"""
        if queue_name not in self.consumers:
            self.consumers[queue_name] = KafkaConsumer(
                queue_name,
                bootstrap_servers=self.bootstrap_servers,
                value_deserializer=lambda m: json.loads(m.decode('utf-8')),
                auto_offset_reset='earliest'
            )
        
        consumer = self.consumers[queue_name]
        message = next(consumer, None)
        return message.value if message else None
    
    async def publish(self, topic: str, message: dict):
        """發布到 Kafka Topic"""
        await self.enqueue(topic, message)
    
    async def subscribe(self, topics: list, callback):
        """訂閱 Kafka Topics"""
        consumer = KafkaConsumer(
            *topics,
            bootstrap_servers=self.bootstrap_servers,
            value_deserializer=lambda m: json.loads(m.decode('utf-8'))
        )
        
        for message in consumer:
            await callback(message.value)
```

**工廠模式**:
```python
# ndh/core/message_queue_factory.py
def create_message_queue(config: dict) -> MessageQueueInterface:
    queue_type = config.get('type', 'redis')
    
    if queue_type == 'redis':
        return RedisMessageQueue(
            host=config['redis']['host'],
            port=config['redis']['port']
        )
    elif queue_type == 'kafka':
        return KafkaMessageQueue(
            bootstrap_servers=config['kafka']['bootstrap_servers']
        )
    else:
        raise ValueError(f"Unknown queue type: {queue_type}")
```

**配置支援兩種**:
```yaml
# config/message_queue.yml

# 選項 1: 使用 Redis
message_queue:
  type: redis
  redis:
    host: localhost
    port: 6379

# 選項 2: 使用 Kafka
message_queue:
  type: kafka
  kafka:
    bootstrap_servers:
      - localhost:9092
      - localhost:9093
      - localhost:9094
    
# 選項 3: 混合使用
message_queue:
  realtime:
    type: redis
    redis:
      host: localhost
      port: 6379
  persistent:
    type: kafka
    kafka:
      bootstrap_servers:
        - localhost:9092
```

### 決策樹

使用以下決策樹來選擇合適的訊息佇列:

```
開始
  │
  ├─ 資料量 < 10 GB/天?
  │   ├─ 是 → 訊息速率 < 10,000/sec?
  │   │   ├─ 是 → 需要長期保留 (> 7天)?
  │   │   │   ├─ 否 → 【使用 Redis】
  │   │   │   └─ 是 → 考慮混合架構
  │   │   └─ 否 → 【使用 Kafka】
  │   └─ 否 → 【使用 Kafka】
  │
  ├─ 延遲要求 < 1ms?
  │   ├─ 是 → 【使用 Redis】
  │   └─ 否 → 繼續評估
  │
  ├─ 需要資料重播?
  │   ├─ 是 → 【使用 Kafka】
  │   └─ 否 → 繼續評估
  │
  ├─ 消費者 > 50 個?
  │   ├─ 是 → 【使用 Kafka】
  │   └─ 否 → 【使用 Redis】
  │
  └─ 預算和維護能力?
      ├─ 有限 → 【使用 Redis】
      └─ 充足 → 【使用 Kafka】
```

### 總結和建議

#### 快速決策指南

| 場景 | 建議 | 理由 |
|------|------|------|
| **小型工廠 (< 1000 設備)** | Redis | 簡單、快速、成本低 |
| **中型工廠 (1000-10000 設備)** | Redis + Kafka | Redis 快取,Kafka 持久化 |
| **大型工廠 (> 10000 設備)** | Kafka | 可擴展性和可靠性 |
| **原型開發** | Redis | 快速啟動 |
| **生產環境 (關鍵業務)** | Kafka | 企業級可靠性 |
| **即時儀表板** | Redis | 超低延遲 |
| **歷史資料分析** | Kafka | 長期保留 |
| **多系統整合** | Kafka | 標準化資料平台 |

#### 實作路徑建議

**第一階段: 使用 Redis**
- 快速啟動 NDH 專案
- 驗證核心功能
- 累積使用經驗

**第二階段: 評估需求**
- 監控資料量和效能
- 評估可靠性需求
- 考慮擴展計畫

**第三階段: 選擇性升級**
- 如果需要,加入 Kafka
- 採用混合架構
- 平滑遷移

#### 關鍵建議

1. **從簡單開始**: 除非確定需要 Kafka 的功能,否則從 Redis 開始
2. **設計抽象層**: 使用介面抽象,方便未來切換或混合使用
3. **監控和測試**: 持續監控效能,根據實際需求調整
4. **考慮總成本**: 包括開發、部署、維護和學習成本
5. **預留擴展空間**: 架構設計要考慮未來擴展的可能性

---

## IADL Editor 與 NDH 整合

### IADL Editor 新增功能

#### 1. NDH 客戶端插件

**功能**:
- 連接到 NDH 服務
- 推送配置到 NDH
- 查詢 NDH 資料
- 監控 NDH 狀態

**實作**:
```cpp
// ndh_client.h
#ifndef NDHCLIENT_H
#define NDHCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class NDHClient : public QObject
{
    Q_OBJECT

public:
    explicit NDHClient(QObject *parent = nullptr);
    ~NDHClient();

    // 連接管理
    void setServerUrl(const QString &url);
    void setApiKey(const QString &apiKey);
    bool testConnection();

    // 配置管理
    bool uploadConfig(const QString &configName, const QJsonObject &config);
    QJsonObject downloadConfig(const QString &configId);
    QStringList listConfigs();
    bool deleteConfig(const QString &configId);

    // 資產管理
    QJsonArray listAssets();
    QJsonObject getAsset(const QString &assetId);
    bool createAsset(const QJsonObject &asset);
    bool updateAsset(const QString &assetId, const QJsonObject &asset);
    bool deleteAsset(const QString &assetId);

    // 資料查詢
    QJsonObject getCurrentData(const QString &assetId);
    QJsonArray getHistoryData(const QString &assetId, 
                               const QDateTime &startTime, 
                               const QDateTime &endTime);

    // 連接器管理
    QJsonArray listConnectors();
    QJsonObject getConnectorStatus(const QString &connectorId);
    bool startConnector(const QString &connectorId);
    bool stopConnector(const QString &connectorId);

    // 監控
    QJsonObject getHealthStatus();
    QJsonObject getMetrics();

signals:
    void connected();
    void disconnected();
    void error(const QString &errorMessage);
    void dataReceived(const QJsonObject &data);

private slots:
    void onReplyFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *m_networkManager;
    QString m_serverUrl;
    QString m_apiKey;
    
    QNetworkReply* sendRequest(const QString &endpoint, 
                                const QString &method, 
                                const QJsonObject &data = QJsonObject());
};

#endif // NDHCLIENT_H
```

#### 2. NDH 配置生成器

**功能**:
- 從 IADL 模型生成 NDH 配置
- 配置路由規則
- 配置轉換規則
- 配置連接器

**配置生成範例**:
```cpp
// ndh_config_generator.cpp
QJsonObject NDHConfigGenerator::generateConfig(const QList<AssetData> &assets)
{
    QJsonObject config;
    
    // 生成資產配置
    QJsonArray assetsArray;
    for (const AssetData &asset : assets) {
        QJsonObject assetObj;
        assetObj["id"] = asset.id;
        assetObj["name"] = asset.name;
        assetObj["type"] = asset.type;
        
        QJsonObject propsObj;
        for (auto it = asset.properties.begin(); it != asset.properties.end(); ++it) {
            propsObj[it.key()] = QJsonValue::fromVariant(it.value());
        }
        assetObj["properties"] = propsObj;
        
        assetsArray.append(assetObj);
    }
    config["assets"] = assetsArray;
    
    // 生成路由規則
    QJsonArray routesArray = generateRoutes(assets);
    config["routes"] = routesArray;
    
    // 生成轉換規則
    QJsonArray transformsArray = generateTransforms(assets);
    config["transforms"] = transformsArray;
    
    // 生成連接器配置
    QJsonArray connectorsArray = generateConnectors(assets);
    config["connectors"] = connectorsArray;
    
    return config;
}

QJsonArray NDHConfigGenerator::generateRoutes(const QList<AssetData> &assets)
{
    QJsonArray routes;
    
    // 根據資產類型和屬性自動生成路由規則
    // 例如: 所有溫度感測器的資料路由到歷史資料庫
    
    QJsonObject tempRoute;
    tempRoute["id"] = "temperature_route";
    tempRoute["name"] = "Temperature Sensors Route";
    
    QJsonObject source;
    source["connector"] = "scada_connector";
    QJsonObject filter;
    filter["assetType"] = "Sensor";
    filter["properties.sensorType"] = "Temperature";
    source["filter"] = filter;
    tempRoute["source"] = source;
    
    QJsonArray destinations;
    QJsonObject dest1;
    dest1["connector"] = "historian_connector";
    destinations.append(dest1);
    tempRoute["destinations"] = destinations;
    
    routes.append(tempRoute);
    
    return routes;
}
```

#### 3. NDH 監控面板

**功能**:
- 顯示 NDH 連接狀態
- 顯示連接器狀態
- 顯示資料流量統計
- 顯示錯誤和警告

**UI 設計**:
```cpp
// ndh_monitor_widget.h
class NDHMonitorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NDHMonitorWidget(NDHClient *client, QWidget *parent = nullptr);

private slots:
    void updateStatus();
    void onConnectorStatusChanged(const QString &connectorId, const QString &status);
    void onDataReceived(const QJsonObject &data);

private:
    void setupUI();
    
    NDHClient *m_client;
    QLabel *m_connectionStatusLabel;
    QTableWidget *m_connectorsTable;
    QTextEdit *m_logTextEdit;
    QTimer *m_updateTimer;
};
```

### 整合工作流程

#### 工作流程 1: 設計到部署

```
1. 設計階段 (IADL Editor)
   ├─ 建立資產模型
   ├─ 定義屬性和關係
   ├─ 設定資料來源和目標
   └─ 驗證模型

2. 配置生成 (IADL Editor)
   ├─ 自動生成 NDH 配置
   ├─ 配置路由規則
   ├─ 配置轉換規則
   └─ 配置連接器

3. 部署配置 (IADL Editor → NDH)
   ├─ 連接到 NDH 服務
   ├─ 上傳配置
   ├─ 驗證配置
   └─ 啟動連接器

4. 執行階段 (NDH)
   ├─ 連接到各系統
   ├─ 開始資料交換
   ├─ 執行路由和轉換
   └─ 記錄和監控

5. 監控和維護 (IADL Editor)
   ├─ 監控 NDH 狀態
   ├─ 查看資料流量
   ├─ 檢視日誌
   └─ 調整配置
```

#### 工作流程 2: 資料分析

```
1. 資料收集 (NDH)
   ├─ 從各系統收集資料
   ├─ 儲存到資料庫
   └─ 維護資料品質

2. 資料匯出 (NDH → IADL Editor)
   ├─ 查詢歷史資料
   ├─ 匯出為 IADL 格式
   └─ 傳輸到 IADL Editor

3. 資料分析 (IADL Editor)
   ├─ 載入資料
   ├─ 視覺化分析
   ├─ 產生報告
   └─ 識別問題

4. 配置最佳化 (IADL Editor)
   ├─ 根據分析結果調整配置
   ├─ 最佳化路由規則
   └─ 改進轉換邏輯

5. 更新部署 (IADL Editor → NDH)
   ├─ 上傳新配置
   ├─ 執行配置更新
   └─ 驗證效果
```

---

## 資料流程

### 配置資料流

```
IADL Editor                      NDH
    │                             │
    │  1. 建立 IADL 模型          │
    │─────────────────────────────│
    │                             │
    │  2. 生成 NDH 配置 (JSON)    │
    │─────────────────────────────│
    │                             │
    │  3. POST /api/v1/config     │
    │────────────────────────────>│
    │                             │
    │  4. 驗證配置                │
    │                             │
    │  5. 儲存配置                │
    │                             │
    │  6. 返回配置 ID             │
    │<────────────────────────────│
    │                             │
    │  7. 啟動連接器              │
    │  POST /api/v1/connectors/   │
    │       {id}/start            │
    │────────────────────────────>│
    │                             │
    │  8. 連接器啟動              │
    │                             │
    │  9. 返回狀態                │
    │<────────────────────────────│
```

### 執行時資料流

```
外部系統              NDH                  IADL Editor
    │                 │                        │
    │  1. 資料變化    │                        │
    │────────────────>│                        │
    │                 │                        │
    │                 │  2. 接收資料           │
    │                 │                        │
    │                 │  3. 路由處理           │
    │                 │                        │
    │                 │  4. 轉換處理           │
    │                 │                        │
    │                 │  5. 儲存到資料庫       │
    │                 │                        │
    │                 │  6. 推送到目標系統     │
    │                 │───────────────────────>│
    │                 │                        │
    │                 │  7. WebSocket 推送     │
    │                 │  (如果 IADL Editor     │
    │                 │   正在監控)            │
    │                 │───────────────────────>│
    │                 │                        │
    │                 │                        │  8. 顯示即時資料
```

### 查詢資料流

```
IADL Editor                      NDH
    │                             │
    │  1. 查詢歷史資料            │
    │  GET /api/v1/data/history   │
    │  ?asset_id=xxx              │
    │  &start=xxx&end=xxx         │
    │────────────────────────────>│
    │                             │
    │                             │  2. 查詢資料庫
    │                             │
    │                             │  3. 轉換為 IADL 格式
    │                             │
    │  4. 返回資料 (JSON)         │
    │<────────────────────────────│
    │                             │
    │  5. 解析和顯示              │
```

---

## API 設計

### REST API 規範

#### 基本規範

**Base URL**: `https://ndh.example.com/api/v1`

**認證**: Bearer Token (JWT)
```
Authorization: Bearer <token>
```

**請求格式**: JSON
```
Content-Type: application/json
```

**回應格式**: JSON
```json
{
  "success": true,
  "data": {...},
  "message": "Operation successful",
  "timestamp": "2024-10-09T10:30:00Z"
}
```

**錯誤回應**:
```json
{
  "success": false,
  "error": {
    "code": "INVALID_CONFIG",
    "message": "Configuration validation failed",
    "details": {...}
  },
  "timestamp": "2024-10-09T10:30:00Z"
}
```

#### API 端點詳細設計

##### 1. 配置管理 API

**上傳配置**
```
POST /api/v1/config
Content-Type: application/json

{
  "name": "Production Config",
  "description": "Production environment configuration",
  "config": {
    "assets": [...],
    "routes": [...],
    "transforms": [...],
    "connectors": [...]
  }
}

Response:
{
  "success": true,
  "data": {
    "config_id": "cfg_001",
    "version": 1,
    "created_at": "2024-10-09T10:30:00Z"
  }
}
```

**取得配置**
```
GET /api/v1/config/{config_id}

Response:
{
  "success": true,
  "data": {
    "id": "cfg_001",
    "name": "Production Config",
    "config": {...},
    "version": 1,
    "created_at": "2024-10-09T10:30:00Z",
    "updated_at": "2024-10-09T10:30:00Z"
  }
}
```

**列出所有配置**
```
GET /api/v1/config?page=1&limit=10

Response:
{
  "success": true,
  "data": {
    "configs": [
      {
        "id": "cfg_001",
        "name": "Production Config",
        "version": 1,
        "created_at": "2024-10-09T10:30:00Z"
      }
    ],
    "total": 1,
    "page": 1,
    "limit": 10
  }
}
```

##### 2. 資產管理 API

**列出資產**
```
GET /api/v1/assets?type=Sensor&page=1&limit=10

Response:
{
  "success": true,
  "data": {
    "assets": [
      {
        "id": "asset_001",
        "name": "Temperature Sensor 1",
        "type": "Sensor",
        "properties": {...},
        "created_at": "2024-10-09T10:30:00Z"
      }
    ],
    "total": 1,
    "page": 1,
    "limit": 10
  }
}
```

**取得資產詳情**
```
GET /api/v1/assets/{asset_id}

Response:
{
  "success": true,
  "data": {
    "id": "asset_001",
    "name": "Temperature Sensor 1",
    "type": "Sensor",
    "properties": {
      "sensorType": "Temperature",
      "unit": "Celsius",
      "range": {"min": -50, "max": 150}
    },
    "current_value": {
      "value": 25.5,
      "timestamp": "2024-10-09T10:30:00Z",
      "quality": "Good"
    },
    "created_at": "2024-10-09T10:00:00Z",
    "updated_at": "2024-10-09T10:30:00Z"
  }
}
```

##### 3. 資料查詢 API

**取得當前值**
```
GET /api/v1/data/current?asset_ids=asset_001,asset_002

Response:
{
  "success": true,
  "data": [
    {
      "asset_id": "asset_001",
      "value": 25.5,
      "timestamp": "2024-10-09T10:30:00Z",
      "quality": "Good"
    },
    {
      "asset_id": "asset_002",
      "value": 1250.0,
      "timestamp": "2024-10-09T10:30:00Z",
      "quality": "Good"
    }
  ]
}
```

**取得歷史資料**
```
GET /api/v1/data/history?asset_id=asset_001
    &start=2024-10-09T00:00:00Z
    &end=2024-10-09T23:59:59Z
    &interval=1h

Response:
{
  "success": true,
  "data": {
    "asset_id": "asset_001",
    "start": "2024-10-09T00:00:00Z",
    "end": "2024-10-09T23:59:59Z",
    "interval": "1h",
    "data_points": [
      {
        "timestamp": "2024-10-09T00:00:00Z",
        "value": 23.5,
        "quality": "Good"
      },
      {
        "timestamp": "2024-10-09T01:00:00Z",
        "value": 24.0,
        "quality": "Good"
      }
    ]
  }
}
```

##### 4. 連接器管理 API

**列出連接器**
```
GET /api/v1/connectors

Response:
{
  "success": true,
  "data": [
    {
      "id": "conn_001",
      "name": "SCADA Connector",
      "type": "opcua",
      "status": "running",
      "connected_assets": 150,
      "last_update": "2024-10-09T10:30:00Z"
    }
  ]
}
```

**取得連接器狀態**
```
GET /api/v1/connectors/{connector_id}/status

Response:
{
  "success": true,
  "data": {
    "id": "conn_001",
    "status": "running",
    "connected": true,
    "uptime": 86400,
    "messages_processed": 1000000,
    "errors": 5,
    "last_error": null
  }
}
```

##### 5. 監控 API

**健康檢查**
```
GET /api/v1/health

Response:
{
  "success": true,
  "data": {
    "status": "healthy",
    "version": "1.0.0",
    "uptime": 86400,
    "components": {
      "api": "healthy",
      "database": "healthy",
      "redis": "healthy",
      "connectors": "healthy"
    }
  }
}
```

**系統指標**
```
GET /api/v1/metrics

Response:
{
  "success": true,
  "data": {
    "messages_per_second": 1000,
    "active_connections": 50,
    "queue_length": 100,
    "cpu_usage": 45.5,
    "memory_usage": 2048,
    "disk_usage": 10240
  }
}
```

### WebSocket API

**連接**
```
ws://ndh.example.com/ws/realtime?token=<jwt_token>
```

**訂閱資產資料**
```json
{
  "action": "subscribe",
  "asset_ids": ["asset_001", "asset_002"]
}
```

**接收即時資料**
```json
{
  "type": "data_update",
  "asset_id": "asset_001",
  "value": 25.5,
  "timestamp": "2024-10-09T10:30:00Z",
  "quality": "Good"
}
```

**取消訂閱**
```json
{
  "action": "unsubscribe",
  "asset_ids": ["asset_001"]
}
```

---

## 實作計畫

### 第一階段: NDH 核心開發 (4-6 週)

**Week 1-2: 基礎架構**
- [ ] 建立專案結構
- [ ] 實作 API 層 (FastAPI)
- [ ] 實作資料庫層 (PostgreSQL + SQLAlchemy)
- [ ] 實作訊息佇列 (Redis)
- [ ] 基本的認證和授權

**Week 3-4: 核心引擎**
- [ ] 實作路由引擎
- [ ] 實作轉換引擎
- [ ] 實作事件處理器
- [ ] 實作配置管理

**Week 5-6: 連接器框架**
- [ ] 設計連接器基礎類別
- [ ] 實作 OPC UA 連接器
- [ ] 實作 Modbus 連接器
- [ ] 實作 REST API 連接器
- [ ] 連接器生命週期管理

### 第二階段: IADL Editor 整合 (2-3 週)

**Week 7-8: NDH 客戶端**
- [ ] 實作 NDH 客戶端類別
- [ ] 實作配置上傳功能
- [ ] 實作資料查詢功能
- [ ] 實作 WebSocket 連接

**Week 9: 配置生成器**
- [ ] 實作 NDH 配置生成器
- [ ] 實作路由規則生成
- [ ] 實作轉換規則生成
- [ ] 實作連接器配置生成

### 第三階段: 測試和文件 (2 週)

**Week 10: 測試**
- [ ] 單元測試
- [ ] 整合測試
- [ ] 效能測試
- [ ] 端到端測試

**Week 11: 文件和部署**
- [ ] API 文件
- [ ] 使用者手冊
- [ ] 開發者指南
- [ ] Docker 化
- [ ] 部署指南

### 第四階段: 進階功能 (3-4 週)

**Week 12-13: 監控和管理**
- [ ] 實作監控面板
- [ ] 實作日誌管理
- [ ] 實作警報系統
- [ ] 實作效能分析

**Week 14-15: 進階連接器**
- [ ] MQTT 連接器
- [ ] Kafka 連接器
- [ ] 資料庫連接器
- [ ] 檔案系統連接器

---

## 測試策略

### 單元測試

**NDH 單元測試**:
```python
# tests/test_routing_engine.py
import pytest
from core.routing_engine import RoutingEngine

def test_route_creation():
    engine = RoutingEngine()
    route = {
        "id": "test_route",
        "source": {"connector": "test_source"},
        "destinations": [{"connector": "test_dest"}]
    }
    engine.add_route(route)
    assert engine.get_route("test_route") is not None

def test_route_matching():
    engine = RoutingEngine()
    # 測試路由匹配邏輯
    pass

def test_route_execution():
    engine = RoutingEngine()
    # 測試路由執行
    pass
```

**IADL Editor 單元測試**:
```cpp
// tests/test_ndh_client.cpp
#include <QtTest/QtTest>
#include "ndh_client.h"

class TestNDHClient : public QObject
{
    Q_OBJECT

private slots:
    void testConnection();
    void testUploadConfig();
    void testQueryAssets();
};

void TestNDHClient::testConnection()
{
    NDHClient client;
    client.setServerUrl("http://localhost:8000");
    QVERIFY(client.testConnection());
}
```

### 整合測試

**API 整合測試**:
```python
# tests/test_api_integration.py
import pytest
from fastapi.testclient import TestClient
from api.main import app

client = TestClient(app)

def test_upload_and_retrieve_config():
    # 上傳配置
    config = {
        "name": "Test Config",
        "config": {"assets": []}
    }
    response = client.post("/api/v1/config", json=config)
    assert response.status_code == 200
    config_id = response.json()["data"]["config_id"]
    
    # 取得配置
    response = client.get(f"/api/v1/config/{config_id}")
    assert response.status_code == 200
    assert response.json()["data"]["name"] == "Test Config"
```

**端到端測試**:
```python
# tests/test_e2e.py
def test_full_workflow():
    # 1. 上傳配置
    # 2. 啟動連接器
    # 3. 模擬資料輸入
    # 4. 驗證資料輸出
    # 5. 查詢歷史資料
    pass
```

### 效能測試

**負載測試**:
```python
# tests/test_performance.py
import locust

class NDHUser(locust.HttpUser):
    @locust.task
    def query_assets(self):
        self.client.get("/api/v1/assets")
    
    @locust.task
    def query_current_data(self):
        self.client.get("/api/v1/data/current?asset_ids=asset_001")
```

### 測試環境

**Docker Compose 測試環境**:
```yaml
# docker-compose.test.yml
version: '3.8'

services:
  ndh:
    build: .
    environment:
      - DATABASE_URL=postgresql://test:test@postgres:5432/ndh_test
      - REDIS_URL=redis://redis:6379
    depends_on:
      - postgres
      - redis

  postgres:
    image: postgres:14
    environment:
      - POSTGRES_DB=ndh_test
      - POSTGRES_USER=test
      - POSTGRES_PASSWORD=test

  redis:
    image: redis:7

  test:
    build: .
    command: pytest
    depends_on:
      - ndh
```

---

## 部署架構

### 開發環境

```
┌─────────────────────────────────────────┐
│         開發者工作站                     │
│  ┌────────────────┐  ┌────────────────┐ │
│  │  IADL Editor   │  │      NDH       │ │
│  │  (本地執行)     │  │  (Docker)      │ │
│  └────────────────┘  └────────────────┘ │
│           │                  │           │
│           └──────────────────┘           │
│                localhost                 │
└─────────────────────────────────────────┘
```

### 測試環境

```
┌─────────────────────────────────────────┐
│           測試伺服器                     │
│  ┌────────────────────────────────────┐ │
│  │      Docker Compose                │ │
│  │  ┌──────┐  ┌──────┐  ┌──────┐    │ │
│  │  │ NDH  │  │ DB   │  │Redis │    │ │
│  │  └──────┘  └──────┘  └──────┘    │ │
│  └────────────────────────────────────┘ │
└─────────────────────────────────────────┘
         ↑
         │ (測試)
┌────────┴────────┐
│  IADL Editor    │
│  (測試客戶端)    │
└─────────────────┘
```

### 生產環境

```
┌─────────────────────────────────────────────────────────┐
│                    生產環境                              │
│                                                          │
│  ┌────────────────────────────────────────────────────┐ │
│  │              Load Balancer (Nginx)                 │ │
│  └────────────────┬───────────────────────────────────┘ │
│                   │                                      │
│       ┌───────────┼───────────┐                         │
│       │           │           │                          │
│  ┌────┴────┐ ┌────┴────┐ ┌────┴────┐                   │
│  │ NDH #1  │ │ NDH #2  │ │ NDH #3  │                   │
│  └────┬────┘ └────┬────┘ └────┬────┘                   │
│       │           │           │                          │
│       └───────────┼───────────┘                         │
│                   │                                      │
│       ┌───────────┼───────────┐                         │
│       │           │           │                          │
│  ┌────┴────┐ ┌────┴────┐ ┌────┴────┐                   │
│  │  DB     │ │  Redis  │ │  Kafka  │                   │
│  │(Primary)│ │(Cluster)│ │(Cluster)│                   │
│  └─────────┘ └─────────┘ └─────────┘                   │
│                                                          │
└─────────────────────────────────────────────────────────┘
         ↑
         │ (API/WebSocket)
┌────────┴────────┐
│  IADL Editor    │
│  (客戶端)        │
└─────────────────┘
```

### Docker 部署

**Dockerfile**:
```dockerfile
FROM python:3.11-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

EXPOSE 8000

CMD ["uvicorn", "api.main:app", "--host", "0.0.0.0", "--port", "8000"]
```

**docker-compose.yml**:
```yaml
version: '3.8'

services:
  ndh:
    build: .
    ports:
      - "8000:8000"
    environment:
      - DATABASE_URL=postgresql://ndh:password@postgres:5432/ndh
      - REDIS_URL=redis://redis:6379
    depends_on:
      - postgres
      - redis
    restart: unless-stopped

  postgres:
    image: postgres:14
    volumes:
      - postgres_data:/var/lib/postgresql/data
    environment:
      - POSTGRES_DB=ndh
      - POSTGRES_USER=ndh
      - POSTGRES_PASSWORD=password
    restart: unless-stopped

  redis:
    image: redis:7
    volumes:
      - redis_data:/data
    restart: unless-stopped

  nginx:
    image: nginx:alpine
    ports:
      - "80:80"
      - "443:443"
    volumes:
      - ./nginx.conf:/etc/nginx/nginx.conf
      - ./ssl:/etc/nginx/ssl
    depends_on:
      - ndh
    restart: unless-stopped

volumes:
  postgres_data:
  redis_data:
```

---

## 結論

本文件詳細描述了 IADL Editor 和 Neutral Data Hub (NDH) 兩個專案的整合架構。透過清晰的專案定位、標準化的 API 設計和完整的實作計畫,兩個專案可以有效地協同工作,為工業資產管理和資料整合提供完整的解決方案。

### 關鍵要點

1. **明確的專案定位**: IADL Editor 作為離線工具,NDH 作為執行時服務
2. **統一的資料模型**: IADL 作為兩者的共同語言
3. **標準化的整合**: 透過 REST API 和 WebSocket 進行通訊
4. **完整的工作流程**: 從設計到部署的端到端支援
5. **可擴展的架構**: 插件化設計,易於擴展

### 下一步行動

1. **NDH 專案啟動**: 建立獨立的 GitHub 儲存庫
2. **技術選型確認**: 確定 NDH 的技術棧
3. **API 規範定義**: 詳細定義所有 API 端點
4. **原型開發**: 開發 MVP 版本進行驗證
5. **IADL Editor 擴展**: 加入 NDH 客戶端功能

---

**文件版本**: 1.0  
**最後更新**: 2024-10-09  
**作者**: Manus AI Assistant  
**狀態**: 規劃階段  
**下次審查**: 待 NDH 專案啟動後

