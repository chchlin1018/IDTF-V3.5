# ISA-95 ↔ IADL/FDL 對映白皮書

**版本**: 1.0
**日期**: 2025-10-16
**作者**: Manus AI

---

## 摘要

本白皮書詳細闡述了 IDTF (Industrial Digital Twin Framework) 如何透過 IADL (Industrial Asset Definition Language) 和 FDL (Factory Design Language) 與 ISA-95 (Enterprise-Control System Integration) 標準進行深度整合。我們將探討 ISA-95 的設備階層、資產類型、工單/物料/人員模型以及狀態轉移事件來源如何精確對映到 IADL 和 FDL 的語義結構中。此外，本文件亦將介紹機器可驗證的對映規則和一致性規則，以確保數位分身數據的準確性、互操作性和生命週期事件的完整性，為工業 4.0 環境下的智慧製造提供堅實的數據基礎。

## 1. 引言

ISA-95 標準為企業與控制系統之間的整合提供了全面的模型和術語，是工業自動化領域的基石。IDTF 旨在建立一個開放、標準化的數位分身框架，其核心挑戰之一是如何有效地將物理世界的工業資產和業務流程，精確且一致地對映到數位模型中。本白皮書將聚焦於 IDTF 如何利用 FDL 和 IADL 來實現與 ISA-95 的深度對映，從而確保數位分身能夠準確反映物理世界的複雜性。

## 2. ISA-95 核心概念概述

ISA-95 標準定義了五個主要模型：

*   **設備階層模型 (Equipment Hierarchy Model)**：描述了企業、站點、區域、生產單元、設備模組和控制模組等不同層次的物理設備組織結構。
*   **資產模型 (Asset Model)**：定義了設備的屬性、能力和狀態。
*   **人員模型 (Personnel Model)**：描述了執行任務所需的人員及其技能。
*   **物料模型 (Material Model)**：定義了生產過程中使用的物料及其特性。
*   **工單模型 (Work Order Model)**：描述了生產或維護任務的詳細資訊。

本文件將主要關注設備階層、資產類型、工單/物料/人員模型以及狀態轉移事件來源與 IDTF 的對映。

## 3. ISA-95 ↔ IADL/FDL 對映詳解

IDTF 透過 FDL 和 IADL 的協同作用，實現了對 ISA-95 各個層面的精確對映。

### 3.1. 設備階層對映 (Equipment Hierarchy Mapping)

FDL 主要負責描述工廠的物理佈局和邏輯組織，因此它與 ISA-95 的設備階層模型有著直接的對應關係。NDH 根據 FDL 的定義構建 Asset Instance Hierarchy，該階層結構嚴格遵循 ISA-95 的層次。

| ISA-95 設備階層元素 | 描述 | IDTF 對映概念 (FDL) | 說明 |
|:---|:---|:---|:---|
| **Enterprise** | 整個企業組織，包含多個站點。 | **FDL Factory** (頂層 `factory_design`) | FDL 的頂層 `factory_design` 實例，代表整個企業的數位分身。 |
| **Site** | 企業內的一個地理位置或主要生產設施。 | **FDL Building** (`building_id`) | FDL 中的 `buildings` 實例，代表一個物理廠區或設施。 |
| **Area** | 站點內的一個邏輯或物理分區，例如一個車間或生產線。 | **FDL Layout Area** (`layout.area`) | FDL 中的 `layout.area` 實例，代表廠區內的特定區域或生產線。 |
| **Process Cell** | 執行主要生產過程的單元，通常包含多個工作單元。 | **FDL Layout Area** (`zone_type: ProcessCell`) | FDL 中的 `layout.area` 實例，當 `zone_type` 為 `ProcessCell` 時，代表一個完整的生產流程單元。 |
| **Unit** | 執行特定生產操作的設備集合。 | **FDL Layout Instance Group** | FDL `layout.instances` 中的一組 Asset Instance，代表一個獨立的工作單元或設備組。 |
| **Equipment Module** | 執行特定控制功能的設備或設備組件。 | **FDL AssetInstance** | FDL 實例化的具體設備，代表一個 Asset Instance。這是 IDTF 中最細粒度的可獨立管理的資產。 |
| **Control Module** | 執行基本控制功能的設備組件，如感測器、執行器。 | **FDL ComponentInstance** | FDL 實例化的具體組件，通常是 Asset Instance 的組成部分。

### 3.2. 資產類型對映 (Asset Type Mapping)

IADL 專注於定義單一資產的藍圖，因此它與 ISA-95 的資產模型有著直接的對應關係。每個 IADL `AssetType` 定義了一個 ISA-95 意義上的資產類型，包含了其所有相關的屬性、數據點、能力和行為。

| ISA-95 資產模型元素 | 描述 | IDTF 對映概念 (IADL) | 說明 |
|:---|:---|:---|:---|
| **Asset Class/Type** | 設備的通用類別。 | **IADL AssetType** | IADL 的核心定義，描述了一類資產的通用特性。 |
| **Properties** | 資產的靜態屬性。 | **IADL `properties`** | IADL `AssetType` 中的 `properties` 區塊，定義了資產的靜態配置參數。 |
| **Capabilities** | 資產能執行什麼功能。 | **IADL `capabilities`** | IADL `AssetType` 中的 `capabilities` 區塊，描述了資產的功能。 |
| **Data Points/Tags** | 資產的實時數據點。 | **IADL `data_tags`** | IADL `AssetType` 中的 `data_tags` 區塊，定義了資產的實時監測點。 |
| **Components** | 資產的組成部分。 | **IADL `components`** | IADL `AssetType` 中的 `components` 區塊，定義了資產的子組件，每個子組件也可以是 IADL `ComponentType`。 |
| **3D Model Reference** | 資產的 3D 模型路徑。 | **IADL `geometry.model_file`** | IADL `AssetType` 中的 `geometry.model_file` 字段，指向該資產的 USD 模型檔案。

