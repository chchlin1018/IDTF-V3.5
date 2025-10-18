

# 虛擬化 MES 接入 NDH 介面架構

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件旨在設計一個「虛擬化 MES」(Virtual MES, V-MES) 系統,用於模擬真實 MES 的核心功能,並與 NDH (Neutral Data Hub) 進行整合。這將為 IDTF 提供一個完整的端到端測試和演示環境,降低新用戶的學習門檻,並加速開發和驗證過程。

### 1.1 核心目標

1. **模擬 MES 核心功能**: 模擬工單管理、在製品追蹤、設備狀態監控等核心功能。
2. **標準化介面**: 提供與真實 MES 系統一致的 API 介面,以便無縫切換。
3. **可配置的行為**: 允許用戶自定義模擬的場景和行為,如:設備故障率、生產節拍等。
4. **輕量化部署**: 能夠以容器化方式快速部署,無需複雜的安裝和配置。

---

## 2. 系統架構

### 2.1 整體架構圖

```mermaid
graph TD
    subgraph IDTF
        NDH_API[NDH API]
        Message_Queue[Message Queue]
        Asset_Servant[Asset Servant]
    end

    subgraph Virtual_MES
        V_MES_API[V-MES API]
        Simulation_Engine[模擬引擎]
        Configuration_UI[配置介面]
        Data_Store[數據存儲 (SQLite/PostgreSQL)]
    end

    V_MES_API -- REST/WebSocket --> NDH_API
    Simulation_Engine -- 發布事件 --> Message_Queue
    Message_Queue -- 訂閱事件 --> Simulation_Engine
    Asset_Servant -- 讀寫 --> NDH_API
    Configuration_UI -- 配置 --> Simulation_Engine
```

### 2.2 核心組件

#### 1. V-MES API

- **職責**: 提供與真實 MES 系統一致的 RESTful API 和 WebSocket 介面。
- **技術**: FastAPI (Python) 或 Express.js (Node.js)。

#### 2. 模擬引擎 (Simulation Engine)

- **職責**: 執行模擬的核心邏輯,包括:
  - **工單生成器**: 根據配置自動生成模擬的生產工單。
  - **事件產生器**: 模擬設備狀態變更、產量回報、品質檢驗等事件。
  - **時間控制器**: 控制模擬的時間流逝速度 (如:加速、暫停)。
- **技術**: Python (SimPy, Celery)。

#### 3. 配置介面 (Configuration UI)

- **職責**: 提供一個 Web 介面,讓用戶可以:
  - 創建和編輯模擬場景。
  - 調整模擬參數 (如:設備 OEE、故障率、維修時間)。
  - 監控模擬的運行狀態。
- **技術**: React 或 Vue.js。

#### 4. 數據存儲 (Data Store)

- **職責**: 存儲模擬的數據,包括:
  - 模擬的工單、物料、產品等主數據。
  - 模擬產生的事件和日誌。
- **技術**: SQLite (輕量級) 或 PostgreSQL (功能更強)。

---



## 3. API 規格與數據模型

V-MES 的 API 將與 `09_MES_Integration` 和 `10_ERP_Integration` 中定義的 API 保持一致,以確保可替換性。

### 3.1 V-MES API 端點

#### `POST /api/v1/work-orders`

手動創建一個生產工單。

**Request Body**:

```json
{
  "productId": "PROD-SIM-01",
  "quantity": 100,
  "equipmentId": "EQ-SIM-01",
  "priority": 1
}
```

#### `GET /api/v1/work-orders/{workOrderId}`

查詢特定工單的狀態。

#### `GET /api/v1/equipments/{equipmentId}/status`

查詢特定設備的狀態。

### 3.2 模擬配置 API

#### `POST /api/v1/simulations`

創建一個新的模擬場景。

**Request Body**:

```json
{
  "scenarioName": "High_Volume_Production",
  "duration": 3600, // 模擬時長 (秒)
  "timeScale": 10, // 時間縮放比例 (10倍速)
  "equipments": [
    {
      "equipmentId": "EQ-SIM-01",
      "oee": 0.85,
      "mtbf": 100, // hours
      "mttr": 2 // hours
    }
  ]
}
```

#### `POST /api/v1/simulations/{simulationId}/start`

開始一個模擬。

#### `POST /api/v1/simulations/{simulationId}/pause`

暫停一個模擬。

#### `POST /api/v1/simulations/{simulationId}/stop`

停止一個模擬。

---

