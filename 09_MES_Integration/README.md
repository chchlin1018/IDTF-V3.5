

# IDTF-MES 整合技術規格書

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件旨在詳細描述 IDTF (Industrial Digital Twin Framework) 與 MES (Manufacturing Execution System) 的整合技術規格,涵蓋整合架構、實施方式、數據模型和預期結果。

### 1.1 整合目標

- **打通 Level 2 到 Level 3 的數據流**: 實現從設備層 (OT) 到製造營運管理層 (MOM) 的無縫數據整合。
- **豐富數位分身的內涵**: 將 MES 的生產工單、BOM、WIP、品質等資訊融入 IDTF 的數位分身模型。
- **實現設計與製造的閉環**: 將製造過程的數據反饋回設計端,優化產品和製程。
- **提供統一的數據訪問入口**: 透過 NDH API 統一訪問 OT 和 IT 數據。

### 1.2 ISA-95 框架下的定位

IDTF 與 MES 的整合是 ISA-95 模型中 Level 2 (控制層) 與 Level 3 (MOM 層) 之間的關鍵連接。

- **IDTF**: 主要涵蓋 Level 0-2,專注於資產的物理和行為模型。
- **MES**: 主要在 Level 3,專注於生產流程的執行和管理。

---

## 2. 整合架構

### 2.1 核心架構圖

```
┌─────────────────────────────────────────────────────────┐
│                  Level 4: ERP (企業資源規劃)                 │
└────────────────────┬────────────────────────────────────────┘
                     ↕ (工單、BOM)
┌─────────────────────────────────────────────────────────┐
│                  Level 3: MES (製造執行系統)                 │
│  - 生產排程 (Production Scheduling)                        │
│  - 工單管理 (Work Order Management)                        │
│  - 製品追蹤 (WIP Tracking)                                 │
│  - 品質管理 (Quality Management)                           │
│  - 設備管理 (Equipment Management)                         │
└────────────────────┬────────────────────────────────────────┘
                     ↕ (雙向整合)
┌─────────────────────────────────────────────────────────┐
│                  IDTF-MES 整合層 (本規格書重點)            │
│  ┌─────────────────┐   ┌─────────────────┐   ┌─────────────────┐ │
│  │ MES Connector   │ ↔ │  Message Queue  │ ↔ │  NDH API        │ │
│  └─────────────────┘   └─────────────────┘   └─────────────────┘ │
└────────────────────┬────────────────────────────────────────┘
                     ↕
┌─────────────────────────────────────────────────────────┐
│          Level 0-2: IDTF (工業數位分身框架)              │
│  - NDH (Neutral Data Hub)                                │
│  - Asset Servant (虛擬分身物件)                          │
│  - IADL/FDL (資產/工廠定義)                              │
│  - Omniverse (3D 視覺化)                                 │
└─────────────────────────────────────────────────────────┘
```

### 2.2 整合層組件說明

#### MES Connector
- **職責**: 負責與特定的 MES 系統進行通訊,處理數據格式的轉換。
- **實現**: 針對不同的 MES (如 Siemens Opcenter, SAP ME, Applied Materials APF) 開發不同的 Connector。
- **通訊方式**: 透過 MES 提供的 API (RESTful, SOAP, OPC UA) 或直接讀取資料庫。

#### Message Queue (訊息佇列)
- **職責**: 作為 IDTF 與 MES 之間的非同步通訊中介,實現系統解耦和削峰填谷。
- **技術選型**: RabbitMQ, Kafka, or Azure Service Bus / AWS SQS。
- **優點**: 提高系統的可靠性和擴展性,即使一方系統暫時離線,數據也不會遺失。

#### NDH API
- **職責**: 提供統一的 API 接口,供 MES Connector 查詢 IDTF 中的數據,或將 MES 的數據寫入 IDTF。
- **接口**: 提供 RESTful API 和 WebSocket 接口。

---



## 3. 技術規格

### 3.1 數據交換模型 (基於 ISA-95)

| 數據類別 | 從 MES 到 IDTF | 從 IDTF 到 MES |
|---|---|---|
| **生產工單 (Production Order)** | `MES → IDTF` | `IDTF → MES` (狀態更新) |
| **物料清單 (BOM)** | `MES → IDTF` | - |
| **在製品 (WIP) 狀態** | `MES → IDTF` | `IDTF → MES` (設備狀態觸發) |
| **設備狀態 (Equipment Status)** | `IDTF → MES` | - |
| **製程參數 (Process Parameters)** | `IDTF → MES` | - |
| **品質數據 (Quality Data)** | `MES → IDTF` (SPC) | `IDTF → MES` (量測數據) |
| **能源消耗 (Energy Consumption)** | `IDTF → MES` | - |

