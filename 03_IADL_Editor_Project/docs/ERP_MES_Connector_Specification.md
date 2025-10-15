# ERP/MES 整合連接器規範

**版本**: 1.1  
**日期**: 2025-10-15  
**作者**: Manus AI

---

## 1. 概述

本文件詳細定義了 **IDTF V3.5 IADL Editor (Asset Designer)** 與主流 **ERP (企業資源規劃)** 和 **MES (製造執行系統)** 系統的整合連接器規範。此整合旨在實現資產從採購、製造到運營維護階段的數據流動，將業務層面的資源規劃資訊和生產現場的執行數據無縫對接到 IADL 資產定義中，從而提供資產的完整數位化上下文，支援更精準的決策和優化。本次更新將特別強調對 **IADL 和 FDL Spec 中定義的擴展屬性（如營運、維護、財務屬性）以及資產生命週期狀態管理**的支援。

## 2. 整合目標

IADL Editor 與 ERP/MES 系統的整合將實現以下核心目標：

1.  **供應商與採購資訊整合**：從 ERP 系統獲取資產的供應商、製造商、採購訂單、物料主數據等資訊，豐富 IADL 資產定義的 `vendor_info` 和 `procurement_info` 字段，並支援**資產實例的生命週期狀態更新**（如 `Ordered`, `In_Production`）。
2.  **製造執行數據整合**：從 MES 系統獲取資產的製程參數、品質標準、生產配方、工單資訊等，補充 IADL 資產定義的 `parameters`、`quality_standards` 和 `recipes` 字段，並支援**資產實例的運營狀態更新**（如 `Running`, `Stopped`, `Faulted`）和**生命週期事件觸發**。
3.  **資產數據上下文豐富**：為數位分身提供更全面的業務和生產現場數據，使其能夠反映資產的真實成本、供應鏈來源、製造過程和品質屬性，並將這些資訊與**資產的生命週期狀態**緊密關聯。
4.  **數據一致性與追溯**：確保 ERP、MES 和 IDTF 平台之間的資產相關數據保持一致，並支援從數位分身追溯到原始業務和生產數據，包括**資產的生命週期事件**。
5.  **支援運營決策**：整合後的數據將支援更精準的資產性能分析、維護排程優化和生產過程改進，特別是透過**資產生命週期狀態的自動更新**來驅動相關業務流程。

## 3. 支援的 ERP/MES 系統與整合方式

IADL Editor 將透過標準 API 或專用連接器與以下主流 ERP/MES 系統進行整合：

| 系統類型 | 系統名稱 | 整合方式 | 支援功能 | 備註 |
|:---|:---|:---|:---|:---|
| **ERP 系統** | SAP ERP | OData API / RFC | 物料主數據、供應商資訊、採購訂單、庫存資訊、**資產會計數據** | 支援 S/4HANA |
| | Oracle EBS | REST API / SOAP | 物料、供應商、採購、資產管理模組數據、**財務數據** | |
| | Microsoft Dynamics 365 | REST API | 供應商、採購、生產訂單、資產數據、**維護工單** | |
| **MES 系統** | Siemens Opcenter | REST API / OPC UA | 製程參數、品質數據、生產配方、工單狀態、**設備運營狀態** | |
| | Rockwell FactoryTalk ProductionCentre | REST API / OPC UA | 設備數據、生產追溯、品質管理、排程、**事件日誌** | |
| | AVEVA MES | REST API / OPC UA | 生產數據、批次管理、能源管理、**警報與事件** | |

## 4. 整合工作流

以下是 IADL Editor 與 ERP/MES 系統整合的典型工作流，已更新以反映 IADL 和 FDL Spec 的修正建議：

1.  **資產設計與定義**：
    *   在 IADL Editor 中，資產的基本設計和工程資訊（來自 CAD/PLM）已經被定義為 IADL 資產定義。此時，資產定義可能處於 `Design_In_Progress` 或 `Approved` 狀態。

2.  **IADL Editor 數據請求與豐富**：
    *   使用者在 IADL Editor 中選擇一個 IADL 資產定義，並發起對 ERP 或 MES 數據的請求，例如根據零件號碼查詢供應商資訊，或根據設備 ID 查詢製程參數。
    *   IADL Editor 透過相應的 ERP 或 MES 連接器呼叫其 API。

3.  **ERP/MES 連接器數據提取與轉換**：
    *   **ERP 連接器**：根據 IADL Editor 提供的關鍵識別符（如物料號、供應商 ID），透過 ERP 系統的 API 查詢相關的物料主數據、供應商詳細資訊、採購訂單歷史、**資產會計數據（如採購成本、折舊資訊）**等。將這些數據轉換為 IADL Editor 可理解的中間數據結構。
    *   **MES 連接器**：根據 IADL Editor 提供的設備 ID 或產品 ID，透過 MES 系統的 API 查詢設備的製程參數、品質標準、生產配方、**設備運營狀態、警報與事件日誌**等。將這些數據轉換為 IADL Editor 可理解的中間數據結構。

