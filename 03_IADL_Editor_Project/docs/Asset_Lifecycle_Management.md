# 資產生命週期管理

**版本**: 1.1  
**日期**: 2025-10-15  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)

---

## 1. 概述

**資產生命週期管理 (Asset Lifecycle Management, ALM)** 是指對資產從概念、設計、採購、製造、部署、運營、維護直至最終退役的整個過程進行系統化管理。在工業數位轉型背景下，有效的 ALM 對於提高資產效率、降低運營成本、延長資產壽命並確保合規性至關重要。IDTF V3.5 透過整合 IADL Editor、NDH Runtime 和 Factory Asset Library，提供了一個全面的數位化 ALM 解決方案，確保資產數據在整個生命週期中的一致性、可追溯性和可操作性。本次更新將特別強調對 **IADL 和 FDL Spec 中定義的更詳細的生命週期狀態和事件管理**的支援。

## 2. 資產生命週期階段與狀態

資產的生命週期通常可劃分為以下主要階段，並在每個階段中定義更細緻的狀態和事件：

| 階段名稱 | 描述 | IDTF V3.5 關鍵活動與狀態 |
|:---|:---|:---|
| **設計與工程** | 資產的概念化、詳細設計、規格定義和工程分析。 | CAD/PLM 整合、IADL 資產定義創建、行為與控制邏輯設計。 **IADL 資產定義狀態**：`Draft` (草稿), `Design_In_Progress` (設計中), `Under_Review` (審閱中), `Approved` (已核准), `Released` (已發布), `Obsolete` (已淘汰)。 |
| **採購與製造** | 根據設計規格採購組件、製造資產、進行品質檢測和組裝。 | ERP 系統整合、供應商資訊管理、BOM 追溯、製造數據關聯。 **FDL 資產實例狀態**：`Ordered` (已訂購), `In_Production` (生產中), `Quality_Check` (品質檢測), `Ready_For_Shipment` (待出貨)。 |
| **部署與調試** | 資產的安裝、配置、測試和與現有系統的整合。 | FDL 資產實例創建、現場數據採集、調試報告記錄。 **FDL 資產實例狀態**：`Shipped` (已出貨), `Received` (已接收), `Installed` (已安裝), `Commissioning` (調試中), `Ready_For_Operation` (準備運營)。 |
| **運營與維護** | 資產的日常運行、性能監控、預防性維護、故障診斷和維修。 | NDH Runtime 數據採集、即時監控、預測性維護、維護記錄管理。 **FDL 資產實例運營狀態**：`Running` (運行中), `Stopped` (已停止), `Standby` (待機), `Under_Maintenance` (維護中), `Faulted` (故障)。 |
| **退役與報廢** | 資產達到使用壽命或不再需要時的拆除、回收或安全處置。 | 退役流程記錄、數據歸檔、資產狀態更新。 **FDL 資產實例狀態**：`Decommissioned` (已退役), `Scrapped` (已報廢), `Archived` (已歸檔)。 |

## 3. IDTF V3.5 中的資產生命週期管理

IDTF V3.5 透過其核心組件和整合能力，在資產的每個生命週期階段提供全面的數位化支援，並強調狀態轉換和事件觸發。

### 3.1 設計與工程 (Design & Engineering)

此階段的重點是將工程設計數據轉化為標準化的 IADL 資產定義，並定義其生命週期行為。

*   **CAD/PLM 整合**：IADL Editor 直接從 CAD 系統（如 Autodesk Inventor, SolidWorks）提取 3D 幾何模型，並從 PLM 系統（如 Siemens Teamcenter, PTC Windchill）獲取物料清單 (BOM)、技術規格、零件屬性及相關文件。這些數據自動映射並生成 IADL 資產定義的初始草稿，並可從 PLM 同步其初始的 **`lifecycle_status`** (例如 `Design_In_Progress`)。
*   **IADL Editor**：工程師在 IADL Editor 中對自動生成的 IADL 定義進行補充，包括定義數據標籤（感測器、執行器）、行為腳本和控制邏輯。同時，IADL Editor 允許定義資產類型的生命週期狀態模型，包括各個狀態（如 `Draft`, `Released`, `Obsolete`）及其之間的**合法轉換規則**和**觸發事件**。
*   **版本控制**：Factory Asset Library 對 IADL 資產定義實施嚴格的版本控制，記錄每次變更的作者、時間和內容，確保設計演進的可追溯性。每次狀態轉換都可視為一個重要的版本里程碑。

### 3.2 採購與製造 (Procurement & Manufacturing)

此階段將設計數據與供應鏈和製造執行數據關聯起來，並追蹤資產實例的製造狀態。

*   **ERP 系統整合**：IADL Editor 與 ERP 系統（如 SAP ERP）整合，獲取供應商資訊、採購訂單、物料主數據等。這些資訊用於豐富 IADL 資產定義中的 `vendor_info` 和 `procurement_info` 字段，提供資產的來源和成本追溯。資產實例的 **`Ordered`** 狀態可由 ERP 系統的採購訂單狀態觸發。
*   **MES 系統整合**：IADL Editor 從 MES 系統（如 Siemens Opcenter）獲取製程參數、品質標準和生產配方。這些數據被整合到 IADL 資產定義的 `parameters` 和 `quality_standards` 中，為製造過程提供數位化上下文。資產實例的 **`In_Production`** 和 **`Quality_Check`** 狀態可由 MES 系統的工單進度或品質檢測結果觸發。
*   **BOM 追溯**：透過 PLM 整合獲取的 BOM 資訊，可以追溯資產的組成部件及其供應商，為後續的維護和備件管理提供基礎。