### 3.2 API 規格 (NDH API)

#### 將 MES 數據寫入 IDTF

- **Endpoint**: `POST /api/v1/mes/events`
- **Request Body**:

```json
{
  "eventType": "WorkOrderStarted",
  "timestamp": "2025-10-14T10:30:00Z",
  "payload": {
    "workOrderId": "WO-12345",
    "productId": "PROD-XYZ",
    "quantity": 1000,
    "equipmentId": "EQ-001"
  }
}
```

#### 從 IDTF 查詢數據 (供 MES 使用)

- **Endpoint**: `GET /api/v1/assets/{assetId}/telemetry?tag=temperature`
- **Response**:

```json
{
  "assetId": "EQ-001",
  "tag": "temperature",
  "values": [
    { "timestamp": "2025-10-14T10:30:00Z", "value": 25.5 },
    { "timestamp": "2025-10-14T10:31:00Z", "value": 25.6 }
  ]
}
```

### 3.3 訊息佇列主題 (Topics)

- `mes.workorder.events`: 工單相關事件 (Created, Started, Completed)
- `mes.quality.events`: 品質相關事件 (SPC Violation, Measurement Taken)
- `idtf.equipment.status`: 設備狀態變更事件 (Running, Stopped, Alarm)
- `idtf.process.parameters`: 製程參數更新事件

### 3.4 IADL 擴展

為了儲存 MES 相關資訊,IADL 需要擴展,增加 `production` 區塊:

```yaml
Equipment Die_Bonder-01 {
    // ... 原有 IADL 定義 ...
    
    // 新增 production 區塊
    production: {
        current_work_order: "WO-12345",
        current_product_id: "PROD-XYZ",
        wip_status: "Processing",
        throughput: 1000 // units/hour
    }
}
```

---



## 4. 實施方式

### 4.1 階段一:單向數據流 (IDTF → MES)

1. **開發 IDTF Provider**: 在 MES 中開發一個服務,定期透過 NDH API 獲取設備狀態和製程參數。
2. **目標**: 讓 MES 能夠即時了解設備的實際運行情況。
3. **時間**: 1-2 個月

### 4.2 階段二:雙向數據流 (非同步)

1. **部署 Message Queue**: 建立 RabbitMQ 或 Kafka 服務。
2. **開發 MES Connector**: 訂閱 `idtf.equipment.status` 主題,並將工單、品質等事件發布到 `mes.*` 主題。
3. **擴展 Asset Servant**: 讓 Asset Servant 能夠訂閱 `mes.*` 主題,並更新 IADL 中的 `production` 區塊。
4. **目標**: 實現 IDTF 與 MES 之間的雙向、非同步數據交換。
5. **時間**: 2-3 個月

### 4.3 階段三:數位分身與 MES 深度整合

1. **開發數位分身應用**: 在 Omniverse 中開發應用,將 MES 的工單、WIP 等資訊與 3D 模型進行視覺化疊加。
2. **實現控制閉環**: 允許在數位分身應用中對 MES 的工單進行操作 (如暫停、繼續)。
3. **目標**: 建立一個整合了 OT 和 IT 數據的、可互動的數位分身。
4. **時間**: 3-4 個月

---

## 5. 預期結果

### 5.1 功能性結果

- **統一的監控平台**: 在 Omniverse 中可以同時看到設備的 3D 模型、即時的 OT 數據和來自 MES 的生產資訊。
- **即時的生產追蹤**: 工單的進度可以直接在 3D 數位分身中以視覺化的方式呈現。
- **快速的根因分析**: 當發生品質問題時,可以快速回溯該批次產品所經過的設備、當時的製程參數和設備狀態。
- **預測性維護**: 結合設備的運行時數 (來自 IDTF) 和生產排程 (來自 MES),可以更精準地預測維護需求。

### 5.2 業務價值

- **提高生產效率 (OEE)**: 透過即時監控和快速反應,減少設備停機時間。
- **改善產品質量**: 透過 OT 與 IT 數據的關聯分析,找到影響品質的關鍵因素。
- **降低營運成本**: 透過能源消耗和生產數據的分析,優化能源使用效率。
- **加速決策制定**: 為管理者提供一個全面、即時的工廠視圖,幫助他們做出更明智的決策。

---