4.  **IADL 資產定義數據豐富**：
    *   IADL Editor 接收中間數據結構，並將其整合到現有的 IADL 資產定義中。
    *   這包括：
        *   更新 `asset_type.vendor_info` 字段，包含供應商 ID、名稱、聯繫方式等。
        *   更新 `asset_type.procurement_info` 字段，包含採購訂單號、採購日期、成本等。
        *   更新 `asset_type.financial_attributes` 字段，添加來自 ERP 的**採購成本、折舊資訊**等。
        *   更新 `asset_type.parameters` 字段，添加來自 MES 的製程參數。
        *   更新 `asset_type.quality_standards` 字段，添加來自 MES 的品質標準。
        *   更新 `asset_type.recipes` 字段，添加來自 MES 的生產配方。
        *   **根據 ERP 採購訂單狀態，更新資產實例的生命週期狀態**（例如從 `Design_In_Progress` 到 `Ordered`）。
        *   **根據 MES 生產工單狀態，更新資產實例的生命週期狀態**（例如從 `Ordered` 到 `In_Production`）。

5.  **儲存到 Factory Asset Library**：
    *   完成數據豐富的 IADL 資產定義被儲存到 NDH Runtime 中的 Factory Asset Library。
    *   Factory Asset Library 會對資產定義進行版本控制，確保每次修改都有記錄。
    *   **資產實例的生命週期狀態和運營狀態的變更也會被記錄為生命週期事件**。

## 5. 數據映射規範

為了確保數據的正確轉換，需要明確定義 ERP/MES 數據與 IADL/FDL 結構之間的映射關係。此處將重點擴展對豐富屬性和生命週期狀態的支援。

### 5.1 ERP 數據映射

| ERP 數據元素 | IADL/FDL 映射路徑 | 說明 |
|:---|:---|:---|
| 物料號 | `asset_type.name` (作為參考) | 用於查詢 ERP 數據的關鍵識別符。 |
| 物料描述 | `asset_type.description` | 資產的詳細描述。 |
| 製造商名稱 | `asset_type.vendor_info.manufacturer.name` | 資產的製造商名稱。 |
| 製造商 ID | `asset_type.vendor_info.manufacturer.id` | 資產的製造商唯一識別符。 |
| 供應商名稱 | `asset_type.vendor_info.supplier.name` | 資產的供應商名稱。 |
| 供應商 ID | `asset_type.vendor_info.supplier.id` | 資產的供應商唯一識別符。 |
| 採購訂單號 | `asset_type.procurement_info.purchase_order` | 採購該資產的訂單號。 |
| 採購日期 | `asset_type.procurement_info.purchase_date` | 採購訂單的日期。 |
| 總金額 | `asset_type.procurement_info.total_amount` | 採購訂單的總金額。 |
| 庫存狀態 | `asset_type.inventory_status` | 資產的庫存狀態。 |
| **採購成本** | `asset_type.financial_attributes.purchase_cost` | 資產的採購成本。 |
| **折舊資訊** | `asset_type.financial_attributes.depreciation_info` | 資產的折舊相關數據。 |
| **資產會計編號** | `asset_type.financial_attributes.asset_account_id` | 資產在會計系統中的編號。 |
| **採購訂單狀態** | `asset_instance.lifecycle_status` (事件觸發) | ERP 中的採購訂單狀態變更可觸發資產實例的生命週期狀態更新（如 `Ordered`, `Received`）。 |

### 5.2 MES 數據映射

| MES 數據元素 | IADL/FDL 映射路徑 | 說明 |
|:---|:---|:---|
| 設備 ID | `asset_type.name` (作為參考) | 用於查詢 MES 數據的關鍵識別符。 |
| 製程參數 (名稱、單位、範圍) | `asset_type.parameters` | 映射為 IADL 的參數列表，包含名稱、數據類型、單位、最小值、最大值等。 |
| 品質標準 (檢驗項目、規範、公差) | `asset_type.quality_standards` | 映射為 IADL 的品質標準列表。 |
| 生產配方 (物料、步驟、參數) | `asset_type.recipes` | 映射為 IADL 的生產配方列表。 |
| 工單號 | `asset_type.operational_attributes.work_order_id` | 關聯的生產工單號。 |
| 生產狀態 | `asset_type.operational_attributes.production_status` | 資產的生產狀態。 |
| **設備運營狀態** | `asset_instance.operational_status` | MES 中的設備運營狀態（如 `Running`, `Stopped`, `Standby`, `Faulted`）直接映射到資產實例的運營狀態。 |
| **警報與事件** | `asset_instance.lifecycle_events` | MES 中的警報和事件可記錄為資產實例的生命週期事件。 |
| **維護工單** | `asset_instance.maintenance_records` (事件觸發) | MES 中生成的維護工單可觸發維護記錄的創建，並更新資產實例的生命週期狀態（如 `Under_Maintenance`）。 |

## 6. API 介面規範

ERP/MES 連接器將提供標準化的 API 介面，供 IADL Editor 調用以提取數據。這些 API 介面應遵循 RESTful 設計原則，並支援 JSON 格式的數據交換。更新後的 API 將能夠提供更豐富的資產元數據和狀態資訊。

