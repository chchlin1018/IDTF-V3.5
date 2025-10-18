# IDTF V3.7 - 工業數位分身框架

**版本**: 3.7.1  
**發布日期**: 2025年10月18日  
**作者**: 林志錚 Michael Lin (Chih Cheng Lin) & Manus AI  
**組織**: IDTF Consortium  
**授權**: Apache 2.0

---

## 📖 專案概述

**IDTF (Industrial Digital Twin Framework)** 是一個全面的開源工業數位分身框架,整合了資產定義、工廠設計、數據中樞、視覺化和企業系統整合的完整生命週期管理。

### 核心理念

> **一次建立,隨處使用 (Build Once, Use Everywhere)**  
> 一次建立工業資產的數位分身,即可在設計、建造、營運、維護等全生命週期中重複使用。

### 核心價值

IDTF 提供了一套完整的工業數位化轉型解決方案:

- **開放標準**: 基於 ISA-95、OPC UA、MQTT 等工業標準
- **供應商中立**: 避免供應商鎖定,保持技術自主權
- **完整生態**: 從設計工具到執行平台的完整工具鏈
- **企業整合**: 與 MES、ERP、CAD/PLM 等企業系統深度整合

---

## 🏗️ IDTF 整體架構

IDTF 採用分層架構設計,從資產定義到企業管理層實現完整的數據流動:

```
┌─────────────────────────────────────────────────────────┐
│            Level 4: ERP (企業資源規劃)                   │
│         SAP, Oracle, Microsoft Dynamics                 │
└────────────────────┬────────────────────────────────────┘
                     │ (ERP 整合層)
┌────────────────────▼────────────────────────────────────┐
│            Level 3: MES (製造執行系統)                   │
│    AVEVA MES, FactoryTalk, Siemens Opcenter            │
└────────────────────┬────────────────────────────────────┘
                     │ (MES 整合層)
┌────────────────────▼────────────────────────────────────┐
│              IDTF 核心平台                               │
│  ┌──────────────────────────────────────────────────┐   │
│  │  NDH (Neutral Data Hub) - 中立數據中樞          │   │
│  │  - Asset Servant (資產執行時)                   │   │
│  │  - 時序資料庫 (TDengine/InfluxDB/TimescaleDB)  │   │
│  │  - 訊息佇列 (RabbitMQ/Kafka)                    │   │
│  │  - MCP (Multi-Agent Control Plane)             │   │
│  └──────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────┐   │
│  │  IADL (Industrial Asset Definition Language)    │   │
│  │  - 資產定義語言                                  │   │
│  │  - 設備屬性、數據標籤、行為邏輯                 │   │
│  └──────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────┐   │
│  │  FDL (Factory Design Language)                  │   │
│  │  - 工廠設計語言                                  │   │
│  │  - 廠房佈局、資產配置、空間關係                 │   │
│  └──────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────┐   │
│  │  Omniverse Integration                          │   │
│  │  - NVIDIA Omniverse 3D 視覺化整合               │   │
│  │  - USD 格式支援、即時渲染                       │   │
│  └──────────────────────────────────────────────────┘   │
└────────────────────┬────────────────────────────────────┘
                     │ (OPC UA, MQTT, Modbus, SECS/GEM)
┌────────────────────▼────────────────────────────────────┐
│         Level 0-2: 設備與控制層                          │
│         PLC, SCADA, 感測器, 執行器                       │
└─────────────────────────────────────────────────────────┘
```

### 核心組件說明

| 組件 | 功能 | 技術 |
|---|---|---|
| **IADL** | 資產定義語言,描述設備的屬性、數據標籤和行為邏輯 | YAML 格式,支援 JSON Schema 驗證 |
| **FDL** | 工廠設計語言,描述廠房佈局和資產配置 | YAML 格式,支援 ISA-95 階層 |
| **NDH** | 中立數據中樞,提供統一的數據訪問和管理 | Python, FastAPI, 時序資料庫 |
| **Asset Servant** | 資產執行時,連結 3D 模型、即時數據和業務邏輯 | Python, 支援多種時序資料庫 |
| **MCP** | 多代理控制平面,管理 AI Agent 的協作 | MQTT, REST API |
| **Omniverse Connector** | 與 NVIDIA Omniverse 的整合,實現 3D 視覺化 | USD, Omniverse Kit |

