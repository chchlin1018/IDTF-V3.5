

# NDH-ERP 整合技術規格書

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件旨在詳細描述 NDH (Neutral Data Hub) 與 ERP (Enterprise Resource Planning) 系統的整合技術規格,涵蓋整合架構、實施方式、數據模型和預期結果。

### 1.1 整合目標

- **打通 Level 3 到 Level 4 的數據流**: 實現從製造營運管理層 (MOM) 到企業資源規劃層的無縫數據整合。
- **實現業務與製造的閉環**: 將生產數據 (如產量、物料消耗、設備效率) 反饋回 ERP,優化企業營運決策。
- **自動化數據採集**: 自動將 NDH 中的設備運行數據轉換為 ERP 所需的業務數據。

### 1.2 ISA-95 框架下的定位

NDH 與 ERP 的整合是 ISA-95 模型中 Level 3 (MOM/MES) 與 Level 4 (ERP) 之間的關鍵連接。

- **NDH**: 主要涵蓋 Level 2-3,專注於資產的物理和行為模型,以及製造數據的採集和情境化。
- **ERP**: 主要在 Level 4,專注於企業的業務流程管理。

---

## 2. 整合架構

### 2.1 核心架構圖

```
┌─────────────────────────────────────────────────────────┐
│                  Level 4: ERP (SAP, Oracle)                │
│  - 訂單管理 (Order Management)                             │
│  - 物料需求規劃 (MRP)                                      │
│  - 財務會計 (Financial Accounting)                         │
└────────────────────┬────────────────────────────────────────┘
                     ↕ (雙向整合)
┌─────────────────────────────────────────────────────────┐
│                  NDH-ERP 整合層 (本規格書重點)             │
│  ┌─────────────────┐   ┌─────────────────┐   ┌─────────────────┐ │
│  │ ERP Connector   │ ↔ │  Message Queue  │ ↔ │  NDH API        │ │
│  └─────────────────┘   └─────────────────┘   └─────────────────┘ │
└────────────────────┬────────────────────────────────────────┘
                     ↕
┌─────────────────────────────────────────────────────────┐
│          Level 2-3: NDH (Neutral Data Hub)               │
│  - Asset Tag Instance (虛擬分身物件)                          │
│  - 時序資料庫 (TDengine/PI)                              │
│  - 元數據資料庫 (PostgreSQL)                             │
└─────────────────────────────────────────────────────────┘
```

### 2.2 整合層組件說明

#### ERP Connector
- **職責**: 負責與特定的 ERP 系統進行通訊,處理數據格式的轉換。
- **實現**: 針對不同的 ERP (如 SAP S/4HANA, Oracle ERP Cloud) 開發不同的 Connector。
- **通訊方式**: 透過 ERP 提供的 API (如 SAP BAPI, OData, SOAP) 或中間件 (如 SAP PI/PO)。

#### Message Queue (訊息佇列)
- **職責**: 作為 NDH 與 ERP 之間的非同步通訊中介,實現系統解耦和削峰填谷。
- **技術選型**: RabbitMQ, Kafka, or Azure Service Bus / AWS SQS。
- **優點**: 提高系統的可靠性和擴展性,即使一方系統暫時離線,數據也不會遺失。

#### NDH API
- **職責**: 提供統一的 API 接口,供 ERP Connector 查詢 NDH 中的數據,或將 ERP 的數據寫入 NDH。
- **接口**: 提供 RESTful API 和 WebSocket 接口。

---



## 3. 技術規格

### 3.1 數據交換模型 (基於 ISA-95)

| 數據類別 | 從 ERP 到 NDH | 從 NDH 到 ERP |
|---|---|---|
| **生產工單 (Production Order)** | ✅ | ✅ (狀態更新、產量回報) |
| **物料清單 (BOM)** | ✅ | - |
| **設備主數據 (Equipment Master)** | ✅ | ✅ (運行時數、維護狀態) |
| **物料消耗 (Material Consumption)** | - | ✅ |
| **能源消耗 (Energy Consumption)** | - | ✅ |
| **OEE (Overall Equipment Effectiveness)** | - | ✅ |

### 3.2 API 規格 (NDH API)

