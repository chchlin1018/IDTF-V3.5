# 宏齊科技廠房 Digital Twins 系統設計方案

**基於 IDTF V3.1 架構**

---

**版本**: 1.0  
**日期**: 2025-10-12  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin) (Michael Lin)  
**公司**: 宏齊科技股份有限公司 (Harvatek Corporation)

---

## 執行摘要

本文件基於最新的 **IDTF V3.1** (Industrial Digital Twin Framework) 架構,為宏齊科技新竹科學園區 LED 封裝廠房設計一套完整的 Digital Twins 系統整合方案。該方案涵蓋從設計、建造到運維的全生命週期,整合 IADL V3.1、NDH V3.1、IADL Editor V3.1 和 Omniverse,實現工廠的完整數位化轉型。

**核心目標**:
- 建立高保真度的廠房 3D 數位分身
- 整合所有生產設備和設施系統的實時數據
- 實現預測性維護和智能優化
- 支援 Mini LED / Micro LED 新產線擴建
- 降低運營成本,提升生產效率

**預期效益**:
- 設備綜合效率 (OEE) 提升 15-20%
- 能源成本降低 12-18%
- 非計劃停機時間減少 30-40%
- 新產線建置時間縮短 25-30%
- 維護成本降低 20-25%

---

## 1. 宏齊科技廠房概況

### 1.1 公司背景

**宏齊科技股份有限公司** (Harvatek Corporation, 股票代碼: 6168) 成立於 1995 年,是台灣領先的 LED 封裝廠商,專注於表面黏著型 (SMD) LED 的設計、製造和測試。

**核心業務**:
- LED 封裝 (Lighting LED, Display LED, Infrared LED)
- Mini LED / Micro LED 先進封裝技術
- COB (Chip on Board) LED 顯示屏
- VCSEL (垂直腔面發射雷射) 紅外線技術

**市場定位**:
- 全球 LED 封裝產能前 20 名
- Mini LED 和 Micro LED 技術領先者
- 2025 年大阪世博台灣館 COB LED 顯示屏供應商

### 1.2 廠房設施

**新竹科學園區廠房**:
- **地址**: 新竹縣寶山鄉創新一路 7 號
- **樓層**: 地下 1 層,地上 5 層
- **總面積**: 約 15,000 平方米
- **產能**: 27 億顆 LED / 月 (2010 年完工時)
- **員工**: 約 800 人

**廠房區域劃分**:
1. **生產區域** (2-4 樓):
   - 晶片分選區 (Die Sorting)
   - 固晶區 (Die Bonding)
   - 打線區 (Wire Bonding)
   - 封膠區 (Encapsulation)
   - 測試區 (Testing)
   - 包裝區 (Packaging)

2. **潔淨室** (Class 10,000 - 100,000):
   - 溫度: 22±2°C
   - 濕度: 50±10% RH
   - 正壓: 5-15 Pa

3. **設施系統**:
   - 空調系統 (HVAC)
   - 純水系統 (DI Water)
   - 氮氣系統 (N2)
   - 壓縮空氣系統 (CDA)
   - 廢氣處理系統 (Scrubber)
   - 廢水處理系統
   - 消防系統
   - 電力系統 (UPS, 緊急發電機)

4. **辦公區域** (1 樓, 5 樓):
   - 研發實驗室
   - 品質實驗室
   - 辦公室
   - 會議室

5. **倉儲物流** (地下 1 樓, 1 樓):
   - 原物料倉庫
   - 成品倉庫
   - 收發貨區

### 1.3 生產設備

**主要設備類型** (約 500 台):

1. **晶片分選設備** (Die Sorter): 20 台
   - 品牌: ASM, K&S
   - 功能: 晶片挑選、測試、分類

2. **固晶設備** (Die Bonder): 50 台
   - 品牌: ASM, K&S, Palomar
   - 功能: 晶片貼裝到基板

3. **打線設備** (Wire Bonder): 80 台
   - 品牌: K&S, ASM, Shinkawa
   - 功能: 金線或銀線連接

4. **封膠設備** (Dispenser): 40 台
   - 品牌: Musashi, Nordson
   - 功能: 環氧樹脂封裝

5. **測試設備** (Tester): 100 台
   - 品牌: Chroma, Keithley
   - 功能: 光電特性測試

6. **包裝設備** (Taping Machine): 30 台
   - 品牌: ASM, Royce
   - 功能: 編帶包裝

7. **檢測設備** (AOI, X-Ray): 20 台
   - 品牌: Saki, Nordson
   - 功能: 外觀和內部缺陷檢測

8. **設施設備**: 160 台
   - 空調箱 (AHU): 40 台
   - 冰水主機 (Chiller): 10 台
   - 純水機: 5 台
   - 氮氣產生器: 3 台
   - 空壓機: 8 台
   - UPS: 5 台
   - 廢氣處理設備: 10 台

### 1.4 現有 IT/OT 系統

**IT 系統**:
- ERP: SAP 或鼎新 ERP
- MES: 自行開發或第三方 MES
- PLM: 產品生命週期管理
- WMS: 倉儲管理系統
- QMS: 品質管理系統

**OT 系統**:
- SCADA: 監控與數據採集系統
- PLC: 設備控制器 (Siemens, Mitsubishi, Omron)
- 設備通訊: SECS/GEM, OPC UA, Modbus
- 環境監控: 溫濕度、潔淨度、壓差

**現有挑戰**:
- IT 和 OT 系統孤島,數據未整合
- 缺乏統一的資產管理平台
- 設備維護多為被動式,缺乏預測能力
- 能源管理不夠精細
- 新產線規劃缺乏 3D 視覺化工具
- 設施系統與生產設備未聯動優化

---

## 2. IDTF V3.1 整體架構設計

### 2.1 系統架構圖

```
┌─────────────────────────────────────────────────────────────────┐
│                    宏齊科技 Digital Twin 系統                      │
│                      (基於 IDTF V3.1)                            │
└─────────────────────────────────────────────────────────────────┘
                                 │
        ┌────────────────────────┼────────────────────────┐
        │                        │                        │
        ▼                        ▼                        ▼
┌───────────────┐       ┌───────────────┐       ┌───────────────┐
│  IADL V3.1    │       │  NDH V3.1     │       │  Omniverse    │
│ 資產描述層     │◄─────►│ 數據中樞層     │◄─────►│  視覺化層     │
│               │       │               │       │               │
│ • 設備定義    │       │ • Kafka       │       │ • USD 場景    │
│ • 設施定義    │       │ • InfluxDB    │       │ • 3D 渲染     │
│ • 管線定義    │       │ • PostgreSQL  │       │ • 實時協同    │
│ • 3D 模型連結 │       │ • Connectors  │       │ • 模擬分析    │
└───────────────┘       └───────────────┘       └───────────────┘
        │                        │                        │
        │                        │                        │
        └────────────────────────┼────────────────────────┘
                                 │
        ┌────────────────────────┼────────────────────────┐
        │                        │                        │
        ▼                        ▼                        ▼
┌───────────────┐       ┌───────────────┐       ┌───────────────┐
│ IADL Editor   │       │ NDH Monitor   │       │ Omniverse     │
│ V3.1          │       │ & Configurator│       │ Applications  │
│               │       │               │       │               │
│ • 資產建模    │       │ • 實時監控    │       │ • 工廠漫遊    │
│ • MEP 設計    │       │ • 數據分析    │       │ • 設備狀態    │
│ • 衝突檢測    │       │ • 告警管理    │       │ • 能源視覺化  │
│ • ISO 圖面    │       │ • 系統配置    │       │ • 模擬訓練    │
└───────────────┘       └───────────────┘       └───────────────┘
                                 │
        ┌────────────────────────┼────────────────────────┐
        │                        │                        │
        ▼                        ▼                        ▼
┌───────────────┐       ┌───────────────┐       ┌───────────────┐
│  OT 層        │       │  IT 層        │       │  外部系統     │
│               │       │               │       │               │
│ • 生產設備    │       │ • ERP (SAP)   │       │ • RIB 4.0     │
│ • SCADA       │       │ • MES         │       │ • ETAP        │
│ • PLC         │       │ • PLM         │       │ • 供應商      │
│ • 感測器      │       │ • WMS/QMS     │       │ • 客戶        │
└───────────────┘       └───────────────┘       └───────────────┘
```

### 2.2 核心組件說明

#### 2.2.1 IADL V3.1 資產描述層

**功能**:
- 定義廠房所有資產的數位化描述
- 連接 3D 模型 (OpenUSD 格式)
- 定義資產屬性、關係和行為
- 支援版本控制和變更追蹤

**資產類型**:
1. **生產設備**: Die Sorter, Die Bonder, Wire Bonder, Dispenser, Tester, Taping Machine
2. **設施設備**: AHU, Chiller, DI Water System, N2 Generator, Compressor, UPS, Scrubber
3. **建築結構**: 建築物、樓層、房間、潔淨室
4. **MEP 系統**: 管線、閥門、風管、電纜、配電盤
5. **感測器**: 溫度、濕度、壓力、流量、電力、振動

#### 2.2.2 NDH V3.1 數據中樞層