---

## 📁 專案結構與文檔說明

IDTF V3.7 包含超過 500 個檔案和 250+ 份 Markdown 文檔。以下是按類別組織的完整文檔說明:

### 📂 00. 分析報告 (Analysis Report)

| 檔案 | 內容簡要 |
|---|---|
| `00_Analysis_Report/README.md` | IDTF V3.5 全面分析報告,包含優勢、挑戰和策略建議 |
| `00_Analysis_Report/Roadmap.md` | IDTF 發展藍圖,包含短期、中期和長期目標 |

### 📂 01. IDTF 規範 (IDTF Spec)

| 檔案 | 內容簡要 |
|---|---|
| `01_IDTF_Spec/docs/IDTF_Introduction.md` | IDTF 框架介紹 (20 頁精簡版),適合新用戶快速了解 |
| `01_IDTF_Spec/docs/01_IDTF_Overview.md` | IDTF 框架完整概述,包含核心概念和架構設計 |
| `01_IDTF_Spec/docs/IDTF_V3.6_MCP_AI_Agent_Extension_Blueprint.md` | MCP 與 AI Agent 擴充設計藍圖 |
| `01_IDTF_Spec/docs/IDTF_Omniverse_Integration_Documentation.md` | IDTF 與 Omniverse 整合文檔 |
| `01_IDTF_Spec/docs/IADL_USD_Integration_Analysis_Report.md` | IADL 與 USD 整合分析報告 |
| `01_IDTF_Spec/docs/IADL_USD_Integration_Proposal.md` | IADL 與 USD 整合提案 |
| `01_IDTF_Spec/docs/IADL_FDL_Documentation.md` | IADL 和 FDL 介紹文檔 (含範例) |

### 📂 02. IADL 規範 (IADL Spec)

| 檔案 | 內容簡要 |
|---|---|
| `02_IADL_Spec/docs/IADL_Specification_v3.2.md` | IADL 語言規範 V3.2,定義資產定義語言的完整語法 |
| `02_IADL_Spec/docs/02_IADL_Specification.md` | IADL 規範原始版本 |
| `02_IADL_Spec/docs/IADL_Changelog.md` | IADL 版本更新記錄 |

### 📂 03. IADL 編輯器專案 (IADL Editor Project)

| 檔案 | 內容簡要 |
|---|---|
| `03_IADL_Editor_Project/README.md` | IADL 編輯器專案說明,基於 Qt6 的跨平台實作 |
| `03_IADL_Editor_Project/docs/IADL_Editor_Complete_Architecture.md` | IADL 編輯器完整架構設計 |
| `03_IADL_Editor_Project/docs/CAD_PLM_Integration_Specification.md` | CAD/PLM 整合規範 |
| `03_IADL_Editor_Project/docs/ERP_MES_Connector_Specification.md` | ERP/MES 連接器規範 |
| `03_IADL_Editor_Project/IADL_SYNTAX_MANUAL.md` | IADL 語法手冊,包含完整的語法說明和範例 |

### 📂 04. FDL 規範 (FDL Spec)

| 檔案 | 內容簡要 |
|---|---|
| `04_FDL_Spec/docs/FDL_Specification_v3.2.md` | FDL 語言規範 V3.2,定義工廠設計語言的完整語法 |
| `04_FDL_Spec/docs/05_FDL_Specification.md` | FDL 規範原始版本 |
| `04_FDL_Spec/docs/11_FDL_Specification_Whitepaper.md` | FDL 白皮書,詳細說明設計理念和應用場景 |
| `04_FDL_Spec/docs/FDL_Changelog.md` | FDL 版本更新記錄 |

### 📂 05. FDL 編輯器專案 (FDL Editor Project)

| 檔案 | 內容簡要 |
|---|---|
| `05_FDL_Editor_Project/README.md` | FDL 編輯器專案說明 (預留結構) |

### 📂 06. NDH 規範 (NDH Spec)

