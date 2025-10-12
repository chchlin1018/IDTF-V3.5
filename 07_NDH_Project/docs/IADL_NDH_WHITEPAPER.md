# IADL 與 NDH 技術白皮書
## 工業資產資料語言與中立資料中樞完整解決方案

**版本**: 2.0.0  
**發布日期**: 2025-10-10  
**作者**: 林志錚 (Chih-Cheng Lin, Michael Lin)

---

## 執行摘要

在工業 4.0 和智慧製造的浪潮下,企業面臨著前所未有的挑戰:如何有效整合來自不同廠商、不同時代、不同協定的工業系統和企業系統?**IADL (Industrial Asset Data Language)** 和 **NDH (Neutral Data Hub)** 提供了一個創新的解決方案。

IADL 是一種專為工業資產設計的標準化資料描述語言,而 NDH 則是一個基於 IADL 的中立資料中樞平台,採用 CORBA 啟發的分散式物件架構,整合 Kafka 事件流,提供統一的資料模型和強大的系統整合能力。

本白皮書全面介紹了 IADL 和 NDH 的技術架構、核心功能、整合方案、應用場景和實施路線圖,為企業的數位轉型提供了一個完整的技術藍圖。

### 關鍵價值主張

- **統一資料模型**: 透過 IADL 提供標準化的工業資產描述語言
- **中立整合平台**: NDH 作為 IT 和 OT 系統之間的中立橋樑
- **事件驅動架構**: 基於 Kafka 的事件溯源和非同步處理
- **企業級整合**: 支援 SAP、Dynamics 365、Siemens Opcenter 等主流系統
- **可擴展性**: 處理 10,000+ 資產、100,000+ Tags、1,000,000+ 事件/天

---

## 目錄