**功能**:
- 整合所有 IT 和 OT 系統的數據
- 實時數據流處理和儲存
- 歷史數據查詢和分析
- 告警和事件管理

**技術架構**:
- **Kafka**: 實時數據流 (100,000+ events/sec)
- **InfluxDB**: 時序數據儲存 (感測器數據、設備狀態)
- **PostgreSQL**: 關聯數據儲存 (資產定義、維護記錄)
- **Connectors**: OPC UA, SECS/GEM, Modbus, MQTT, REST API

**數據來源**:
- 生產設備 (500+ 台)
- SCADA 系統
- PLC 控制器
- 環境感測器 (1,000+ 點)
- ERP / MES / PLM
- 能源監控系統

#### 2.2.3 Omniverse 視覺化層

**功能**:
- 高保真度 3D 廠房模型
- 實時數據視覺化
- 多使用者協同
- 模擬和訓練

**應用場景**:
1. **工廠漫遊**: 虛擬參觀廠房
2. **設備監控**: 實時設備狀態 3D 視覺化
3. **能源管理**: 能源流動和消耗視覺化
4. **維護管理**: 維護任務 3D 指引
5. **新產線規劃**: 3D 佈局模擬和衝突檢測
6. **培訓**: 虛擬實境操作培訓

---

## 3. IADL V3.1 資產定義

### 3.1 資產架構

```
宏齊科技新竹廠
├── 建築物 (Building)
│   ├── 地下 1 樓 (B1)
│   │   ├── 倉庫區
│   │   └── 機房
│   ├── 1 樓 (1F)
│   │   ├── 辦公區
│   │   ├── 收發貨區
│   │   └── 設施機房
│   ├── 2 樓 (2F) - 生產區
│   │   ├── 晶片分選區
│   │   ├── 固晶區
│   │   └── 打線區
│   ├── 3 樓 (3F) - 生產區
│   │   ├── 封膠區
│   │   ├── 測試區
│   │   └── 包裝區
│   ├── 4 樓 (4F) - 生產區
│   │   ├── Mini LED 產線
│   │   └── Micro LED 產線
│   └── 5 樓 (5F)
│       ├── 研發實驗室
│       └── 辦公區
├── 生產設備 (Production Equipment)
│   ├── Die Sorter (20 台)
│   ├── Die Bonder (50 台)
│   ├── Wire Bonder (80 台)
│   ├── Dispenser (40 台)
│   ├── Tester (100 台)
│   ├── Taping Machine (30 台)
│   └── AOI / X-Ray (20 台)
├── 設施系統 (Facility Systems)
│   ├── HVAC 系統
│   │   ├── 冰水主機 (10 台)
│   │   ├── 空調箱 (40 台)
│   │   ├── FFU (1,000+ 台)
│   │   └── 排風機
│   ├── 純水系統 (5 台)
│   ├── 氮氣系統 (3 台)
│   ├── 壓縮空氣系統 (8 台)
│   ├── 電力系統
│   │   ├── 變壓器
│   │   ├── 配電盤
│   │   ├── UPS (5 台)
│   │   └── 發電機 (2 台)
│   ├── 廢氣處理系統 (10 台)
│   └── 廢水處理系統 (2 套)
└── 感測器網路 (Sensor Network)
    ├── 環境感測器 (溫度、濕度、壓差、潔淨度)
    ├── 能源感測器 (電力、水、氣)
    ├── 設備感測器 (振動、溫度、電流)
    └── 安全感測器 (煙霧、氣體、門禁)
```

### 3.2 IADL 範例: Wire Bonder 設備

```yaml
# IADL 資產定義 - 打線機
asset_id: WB-2F-001
asset_type: WireBonder
manufacturer: Kulicke & Soffa (K&S)
model: MaxumUltra
serial_number: KS-2023-WB-001
description: 金線打線機,用於 LED 封裝金線連接

# 基本屬性
properties:
  installation_date: 2020-03-15
  warranty_expiry_date: 2025-03-15
  purchase_cost_usd: 450000
  location:
    building: Harvatek-Hsinchu
    floor: 2
    room: Wire-Bonding-Area-A
    coordinates:
      x: 25.3
      y: 15.7
      z: 0.0
  specifications:
    bonding_speed_uph: 8000  # Units per hour
    wire_diameter_um: 25.4   # Gold wire diameter
    bonding_force_gf: 30-80  # Gram force
    accuracy_um: 3           # ±3 μm
    power_consumption_kw: 5.5
    dimensions_mm:
      length: 1200
      width: 900
      height: 1800
    weight_kg: 850

# 3D 模型定義 (OpenUSD)
geometry:
  usd_reference: omniverse://harvatek/assets/equipment/wire_bonder_ks_maxum.usd
  alternative_formats:
    - type: glTF
      path: /models/wb_2f_001.gltf
    - type: STEP
      path: /models/wb_2f_001.step
  transform:
    translation: [25.3, 15.7, 0.0]
    rotation: [0.0, 0.0, 180.0]
    scale: [1.0, 1.0, 1.0]
  lod_levels:
    - level: high
      usd_reference: omniverse://harvatek/assets/equipment/wire_bonder_ks_maxum_high.usd
      max_distance: 5.0
    - level: medium
      usd_reference: omniverse://harvatek/assets/equipment/wire_bonder_ks_maxum_med.usd
      max_distance: 20.0
    - level: low
      usd_reference: omniverse://harvatek/assets/equipment/wire_bonder_ks_maxum_low.usd
      max_distance: 100.0
  collision:
    type: box
    dimensions: [1.2, 0.9, 1.8]

# 材質
materials:
  body:
    usd_material: omniverse://harvatek/materials/equipment_white.mdl
    color: [0.95, 0.95, 0.95, 1.0]
  window:
    usd_material: omniverse://harvatek/materials/glass_transparent.mdl
    color: [0.8, 0.8, 1.0, 0.3]

# 連接點定義
connection_points:
  - id: power_inlet
    type: electrical
    voltage: 220V
    current: 25A
    position: [-0.6, -0.45, 0.3]
  - id: n2_inlet
    type: gas
    gas_type: N2
    pressure_bar: 6.0
    flow_rate_lpm: 50
    position: [-0.6, 0.45, 0.3]
  - id: cda_inlet
    type: gas
    gas_type: compressed_air
    pressure_bar: 6.0
    flow_rate_lpm: 100
    position: [-0.6, 0.45, 0.5]
  - id: exhaust
    type: exhaust
    flow_rate_cmm: 5.0
    position: [0.6, 0.0, 1.7]

# 與其他資產的連接
connections:
  - type: powered_by
    target_asset_id: PANEL-2F-A-03
  - type: supplied_by
    target_asset_id: N2-SYSTEM-MAIN
    connection_point: n2_inlet
  - type: supplied_by
    target_asset_id: CDA-SYSTEM-MAIN
    connection_point: cda_inlet
  - type: exhausted_to
    target_asset_id: EXHAUST-DUCT-2F-A
    connection_point: exhaust
  - type: controlled_by
    target_asset_id: PLC-2F-001
  - type: monitored_by
    target_asset_id: SCADA-SYSTEM

# OT 數據標籤對應
data_tags:
  # 設備狀態
  - tag_id: WB_2F_001.Status
    protocol: SECS_GEM
    equipment_id: WB-2F-001
    variable_id: SVID_1
    data_type: string
    enum_values: [IDLE, RUNNING, ALARM, MAINTENANCE]
    update_rate_ms: 1000
  
  # 生產數據
  - tag_id: WB_2F_001.UPH
    protocol: SECS_GEM
    equipment_id: WB-2F-001
    variable_id: SVID_10
    data_type: integer
    unit: units/hour
    update_rate_ms: 5000
  
  - tag_id: WB_2F_001.TotalCount
    protocol: SECS_GEM
    equipment_id: WB-2F-001
    variable_id: SVID_11
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: WB_2F_001.GoodCount
    protocol: SECS_GEM
    equipment_id: WB-2F-001
    variable_id: SVID_12
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: WB_2F_001.RejectCount
    protocol: SECS_GEM
    equipment_id: WB-2F-001
    variable_id: SVID_13
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  # 設備參數
  - tag_id: WB_2F_001.BondingForce
    protocol: SECS_GEM
    equipment_id: WB-2F-001
    variable_id: SVID_20
    data_type: float
    unit: gf
    update_rate_ms: 2000
  
  - tag_id: WB_2F_001.Temperature
    protocol: SECS_GEM
    equipment_id: WB-2F-001
    variable_id: SVID_21
    data_type: float
    unit: celsius
    update_rate_ms: 5000
  
  # 能源數據
  - tag_id: WB_2F_001.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.WB_2F_001.Power
    data_type: float
    unit: kW
    update_rate_ms: 5000
  
  # 告警
  - tag_id: WB_2F_001.Alarm
    protocol: SECS_GEM
    equipment_id: WB-2F-001
    event_id: ALID_ALL
    data_type: string
    update_rate_ms: 500

# 行為模型
behavior_model:
  type: PythonScript
  path: /models/wire_bonder_performance.py
  inputs:
    - uph
    - bonding_force
    - temperature
  outputs:
    - oee
    - quality_rate
    - predicted_maintenance_date
  update_frequency_hz: 0.1  # 每 10 秒更新一次

# Omniverse 整合
omniverse:
  scene_path: /World/Harvatek/Floor_2/Wire_Bonding_Area_A/WB_2F_001
  
  # 實時數據綁定
  data_bindings:
    - usd_attribute: /WB_2F_001/Status/Color
      data_tag: WB_2F_001.Status
      mapping:
        IDLE: [0.5, 0.5, 0.5, 1.0]      # 灰色
        RUNNING: [0.0, 1.0, 0.0, 1.0]   # 綠色
        ALARM: [1.0, 0.0, 0.0, 1.0]     # 紅色
        MAINTENANCE: [1.0, 1.0, 0.0, 1.0]  # 黃色
    
    - usd_attribute: /WB_2F_001/Display/UPH
      data_tag: WB_2F_001.UPH
      format: "{value} UPH"
    
    - usd_attribute: /WB_2F_001/Display/OEE
      data_tag: WB_2F_001.OEE
      format: "OEE: {value:.1f}%"
  
  # 動畫
  animations:
    - name: bonding_head_motion
      usd_prim: /WB_2F_001/BondingHead
      type: translation
      axis: [0, 0, 1]
      amplitude: 0.05
      frequency_source: WB_2F_001.UPH
      frequency_multiplier: 0.001

# 維護資訊
maintenance:
  maintenance_strategy: predictive
  last_pm_date: 2024-09-01
  next_pm_date: 2025-03-01
  pm_interval_days: 180
  mtbf_hours: 8760  # Mean Time Between Failures
  mttr_hours: 4     # Mean Time To Repair
  
  maintenance_procedures:
    - id: PM-WB-001
      name: 金線更換
      frequency_days: 7
      estimated_duration_hours: 0.5
    
    - id: PM-WB-002
      name: 劈刀更換
      frequency_days: 30
      estimated_duration_hours: 1.0
    
    - id: PM-WB-003
      name: 光學系統校正
      frequency_days: 90
      estimated_duration_hours: 2.0
    
    - id: PM-WB-004
      name: 全面保養
      frequency_days: 180
      estimated_duration_hours: 8.0
  
  spare_parts:
    - part_id: SP-WB-001
      name: 金線 (25.4μm)
      quantity_on_hand: 50
      reorder_point: 20
      lead_time_days: 7
    
    - part_id: SP-WB-002
      name: 劈刀
      quantity_on_hand: 10
      reorder_point: 5
      lead_time_days: 14

# 文件連結
documents:
  - type: manual
    title: 操作手冊
    path: /documents/KS_MaxumUltra_Operation_Manual.pdf
  
  - type: manual
    title: 維護手冊
    path: /documents/KS_MaxumUltra_Maintenance_Manual.pdf
  
  - type: drawing
    title: 設備圖面
    path: /documents/WB_2F_001_Drawing.pdf
    usd_reference: omniverse://harvatek/documents/WB_2F_001_Drawing.usd
  
  - type: sop
    title: 標準作業程序
    path: /documents/SOP_Wire_Bonding.pdf

# MES 整合
mes_integration:
  equipment_id: WB-2F-001
  work_center: WB-AREA-A
  resource_type: WIRE_BONDER
  capacity_uph: 8000
  
  # 配方管理
  recipes:
    - recipe_id: RCP-LED-3528
      product_type: LED-3528
      wire_diameter_um: 25.4
      bonding_force_gf: 50
      bonding_time_ms: 30
    
    - recipe_id: RCP-LED-5050
      product_type: LED-5050
      wire_diameter_um: 25.4
      bonding_force_gf: 60
      bonding_time_ms: 35

# ERP 整合
erp_integration:
  asset_number: FA-WB-2F-001
  cost_center: CC-2F-PRODUCTION
  depreciation_method: straight_line
  depreciation_period_years: 7
  residual_value_usd: 45000

# 元數據
metadata:
  created_date: 2020-03-01
  created_by: engineer@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: engineer@harvatek.com
  version: 3.2
  tags:
    - production_equipment
    - wire_bonding
    - critical_equipment
    - predictive_maintenance
```

