# NDH (Neutral Data Hub) 技術文件索引

**IDTF V3.5 - NDH 規格文件集**

本目錄包含 NDH (Neutral Data Hub) 的完整技術文件,涵蓋架構設計、整合方案、比較分析等多個面向。

---

## 📚 文件分類

### 1. 核心架構文件

#### 1.1 NDH 架構設計
- **[06_NDH_Architecture.md](06_NDH_Architecture.md)** - NDH V3.1 核心架構設計
  - MEP 設計服務
  - 衝突檢測服務
  - ISO 圖面生成
  - 流體分析
  - 即時協同服務

- **[NDH_Architecture_Proposal.md](NDH_Architecture_Proposal.md)** - NDH V3.5 完整架構建議書
  - 分層架構設計
  - 微服務架構
  - IT/OT 融合
  - 企業應用整合
  - 部署架構與擴展性

#### 1.2 時序資料庫架構
- **[NDH_Pluggable_TSDB_Architecture.md](NDH_Pluggable_TSDB_Architecture.md)** - 可抽換時序資料庫架構
  - 支援 InfluxDB, TDengine, TimescaleDB, QuestDB
  - 統一抽象層設計
  - 適配器模式實現
  - 零停機遷移方案

- **[ndh_pluggable_tsdb_architecture.md](ndh_pluggable_tsdb_architecture.md)** - 時序資料庫架構 (備用版本)

- **[ndh_timeseries_database_analysis.md](ndh_timeseries_database_analysis.md)** - 時序資料庫分析與選型

---

### 2. 整合方案文件

#### 2.1 企業系統整合
- **[NDH_PI_Historian_Integration.md](NDH_PI_Historian_Integration.md)** ⭐ - AVEVA PI System 與 Historian 整合架構
  - PI Web API 整合
  - PI Asset Framework 映射
  - Historian (GE, Honeywell, Siemens) 整合
  - 混合架構部署策略

#### 2.2 AI 與協作整合
- **[NDH_MCP_Server_Integration.md](NDH_MCP_Server_Integration.md)** - Model Context Protocol (MCP) 整合
  - AI Agent 整合
  - 語義化查詢
  - 自然語言接口

---

### 3. Asset Tag Instance 核心概念

- **[NDH_Asset_Servant_Correct_Definition.md](NDH_Asset_Servant_Correct_Definition.md)** ⭐⭐⭐ - Asset Tag Instance 正確定義 (最新版)
  - **連結層**: 連結 3D Model Transformation 與即時 IOT Tag 資訊
  - **狀態容器**: 儲存 Asset 狀態和計算值
  - **Behavior 引擎**: 執行告警規則、狀態機、自動化邏輯
  - 完整程式碼實現範例

- **[NDH_Asset_Servant_Clarification.md](NDH_Asset_Servant_Clarification.md)** - Asset Tag Instance 概念澄清
  - Tag 映射功能
  - 與 IADL 的關係

- **[NDH_Asset_Servant_Storage_Clarification.md](NDH_Asset_Servant_Storage_Clarification.md)** - Asset Tag Instance 儲存職責澄清
  - 不儲存 USD 3D Model
  - 不儲存時序數據
  - 只儲存運行時狀態

---

### 4. 比較分析文件

#### 4.1 IDTF vs. 商業解決方案
- **[idtf_vs_aveva_comparison_zh.md](idtf_vs_aveva_comparison_zh.md)** - IDTF vs AVEVA Connect vs AVEVA AIM (中文)
  - 完整功能比較
  - TCO 分析
  - 決策建議

- **[idtf_vs_cognite_comparison_zh.md](idtf_vs_cognite_comparison_zh.md)** - IDTF vs Cognite Data Fusion (中文)
  - 設計驅動 vs 數據驅動
  - 開源 vs 商業
  - 混合架構建議

#### 4.2 NDH vs. 商業解決方案
- **[ndh_vs_aveva_comparison.md](ndh_vs_aveva_comparison.md)** - NDH vs AVEVA 比較 (中文)
- **[ndh_vs_aveva_comparison_en.md](ndh_vs_aveva_comparison_en.md)** - NDH vs AVEVA 比較 (英文)

---

### 5. 系統管理文件

- **[NDH_High_Availability_and_Disaster_Recovery.md](NDH_High_Availability_and_Disaster_Recovery.md)** - 高可用性與災難恢復
  - HA 架構設計
  - 災難恢復策略
  - 備份與還原

- **[NDH_System_Update_Mechanism.md](NDH_System_Update_Mechanism.md)** - 系統更新機制
  - 零停機更新
  - 版本管理
  - 回滾策略

---

### 6. 應用案例文件

- **[production_material_tracking_system.md](production_material_tracking_system.md)** - 生產物料追蹤系統
  - SECS/GEM 整合
  - 即時追蹤
  - WIP 管理

