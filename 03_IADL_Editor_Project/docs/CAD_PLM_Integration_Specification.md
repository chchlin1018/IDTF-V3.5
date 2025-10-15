# CAD/PLM 系統整合介面規範

**版本**: 1.1  
**日期**: 2025-10-15  
**作者**: Manus AI

---

## 1. 概述

本文件詳細定義了 **IDTF V3.5 IADL Editor (Asset Designer)** 與主流 **CAD (電腦輔助設計)** 和 **PLM (產品生命週期管理)** 系統的整合介面規範。此整合旨在實現設計階段資產數據的無縫流動，將工程設計資訊自動轉換為 IADL (Industrial Asset Definition Language) 資產定義，從而提高數據一致性、減少手動錯誤並加速數位分身模型的建立。本次更新將特別強調對 **ISA-95 階層、多維度視圖、豐富數據模型和生命週期管理**等 IADL 修正建議的支援。

## 2. 整合目標

IADL Editor 與 CAD/PLM 系統的整合將實現以下核心目標：

1.  **自動化數據提取**：從 CAD 模型中提取 3D 幾何資訊，並從 PLM 系統中提取物料清單 (BOM)、技術規格、零件屬性、文件連結，以及 **ISA-95 階層相關屬性**和**資產生命週期狀態**。
2.  **IADL 自動生成**：根據提取的 CAD/PLM 數據，自動生成符合 IADL 規範的資產定義草稿，並預填充 ISA-95 階層和初始生命週期狀態。
3.  **數據豐富與補充**：允許工程師在 IADL Editor 中對自動生成的資產定義進行補充，例如添加數據標籤、行為腳本、控制邏輯、**次要階層分組**、以及 IADL 擴展的**營運、維護、財務屬性**。
4.  **版本控制與追溯**：確保資產定義的變更能夠與原始 CAD/PLM 數據保持同步，並支援版本追溯。
5.  **統一資產庫**：將設計完成的 IADL 資產定義儲存到 Factory Asset Library，作為所有下游應用（如 NDH Runtime、數位分身應用）的單一數據源。

## 3. 支援的 CAD/PLM 系統與整合方式

IADL Editor 將透過標準 API 或專用連接器與以下主流 CAD/PLM 系統進行整合：

| 系統類型 | 系統名稱 | 整合方式 | 支援功能 | 備註 |
|:---|:---|:---|:---|:---|
| **CAD 系統** | Autodesk Inventor | COM API / Forge API | 3D 模型匯入 (USD 格式)、BOM 提取、參數同步 | 優先使用 Forge API 進行雲端整合 |
| | SolidWorks | SolidWorks API | 3D 模型匯入 (USD 格式)、配置變體、裝配體結構 | |
| | CATIA | CAA API | 3D 模型匯入 (USD 格式)、產品結構、PMI 數據 | |
| | Siemens NX | NX Open API | 3D 模型匯入 (USD 格式)、裝配體、製造資訊 | |
| **PLM 系統** | Siemens Teamcenter | REST API + SOA | 零件資訊、BOM、變更管理、文件管理、**資產生命週期狀態同步、ISA-95 階層屬性** | 支援 Teamcenter X (雲端) |
| | PTC Windchill | REST API | 零件資訊、BOM、生命週期狀態、**ISA-95 階層屬性** | |
| | Dassault ENOVIA | REST API | 產品結構、文件、變更單、**ISA-95 階層屬性** | |
| | SAP PLM | OData API | 物料主數據、BOM、ECN/ECO、**ISA-95 階層屬性** | |

## 4. 整合工作流

以下是 IADL Editor 與 CAD/PLM 系統整合的典型工作流，已更新以反映 IADL Spec 的修正建議：

1.  **CAD 設計與 PLM 管理**：
    *   工程師在 CAD 軟體中設計資產的 3D 模型，並定義其幾何、尺寸和材料屬性。
    *   設計數據同步到 PLM 系統，在 PLM 中管理零件、物料清單 (BOM)、技術規格、文件、**資產生命週期狀態**，並可定義**資產在 ISA-95 階層中的歸屬**。

2.  **IADL Editor 數據提取請求**：
    *   在 IADL Editor 中，使用者選擇一個 CAD 模型或 PLM 零件/組件，並發起數據提取請求。
    *   IADL Editor 透過相應的 CAD/PLM 連接器呼叫其 API。