### 3.3 IADL 範例: HVAC 空調箱

```yaml
# IADL 資產定義 - 空調箱
asset_id: AHU-2F-A-01
asset_type: AirHandlingUnit
manufacturer: TRANE
model: AHU-2000
serial_number: TRANE-2019-AHU-001
description: 2 樓 A 區潔淨室空調箱

# 基本屬性
properties:
  installation_date: 2019-06-01
  location:
    building: Harvatek-Hsinchu
    floor: 2
    room: AHU-Room-A
    coordinates:
      x: 5.0
      y: 2.0
      z: 0.0
  specifications:
    air_flow_cmm: 200  # Cubic meters per minute
    cooling_capacity_kw: 50
    heating_capacity_kw: 30
    fan_power_kw: 15
    filter_type: HEPA_H14
    dimensions_mm:
      length: 4000
      width: 2000
      height: 2500

# 3D 模型定義
geometry:
  usd_reference: omniverse://harvatek/assets/facility/ahu_trane_2000.usd
  transform:
    translation: [5.0, 2.0, 0.0]
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]

# 連接點定義 (MEP)
connection_points:
  # 冰水管
  - id: chilled_water_inlet
    type: pipe
    pipe_type: chilled_water
    nominal_diameter: DN100
    position: [-2.0, 0.0, 0.5]
    orientation: [-1.0, 0.0, 0.0]
    usd_prim_path: /AHU_2F_A_01/Piping/CHW_Inlet
  
  - id: chilled_water_outlet
    type: pipe
    pipe_type: chilled_water_return
    nominal_diameter: DN100
    position: [-2.0, 0.0, 1.0]
    orientation: [-1.0, 0.0, 0.0]
    usd_prim_path: /AHU_2F_A_01/Piping/CHW_Outlet
  
  # 風管
  - id: supply_air_outlet
    type: duct
    duct_type: supply_air
    dimensions: [800, 600]  # mm x mm
    position: [2.0, 0.0, 2.0]
    orientation: [1.0, 0.0, 0.0]
    usd_prim_path: /AHU_2F_A_01/Ductwork/Supply_Air
  
  - id: return_air_inlet
    type: duct
    duct_type: return_air
    dimensions: [800, 600]
    position: [-2.0, 0.0, 2.0]
    orientation: [-1.0, 0.0, 0.0]
    usd_prim_path: /AHU_2F_A_01/Ductwork/Return_Air
  
  # 電力
  - id: power_inlet
    type: electrical
    voltage: 380V
    current: 50A
    position: [-2.0, -1.0, 0.3]

# 與其他資產的連接
connections:
  - type: supplied_by
    target_asset_id: CHILLER-01
    connection_point: chilled_water_inlet
    target_connection_point: chw_secondary_outlet
  
  - type: supplies_to
    target_asset_id: CLEANROOM-2F-A
    connection_point: supply_air_outlet
  
  - type: powered_by
    target_asset_id: PANEL-2F-FACILITY-01
  
  - type: controlled_by
    target_asset_id: BMS-SYSTEM

# OT 數據標籤對應
data_tags:
  # 運行狀態
  - tag_id: AHU_2F_A_01.Status
    protocol: BACnet
    device_id: 101
    object_type: BINARY_VALUE
    object_instance: 1
    data_type: boolean
    mapping:
      true: RUNNING
      false: STOPPED
    update_rate_ms: 5000
  
  # 溫度
  - tag_id: AHU_2F_A_01.SupplyAirTemp
    protocol: BACnet
    device_id: 101
    object_type: ANALOG_INPUT
    object_instance: 1
    data_type: float
    unit: celsius
    update_rate_ms: 10000
  
  - tag_id: AHU_2F_A_01.ReturnAirTemp
    protocol: BACnet
    device_id: 101
    object_type: ANALOG_INPUT
    object_instance: 2
    data_type: float
    unit: celsius
    update_rate_ms: 10000
  
  # 濕度
  - tag_id: AHU_2F_A_01.SupplyAirHumidity
    protocol: BACnet
    device_id: 101
    object_type: ANALOG_INPUT
    object_instance: 3
    data_type: float
    unit: percent_rh
    update_rate_ms: 10000
  
  # 風量
  - tag_id: AHU_2F_A_01.AirFlow
    protocol: BACnet
    device_id: 101
    object_type: ANALOG_INPUT
    object_instance: 4
    data_type: float
    unit: cmm
    update_rate_ms: 10000
  
  # 壓差
  - tag_id: AHU_2F_A_01.FilterPressureDrop
    protocol: BACnet
    device_id: 101
    object_type: ANALOG_INPUT
    object_instance: 5
    data_type: float
    unit: pa
    update_rate_ms: 10000
  
  # 能源
  - tag_id: AHU_2F_A_01.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.AHU_2F_A_01.Power
    data_type: float
    unit: kW
    update_rate_ms: 5000

# 行為模型
behavior_model:
  type: PythonScript
  path: /models/ahu_performance.py
  inputs:
    - supply_air_temp
    - return_air_temp
    - air_flow
    - power_consumption
  outputs:
    - cooling_efficiency
    - energy_consumption_kwh
    - predicted_filter_change_date
  update_frequency_hz: 0.017  # 每分鐘更新一次

# Omniverse 整合
omniverse:
  scene_path: /World/Harvatek/Floor_2/AHU_Room_A/AHU_2F_A_01
  
  data_bindings:
    - usd_attribute: /AHU_2F_A_01/Status/Color
      data_tag: AHU_2F_A_01.Status
      mapping:
        RUNNING: [0.0, 1.0, 0.0, 1.0]
        STOPPED: [0.5, 0.5, 0.5, 1.0]
    
    - usd_attribute: /AHU_2F_A_01/Display/SupplyTemp
      data_tag: AHU_2F_A_01.SupplyAirTemp
      format: "{value:.1f}°C"
    
    - usd_attribute: /AHU_2F_A_01/Display/AirFlow
      data_tag: AHU_2F_A_01.AirFlow
      format: "{value:.0f} CMM"
  
  animations:
    - name: fan_rotation
      usd_prim: /AHU_2F_A_01/Fan/Blades
      type: rotation
      axis: [1, 0, 0]
      speed_source: AHU_2F_A_01.AirFlow
      speed_multiplier: 10.0

# 維護資訊
maintenance:
  maintenance_strategy: preventive
  last_pm_date: 2024-10-01
  next_pm_date: 2025-01-01
  pm_interval_days: 90
  
  maintenance_procedures:
    - id: PM-AHU-001
      name: 濾網更換
      frequency_days: 90
      estimated_duration_hours: 2.0
    
    - id: PM-AHU-002
      name: 風扇皮帶檢查
      frequency_days: 180
      estimated_duration_hours: 1.0
    
    - id: PM-AHU-003
      name: 冰水盤管清洗
      frequency_days: 180
      estimated_duration_hours: 4.0
    
    - id: PM-AHU-004
      name: 全面保養
      frequency_days: 365
      estimated_duration_hours: 8.0

# 元數據
metadata:
  created_date: 2019-05-15
  created_by: facility@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: facility@harvatek.com
  version: 2.5
  tags:
    - facility_equipment
    - hvac
    - cleanroom
    - critical_facility
```

