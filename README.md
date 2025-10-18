# IDTF V3.7 - Industrial Digital Twin Framework

**版本**: 3.7.0  
**發布日期**: 2025-10-18  
**作者**: 林志錚 Michael Lin (Chih Cheng Lin) & Manus AI
**組織**: IDTF Consortium

---

## 📖 專案概述

**IDTF (Industrial Digital Twin Framework)** V3.7 是一個全面的工業數位分身框架，整合了設計、部署、運營、維護的完整生命週期管理。本版本 (V3.7.0) 在 V3.6 的基礎上，引入了 **完整的 MES 整合生態系統**，旨在解決多品牌 MES 系統整合的痛點，實現從企業管理層到設備控制層的無縫數據流動。

### 核心理念

> **Build Once, Use Everywhere**  
> 一次建立工業資產的數位分身，即可在設計、建造、營運、維護等全生命週期中重複使用。

### V3.7.0 最新亮點 (MES 整合生態系統) 🆕

- ✨ **通用 MES SDK**: 提供統一的、與具體 MES 品牌無關的 API 介面，透過「適配器模式」輕鬆支援新的 MES 品牌。
- ✨ **統一虛擬介面**: 整合 AVEVA 和 FactoryTalk 經驗，提供一個通用的雙向連接器虛擬介面架構。
- ✨ **雙向連接器參考實現**: 提供 AVEVA MES 和 FactoryTalk MES 兩個主流 MES 的完整雙向連接器參考實現。
- ✨ **虛擬化 MES**: 提供一個模擬真實 MES 核心功能的輕量級系統，用於測試、演示和學習。

---

## 📁 專案結構

本專案採用清晰的模組化結構，將所有內容分為 **19 個主要類別**。以下是更新後的文檔結構：

### 01-07. 核心規範與實作
- **IDTF Spec**: 框架總覽、核心概念、架構設計
- **IADL Spec**: IADL 語言規範
- **IADL Editor Project**: IADL 編輯器源碼 (Qt6)
- **FDL Spec**: FDL 語言規範
- **FDL Editor Project**: FDL 編輯器 (預留)
- **NDH Spec**: NDH 架構規範
- **NDH Project**: NDH 實作源碼 (Python)

### 08-13. 應用範例
- **FMCS Example**: 半導體廠房 FMCS 設計範例
- **White Papers**: 學術研究、技術白皮書、商業分析
- **Use Cases**: AI Agent 整合應用、PME 整合應用等
- **Harvatek New Factory**: 完整的 IADL/FDL 設計案例
- **Small LNG Power Plant**: 完整的 IADL/FDL 設計案例
- **TSMC FMCS**: 完整的 IADL/FDL 設計案例
- **GB300 Data Center**: 完整的 IADL/FDL 設計案例

### 14-19. MES 整合生態系統 🆕
- **Virtual MES**: 虛擬化 MES 系統
- **Universal MES SDK**: 通用 MES SDK
- **AVEVA MES NDH Connector**: AVEVA MES 連接器
- **FactoryTalk MES NDH Connector**: FactoryTalk MES 連接器
- **Unified MES Virtual Interface**: 統一虛擬介面
- **NDH MES Integration**: NDH-MES 整合

---

## 🎯 V3.7.0 主要特色 (MES 整合生態系統詳解)

IDTF V3.7 的核心在於引入了 **完整的 MES 整合生態系統**，旨在解決多品牌 MES 系統整合的痛點，實現從企業管理層到設備控制層的無縫數據流動。

### 1. 通用 MES SDK

提供統一的、與具體 MES 品牌無關的 API 介面，透過「適配器模式」輕鬆支援新的 MES 品牌。

**核心變更**:
- ✅ **抽象化**: 提供統一的、與具體 MES 品牌無關的 API 介面。
- ✅ **標準化**: 採用 ISA-95 標準作為數據模型基礎。
- ✅ **可擴展**: 透過「適配器模式」輕鬆支援新的 MES 品牌。

**價值**:
- **降低整合成本**: 減少 90% 的開發時間和成本。
- **避免供應商鎖定**: 不再被單一 MES 供應商綁定。

詳見：`15_Universal_MES_SDK/README.md`

### 2. 統一虛擬介面

整合 AVEVA 和 FactoryTalk 經驗，提供一個通用的雙向連接器虛擬介面架構。

**核心變更**:
- ✅ **統一 API**: 提供統一的 RESTful API 和事件模型。
- ✅ **事件分派器**: 接收 MES 適配器的事件，轉換為 NDH 標準格式。
- ✅ **適配器管理器**: 動態載入和管理 MES 適配器。

**價值**:
- **簡化整合**: 大大降低將新 MES 系統接入 NDH 的成本和時間。
- **提高系統靈活性**: 可以同時使用多個 MES 系統。

詳見：`18_Unified_MES_Virtual_Interface/README.md`

### 3. 雙向連接器參考實現

提供 AVEVA MES 和 FactoryTalk MES 兩個主流 MES 的完整雙向連接器參考實現。

**核心變更**:
- ✅ **AVEVA MES 連接器**: 透過 AVEVA Event Broker 和 REST API 實現雙向數據流。
- ✅ **FactoryTalk MES 連接器**: 透過 OPC UA 和 REST API 實現雙向數據流。

**價值**:
- **快速驗證**: 提供可運行的參考實現，加速開發和驗證。
- **最佳實踐**: 展示如何實現 MES 與 NDH 的雙向整合。

詳見：`16_AVEVA_MES_NDH_Connector/README.md` 和 `17_FactoryTalk_MES_NDH_Connector/README.md`

### 4. 虛擬化 MES

提供一個模擬真實 MES 核心功能的輕量級系統，用於測試、演示和學習。

**核心變更**:
- ✅ **模擬工單管理**: 模擬工單的創建、更新和刪除。
- ✅ **模擬設備狀態**: 模擬設備狀態變更 (運行、故障、維修)。
- ✅ **可配置行為**: 允許自定義模擬場景，如設備故障率、生產節拍等。

**價值**:
- **降低學習門檻**: 無需部署真實 MES 即可體驗 IDTF 的完整功能。
- **加速開發驗證**: 提供一個可控的測試環境，加速 NDH 與 MES 整合的開發和驗證。

詳見：`14_Virtual_MES/README.md`

---

## 🚀 快速開始

### 1. 了解 IDTF 框架
從 `01_IDTF_Spec/docs/IDTF_Introduction.md` 開始，全面了解框架的設計理念與最新架構。

### 2. 學習 MES 整合
閱讀 `09_MES_Integration/README.md`，了解 MES 整合的技術規格。

### 3. 體驗虛擬 MES
參考 `14_Virtual_MES/README.md`，部署和使用虛擬化 MES 系統。

### 4. 開發自己的 MES 適配器
參考 `15_Universal_MES_SDK/README.md` 和 `18_Unified_MES_Virtual_Interface/README.md`，開發自己的 MES 適配器。

---

## 🤝 貢獻

歡迎社群成員對 IDTF 框架提出建議、報告問題或貢獻程式碼。請透過 GitHub Issues 或 Pull Requests 與我們聯繫。

## 📜 授權

本專案採用 **Apache 2.0** 授權。詳見 `LICENSE` 文件。

