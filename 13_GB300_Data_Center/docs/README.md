# GB300 AI Data Center 設計文件

**版本**: 1.0.0  
**日期**: 2025-10-14  
**作者**: IDTF Technical Team

---

## 目錄

1. [專案概述](#專案概述)
2. [GB300 技術規格](#gb300-技術規格)
3. [IADL 資產定義](#iadl-資產定義)
4. [FDL 數據中心佈局](#fdl-數據中心佈局)
5. [系統架構](#系統架構)
6. [技術規格](#技術規格)
7. [部署指南](#部署指南)
8. [運營管理](#運營管理)
9. [應用場景](#應用場景)
10. [附錄](#附錄)

---

## 專案概述

### 背景

本專案旨在利用 **IDTF V3.5** 框架，設計一座中型規模的 **NVIDIA GB300 AI 數據中心**。GB300 是 NVIDIA 最新的 AI 超級計算平台，結合了 **Grace CPU** 和 **Blackwell GPU**，專為大規模 AI 訓練和推理而設計。

### 專案目標

1. ✅ **超大規模 AI 計算**：支援大型語言模型 (LLM) 訓練和推理
2. ✅ **極致效能**：28.8 ExaFLOPS (FP8) 計算能力
3. ✅ **高能源效率**：PUE ≤ 1.3，液冷技術
4. ✅ **高可靠性**：99.99% 系統可用性
5. ✅ **智能運營**：整合 NDH、Omniverse、MCP Server

### 專案規模

- **計算能力**：
  - 1,440 個 Blackwell GPUs (20 racks × 72 GPUs)
  - 720 個 Grace CPUs (20 racks × 36 CPUs)
  - 28.8 ExaFLOPS (FP8) 峰值效能
  - 207 TB GPU HBM3e 記憶體
  - 92 TB 系統記憶體

- **儲存容量**：10 PB NVMe SSD

- **網路頻寬**：
  - InfiniBand: 400G NDR (1:1 non-blocking)
  - Ethernet: 400G (管理網路)

- **電力需求**：
  - IT 負載：2 MW
  - 總負載：2.6 MW (PUE 1.29)
  - UPS 容量：4 MW (N+1)
  - 發電機容量：6 MW (N+1)

- **冷卻容量**：
  - 液冷：8 MW (4 × 2MW chillers, N+1)
  - 氣冷：1 MW (CRAC units)

- **建築面積**：
  - 數據大廳：4,800 m²
  - 總建築面積：8,400 m²

---

## GB300 技術規格

### NVIDIA GB300 NVL72 系統

GB300 NVL72 是 NVIDIA 最新的機架級 AI 超級計算系統，專為大規模 AI 訓練和推理而設計。

#### 硬體架構

**單一 GB300 NVL72 機架包含**：

| 組件 | 數量 | 規格 |
|------|------|------|
| **Blackwell GPUs** | 72 | 每個 GPU: 20 PFLOPS (FP8), 144GB HBM3e |
| **Grace CPUs** | 36 | 每個 CPU: 72 Arm Neoverse V2 cores, 128GB LPDDR5X |
| **NVLink Switch** | 36 | 每個 GPU: 1.8 TB/s 頻寬 |
| **總 GPU 記憶體** | 10.4 TB | 72 × 144GB HBM3e |
| **總系統記憶體** | 4.6 TB | 36 × 128GB LPDDR5X |
| **峰值效能 (FP8)** | 1.44 ExaFLOPS | 72 × 20 PFLOPS |
| **功耗** | 120 kW | 最大功耗 |

#### 關鍵技術

1. **Blackwell GPU**：
   - 第二代 Transformer Engine
   - FP4 精度支援
   - 208B 電晶體
   - 雙芯片設計 (2 × 104B)

2. **Grace CPU**：
   - 72 個 Arm Neoverse V2 核心
   - 3.5 GHz 時脈
   - 117 MB L3 快取
   - 512 GB/s 記憶體頻寬

3. **NVLink Switch**：
   - 第五代 NVLink
   - 1.8 TB/s 每 GPU 頻寬
   - 全連接拓撲 (72-way)
   - 延遲 < 1 μs

4. **液冷技術**：
   - 直接接觸式液冷 (Direct-to-chip)
   - 90% 熱量由液冷移除
   - 供水溫度：25°C
   - 回水溫度：45°C

---

## IADL 資產定義

### 概述

IADL 定義了 GB300 數據中心中所有設備的類型、屬性、行為和關係。

### 資產類型清單

本專案定義了 **18 種** 設備類型，分為 **5 大系統**：

#### 1. 計算基礎設施 (4 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 1 | GB300 NVL72 機架 | GB300 NVL72 Rack | 72-GPU 機架級 AI 超級計算系統 |
| 2 | InfiniBand 交換機 (400G) | InfiniBand Switch 400G | NVIDIA Quantum-2 交換機 |
| 3 | 乙太網路交換機 (400G) | Ethernet Switch 400G | NVIDIA Spectrum-4 交換機 |
| 4 | 儲存伺服器 | Storage Server | NVMe SSD 儲存伺服器 |

#### 2. 電力基礎設施 (3 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 5 | 配電單元 (400kW) | PDU 400kW | 高容量配電單元 |
| 6 | 不斷電系統 (2MW) | UPS 2MW | 高容量 UPS 系統 |
| 7 | 柴油發電機 (3MW) | Diesel Generator 3MW | 備援發電機 |

#### 3. 冷卻基礎設施 (4 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 8 | 液冷分配單元 (500kW) | Liquid CDU 500kW | 直接接觸式液冷系統 |
| 9 | 冰水主機 (2MW) | Chiller 2MW | 高效率冰水主機 |
| 10 | 冷卻水塔 (3MW) | Cooling Tower 3MW | 蒸發式冷卻水塔 |
| 11 | 精密空調 (100kW) | CRAC 100kW | 機房精密空調 |

#### 4. 監控與管理 (4 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 12 | DCIM 伺服器 | DCIM Server | 數據中心基礎設施管理 |
| 13 | 環境感測器 | Environmental Sensor | 溫濕度監控 |
| 14 | 消防系統 | Fire Suppression System | 潔淨氣體滅火系統 |

#### 5. 網路與連接 (3 種)

| 編號 | 資產類型 | 英文名稱 | 主要功能 |
|------|---------|---------|---------|
| 15 | 核心路由器 | Core Router | 高效能核心路由器 |
| 16 | 防火牆 | Firewall | 次世代防火牆 |

### IADL 檔案

- **檔案路徑**：`iadl/gb300_datacenter_assets.iadl`
- **行數**：~950 行
- **語言**：IADL V3.1

---

## FDL 數據中心佈局

### 概述

FDL 定義了 GB300 數據中心的完整佈局，包括建築結構、設備配置、網路拓撲、電力分配、冷卻系統。

### 建築結構

數據中心包含 **1 棟主建築** + **1 個戶外設備區**：

#### 主數據中心建築 (80m × 60m × 12m)

| 樓層 | 名稱 | 高度 | 面積 | 主要用途 |
|------|------|------|------|---------|
| **Ground Floor** | 地面層 - 數據大廳 | 6m | 4,800 m² | 計算、網路、儲存設備 |
| **Mezzanine** | 夾層 - 基礎設施 | 4m | 2,400 m² | 冷卻設備、電力設備 |
| **Top Floor** | 頂層 - 辦公與控制 | 2m | 1,200 m² | NOC、辦公室、會議室 |

#### 地面層區域劃分

| 區域 | 面積 | 用途 | 設備 |
|------|------|------|------|
| **Compute Zone A** | 1,200 m² | GB300 機架 (10 racks) | 10 × GB300 NVL72 |
| **Compute Zone B** | 1,200 m² | GB300 機架 (10 racks) | 10 × GB300 NVL72 |
| **Network Zone** | 400 m² | 網路設備 | IB/Eth 交換機、路由器、防火牆 |
| **Storage Zone** | 400 m² | 儲存設備 | 10 × Storage Servers (10 PB) |
| **Hot Aisle A/B** | 1,200 m² | 熱通道 | - |
| **Cold Aisle A/B** | 1,200 m² | 冷通道 | - |

#### 戶外設備區 (40m × 30m)

| 區域 | 面積 | 用途 | 設備 |
|------|------|------|------|
| **Generator Yard** | 400 m² | 發電機 | 2 × 3MW 柴油發電機 |
| **Cooling Tower Yard** | 600 m² | 冷卻水塔 | 4 × 3MW 冷卻水塔 |
| **Chiller Yard** | 200 m² | 冰水主機 | 4 × 2MW 冰水主機 |

### 設備配置

#### 計算設備

- **GB300 NVL72 機架**：20 個
  - Zone A：10 個 (A01-A10)
  - Zone B：10 個 (B01-B10)
  - 每個機架：72 GPUs, 36 CPUs, 120 kW

#### 網路設備

**InfiniBand 網路 (Spine-Leaf 架構)**：
- **Spine 交換機**：4 個 (64-port, 400G)
- **Leaf 交換機**：20 個 (每個機架 1 個)
- **拓撲**：1:1 non-blocking

**乙太網路 (管理網路)**：
- **Core 交換機**：2 個 (64-port, 400G)
- **核心路由器**：2 個 (25.6 Tb/s)
- **防火牆**：2 個 (100 Gb/s)

#### 儲存設備

- **儲存伺服器**：10 個
  - 每個：1 PB NVMe SSD
  - 總容量：10 PB
  - 總讀取頻寬：1 TB/s
  - 總寫入頻寬：800 GB/s

#### 電力設備

- **PDU**：20 個 (每個機架 1 個, 400kW)
- **UPS**：2 個 (2MW, N+1 冗餘)
- **發電機**：2 個 (3MW, N+1 冗餘)

#### 冷卻設備

- **液冷 CDU**：20 個 (每個機架 1 個, 500kW)
- **冰水主機**：4 個 (2MW, N+1 冗餘)
- **冷卻水塔**：4 個 (3MW, N+1 冗餘)
- **CRAC 單元**：5 個 (100kW, 補充冷卻)

### FDL 檔案

- **檔案路徑**：`fdl/gb300_datacenter.fdl.yaml`
- **行數**：~1,000 行
- **格式**：YAML

---

## 系統架構

### 計算架構

```
20 × GB300 NVL72 Racks
├─ 1,440 Blackwell GPUs (72 per rack)
├─ 720 Grace CPUs (36 per rack)
├─ 207 TB GPU Memory (HBM3e)
├─ 92 TB System Memory (LPDDR5X)
└─ 28.8 ExaFLOPS (FP8)
```

### 網路架構

```
Spine-Leaf InfiniBand Fabric (400G NDR)
├─ 4 Spine Switches (64-port, 400G)
├─ 20 Leaf Switches (64-port, 400G)
└─ 1:1 Non-blocking (51.2 Tb/s per switch)

Management Ethernet Network (400G)
├─ 2 Core Switches (64-port, 400G)
├─ 2 Core Routers (25.6 Tb/s)
└─ 2 Firewalls (100 Gb/s)
```

### 儲存架構

```
Distributed NVMe Storage
├─ 10 Storage Servers
├─ 10 PB Total Capacity
├─ 1 TB/s Read Bandwidth
└─ 800 GB/s Write Bandwidth
```

### 電力架構

```
Utility Power (22 kV) → Transformer (480V)
    ↓
2 × UPS (2MW, N+1) ← 2 × Generator (3MW, N+1)
    ↓
20 × PDU (400kW)
    ↓
20 × GB300 Racks (120kW each)
```

### 冷卻架構

```
4 × Cooling Towers (3MW, N+1)
    ↓
4 × Chillers (2MW, N+1)
    ↓
20 × Liquid CDUs (500kW)
    ↓
20 × GB300 Racks (Direct-to-chip liquid cooling)

Supplemental: 5 × CRAC Units (100kW)
```

---

## 技術規格

### 計算規格

| 指標 | 數值 |
|------|------|
| **總 GPU 數量** | 1,440 (20 racks × 72 GPUs) |
| **總 CPU 數量** | 720 (20 racks × 36 CPUs) |
| **總 GPU 記憶體** | 207 TB (1,440 × 144GB HBM3e) |
| **總系統記憶體** | 92 TB (720 × 128GB LPDDR5X) |
| **峰值效能 (FP8)** | 28.8 ExaFLOPS (1,440 × 20 PFLOPS) |
| **峰值效能 (FP16)** | 7.2 ExaFLOPS |
| **峰值效能 (FP32)** | 3.6 ExaFLOPS |
| **NVLink 總頻寬** | 2,592 TB/s (1,440 × 1.8 TB/s) |

### 電力規格

| 指標 | 數值 |
|------|------|
| **IT 負載** | 2,000 kW (20 racks × 100kW) |
| **冷卻負載** | 571 kW (4 chillers × 142.75kW @ COP 3.5) |
| **總設施負載** | 2,571 kW |
| **PUE** | 1.29 (2,571 / 2,000) |
| **設計容量** | 3,000 kW |
| **UPS 容量** | 4,000 kW (2 × 2,000kW, N+1) |
| **發電機容量** | 6,000 kW (2 × 3,000kW, N+1) |
| **UPS 備援時間** | 15 分鐘 (全載) |
| **發電機運行時間** | 24 小時 (全載) |

### 冷卻規格

| 指標 | 數值 |
|------|------|
| **總冷卻容量** | 8,000 kW (4 chillers × 2,000kW, N+1) |
| **液冷容量** | 10,000 kW (20 CDUs × 500kW) |
| **氣冷容量** | 500 kW (5 CRACs × 100kW) |
| **液冷比例** | 90% |
| **氣冷比例** | 10% |
| **冰水供水溫度** | 18°C |
| **冰水回水溫度** | 28°C |
| **液冷供水溫度** | 25°C |
| **液冷回水溫度** | 45°C |
| **冷卻水塔容量** | 12,000 kW (4 towers × 3,000kW, N+1) |
| **Chiller COP** | 3.5 |

### 儲存規格

| 指標 | 數值 |
|------|------|
| **總容量** | 10 PB (10 servers × 1 PB) |
| **總讀取頻寬** | 1 TB/s (10 servers × 100 GB/s) |
| **總寫入頻寬** | 800 GB/s (10 servers × 80 GB/s) |
| **總 IOPS** | 100M IOPS (10 servers × 10M IOPS) |
| **儲存類型** | NVMe SSD |
| **RAID 等級** | RAID 6 |

### 網路規格

| 指標 | 數值 |
|------|------|
| **InfiniBand 規格** | 400G NDR |
| **Ethernet 規格** | 400G |
| **Spine 交換機數量** | 4 |
| **Leaf 交換機數量** | 20 |
| **Oversubscription 比例** | 1:1 (Non-blocking) |
| **每個交換機頻寬** | 51.2 Tb/s |
| **Port-to-port 延遲** | < 0.6 μs |

### 環境規格

| 指標 | 數值 |
|------|------|
| **數據大廳溫度** | 24°C |
| **數據大廳濕度** | 45%RH |
| **最大海拔** | 1,000 m |
| **噪音等級** | < 70 dB |

### 可靠性規格

| 指標 | 數值 |
|------|------|
| **目標正常運行時間** | 99.99% |
| **年度停機時間** | < 53 分鐘 |
| **MTBF** | 100,000 小時 |
| **MTTR** | 4 小時 |

---

## 部署指南

### 前置需求

#### 1. 硬體需求

**NDH 叢集**：
- Master 節點：3 台 (16 Core, 64 GB RAM, 1 TB SSD)
- Worker 節點：5 台 (32 Core, 128 GB RAM, 2 TB SSD)

**Omniverse**：
- Nucleus Server：1 台 (16 Core, 64 GB RAM, 100 TB NVMe)

**DCIM**：
- 管理伺服器：2 台 (8 Core, 32 GB RAM, 1 TB SSD)

#### 2. 軟體需求

- Ubuntu 22.04 LTS
- Kubernetes 1.28+
- IDTF V3.5
- NDH V3.5
- Omniverse Nucleus 2023.2+
- DCIM Software (Schneider EcoStruxure / Vertiv Trellis)

#### 3. 網路需求

- 400G InfiniBand (NDR)
- 400G Ethernet (管理)
- 4 個 VLAN (管理/InfiniBand/儲存/帶外管理)

### 部署步驟

#### 步驟 1：準備 IADL 和 FDL 檔案

```bash
# 複製檔案到部署目錄
cp iadl/gb300_datacenter_assets.iadl /opt/idtf/iadl/
cp fdl/gb300_datacenter.fdl.yaml /opt/idtf/fdl/
```

#### 步驟 2：驗證語法

```bash
# 驗證 IADL
idtf-cli validate-iadl /opt/idtf/iadl/gb300_datacenter_assets.iadl

# 驗證 FDL
idtf-cli validate-fdl /opt/idtf/fdl/gb300_datacenter.fdl.yaml
```

#### 步驟 3：部署 NDH 叢集

```bash
# 使用 Helm 部署
helm install ndh-gb300 idtf/ndh \
  --set master.replicas=3 \
  --set worker.replicas=5 \
  --set highAvailability.enabled=true \
  --set disasterRecovery.enabled=true \
  --namespace gb300-datacenter
```

#### 步驟 4：載入 IADL

```bash
ndh-admin load-iadl \
  --file /opt/idtf/iadl/gb300_datacenter_assets.iadl \
  --namespace gb300-datacenter
```

#### 步驟 5：部署 FDL

```bash
# Dry-run 檢查
ndh-admin deploy-fdl \
  --file /opt/idtf/fdl/gb300_datacenter.fdl.yaml \
  --namespace gb300-datacenter \
  --dry-run

# 正式部署
ndh-admin deploy-fdl \
  --file /opt/idtf/fdl/gb300_datacenter.fdl.yaml \
  --namespace gb300-datacenter
```

#### 步驟 6：連接 Omniverse

```bash
ndh-admin configure-omniverse \
  --nucleus-server omniverse://gb300-datacenter-nucleus.local \
  --usd-stage /DataCenter/GB300_DataCenter.usd \
  --real-time-sync true \
  --namespace gb300-datacenter
```

#### 步驟 7：連接 DCIM

```bash
ndh-admin configure-dcim \
  --dcim-server dcim-server-01.gb300.local \
  --protocol REST \
  --namespace gb300-datacenter
```

#### 步驟 8：啟動 MCP Server

```bash
ndh-admin enable-mcp-server \
  --port 8080 \
  --authentication oauth2 \
  --namespace gb300-datacenter
```

#### 步驟 9：驗證部署

```bash
# 檢查所有 Asset Tag Instances
ndh-admin list-assets --namespace gb300-datacenter

# 檢查 Omniverse 同步
ndh-admin check-omniverse-sync --namespace gb300-datacenter

# 檢查 DCIM 連接
ndh-admin check-dcim-connection --namespace gb300-datacenter

# 檢查 MCP Server
curl http://mcp-server.gb300.local:8080/health
```

### 驗證清單

- [ ] NDH 叢集運行正常 (3 Master + 5 Worker)
- [ ] 所有 Asset Tag Instances 已建立 (20 GB300 racks + infrastructure)
- [ ] Omniverse 同步正常
- [ ] DCIM 連接正常
- [ ] MCP Server 運行正常
- [ ] InfiniBand 網路正常 (Spine-Leaf)
- [ ] 儲存系統正常 (10 PB)
- [ ] 電力系統正常 (UPS, Generator)
- [ ] 冷卻系統正常 (Chillers, CDUs)
- [ ] 監控告警系統運行

---

## 運營管理

### 監控指標

#### 計算指標

| 指標 | 目標值 | 告警閾值 |
|------|--------|---------|
| **GPU 使用率** | > 80% | < 50% (低使用率) |
| **GPU 溫度** | < 80°C | > 85°C |
| **GPU 記憶體使用率** | > 70% | > 95% |
| **NVLink 錯誤率** | 0 | > 0 |
| **ECC 錯誤率** | 0 | > 10/hour |

#### 電力指標

| 指標 | 目標值 | 告警閾值 |
|------|--------|---------|
| **PUE** | ≤ 1.3 | > 1.4 |
| **IT 負載** | 2,000 kW | > 2,200 kW |
| **UPS 負載** | < 80% | > 90% |
| **UPS 電池電量** | 100% | < 80% |
| **發電機燃料** | 100% | < 50% |

#### 冷卻指標

| 指標 | 目標值 | 告警閾值 |
|------|--------|---------|
| **Chiller COP** | ≥ 3.5 | < 3.0 |
| **液冷供水溫度** | 25°C | > 27°C |
| **液冷回水溫度** | 45°C | > 50°C |
| **數據大廳溫度** | 24°C | > 26°C |
| **數據大廳濕度** | 45%RH | < 40% or > 50% |

#### 網路指標

| 指標 | 目標值 | 告警閾值 |
|------|--------|---------|
| **InfiniBand 使用率** | < 70% | > 80% |
| **Packet Loss** | 0% | > 0.01% |
| **延遲** | < 1 μs | > 2 μs |
| **錯誤率** | 0 | > 0 |

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
| **Critical** | 嚴重影響運算 | < 5 分鐘 | GPU 故障、UPS 故障、冷卻系統故障 |
| **High** | 影響運算 | < 15 分鐘 | 高溫告警、網路錯誤 |
| **Medium** | 可能影響運算 | < 1 小時 | 使用率過高、預測性維護 |
| **Low** | 不影響運算 | < 4 小時 | 設備維護提醒 |

---

## 應用場景

### 1. 大型語言模型 (LLM) 訓練

**模型規模**：
- GPT-4 等級：1.76 兆參數
- 訓練數據：10 TB
- 訓練時間：30 天

**效能**：
- 28.8 ExaFLOPS (FP8)
- 207 TB GPU 記憶體
- 1.8 TB/s NVLink 頻寬

**經濟效益**：
- 訓練成本：$500,000 (電力 + 折舊)
- 市場價值：$10,000,000+
- ROI：2,000%

### 2. AI 推理服務

**服務規模**：
- 每秒請求數：100,000 QPS
- 平均延遲：< 100 ms
- 同時在線模型：10 個

**效能**：
- 7.2 ExaFLOPS (FP16)
- 低延遲推理
- 高吞吐量

**經濟效益**：
- 每月收入：$1,000,000
- 運營成本：$200,000
- 毛利率：80%

### 3. 科學計算與模擬

**應用領域**：
- 氣候模擬
- 藥物發現
- 材料科學
- 流體力學

**效能**：
- 3.6 ExaFLOPS (FP32)
- 高精度計算
- 大規模並行

### 4. 多租戶雲服務

**租戶數量**：20-50 個
**資源分配**：動態調度
**計費模式**：按使用量計費

---

## 附錄

### A. 設備清單

完整的設備清單請參考 FDL 檔案。

### B. 技術規格表

完整的技術規格表請參考 IADL 檔案。

### C. 參考文獻

1. "NVIDIA GB300 Grace Blackwell Superchip", NVIDIA, 2024
2. "Data Center Design Best Practices", Uptime Institute, 2024
3. "Liquid Cooling for AI Data Centers", ASHRAE, 2024
4. "InfiniBand Architecture Specification", InfiniBand Trade Association, 2024
5. "IDTF V3.5 Specification", IDTF Consortium, 2025

### D. 縮寫對照表

| 縮寫 | 全名 | 中文 |
|------|------|------|
| GB300 | Grace Blackwell 300 | Grace Blackwell 300 |
| NVL72 | NVIDIA Link 72 | NVIDIA Link 72 |
| HBM3e | High Bandwidth Memory 3 Enhanced | 高頻寬記憶體 3 增強版 |
| LPDDR5X | Low Power DDR5 Extended | 低功耗 DDR5 擴展版 |
| NDR | Next Data Rate | 下一代數據速率 |
| CDU | Coolant Distribution Unit | 冷卻液分配單元 |
| CRAC | Computer Room Air Conditioning | 機房空調 |
| DCIM | Data Center Infrastructure Management | 數據中心基礎設施管理 |
| PUE | Power Usage Effectiveness | 電力使用效率 |
| COP | Coefficient of Performance | 性能係數 |

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