| 檔案 | 內容簡要 |
|---|---|
| `06_NDH_Spec/docs/NDH_Introduction.md` | NDH 介紹文檔,快速了解 NDH 的功能與架構 |
| `06_NDH_Spec/docs/NDH_Architecture.md` | NDH 完整架構設計,包含所有核心組件 |
| `06_NDH_Spec/docs/06_NDH_Architecture.md` | NDH 架構原始版本 |
| `06_NDH_Spec/docs/MCP_Layer_Design.md` | MCP (Multi-Agent Control Plane) 層設計 |
| `06_NDH_Spec/docs/EnergyOptimizerAgent_Implementation.md` | 能源優化 Agent 實作 |
| `06_NDH_Spec/docs/MaintenanceAdvisorAgent_Design.md` | 維護建議 Agent 設計 |
| `06_NDH_Spec/docs/SafetySentinelAgent_Design.md` | 安全監控 Agent 設計 |
| `06_NDH_Spec/docs/USD_Integration_Service_Design.md` | USD 整合服務設計 |
| `06_NDH_Spec/docs/NDH_Pluggable_TSDB_Architecture.md` | NDH 可抽換時序資料庫架構 |
| `06_NDH_Spec/docs/NDH_PI_Historian_Integration.md` | PI System 與 Historian 整合 |
| `06_NDH_Spec/docs/NDH_Asset_Servant_Correct_Definition.md` | Asset Servant 正確定義 |

### 📂 07. NDH 專案 (NDH Project)

| 檔案 | 內容簡要 |
|---|---|
| `07_NDH_Project/README.md` | NDH 實作專案說明,包含 Python 源碼 |
| `07_NDH_Project/docs/README.md` | NDH 專案文檔索引 |

### 📂 08. 白皮書 (White Papers)

| 檔案 | 內容簡要 |
|---|---|
| `08_White_Papers/README.md` | 白皮書索引,包含學術研究、技術和商業分析 |
| `08_White_Papers/technical/IDTF_Whitepaper_V2_Part2_Technical_Architecture.md` | IDTF 技術架構白皮書 |
| `08_White_Papers/technical/NDH_Omniverse_Digital_Twins_Architecture_Whitepaper.md` | NDH 與 Omniverse 數位分身架構白皮書 |

### 📂 09. MES 整合 (MES Integration)

| 檔案 | 內容簡要 |
|---|---|
| `09_MES_Integration/README.md` | MES 整合技術規格,包含 ISA-95 數據模型 |
| `09_MES_Integration/Implementation_Guide.md` | MES 整合實施指南 |
| `09_MES_Integration/NDH_API_Spec.md` | NDH API 規格 (MES 擴展) |
| `09_MES_Integration/Asset_Servant_Update.md` | Asset Servant 更新 (MES 支援) |
| `09_MES_Integration/MES_Connector_Reference.md` | MES 連接器參考實現 |

### 📂 10. ERP 整合 (ERP Integration)

| 檔案 | 內容簡要 |
|---|---|
| `10_ERP_Integration/README.md` | ERP 整合技術規格,包含 SAP、Oracle 整合 |
| `10_ERP_Integration/Implementation_Guide.md` | ERP 整合實施指南 |
| `10_ERP_Integration/NDH_API_Spec.md` | NDH API 規格 (ERP 擴展) |
| `10_ERP_Integration/Asset_Servant_Update.md` | Asset Servant 更新 (ERP 支援 + KPI 引擎) |
| `10_ERP_Integration/ERP_Connector_SAP_Reference.md` | SAP 連接器參考實現 |

### 📂 14. 虛擬 MES (Virtual MES) 🆕

| 檔案 | 內容簡要 |
|---|---|
| `14_Virtual_MES/README.md` | 虛擬化 MES 系統架構,模擬真實 MES 核心功能 |
| `14_Virtual_MES/Implementation_Guide.md` | 虛擬 MES 實施指南,包含部署和配置 |

### 📂 15. 通用 MES SDK (Universal MES SDK) 🆕

| 檔案 | 內容簡要 |
|---|---|
| `15_Universal_MES_SDK/README.md` | 通用 MES SDK 架構,標準化多品牌 MES 整合 |
| `15_Universal_MES_SDK/Implementation_Guide.md` | 通用 MES SDK 實施指南,包含適配器開發 |