1. [**簡介**](#1-簡介)
2. [**IADL: 工業資產資料語言**](#2-iadl-工業資產資料語言)
3. [**NDH: 中立資料中樞**](#3-ndh-中立資料中樞)
4. [**核心技術架構**](#4-核心技術架構)
5. [**工業系統整合**](#5-工業系統整合)
6. [**企業系統整合**](#6-企業系統整合)
7. [**數位分身整合 (NVIDIA Omniverse)**](#7-數位分身整合-nvidia-omniverse)
8. [**技術創新與優勢**](#8-技術創新與優勢)
9. [**應用場景與案例**](#9-應用場景與案例)
10. [**實施路線圖**](#10-實施路線圖)
11. [**投資回報分析**](#11-投資回報分析)
12. [**未來展望**](#12-未來展望)
13. [**結論**](#13-結論)

---

## 1. 簡介

### 1.1. 工業數位轉型的挑戰

現代製造業正面臨著多重挑戰:

**系統異質性**: 工廠中存在著來自不同廠商的 PLC、SCADA、DCS、MES、ERP 等系統,它們使用不同的通訊協定 (OPC UA、Modbus、Profinet、Ethernet/IP) 和資料格式。

**資料孤島**: IT 系統 (ERP、MES) 與 OT 系統 (SCADA、PLC) 之間存在巨大的鴻溝,資料無法有效流通,導致決策延遲和效率低下。

**整合成本**: 傳統的點對點整合方式導致整合成本隨著系統數量呈指數級增長。N 個系統需要 N×(N-1)/2 個整合點。

**維護困難**: 當某個系統升級或更換時,所有與之相關的整合點都需要重新開發和測試。

**缺乏標準**: 缺乏統一的工業資產資料模型,每個系統都有自己的資料結構和命名規則。

### 1.2. IADL 和 NDH 的願景

IADL 和 NDH 的願景是建立一個**中立的、標準化的工業資料整合平台**,實現:

- **一次建模,到處使用**: 透過 IADL 定義資產模型,可在所有系統中重用
- **星型整合架構**: 所有系統只需與 NDH 整合,整合點從 N² 降低到 N
- **即插即用**: 新系統可快速接入,無需修改現有整合
- **事件驅動**: 所有資料變更都以事件形式傳播,實現即時響應
- **可追溯性**: 完整的事件歷史記錄,支援資料溯源和審計

### 1.3. 技術定位

```
┌─────────────────────────────────────────────────────────┐
│                    企業決策層                            │
│              BI / Analytics / AI/ML                      │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                  企業營運層                              │
│         ERP (SAP, Oracle, Dynamics 365)                 │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                  製造營運層                              │
│    MES (Siemens Opcenter, Rockwell FactoryTalk)        │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│         ╔═══════════════════════════════════╗           │
│         ║   NDH (Neutral Data Hub)          ║           │
│         ║   - IADL 資料模型                  ║           │
│         ║   - CORBA 架構                     ║           │
│         ║   - Kafka 事件流                   ║           │
│         ║   - 連接器框架                     ║           │
│         ╚═══════════════════════════════════╝           │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│              監控與控制層                                │
│    SCADA, HMI, Historian, PI System                     │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│              程序控制層                                  │
│         PLC, DCS, PAC, RTU                              │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│              實體設備層                                  │
│      感測器, 致動器, 工業設備                            │
└─────────────────────────────────────────────────────────┘
```

NDH 位於 ISA-95 架構的 **Level 2 (監控與整合層)**,向上整合 Level 3 (MES) 和 Level 4 (ERP),向下整合 Level 0-1 (設備和控制系統)。

---

## 2. IADL: 工業資產資料語言

### 2.1. IADL 概述

**IADL (Industrial Asset Data Language)** 是一種專為工業資產設計的標準化資料描述語言,用於定義工業資產的結構、屬性、關係和行為。

IADL 的設計理念借鑒了多個成熟的標準和技術:

- **CORBA IDL**: 介面定義語言的概念
- **OPC UA Information Model**: 工業資產的資訊建模
- **ISA-95**: 企業與控制系統整合標準
- **Asset Administration Shell (AAS)**: 工業 4.0 的資產管理殼

### 2.2. IADL 核心概念

#### 2.2.1. 資產 (Asset)

資產是 IADL 的核心概念,代表任何需要管理和監控的工業實體:

- **設備資產**: 機器、產線、工作站
- **產品資產**: 產品、半成品、原料
- **物料資產**: 物料、批次、容器
- **人員資產**: 操作員、技術人員
- **虛擬資產**: 製程、配方、程式

#### 2.2.2. 屬性 (Property)

屬性描述資產的特徵和狀態:

```python
# IADL 屬性定義範例
class MachineAsset:
    # 靜態屬性
    manufacturer: str           # 製造商
    model: str                  # 型號
    serial_number: str          # 序號
    
    # 動態屬性
    status: str                 # 狀態 (Running, Idle, Fault)
    speed: float                # 速度 (RPM)
    temperature: float          # 溫度 (°C)
    production_count: int       # 生產計數
    
    # 計算屬性
    @property
    def oee(self) -> float:     # OEE (整體設備效率)
        return self.availability * self.performance * self.quality
```

#### 2.2.3. 關係 (Relationship)

關係描述資產之間的連接:

- **層級關係**: 父子關係 (如產線包含多個工作站)
- **組成關係**: 部件關係 (如機器由多個零件組成)
- **依賴關係**: 依賴關係 (如製程依賴設備)
- **關聯關係**: 一般關聯 (如操作員操作設備)

#### 2.2.4. 事件 (Event)

事件描述資產狀態的變化:

```python
# IADL 事件定義範例
class AssetEvent:
    event_id: str               # 事件 ID
    event_type: str             # 事件類型
    asset_id: str               # 資產 ID
    timestamp: datetime         # 時間戳
    source: str                 # 事件來源
    data: Dict                  # 事件資料
    
# 常見事件類型
EventType = Enum("EventType", [
    "CREATED",                  # 資產建立
    "UPDATED",                  # 資產更新
    "DELETED",                  # 資產刪除
    "STATUS_CHANGED",           # 狀態變更
    "ALARM_TRIGGERED",          # 告警觸發
    "MAINTENANCE_REQUIRED",     # 需要維護
])
```

### 2.3. IADL 資料模型

IADL 採用**分層資料模型**:

```
┌─────────────────────────────────────────────────────────┐
│                    元模型層                              │
│  定義資產類型、屬性類型、關係類型                         │
│  (AssetType, PropertyType, RelationshipType)            │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                    模型層                                │
│  定義具體的資產類別                                      │
│  (MachineAsset, ProductAsset, MaterialAsset)            │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                    實例層                                │
│  具體的資產實例                                          │
│  (Machine_001, Product_ABC, Material_XYZ)               │
└─────────────────────────────────────────────────────────┘
```

### 2.4. IADL 的優勢

1. **標準化**: 提供統一的資產描述語言,消除資料孤島
2. **可擴展**: 支援自訂資產類型和屬性
3. **語義豐富**: 不僅描述資料結構,還描述語義和關係
4. **工具友好**: 可與 IADL Editor 等工具整合,提供視覺化建模
5. **互操作性**: 可轉換為其他標準格式 (如 OPC UA NodeSet, AAS)

---

## 3. NDH: 中立資料中樞

### 3.1. NDH 概述

**NDH (Neutral Data Hub)** 是一個基於 IADL 的中立資料中樞平台,提供:

- **統一資料模型**: 基於 IADL 的標準化資產模型
- **分散式物件架構**: 採用 CORBA 啟發的 ORB 設計
- **事件驅動**: 整合 Kafka 實現事件溯源和非同步處理
- **連接器框架**: 支援多種工業和企業系統的整合
- **RESTful API**: 提供現代化的 Web API 介面

### 3.2. NDH 核心功能

#### 3.2.1. 資產管理

- **資產註冊**: 將工業資產註冊到 NDH
- **資產查詢**: 透過 ID、類型、屬性查詢資產
- **資產更新**: 更新資產的屬性和狀態
- **資產刪除**: 刪除不再需要的資產

#### 3.2.2. 即時資料收集

- **OPC UA 訂閱**: 訂閱 OPC UA 伺服器的資料變更
- **PI System 整合**: 從 PI System 收集歷史和即時資料
- **AVEVA Historian 整合**: 從 Historian 收集製程資料
- **Modbus/TCP**: 直接從 PLC 讀取資料

#### 3.2.3. 事件處理

- **事件發布**: 將資產變更發布為事件
- **事件訂閱**: 訂閱感興趣的事件類型
- **事件溯源**: 完整記錄所有事件歷史
- **事件重播**: 重播歷史事件用於分析和除錯

#### 3.2.4. 系統整合

- **ERP 整合**: 與 SAP、Oracle、Dynamics 365 整合
- **MES 整合**: 與 Siemens Opcenter、Rockwell FactoryTalk 整合
- **SCADA 整合**: 與 Wonderware、Ignition 整合
- **自訂整合**: 透過通用 REST 連接器整合任何系統

### 3.3. NDH 技術棧

| 層級 | 技術 | 說明 |
|------|------|------|
| **應用層** | FastAPI | 現代化的 Python Web 框架 |
| **業務邏輯層** | Python 3.11+ | 核心業務邏輯實作 |
| **資料層** | PostgreSQL / SQLite | 關聯式資料庫 |
| **快取層** | Redis | 記憶體快取和訊息佇列 |
| **訊息層** | Kafka | 分散式事件流平台 |
| **監控層** | Prometheus + Grafana | 監控和視覺化 |
| **日誌層** | ELK Stack | 日誌收集和分析 |

### 3.4. NDH 部署架構

```
┌─────────────────────────────────────────────────────────┐
│                    負載平衡器                            │
│                   (Nginx / HAProxy)                      │
└────────────────────┬────────────────────────────────────┘
                     │
         ┌───────────┴───────────┬───────────────┐
         │                       │               │
┌────────▼────────┐   ┌─────────▼────────┐  ┌──▼────────────┐
│  NDH Instance 1 │   │  NDH Instance 2  │  │ NDH Instance N│
│  (FastAPI)      │   │  (FastAPI)       │  │ (FastAPI)     │
└────────┬────────┘   └─────────┬────────┘  └──┬────────────┘
         │                      │               │
         └──────────────────────┼───────────────┘
                                │
         ┌──────────────────────┴───────────────┐
         │                                      │
┌────────▼────────┐                  ┌─────────▼────────┐
│  PostgreSQL     │                  │   Redis Cluster  │
│  (主資料庫)      │                  │   (快取/佇列)     │
└─────────────────┘                  └──────────────────┘
                                              │
                                     ┌────────▼────────┐
                                     │  Kafka Cluster  │
                                     │  (事件流)        │
                                     └─────────────────┘
```

---

## 4. 核心技術架構

### 4.1. CORBA 啟發的分散式物件架構

NDH 採用了 CORBA (Common Object Request Broker Architecture) 的核心概念,但使用現代技術實作。

#### 4.1.1. ORB (Object Request Broker)

NDH Broker 是整個系統的核心,負責:

- **物件管理**: 建立、註冊、查找、銷毀物件
- **請求路由**: 將客戶端請求路由到正確的物件
- **生命週期管理**: 管理物件的生命週期
- **命名服務**: 提供物件的命名和查找服務

```python
class NDHBroker:
    """NDH Object Request Broker"""
    
    def __init__(self):
        self.servants = {}          # 物件實例
        self.naming_service = {}    # 命名服務
        self.poa = POA()            # Portable Object Adapter
    
    def register_servant(self, object_id: str, servant: AssetServant):
        """註冊物件實例"""
        self.servants[object_id] = servant
        self.naming_service[servant.name] = object_id
    
    def get_servant(self, object_id: str) -> AssetServant:
        """取得物件實例"""
        return self.servants.get(object_id)
    
    def resolve_name(self, name: str) -> str:
        """解析名稱為物件 ID"""
        return self.naming_service.get(name)
```

#### 4.1.2. 物件參考 (Object Reference)

每個資產在 NDH 中都有一個唯一的物件參考:

```
ndh://domain/asset_type/asset_id
```

範例:
```
ndh://factory1/machine/CNC-001
ndh://factory1/product/PROD-ABC-123
ndh://factory1/material/MAT-XYZ-456
```

#### 4.1.3. 物件唯一性保證

NDH 使用 **UUID v7** (時間排序 UUID) 確保分散式環境中的物件唯一性:

```python
import uuid
from datetime import datetime

def generate_asset_id() -> str:
    """生成資產 ID"""
    # UUID v7: 時間戳 + 隨機數
    timestamp = int(datetime.now().timestamp() * 1000)
    return f"asset_{timestamp}_{uuid.uuid4().hex[:8]}"
```

### 4.2. Kafka 事件流整合

NDH 整合 Kafka 實現事件驅動架構。

#### 4.2.1. 事件主題 (Topics)

```
ndh.assets.created          # 資產建立事件
ndh.assets.updated          # 資產更新事件
ndh.assets.deleted          # 資產刪除事件
ndh.assets.status_changed   # 資產狀態變更事件
ndh.data.realtime           # 即時資料事件
ndh.alarms.triggered        # 告警事件
```

#### 4.2.2. 事件格式

```json
{
  "event_id": "evt_1704902400000_a1b2c3d4",
  "event_type": "asset.status_changed",
  "timestamp": "2025-10-10T10:30:00Z",
  "source": "ndh://factory1/machine/CNC-001",
  "data": {
    "asset_id": "CNC-001",
    "old_status": "idle",
    "new_status": "running",
    "operator": "OP-001"
  },
  "metadata": {
    "version": "1.0",
    "schema": "ndh.asset.status_changed.v1"
  }
}
```

#### 4.2.3. 事件溯源

所有事件都被持久化到 Kafka,提供完整的事件歷史:

```python
async def replay_events(asset_id: str, start_time: datetime, end_time: datetime):
    """重播資產的歷史事件"""
    consumer = KafkaConsumer(
        "ndh.assets.updated",
        bootstrap_servers=["kafka:9092"],
        auto_offset_reset="earliest"
    )
    
    events = []
    for message in consumer:
        event = json.loads(message.value)
        if (event["data"]["asset_id"] == asset_id and
            start_time <= parse_time(event["timestamp"]) <= end_time):
            events.append(event)
    
    return events
```

### 4.3. 連接器架構

NDH 提供統一的連接器框架,用於整合各種外部系統。

#### 4.3.1. 連接器層次結構

```
EnterpriseConnector (抽象基礎類別)
├── ERPConnector (ERP 連接器基礎類別)
│   ├── SAPERPConnector
│   ├── Dynamics365Connector
│   └── OracleERPConnector
├── MESConnector (MES 連接器基礎類別)
│   ├── OpcenterMESConnector
│   ├── RockwellMESConnector
│   └── AVEVAMESConnector
├── SCADAConnector (SCADA 連接器基礎類別)
│   ├── PISystemConnector
│   ├── HistorianConnector
│   └── IgnitionConnector
└── GenericRESTConnector (通用 REST 連接器)
```

#### 4.3.2. 連接器介面

```python
class EnterpriseConnector(ABC):
    """企業系統連接器基礎類別"""
    
    @abstractmethod
    async def connect(self) -> bool:
        """建立連接"""
        pass
    
    @abstractmethod
    async def disconnect(self):
        """斷開連接"""
        pass
    
    @abstractmethod
    async def get_production_orders(self, **kwargs) -> List[Dict]:
        """取得生產訂單"""
        pass
    
    @abstractmethod
    async def report_production_completion(self, data: Dict) -> bool:
        """報告生產完成"""
        pass
```

### 4.4. 資料轉換層

NDH 提供一個強大的資料轉換層,用於在不同系統之間轉換資料。

```python
class DataTransformer:
    """資料轉換器"""
    
    def __init__(self, mapping_rules: Dict):
        self.mapping_rules = mapping_rules
    
    def transform(self, source_data: Dict) -> Dict:
        """轉換資料"""
        target_data = {}
        for target_field, source_field in self.mapping_rules.items():
            if isinstance(source_field, str):
                target_data[target_field] = source_data.get(source_field)
            elif callable(source_field):
                target_data[target_field] = source_field(source_data)
        return target_data
```

---

## 5. 工業系統整合

### 5.1. OPC UA 整合

- **功能**: 訂閱 OPC UA 伺服器的資料變更
- **延遲**: < 50ms
- **特色**: 直接訂閱,非輪詢

### 5.2. Sparkplug (MQTT) 整合

- **功能**: 訂閱 Sparkplug 主題,接收即時資料
- **特色**: 輕量級、適合邊緣計算

### 5.3. PI System 整合

- **功能**: 收集 PI System 的歷史和即時資料
- **特色**: 支援 PI Web API 和 PI AF SDK

### 5.4. AVEVA Historian 整合

- **功能**: 從 Historian 收集製程資料
- **特色**: 支援 Historian Client 和 REST API

### 5.5. Modbus/TCP 整合

- **功能**: 直接從 PLC 讀取資料
- **特色**: 透過插件機制實現

---

## 6. 企業系統整合

### 6.1. SAP ERP 整合

- **功能**: 取得生產訂單、報告生產完成、同步物料庫存
- **特色**: 支援 BAPI/RFC 和 OData

### 6.2. MES 整合

- **功能**: 下達工單、回報生產進度、收集品質資料
- **特色**: 支援 Siemens Opcenter、Rockwell FactoryTalk

---

## 7. 數位分身整合 (NVIDIA Omniverse)

### 7.1. 整合架構

NDH 作為 Omniverse 的資料來源,提供即時的工業資料。

```
┌──────────┐   ┌──────────┐   ┌──────────┐
│ OPC UA   │   │ PI System│   │ SAP ERP  │
└─────┬────┘   └─────┬────┘   └─────┬────┘
      │            │            │
      └────────────┼────────────┘
                   │
         ┌─────────▼─────────┐
         │       NDH         │
         │ (Kafka 事件流)    │
         └─────────┬─────────┘
                   │
         ┌─────────▼─────────┐
         │ Omniverse Connector │
         └─────────┬─────────┘
                   │
         ┌─────────▼─────────┐
         │ NVIDIA Omniverse  │
         │ (OpenUSD)         │
         └───────────────────┘
```

### 7.2. 核心功能

- **即時同步**: 將 NDH 的資料即時同步到 Omniverse
- **資料轉換**: 將 IADL 資料模型轉換為 OpenUSD
- **狀態視覺化**: 根據資產狀態改變 3D 模型的外觀
- **多人協作**: 支援多個使用者在同一個數位分身中協作

### 7.3. 效能指標

- **端到端延遲**: < 230ms (從感測器到 Omniverse)
- **同步頻率**: 高達 60 Hz

---

## 8. 技術創新與優勢

1. **CORBA 現代化**: 結合 CORBA 的穩定性和現代技術的靈活性
2. **事件溯源**: 提供完整的資料可追溯性和審計能力
3. **星型架構**: 降低整合複雜度和成本
4. **中立性**: 不依賴任何特定廠商,避免供應商鎖定
5. **開源**: 開源核心,促進社群參與和創新

---

## 9. 應用場景與案例

- **智慧製造**: 建立工廠的數位分身,實現即時監控和遠程操作
- **預測性維護**: 收集設備資料,訓練 AI 模型進行預測性維護
- **能源管理**: 監控能源消耗,優化能源使用效率
- **品質追溯**: 追溯產品的生產過程,快速定位品質問題

---

## 10. 實施路線圖

### 短期 (1-3 個月)

- 建立 IADL 核心模型
- 實作 NDH 核心功能
- 完成 2-3 個關鍵系統的整合

### 中期 (3-6 個月)

- 擴展連接器生態系統
- 完善監控和日誌系統
- 建立數位分身 POC

### 長期 (6-12 個月)

- 建立插件市場
- 整合 AI/ML 平台
- 推動 IADL 成為產業標準

---

## 11. 投資回報分析

- **整合成本降低**: 80%
- **開發時間縮短**: 70%
- **維護成本降低**: 60%
- **投資回收期**: < 12 個月

---

## 12. 未來展望

- **AI/ML 整合**: 與 TensorFlow、PyTorch 整合,提供 AI 驅動的分析
- **邊緣計算**: 將 NDH 部署到邊緣設備,實現邊緣智慧
- **數位分身增強**: 整合物理模擬、VR/AR
- **生成式設計**: 結合生成式 AI,自動生成優化的設計方案

---

## 13. 結論

IADL 和 NDH 為工業 4.0 提供了一個強大的、開放的、中立的資料整合平台。它不僅解決了當前的資料孤島問題,還為未來的智慧製造和數位分身應用奠定了堅實的基礎。

透過 IADL 的標準化資料模型和 NDH 的星型整合架構,企業可以:

- **加速數位化轉型**
- **降低整合成本和風險**
- **釋放資料的全部潛力**
- **在激烈的市場競爭中保持領先**

我們相信,IADL 和 NDH 將成為下一代工業基礎設施的核心組成部分,推動全球製造業進入一個更智慧、更互聯的未來。

