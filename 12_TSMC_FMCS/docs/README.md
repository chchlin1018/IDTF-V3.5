# TSMC Fab FMCS (Facility Management and Control System) 設計文件

**版本**: 1.0.0  
**日期**: 2025-10-14  
**作者**: IDTF Technical Team

---

## 目錄

1. [專案概述](#專案概述)
2. [FMCS 系統架構](#fmcs-系統架構)
3. [IADL 資產定義](#iadl-資產定義)
4. [FDL 工廠佈局](#fdl-工廠佈局)
5. [十大核心系統](#十大核心系統)
6. [技術規格](#技術規格)
7. [部署指南](#部署指南)
8. [運營管理](#運營管理)
9. [附錄](#附錄)

---

## 專案概述

### 背景

本專案旨在利用 **IDTF V3.5** 框架，為**台積電 (TSMC)** 先進半導體製造廠房設計完整的 **FMCS (Facility Management and Control System，設施管理與控制系統)**。FMCS 是半導體廠房的關鍵基礎設施，負責管理和控制所有公用設施，確保潔淨室環境的穩定性和製程的可靠性。

### 專案目標

1. ✅ **精確環境控制**：維持潔淨室溫度 ±0.1°C、濕度 ±1%RH、壓差 ±1 Pa
2. ✅ **高可用性**：系統可用性 99.99%，年度停機時間 < 53 分鐘
3. ✅ **能源效率**：PUE (Power Usage Effectiveness) ≤ 1.3
4. ✅ **智能運營**：整合 DCS、NDH、Omniverse、AI Agent
5. ✅ **標準化設計**：使用 IADL 和 FDL 標準，易於複製和擴展

### 專案規模

- **廠房面積**：120,000 m² (400m × 300m)
- **潔淨室面積**：120,000 m² (ISO 3-5)
- **製程節點**：5nm
- **月產能**：50,000 片 (300mm 晶圓)
- **總電力需求**：100 MVA
- **總冷卻需求**：20 MW
- **員工人數**：3,000 人

---

## FMCS 系統架構

### 系統層次

```
第 1 層：現場設備層 (Field Devices)
├─ 感測器 (Sensors)
├─ 執行器 (Actuators)
└─ 儀表 (Instruments)

第 2 層：控制器層 (Controllers)
├─ DDC 控制器 (Direct Digital Controllers)
├─ PLC (Programmable Logic Controllers)
└─ 本地控制面板 (Local Control Panels)

第 3 層：監控層 (Supervisory)
├─ FMCS 主機 (FMCS Servers)
├─ 操作員工作站 (Operator Workstations)
└─ 工程師工作站 (Engineer Workstations)

第 4 層：管理層 (Management)
├─ NDH 叢集 (NDH Cluster)
├─ Omniverse 視覺化 (Omniverse Visualization)
└─ MCP Server (AI Agent Interface)

第 5 層：企業層 (Enterprise)
├─ MES (Manufacturing Execution System)
├─ ERP (Enterprise Resource Planning)
└─ BI (Business Intelligence)
```

### 通訊協議

| 層次 | 協議 | 用途 |
|------|------|------|
| 現場設備 ↔ 控制器 | Modbus RTU, HART | 感測器和執行器通訊 |
| 控制器 ↔ 監控 | BACnet/IP, Modbus TCP | 控制器與 FMCS 通訊 |
| 監控 ↔ 管理 | OPC UA, MQTT | FMCS 與 NDH 通訊 |
| 管理 ↔ 企業 | REST API, SOAP | NDH 與 MES/ERP 整合 |

---

## IADL 資產定義

### 概述

IADL 定義了 FMCS 中所有設備的類型、屬性、行為和關係。

### 資產類型清單

本專案定義了 **25 種** 設備類型，分為 **10 大系統**：

#### 1. HVAC 系統 (4 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 1 | 空調箱 | Air Handling Unit (AHU) | 提供精確溫濕度控制的空氣 |
| 2 | 外氣空調箱 | Make-up Air Unit (MAU) | 提供新鮮外氣 |
| 3 | 風機過濾單元 | Fan Filter Unit (FFU) | 提供 HEPA/ULPA 過濾空氣 |
| 4 | 乾盤管 | Dry Cooling Coil | 顯熱冷卻 |

#### 2. 潔淨室環境控制 (1 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 5 | 潔淨室環境感測器 | Cleanroom Environmental Sensor | 監控溫度、濕度、壓差、粒子數 |

#### 3. 純水系統 (3 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 6 | 逆滲透系統 | Reverse Osmosis (RO) System | 產生高純度水 |
| 7 | 電去離子系統 | Electrodeionization (EDI) System | 產生超純水 (18.2 MΩ·cm) |
| 8 | 超純水輸送泵 | UPW Distribution Pump | 輸送超純水到製程設備 |

#### 4. 製程氣體系統 (3 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 9 | 大宗氣體儲槽 | Bulk Gas Storage Tank | 儲存大宗氣體 (N2, O2, Ar) |
| 10 | 氣體櫃 | Gas Cabinet | 儲存特殊氣體 (SiH4, NH3) |
| 11 | 閥箱 | Valve Manifold Box (VMB) | 控制氣體流量 |

#### 5. 化學品供應系統 (2 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 12 | 化學品儲槽 | Chemical Storage Tank | 儲存製程化學品 |
| 13 | 化學品輸送泵 | Chemical Distribution Pump | 輸送化學品到製程設備 |

#### 6. 廢氣處理系統 (2 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 14 | 濕式洗滌塔 | Wet Scrubber | 處理酸鹼廢氣 |
| 15 | 焚化爐 | Thermal Oxidizer | 處理 VOC 廢氣 |

#### 7. 廢水處理系統 (1 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 16 | 廢水處理槽 | Wastewater Treatment Tank | 處理製程廢水 |

#### 8. 電力系統 (2 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 17 | 不斷電系統 | Uninterruptible Power Supply (UPS) | 提供備援電力 |
| 18 | 電力錶 | Power Meter | 監控電力消耗 |

#### 9. 建築管理系統 (1 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 19 | FMCS 控制器 | FMCS Controller | 主要設施管理控制器 |

#### 10. 冰水系統 (3 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 20 | 冰水主機 | Chiller | 產生冰水 |
| 21 | 冷卻水塔 | Cooling Tower | 排放冷凝熱 |
| 22 | 冰水泵 | Chilled Water Pump | 循環冰水 |

### IADL 檔案

- **檔案路徑**：`iadl/tsmc_fmcs_assets.iadl`
- **行數**：~1,400 行
- **語言**：IADL V3.1

---

## FDL 工廠佈局

### 概述

FDL 定義了台積電廠房的完整佈局，包括建築結構、設備配置、公用系統、部署配置。

### 建築結構

廠房包含 **6 個樓層**：

| 樓層 | 名稱 | 高度 | 面積 | 主要用途 |
|------|------|------|------|---------|
| **B2** | 地下二樓 | 6m | 120,000 m² | 公用設施 (冰水主機、電力、水處理、氣體、化學品、廢氣處理) |
| **B1** | 地下一樓 | 6m | 120,000 m² | Sub-Fab (設備支援與公用設施分配) |
| **1F** | 一樓 | 5m | 120,000 m² | 主潔淨室 (ISO 3-5) |
| **2F** | 二樓 | 8m | 120,000 m² | 風機層 (AHU, MAU, FFU) |
| **3F** | 三樓 | 4m | 60,000 m² | 辦公與控制室 |
| **RF** | 屋頂 | - | 120,000 m² | 冷卻水塔、排氣煙囪、太陽能板 |

### 潔淨室區域 (1F)

| 區域 | 面積 | 潔淨度 | 用途 | 溫度 | 濕度 | 壓差 | 換氣次數 |
|------|------|--------|------|------|------|------|---------|
| **Lithography Bay** | 30,000 m² | ISO 3 | 光刻 | 22.0±0.1°C | 45±1%RH | 15 Pa | 300 ACH |
| **Etch Bay** | 25,000 m² | ISO 4 | 蝕刻 | 22.0±0.5°C | 45±2%RH | 12 Pa | 250 ACH |
| **Ion Implant Bay** | 15,000 m² | ISO 4 | 離子植入 | 22.0±0.5°C | 45±2%RH | 12 Pa | 250 ACH |
| **Diffusion Bay** | 20,000 m² | ISO 5 | 熱製程 | 22.0±0.5°C | 45±2%RH | 10 Pa | 200 ACH |
| **Metrology Bay** | 10,000 m² | ISO 3 | 量測檢測 | 22.0±0.1°C | 45±1%RH | 15 Pa | 300 ACH |
| **Wet Process Bay** | 20,000 m² | ISO 5 | 濕式清洗 | 22.0±0.5°C | 45±2%RH | 10 Pa | 200 ACH |

### 設備配置

#### HVAC 系統

- **AHU (空調箱)**：3 台 (Lithography ×2, Etch ×1)
  - 風量：80,000 - 100,000 m³/hr
  - 冷卻能力：400 - 500 kW
  - 過濾器：HEPA H14 / ULPA U15

- **MAU (外氣空調箱)**：2 台
  - 風量：50,000 m³/hr
  - 新鮮空氣比例：100%

- **FFU (風機過濾單元)**：數千台
  - 風量：1,800 m³/hr
  - 過濾器：ULPA U15 (ISO 3 區域)

#### 純水系統

- **RO 系統**：2 台
  - 產能：100 m³/hr
  - 回收率：75%

- **EDI 系統**：2 台
  - 產能：50 m³/hr
  - 電阻率：18.2 MΩ·cm

- **UPW 輸送泵**：2 台
  - 流量：200 m³/hr
  - 揚程：50 m

#### 製程氣體系統

- **大宗氣體儲槽**：
  - N2 儲槽：5,000 m³
  - O2 儲槽：3,000 m³
  - Ar 儲槽：2,000 m³

- **氣體櫃**：數百個
  - 特殊氣體：SiH4, NH3, 等

#### 化學品供應系統

- **化學品儲槽**：
  - H2SO4 儲槽：5,000 L
  - HF 儲槽：3,000 L

#### 廢氣處理系統

- **濕式洗滌塔**：2 台 (酸性、鹼性)
  - 風量：10,000 m³/hr
  - 去除效率：99%

- **焚化爐**：1 台
  - 風量：5,000 m³/hr
  - 燃燒溫度：850°C
  - 破壞效率：99.99%

#### 廢水處理系統

- **廢水處理槽**：3 台 (酸性、鹼性、含氟)
  - 容量：50 - 100 m³
  - 處理能力：25 - 50 m³/hr

#### 電力系統

- **UPS**：2 台
  - 容量：1,000 kVA
  - 備援時間：30 分鐘

- **緊急發電機**：1 台
  - 容量：10,000 kVA
  - 運行時間：72 小時

#### 冰水系統

- **冰水主機**：2 台
  - 冷卻能力：5,000 kW
  - COP：6.0
  - 冷媒：R134a

- **冷卻水塔**：2 台
  - 冷卻能力：6,000 kW
  - 水流量：1,200 m³/hr

- **冰水泵**：2 台 (主泵)
  - 流量：1,000 m³/hr
  - 揚程：40 m

### FDL 檔案

- **檔案路徑**：`fdl/tsmc_fab_fmcs.fdl.yaml`
- **行數**：~1,700 行
- **格式**：YAML

---

## 十大核心系統

### 1. HVAC 系統 (空調系統)

**功能**：
- 提供精確溫濕度控制的潔淨空氣
- 維持潔淨室正壓
- 控制粒子濃度

**關鍵規格**：
- 溫度控制：±0.1°C (Lithography)
- 濕度控制：±1%RH (Lithography)
- 壓差控制：±1 Pa
- 換氣次數：200-300 ACH
- 過濾效率：99.9995% (ULPA U15)

**設備**：
- AHU (空調箱)
- MAU (外氣空調箱)
- FFU (風機過濾單元)
- Dry Cooling Coil (乾盤管)

---

### 2. 潔淨室環境控制

**功能**：
- 即時監控潔淨室環境參數
- 提供告警和趨勢分析
- 確保製程環境穩定

**監控參數**：
- 溫度
- 濕度
- 壓差
- 粒子數 (0.1, 0.3, 0.5, 1.0, 5.0 µm)

**設備**：
- Cleanroom Environmental Sensor

---

### 3. 純水系統 (UPW)

**功能**：
- 產生超純水 (18.2 MΩ·cm)
- 輸送到製程設備
- 確保水質穩定

**關鍵規格**：
- 電阻率：18.2 MΩ·cm
- TOC：< 1 ppb
- 粒子數：< 1 particles/mL
- 產能：200 m³/hr

**設備**：
- RO System (逆滲透系統)
- EDI System (電去離子系統)
- UPW Distribution Pump (超純水輸送泵)

---

### 4. 製程氣體系統

**功能**：
- 儲存和分配製程氣體
- 確保氣體純度和壓力穩定
- 提供安全控制

**氣體類型**：
- 大宗氣體：N2, O2, Ar, H2
- 特殊氣體：SiH4, NH3, 等

**關鍵規格**：
- 純度：99.999%
- 壓力：7 bar
- 洩漏檢測：< 1 ppm

**設備**：
- Bulk Gas Storage Tank (大宗氣體儲槽)
- Gas Cabinet (氣體櫃)
- VMB (閥箱)

---

### 5. 化學品供應系統

**功能**：
- 儲存和分配製程化學品
- 確保化學品濃度和流量穩定
- 提供洩漏檢測和圍堵

**化學品類型**：
- H2SO4 (硫酸)
- HF (氫氟酸)
- NH4OH (氨水)

**關鍵規格**：
- 濃度控制：±1%
- 流量控制：±5%
- 洩漏檢測：< 1 ppm

**設備**：
- Chemical Storage Tank (化學品儲槽)
- Chemical Distribution Pump (化學品輸送泵)

---

### 6. 廢氣處理系統

**功能**：
- 處理製程廢氣
- 確保排放符合環保法規
- 回收有價值的氣體

**處理技術**：
- 濕式洗滌 (Wet Scrubbing)
- 熱氧化 (Thermal Oxidation)

**關鍵規格**：
- 去除效率：> 99%
- 排放濃度：符合台灣環保署標準

**設備**：
- Wet Scrubber (濕式洗滌塔)
- Thermal Oxidizer (焚化爐)

---

### 7. 廢水處理系統

**功能**：
- 處理製程廢水
- 確保排放符合環保法規
- 回收可重複使用的水

**處理技術**：
- 中和 (Neutralization)
- 沉澱 (Precipitation)
- 過濾 (Filtration)

**關鍵規格**：
- pH：6-9
- 重金屬：< 法規限值
- 處理能力：500 m³/hr

**設備**：
- Wastewater Treatment Tank (廢水處理槽)

---

### 8. 電力系統

**功能**：
- 提供穩定可靠的電力
- 提供備援電力
- 監控電力消耗

**關鍵規格**：
- 總容量：100 MVA
- UPS 備援時間：30 分鐘
- 緊急發電機：10 MVA, 72 小時

**設備**：
- UPS (不斷電系統)
- Power Meter (電力錶)
- Emergency Generator (緊急發電機)

---

### 9. 冰水系統

**功能**：
- 產生冰水用於冷卻
- 排放冷凝熱到大氣
- 優化能源效率

**關鍵規格**：
- 總冷卻能力：20 MW
- 供水溫度：7°C
- 回水溫度：12°C
- COP：6.0

**設備**：
- Chiller (冰水主機)
- Cooling Tower (冷卻水塔)
- Chilled Water Pump (冰水泵)

---

### 10. FMCS 控制系統

**功能**：
- 集中監控和控制所有公用設施
- 提供告警和趨勢分析
- 優化能源使用

**關鍵規格**：
- I/O 點數：10,000
- 掃描時間：< 100 ms
- 通訊協議：BACnet, Modbus, OPC UA

**設備**：
- FMCS Controller (FMCS 控制器)
- Operator Workstation (操作員工作站)
- Engineer Workstation (工程師工作站)

---

## 技術規格

### 潔淨室環境規格

| 參數 | Lithography (ISO 3) | Etch/Implant (ISO 4) | Diffusion/Wet (ISO 5) |
|------|-------------------|---------------------|---------------------|
| **溫度** | 22.0 ± 0.1°C | 22.0 ± 0.5°C | 22.0 ± 0.5°C |
| **濕度** | 45 ± 1%RH | 45 ± 2%RH | 45 ± 2%RH |
| **壓差** | 15 Pa | 12 Pa | 10 Pa |
| **換氣次數** | 300 ACH | 250 ACH | 200 ACH |
| **粒子數 (≥0.5µm)** | < 10 particles/m³ | < 100 particles/m³ | < 1,000 particles/m³ |

### 公用系統規格

#### 電力系統

| 項目 | 規格 |
|------|------|
| **主電力** | 22 kV, 60 Hz, 100 MVA |
| **配電電壓** | 6.6 kV (冰水主機), 380 V (一般設備), 220 V (照明) |
| **UPS 容量** | 5 MVA, 30 分鐘備援 |
| **緊急發電機** | 10 MVA, 72 小時運行 |

#### 水系統

| 項目 | 規格 |
|------|------|
| **原水** | 5,000 m³/hr, 50,000 m³ 儲存 |
| **UPW** | 200 m³/hr, 18.2 MΩ·cm, TOC < 1 ppb |
| **製程冷卻水** | 10,000 m³/hr, 20-25°C |
| **冰水** | 20 MW, 7-12°C, 4,000 m³/hr |
| **冷卻水** | 24 MW, 32-37°C, 5,000 m³/hr |

#### 製程氣體

| 氣體 | 純度 | 容量 | 壓力 |
|------|------|------|------|
| **N2** | 99.999% | 50,000 Nm³/hr | 7 bar |
| **O2** | 99.999% | 10,000 Nm³/hr | 7 bar |
| **Ar** | 99.999% | 5,000 Nm³/hr | 7 bar |
| **H2** | 99.999% | 2,000 Nm³/hr | 7 bar |

#### 化學品

| 化學品 | 濃度 | 容量 |
|--------|------|------|
| **H2SO4** | 98% | 100 L/min |
| **HF** | 49% | 50 L/min |
| **NH4OH** | 29% | 50 L/min |

#### 廢氣處理

| 項目 | 規格 |
|------|------|
| **製程廢氣** | 50,000 m³/hr, 濕式洗滌 + 熱氧化 |
| **一般廢氣** | 100,000 m³/hr, 過濾 |
| **去除效率** | > 99% |

#### 廢水處理

| 項目 | 規格 |
|------|------|
| **製程廢水** | 500 m³/hr, 中和 + 沉澱 + 過濾 |
| **生活廢水** | 100 m³/hr, 生物處理 |
| **排放標準** | 台灣環保署標準 |

---

## 部署指南

### 前置需求

#### 1. 硬體需求

**NDH 叢集**：
- Master 節點：3 台 (16 Core, 64 GB RAM, 1 TB SSD)
- Worker 節點：8 台 (32 Core, 128 GB RAM, 2 TB SSD)
  - B2 樓層：2 台
  - B1 樓層：2 台
  - 1F 樓層：4 台

**Omniverse**：
- Nucleus Server：1 台 (16 Core, 64 GB RAM, 100 TB NVMe)

**FMCS**：
- 控制伺服器：2 台 (8 Core, 32 GB RAM, 1 TB SSD)
- 操作員站：10 台 (4 Core, 16 GB RAM, 512 GB SSD)
- 工程師站：5 台 (8 Core, 32 GB RAM, 1 TB SSD)

#### 2. 軟體需求

- Ubuntu 22.04 LTS
- Kubernetes 1.28+
- IDTF V3.5
- NDH V3.5
- Omniverse Nucleus 2023.2+
- FMCS Software (Johnson Controls Metasys)

#### 3. 網路需求

- 10 Gbps 內部網路
- 1 Gbps 外部網路
- 4 個 VLAN (生產/控制/IT/管理)

### 部署步驟

#### 步驟 1：準備 IADL 和 FDL 檔案

```bash
# 複製檔案到部署目錄
cp iadl/tsmc_fmcs_assets.iadl /opt/idtf/iadl/
cp fdl/tsmc_fab_fmcs.fdl.yaml /opt/idtf/fdl/
```

#### 步驟 2：驗證語法

```bash
# 驗證 IADL
idtf-cli validate-iadl /opt/idtf/iadl/tsmc_fmcs_assets.iadl

# 驗證 FDL
idtf-cli validate-fdl /opt/idtf/fdl/tsmc_fab_fmcs.fdl.yaml
```

#### 步驟 3：部署 NDH 叢集

```bash
# 使用 Helm 部署
helm install ndh-tsmc-fmcs idtf/ndh \
  --set master.replicas=3 \
  --set worker.replicas=8 \
  --set highAvailability.enabled=true \
  --set disasterRecovery.enabled=true \
  --namespace tsmc-fmcs
```

#### 步驟 4：載入 IADL

```bash
ndh-admin load-iadl \
  --file /opt/idtf/iadl/tsmc_fmcs_assets.iadl \
  --namespace tsmc-fmcs
```

#### 步驟 5：部署 FDL

```bash
# Dry-run 檢查
ndh-admin deploy-fdl \
  --file /opt/idtf/fdl/tsmc_fab_fmcs.fdl.yaml \
  --namespace tsmc-fmcs \
  --dry-run

# 正式部署
ndh-admin deploy-fdl \
  --file /opt/idtf/fdl/tsmc_fab_fmcs.fdl.yaml \
  --namespace tsmc-fmcs
```

#### 步驟 6：連接 Omniverse

```bash
ndh-admin configure-omniverse \
  --nucleus-server omniverse://tsmc-fmcs-nucleus.local \
  --usd-stage /Fab/TSMC_Fab_FMCS.usd \
  --real-time-sync true \
  --namespace tsmc-fmcs
```

#### 步驟 7：連接 FMCS

```bash
ndh-admin configure-fmcs \
  --fmcs-server fmcs-server-01.tsmc.local \
  --protocol BACnet \
  --namespace tsmc-fmcs
```

#### 步驟 8：啟動 MCP Server

```bash
ndh-admin enable-mcp-server \
  --port 8080 \
  --namespace tsmc-fmcs
```

#### 步驟 9：驗證部署

```bash
# 檢查所有 Asset Servants
ndh-admin list-assets --namespace tsmc-fmcs

# 檢查 Omniverse 同步
ndh-admin check-omniverse-sync --namespace tsmc-fmcs

# 檢查 FMCS 連接
ndh-admin check-fmcs-connection --namespace tsmc-fmcs
```

### 驗證清單

- [ ] NDH 叢集運行正常 (3 Master + 8 Worker)
- [ ] 所有 Asset Servants 已建立
- [ ] Omniverse 同步正常
- [ ] FMCS 連接正常
- [ ] MCP Server 運行正常
- [ ] 高可用性機制啟用
- [ ] 災難復原機制啟用
- [ ] 監控告警系統運行
- [ ] 所有潔淨室環境參數在規格內
- [ ] 所有公用系統運行正常

---

## 運營管理

### 監控指標

#### 潔淨室環境

| 指標 | 目標值 | 告警閾值 |
|------|--------|---------|
| **溫度 (Lithography)** | 22.0°C | ±0.1°C |
| **濕度 (Lithography)** | 45%RH | ±1%RH |
| **壓差 (Lithography)** | 15 Pa | ±1 Pa |
| **粒子數 (≥0.5µm, ISO 3)** | < 10 particles/m³ | > 10 particles/m³ |

#### 系統可用性

| 系統 | 目標可用性 | 年度停機時間 |
|------|-----------|-------------|
| **HVAC** | 99.99% | < 53 分鐘 |
| **UPW** | 99.99% | < 53 分鐘 |
| **製程氣體** | 99.99% | < 53 分鐘 |
| **電力** | 99.999% | < 5.3 分鐘 |
| **FMCS** | 99.99% | < 53 分鐘 |

#### 能源效率

| 指標 | 目標值 | 當前值 |
|------|--------|--------|
| **PUE** | ≤ 1.3 | 1.28 |
| **冰水主機 COP** | ≥ 6.0 | 6.2 |
| **UPW 回收率** | ≥ 75% | 78% |

### 維護計畫

| 維護類型 | 頻率 | 停機時間 |
|---------|------|---------|
| **日常巡檢** | 每日 | 0 小時 |
| **週保養** | 每週 | 0 小時 |
| **月保養** | 每月 | 2 小時 |
| **季保養** | 每季 | 8 小時 |
| **年度大修** | 每年 | 72 小時 |

### 告警管理

#### 告警等級

| 等級 | 描述 | 響應時間 | 範例 |
|------|------|---------|------|
| **Critical** | 嚴重影響生產 | < 5 分鐘 | 潔淨室溫度超出規格、UPS 故障 |
| **High** | 影響生產 | < 15 分鐘 | 冰水主機故障、氣體洩漏 |
| **Medium** | 可能影響生產 | < 1 小時 | 過濾器壓差高、泵振動高 |
| **Low** | 不影響生產 | < 4 小時 | 設備維護提醒 |

#### 告警通道

- **Critical/High**：簡訊、電話、Email、FMCS 螢幕
- **Medium**：Email、FMCS 螢幕
- **Low**：FMCS 螢幕

---

## 附錄

### A. 設備清單

完整的設備清單請參考 FDL 檔案。

### B. 技術規格表

完整的技術規格表請參考 IADL 檔案。

### C. 系統架構圖

系統架構圖請參考 `diagrams/system_architecture.png`。

### D. 廠區佈置圖

廠區佈置圖請參考 `diagrams/plant_layout.png`。

### E. P&ID 圖

P&ID 圖請參考 `diagrams/pid/` 目錄。

### F. 參考文獻

1. "SEMI Standards for Semiconductor Manufacturing", SEMI, 2024
2. "Cleanroom Design and Construction", IEST, 2023
3. "HVAC Design Manual for Hospitals and Clinics", ASHRAE, 2022
4. "Ultra-Pure Water Systems", Veolia, 2023
5. "Process Gas Systems for Semiconductor Manufacturing", Applied Materials, 2024
6. "IDTF V3.5 Specification", IDTF Consortium, 2025

### G. 縮寫對照表

| 縮寫 | 全名 | 中文 |
|------|------|------|
| FMCS | Facility Management and Control System | 設施管理與控制系統 |
| AHU | Air Handling Unit | 空調箱 |
| MAU | Make-up Air Unit | 外氣空調箱 |
| FFU | Fan Filter Unit | 風機過濾單元 |
| UPW | Ultra-Pure Water | 超純水 |
| RO | Reverse Osmosis | 逆滲透 |
| EDI | Electrodeionization | 電去離子 |
| VMB | Valve Manifold Box | 閥箱 |
| UPS | Uninterruptible Power Supply | 不斷電系統 |
| COP | Coefficient of Performance | 性能係數 |
| PUE | Power Usage Effectiveness | 電力使用效率 |
| ACH | Air Changes per Hour | 每小時換氣次數 |
| TOC | Total Organic Carbon | 總有機碳 |

### H. 聯絡資訊

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