---

### 7. 團隊規劃文件

- **[ndh_development_team_planning.md](ndh_development_team_planning.md)** - NDH 開發團隊規劃
  - 團隊組織架構
  - 技能需求
  - 人力配置

---

## 🎯 推薦閱讀順序

### 初學者路徑
1. **[NDH_Architecture_Proposal.md](NDH_Architecture_Proposal.md)** - 了解 NDH 整體架構
2. **[NDH_Asset_Servant_Correct_Definition.md](NDH_Asset_Servant_Correct_Definition.md)** - 理解核心概念
3. **[NDH_Pluggable_TSDB_Architecture.md](NDH_Pluggable_TSDB_Architecture.md)** - 了解數據存儲策略
4. **[idtf_vs_aveva_comparison_zh.md](idtf_vs_aveva_comparison_zh.md)** - 理解市場定位

### 技術實施路徑
1. **[NDH_Architecture_Proposal.md](NDH_Architecture_Proposal.md)** - 架構設計
2. **[NDH_PI_Historian_Integration.md](NDH_PI_Historian_Integration.md)** - 企業系統整合
3. **[NDH_Pluggable_TSDB_Architecture.md](NDH_Pluggable_TSDB_Architecture.md)** - 數據庫實施
4. **[NDH_High_Availability_and_Disaster_Recovery.md](NDH_High_Availability_and_Disaster_Recovery.md)** - 生產環境部署

### 決策者路徑
1. **[idtf_vs_aveva_comparison_zh.md](idtf_vs_aveva_comparison_zh.md)** - 與 AVEVA 比較
2. **[idtf_vs_cognite_comparison_zh.md](idtf_vs_cognite_comparison_zh.md)** - 與 Cognite 比較
3. **[NDH_Architecture_Proposal.md](NDH_Architecture_Proposal.md)** - 技術可行性
4. **[ndh_development_team_planning.md](ndh_development_team_planning.md)** - 團隊與成本規劃

---

## 📊 文件統計

- **總文件數**: 19 個 Markdown 文件
- **核心架構文件**: 4 個
- **整合方案文件**: 2 個
- **Asset Tag Instance 文件**: 3 個
- **比較分析文件**: 4 個
- **系統管理文件**: 2 個
- **應用案例文件**: 1 個
- **團隊規劃文件**: 1 個

---

## 🔑 核心概念速查

### Asset Tag Instance
**定義**: 連結 3D Model Transformation 與時序資料庫即時 IOT Tag 資訊,並儲存具有 Asset 狀態和 Behavior 的虛擬分身物件。

**三大職責**:
1. **連結層**: 3D Model ↔ Asset Tag Instance ↔ 時序資料庫
2. **狀態容器**: 儲存即時狀態、計算值、告警狀態
3. **Behavior 引擎**: 執行告警規則、狀態機、自動化

詳見: [NDH_Asset_Servant_Correct_Definition.md](NDH_Asset_Servant_Correct_Definition.md)

### 可抽換時序資料庫
**支援的資料庫**:
- ✅ InfluxDB 2.x/3.x
- ✅ TDengine 3.x
- ✅ TimescaleDB 2.x
- ✅ QuestDB
- ✅ AVEVA PI System (透過 PI Web API)
- ✅ Historian (GE, Honeywell, Siemens)

詳見: [NDH_Pluggable_TSDB_Architecture.md](NDH_Pluggable_TSDB_Architecture.md)

### IT/OT 融合
**整合的系統**:
- **IT 層**: ERP, MES, PLM, WMS, QMS
- **OT 層**: SCADA, PLC, DCS, HMI
- **時序數據**: PI System, Historian, TDengine

詳見: [NDH_Architecture_Proposal.md](NDH_Architecture_Proposal.md)

---

## 🔗 相關資源

- **GitHub 倉庫**: https://github.com/chchlin1018/IDTF-V3.5
- **IDTF 主文件**: [../README.md](../README.md)
- **IADL 規格**: [../../05_IADL_Spec/](../../05_IADL_Spec/)
- **FDL 規格**: [../../07_FDL_Spec/](../../07_FDL_Spec/)

---

## 📝 文件版本

- **最後更新**: 2025年10月14日
- **文件版本**: V3.5
- **維護者**: 林志錚 (Chih Cheng Lin, Michael Lin)

---

## 💡 貢獻指南

如需更新或新增文件,請遵循以下原則:
1. 使用 Markdown 格式
2. 同時提供 PDF 版本 (使用 `manus-md-to-pdf`)
3. 更新本 README.md 索引
4. 提交到 GitHub 倉庫

---

**NDH - 供應商中立的工業數據整合中樞**  
*開源、開放、可負擔的數位分身解決方案*