### 📂 16. AVEVA MES 連接器 (AVEVA MES NDH Connector) 🆕

| 檔案 | 內容簡要 |
|---|---|
| `16_AVEVA_MES_NDH_Connector/README.md` | AVEVA MES 雙向連接器架構 |
| `16_AVEVA_MES_NDH_Connector/Implementation_Guide.md` | AVEVA MES 連接器實施指南 |

### 📂 17. FactoryTalk MES 連接器 (FactoryTalk MES NDH Connector) 🆕

| 檔案 | 內容簡要 |
|---|---|
| `17_FactoryTalk_MES_NDH_Connector/README.md` | FactoryTalk MES 雙向連接器架構 |
| `17_FactoryTalk_MES_NDH_Connector/Implementation_Guide.md` | FactoryTalk MES 連接器實施指南 |

### 📂 18. 統一 MES 虛擬介面 (Unified MES Virtual Interface) 🆕

| 檔案 | 內容簡要 |
|---|---|
| `18_Unified_MES_Virtual_Interface/README.md` | 統一 MES 虛擬介面架構,整合 AVEVA 和 FactoryTalk 經驗 |
| `18_Unified_MES_Virtual_Interface/Implementation_Guide.md` | 統一虛擬介面實施指南 |

### 📂 19. NDH-MES 整合 (NDH MES Integration) 🆕

| 檔案 | 內容簡要 |
|---|---|
| `19_NDH_MES_Integration/README.md` | NDH-MES 整合架構,將虛擬介面層整合到 NDH |
| `19_NDH_MES_Integration/Deployment_Guide.md` | NDH-MES 整合部署指南 |
| `19_NDH_MES_Integration/Configuration_Guide.md` | NDH-MES 整合配置管理 |

### 📂 應用範例

| 檔案 | 內容簡要 |
|---|---|
| `08_FMCS_Example/README.md` | 半導體廠房 FMCS 設計範例 |
| `10_Harvatek_New_Factory/docs/README.md` | 宏齊科技新廠房完整設計案例 |
| `11_Small_LNG_Power_Plant/docs/README.md` | 小型 LNG 發電廠完整設計案例 |
| `12_TSMC_FMCS/docs/README.md` | 台積電廠房設施管理系統設計案例 |
| `13_GB300_Data_Center/docs/README.md` | GB300 AI 數據中心設計案例 |

### 📂 其他目錄

| 目錄 | 內容簡要 |
|---|---|
| `03_IADL_Spec/` | IADL 規範的重複目錄,內容已整合至 `02_IADL_Spec` |
| `05_NDH_Connector_Agent/` | NDH 連接器代理的早期開發目錄 |
| `09_Use_Cases/` | 各行業的應用案例研究 |
| `10_Examples_Library/` | IADL 和 FDL 的範例庫 |
| `contracts/` | 智能合約與區塊鏈整合(實驗性) |
| `diagrams/` | 專案架構圖原始檔 (e.g., Mermaid, PlantUML) |
| `examples/` | 其他小型程式碼範例 |
| `presentation/` | 專案簡報檔案 |
| `scripts/` | 各類自動化腳本 |
| `specs/` | 其他技術規格文件 |

---

## 📋 版本更新說明

### V3.7.1 (2025-10-18) - README 更新

- **修正**: 補全專案結構說明,新增 `03_IADL_Spec`, `05_NDH_Connector_Agent`, `09_Use_Cases`, `10_Examples_Library`, `contracts`, `diagrams`, `examples`, `presentation`, `scripts`, `specs` 等目錄的說明。
- **優化**: 調整版本號為 3.7.1 以反映 README 的更新。

### V3.7.0 (2025-10-18) 🆕

**主題**: 完整的 MES 整合生態系統

**新增功能**:
- ✨ **通用 MES SDK**: 提供統一的、與具體 MES 品牌無關的 API 介面
- ✨ **統一虛擬介面**: 整合 AVEVA 和 FactoryTalk 經驗的通用架構
- ✨ **AVEVA MES 連接器**: AVEVA MES 雙向連接器參考實現
- ✨ **FactoryTalk MES 連接器**: FactoryTalk MES 雙向連接器參考實現
- ✨ **虛擬化 MES**: 模擬真實 MES 核心功能的輕量級系統
- ✨ **NDH-MES 整合**: 將虛擬介面層整合到 NDH 架構