3.  **CAD/PLM 連接器數據轉換**：
    *   CAD 連接器負責將 3D 模型匯出為標準的 USD (Universal Scene Description) 格式，並提取相關的幾何和視覺屬性。
    *   PLM 連接器負責從 PLM 系統中讀取零件的 BOM 結構、屬性（如製造商、型號、序列號、技術參數）、關聯文件（如操作手冊、檢驗報告）、**ISA-95 階層級別**和**資產生命週期狀態**。
    *   連接器將這些原始數據轉換為 IADL Editor 可理解的中間數據結構。

4.  **IADL 資產定義自動生成**：
    *   IADL Editor 接收中間數據結構，並自動生成 IADL 資產定義的初始草稿。
    *   這包括：
        *   `asset_type` 的基本資訊 (名稱、顯示名稱、描述、類別、製造商、型號、版本)。
        *   `geometry` 資訊 (USD 文件路徑、縮略圖)。
        *   `components` (從 BOM 映射)。
        *   `documents` (從 PLM 文件連結映射，指向 NDH Document Server)。
        *   `parameters` (從 PLM 屬性映射)。
        *   **`isa95_hierarchy_level`** (從 PLM 映射，如 `equipment_type`, `equipment`)。
        *   **`lifecycle_status`** (從 PLM 映射，如 `design_in_progress`, `released`)。
        *   **`engineering_attributes`** (從 PLM 映射，如 `design_pressure`, `material` 等擴展工程屬性)。

5.  **工程師補充與完善**：
    *   工程師在 IADL Editor 中審閱自動生成的 IADL 定義。
    *   補充數位分身特有的資訊，例如：
        *   `data_tags` (感測器、執行器、控制點)。
        *   `behaviors` (資產的運作邏輯、狀態機)。
        *   `control_logic` (與 PLC/SCADA 系統的介面定義)。
        *   `vendor_info` (從 ERP 系統獲取的供應商詳細資訊)。
        *   **`logical_groups`** (定義資產所屬的次要階層，如學科、維護分組)。
        *   **`operational_attributes`** (營運屬性，如 `operating_limits`, `setpoints`)。
        *   **`maintenance_attributes`** (維護屬性，如 `maintenance_schedule`, `mtbf`)。
        *   **`financial_attributes`** (財務屬性，如 `purchase_cost`, `depreciation_info`)。
        *   **`connections`** (定義資產之間的連接關係)。

6.  **儲存到 Factory Asset Library**：
    *   完成的 IADL 資產定義被儲存到 NDH Runtime 中的 Factory Asset Library。
    *   Factory Asset Library 會對資產定義進行版本控制，確保每次修改都有記錄。

## 5. 數據映射規範

為了確保數據的正確轉換，需要明確定義 CAD/PLM 數據與 IADL 結構之間的映射關係。此處將重點擴展 PLM 數據映射。

### 5.1 CAD 數據映射 (保持不變)

| CAD 數據元素 | IADL 映射路徑 | 說明 |
|:---|:---|:---|
| 3D 模型文件 | `asset_type.geometry.usd_file` | 匯出為 USD 格式，儲存路徑。 |
| 縮略圖 | `asset_type.geometry.thumbnail` | 3D 模型預覽圖，儲存路徑。 |
| 零件名稱 | `asset_type.name` / `asset_type.display_name` | 資產類型名稱。 |
| 零件屬性 (尺寸、材料) | `asset_type.parameters` | 映射為 IADL 的參數列表。 |

### 5.2 PLM 數據映射 (更新)

| PLM 數據元素 | IADL 映射路徑 | 說明 |
|:---|:---|:---|
| 零件 ID | `asset_type.name` | 作為 IADL 資產類型的唯一識別符。 |
| 零件名稱 | `asset_type.display_name` | 資產的顯示名稱。 |
| 零件描述 | `asset_type.description` | 資產的詳細描述。 |
| 零件類型/分類 | `asset_type.category` | 資產的分類，例如「泵」、「馬達」。 |
| 製造商 | `asset_type.manufacturer` | 資產的製造商資訊。 |
| 型號 | `asset_type.model` | 資產的型號。 |
| 版本/修訂號 | `asset_type.version` | 資產定義的版本號。 |
| BOM 結構 | `asset_type.components` | 映射為 IADL 的組件列表，包含子零件 ID、名稱、數量等。 |
| 關聯文件 | `asset_type.documents` | 映射為 IADL 的文件連結列表，連結到 NDH Document Server。 |
| 其他屬性 (例如電壓、功率) | `asset_type.engineering_attributes` 或 `asset_type.parameters` | 映射為 IADL 的工程屬性或參數列表。 |
| **ISA-95 階層級別** | `asset_type.isa95_hierarchy_level` | 資產在 ISA-95 階層中的級別 (例如 `equipment_type`, `equipment`)。 |
| **生命週期狀態** | `asset_type.lifecycle_status` | 資產的生命週期狀態 (例如 `design_in_progress`, `released`)。 |
| **次要階層標籤** | `asset_type.logical_groups` | PLM 中可能存在的分類或標籤，可映射為 IADL 的次要階層分組。 |