### 6.1 ERP 連接器 API (範例)

**Base URL**: `/api/v1/erp/{erp_system_name}`

-   **`GET /materials/{material_number}`**
    *   **描述**: 獲取指定物料號的物料主數據，包括財務相關屬性。
    *   **參數**: `material_number` (string) - 物料唯一識別符。
    *   **回應**: `application/json` (包含物料描述、製造商、型號、單位、**採購成本、折舊資訊**等)。

-   **`GET /vendors/{vendor_id}`**
    *   **描述**: 獲取指定供應商 ID 的詳細資訊。
    *   **參數**: `vendor_id` (string) - 供應商唯一識別符。
    *   **回應**: `application/json` (包含供應商名稱、聯繫方式、地址等)。

-   **`GET /purchase_orders/{po_number}`**
    *   **描述**: 獲取指定採購訂單號的詳細資訊，包括其當前狀態。
    *   **參數**: `po_number` (string) - 採購訂單號。
    *   **回應**: `application/json` (包含採購日期、供應商 ID、總金額、**訂單狀態**等)。

### 6.2 MES 連接器 API (範例)

**Base URL**: `/api/v1/mes/{mes_system_name}`

-   **`GET /equipment/{equipment_id}/parameters`**
    *   **描述**: 獲取指定設備的製程參數。
    *   **參數**: `equipment_id` (string) - 設備唯一識別符。
    *   **回應**: `application/json` (包含參數名稱、數據類型、單位、範圍等)。

-   **`GET /products/{product_id}/quality_standards`**
    *   **描述**: 獲取指定產品的品質標準。
    *   **參數**: `product_id` (string) - 產品唯一識別符。
    *   **回應**: `application/json` (包含檢驗項目、規範、公差等)。

-   **`GET /recipes/{recipe_id}`**
    *   **描述**: 獲取指定生產配方的詳細資訊。
    *   **參數**: `recipe_id` (string) - 配方唯一識別符。
    *   **回應**: `application/json` (包含物料清單、製程步驟、參數等)。

-   **`GET /equipment/{equipment_id}/status`** (新增)
    *   **描述**: 獲取指定設備的即時運營狀態和最近的事件。
    *   **參數**: `equipment_id` (string) - 設備唯一識別符。
    *   **回應**: `application/json` (包含 `operational_status`、`last_event_timestamp`、`last_event_type` 等)。

## 7. 實作考量

### 7.1 數據同步策略

-   **手動觸發**：使用者在 IADL Editor 中手動觸發數據提取和同步。
-   **事件驅動**：監聽 ERP/MES 系統中的數據變更事件，自動觸發 IADL Editor 中的更新流程。這需要 ERP/MES 系統提供 Webhook 或訊息發布機制，以實現**資產生命週期狀態的自動更新**。
-   **排程同步**：定期從 ERP/MES 系統中拉取數據，檢查更新。適用於數據更新頻率不高的場景，例如**定期同步財務屬性或庫存狀態**。

### 7.2 錯誤處理與日誌

-   整合過程中可能出現數據格式不匹配、API 調用失敗、權限不足等錯誤，需要有完善的錯誤處理機制，並向使用者提供清晰的錯誤訊息。
-   詳細的日誌記錄對於問題診斷和追溯至關重要，應記錄每次 API 調用、數據轉換、**資產狀態變更**和錯誤發生情況。

### 7.3 性能優化

-   對於大量數據的提取，應考慮分頁 (pagination) 和批次處理 (batch processing) 機制，避免單次請求過載。
-   連接器應支援數據快取，減少對 ERP/MES 系統的重複請求。

### 7.4 安全性

-   與 ERP/MES 系統的連接應使用安全的通訊協定 (HTTPS)。
-   API 訪問應透過嚴格的認證和授權機制進行保護，例如 OAuth 2.0、API Key 或基於角色的訪問控制 (RBAC)。
-   敏感數據（如供應商價格、生產機密）在傳輸和儲存過程中應進行加密。

### 7.5 數據轉換與標準化

-   ERP/MES 系統的數據模型可能與 IADL 存在差異，連接器需要負責數據的轉換和標準化，確保數據符合 IADL 規範。這包括將 ERP/MES 的狀態碼映射到 IADL/FDL 定義的生命週期狀態。
-   應提供可配置的映射規則，允許使用者或管理員根據實際需求調整數據映射關係。

## 8. 結論

本 ERP/MES 整合連接器規範為 IADL Editor 實現與業務資源規劃和製造執行數據的深度融合提供了詳細指導。透過標準化的介面和嚴謹的數據映射，並深度整合**擴展的業務和生產相關屬性以及資產生命週期狀態管理**，IDTF V3.5 將能夠構建一個真正全面的數位分身，不僅包含資產的物理和行為特性，還涵蓋其業務和生產上下文，為智慧製造和工業 4.0 的實施提供強大支援。這將極大地提升資產管理的精確性、自動化程度和決策支援能力。