---

## 4. NDH V3.1 數據整合方案

### 4.1 數據架構

```
┌─────────────────────────────────────────────────────────────┐
│                      NDH V3.1 數據中樞                        │
└─────────────────────────────────────────────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│   Kafka      │      │  InfluxDB    │      │ PostgreSQL   │
│ 實時數據流    │      │  時序數據庫   │      │  關聯數據庫   │
│              │      │              │      │              │
│ • 設備狀態   │      │ • 感測器數據 │      │ • 資產定義   │
│ • 生產數據   │      │ • 設備參數   │      │ • 維護記錄   │
│ • 告警事件   │      │ • 能源數據   │      │ • 配方數據   │
│ • 日誌       │      │ • 環境數據   │      │ • 使用者     │
└──────────────┘      └──────────────┘      └──────────────┘
        ▲                     ▲                     ▲
        │                     │                     │
        └─────────────────────┴─────────────────────┘
                              │
        ┌─────────────────────┼─────────────────────┐
        │                     │                     │
        ▼                     ▼                     ▼
┌──────────────┐      ┌──────────────┐      ┌──────────────┐
│OT Connectors │      │IT Connectors │      │ External     │
│              │      │              │      │ Connectors   │
│• SECS/GEM    │      │• SAP API     │      │• Weather API │
│• OPC UA      │      │• MES API     │      │• Supplier    │
│• Modbus      │      │• PLM API     │      │• Customer    │
│• BACnet      │      │• WMS API     │      │              │
│• MQTT        │      │• QMS API     │      │              │
└──────────────┘      └──────────────┘      └──────────────┘
```

### 4.2 數據流設計

#### 4.2.1 生產設備數據流

```
生產設備 (500 台)
    │
    │ SECS/GEM, OPC UA
    ▼
Kafka Topic: equipment.production.events
    │
    ├─► InfluxDB: equipment_data
    │   - 設備狀態 (Status)
    │   - 生產數據 (UPH, Count)
    │   - 設備參數 (Temperature, Force)
    │   - 能源數據 (Power)
    │
    ├─► PostgreSQL: equipment_events
    │   - 狀態變更事件
    │   - 告警事件
    │   - 維護事件
    │
    └─► Omniverse (via WebSocket)
        - 實時 3D 視覺化
        - 設備狀態顏色
        - 動畫驅動
```

#### 4.2.2 設施系統數據流

```
設施設備 (160 台) + 感測器 (1,000+ 點)
    │
    │ BACnet, OPC UA, Modbus
    ▼
Kafka Topic: facility.systems.events
    │
    ├─► InfluxDB: facility_data
    │   - 環境數據 (溫度、濕度、壓差)
    │   - 設備狀態 (運行、停止)
    │   - 能源數據 (電力、水、氣)
    │   - 性能數據 (效率、負載)
    │
    ├─► PostgreSQL: facility_events
    │   - 告警事件
    │   - 維護事件
    │
    └─► Omniverse (via WebSocket)
        - 能源流動視覺化
        - 環境熱圖
        - 設備狀態
```

#### 4.2.3 IT 系統數據流

```
ERP / MES / PLM / WMS / QMS
    │
    │ REST API, Database Connector
    ▼
Kafka Topic: it.systems.events
    │
    ├─► PostgreSQL: business_data
    │   - 工單數據
    │   - 物料數據
    │   - 品質數據
    │   - 維護工單
    │
    └─► InfluxDB: kpi_data
        - OEE
        - 良率
        - 交期達成率
        - 庫存週轉率
```

### 4.3 Kafka Topic 設計

| Topic 名稱 | 分區數 | 保留期 | 數據來源 | 數據類型 |
|-----------|-------|-------|---------|---------|
| `equipment.production.status` | 10 | 7 天 | 生產設備 | 設備狀態變更 |
| `equipment.production.data` | 10 | 30 天 | 生產設備 | 生產數據 (UPH, Count) |
| `equipment.production.alarms` | 5 | 90 天 | 生產設備 | 告警事件 |
| `facility.hvac.data` | 5 | 30 天 | HVAC 系統 | 溫濕度、風量 |
| `facility.power.data` | 5 | 90 天 | 電力系統 | 電力消耗 |
| `facility.utilities.data` | 5 | 30 天 | 公用系統 | 水、氣消耗 |
| `environment.cleanroom.data` | 5 | 30 天 | 環境感測器 | 潔淨度、壓差 |
| `it.mes.workorders` | 3 | 90 天 | MES | 工單數據 |
| `it.erp.materials` | 3 | 90 天 | ERP | 物料數據 |
| `it.qms.quality` | 3 | 365 天 | QMS | 品質數據 |

### 4.4 InfluxDB Schema 設計

#### 4.4.1 設備數據 Measurement

```
measurement: equipment_data

tags:
  - asset_id: 設備 ID (e.g., WB-2F-001)
  - asset_type: 設備類型 (e.g., WireBonder)
  - location: 位置 (e.g., Floor_2_Area_A)
  - manufacturer: 製造商
  - model: 型號

fields:
  - status: 設備狀態 (string)
  - uph: 每小時產量 (integer)
  - total_count: 總產量 (integer)
  - good_count: 良品數 (integer)
  - reject_count: 不良品數 (integer)
  - temperature: 溫度 (float)
  - power_consumption: 功耗 (float)
  - oee: OEE (float)
  - quality_rate: 良率 (float)

time: 時間戳記 (nanosecond precision)
```

#### 4.4.2 環境數據 Measurement

```
measurement: environment_data

tags:
  - location: 位置 (e.g., Floor_2_Cleanroom_A)
  - sensor_id: 感測器 ID
  - sensor_type: 感測器類型

fields:
  - temperature: 溫度 (float, °C)
  - humidity: 濕度 (float, %RH)
  - pressure_diff: 壓差 (float, Pa)
  - particle_count_0_5um: 粒子數 0.5μm (integer)
  - particle_count_5_0um: 粒子數 5.0μm (integer)

time: 時間戳記
```

#### 4.4.3 能源數據 Measurement

```
measurement: energy_data

tags:
  - asset_id: 資產 ID
  - asset_type: 資產類型
  - energy_type: 能源類型 (electricity, water, gas, n2, cda)
  - location: 位置

fields:
  - instantaneous_value: 瞬時值 (float)
  - cumulative_value: 累積值 (float)
  - cost: 成本 (float)

time: 時間戳記
```

### 4.5 PostgreSQL Schema 設計

#### 4.5.1 資產表 (assets)

```sql
CREATE TABLE assets (
    asset_id VARCHAR(50) PRIMARY KEY,
    asset_type VARCHAR(50) NOT NULL,
    manufacturer VARCHAR(100),
    model VARCHAR(100),
    serial_number VARCHAR(100),
    description TEXT,
    location_building VARCHAR(50),
    location_floor INTEGER,
    location_room VARCHAR(100),
    location_x FLOAT,
    location_y FLOAT,
    location_z FLOAT,
    installation_date DATE,
    purchase_cost_usd DECIMAL(12, 2),
    usd_reference VARCHAR(500),
    iadl_definition JSONB,
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_modified_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    version VARCHAR(20),
    tags TEXT[]
);

CREATE INDEX idx_assets_type ON assets(asset_type);
CREATE INDEX idx_assets_location ON assets(location_building, location_floor);
CREATE INDEX idx_assets_tags ON assets USING GIN(tags);
```

