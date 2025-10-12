# IDTF V3.3 - Industrial Digital Twin Framework

**版本**: 3.3  
**發布日期**: 2025-10-12  
**作者**: C.C. Lin  
**組織**: IDTF Consortium

---

## 📖 目錄

- [專案概述](#專案概述)
- [核心特色](#核心特色)
- [技術架構](#技術架構)
- [生態系統](#生態系統)
- [文檔結構](#文檔結構)
- [快速開始](#快速開始)
- [應用案例](#應用案例)
- [相關專案](#相關專案)
- [貢獻指南](#貢獻指南)
- [授權條款](#授權條款)

---

## 專案概述

**IDTF (Industrial Digital Twin Framework)** 是一個開放式的工業數位分身框架,旨在實現「**一次建模,到處使用**」的理想。IDTF V3.3 在 V3.1 的基礎上,新增了 **Factory Designer** 工具和完整的 **Omniverse 雙向連接器**,形成了從資產建模到工廠部署的完整生態系統。

### 核心理念

> **Build Once, Use Everywhere**  
> 一次建立工業資產的數位分身,即可在設計、建造、營運、維護等全生命週期中重複使用。

### V3.3 新增功能

- ✨ **Factory Designer**: 視覺化工廠佈局設計工具
- ✨ **FDL 規範設計白皮書**: 基於 5 種實際廠房類型的完整 FDL 規範
- ✨ **IADL/FDL ↔ Omniverse 雙向連接器**: 支援即時多人協作編輯
- ✨ **完整的實作範例**: Python 代碼、API 設計、部署指南

---

## 核心特色

### 1. 完整的生態系統

```
IADL Editor (資產建模)
    ↓ IADL 檔案
Factory Designer (工廠佈局) ⭐ 新增
    ↓ FDL 檔案
NDH (數據中樞)
    ↓ 即時數據
Omniverse (3D 視覺化)
    ↓
實體工廠
```

### 2. 雙向 Omniverse 整合

- **IADL ↔ USD**: 資產定義與 3D 模型雙向同步
- **FDL ↔ USD**: 工廠佈局與 3D 場景雙向同步
- **即時協作**: 多用戶同時編輯,資源鎖定,衝突解決
- **增量同步**: Delta Sync 提高效能

### 3. 標準化語言

- **IADL (Industrial Asset Definition Language)**: 資產定義語言
- **FDL (Factory Design Language)**: 工廠設計語言
- **基於 YAML/JSON**: 人類可讀,機器可解析

### 4. 多產業適用

- ✅ 半導體廠房 (台積電等)
- ✅ LED 封裝廠 (宏齊科技)
- ✅ LNG 發電廠
- ✅ NVIDIA GPU 數據中心
- ✅ Smart City 基礎設施

---

## 技術架構

### IDTF V3.3 七層架構

```
┌─────────────────────────────────────────────────────────────┐
│                    應用層 (Application Layer)                │
│  Digital Twin Applications, Analytics, AI/ML Services       │
├─────────────────────────────────────────────────────────────┤
│                  視覺化層 (Visualization Layer)              │
│  Omniverse, Web 3D Viewer, AR/VR, Dashboards               │
├─────────────────────────────────────────────────────────────┤
│                    服務層 (Service Layer)                    │
│  RESTful API, GraphQL, WebSocket, Authentication           │
├─────────────────────────────────────────────────────────────┤
│                數據處理層 (Data Processing Layer)            │
│  Stream Processing, Batch Processing, AI/ML Pipeline       │
├─────────────────────────────────────────────────────────────┤
│                 數據中樞層 (Data Hub Layer)                  │
│  NDH: Kafka, InfluxDB, PostgreSQL, Redis                   │
├─────────────────────────────────────────────────────────────┤
│                數據採集層 (Data Acquisition Layer)           │
│  OPC UA, MQTT, Modbus, BACnet, SECS/GEM, SNMP             │
├─────────────────────────────────────────────────────────────┤
│                    資產層 (Asset Layer)                      │
│  IADL Assets, FDL Layouts, 3D Models, Metadata             │
└─────────────────────────────────────────────────────────────┘
```

### 核心組件

#### 1. IADL Editor (資產建模工具)

- 視覺化資產定義編輯器
- 3D 模型整合
- 數據標籤配置
- 行為模型編輯
- **雙向 Omniverse 連接器** ⭐

#### 2. Factory Designer (工廠佈局工具) ⭐ 新增

- 視覺化工廠佈局設計
- 拖放式資產配置
- 自動生成 FDL 配置
- 一鍵部署到 NDH
- **雙向 Omniverse 連接器** ⭐

#### 3. NDH (Neutral Data Hub)

- Kafka 數據流處理
- InfluxDB 時序數據儲存
- PostgreSQL 關聯數據管理
- RESTful API 服務

#### 4. Omniverse Connector ⭐ 新增

- IADL ↔ USD 雙向轉換
- FDL ↔ USD 場景生成
- 即時多人協作
- Delta Sync 增量同步
- 版本控制整合

---

## 生態系統

### 工作流程

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│  設計階段     │     │  建造階段     │     │  營運階段     │
├──────────────┤     ├──────────────┤     ├──────────────┤
│              │     │              │     │              │
│ IADL Editor  │────▶│   Factory    │────▶│     NDH      │
│              │     │  Designer    │     │              │
│ 定義資產藍圖  │     │ 設計工廠佈局  │     │ 執行與監控    │
│              │     │              │     │              │
└──────┬───────┘     └──────┬───────┘     └──────┬───────┘
       │                    │                    │
       └────────────────────┼────────────────────┘
                            │
                            ▼
                ┌───────────────────────┐
                │    Omniverse Nucleus  │
                │   3D 視覺化與協作      │
                └───────────────────────┘
```

### 協作流程 ⭐ 新增

```
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│  User A      │     │  User B      │     │  User C      │
│ IADL Editor  │     │   Factory    │     │ IADL Editor  │
│              │     │  Designer    │     │              │
└──────┬───────┘     └──────┬───────┘     └──────┬───────┘
       │                    │                    │
       └────────────────────┼────────────────────┘
                            │
                            ▼
                ┌───────────────────────┐
                │  Collaboration        │
                │  Service              │
                │  - Session Management │
                │  - Lock Management    │
                │  - Change Broadcast   │
                └───────────┬───────────┘
                            │
                            ▼
                ┌───────────────────────┐
                │  Omniverse Nucleus    │
                │  - USD Scene Storage  │
                │  - Version Control    │
                │  - Live Sync          │
                └───────────────────────┘
```

---

## 文檔結構

### 📚 核心文檔 (12 份)

所有文檔提供 **Markdown** 和 **PDF** 雙格式。

| # | 文檔名稱 | 說明 | MD | PDF |
|---|----------|------|----|----|
| 01 | **IDTF Overview** | IDTF V3.3 總覽,核心概念和架構 | [MD](docs/01_IDTF_Overview.md) | [PDF](pdfs/01_IDTF_Overview.pdf) |
| 02 | **IADL Specification** | IADL V3.1 完整規範 | [MD](docs/02_IADL_Specification.md) | [PDF](pdfs/02_IADL_Specification.pdf) |
| 03 | **IADL Editor** | IADL Editor 資產建模工具 | [MD](docs/03_IADL_Editor.md) | [PDF](pdfs/03_IADL_Editor.pdf) |
| 04 | **Factory Designer** ⭐ | Factory Designer 工廠佈局工具 | [MD](docs/04_Factory_Designer.md) | [PDF](pdfs/04_Factory_Designer.pdf) |
| 05 | **FDL Specification** | FDL 工廠設計語言規範 | [MD](docs/05_FDL_Specification.md) | [PDF](pdfs/05_FDL_Specification.pdf) |
| 06 | **NDH Architecture** | NDH 數據中樞架構 | [MD](docs/06_NDH_Architecture.md) | [PDF](pdfs/06_NDH_Architecture.pdf) |
| 07 | **Omniverse Integration** | Omniverse 整合方案 | [MD](docs/07_Omniverse_Integration.md) | [PDF](pdfs/07_Omniverse_Integration.pdf) |
| 08 | **Implementation Guide** | 實施指南 | [MD](docs/08_Implementation_Guide.md) | [PDF](pdfs/08_Implementation_Guide.pdf) |
| 09 | **Use Cases** | 應用案例 (Harvatek 等) | [MD](docs/09_Use_Cases.md) | [PDF](pdfs/09_Use_Cases.pdf) |
| 10 | **API Reference** | API 參考文檔 | [MD](docs/10_API_Reference.md) | [PDF](pdfs/10_API_Reference.pdf) |
| 11 | **FDL Specification Whitepaper** ⭐ | FDL 規範設計白皮書 (5 種廠房類型) | [MD](docs/11_FDL_Specification_Whitepaper.md) | [PDF](pdfs/11_FDL_Specification_Whitepaper.pdf) |
| 12 | **IADL/FDL Omniverse Connector** ⭐ | 雙向連接器技術規範 (即時協作) | [MD](docs/12_IADL_FDL_Omniverse_Connector.md) | [PDF](pdfs/12_IADL_FDL_Omniverse_Connector.pdf) |

### 🖼️ 架構圖 (5 份)

| # | 圖片名稱 | 說明 | 格式 |
|---|----------|------|------|
| 1 | **IDTF Layered Architecture** | IDTF 七層架構 | [PNG](images/idtf_layered_architecture.png) |
| 2 | **IDTF Ecosystem** | IDTF 生態系統 | [PNG](images/idtf_ecosystem.png) |
| 3 | **Factory Designer Workflow** | Factory Designer 工作流程 | [PNG](images/factory_designer_workflow.png) |
| 4 | **NDH Architecture** | NDH 架構 | [PNG](images/ndh_architecture.png) |
| 5 | **IDTF Workflow** | IDTF 完整工作流程 | [PNG](images/idtf_workflow.png) |

所有架構圖的 Mermaid 原始檔案位於 `diagrams/` 目錄。

### 📊 文檔統計

- **總文檔數**: 12 份
- **MD 文件**: 163 KB
- **PDF 文件**: 5.9 MB
- **架構圖**: 5 份 PNG (388 KB)
- **Mermaid 原始檔**: 5 份
- **總文件數**: 34 個
- **總大小**: ~6.5 MB

---

## 快速開始

### 1. 瀏覽文檔

從 [IDTF Overview](docs/01_IDTF_Overview.md) 開始,了解 IDTF V3.3 的核心概念和架構。

### 2. 學習 IADL

閱讀 [IADL Specification](docs/02_IADL_Specification.md),了解如何定義工業資產。

**IADL 範例**:

\`\`\`yaml
asset_id: "DieSorter_v1.0"
name: "Die Sorter"
type: "Production_Equipment"

geometry:
  model_url: "s3://assets/die_sorter.fbx"
  dimensions:
    length: 2.5
    width: 1.8
    height: 2.0

connection_points:
  - id: "power_in"
    type: "electrical"
    voltage: 220
    current: 10

data_tags:
  - name: "throughput"
    type: "float"
    unit: "UPH"
    opc_ua:
      node_id: "ns=2;s=Equipment.DieSorter.Throughput"

visual:
  color: {r: 0.2, g: 0.5, b: 0.8}
  material: "metal"
\`\`\`

### 3. 學習 FDL

閱讀 [FDL Specification Whitepaper](docs/11_FDL_Specification_Whitepaper.md),了解如何設計工廠佈局。

**FDL 範例**:

\`\`\`yaml
factory_design:
  metadata:
    factory_id: "Harvatek_Fab1"
    name: "Harvatek LED Packaging Factory"
    location: "Taiwan"

  buildings:
    - building_id: "Main_Building"
      name: "Main Production Building"
      floors:
        - floor_id: "2F"
          level: 2
          height: 4.5

  layout:
    - area: "Zone_A"
      building: "Main_Building"
      floor: "2F"
      instances:
        - type: "DieSorter_v1.0"
          count: 10
          naming_prefix: "DS_2F_A_"
          layout_pattern: "grid"
          grid_config:
            rows: 2
            columns: 5
            spacing_x: 5.0
            spacing_y: 8.0
\`\`\`

---

## 應用案例

### 案例 1: 宏齊科技 LED 封裝廠 Digital Twins

**專案**: Harvatek LED Packaging Factory Digital Twins System

**規模**:
- 建築: 1 棟,6 層
- 生產設備: 340 台 (8 種類型)
- 設施設備: 1,240+ 台
- 感測器: 1,600+ 點
- 總資產: 2,000+ 項

**投資與效益**:
- 總投資: USD 6.5M
- 年效益: USD 14.6M
- 投資回收期: **6.2 個月**
- IRR: **193.8%**
- 5 年 NPV: USD 41.2M

**相關專案**: [harvatek-digital-twin](https://github.com/chchlin1018/harvatek-digital-twin)

### 案例 2-5

詳見 [FDL Specification Whitepaper](docs/11_FDL_Specification_Whitepaper.md):
- 台積電半導體廠房
- LNG 發電廠
- NVIDIA GPU 數據中心
- Smart City 基礎設施

---

## 相關專案

1. **IDTF V3.3** (本專案) - https://github.com/chchlin1018/idtf-v3.3
2. **IDTF V3.1** - https://github.com/chchlin1018/idtf-v3
3. **Harvatek Digital Twins** - https://github.com/chchlin1018/harvatek-digital-twin
4. **NDH 測試框架** - https://github.com/chchlin1018/NDH-Win-Test1
5. **IADL Editor 測試** - https://github.com/chchlin1018/IADL-Editor-Test1

---

## 貢獻指南

我們歡迎社群貢獻!請參閱 [CONTRIBUTING.md](CONTRIBUTING.md) 了解詳情。

---

## 授權條款

本專案採用 **Creative Commons Attribution-ShareAlike 4.0 International License** (CC BY-SA 4.0)。

詳細資訊: https://creativecommons.org/licenses/by-sa/4.0/

---

## 聯絡資訊

**作者**: C.C. Lin  
**Email**: chchlin1018@gmail.com  
**GitHub**: https://github.com/chchlin1018

---

## 更新日誌

### V3.3 (2025-10-12)

**新增**:
- ✨ Factory Designer 工廠佈局設計工具
- ✨ FDL 規範設計白皮書 (5 種廠房類型)
- ✨ IADL/FDL ↔ Omniverse 雙向連接器
- ✨ 即時多人協作機制
- ✨ Delta Sync 增量同步
- ✨ 完整的 Python 實作範例

**改進**:
- 📝 更新所有文檔到最新架構
- 🖼️ 新增 5 個核心架構圖
- 📚 完整的 API 設計和部署指南

---

**IDTF V3.3 - Build Once, Use Everywhere!** 🚀

**讓工業數位分身成為現實!** 🏭✨