## 6. API 介面規範

CAD/PLM 連接器將提供標準化的 API 介面，供 IADL Editor 調用以提取數據。這些 API 介面應遵循 RESTful 設計原則，並支援 JSON 格式的數據交換。更新後的 API 將能夠提供更豐富的資產元數據。

### 6.1 CAD 連接器 API (範例) (保持不變)

**Base URL**: `/api/v1/cad/{cad_system_name}`

-   **`GET /models/{model_id}/usd`**
    *   **描述**: 匯出指定 CAD 模型的 USD 文件。
    *   **參數**: `model_id` (string) - CAD 模型唯一識別符。
    *   **回應**: `application/octet-stream` (USD 文件內容)。

-   **`GET /models/{model_id}/thumbnail`**
    *   **描述**: 獲取指定 CAD 模型的縮略圖。
    *   **參數**: `model_id` (string) - CAD 模型唯一識別符。
    *   **回應**: `image/png` (圖片內容)。

-   **`GET /models/{model_id}/properties`**
    *   **描述**: 獲取指定 CAD 模型的屬性。
    *   **參數**: `model_id` (string) - CAD 模型唯一識別符。
    *   **回應**: `application/json` (包含尺寸、材料等屬性)。

### 6.2 PLM 連接器 API (範例) (更新)

**Base URL**: `/api/v1/plm/{plm_system_name}`

-   **`GET /parts/{part_id}`**
    *   **描述**: 獲取指定零件的詳細資訊，包括其 ISA-95 階層級別和生命週期狀態。
    *   **參數**: `part_id` (string) - PLM 零件唯一識別符。
    *   **回應**: `application/json` (包含零件名稱、描述、製造商、型號、版本、生命週期狀態、**ISA-95 階層級別**等)。

-   **`GET /parts/{part_id}/bom`**
    *   **描述**: 獲取指定零件的物料清單 (BOM)。
    *   **參數**: `part_id` (string) - PLM 零件唯一識別符。
    *   **回應**: `application/json` (包含子零件列表，每個子零件包含 ID、名稱、數量等)。

-   **`GET /parts/{part_id}/documents`**
    *   **描述**: 獲取指定零件的關聯文件列表。
    *   **參數**: `part_id` (string) - PLM 零件唯一識別符。
    *   **回應**: `application/json` (包含文件名稱、類型、NDH Document Server 連結等)。

## 7. 實作考量

### 7.1 數據同步策略 (保持不變)

-   **手動觸發**：使用者在 IADL Editor 中手動觸發數據提取和同步。
-   **事件驅動**：監聽 CAD/PLM 系統中的數據變更事件，自動觸發 IADL Editor 中的更新流程。
-   **排程同步**：定期從 CAD/PLM 系統中拉取數據，檢查更新。

### 7.2 錯誤處理與日誌 (保持不變)

-   整合過程中可能出現數據格式不匹配、API 調用失敗等錯誤，需要有完善的錯誤處理機制。
-   詳細的日誌記錄對於問題診斷和追蹤至關重要。

### 7.3 性能優化 (保持不變)

-   對於大型裝配體和複雜 BOM，數據提取和轉換可能耗時較長，需要考慮非同步處理和批次處理。
-   CAD 模型的 USD 匯出應盡量優化，減少文件大小。

### 7.4 安全性 (保持不變)

-   與 CAD/PLM 系統的連接應使用安全的通訊協定 (HTTPS)。
-   API 訪問應透過認證和授權機制進行保護 (例如 OAuth 2.0 或 API Key)。

## 8. 結論

本 CAD/PLM 系統整合介面規範為 IADL Editor 實現與工程設計數據的無縫連接提供了詳細指導。透過標準化的介面和明確的數據映射，並深度整合 ISA-95 階層、生命週期狀態和豐富數據模型，IDTF V3.5 將能夠高效地將設計階段的資產資訊轉化為可操作的數位分身定義，為工業數位化轉型奠定堅實基礎。