#### 4.5.2 維護記錄表 (maintenance_records)

```sql
CREATE TABLE maintenance_records (
    record_id SERIAL PRIMARY KEY,
    asset_id VARCHAR(50) REFERENCES assets(asset_id),
    maintenance_type VARCHAR(50),  -- preventive, corrective, predictive
    procedure_id VARCHAR(50),
    procedure_name VARCHAR(200),
    scheduled_date DATE,
    actual_start_time TIMESTAMP,
    actual_end_time TIMESTAMP,
    duration_hours FLOAT,
    technician VARCHAR(100),
    status VARCHAR(20),  -- scheduled, in_progress, completed, cancelled
    notes TEXT,
    spare_parts_used JSONB,
    cost_usd DECIMAL(10, 2),
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_maintenance_asset ON maintenance_records(asset_id);
CREATE INDEX idx_maintenance_date ON maintenance_records(scheduled_date);
CREATE INDEX idx_maintenance_status ON maintenance_records(status);
```

#### 4.5.3 告警事件表 (alarm_events)

```sql
CREATE TABLE alarm_events (
    event_id SERIAL PRIMARY KEY,
    asset_id VARCHAR(50) REFERENCES assets(asset_id),
    alarm_code VARCHAR(50),
    alarm_text TEXT,
    severity VARCHAR(20),  -- critical, high, medium, low
    occurrence_time TIMESTAMP NOT NULL,
    acknowledgement_time TIMESTAMP,
    clear_time TIMESTAMP,
    acknowledged_by VARCHAR(100),
    status VARCHAR(20),  -- active, acknowledged, cleared
    root_cause TEXT,
    corrective_action TEXT,
    created_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX idx_alarm_asset ON alarm_events(asset_id);
CREATE INDEX idx_alarm_time ON alarm_events(occurrence_time);
CREATE INDEX idx_alarm_status ON alarm_events(status);
CREATE INDEX idx_alarm_severity ON alarm_events(severity);
```

### 4.6 數據保留策略

| 數據類型 | 熱數據保留期 | 溫數據保留期 | 冷數據保留期 | 總保留期 |
|---------|------------|------------|------------|---------|
| 設備狀態 | 7 天 (Kafka) | 30 天 (InfluxDB) | 1 年 (S3) | 1 年 |
| 生產數據 | 7 天 | 90 天 | 3 年 | 3 年 |
| 環境數據 | 7 天 | 30 天 | 1 年 | 1 年 |
| 能源數據 | 7 天 | 90 天 | 5 年 | 5 年 |
| 告警事件 | 30 天 | 1 年 | 5 年 | 5 年 |
| 維護記錄 | - | - | 永久 (PostgreSQL) | 永久 |
| 品質數據 | 30 天 | 1 年 | 10 年 | 10 年 |

---

## 5. Omniverse 視覺化應用

### 5.1 3D 場景架構

```
/World/Harvatek
├── /Building
│   ├── /Structure (建築結構)
│   ├── /B1_Floor (地下 1 樓)
│   ├── /1F_Floor (1 樓)
│   ├── /2F_Floor (2 樓)
│   │   ├── /Die_Sorting_Area
│   │   ├── /Die_Bonding_Area
│   │   └── /Wire_Bonding_Area
│   │       ├── /WB_2F_001 (設備)
│   │       ├── /WB_2F_002
│   │       └── ...
│   ├── /3F_Floor (3 樓)
│   ├── /4F_Floor (4 樓)
│   └── /5F_Floor (5 樓)
├── /MEP_Systems
│   ├── /HVAC
│   │   ├── /Chillers
│   │   ├── /AHUs
│   │   ├── /Ductwork
│   │   └── /Piping
│   ├── /Electrical
│   │   ├── /Transformers
│   │   ├── /Panels
│   │   └── /Cables
│   ├── /Plumbing
│   │   ├── /DI_Water
│   │   ├── /N2_System
│   │   └── /CDA_System
│   └── /Fire_Protection
├── /Sensors
│   ├── /Environment_Sensors
│   ├── /Power_Meters
│   └── /Flow_Meters
├── /Lighting
│   ├── /Ambient_Light
│   └── /Area_Lights
└── /UI_Overlays
    ├── /Equipment_Status_Panels
    ├── /KPI_Dashboards
    └── /Alarm_Notifications
```

### 5.2 應用場景

#### 5.2.1 工廠漫遊 (Factory Tour)

**功能**:
- 虛擬參觀廠房各區域
- 查看設備佈局和 MEP 系統
- 了解生產流程

**使用者**:
- 客戶參訪
- 新進員工培訓
- 遠端稽核

**實現**:
- Omniverse Create / View
- 預設相機路徑
- 語音導覽 (可選)

#### 5.2.2 設備監控 (Equipment Monitoring)

**功能**:
- 實時設備狀態 3D 視覺化
- 設備顏色根據狀態變化 (綠/黃/紅)
- 顯示關鍵參數 (UPH, OEE, 溫度)
- 設備動畫 (運轉時旋轉、移動)

**使用者**:
- 生產主管
- 設備工程師
- 維護人員

**實現**:
- Omniverse Kit Extension
- WebSocket 實時數據綁定
- USD Attribute 動態更新

#### 5.2.3 能源管理 (Energy Management)

**功能**:
- 能源流動視覺化 (電力、水、氣)
- 能源消耗熱圖
- 實時功率顯示
- 能源成本分析

**使用者**:
- 設施管理人員
- 能源管理團隊
- 財務部門

**實現**:
- 粒子系統模擬能源流動
- 熱圖 Shader
- 數據綁定到 USD

#### 5.2.4 維護管理 (Maintenance Management)

**功能**:
- 維護任務 3D 定位
- 維護步驟 AR 指引
- 備品備件位置顯示
- 維護歷史查詢

**使用者**:
- 維護技術人員
- 維護主管

**實現**:
- Omniverse AR 應用
- 維護程序動畫
- 與 CMMS 系統整合

#### 5.2.5 新產線規劃 (New Line Planning)

**功能**:
- 3D 設備佈局模擬
- 自動衝突檢測
- 人流、物流模擬
- MEP 系統路由規劃

**使用者**:
- 工廠規劃工程師
- 設施工程師
- 專案經理

**實現**:
- IADL Editor V3.1 MEP 模組
- Omniverse Connector
- 衝突檢測 API

#### 5.2.6 培訓與模擬 (Training & Simulation)

**功能**:
- 設備操作 VR 培訓
- 緊急應變模擬
- 新產品試產模擬
- 產線平衡分析

**使用者**:
- 操作人員
- 工程師
- 管理層

**實現**:
- Omniverse VR 應用
- 物理模擬
- 行為模型

### 5.3 實時數據綁定範例

```python
# Omniverse Kit Extension - 實時數據綁定
import omni.kit.app
import omni.usd
from pxr import Usd, UsdGeom, Gf
import asyncio
import websocket
import json

class RealtimeDataBinding:
    def __init__(self):
        self.stage = omni.usd.get_context().get_stage()
        self.ws = None
        self.bindings = {}
    
    def add_binding(self, usd_prim_path, attribute_name, data_tag, mapping=None):
        """
        添加數據綁定
        
        Args:
            usd_prim_path: USD Prim 路徑
            attribute_name: USD 屬性名稱
            data_tag: NDH 數據標籤
            mapping: 數據映射 (可選)
        """
        self.bindings[data_tag] = {
            'usd_prim_path': usd_prim_path,
            'attribute_name': attribute_name,
            'mapping': mapping
        }
    
    async def connect_to_ndh(self, ndh_url):
        """連接到 NDH WebSocket"""
        self.ws = await websocket.connect(ndh_url)
        print(f"Connected to NDH: {ndh_url}")
    
    async def update_loop(self):
        """實時數據更新循環"""
        while True:
            try:
                # 從 NDH 接收數據
                message = await self.ws.recv()
                data = json.loads(message)
                
                # 更新 USD 屬性
                for tag, value in data.items():
                    if tag in self.bindings:
                        binding = self.bindings[tag]
                        prim = self.stage.GetPrimAtPath(binding['usd_prim_path'])
                        
                        if prim:
                            # 應用映射 (如果有)
                            if binding['mapping'] and value in binding['mapping']:
                                value = binding['mapping'][value]
                            
                            # 更新屬性
                            attr = prim.GetAttribute(binding['attribute_name'])
                            if attr:
                                attr.Set(value)
            
            except Exception as e:
                print(f"Error in update loop: {e}")
                await asyncio.sleep(1)

# 使用範例
binding = RealtimeDataBinding()

# 綁定設備狀態到顏色
binding.add_binding(
    usd_prim_path='/World/Harvatek/Floor_2/Wire_Bonding_Area_A/WB_2F_001',
    attribute_name='primvars:displayColor',
    data_tag='WB_2F_001.Status',
    mapping={
        'IDLE': Gf.Vec3f(0.5, 0.5, 0.5),      # 灰色
        'RUNNING': Gf.Vec3f(0.0, 1.0, 0.0),   # 綠色
        'ALARM': Gf.Vec3f(1.0, 0.0, 0.0),     # 紅色
        'MAINTENANCE': Gf.Vec3f(1.0, 1.0, 0.0)  # 黃色
    }
)

# 綁定 UPH 到文字顯示
binding.add_binding(
    usd_prim_path='/World/Harvatek/Floor_2/Wire_Bonding_Area_A/WB_2F_001/Display/UPH_Text',
    attribute_name='text',
    data_tag='WB_2F_001.UPH'
)

# 連接並啟動更新循環
asyncio.run(binding.connect_to_ndh('ws://ndh-server:8080/realtime'))
asyncio.run(binding.update_loop())
```

