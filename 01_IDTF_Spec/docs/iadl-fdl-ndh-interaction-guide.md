# IADL、FDL 與 NDH 互動流程指南

**文件版本**: 1.0  
**發布日期**: 2025-10-12  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)

---

## 1. 概述

本文件詳細說明 IADL (Industrial Asset Description Language)、FDL (Factory Design Language) 和 NDH (Neutral Data Hub) 三者之間的完整互動流程。這三個核心組件共同構成了 IDTF 生態系統從設計到執行的完整鏈路。

![IADL、FDL 與 NDH 互動流程圖](../images/iadl-fdl-ndh-interaction.png)

---

## 2. 三大階段

### 2.1. 設計階段 (Design Time - Offline)

這是離線的資產建模和工廠佈局設計階段，主要涉及兩個工具和兩種檔案格式。

#### 核心組件

**IADL Files (資產藍圖)**
- **格式**: YAML 或 JSON
- **內容**: 定義資產的類型、屬性、行為、事件和資料介面
- **作用**: 作為可重用的資產模板，描述「一個資產是什麼」
- **範例**: `StandardPump.iadl.yml`, `TemperatureSensor.iadl.yml`

**IADL Editor (資產建模工具)**
- **類型**: Qt/C++ 桌面應用程式
- **功能**: 
  - 從 CAD/BIM/PLM 系統匯入資產資料
  - 建立和編輯 IADL 資產藍圖
  - 提供語法高亮、資產瀏覽器、視覺化工具
  - 匯出 IADL 檔案供 Factory Designer 使用

**Factory Designer (工廠佈局設計工具)**
- **類型**: Web 應用程式 (React/Vue.js)
- **功能**:
  - 從 IADL Editor 載入資產藍圖
  - 提供 2D/3D 視覺化佈局設計器
  - 自動生成 FDL 配置檔案
  - 一鍵部署 FDL 到 NDH

**FDL Files (工廠佈局)**
- **格式**: YAML 或 JSON
- **內容**: 定義工廠的區域、要實例化的資產數量、位置和初始參數
- **作用**: 作為工廠部署計畫，描述「哪裡需要多少個什麼樣的資產」
- **範例**: `Fab01_Layout.fdl.yml`

### 2.2. 執行階段 (Runtime - Online)

這是線上的資產實例生成和即時資料交換階段，由 NDH 負責執行。

#### 核心組件

**NDH API (應用程式介面)**
- **協議**: RESTful API (HTTP/HTTPS)
- **端點**: `/api/deploy` (接收 FDL 部署請求)
- **功能**: 接收來自 Factory Designer 的 FDL 檔案

**IADL Connector (IADL 連接器)**
- **功能**: 
  - 載入和解析 IADL 資產定義
  - 提供資產的 Schema 給實例化引擎
  - 驗證 FDL 中引用的資產類型是否存在

**FDL Parser (FDL 解析器)**
- **功能**:
  - 解析和驗證 FDL 檔案的語法正確性
  - 提取區域、實例和關係資訊
  - 將解析結果傳遞給實例化引擎

**Instantiation Engine (實例化引擎)**
- **功能**:
  - 根據 FDL 的 `instances` 定義，批次創建資產物件實例
  - 根據 IADL 的 Schema，初始化每個實例的屬性
  - 生成唯一的實例 ID (例如 `Area1_Pump_001`)
  - 建立實例之間的關係

**Asset Servants (資產僕人 - 活躍實例)**
- **本質**: 記憶體中的 Python/Node.js 物件
- **功能**:
  - 儲存資產的當前狀態和屬性值
  - 提供標準介面 (`get_attribute`, `set_attribute`, `invoke_operation`)
  - 與實體工廠的感測器和執行器進行即時通訊

**Naming Service (命名服務)**
- **功能**:
  - 註冊所有活躍的資產實例
  - 提供「物件電話簿」功能，讓其他系統可以查找和存取資產實例
  - 生成 AssetReference (類似 CORBA IOR)

**Asset Adapter (資產適配器)**
- **功能**:
  - 管理資產實例的生命週期 (啟動、停用、銷毀)
  - 類似 CORBA 的 POA (Portable Object Adapter)

### 2.3. 物理世界 (Physical World)

**Physical Factory (實體工廠)**
- **組成**: 感測器、執行器、設備、PLC、SCADA 系統
- **互動**: 
  - NDH 透過 OPC UA、Modbus、MQTT 等協議與實體設備通訊
  - 即時控制與監控
  - 回饋資料到 NDH 的 Asset Servants

---

## 3. 完整互動流程

