# SCADA HMI 與 IDTF/NDH 整合技術說明

**文件編號**: IDTF-V3.5-06-02
**版本**: 1.2
**日期**: 2025-10-16
**作者**: Manus AI

---

## 1. 簡介

本文件旨在探討如何將 IDTF (Industrial Digital Twin Framework) 中的 NDH (Neutral Data Hub) 資產 IoT Tag Source Mapping 概念，與主流的 SCADA HMI 設計工具（如 Aveva Development Studio 和 Ignition SCADA）進行有效整合。數位分身技術的應用，要求底層的資產數據能夠無縫地對映到操作層面的可視化和控制介面。本文將比較這兩種 SCADA 平台在 SDK/API 和開發環境方面的異同，並闡述實現自動化 Tag 設計導出的可行性與方法。

## 2. Aveva Development Studio 的 SDK/API 與開發環境

Aveva Development Studio 及其相關產品（如 System Platform、InTouch）提供了一個整合的工程環境，用於開發監控 SCADA、HMI 和 MES 應用。其整合能力主要透過以下 SDK/API 實現：

*   **GRAccess API**: 這是 Aveva System Platform 最核心的 API 之一，允許開發者透過程式碼存取和操作本地及遠端 Galaxy 的配置。Galaxy 是 Aveva System Platform 的核心數據庫，包含了所有的物件模板 (Object Templates)、實例 (Instances) 和屬性 (Attributes)，這些是 HMI 中 Tag 定義的基礎。GRAccess API 使得程式化地讀取、修改和創建這些配置成為可能，對於自動化 Tag 設計導出至關重要。
*   **AF SDK (Asset Framework SDK)**: 主要用於與 Aveva PI System 的 Asset Framework 互動，管理資產模型和時間序列數據。雖然不直接用於 HMI 配置，但對於理解和提取資產的結構化數據非常有用，可以作為 IADL 資訊來源的補充。
*   **AVEVA OMI SDK**: 提供了對 OMI (Operations Management Interface) 組件 API 和命名空間的程式化存取，主要用於 HMI 和視覺化組件的擴展和互動。

**開發環境**: Aveva Development Studio 是一個功能豐富的整合開發環境，提供可視化設計工具、物件瀏覽器和配置介面。開發者通常使用 .NET 語言（如 C#）結合其 SDK 進行客製化開發。

## 3. Ignition SCADA 的 SDK/API 與開發環境

Ignition SCADA 以其開放性、跨平台特性和強大的模組化架構而聞名。它提供了多種 SDK/API 和靈活的開發環境：

*   **Ignition Module SDK (Java)**: 這是 Ignition 最核心的擴展機制，允許開發者使用 Java 語言創建自定義模組，以擴展 Ignition 的核心功能。這包括開發新的 Gateway 服務、Client 模組、UI 組件、數據源連接器等。Inductive Automation 提供了詳細的 **Ignition SDK Programmer's Guide** 和豐富的 GitHub 範例，極大地降低了開發門檻。
*   **REST API (Python)**: Ignition 內建的 WebDev 模組允許開發者使用 Python 編寫 RESTful API 服務。這些 API 可以用於遠端配置 Ignition 專案、讀取和寫入 Tag 數據、觸發事件等。其基於 OpenAPI 規範的設計確保了良好的文件和外部整合能力。
*   **Scripting API (Python)**: Ignition 在其 Gateway、Client 和 Designer 環境中都深度整合了 Python 腳本功能。開發者可以直接使用 Python 腳本來存取和操作 Ignition 的內部物件、Tag、數據庫和 UI 組件。此外，`ignition-api` Python 套件提供了程式碼自動完成功能，提升了開發效率。

**開發環境**: Ignition 的主要開發工具是 **Ignition Designer**，這是一個功能強大的 IDE，提供可視化 HMI/SCADA 畫面設計器、Tag 瀏覽器和編輯器、數據庫查詢工具和內建的 Python 腳本編輯器。對於 Java 模組開發，通常使用標準的 Java IDE (如 IntelliJ IDEA) 結合 Maven/Gradle。