---

## 6. IADL Editor V3.1 應用

### 6.1 資產建模工作流程

```
1. 匯入 CAD 模型
   ├─ 從 STEP / IGES 匯入設備 3D 模型
   ├─ 從 Revit / IFC 匯入建築模型
   └─ 自動轉換為 USD 格式

2. 資產定義
   ├─ 填寫資產基本資訊 (製造商、型號、序號)
   ├─ 定義資產屬性 (規格、參數)
   ├─ 設定位置座標
   └─ 連接 3D 模型

3. 連接點定義
   ├─ 定義管線連接點 (位置、方向、規格)
   ├─ 定義電力連接點
   ├─ 定義氣體連接點
   └─ 連接到 USD Prim

4. 數據標籤對應
   ├─ 配置 OPC UA / SECS/GEM 連接
   ├─ 映射數據標籤到資產屬性
   ├─ 設定更新頻率
   └─ 測試數據連接

5. Omniverse 整合
   ├─ 設定場景路徑
   ├─ 配置數據綁定
   ├─ 定義動畫
   └─ 預覽效果

6. 匯出與部署
   ├─ 匯出 IADL YAML 檔案
   ├─ 部署到 NDH
   ├─ 同步到 Omniverse
   └─ 驗證功能
```

### 6.2 MEP 設計工作流程

```
1. 新產線規劃
   ├─ 匯入設備 3D 模型
   ├─ 佈局設備位置
   ├─ 自動衝突檢測
   └─ 優化佈局

2. 管線路由設計
   ├─ 選擇起點和終點連接點
   ├─ 自動路由演算法
   ├─ 手動調整路徑
   ├─ 管件自動插入 (彎頭、三通、閥門)
   └─ 管線標註

3. 風管路由設計
   ├─ 定義風管起點 (AHU 出口)
   ├─ 定義終點 (FFU 入口)
   ├─ 自動路由
   ├─ 風管尺寸計算
   └─ 風阻分析

4. 電纜路由設計
   ├─ 定義電源點和負載點
   ├─ 選擇電纜規格
   ├─ 路由設計 (電纜橋架)
   └─ 電壓降計算

5. 衝突檢測
   ├─ Hard Clash 檢測 (幾何重疊)
   ├─ Soft Clash 檢測 (間隙不足)
   ├─ Clearance 檢測 (維護空間)
   └─ 生成衝突報表

6. ISO 圖面生成
   ├─ 選擇管線系統
   ├─ 自動生成 ISO 圖
   ├─ 標註尺寸和材料
   ├─ 生成 BOM
   └─ 匯出 DWG / PDF

7. Flow Analysis
   ├─ 定義流體屬性
   ├─ 設定邊界條件
   ├─ 運行 CFD 模擬
   ├─ 視覺化結果 (壓力、流速)
   └─ 優化設計
```

---

## 7. 實施路線圖

### 7.1 整體時程 (12 個月)

```
階段 1: 基礎建設 (月 1-3)
├─ NDH V3.1 部署
├─ Omniverse 環境建置
├─ IADL Editor V3.1 安裝
└─ 網路和伺服器準備

階段 2: 資產建模 (月 3-5)
├─ 建築 3D 模型建立
├─ 生產設備 IADL 定義 (500 台)
├─ 設施設備 IADL 定義 (160 台)
└─ MEP 系統建模

階段 3: OT 整合 (月 4-6)
├─ SECS/GEM Connector 開發
├─ OPC UA Connector 配置
├─ BACnet Connector 配置
└─ 數據流測試

階段 4: IT 整合 (月 5-7)
├─ ERP 整合 (SAP API)
├─ MES 整合
├─ PLM 整合
└─ 數據同步測試

階段 5: Omniverse 應用開發 (月 6-9)
├─ 工廠漫遊應用
├─ 設備監控應用
├─ 能源管理應用
├─ 維護管理應用
└─ 培訓模擬應用

階段 6: 試點運行 (月 8-10)
├─ 選擇試點區域 (2 樓 A 區)
├─ 完整功能測試
├─ 使用者培訓
└─ 問題修正

階段 7: 全廠推廣 (月 10-12)
├─ 擴展到所有樓層
├─ 全員培訓
├─ 性能優化
└─ 文件交付
```

### 7.2 詳細實施計畫

#### 階段 1: 基礎建設 (月 1-3)

**目標**: 建立 IDTF V3.1 基礎架構

**任務**:

1. **硬體準備** (週 1-2)
   - 採購伺服器 (NDH, Omniverse, IADL Editor)
   - 網路升級 (10 Gbps 骨幹)
   - 儲存系統 (NAS, SAN)

2. **NDH V3.1 部署** (週 3-5)
   - 安裝 Kafka 叢集 (3 節點)
   - 安裝 InfluxDB 叢集 (3 節點)
   - 安裝 PostgreSQL (HA 配置)
   - 配置 Kafka Connect
   - 安裝 NDH Monitor & Configurator

3. **Omniverse 環境建置** (週 4-6)
   - 安裝 Omniverse Nucleus Server
   - 安裝 Omniverse Create / View
   - 配置 Omniverse Farm (渲染農場)
   - 建立專案目錄結構

4. **IADL Editor V3.1 安裝** (週 6-8)
   - 安裝 IADL Editor Server
   - 安裝 IADL Editor Client
   - 配置插件系統
   - 整合 NDH 和 Omniverse

5. **網路和安全** (週 8-10)
   - VLAN 劃分 (OT, IT, DMZ)
   - 防火牆規則配置
   - VPN 設定 (遠端存取)
   - 使用者權限管理 (RBAC)

**交付物**:
- 基礎架構部署文件
- 網路拓撲圖
- 系統配置文件
- 使用者手冊

**里程碑**:
- M1.1: 硬體到位 (週 2)
- M1.2: NDH 上線 (週 5)
- M1.3: Omniverse 上線 (週 6)
- M1.4: IADL Editor 上線 (週 8)
- M1.5: 系統整合測試完成 (週 10)

#### 階段 2: 資產建模 (月 3-5)

**目標**: 建立完整的廠房數位分身模型

**任務**:

1. **建築 3D 模型** (週 11-13)
   - 收集建築圖面 (CAD, Revit)
   - 轉換為 USD 格式
   - 建立樓層、房間結構
   - 添加材質和光照

2. **生產設備 IADL 定義** (週 12-16)
   - 收集設備資料 (500 台)
   - 建立設備 3D 模型 (或從製造商獲取)
   - 使用 IADL Editor 定義資產
   - 連接 USD 模型
   - 定義數據標籤對應

3. **設施設備 IADL 定義** (週 14-18)
   - 收集設施設備資料 (160 台)
   - 建立設備 3D 模型
   - 定義資產和連接點
   - 配置 BACnet / Modbus 連接

4. **MEP 系統建模** (週 16-20)
   - 收集 MEP 圖面
   - 使用 IADL Editor MEP 模組建模
   - 管線系統 (冰水、純水、N2, CDA)
   - 風管系統
   - 電纜系統
   - 衝突檢測和修正

**交付物**:
- 完整的 3D 廠房模型 (USD)
- 500 個生產設備 IADL 定義
- 160 個設施設備 IADL 定義
- MEP 系統模型和 ISO 圖面
- 資產清單和數據字典

**里程碑**:
- M2.1: 建築模型完成 (週 13)
- M2.2: 50% 生產設備建模完成 (週 15)
- M2.3: 100% 生產設備建模完成 (週 16)
- M2.4: 設施設備建模完成 (週 18)
- M2.5: MEP 系統建模完成 (週 20)

#### 階段 3: OT 整合 (月 4-6)

**目標**: 整合所有 OT 系統,實現實時數據流

**任務**:

1. **SECS/GEM Connector 開發** (週 17-20)
   - 分析設備 SECS/GEM 介面
   - 開發 Kafka Connect SECS/GEM Source Connector
   - 配置 500 台生產設備連接
   - 數據流測試

2. **OPC UA Connector 配置** (週 19-22)
   - 配置 OPC UA Server 連接
   - 訂閱數據標籤 (環境、能源)
   - 數據流測試

3. **BACnet Connector 配置** (週 20-23)
   - 配置 BACnet/IP 連接
   - 讀取 HVAC 系統數據
   - 數據流測試

4. **數據驗證** (週 22-24)
   - 驗證數據準確性
   - 驗證數據完整性
   - 驗證數據時效性
   - 性能測試 (吞吐量、延遲)

