# IDTF V3.5 - Industrial Digital Twin Framework

**版本**: 3.5.2  
**發布日期**: 2025-10-15  
**作者**: 林志錚 Michael Lin (Chih Cheng Lin) & Manus AI
**組織**: IDTF Consortium

---

## 📖 專案概述

**IDTF (Industrial Digital Twin Framework)** V3.5 是一個全面的工業數位分身框架，整合了設計、部署、運營、維護的完整生命週期管理。本版本 (V3.5.2) 進行了重大的架構性修正與精煉，旨在深化 **ISA-95 標準符合性**、**提升數據模型的豐富性與靈活性**，並**強化資產生命週期的管理能力**。這些更新是基於資深架構師的全面分析以及對核心規範 (IADL/FDL) 的修正建議，為實現更精準、更具互操作性的工業數位分身奠定了堅實的基礎。

### 核心理念

> **Build Once, Use Everywhere**  
> 一次建立工業資產的數位分身，即可在設計、建造、營運、維護等全生命週期中重複使用。

### V3.5.2 最新亮點 (架構精煉) 🆕

- ✨ **深度 ISA-95 整合**: 將 ISA-95 設備階層模型深度整合到 IADL/FDL 規範與核心架構中，確保數據的標準化與互操作性。
- ✨ **多維度資產視圖**: 引入「次要階層」(Logical Groups) 概念，允許從不同維度（如學科、維護、安全）組織和檢視資產。
- ✨ **豐富數據模型**: 大幅擴展 IADL 的核心屬性，涵蓋工程、營運、維護、財務等多個維度，提供更完整的資產數位化描述。
- ✨ **精細化生命週期管理**: 為資產定義與實例引入了更詳細的生命週期狀態模型和事件管理機制，實現從設計到退役的全程追蹤。
- ✨ **強化外部系統整合**: 更新了與 CAD/PLM、ERP/MES 系統的整合規範，支援擴展屬性和生命週期狀態的數據映射與同步。

---

## 📁 專案結構

本專案採用清晰的模組化結構，將所有內容分為 **13 個主要類別**。以下是更新後的文檔結構：

### 01. IDTF Spec（IDTF 規範）
- **內容**: IDTF 框架總覽、核心概念、架構設計、Omniverse 整合
- **文檔**: 
  - `IDTF_Overview_v1.1.md` 🆕
  - `Omniverse_Integration.md`
  - `IADL_FDL_Omniverse_Connector.md`

### 02. IADL Spec（IADL 規範）
- **內容**: Industrial Asset Definition Language 完整規範與修正建議
- **文檔**: 
  - `IADL_Specification_V3.1.md`
  - `IADL_Editor_Usage_Guide.md`
  - `IADL_Integration_with_Plant_Design_Software.md`
  - `IADL_Spec_Revision_Suggestions.md` 🆕 (修正依據)

### 03. IADL Editor Project（IADL 編輯器專案）
- **內容**: IADL 編輯器的完整源碼專案與架構設計
- **技術**: Qt6 跨平台實作
- **文檔**:
  - `IADL_Editor_Complete_Architecture_v1.1.md` 🆕
  - `Factory_Asset_Library_Implementation_v1.1.md` 🆕
  - `CAD_PLM_Integration_Specification_v1.1.md` 🆕
  - `Asset_Lifecycle_Management_v1.1.md` 🆕
  - `ERP_MES_Connector_Specification_v1.1.md` 🆕

### 04. FDL Spec（FDL 規範）
- **內容**: Factory Design Language 完整規範與修正建議
- **文檔**: 
  - `FDL_Specification.md`
  - `FDL_Specification_Whitepaper.md`
  - `FDL_Integration_with_Plant_Design_Software.md`
  - `FDL_Generation_from_Omniverse.md`
  - `FDL_Spec_Revision_Suggestions.md` 🆕 (修正依據)

### 05. FDL Editor Project（FDL 編輯器專案）
- **內容**: Factory Designer 工具專案（預留結構）

### 06. NDH Spec（NDH 規範）
- **內容**: Neutral Data Hub 架構規範
- **文檔**: 包含 NDH 架構、API、高可用性、MCP Server 整合、Connector Agent 架構等。

### 07. NDH Project（NDH 專案）
- **內容**: NDH 實作專案源碼

### 08. White Papers（白皮書）
- **內容**: 包含學術研究、技術白皮書、商業分析，以及新增的 **`IDTF_V3.5_Architectural_Analysis_Report.md`** 🆕。

### 09. Use Cases（應用案例）
- **內容**: 包含 AI Agent 整合應用、PME 整合應用、以及宏齊、台積電、LNG 電廠、GB300 數據中心等實際應用場景。

### 10. Harvatek New Factory（宏齊科技新廠房）
- **內容**: 完整的 IADL/FDL 設計案例。

### 11. Small LNG Power Plant（小型 LNG 發電廠）
- **內容**: 完整的 IADL/FDL 設計案例。

### 12. TSMC FMCS（台積電廠房設施管理系統）
- **內容**: 完整的 IADL/FDL 設計案例。

### 13. GB300 Data Center（GB300 AI 數據中心）
- **內容**: 完整的 IADL/FDL 設計案例。

---

## 🎯 V3.5.2 主要特色 (架構精煉詳解)

### 1. 深度 ISA-95 整合與多維度視圖