#### 將 ERP 數據寫入 NDH

- **Endpoint**: `POST /api/v1/erp/events`
- **Request Body**:

```json
{
  "eventType": "ProductionOrderCreated",
  "timestamp": "2025-10-14T11:00:00Z",
  "payload": {
    "productionOrderId": "PO-67890",
    "productId": "PROD-ABC",
    "quantity": 5000,
    "equipmentId": "EQ-002"
  }
}
```

#### 從 NDH 查詢數據 (供 ERP 使用)

- **Endpoint**: `GET /api/v1/assets/{assetId}/kpis?name=oee`
- **Response**:

```json
{
  "assetId": "EQ-002",
  "kpi": "oee",
  "value": 0.85,
  "timestamp": "2025-10-14T11:00:00Z"
}
```

### 3.3 訊息佇列主題 (Topics)

- `erp.productionorder.events`: 生產工單相關事件 (Created, Released)
- `erp.equipment.master`: 設備主數據同步事件
- `ndh.production.confirmation`: 生產確認事件 (產量、工時、物料消耗)
- `ndh.equipment.kpis`: 設備 KPI 事件 (OEE, MTBF, MTTR)

### 3.4 IADL 擴展

為了儲存 ERP 相關資訊,IADL 需要擴展,增加 `erp` 區塊:

```yaml
Equipment Die_Bonder-01 {
    // ... 原有 IADL 定義 ...
    
    // 新增 erp 區塊
    erp: {
        equipment_id: "10001234", // ERP 中的設備編號
        cost_center: "CC-FAB1-ASSEMBLY", // 成本中心
        work_center: "WC-DB-01" // 工作中心
    }
}
```

---



## 4. 實施方式

### 4.1 階段一: 主數據同步 (1個月)

1. **開發 ERP Connector**: 實現從 ERP 獲取設備主數據的功能。
2. **擴展 IADL**: 為所有相關資產增加 `erp` 區塊。
3. **開發同步服務**: 定期將 ERP 的設備主數據同步到 IADL 中。
4. **目標**: 確保 NDH 與 ERP 之間的資產數據一致。

### 4.2 階段二: 生產數據上傳 (NDH → ERP) (2-3個月)

1. **開發 KPI 計算引擎**: 在 NDH 中開發服務,根據時序數據計算 OEE、MTBF、MTTR 等 KPI。
2. **開發生產確認服務**: 在 NDH 中開發服務,匯總產量、工時、物料消耗等數據。
3. **擴展 ERP Connector**: 實現將生產確認和 KPI 數據寫入 ERP 的功能。
4. **目標**: 將製造現場的實際數據自動回傳到 ERP,實現成本核算和績效評估的自動化。

### 4.3 階段三: 業務流程整合 (ERP → NDH) (2-3個月)

1. **擴展 ERP Connector**: 實現從 ERP 獲取生產工單的功能。
2. **擴展 Asset Tag Instance**: 讓 Asset Tag Instance 能夠接收生產工單,並將其與 MES 的工單進行關聯。
3. **目標**: 實現從 ERP 到製造現場的端到端業務流程整合。

---

## 5. 預期結果

### 5.1 功能性結果

- **自動化的成本核算**: ERP 可以根據從 NDH 自動獲取的物料消耗、能源消耗和工時數據,自動計算生產成本。
- **即時的績效監控**: 管理者可以在 ERP 中即時查看各個設備和產線的 OEE,並與財務數據進行關聯分析。
- **精準的資產管理**: ERP 中的設備主數據可以與 NDH 中的運行數據 (如運行時數、維護記錄) 進行同步,實現更精準的資產生命週期管理。
- **優化的生產排程**: ERP 可以根據從 NDH 獲取的即時設備狀態,動態調整生產排程。

### 5.2 業務價值

- **提高決策效率**: 為管理者提供即時、準確的數據,幫助他們做出更明智的業務決策。
- **降低營運成本**: 透過精準的成本核算和能源管理,找到降低成本的機會。
- **提高資產利用率**: 透過 OEE 監控和預測性維護,提高設備的綜合利用效率。
- **增強企業競爭力**: 打通企業的業務層和製造層,實現數據驅動的智慧營運。

---