**交付物**:
- SECS/GEM Connector 程式碼和文件
- OPC UA Connector 配置文件
- BACnet Connector 配置文件
- 數據流測試報告
- 性能測試報告

**里程碑**:
- M3.1: SECS/GEM Connector 完成 (週 20)
- M3.2: OPC UA Connector 完成 (週 22)
- M3.3: BACnet Connector 完成 (週 23)
- M3.4: 數據驗證完成 (週 24)

#### 階段 4: IT 整合 (月 5-7)

**目標**: 整合所有 IT 系統,實現數據互通

**任務**:

1. **ERP 整合** (週 21-25)
   - 分析 SAP API
   - 開發 Kafka Connect SAP Source Connector
   - 同步資產主數據
   - 同步維護工單
   - 同步物料數據

2. **MES 整合** (週 23-26)
   - 分析 MES API
   - 開發 Kafka Connect MES Source/Sink Connector
   - 同步工單數據
   - 同步生產數據
   - 雙向數據流

3. **PLM 整合** (週 24-27)
   - 分析 PLM API
   - 同步產品定義
   - 同步 BOM
   - 同步工程變更

4. **數據同步測試** (週 26-28)
   - 端到端數據流測試
   - 數據一致性驗證
   - 性能測試

**交付物**:
- ERP Connector 程式碼和文件
- MES Connector 程式碼和文件
- PLM Connector 程式碼和文件
- 數據同步測試報告
- 整合架構文件

**里程碑**:
- M4.1: ERP 整合完成 (週 25)
- M4.2: MES 整合完成 (週 26)
- M4.3: PLM 整合完成 (週 27)
- M4.4: 數據同步測試完成 (週 28)

#### 階段 5: Omniverse 應用開發 (月 6-9)

**目標**: 開發各種 Omniverse 應用場景

**任務**:

1. **工廠漫遊應用** (週 25-28)
   - 設計相機路徑
   - 添加導覽標記
   - 語音導覽 (可選)
   - 互動功能

2. **設備監控應用** (週 27-30)
   - 實時數據綁定
   - 設備狀態視覺化
   - 動畫驅動
   - KPI 儀表板

3. **能源管理應用** (週 29-32)
   - 能源流動視覺化
   - 能源消耗熱圖
   - 能源成本分析
   - 優化建議

4. **維護管理應用** (週 31-34)
   - 維護任務 3D 定位
   - AR 維護指引
   - 備品備件管理
   - 維護歷史查詢

5. **培訓模擬應用** (週 33-36)
   - VR 設備操作培訓
   - 緊急應變模擬
   - 新產品試產模擬
   - 評估和認證

**交付物**:
- 工廠漫遊應用
- 設備監控應用
- 能源管理應用
- 維護管理應用
- 培訓模擬應用
- 使用者手冊

**里程碑**:
- M5.1: 工廠漫遊應用完成 (週 28)
- M5.2: 設備監控應用完成 (週 30)
- M5.3: 能源管理應用完成 (週 32)
- M5.4: 維護管理應用完成 (週 34)
- M5.5: 培訓模擬應用完成 (週 36)

#### 階段 6: 試點運行 (月 8-10)

**目標**: 在試點區域驗證系統功能

**任務**:

1. **試點區域選擇** (週 33)
   - 選擇 2 樓 A 區 (Wire Bonding Area)
   - 包含 30 台設備
   - 包含 HVAC, 電力, N2, CDA 系統

2. **完整功能測試** (週 34-37)
   - 實時數據流測試
   - Omniverse 應用測試
   - 告警功能測試
   - 維護管理測試
   - 能源管理測試

3. **使用者培訓** (週 36-38)
   - 系統管理員培訓
   - 設備工程師培訓
   - 維護人員培訓
   - 管理層簡報

4. **問題修正** (週 37-40)
   - 收集使用者反饋
   - 修正 Bug
   - 優化性能
   - 改進 UI/UX

**交付物**:
- 試點運行報告
- 使用者反饋報告
- Bug 修正記錄
- 培訓教材
- 操作手冊

**里程碑**:
- M6.1: 試點區域上線 (週 34)
- M6.2: 功能測試完成 (週 37)
- M6.3: 使用者培訓完成 (週 38)
- M6.4: 問題修正完成 (週 40)

#### 階段 7: 全廠推廣 (月 10-12)

**目標**: 將系統擴展到全廠

**任務**:

1. **擴展到所有樓層** (週 41-46)
   - 2 樓 B, C 區
   - 3 樓全區
   - 4 樓全區
   - 1 樓和 5 樓
   - 地下 1 樓

2. **全員培訓** (週 43-48)
   - 分批培訓所有使用者
   - 製作培訓影片
   - 建立知識庫

3. **性能優化** (週 45-50)
   - 數據庫查詢優化
   - Kafka 性能調優
   - Omniverse 渲染優化
   - 網路頻寬優化

4. **文件交付** (週 48-52)
   - 系統架構文件
   - 操作手冊
   - 維護手冊
   - API 文件
   - 培訓教材

**交付物**:
- 全廠 Digital Twin 系統
- 完整文件集
- 培訓教材和影片
- 維護合約
- 專案結案報告

**里程碑**:
- M7.1: 50% 樓層上線 (週 44)
- M7.2: 100% 樓層上線 (週 46)
- M7.3: 全員培訓完成 (週 48)
- M7.4: 性能優化完成 (週 50)
- M7.5: 專案結案 (週 52)

### 7.3 資源需求

#### 7.3.1 團隊組成

| 角色 | 人數 | 職責 |
|-----|-----|-----|
| 專案經理 | 1 | 整體專案管理 |
| 系統架構師 | 1 | 技術架構設計 |
| IDTF 工程師 | 2 | IDTF 部署和配置 |
| 3D 建模工程師 | 2 | 3D 模型建立和 USD 轉換 |
| OT 整合工程師 | 2 | SECS/GEM, OPC UA, BACnet 整合 |
| IT 整合工程師 | 2 | ERP, MES, PLM 整合 |
| Omniverse 開發工程師 | 2 | Omniverse 應用開發 |
| 數據工程師 | 1 | 數據管道和 ETL |
| 測試工程師 | 1 | 系統測試和驗證 |
| 培訓專員 | 1 | 使用者培訓和文件 |
| **總計** | **15** | |

#### 7.3.2 硬體需求

| 項目 | 規格 | 數量 | 用途 |
|-----|-----|-----|-----|
| NDH Server | 2x Xeon, 128GB RAM, 2TB SSD | 3 | Kafka, InfluxDB, PostgreSQL |
| Omniverse Nucleus Server | 2x Xeon, 64GB RAM, 10TB SSD | 1 | USD 檔案伺服器 |
| Omniverse Workstation | RTX 4090, 64GB RAM, 2TB SSD | 5 | IADL Editor, Omniverse Create |
| Rendering Farm | RTX 4080, 32GB RAM, 1TB SSD | 10 | Omniverse Farm |
| Network Switch | 10 Gbps, 48 Port | 2 | 網路骨幹 |
| Storage (NAS) | 100TB, RAID 6 | 1 | 檔案儲存 |
| **總預算** | | | **約 USD 300,000** |

#### 7.3.3 軟體授權

| 軟體 | 授權類型 | 數量 | 年費 (USD) |
|-----|---------|-----|-----------|
| IDTF V3.1 | 開源 (Apache 2.0) | - | 0 |
| Kafka | 開源 (Apache 2.0) | - | 0 |
| InfluxDB | 開源 (MIT) | - | 0 |
| PostgreSQL | 開源 (PostgreSQL) | - | 0 |
| Omniverse Enterprise | 企業授權 | 15 用戶 | 45,000 |
| NVIDIA RTX Server | 伺服器授權 | 1 | 10,000 |
| **總計** | | | **55,000** |

---

## 8. 預期效益分析

### 8.1 量化效益

#### 8.1.1 生產效率提升

**設備綜合效率 (OEE) 提升**:
- **現況**: 平均 OEE 70%
- **目標**: 平均 OEE 85%
- **提升**: 15 百分點

**計算**:
```
假設:
- 生產設備: 500 台
- 平均設備產能: 5,000 units/hour
- 每日運行時間: 20 小時
- 產品平均售價: USD 0.05 / unit

現況產量:
500 台 × 5,000 units/hr × 20 hr/day × 70% OEE = 35,000,000 units/day

目標產量:
500 台 × 5,000 units/hr × 20 hr/day × 85% OEE = 42,500,000 units/day

增加產量:
42,500,000 - 35,000,000 = 7,500,000 units/day

年增加營收:
7,500,000 units/day × 300 days/year × USD 0.05 = USD 112,500,000 / year
```

**年增加營收**: **USD 112.5M**

#### 8.1.2 能源成本降低

**能源消耗降低**:
- **現況**: 年能源成本 USD 10M
- **目標**: 降低 15%
- **節省**: USD 1.5M / year

**實現方式**:
- 實時能源監控和優化
- 設備閒置時自動降低功耗
- HVAC 系統智能控制
- 照明系統優化

#### 8.1.3 維護成本降低

**預測性維護**:
- **現況**: 被動式維護,年維護成本 USD 5M
- **目標**: 預測性維護,降低 25%
- **節省**: USD 1.25M / year