## 4. Aveva Development Studio 與 Ignition SCADA 的比較

下表總結了 Aveva Development Studio 和 Ignition SCADA 在 SDK/API 和開發環境方面的主要異同：

| 特性             | Aveva Development Studio (System Platform)                                | Ignition SCADA                                                          |
|:-----------------|:--------------------------------------------------------------------------|:------------------------------------------------------------------------|
| **核心 SDK/API** | GRAccess API (.NET), AF SDK (.NET), OMI SDK (.NET)                        | Ignition Module SDK (Java), REST API (Python), Scripting API (Python)   |
| **開發語言**     | 主要為 .NET 語言 (C#)                                                     | Java (模組開發), Python (腳本, REST API)                                |
| **開放性**       | 專有 SDK，需要特定授權和環境，文件通常受限於客戶/合作夥伴入口網站。     | 更為開放，SDK 文件和範例公開，社群活躍。                                |
| **數據模型存取** | GRAccess API 直接存取 Galaxy 物件模型 (Templates, Instances, Attributes)。 | Scripting API 和 REST API 存取 Tag Provider 和 Tag 結構。             |
| **自動化 Tag 導出** | 可透過 GRAccess API 程式化實現。                                         | 可透過 Module SDK (Java) 或 Scripting API/REST API (Python) 程式化實現。|
| **開發環境**     | 整合的 Development Studio，提供可視化設計和配置介面。                     | Ignition Designer (HMI/SCADA 設計, Tag 管理, 腳本編輯), 標準 Java IDE (模組開發)。|
| **跨平台**       | 主要基於 Windows 環境。                                                   | 跨平台 (Windows, Linux, macOS)，Gateway 和 Designer 均可運行。         |

## 5. Asset IoT Tag Source Mapping 與 SCADA HMI 設計工具的整合

將 IDTF/NDH 中的 Asset IoT Tag Source Mapping 與 SCADA HMI 設計工具結合，旨在實現資產數據的自動化同步和可視化。無論是 Aveva 還是 Ignition，其整合策略都圍繞著 NDH 作為統一數據源和標準化介面展開。

### 5.1. NDH 作為統一數據源

NDH 整合了來自各種 OT/IT 系統的數據，並透過 Asset Tag Instance 機制將抽象的 `tag_id` 映射到後端時序數據庫中的實際數據點。SCADA HMI 設計工具無需直接連接多個底層數據源，只需連接到 NDH 暴露的標準介面。

### 5.2. 標準化數據接口

NDH 可以透過以下標準協議暴露其整合後的資產數據，供 SCADA HMI 工具消費：

*   **OPC UA Server**: NDH 可以將其內部資產模型和所有 Asset Tag Instance 提供的數據，以 OPC UA 節點空間的形式暴露。Aveva 和 Ignition 都對 OPC UA 有強大的原生支援，可以直接瀏覽和訂閱這些 Tag。
*   **MQTT Broker**: NDH 內部使用 MQTT 作為事件總線。重要的資產數據更新可以發布到特定的 MQTT Topic，SCADA HMI 工具可以訂閱這些 Topic 獲取實時數據。
*   **RESTful API / GraphQL**: NDH 可以提供這些接口，用於更靈活的數據查詢或配置，儘管 HMI 工具直接使用較少，但可用於 HMI 後端服務。

### 5.3. 自動化 Tag 設計導出 (IADL Tag Design)

實現從 SCADA HMI 設計工具自動導出 IADL Tag Design，或從其輸出檔案自動解析建立 IADL Tag Design，是提高效率和確保數據一致性的關鍵。這兩種方法各有優缺點：

#### 5.3.1. 開發 SCADA HMI 插件自動導出 IADL Tag Design

*   **優點**: 最理想的整合方式，插件直接存取 SCADA 內部物件模型，程式碼層級轉換減少錯誤，提供最佳使用者體驗和數據一致性。
*   **挑戰**: 需要深入了解 SCADA 平台的 SDK/API (例如 Aveva 的 GRAccess API 或 Ignition 的 Module SDK)，開發複雜度較高，且可能受限於版本相容性。
*   **實作概要**: 透過 SCADA 平台的 SDK 存取其 Tag 定義（例如 Aveva 的 `Attributes` 或 Ignition 的 `Tags`），將其映射為 IADL 的 `data_tags` 和 `properties`，並生成 IADL 文件。

#### 5.3.2. 自動解析 SCADA HMI 輸出檔案建立 IADL Tag Design

*   **優點**: 無需 SCADA 平台的 SDK，降低開發門檻，可以使用任何程式語言開發解析器，工具獨立性高。
*   **挑戰**: 高度依賴 SCADA 平台的輸出檔案格式，格式變化可能導致解析器失效；導出的檔案可能不包含所有必要的元數據；需要手動導出步驟。
*   **實作概要**: 識別 SCADA HMI 可以導出的包含 Tag 定義的檔案（例如 Aveva 的 `Galaxy` 匯出檔案或 Ignition 的 Tag 匯出 JSON/XML），開發解析腳本提取關鍵資訊，然後映射到 IADL 結構並生成 IADL 文件。

### 5.4. Asset Instance Hierarchy 與 SCADA HMI 的整合

NDH 中的 Asset Instance Hierarchy (資產實例階層) 是透過 FDL (Factory Design Language) 定義的，它反映了工廠的物理和邏輯組織結構（例如 ISA-95 階層）。這個階層對於 SCADA HMI 系統的整合至關重要：

*   **上下文感知導航**：SCADA HMI 可以利用 NDH 提供的 Asset Instance Hierarchy，實現基於工廠區域、生產線或設備組的上下文感知導航。操作員可以從高層次的工廠概覽鑽取到特定的設備，並查看其相關數據和控制介面。
*   **統一的資產視圖**：無論底層數據來自何種設備或系統，NDH 都將其整合到統一的 Asset Instance Hierarchy 中。SCADA HMI 系統只需連接到 NDH，即可獲得一個完整且一致的資產視圖，簡化了 HMI 的開發和維護。
*   **分散式物件管理**：NDH 中的 Asset Instance 採用類似 CORBA (Common Object Request Broker Architecture) 的分散式物件管理架構，在分散式服務器叢集中被管理。這意味著 SCADA HMI 系統可以透過標準化的介面與這些分散式 Asset Instance 互動，而無需關心它們實際運行在哪個伺服器節點上。這種透明性確保了 HMI 系統在高併發和大規模部署下的穩定性和可擴展性。
*   **FDL 與 Layout Mapping**：FDL 作為中立的工廠佈局描述語言，可以對映不同工廠 Layout 設計軟體所產生的佈局。這使得 SCADA HMI 系統能夠基於 FDL 定義的 Asset Instance Hierarchy，在 HMI 畫面中精確地呈現資產的物理位置和相互關係，實現數位分身在操作層面的視覺化。FDL 的這種對映能力是構建完整數位分身的核心基礎，它將設計階段的資訊（佈局、資產類型）與運行階段的資訊（實時數據、狀態）無縫連接起來。

## 6. 結論

無論是 Aveva Development Studio 還是 Ignition SCADA，都提供了足夠的 SDK/API 和開發工具來實現與 IDTF/NDH 架構的深度整合。其中，**開發插件直接從 SCADA 平台導出 IADL Tag Design 是最推薦的方法**，它能提供最緊密的整合和最高的數據一致性。對於 Aveva，GRAccess API 是實現此功能的關鍵；對於 Ignition，其 Module SDK 或強大的 Python Scripting API 和 REST API 提供了多種靈活的實作途徑。透過這些自動化工具，可以有效地將 SCADA HMI 的操作層數據與數位分身的數據模型進行同步，為工業數位化轉型提供堅實的基礎。