**核心價值**:
- 降低 90% 的 MES 整合成本
- 避免供應商鎖定
- 支援多品牌 MES 系統
- 提供完整的測試和演示環境

**新增文檔**: 13 個 (6 個新目錄)

---

### V3.6.0 (2025-10-16)

**主題**: MCP 與 AI Agent 擴充

**新增功能**:
- ✨ **MCP (Multi-Agent Control Plane)**: 多代理控制平面
- ✨ **Agent SDK**: 標準化的 Agent 開發工具包
- ✨ **Reference Agents**: 能源優化、維護建議、安全監控三個參考 Agent
- ✨ **IADL-USD 整合**: IADL 與 USD 3D 模型的深度整合

**核心價值**:
- 實現更智能、更自主的工廠運營
- 簡化 AI Agent 開發流程
- 支援多種 Agent 協作模式

---

### V3.5.2 (2025-10-14)

**主題**: 架構精煉與 ISA-95 深度整合

**新增功能**:
- ✨ **深度 ISA-95 整合**: 將 ISA-95 設備階層模型整合到 IADL/FDL
- ✨ **多維度資產視圖**: 引入「次要階層」(Logical Groups) 概念
- ✨ **豐富數據模型**: 擴展 IADL 核心屬性 (工程、營運、維護、財務)
- ✨ **精細化生命週期管理**: 詳細的生命週期狀態模型和事件管理
- ✨ **強化外部系統整合**: 更新 CAD/PLM、ERP/MES 整合規範

**核心價值**:
- 提高互操作性
- 增強靈活性
- 實現更精準的決策

---

### V3.5.0 (2025-10-10)

**主題**: NDH 核心架構與 Omniverse 整合

**新增功能**:
- ✨ **NDH (Neutral Data Hub)**: 中立數據中樞核心架構
- ✨ **Asset Servant**: 資產執行時,連結 3D 模型與即時數據
- ✨ **可抽換時序資料庫**: 支援 TDengine、InfluxDB、TimescaleDB
- ✨ **Omniverse 整合**: 與 NVIDIA Omniverse 的深度整合
- ✨ **PI System 整合**: 支援 AVEVA PI System 和 Historian

**核心價值**:
- 提供統一的數據訪問和管理
- 實現 3D 視覺化與實際設備的即時聯動
- 支援多種時序資料庫,避免供應商鎖定

---

## 🚀 快速開始

### 1. 了解 IDTF 框架
從 `01_IDTF_Spec/docs/IDTF_Introduction.md` 開始,全面了解框架的設計理念與最新架構。

### 2. 學習 IADL 和 FDL
閱讀 `01_IDTF_Spec/docs/IADL_FDL_Documentation.md`,快速了解 IADL 和 FDL 的核心概念。

### 3. 深入 NDH 架構
研究 `06_NDH_Spec/docs/NDH_Introduction.md`,了解 NDH 的功能與架構。

### 4. 體驗 MES 整合
參考 `09_MES_Integration/README.md` 和 `14_Virtual_MES/README.md`,了解 MES 整合方案。

### 5. 查看應用範例
瀏覽 `08_FMCS_Example`、`10_Harvatek_New_Factory` 等目錄,學習實際應用案例。

---

## 🤝 貢獻

歡迎社群成員對 IDTF 框架提出建議、報告問題或貢獻程式碼。請透過 GitHub Issues 或 Pull Requests 與我們聯繫。

## 📜 授權

本專案採用 **Apache 2.0** 授權。詳見 `LICENSE` 文件。

---

## 🔗 相關連結

- **GitHub 倉庫**: https://github.com/chchlin1018/IDTF-V3.5
- **官方網站**: (待建立)
- **技術文檔**: https://github.com/chchlin1018/IDTF-V3.5/tree/master
- **問題回報**: https://github.com/chchlin1018/IDTF-V3.5/issues

---

**最後更新**: 2025年10月18日  
**維護者**: IDTF Consortium