**實現方式**:
- 設備狀態實時監控
- AI 預測模型
- 提前備料
- 減少非計劃停機

#### 8.1.4 非計劃停機時間減少

**停機時間減少**:
- **現況**: 年非計劃停機 500 小時
- **目標**: 減少 35%,降至 325 小時
- **減少**: 175 小時

**計算**:
```
假設:
- 平均產能: 2,500,000 units/hour (全廠)
- 產品平均售價: USD 0.05 / unit

減少停機損失:
175 hours × 2,500,000 units/hr × USD 0.05 = USD 21,875,000 / year
```

**年減少損失**: **USD 21.9M**

#### 8.1.5 新產線建置時間縮短

**建置時間縮短**:
- **現況**: 新產線建置 12 個月
- **目標**: 縮短 30%,降至 8.4 個月
- **縮短**: 3.6 個月

**效益**:
- 更快進入市場
- 提前產生營收
- 降低建置成本 (人力、租金)

**假設新產線年營收 USD 50M**:
```
提前 3.6 個月營收:
USD 50M × (3.6 / 12) = USD 15M
```

**提前營收**: **USD 15M**

#### 8.1.6 總量化效益

| 效益項目 | 年效益 (USD) |
|---------|-------------|
| OEE 提升增加營收 | 112,500,000 |
| 能源成本降低 | 1,500,000 |
| 維護成本降低 | 1,250,000 |
| 減少非計劃停機損失 | 21,875,000 |
| 新產線提前營收 (一次性) | 15,000,000 |
| **年度總效益** | **137,125,000** |
| **3 年總效益** | **426,375,000** |

### 8.2 質化效益

#### 8.2.1 決策支援

- **實時可見性**: 管理層可實時掌握全廠狀況
- **數據驅動決策**: 基於實際數據而非經驗
- **模擬分析**: 在虛擬環境中測試不同策略
- **快速響應**: 快速識別和解決問題

#### 8.2.2 協同效率

- **跨部門協同**: 打破部門孤島,促進協作
- **遠端協作**: 支援遠端辦公和多地協同
- **知識共享**: 建立企業知識庫
- **培訓效率**: VR/AR 培訓提升學習效率

#### 8.2.3 創新能力

- **數位化轉型**: 建立數位化基礎
- **AI/ML 應用**: 為 AI/ML 提供數據基礎
- **新技術驗證**: 在虛擬環境中驗證新技術
- **持續改進**: 建立持續改進文化

#### 8.2.4 競爭優勢

- **技術領先**: 在 LED 產業中技術領先
- **品牌形象**: 提升科技創新形象
- **客戶信任**: 展示先進製造能力
- **人才吸引**: 吸引優秀人才加入

### 8.3 投資回報分析 (ROI)

#### 8.3.1 總投資成本

| 成本項目 | 金額 (USD) |
|---------|-----------|
| 硬體設備 | 300,000 |
| 軟體授權 (3 年) | 165,000 |
| 實施服務 (15 人 × 12 月) | 1,800,000 |
| 培訓 | 50,000 |
| 其他 (差旅、雜項) | 85,000 |
| **總投資** | **2,400,000** |

#### 8.3.2 ROI 計算

```
年度淨效益:
USD 137,125,000 (效益) - USD 55,000 (年度軟體授權) = USD 137,070,000

ROI (第一年):
(USD 137,070,000 - USD 2,400,000) / USD 2,400,000 × 100% = 5,611%

投資回收期:
USD 2,400,000 / USD 137,070,000 × 12 月 = 0.21 月 ≈ 6.3 天
```

**ROI**: **5,611%**  
**投資回收期**: **6.3 天**

> **注意**: 此 ROI 計算假設所有效益都能實現。實際效益可能因執行情況而異。保守估計,即使只實現 20% 的預期效益,ROI 仍高達 1,122%,投資回收期約 32 天。

---

## 9. 風險管理

### 9.1 技術風險

| 風險 | 可能性 | 影響 | 緩解措施 |
|-----|-------|-----|---------|
| SECS/GEM 整合困難 | 中 | 高 | 提前進行設備介面調查,必要時尋求設備商支援 |
| 3D 模型品質不足 | 中 | 中 | 建立模型品質標準,必要時重新建模 |
| 網路頻寬不足 | 低 | 高 | 升級網路基礎設施到 10 Gbps |
| 系統性能不足 | 中 | 中 | 進行性能測試,必要時擴充硬體 |
| 數據安全問題 | 低 | 高 | 實施多層安全措施,定期安全稽核 |

### 9.2 組織風險

| 風險 | 可能性 | 影響 | 緩解措施 |
|-----|-------|-----|---------|
| 使用者抗拒變革 | 中 | 高 | 充分溝通,展示效益,提供培訓 |
| 關鍵人員流失 | 低 | 高 | 建立知識庫,交叉培訓 |
| 跨部門協調困難 | 中 | 中 | 建立專案管理辦公室 (PMO) |
| 預算超支 | 中 | 中 | 嚴格預算控制,定期審查 |

### 9.3 專案風險

| 風險 | 可能性 | 影響 | 緩解措施 |
|-----|-------|-----|---------|
| 進度延遲 | 中 | 中 | 建立緩衝時間,關鍵路徑管理 |
| 需求變更 | 高 | 中 | 建立變更管理流程 |
| 供應商延遲交付 | 低 | 中 | 選擇可靠供應商,建立備選方案 |
| 試點失敗 | 低 | 高 | 充分準備,小範圍試點,快速迭代 |

---

## 10. 成功關鍵因素

### 10.1 高層支持

- 獲得 CEO 和高層管理團隊的全力支持
- 將 Digital Twin 列為公司戰略重點
- 提供充足的預算和資源

### 10.2 跨部門協作

- 建立跨部門專案團隊
- 定期溝通和協調
- 打破部門孤島

### 10.3 技術能力

- 選擇合適的技術夥伴 (NVIDIA, IDTF 社群)
- 建立內部技術團隊
- 持續學習和技能提升

### 10.4 變革管理

- 充分溝通專案目標和效益
- 提供全面的培訓
- 建立激勵機制

### 10.5 數據品質

- 確保數據準確性和完整性
- 建立數據治理機制
- 定期數據清理和驗證

### 10.6 持續改進

- 建立反饋機制
- 定期評估和優化
- 擁抱創新和變革

---

## 11. 結論

基於 **IDTF V3.1** 架構的宏齊科技廠房 Digital Twins 系統,將為宏齊科技帶來全面的數位化轉型,實現從設計、建造到運維的全生命週期管理。透過整合 IADL V3.1、NDH V3.1、IADL Editor V3.1 和 Omniverse,宏齊科技將建立一個開放、中立、低成本的數位分身平台,大幅提升生產效率、降低運營成本、增強創新能力。

**核心價值**:
1. **開源中立**: 避免供應商鎖定,降低長期成本
2. **全生命週期**: 從設計到運維的完整覆蓋
3. **實時可見**: 全廠實時數據視覺化
4. **預測性維護**: 減少非計劃停機
5. **智能優化**: AI/ML 驅動的持續改進

**預期效益**:
- **年度效益**: USD 137M
- **ROI**: 5,611%
- **投資回收期**: 6.3 天

宏齊科技 Digital Twins 系統將成為 LED 產業數位化轉型的標竿案例,為宏齊科技在 Mini LED 和 Micro LED 市場的競爭中提供強大的技術支撐。

---

## 附錄

### 附錄 A: 術語表

| 術語 | 全稱 | 說明 |
|-----|-----|-----|
| IDTF | Industrial Digital Twin Framework | 工業數位分身框架 |
| IADL | Industrial Asset Description Language | 工業資產描述語言 |
| NDH | Neutral Data Hub | 中立數據中樞 |
| USD | Universal Scene Description | 通用場景描述 (Pixar 開發) |
| OEE | Overall Equipment Effectiveness | 設備綜合效率 |
| MEP | Mechanical, Electrical, Plumbing | 機械、電氣、管道 |
| HVAC | Heating, Ventilation, Air Conditioning | 暖通空調 |
| SECS/GEM | SEMI Equipment Communications Standard / Generic Equipment Model | 半導體設備通訊標準 |
| OPC UA | OPC Unified Architecture | OPC 統一架構 |
| BACnet | Building Automation and Control Networks | 樓宇自動化控制網路 |
| CFD | Computational Fluid Dynamics | 計算流體力學 |
| VR | Virtual Reality | 虛擬實境 |
| AR | Augmented Reality | 擴增實境 |

### 附錄 B: 參考文獻

1. IDTF V3.1 技術白皮書 (2025)
2. IADL V3.1 語法規範 (2025)
3. NDH V3.1 設計文件 (2025)
4. Omniverse 技術文件 (NVIDIA, 2025)
5. 宏齊科技年報 (2024)
6. LED 產業報告 (2025)

### 附錄 C: 聯絡資訊

**專案聯絡人**:
- 姓名: [待填]
- 職稱: [待填]
- Email: [待填]
- 電話: [待填]

**技術支援**:
- IDTF 社群: https://github.com/chchlin1018/idtf-v3
- Email: [待填]

---

**文件結束**