### 3.3 部署與調試 (Deployment & Commissioning)

此階段將數位資產定義與實際物理資產建立連接，並追蹤其部署進度。

*   **FDL 資產實例創建**：在 IADL Editor 或 FDL Editor 中，根據 IADL 資產定義創建 FDL 資產實例，並指定其在工廠中的物理位置、序列號等唯一識別資訊。資產實例的初始狀態為 **`Shipped`** 或 **`Received`**。
*   **NDH Runtime 數據採集**：NDH Connector Agent 負責連接現場設備（PLC, SCADA, 感測器），採集即時數據，並將其與 Factory Asset Library 中的資產實例關聯。這使得數位分身能夠反映物理資產的真實狀態。資產實例的 **`Installed`** 和 **`Commissioning`** 狀態可透過現場安裝和調試的事件觸發。
*   **調試記錄**：在調試過程中產生的配置參數、測試報告等文件，可以透過 NDH Document Server 儲存並與資產實例關聯，確保調試過程的可追溯性。調試完成後，資產實例狀態轉換為 **`Ready_For_Operation`**。

### 3.4 運營與維護 (Operation & Maintenance)

此階段是資產生命週期中最長的階段，IDTF V3.5 提供強大的監控和維護管理能力，並細化運營狀態和維護事件。

*   **即時監控**：NDH Runtime 持續採集資產的即時數據，並儲存到 InfluxDB。IADL Editor 的 Runtime Mode 提供直觀的儀表板，顯示資產的即時狀態、性能指標和警報。資產實例的 **`operational_status`** (如 `Running`, `Stopped`, `Standby`, `Faulted`) 會根據即時數據和預設規則自動更新。
*   **預測性維護**：透過分析歷史數據和即時數據，結合 AI/ML 模型，預測資產的潛在故障，並觸發預防性維護任務。這些任務會觸發資產實例狀態從 `Running` 轉換到 **`Under_Maintenance`**。
*   **維護記錄管理**：Factory Asset Library 儲存所有維護活動的記錄，包括預防性維護、糾正性維護和檢查。這些記錄包含維護人員、時間、內容、更換部件等詳細資訊，為資產的健康評估和維護策略優化提供依據。每次維護活動都可記錄為一個 **`lifecycle_event`**，並可能觸發資產狀態的變更。
*   **文件訪問**：維護人員可以透過 IADL Editor 快速訪問 NDH Document Server 中儲存的操作手冊、維護指南、電氣圖紙等相關文件。

### 3.5 退役與報廢 (Decommissioning & Disposal)

此階段確保資產的生命週期得到完整記錄和妥善處理，並更新最終狀態。

*   **狀態更新**：資產在 Factory Asset Library 中的生命週期狀態被更新為 **`Decommissioned`** (已退役) 或 **`Scrapped`** (已報廢)。這些狀態轉換通常由人工審批流程觸發。
*   **數據歸檔**：資產的歷史數據、維護記錄和所有相關文件被歸檔，以滿足合規性要求和未來分析需求。歸檔後的資產狀態可標記為 **`Archived`**。
*   **環境合規**：記錄資產的拆除和處置方式，確保符合環保法規和企業社會責任。

## 4. 關鍵組件協同工作

資產生命週期管理在 IDTF V3.5 中是透過多個核心組件的緊密協同來實現的：

*   **IADL Editor (Asset Designer)**：作為資產定義和實例的創建、編輯和管理中心，連接外部 CAD/PLM、ERP、MES 系統，並將數據整合到 Factory Asset Library。它還負責定義資產類型的**生命週期狀態模型和轉換規則**。
*   **Factory Asset Library**：作為資產定義、實例、維護記錄和文件連結的集中式儲存庫，支援版本控制和高效查詢。它儲存了資產定義的 `lifecycle_status` 和資產實例的 `operational_status`，並記錄所有 **`lifecycle_events`**。
*   **NDH Runtime**：負責數據採集、即時數據處理和與 Factory Asset Library 的交互，為數位分身應用提供底層數據支援。它監控資產的即時數據，並根據預設規則觸發資產實例的 `operational_status` 變更。
*   **NDH Document Server**：集中管理所有與資產相關的文件，提供版本控制和安全訪問，確保在資產生命週期的任何階段都能快速獲取所需文件。

## 5. 結論

IDTF V3.5 的資產生命週期管理框架提供了一個從設計到退役的全面、數位化解決方案。透過將工程設計、供應鏈、製造執行和運營維護數據整合到統一的數位分身平台中，並引入更為細緻的生命週期狀態和事件管理，企業可以實現資產數據的單一事實來源，優化資產性能，降低風險，並為智慧工廠的運營奠定堅實基礎。這將極大地提升資產管理的精確性、自動化程度和決策支援能力。
