# IDTF V3.5 技術文件總覽

**版本**: 1.0
**日期**: 2025年10月14日

## 1. 簡介

歡迎來到 **IDTF (Industrial Digital Twin Framework) V3.5** 的技術文件中心。本文件旨在作為一個中心入口，引導開發者、架構師和技術決策者快速找到所需的技術規格、白皮書、設計文檔和部署指南。

IDTF V3.5 是一個開放、中立、可擴展的工業數位分身框架，其核心目標是打通 OT 與 IT 的數據壁壘，實現從物理世界到虛擬世界的高保真、即時、雙向同步。

## 2. 整體架構

IDTF V3.5 的架構圍繞四大核心組件構建，形成一個完整的從設計、部署到運維的閉環生態系統。

![IDTF V3.5 Architecture](./diagrams/idtf_v3.5_architecture.png)*

### 核心組件

| 組件 | 全名 | 核心職責 |
|---|---|---|
| **IADL** | Industrial Asset Description Language | **資產建模**: 定義資產的數據模型、3D 幾何與行為邏輯 (資產的「藍圖」)。 |
| **FDL** | Factory Design Language | **工廠設計**: 以聲明式語言描述工廠中資產的佈局、數量、位置和關係 (工廠的「佈局圖」)。 |
| **NDH** | Neutral Data Hub | **數據中樞**: 負責即時數據採集、處理、存儲，並管理 Asset Servants 的生命週期 (數位分身的「執行時」)。 |
| **Omniverse Connector** | - | **可視化與協同**: 將 NDH 中的數位分身狀態即時同步到 NVIDIA Omniverse，實現高保真渲染和多用戶協同。 |

## 3. 核心規格與白皮書

### 3.1 綜合技術白皮書

這些文件提供了對 IDTF V3.5 架構的全面性、高層次的概述。

- **[NDH、Omniverse 與數位分身整合架構白皮書](./08_White_Papers/technical/NDH_Omniverse_Digital_Twins_Architecture_Whitepaper.md)**
  - **摘要**: 深入探討 NDH、Asset Servants、Omniverse 和 Digital Twins 四大核心組件的整合機制，是理解 IDTF 執行時架構的**必讀文件**。
  - **包含內容**: 分散式部署、故障轉移、API 規範、即時同步機制。

- **[IDTF V3.5 綜合技術論文](./08_White_Papers/research/idtf_comprehensive_technical_paper.md)**
  - **摘要**: 從研究角度全面闡述 IDTF V3.5 的設計理念、技術創新和相較於行業內其他方案的優勢。

### 3.2 IADL (資產定義語言)

- **[IADL 規格書](./02_IADL_Spec/docs/02_IADL_Specification.md)**
  - **摘要**: IADL 的官方語法規範，詳細定義了數據標籤、幾何模型、行為腳本等核心元素。

### 3.3 FDL (工廠設計語言)

- **[FDL 規格設計白皮書](./04_FDL_Spec/docs/11_FDL_Specification_Whitepaper.md)**
  - **摘要**: FDL 的官方語法規範與設計指南，包含多種廠房的設計模式和最佳實踐。
  - **包含內容**: 分散式部署的 Worker 分配提示 (`worker_assignment hints`)。

### 3.4 NDH (中立數據中樞)

- **[NDH 架構文檔](./06_NDH_Spec/docs/06_NDH_Architecture.md)**
  - **摘要**: 描述 NDH 的內部微服務架構、數據流和核心組件。

- **[NDH 可插拔時序數據庫架構](./06_NDH_Spec/docs/NDH_Pluggable_TSDB_Architecture.md)**
  - **摘要**: 闡述 NDH 如何支援多種時序數據庫 (InfluxDB, TDengine 等) 的可插拔設計。

## 4. 部署與整合

### 4.1 部署指南

- **[NDH 叢集部署指南](./06_NDH_Spec/docs/NDH_Cluster_Deployment_Guide.md)**
  - **摘要**: 在 Kubernetes 環境中部署高可用、可擴展的 NDH 叢集的詳細步驟和配置建議。

### 4.2 整合指南

- **[IADL、FDL 與 NDH 互動指南](./08_White_Papers/technical/IADL_FDL_NDH_Interaction_Guide.md)**
  - **摘要**: 說明三大核心組件如何協同工作，從資產定義、工廠佈局到最終執行時實例化的完整流程。

## 5. 專案與源代碼

- **[IDTF V3.5 專案根目錄](./)**
  - 包含所有規格、文檔、白皮書和源代碼。

---


## 最新更新 (2025年10月14日)

### NDH 高可用性與災難復原機制

為確保 NDH 在生產環境中的穩定性和可靠性，我們新增了完整的高可用性與災難復原機制文件：

- **[NDH 高可用性與災難復原詳細文件](./06_NDH_Spec/docs/NDH_High_Availability_and_Disaster_Recovery.md)**
  - 鏡像備援機制（Master/Worker 節點、數據庫層）
  - 系統快照與復原機制（一致性快照、增量復原）
  - 停電保護機制（UPS 整合、優雅關機、快速啟動）
  - 故障檢測與自動恢復（秒級檢測、分鐘級恢復）
  - 災難復原計畫（RTO < 15分鐘、RPO < 5分鐘）
  - 監控與告警（Prometheus、Grafana、多通道告警）

**核心特性**：
- ✅ **99.9%+ 系統可用性**：年度停機時間 < 8.76 小時
- ✅ **無單點故障**：所有關鍵組件均採用冗餘部署
- ✅ **自動故障轉移**：Master 節點 < 10秒、Worker 節點 < 60秒
- ✅ **一致性快照**：不停機建立系統狀態快照（暫停寫入 < 5秒）
- ✅ **快速復原**：從停電到系統完全可用 < 5分鐘
- ✅ **數據零丟失**：透過 WAL 重放和增量復原機制

這些機制使 NDH 成為一個**安全、穩健、可靠**的工業級數位分身平台。