### 3.3. 工單/物料/人員模型對映

雖然 FDL 和 IADL 主要關注物理資產的描述，但 IDTF 的 NDH 作為數據中樞，能夠整合來自 MES (製造執行系統)、ERP (企業資源規劃) 和 CMMS (電腦化維護管理系統) 的工單、物料和人員數據，並將其與 Asset Instance 關聯。

*   **工單 (Work Order)**：NDH 可以接收來自 MES/CMMS 的工單事件，並將其與受影響的 Asset Instance 關聯。工單的狀態變化（例如啟動、完成、故障）可以更新 Asset Instance 的狀態或觸發相關的事件。
*   **物料 (Material)**：物料消耗或生產數據可以與 Asset Instance 關聯，反映生產過程中的物料流動。IADL 也可以定義資產處理的物料類型。
*   **人員 (Personnel)**：與特定 Asset Instance 互動的人員（例如操作員、維護人員）的資訊可以透過 NDH 整合，用於追溯操作歷史和權限管理。

### 3.4. 狀態轉移事件來源對映

ISA-95 強調資產的狀態管理和生命週期事件。IDTF 的 NDH 透過監聽來自不同源系統的事件，來驅動 Asset Instance 的狀態轉移。這些事件來源包括：

*   **MES 事件**：例如生產訂單啟動/停止、批次完成、質量異常等。
*   **CMMS 工單**：例如維護工單的創建、執行、完成或取消。
*   **SCADA/PLC 事件**：例如設備啟動/停止、故障報警、模式切換等。
*   **IoT 感測器數據**：例如當某個關鍵參數超出閾值時觸發的事件。

IDTF 確保這些事件能夠被 NDH 捕獲，並根據預定義的規則觸發 Asset Instance 的狀態更新或相關行為。

## 4. 機器可驗證對映與一致性規則

為確保 IDTF 數位分身數據的質量和一致性，我們定義了機器可驗證的對映規則和一致性規則。

### 4.1. 機器可驗證對映 (Machine-Verifiable Mapping)

對映規則將以結構化格式（如 CSV 或 JSON）定義，並輔以 JSON-Schema 進行驗證。這確保了從源系統（如 FDL/IADL 定義）到 NDH 內部模型的轉換是正確且符合預期的。

**示例：IADL `data_tags` 到 NDH Asset Tag Instance 的對映規則 (JSON 格式)**

```json
[
  {
    "source_path": "IADL.AssetType.data_tags[*].tag_id",
    "target_path": "NDH.AssetInstance.AssetTagInstances[*].tag_id",
    "type_mapping": "string",
    "required": true,
    "validation_regex": "^[a-zA-Z0-9_]+$",
    "description": "IADL 中定義的數據點 ID 必須對應到 NDH 中的 Asset Tag Instance ID"
  },
  {
    "source_path": "IADL.AssetType.data_tags[*].data_type",
    "target_path": "NDH.AssetInstance.AssetTagInstances[*].data_type",
    "type_mapping": "string",
    "enum": ["float", "integer", "boolean", "string"],
    "required": true,
    "description": "數據類型必須為預定義的類型之一"
  }
]
```

這些規則將作為自動化工具（例如 Conformance Checker）的輸入，用於驗證 FDL 和 IADL 定義的正確性以及它們在 NDH 中的實例化。

### 4.2. 一致性規則 (Consistency Rules)

一致性規則定義了數位分身在運行時必須滿足的約束條件，特別是關於狀態轉移和數據來源的。

**示例：FDL.Asset.state 的一致性規則**

*   **規則描述**：「FDL.Asset.state (即 Asset Instance 的運行狀態) 只能由 MES 事件（例如生產啟動/停止、批次完成）或 CMMS 工單（例如維護開始/結束）的明確指令觸發關閉或變更，不能由其他隨機事件或直接操作修改。」
*   **驗證機制**：NDH 內部狀態機將強制執行此規則。任何嘗試透過非授權事件源修改 Asset Instance 狀態的請求將被拒絕或記錄為異常。

這些一致性規則確保了數位分身行為的預測性和可靠性，特別是在關鍵的生產和維護流程中。

## 5. 結論

本白皮書詳細闡述了 IDTF 如何透過 FDL 和 IADL 與 ISA-95 標準進行深度對映，並透過機器可驗證的對映規則和一致性規則確保了數位分身數據的質量。這種整合為工業數位分身提供了堅實的數據基礎，使得企業能夠更有效地管理其資產、優化生產流程並提高運營效率。隨著 IDTF 的不斷發展，我們將持續完善這些對映和規則，以適應工業領域日益增長的需求。

## 參考文獻

*   [1] ISA-95 Standard: Enterprise-Control System Integration. International Society of Automation.
*   [2] FDL (Factory Design Language) Specification. IDTF v3.5 Documentation.
*   [3] IADL (Industrial Asset Definition Language) Specification. IDTF v3.5 Documentation.
*   [4] NDH (Neutral Data Hub) Architecture. IDTF v3.5 Documentation.

