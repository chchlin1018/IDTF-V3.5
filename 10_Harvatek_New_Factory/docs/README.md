# 宏齊科技新廠房 IDTF V3.5 設計文件

**版本**: 1.0.0  
**日期**: 2025-10-14  
**作者**: IDTF Technical Team

---

## 目錄

1. [專案概述](#專案概述)
2. [檔案結構](#檔案結構)
3. [IADL 資產定義](#iadl-資產定義)
4. [FDL 工廠佈局](#fdl-工廠佈局)
5. [技術規格](#技術規格)
6. [部署指南](#部署指南)
7. [運營指標](#運營指標)
8. [附錄](#附錄)

---

## 專案概述

### 背景

**宏齊科技股份有限公司** (Harvatek Corporation) 成立於 1995 年，是全球領先的 LED 封裝製造商，總部位於台灣新竹科學園區。公司專注於發光二極體 (LED) 的封裝設計、製造、測試及產品進出口貿易業務。

### 專案目標

本專案旨在利用 **IDTF V3.5** 框架，為宏齊科技設計一座全新的智能 LED 封裝廠房，實現：

1. ✅ **標準化設計**：使用 IADL 和 FDL 標準定義所有資產和佈局
2. ✅ **數位分身**：建立完整的工廠數位分身，實現設計、執行、視覺化的閉環
3. ✅ **智能製造**：整合 MES、AI Agent、Omniverse，實現 Industry 4.0
4. ✅ **高效生產**：年產能 50 億顆 LED，OEE 目標 85%，良率目標 98%
5. ✅ **綠色工廠**：能源效率 85%，水資源回收率 70%

### 專案範圍

- **建築面積**：60,000 m² (6 層樓)
- **生產線**：完整的 LED 封裝製程
- **設備數量**：200+ 台生產設備
- **員工人數**：500 人
- **運營模式**：24/7 三班制
- **年產能**：50 億顆 LED

---

## 檔案結構

```
10_Harvatek_New_Factory/
├── iadl/
│   └── harvatek_led_assets.iadl          # IADL 資產定義
├── fdl/
│   └── harvatek_new_factory.fdl.yaml     # FDL 工廠佈局
├── docs/
│   ├── README.md                         # 本文件
│   ├── deployment_guide.md               # 部署指南
│   ├── operations_manual.md              # 運營手冊
│   └── technical_specifications.md       # 技術規格
└── diagrams/
    ├── factory_layout.png                # 工廠佈局圖
    ├── process_flow.png                  # 製程流程圖
    └── system_architecture.png           # 系統架構圖
```

---

## IADL 資產定義

### 概述

IADL (Industrial Automation Description Language) 定義了工廠中所有設備的類型、屬性、行為和關係。

### 資產類型

本專案定義了 **15 種** LED 封裝製程所需的設備類型：

| 編號 | 資產類型 | 英文名稱 | 數量 | 用途 |
|------|---------|---------|------|------|
| 1 | 晶片分選機 | DieSorter | 20 | 晶片分選與檢測 |
| 2 | 固晶機 | DieBonder | 30 | 晶片固定到基板 |
| 3 | 打線機 | WireBonder | 40 | 金線/銅線打線 |
| 4 | 封膠機 | MoldingMachine | 20 | 樹脂封裝 |
| 5 | 固化爐 | CuringOven | 10 | 樹脂固化 |
| 6 | 電鍍機 | PlatingMachine | 15 | 電鍍銀/金 |
| 7 | 切割機 | SingulationMachine | 15 | 切割分離 |
| 8 | 測試機 | TestingMachine | 50 | 電性/光學測試 |
| 9 | 分光分色機 | BinningMachine | 30 | 分光分色分類 |
| 10 | 包裝機 | PackagingMachine | 20 | Tape & Reel 包裝 |
| 11 | AGV | AGV | 15 | 物料運輸 |
| 12 | 輸送帶 | Conveyor | 多條 | 連續運輸 |
| 13 | 儲罐 | StorageTank | 多個 | 化學品/氣體儲存 |
| 14 | 空調系統 | HVACUnit | 多個 | 溫濕度控制 |
| 15 | MES 伺服器 | MESServer | 2 | 製造執行系統 |

### IADL 檔案

- **檔案路徑**：`iadl/harvatek_led_assets.iadl`
- **檔案大小**：~30 KB
- **行數**：~1,000 行
- **語言**：IADL V3.1

### 關鍵特性

1. **完整的屬性定義**：每種設備都定義了技術規格、運行狀態、電力需求、維護資訊
2. **數據標籤**：定義了所有需要監控的數據點（溫度、壓力、速度等）
3. **行為定義**：定義了設備可執行的操作（啟動、停止、校準等）
4. **關係定義**：定義了設備之間的連接關係（物料流、數據連接、電力供應等）

---

## FDL 工廠佈局

### 概述

FDL (Factory Design Language) 定義了工廠的完整佈局，包括建築結構、設備配置、公用系統、部署配置。

### 建築結構

工廠共 **6 層樓**，總面積 **60,000 m²**：

| 樓層 | 用途 | 面積 (m²) | 主要功能 |
|------|------|----------|---------|
| **B1** | 公用設施 | 12,000 | 電力、機械、壓縮機、純水、氮氣、原料倉庫 |
| **1F** | 前段製程 | 12,000 | 晶片分選、固晶、打線 |
| **2F** | 中段製程 | 12,000 | 封膠、固化、電鍍、切割 |
| **3F** | 後段製程 | 12,000 | 電性測試、光學測試、分光分色、包裝 |
| **4F** | 研發品質 | 6,000 | R&D 實驗室、可靠性實驗室、失效分析 |
| **5F** | 辦公行政 | 6,000 | 辦公室、會議室、培訓室、餐廳 |

### 製程流程

完整的 LED 封裝製程分為 **10 個主要步驟**：

```
1. 晶片分選 (Die Sorting)
   ↓
2. 固晶 (Die Bonding)
   ↓
3. 打線 (Wire Bonding)
   ↓ (垂直運輸到 2F)
4. 封膠 (Molding)
   ↓
5. 固化 (Curing)
   ↓
6. 電鍍 (Plating)
   ↓
7. 切割 (Singulation)
   ↓ (垂直運輸到 3F)
8. 電性測試 (Electrical Testing)
   ↓
9. 光學測試 (Optical Testing)
   ↓
10. 分光分色 (Binning)
    ↓
11. 包裝 (Packaging)
    ↓
成品入庫
```

### 設備配置

#### 1F - 前段製程

| 區域 | 設備類型 | 數量 | 潔淨度 | 溫度 | 濕度 |
|------|---------|------|--------|------|------|
| Zone A | 晶片分選機 | 20 | ISO Class 6 | 23°C | 45% |
| Zone B | 固晶機 | 30 | ISO Class 5 | 23°C | 40% |
| Zone C | 打線機 | 40 | ISO Class 5 | 23°C | 40% |

#### 2F - 中段製程

| 區域 | 設備類型 | 數量 | 溫度 | 濕度 |
|------|---------|------|------|------|
| Zone A | 封膠機 | 20 | 25°C | 50% |
| Zone B | 固化爐 | 10 | 25°C | 50% |
| Zone C | 電鍍機 | 15 | 25°C | 50% |
| Zone D | 切割機 | 15 | 25°C | 50% |

#### 3F - 後段製程

| 區域 | 設備類型 | 數量 | 溫度 | 濕度 |
|------|---------|------|------|------|
| Zone A | 測試機 (電性) | 25 | 23°C | 45% |
| Zone B | 測試機 (光學) | 25 | 23°C | 45% |
| Zone C | 分光分色機 | 30 | 23°C | 45% |
| Zone D | 包裝機 | 20 | 23°C | 45% |

### 公用系統

#### 電力系統

- **總容量**：8,000 kW
- **變壓器**：4 台 (3,000 + 2,000 + 2,000 + 1,000 kVA)
- **UPS**：500 kVA，備援 30 分鐘
- **電壓等級**：22 kV / 380 V / 220 V

#### HVAC 系統

- **總冷卻能力**：3,000 RT
- **冰水主機**：2 台 × 1,500 RT
- **空調箱**：5 台，總風量 410,000 CMH
- **濾網等級**：ULPA U15 (前段) / HEPA H13 (中後段)

#### 純水系統

- **產能**：200 m³/day
- **電阻率**：18.2 MΩ·cm
- **儲水槽**：2 × 100 m³
- **製程**：RO + EDI + UV

#### 氮氣系統

- **純度**：99.999%
- **產能**：800 Nm³/hr
- **製程**：PSA (Pressure Swing Adsorption)
- **儲氣槽**：50 m³ @ 20 bar

#### 壓縮空氣系統

- **壓力**：7.0 bar
- **產能**：1,500 Nm³/hr
- **露點**：-40°C
- **壓縮機**：2 × 750 Nm³/hr

### FDL 檔案

- **檔案路徑**：`fdl/harvatek_new_factory.fdl.yaml`
- **檔案大小**：~80 KB
- **行數**：~2,000 行
- **格式**：YAML

### 關鍵特性

1. **階層結構**：建築物 → 樓層 → 區域 → 設備實例
2. **參數化配置**：使用 `grid` 和 `linear` 佈局模式自動生成設備位置
3. **關係定義**：明確定義物料流、數據連接、電力供應關係
4. **Worker Assignment Hints**：支援分散式 NDH 部署
5. **完整的公用系統**：電力、HVAC、純水、氮氣、壓縮空氣、真空、化學品、廢水處理

---

## 技術規格

### 生產能力

| 指標 | 目標值 |
|------|--------|
| **年產能** | 50 億顆 LED |
| **日產能** | 1,430 萬顆 LED |
| **小時產能** | 59.5 萬顆 LED |
| **OEE** | 85% |
| **良率** | 98% |
| **稼動率** | 90% |

### 製程參數

| 製程 | 設備 | UPH | Cycle Time |
|------|------|-----|------------|
| 晶片分選 | DieSorter | 12,000 | 0.3 秒 |
| 固晶 | DieBonder | 8,000 | 0.45 秒 |
| 打線 | WireBonder | 10,000 | 0.36 秒 |
| 封膠 | MoldingMachine | 6,000 | 0.6 秒 |
| 固化 | CuringOven | 6,000 | 0.6 秒 |
| 電鍍 | PlatingMachine | 5,000 | 0.72 秒 |
| 切割 | SingulationMachine | 7,000 | 0.51 秒 |
| 測試 | TestingMachine | 15,000 | 0.24 秒 |
| 分光分色 | BinningMachine | 18,000 | 0.2 秒 |
| 包裝 | PackagingMachine | 20,000 | 0.18 秒 |

### 環境規格

| 區域 | 潔淨度 | 溫度 | 濕度 | 換氣次數 |
|------|--------|------|------|---------|
| 晶片分選 | ISO Class 6 | 23±1°C | 45±5% | 30 次/hr |
| 固晶 | ISO Class 5 | 23±1°C | 40±5% | 30 次/hr |
| 打線 | ISO Class 5 | 23±1°C | 40±5% | 30 次/hr |
| 封膠 | - | 25±2°C | 50±5% | 20 次/hr |
| 測試 | - | 23±1°C | 45±5% | 25 次/hr |

### IT 架構

| 組件 | 規格 |
|------|------|
| **MES 系統** | MES v5.0，2 台伺服器，最大 500 連接 |
| **NDH 叢集** | 3 Master + 10 Worker 節點 |
| **Omniverse** | Nucleus Server，支援 50 並發用戶 |
| **網路** | 3 個 VLAN (生產/IT/管理) |
| **資料處理** | 10,000 records/sec |

---

## 部署指南

### 前置需求

1. **硬體需求**
   - NDH Master 節點：3 台 (16 Core, 64 GB RAM, 1 TB SSD)
   - NDH Worker 節點：10 台 (32 Core, 128 GB RAM, 2 TB SSD)
   - Omniverse Nucleus Server：1 台 (16 Core, 64 GB RAM, 10 TB NVMe)
   - MES 伺服器：2 台 (16 Core, 64 GB RAM, 2 TB SSD)

2. **軟體需求**
   - Ubuntu 22.04 LTS
   - Kubernetes 1.28+
   - IDTF V3.5
   - NDH V3.5
   - Omniverse Nucleus 2023.2+
   - MES v5.0

3. **網路需求**
   - 10 Gbps 內部網路
   - 1 Gbps 外部網路
   - VLAN 分段 (生產/IT/管理)

### 部署步驟

#### 步驟 1：準備 IADL 和 FDL 檔案

```bash
# 複製 IADL 和 FDL 檔案到部署目錄
cp iadl/harvatek_led_assets.iadl /opt/idtf/iadl/
cp fdl/harvatek_new_factory.fdl.yaml /opt/idtf/fdl/
```

#### 步驟 2：驗證 IADL 和 FDL 語法

```bash
# 驗證 IADL 語法
idtf-cli validate-iadl /opt/idtf/iadl/harvatek_led_assets.iadl

# 驗證 FDL 語法
idtf-cli validate-fdl /opt/idtf/fdl/harvatek_new_factory.fdl.yaml
```

#### 步驟 3：部署 NDH 叢集

```bash
# 使用 Helm 部署 NDH
helm install ndh-harvatek idtf/ndh \
  --set master.replicas=3 \
  --set worker.replicas=10 \
  --set highAvailability.enabled=true \
  --set disasterRecovery.enabled=true \
  --set backup.frequency=hourly \
  --namespace harvatek-production
```

#### 步驟 4：載入 IADL 資產定義

```bash
# 載入 IADL 到 NDH
ndh-admin load-iadl \
  --file /opt/idtf/iadl/harvatek_led_assets.iadl \
  --namespace harvatek-production
```

#### 步驟 5：部署 FDL 工廠佈局

```bash
# 部署 FDL 到 NDH
ndh-admin deploy-fdl \
  --file /opt/idtf/fdl/harvatek_new_factory.fdl.yaml \
  --namespace harvatek-production \
  --dry-run  # 先執行 dry-run 檢查

# 確認無誤後正式部署
ndh-admin deploy-fdl \
  --file /opt/idtf/fdl/harvatek_new_factory.fdl.yaml \
  --namespace harvatek-production
```

#### 步驟 6：連接 Omniverse

```bash
# 配置 Omniverse 連接器
ndh-admin configure-omniverse \
  --nucleus-server omniverse://harvatek-nucleus.local \
  --usd-stage /Factory/HarvaTek_New_Factory.usd \
  --real-time-sync true \
  --namespace harvatek-production
```

#### 步驟 7：連接 MES 系統

```bash
# 配置 MES 連接
ndh-admin configure-mes \
  --mes-server mes-server-01.harvatek.local \
  --protocol SECS/GEM \
  --namespace harvatek-production
```

#### 步驟 8：啟動 MCP Server

```bash
# 啟動 MCP Server
ndh-admin enable-mcp-server \
  --port 8080 \
  --namespace harvatek-production
```

#### 步驟 9：驗證部署

```bash
# 檢查所有 Asset Servants 狀態
ndh-admin list-assets --namespace harvatek-production

# 檢查 Omniverse 同步狀態
ndh-admin check-omniverse-sync --namespace harvatek-production

# 檢查 MES 連接狀態
ndh-admin check-mes-connection --namespace harvatek-production
```

### 驗證清單

- [ ] NDH 叢集運行正常 (3 Master + 10 Worker)
- [ ] 所有 Asset Servants 已建立 (200+ 個)
- [ ] Omniverse 同步正常
- [ ] MES 連接正常
- [ ] MCP Server 運行正常
- [ ] 高可用性機制啟用
- [ ] 災難復原機制啟用
- [ ] 監控告警系統運行

---

## 運營指標

### KPI 目標

| KPI | 目標值 | 計算方式 |
|-----|--------|---------|
| **OEE** | 85% | 可用率 × 效能 × 良率 |
| **可用率** | 90% | 實際運行時間 / 計畫運行時間 |
| **效能** | 95% | 實際產量 / 理論產量 |
| **良率** | 98% | 良品數 / 總產量 |
| **MTBF** | 5,000 hr | 平均故障間隔時間 |
| **MTTR** | 2 hr | 平均修復時間 |
| **能源效率** | 85% | 有效能源 / 總能源 |
| **水資源回收率** | 70% | 回收水量 / 總用水量 |

### 生產排程

| 班次 | 時間 | 人數 | 產能 (萬顆/hr) |
|------|------|------|---------------|
| 早班 | 08:00-16:00 | 180 | 60 |
| 中班 | 16:00-00:00 | 170 | 60 |
| 夜班 | 00:00-08:00 | 150 | 55 |
| **總計** | 24 小時 | 500 | **59.5** |

### 維護排程

| 維護類型 | 頻率 | 停機時間 | 負責團隊 |
|---------|------|---------|---------|
| **日常巡檢** | 每日 | 0 hr | 操作員 |
| **預防性維護** | 每週 | 2 hr | 維護團隊 |
| **預測性維護** | 依 AI 預測 | 1 hr | 維護團隊 |
| **年度大保養** | 每年 | 48 hr | 外部廠商 |

---

## 附錄

### A. 設備清單

完整的設備清單請參考 FDL 檔案。

### B. 製程參數表

完整的製程參數表請參考技術規格文件。

### C. 公用系統規格

完整的公用系統規格請參考 FDL 檔案的 `utilities` 章節。

### D. 網路架構圖

網路架構圖請參考 `diagrams/network_architecture.png`。

### E. 聯絡資訊

- **專案負責人**：C.C. Lin
- **技術支援**：IDTF Technical Team
- **電子郵件**：support@idtf.org
- **網站**：https://idtf.org

---

## 版本歷史

| 版本 | 日期 | 作者 | 變更說明 |
|------|------|------|---------|
| 1.0.0 | 2025-10-14 | IDTF Technical Team | 初始版本 |

---

## 授權

本文件採用 **CC BY-NC-SA 4.0** 授權。

© 2025 IDTF Consortium. All rights reserved.

