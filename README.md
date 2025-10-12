# IDTF V3.5 - Industrial Digital Twin Framework

**版本**: 3.5  
**發布日期**: 2025-10-13  
**作者**: C.C. Lin  
**組織**: IDTF Consortium

---

## 📖 專案概述

**IDTF (Industrial Digital Twin Framework)** V3.5 是一個全面重組和優化的工業數位分身框架版本，整合了 V3.2 和 V3.3 的所有核心功能，並採用全新的模組化架構組織。

### 核心理念

> **Build Once, Use Everywhere**  
> 一次建立工業資產的數位分身，即可在設計、建造、營運、維護等全生命週期中重複使用。

---

## 📁 專案結構

本專案採用清晰的模組化結構，將所有內容分為 11 個主要類別：

### 01. IDTF Spec（IDTF 規範）
- **內容**: IDTF 框架總覽、核心概念、架構設計、Omniverse 整合
- **文檔**: 
  - IDTF Overview
  - Omniverse Integration
  - IADL/FDL Omniverse Connector
  - V3.2 和 V3.3 規範文檔

### 02. IADL Spec（IADL 規範）
- **內容**: Industrial Asset Definition Language 完整規範
- **文檔**: 
  - IADL Specification V3.1
  - IADL Editor 使用指南
- **包含**: 範例、Schema 定義

### 03. IADL Editor Project（IADL 編輯器專案）
- **內容**: IADL 編輯器的完整源碼專案
- **技術**: Qt6 跨平台實作
- **功能**: 視覺化資產建模工具

### 04. FDL Spec（FDL 規範）
- **內容**: Factory Design Language 完整規範
- **文檔**: 
  - FDL Specification
  - Factory Designer 使用指南
  - FDL Specification Whitepaper（基於 5 種廠房類型）
- **包含**: 範例、Schema 定義

### 05. FDL Editor Project（FDL 編輯器專案）
- **內容**: Factory Designer 工具專案（預留結構）
- **功能**: 視覺化工廠佈局設計工具

### 06. NDH Spec（NDH 規範）
- **內容**: Neutral Data Hub 架構規範
- **文檔**: 
  - NDH Architecture
  - API Reference
  - 時序數據庫分析
  - 安全性分析
- **包含**: 架構圖、API 規格

### 07. NDH Project（NDH 專案）
- **內容**: NDH 實作專案源碼
- **技術**: Windows 優化版本
- **功能**: 數據中樞、Kafka、InfluxDB、PostgreSQL 整合

### 08. White Papers（白皮書）
- **research/**: 學術研究論文
  - IDTF V3.3 Research Paper（中英文版）
- **technical/**: 技術白皮書
  - IDTF Whitepaper V2（完整版及分章節）
  - 各種技術分析文檔
- **business/**: 商業白皮書
  - Value Proposition
  - ROI 分析

### 09. Images（圖片資源）
- **architecture/**: 架構圖
- **diagrams/**: 流程圖、系統圖
- **screenshots/**: 工具截圖
- **logos/**: 品牌標識

### 10. Examples Library（範例庫）
- **iadl_examples/**: IADL 範例檔案
- **fdl_examples/**: FDL 範例檔案
- **use_cases/**: 應用案例
- **tutorials/**: 教學指南

### 11. Business Planning（商業規劃）
- **roi_analysis/**: 投資回報分析
- **case_studies/**: 案例研究（包含 Harvatek LED 廠等）
- **presentations/**: 商業簡報

---

## 🎯 V3.5 主要特色

### 1. 模組化架構
- 清晰的分類結構
- 獨立的規範、專案和文檔
- 易於維護和擴展

### 2. 完整的文檔體系
- 技術規範文檔
- 學術研究論文
- 商業價值分析
- 實作指南和範例

### 3. 整合多版本精華
- V3.2 的規範設計
- V3.3 的完整實作
- 統一的組織架構

### 4. 實戰驗證
- 多個產業應用案例
- 量化的 ROI 分析
- 完整的實施指南

---

## 🚀 快速開始

### 1. 了解 IDTF 框架
從 `01_IDTF_Spec/docs/01_IDTF_Overview.md` 開始

### 2. 學習 IADL 語言
閱讀 `02_IADL_Spec/docs/02_IADL_Specification.md`

### 3. 學習 FDL 語言
閱讀 `04_FDL_Spec/docs/05_FDL_Specification.md`

### 4. 查看實際範例
瀏覽 `10_Examples_Library/` 目錄

### 5. 了解商業價值
查看 `11_Business_Planning/case_studies/`

---

## 📊 應用案例

### 宏齊科技 LED 封裝廠
- **規模**: 340 台生產設備，1,600+ 感測器
- **投資**: USD 6.5M
- **年效益**: USD 14.6M
- **ROI**: 6.2 個月回收期，193.8% IRR

### 其他案例
- 半導體廠房（台積電等）
- LNG 發電廠
- NVIDIA GPU 數據中心
- Smart City 基礎設施

詳見 `11_Business_Planning/case_studies/`

---

## 🔗 相關專案

1. **IDTF V3.3** - https://github.com/chchlin1018/idtf-v3.3
2. **IDTF V3.2** - https://github.com/chchlin1018/IDTF-V3.2
3. **IDTF V3.1** - https://github.com/chchlin1018/idtf-v3
4. **IDTF Documentation** - https://github.com/chchlin1018/idtf-documentation
5. **Harvatek Digital Twin** - https://github.com/chchlin1018/harvatek-digital-twin
6. **NDH Test Framework** - https://github.com/chchlin1018/NDH-Win-Test1
7. **IADL Editor** - https://github.com/chchlin1018/IADL-Editor-Test1

---

## 📝 授權條款

本專案採用 **Creative Commons Attribution-ShareAlike 4.0 International License** (CC BY-SA 4.0)。

詳細資訊: https://creativecommons.org/licenses/by-sa/4.0/

---

## 📧 聯絡資訊

**作者**: C.C. Lin  
**Email**: chchlin1018@gmail.com  
**GitHub**: https://github.com/chchlin1018

---

## 📅 版本歷史

### V3.5 (2025-10-13)
- ✨ 全新的模組化架構組織
- ✨ 整合 V3.2 和 V3.3 所有內容
- ✨ 清晰的 11 大分類結構
- ✨ 完整的文檔、專案和範例庫

### V3.3 (2025-10-12)
- Factory Designer 工具
- FDL 規範設計白皮書
- IADL/FDL ↔ Omniverse 雙向連接器

### V3.2
- 核心規範設計
- 提案和分析文檔

---

**IDTF V3.5 - Build Once, Use Everywhere!** 🚀