IDTF V3.5.2 將 **ISA-95** 作為核心設計原則，並引入**次要階層**概念，實現了更靈活、更標準化的資產管理。

**核心變更**:
- ✅ **主要階層**: 所有資產必須歸屬於 ISA-95 的八級設備階層模型中的一級，確保了資產組織的標準化。
- ✅ **次要階層 (Logical Groups)**: 允許使用者創建任意數量的邏輯分組（如按學科、按維護區域、按安全等級），並將主要階層中的資產**引用**到這些分組中。這提供了超越物理結構的、基於角色的資產視圖。
- ✅ **數據庫與 API 支援**: Factory Asset Library 的數據庫結構和 API 已全面更新，以支援對主要階層和次要階層的高效查詢與管理。

**價值**:
- **互操作性**: 遵循 ISA-95 標準，易於與其他工業系統（如 MES, ERP）整合。
- **靈活性**: 滿足不同部門（如維運、維護、安全）對資產的不同檢視需求。
- **清晰性**: 將物理/功能階層與邏輯分組明確分離，使架構更清晰。

詳見：`03_IADL_Editor_Project/docs/IADL_Editor_Complete_Architecture_v1.1.md`

---

### 2. 豐富數據模型與精細化生命週期管理

IDTF V3.5.2 大幅擴展了 IADL 的數據模型，並引入了詳細的生命週期狀態管理，使數位分身能夠更全面地反映物理資產。

**核心變更**:
- ✅ **擴展核心屬性**: IADL 規範新增了四大類核心屬性：
  - **工程屬性 (Engineering)**: 如設計壓力、材料規格。
  - **營運屬性 (Operational)**: 如操作限制、設定點。
  - **維護屬性 (Maintenance)**: 如維護計畫、平均無故障時間 (MTBF)。
  - **財務屬性 (Financial)**: 如採購成本、折舊資訊。
- ✅ **詳細生命週期狀態**: 為資產定義 (IADL) 和資產實例 (FDL) 分別定義了詳細的生命週期狀態，例如：
  - **IADL 狀態**: `Draft`, `Released`, `Obsolete`
  - **FDL 狀態**: `Ordered`, `Installed`, `Commissioning`, `Running`, `Under_Maintenance`, `Decommissioned`
- ✅ **事件驅動的狀態轉換**: 資產的狀態變更由明確的生命週期事件觸發，這些事件可來自外部系統（如 ERP 的採購訂單狀態、MES 的工單進度）或內部操作。

**價值**:
- **完整數位畫像**: 數位分身不僅包含物理和行為特徵，還包含其業務和營運上下文。
- **自動化流程**: 可根據資產的生命週期狀態自動觸發相關業務流程（如採購、維護）。
- **精準決策**: 基於更豐富的數據和更精確的狀態，做出更優的營運和維護決策。

詳見：`03_IADL_Editor_Project/docs/Asset_Lifecycle_Management_v1.1.md`

---

### 3. 強化外部系統整合

為了支援豐富數據模型和生命週期管理，與外部系統的整合規範也進行了全面升級。

**核心變更**:
- ✅ **CAD/PLM 整合**: 連接器現在能夠讀取和映射資產的 **ISA-95 階層屬性**和**生命週期狀態**，並將其自動填充到 IADL 定義中。
- ✅ **ERP/MES 整合**: 連接器現在能夠處理擴展的**財務、營運、維護屬性**，並能夠根據 ERP/MES 中的事件（如採購訂單、工單狀態）觸發資產實例的生命週期狀態變更。
- ✅ **雙向同步**: 強化了數據的雙向同步能力，確保 IDTF 平台與外部系統之間的數據一致性。

**價值**:
- **數據自動化流動**: 最大限度地減少手動數據輸入，提高效率和準確性。
- **單一事實來源**: 將 IDTF 平台打造成資產全生命週期數據的單一事實來源。
- **跨系統協同**: 實現設計、採購、製造、營運等不同階段的業務流程無縫銜接。

詳見：
- `03_IADL_Editor_Project/docs/CAD_PLM_Integration_Specification_v1.1.md`
- `03_IADL_Editor_Project/docs/ERP_MES_Connector_Specification_v1.1.md`

---

## 🚀 快速開始

### 1. 了解 IDTF 框架
從 `IDTF_V3.5_Technical_Documentation_Overview.md` 和 `08_White_Papers/IDTF_V3.5_Architectural_Analysis_Report.md` 開始，全面了解框架的設計理念與最新架構。

### 2. 學習 IADL 語言
閱讀 `02_IADL_Spec/docs/IADL_Specification_V3.1.md`，並參考 `IADL_Spec_Revision_Suggestions.md` 了解最新的設計思想。

### 3. 學習 FDL 語言
閱讀 `04_FDL_Spec/docs/11_FDL_Specification_Whitepaper.md`，並參考 `FDL_Spec_Revision_Suggestions.md`。

### 4. 瀏覽核心架構文檔
深入研究 `03_IADL_Editor_Project/docs/` 目錄下的最新架構文檔 (v1.1)，理解各個組件的詳細設計。

---

## 🤝 貢獻

歡迎社群成員對 IDTF 框架提出建議、報告問題或貢獻程式碼。請透過 GitHub Issues 或 Pull Requests 與我們聯繫。

## 📜 授權

本專案採用 **Apache 2.0** 授權。詳見 `LICENSE` 文件。