### 步驟 1：資產建模 (IADL Editor)

工程師使用 IADL Editor 從 CAD/BIM/PLM 系統匯入資產資料，或手動建立 IADL 資產藍圖。

**輸入**: CAD/BIM/PLM 資料  
**輸出**: IADL 檔案 (例如 `StandardPump.iadl.yml`)

### 步驟 2：工廠佈局設計 (Factory Designer)

工程師使用 Factory Designer 載入 IADL 檔案，在 2D/3D 佈局設計器中設計工廠佈局。

**輸入**: IADL 檔案  
**輸出**: FDL 檔案 (例如 `Fab01_Layout.fdl.yml`)

### 步驟 3：部署到 NDH

Factory Designer 透過 HTTP POST 請求，將 FDL 檔案推送到 NDH 的 `/api/deploy` 端點。

**請求範例**:
```http
POST /api/deploy HTTP/1.1
Host: ndh.factory.com:8000
Content-Type: application/json
Authorization: Bearer <API_KEY>

{
  "fdl_content": "...",
  "iadl_refs": ["StandardPump.iadl.yml", "TemperatureSensor.iadl.yml"]
}
```

### 步驟 4：FDL 解析與驗證

NDH 的 FDL Parser 解析 FDL 檔案，驗證語法正確性，並提取實例化資訊。

**驗證項目**:
- FDL 語法是否正確
- 引用的 IADL 資產類型是否存在
- 區域 ID 是否唯一
- 關係定義是否有效

### 步驟 5：載入 IADL 資產定義

IADL Connector 載入所有引用的 IADL 檔案，提供資產的 Schema 給實例化引擎。

### 步驟 6：批次實例化

Instantiation Engine 根據 FDL 的 `instances` 定義，批次創建資產物件實例。

**範例**:
```yaml
# FDL 中的定義
instances:
  - type: StandardPump
    count: 5
    naming_prefix: "Pump_"
```

**NDH 執行**:
```python
for i in range(1, 6):
    instance_id = f"Area1_Pump_{i:03d}"
    servant = AssetServant(
        asset_id=instance_id,
        asset_type="StandardPump",
        location="Area1"
    )
    # 根據 IADL Schema 初始化屬性
    servant.attributes = load_from_iadl("StandardPump")
    # 啟動物件
    adapter.activate_object(servant)
```

### 步驟 7：註冊到命名服務

每個創建的實例都會註冊到 Naming Service，生成 AssetReference。

**AssetReference 範例**:
```
ndhref://http@ndh.factory.com:8000/Area1/Pump_001?uuid=xxx
```

### 步驟 8：即時運行與監控

Asset Servants 與實體工廠的設備進行即時通訊，執行控制邏輯並收集回饋資料。

**資料流**:
- NDH → 實體工廠: 控制指令 (例如 `start_pump()`)
- 實體工廠 → NDH: 感測器資料 (例如溫度、壓力、流量)

---

## 4. 關鍵設計模式

### 4.1. 關注點分離 (Separation of Concerns)

- **IADL**: 只關心「資產是什麼」
- **FDL**: 只關心「哪裡需要多少個」
- **NDH**: 只關心「如何執行和管理」

### 4.2. 工廠模式 (Factory Pattern)

Instantiation Engine 作為工廠，根據 IADL 類型創建對應的 Asset Servant 實例。

### 4.3. 適配器模式 (Adapter Pattern)

Asset Adapter 管理物件的生命週期，類似 CORBA 的 POA。

### 4.4. 命名服務模式 (Naming Service Pattern)

Naming Service 提供全域的物件查找功能，實現位置透明性。

---

## 5. 資料流總結

```
[IADL Files] 
    ↓ (Import)
[IADL Editor] 
    ↓ (Export IADL)
[IADL Files] 
    ↓ (Load)
[Factory Designer] 
    ↓ (Generate)
[FDL Files] 
    ↓ (Deploy via HTTP POST)
[NDH API] 
    ↓ (Parse & Validate)
[FDL Parser + IADL Connector] 
    ↓ (Instantiate)
[Instantiation Engine] 
    ↓ (Create)
[Asset Servants] 
    ↓ (Control & Monitor)
[Physical Factory]
    ↓ (Feedback)
[Asset Servants]
```

---

## 6. 總結

IADL、FDL 和 NDH 三者形成了一個完整的閉環：

- **IADL** 定義了「什麼」
- **FDL** 聲明了「多少」和「哪裡」
- **NDH** 執行了「如何」

這種清晰的職責劃分，使得整個系統易於理解、維護和擴展，是 IDTF 生態系統能夠成功的關鍵基礎。

