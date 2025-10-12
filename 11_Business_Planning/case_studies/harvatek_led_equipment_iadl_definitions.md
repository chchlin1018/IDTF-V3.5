# Harvatek LED 封裝設備 IADL V3.1 定義

**版本**: 1.0  
**日期**: 2025-10-12  
**作者**: IDTF V3.1 Team  

---

## 目錄

1. [Die Sorter (晶片分選機)](#1-die-sorter-晶片分選機)
2. [Die Bonder (固晶機)](#2-die-bonder-固晶機)
3. [Wire Bonder (打線機)](#3-wire-bonder-打線機)
4. [Dispenser (點膠機/封膠機)](#4-dispenser-點膠機封膠機)
5. [Tester (測試機)](#5-tester-測試機)
6. [Taping Machine (編帶包裝機)](#6-taping-machine-編帶包裝機)
7. [AOI System (自動光學檢測)](#7-aoi-system-自動光學檢測)
8. [X-Ray Inspection (X光檢測)](#8-x-ray-inspection-x光檢測)

---

## 1. Die Sorter (晶片分選機)

### 1.1 設備概述

**功能**: 從晶圓上分選、測試和分類 LED 晶片,根據光電特性進行分級。

**關鍵參數**:
- 分選速度: 8,000-12,000 UPH
- 測試項目: 波長、亮度、電壓、漏電流
- 分選精度: ±1 nm (波長), ±5% (亮度)

### 1.2 IADL 定義

```yaml
# ========================================
# IADL Asset Definition - Die Sorter
# ========================================

asset_id: DS-2F-001
asset_type: DieSorter
manufacturer: ASM Pacific Technology
model: AD830Plus
serial_number: ASM-2021-DS-001
description: LED 晶片分選機,用於晶圓分選和測試

# ========================================
# 基本屬性
# ========================================
properties:
  installation_date: 2021-05-15
  warranty_expiry_date: 2026-05-15
  purchase_cost_usd: 850000
  
  location:
    building: Harvatek-Hsinchu
    floor: 2
    room: Die-Sorting-Area
    coordinates:
      x: 10.5
      y: 8.3
      z: 0.0
  
  specifications:
    sorting_speed_uph: 12000
    wafer_size_inch: 4
    pick_accuracy_um: 5
    test_items:
      - wavelength
      - luminous_intensity
      - forward_voltage
      - reverse_current
    wavelength_range_nm: [400, 700]
    wavelength_accuracy_nm: 1
    power_consumption_kw: 8.5
    compressed_air_pressure_bar: 6.0
    compressed_air_flow_lpm: 80
    vacuum_pressure_kpa: -80
    dimensions_mm:
      length: 1800
      width: 1500
      height: 2000
    weight_kg: 1200

# ========================================
# 3D 模型定義 (OpenUSD)
# ========================================
geometry:
  usd_reference: omniverse://harvatek/assets/equipment/die_sorter_asm_ad830.usd
  alternative_formats:
    - type: glTF
      path: /models/ds_2f_001.gltf
    - type: STEP
      path: /models/ds_2f_001.step
  
  transform:
    translation: [10.5, 8.3, 0.0]
    rotation: [0.0, 0.0, 90.0]
    scale: [1.0, 1.0, 1.0]
  
  lod_levels:
    - level: high
      usd_reference: omniverse://harvatek/assets/equipment/die_sorter_asm_ad830_high.usd
      max_distance: 5.0
      triangle_count: 150000
    - level: medium
      usd_reference: omniverse://harvatek/assets/equipment/die_sorter_asm_ad830_med.usd
      max_distance: 20.0
      triangle_count: 50000
    - level: low
      usd_reference: omniverse://harvatek/assets/equipment/die_sorter_asm_ad830_low.usd
      max_distance: 100.0
      triangle_count: 5000
  
  collision:
    type: box
    dimensions: [1.8, 1.5, 2.0]

# ========================================
# 材質定義
# ========================================
materials:
  body:
    usd_material: omniverse://harvatek/materials/equipment_white.mdl
    color: [0.95, 0.95, 0.95, 1.0]
    roughness: 0.3
    metallic: 0.1
  
  window:
    usd_material: omniverse://harvatek/materials/glass_transparent.mdl
    color: [0.8, 0.8, 1.0, 0.3]
    roughness: 0.05
    metallic: 0.0
  
  display:
    usd_material: omniverse://harvatek/materials/lcd_screen.mdl
    emissive_color: [0.2, 0.4, 0.8, 1.0]
    emissive_intensity: 2.0

# ========================================
# 連接點定義
# ========================================
connection_points:
  - id: power_inlet
    type: electrical
    voltage: 220V
    current: 40A
    phases: 3
    position: [-0.9, -0.75, 0.3]
    usd_prim_path: /DS_2F_001/Connections/Power_Inlet
  
  - id: cda_inlet
    type: gas
    gas_type: compressed_air
    pressure_bar: 6.0
    flow_rate_lpm: 80
    position: [-0.9, 0.75, 0.3]
    usd_prim_path: /DS_2F_001/Connections/CDA_Inlet
  
  - id: vacuum_inlet
    type: vacuum
    pressure_kpa: -80
    flow_rate_lpm: 100
    position: [-0.9, 0.75, 0.5]
    usd_prim_path: /DS_2F_001/Connections/Vacuum_Inlet
  
  - id: network_port
    type: network
    protocol: ethernet
    speed_gbps: 1.0
    position: [-0.9, -0.75, 0.5]
    usd_prim_path: /DS_2F_001/Connections/Network

# ========================================
# 與其他資產的連接
# ========================================
connections:
  - type: powered_by
    target_asset_id: PANEL-2F-A-05
    connection_point: power_inlet
  
  - type: supplied_by
    target_asset_id: CDA-SYSTEM-MAIN
    connection_point: cda_inlet
  
  - type: supplied_by
    target_asset_id: VACUUM-SYSTEM-2F
    connection_point: vacuum_inlet
  
  - type: controlled_by
    target_asset_id: PLC-2F-001
  
  - type: monitored_by
    target_asset_id: SCADA-SYSTEM
  
  - type: feeds_to
    target_asset_id: DB-2F-001
    description: 分選後的晶片送至固晶機

# ========================================
# OT 數據標籤對應
# ========================================
data_tags:
  # 設備狀態
  - tag_id: DS_2F_001.Status
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_1
    data_type: string
    enum_values: [IDLE, RUNNING, ALARM, MAINTENANCE, SETUP]
    update_rate_ms: 1000
    description: 設備運行狀態
  
  # 生產數據
  - tag_id: DS_2F_001.UPH
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_10
    data_type: integer
    unit: units/hour
    range: [0, 15000]
    update_rate_ms: 5000
    description: 實際分選速度
  
  - tag_id: DS_2F_001.TotalCount
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_11
    data_type: integer
    unit: units
    update_rate_ms: 1000
    description: 累計分選數量
  
  - tag_id: DS_2F_001.GoodCount
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_12
    data_type: integer
    unit: units
    update_rate_ms: 1000
    description: 良品數量
  
  - tag_id: DS_2F_001.RejectCount
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_13
    data_type: integer
    unit: units
    update_rate_ms: 1000
    description: 不良品數量
  
  - tag_id: DS_2F_001.YieldRate
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_14
    data_type: float
    unit: percent
    range: [0, 100]
    update_rate_ms: 5000
    description: 良率
  
  # 測試數據
  - tag_id: DS_2F_001.AvgWavelength
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_20
    data_type: float
    unit: nm
    range: [400, 700]
    update_rate_ms: 10000
    description: 平均波長
  
  - tag_id: DS_2F_001.AvgLuminousIntensity
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_21
    data_type: float
    unit: mcd
    range: [0, 10000]
    update_rate_ms: 10000
    description: 平均亮度
  
  - tag_id: DS_2F_001.AvgForwardVoltage
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_22
    data_type: float
    unit: V
    range: [0, 5]
    update_rate_ms: 10000
    description: 平均順向電壓
  
  # 設備參數
  - tag_id: DS_2F_001.VacuumPressure
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_30
    data_type: float
    unit: kPa
    range: [-100, 0]
    update_rate_ms: 5000
    description: 真空壓力
  
  - tag_id: DS_2F_001.Temperature
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    variable_id: SVID_31
    data_type: float
    unit: celsius
    range: [20, 30]
    update_rate_ms: 10000
    description: 設備溫度
  
  # 能源數據
  - tag_id: DS_2F_001.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.DS_2F_001.Power
    data_type: float
    unit: kW
    range: [0, 15]
    update_rate_ms: 5000
    description: 實時功耗
  
  - tag_id: DS_2F_001.EnergyConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.DS_2F_001.Energy
    data_type: float
    unit: kWh
    update_rate_ms: 60000
    description: 累計電量
  
  # 告警
  - tag_id: DS_2F_001.Alarm
    protocol: SECS_GEM
    equipment_id: DS-2F-001
    event_id: ALID_ALL
    data_type: string
    update_rate_ms: 500
    description: 告警訊息

# ========================================
# 行為模型
# ========================================
behavior_model:
  type: PythonScript
  path: /models/die_sorter_performance.py
  
  inputs:
    - uph
    - yield_rate
    - power_consumption
    - vacuum_pressure
    - temperature
  
  outputs:
    - oee
    - quality_rate
    - energy_efficiency
    - predicted_maintenance_date
    - anomaly_score
  
  update_frequency_hz: 0.1
  
  algorithms:
    - name: oee_calculation
      description: 計算設備綜合效率
      formula: "OEE = Availability × Performance × Quality"
    
    - name: predictive_maintenance
      description: 預測性維護模型
      method: LSTM
      features: [uph, vacuum_pressure, temperature, vibration]
      prediction_horizon_days: 30
    
    - name: anomaly_detection
      description: 異常檢測
      method: isolation_forest
      threshold: 0.8

# ========================================
# Omniverse 整合
# ========================================
omniverse:
  scene_path: /World/Harvatek/Floor_2/Die_Sorting_Area/DS_2F_001
  
  # 實時數據綁定
  data_bindings:
    # 狀態顏色
    - usd_attribute: /DS_2F_001/StatusLight/Color
      data_tag: DS_2F_001.Status
      mapping:
        IDLE: [0.5, 0.5, 0.5, 1.0]        # 灰色
        RUNNING: [0.0, 1.0, 0.0, 1.0]     # 綠色
        ALARM: [1.0, 0.0, 0.0, 1.0]       # 紅色
        MAINTENANCE: [1.0, 1.0, 0.0, 1.0] # 黃色
        SETUP: [0.0, 0.5, 1.0, 1.0]       # 藍色
    
    # 顯示面板
    - usd_attribute: /DS_2F_001/Display/UPH
      data_tag: DS_2F_001.UPH
      format: "{value} UPH"
      font_size: 24
    
    - usd_attribute: /DS_2F_001/Display/YieldRate
      data_tag: DS_2F_001.YieldRate
      format: "Yield: {value:.1f}%"
      font_size: 20
    
    - usd_attribute: /DS_2F_001/Display/OEE
      data_tag: DS_2F_001.OEE
      format: "OEE: {value:.1f}%"
      font_size: 20
    
    # 功耗顯示
    - usd_attribute: /DS_2F_001/Display/Power
      data_tag: DS_2F_001.PowerConsumption
      format: "{value:.1f} kW"
      font_size: 16
  
  # 動畫
  animations:
    - name: wafer_rotation
      usd_prim: /DS_2F_001/Wafer
      type: rotation
      axis: [0, 0, 1]
      speed_source: DS_2F_001.UPH
      speed_multiplier: 0.01
      enabled_when: DS_2F_001.Status == RUNNING
    
    - name: picker_motion
      usd_prim: /DS_2F_001/Picker
      type: translation
      axis: [0, 0, 1]
      amplitude: 0.1
      frequency_source: DS_2F_001.UPH
      frequency_multiplier: 0.001
      enabled_when: DS_2F_001.Status == RUNNING
  
  # 粒子效果
  particle_effects:
    - name: sorting_particles
      usd_prim: /DS_2F_001/Effects/Particles
      type: point_cloud
      count_source: DS_2F_001.UPH
      color: [1.0, 1.0, 0.0, 0.5]
      enabled_when: DS_2F_001.Status == RUNNING

# ========================================
# 維護資訊
# ========================================
maintenance:
  maintenance_strategy: predictive
  last_pm_date: 2024-10-01
  next_pm_date: 2025-01-01
  pm_interval_days: 90
  mtbf_hours: 5000
  mttr_hours: 3
  
  maintenance_procedures:
    - id: PM-DS-001
      name: 真空系統檢查
      frequency_days: 30
      estimated_duration_hours: 1.0
      required_skills: [vacuum_system]
      required_parts: []
    
    - id: PM-DS-002
      name: 光學系統校正
      frequency_days: 90
      estimated_duration_hours: 2.0
      required_skills: [optical_calibration]
      required_parts: [calibration_wafer]
    
    - id: PM-DS-003
      name: 機械臂保養
      frequency_days: 180
      estimated_duration_hours: 4.0
      required_skills: [mechanical_maintenance]
      required_parts: [lubricant, bearings]
    
    - id: PM-DS-004
      name: 全面保養
      frequency_days: 365
      estimated_duration_hours: 16.0
      required_skills: [mechanical_maintenance, electrical_maintenance, optical_calibration]
      required_parts: [filters, bearings, belts, lubricant]
  
  spare_parts:
    - part_id: SP-DS-001
      name: 真空吸嘴
      quantity_on_hand: 20
      reorder_point: 10
      lead_time_days: 14
      unit_cost_usd: 50
    
    - part_id: SP-DS-002
      name: 校正晶圓
      quantity_on_hand: 2
      reorder_point: 1
      lead_time_days: 30
      unit_cost_usd: 5000
    
    - part_id: SP-DS-003
      name: 軸承組
      quantity_on_hand: 4
      reorder_point: 2
      lead_time_days: 21
      unit_cost_usd: 800
  
  maintenance_history:
    - date: 2024-10-01
      type: preventive
      procedure_id: PM-DS-004
      duration_hours: 15.5
      cost_usd: 3500
      technician: John Doe
      notes: 全面保養,更換所有耗材

# ========================================
# 文件連結
# ========================================
documents:
  - type: manual
    title: 操作手冊
    path: /documents/ASM_AD830Plus_Operation_Manual.pdf
    language: en
  
  - type: manual
    title: 維護手冊
    path: /documents/ASM_AD830Plus_Maintenance_Manual.pdf
    language: en
  
  - type: drawing
    title: 設備圖面
    path: /documents/DS_2F_001_Drawing.pdf
    usd_reference: omniverse://harvatek/documents/DS_2F_001_Drawing.usd
  
  - type: sop
    title: 標準作業程序
    path: /documents/SOP_Die_Sorting.pdf
    language: zh-TW
  
  - type: training
    title: 培訓教材
    path: /documents/Training_Die_Sorter.pdf
    language: zh-TW

# ========================================
# MES 整合
# ========================================
mes_integration:
  equipment_id: DS-2F-001
  work_center: DS-AREA
  resource_type: DIE_SORTER
  capacity_uph: 12000
  
  recipes:
    - recipe_id: RCP-DS-BLUE-LED
      product_type: BLUE-LED-CHIP
      wavelength_target_nm: 465
      wavelength_tolerance_nm: 5
      luminous_intensity_min_mcd: 1000
      forward_voltage_max_v: 3.2
    
    - recipe_id: RCP-DS-GREEN-LED
      product_type: GREEN-LED-CHIP
      wavelength_target_nm: 525
      wavelength_tolerance_nm: 5
      luminous_intensity_min_mcd: 1500
      forward_voltage_max_v: 3.4
    
    - recipe_id: RCP-DS-RED-LED
      product_type: RED-LED-CHIP
      wavelength_target_nm: 625
      wavelength_tolerance_nm: 5
      luminous_intensity_min_mcd: 800
      forward_voltage_max_v: 2.2
  
  material_tracking:
    input_material: LED_WAFER
    output_material: SORTED_LED_CHIP
    lot_tracking: true
    serial_tracking: false

# ========================================
# ERP 整合
# ========================================
erp_integration:
  asset_number: FA-DS-2F-001
  cost_center: CC-2F-PRODUCTION
  depreciation_method: straight_line
  depreciation_period_years: 7
  residual_value_usd: 85000
  annual_depreciation_usd: 109286

# ========================================
# 品質管理
# ========================================
quality_management:
  spc_enabled: true
  spc_parameters:
    - parameter: wavelength
      control_chart_type: X-bar_R
      sample_size: 25
      sample_frequency_minutes: 30
      ucl: 470
      lcl: 460
      target: 465
    
    - parameter: luminous_intensity
      control_chart_type: X-bar_R
      sample_size: 25
      sample_frequency_minutes: 30
      ucl: 1200
      lcl: 800
      target: 1000
  
  calibration:
    calibration_required: true
    calibration_frequency_days: 90
    last_calibration_date: 2024-10-01
    next_calibration_date: 2024-12-30
    calibration_standard: NIST_traceable

# ========================================
# 安全資訊
# ========================================
safety:
  safety_rating: CE
  risk_assessment_date: 2021-05-01
  
  hazards:
    - type: electrical
      severity: high
      mitigation: Lockout/tagout procedures
    
    - type: mechanical
      severity: medium
      mitigation: Machine guarding, emergency stop
    
    - type: laser
      severity: medium
      mitigation: Laser safety glasses, warning signs
  
  emergency_procedures:
    - type: fire
      procedure: Use CO2 extinguisher, evacuate area
    
    - type: electrical_shock
      procedure: Cut power, call emergency services
    
    - type: mechanical_injury
      procedure: Emergency stop, call first aid

# ========================================
# 元數據
# ========================================
metadata:
  created_date: 2021-05-01
  created_by: engineer@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 2.1
  iadl_version: 3.1
  schema_version: 1.0
  
  tags:
    - production_equipment
    - die_sorting
    - critical_equipment
    - predictive_maintenance
    - led_manufacturing
  
  custom_fields:
    criticality_level: high
    backup_equipment: DS-2F-002
    vendor_support_contract: active
    vendor_support_expiry: 2026-05-15
```

---

## 2. Die Bonder (固晶機)

### 2.1 設備概述

**功能**: 將分選後的 LED 晶片精確貼裝到基板或支架上。

**關鍵參數**:
- 貼裝速度: 6,000-8,000 UPH
- 貼裝精度: ±10 μm
- 晶片尺寸: 200-1000 μm

### 2.2 IADL 定義

```yaml
# ========================================
# IADL Asset Definition - Die Bonder
# ========================================

asset_id: DB-2F-001
asset_type: DieBonder
manufacturer: Kulicke & Soffa (K&S)
model: iStack
serial_number: KS-2022-DB-001
description: LED 晶片固晶機,用於晶片貼裝

# ========================================
# 基本屬性
# ========================================
properties:
  installation_date: 2022-03-20
  warranty_expiry_date: 2027-03-20
  purchase_cost_usd: 650000
  
  location:
    building: Harvatek-Hsinchu
    floor: 2
    room: Die-Bonding-Area-A
    coordinates:
      x: 15.2
      y: 8.5
      z: 0.0
  
  specifications:
    bonding_speed_uph: 8000
    placement_accuracy_um: 10
    die_size_range_um: [200, 1000]
    substrate_size_mm: [50, 150]
    bonding_force_range_n: [0.5, 5.0]
    bonding_temperature_range_c: [150, 400]
    power_consumption_kw: 6.5
    compressed_air_pressure_bar: 6.0
    compressed_air_flow_lpm: 60
    nitrogen_flow_lpm: 20
    dimensions_mm:
      length: 1600
      width: 1200
      height: 1900
    weight_kg: 950

# ========================================
# 3D 模型定義
# ========================================
geometry:
  usd_reference: omniverse://harvatek/assets/equipment/die_bonder_ks_istack.usd
  alternative_formats:
    - type: glTF
      path: /models/db_2f_001.gltf
    - type: STEP
      path: /models/db_2f_001.step
  
  transform:
    translation: [15.2, 8.5, 0.0]
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]
  
  lod_levels:
    - level: high
      usd_reference: omniverse://harvatek/assets/equipment/die_bonder_ks_istack_high.usd
      max_distance: 5.0
      triangle_count: 120000
    - level: medium
      usd_reference: omniverse://harvatek/assets/equipment/die_bonder_ks_istack_med.usd
      max_distance: 20.0
      triangle_count: 40000
    - level: low
      usd_reference: omniverse://harvatek/assets/equipment/die_bonder_ks_istack_low.usd
      max_distance: 100.0
      triangle_count: 4000

# ========================================
# 連接點定義
# ========================================
connection_points:
  - id: power_inlet
    type: electrical
    voltage: 220V
    current: 30A
    phases: 3
    position: [-0.8, -0.6, 0.3]
  
  - id: cda_inlet
    type: gas
    gas_type: compressed_air
    pressure_bar: 6.0
    flow_rate_lpm: 60
    position: [-0.8, 0.6, 0.3]
  
  - id: n2_inlet
    type: gas
    gas_type: N2
    pressure_bar: 6.0
    flow_rate_lpm: 20
    position: [-0.8, 0.6, 0.5]
  
  - id: exhaust
    type: exhaust
    flow_rate_cmm: 3.0
    position: [0.8, 0.0, 1.8]

# ========================================
# 與其他資產的連接
# ========================================
connections:
  - type: powered_by
    target_asset_id: PANEL-2F-A-06
  
  - type: supplied_by
    target_asset_id: CDA-SYSTEM-MAIN
    connection_point: cda_inlet
  
  - type: supplied_by
    target_asset_id: N2-SYSTEM-MAIN
    connection_point: n2_inlet
  
  - type: exhausted_to
    target_asset_id: EXHAUST-DUCT-2F-A
  
  - type: controlled_by
    target_asset_id: PLC-2F-001
  
  - type: fed_by
    target_asset_id: DS-2F-001
    description: 接收分選後的晶片
  
  - type: feeds_to
    target_asset_id: WB-2F-001
    description: 固晶後送至打線機

# ========================================
# OT 數據標籤對應
# ========================================
data_tags:
  # 設備狀態
  - tag_id: DB_2F_001.Status
    protocol: SECS_GEM
    equipment_id: DB-2F-001
    variable_id: SVID_1
    data_type: string
    enum_values: [IDLE, RUNNING, ALARM, MAINTENANCE, SETUP]
    update_rate_ms: 1000
  
  # 生產數據
  - tag_id: DB_2F_001.UPH
    protocol: SECS_GEM
    equipment_id: DB-2F-001
    variable_id: SVID_10
    data_type: integer
    unit: units/hour
    range: [0, 10000]
    update_rate_ms: 5000
  
  - tag_id: DB_2F_001.TotalCount
    protocol: SECS_GEM
    equipment_id: DB-2F-001
    variable_id: SVID_11
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: DB_2F_001.GoodCount
    protocol: SECS_GEM
    equipment_id: DB-2F-001
    variable_id: SVID_12
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: DB_2F_001.RejectCount
    protocol: SECS_GEM
    equipment_id: DB-2F-001
    variable_id: SVID_13
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  # 製程參數
  - tag_id: DB_2F_001.BondingForce
    protocol: SECS_GEM
    equipment_id: DB-2F-001
    variable_id: SVID_20
    data_type: float
    unit: N
    range: [0.5, 5.0]
    update_rate_ms: 2000
  
  - tag_id: DB_2F_001.BondingTemperature
    protocol: SECS_GEM
    equipment_id: DB-2F-001
    variable_id: SVID_21
    data_type: float
    unit: celsius
    range: [150, 400]
    update_rate_ms: 5000
  
  - tag_id: DB_2F_001.PlacementAccuracy
    protocol: SECS_GEM
    equipment_id: DB-2F-001
    variable_id: SVID_22
    data_type: float
    unit: um
    range: [0, 50]
    update_rate_ms: 10000
  
  # 能源數據
  - tag_id: DB_2F_001.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.DB_2F_001.Power
    data_type: float
    unit: kW
    range: [0, 10]
    update_rate_ms: 5000
  
  # 告警
  - tag_id: DB_2F_001.Alarm
    protocol: SECS_GEM
    equipment_id: DB-2F-001
    event_id: ALID_ALL
    data_type: string
    update_rate_ms: 500

# ========================================
# 行為模型
# ========================================
behavior_model:
  type: PythonScript
  path: /models/die_bonder_performance.py
  
  inputs:
    - uph
    - bonding_force
    - bonding_temperature
    - placement_accuracy
    - power_consumption
  
  outputs:
    - oee
    - quality_rate
    - energy_efficiency
    - predicted_maintenance_date
    - process_capability_cpk
  
  update_frequency_hz: 0.1

# ========================================
# Omniverse 整合
# ========================================
omniverse:
  scene_path: /World/Harvatek/Floor_2/Die_Bonding_Area_A/DB_2F_001
  
  data_bindings:
    - usd_attribute: /DB_2F_001/StatusLight/Color
      data_tag: DB_2F_001.Status
      mapping:
        IDLE: [0.5, 0.5, 0.5, 1.0]
        RUNNING: [0.0, 1.0, 0.0, 1.0]
        ALARM: [1.0, 0.0, 0.0, 1.0]
        MAINTENANCE: [1.0, 1.0, 0.0, 1.0]
        SETUP: [0.0, 0.5, 1.0, 1.0]
    
    - usd_attribute: /DB_2F_001/Display/UPH
      data_tag: DB_2F_001.UPH
      format: "{value} UPH"
    
    - usd_attribute: /DB_2F_001/Display/Temperature
      data_tag: DB_2F_001.BondingTemperature
      format: "{value:.0f}°C"
  
  animations:
    - name: bonding_head_motion
      usd_prim: /DB_2F_001/BondingHead
      type: translation
      axis: [0, 0, 1]
      amplitude: 0.08
      frequency_source: DB_2F_001.UPH
      frequency_multiplier: 0.001
      enabled_when: DB_2F_001.Status == RUNNING

# ========================================
# 維護資訊
# ========================================
maintenance:
  maintenance_strategy: predictive
  last_pm_date: 2024-09-15
  next_pm_date: 2025-03-15
  pm_interval_days: 180
  mtbf_hours: 6000
  mttr_hours: 3.5
  
  maintenance_procedures:
    - id: PM-DB-001
      name: 加熱器校正
      frequency_days: 90
      estimated_duration_hours: 2.0
    
    - id: PM-DB-002
      name: 機械臂精度校正
      frequency_days: 180
      estimated_duration_hours: 3.0
    
    - id: PM-DB-003
      name: 全面保養
      frequency_days: 365
      estimated_duration_hours: 12.0
  
  spare_parts:
    - part_id: SP-DB-001
      name: 加熱器
      quantity_on_hand: 3
      reorder_point: 2
      lead_time_days: 21
      unit_cost_usd: 1500
    
    - part_id: SP-DB-002
      name: 吸嘴組
      quantity_on_hand: 10
      reorder_point: 5
      lead_time_days: 14
      unit_cost_usd: 200

# ========================================
# MES 整合
# ========================================
mes_integration:
  equipment_id: DB-2F-001
  work_center: DB-AREA-A
  resource_type: DIE_BONDER
  capacity_uph: 8000
  
  recipes:
    - recipe_id: RCP-DB-LED-3528
      product_type: LED-3528
      bonding_force_n: 2.0
      bonding_temperature_c: 250
      bonding_time_ms: 100
    
    - recipe_id: RCP-DB-LED-5050
      product_type: LED-5050
      bonding_force_n: 2.5
      bonding_temperature_c: 280
      bonding_time_ms: 120

# ========================================
# 元數據
# ========================================
metadata:
  created_date: 2022-03-01
  created_by: engineer@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 1.8
  iadl_version: 3.1
  
  tags:
    - production_equipment
    - die_bonding
    - critical_equipment
    - predictive_maintenance
```

---

## 3. Wire Bonder (打線機)

### 3.1 設備概述

**功能**: 使用金線或銀線連接 LED 晶片與基板電極。

**關鍵參數**:
- 打線速度: 6,000-10,000 UPH
- 打線精度: ±3 μm
- 線徑: 15-30 μm

### 3.2 IADL 定義

```yaml
# ========================================
# IADL Asset Definition - Wire Bonder
# ========================================

asset_id: WB-2F-001
asset_type: WireBonder
manufacturer: Kulicke & Soffa (K&S)
model: MaxumUltra
serial_number: KS-2023-WB-001
description: 金線打線機,用於 LED 封裝金線連接

# ========================================
# 基本屬性
# ========================================
properties:
  installation_date: 2023-03-15
  warranty_expiry_date: 2028-03-15
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
    bonding_speed_uph: 10000
    wire_diameter_um: 25.4
    bonding_force_range_gf: [30, 80]
    placement_accuracy_um: 3
    loop_height_um: [80, 150]
    power_consumption_kw: 5.5
    nitrogen_flow_lpm: 50
    compressed_air_flow_lpm: 100
    dimensions_mm:
      length: 1200
      width: 900
      height: 1800
    weight_kg: 850

# ========================================
# 3D 模型定義
# ========================================
geometry:
  usd_reference: omniverse://harvatek/assets/equipment/wire_bonder_ks_maxum.usd
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

# ========================================
# 連接點定義
# ========================================
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

# ========================================
# 與其他資產的連接
# ========================================
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
  
  - type: controlled_by
    target_asset_id: PLC-2F-001
  
  - type: fed_by
    target_asset_id: DB-2F-001
  
  - type: feeds_to
    target_asset_id: DISP-3F-001

# ========================================
# OT 數據標籤對應
# ========================================
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
  
  # 製程參數
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
  
  - tag_id: WB_2F_001.LoopHeight
    protocol: SECS_GEM
    equipment_id: WB-2F-001
    variable_id: SVID_22
    data_type: float
    unit: um
    update_rate_ms: 10000
  
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

# ========================================
# 行為模型
# ========================================
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
  update_frequency_hz: 0.1

# ========================================
# Omniverse 整合
# ========================================
omniverse:
  scene_path: /World/Harvatek/Floor_2/Wire_Bonding_Area_A/WB_2F_001
  
  data_bindings:
    - usd_attribute: /WB_2F_001/Status/Color
      data_tag: WB_2F_001.Status
      mapping:
        IDLE: [0.5, 0.5, 0.5, 1.0]
        RUNNING: [0.0, 1.0, 0.0, 1.0]
        ALARM: [1.0, 0.0, 0.0, 1.0]
        MAINTENANCE: [1.0, 1.0, 0.0, 1.0]
    
    - usd_attribute: /WB_2F_001/Display/UPH
      data_tag: WB_2F_001.UPH
      format: "{value} UPH"
    
    - usd_attribute: /WB_2F_001/Display/OEE
      data_tag: WB_2F_001.OEE
      format: "OEE: {value:.1f}%"
  
  animations:
    - name: bonding_head_motion
      usd_prim: /WB_2F_001/BondingHead
      type: translation
      axis: [0, 0, 1]
      amplitude: 0.05
      frequency_source: WB_2F_001.UPH
      frequency_multiplier: 0.001

# ========================================
# 維護資訊
# ========================================
maintenance:
  maintenance_strategy: predictive
  last_pm_date: 2024-09-01
  next_pm_date: 2025-03-01
  pm_interval_days: 180
  mtbf_hours: 8760
  mttr_hours: 4
  
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
      unit_cost_usd: 150
    
    - part_id: SP-WB-002
      name: 劈刀
      quantity_on_hand: 10
      reorder_point: 5
      lead_time_days: 14
      unit_cost_usd: 500

# ========================================
# MES 整合
# ========================================
mes_integration:
  equipment_id: WB-2F-001
  work_center: WB-AREA-A
  resource_type: WIRE_BONDER
  capacity_uph: 10000
  
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

# ========================================
# 元數據
# ========================================
metadata:
  created_date: 2023-03-01
  created_by: engineer@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 1.5
  iadl_version: 3.1
  
  tags:
    - production_equipment
    - wire_bonding
    - critical_equipment
    - predictive_maintenance
```

---

## 4. Dispenser (點膠機/封膠機)

### 4.1 設備概述

**功能**: 將環氧樹脂或矽膠精確點膠到 LED 封裝上,形成保護層和透鏡。

**關鍵參數**:
- 點膠速度: 4,000-6,000 UPH
- 點膠精度: ±0.1 mg
- 膠水類型: 環氧樹脂、矽膠

### 4.2 IADL 定義

```yaml
# ========================================
# IADL Asset Definition - Dispenser
# ========================================

asset_id: DISP-3F-001
asset_type: Dispenser
manufacturer: Musashi Engineering
model: SuperΣ-G4
serial_number: MUSASHI-2022-DISP-001
description: 高精度點膠機,用於 LED 封膠

# ========================================
# 基本屬性
# ========================================
properties:
  installation_date: 2022-06-10
  warranty_expiry_date: 2027-06-10
  purchase_cost_usd: 380000
  
  location:
    building: Harvatek-Hsinchu
    floor: 3
    room: Encapsulation-Area
    coordinates:
      x: 12.5
      y: 10.2
      z: 0.0
  
  specifications:
    dispensing_speed_uph: 6000
    dispensing_accuracy_mg: 0.1
    dispensing_volume_range_mg: [0.5, 50]
    material_types: [epoxy_resin, silicone]
    material_viscosity_range_cps: [1000, 100000]
    power_consumption_kw: 4.5
    compressed_air_pressure_bar: 6.0
    compressed_air_flow_lpm: 40
    dimensions_mm:
      length: 1400
      width: 1000
      height: 1600
    weight_kg: 650

# ========================================
# 3D 模型定義
# ========================================
geometry:
  usd_reference: omniverse://harvatek/assets/equipment/dispenser_musashi_g4.usd
  transform:
    translation: [12.5, 10.2, 0.0]
    rotation: [0.0, 0.0, 45.0]
    scale: [1.0, 1.0, 1.0]
  
  lod_levels:
    - level: high
      usd_reference: omniverse://harvatek/assets/equipment/dispenser_musashi_g4_high.usd
      max_distance: 5.0
    - level: medium
      usd_reference: omniverse://harvatek/assets/equipment/dispenser_musashi_g4_med.usd
      max_distance: 20.0
    - level: low
      usd_reference: omniverse://harvatek/assets/equipment/dispenser_musashi_g4_low.usd
      max_distance: 100.0

# ========================================
# 連接點定義
# ========================================
connection_points:
  - id: power_inlet
    type: electrical
    voltage: 220V
    current: 20A
    position: [-0.7, -0.5, 0.3]
  
  - id: cda_inlet
    type: gas
    gas_type: compressed_air
    pressure_bar: 6.0
    flow_rate_lpm: 40
    position: [-0.7, 0.5, 0.3]
  
  - id: exhaust
    type: exhaust
    flow_rate_cmm: 2.0
    position: [0.7, 0.0, 1.5]

# ========================================
# 與其他資產的連接
# ========================================
connections:
  - type: powered_by
    target_asset_id: PANEL-3F-A-02
  
  - type: supplied_by
    target_asset_id: CDA-SYSTEM-MAIN
    connection_point: cda_inlet
  
  - type: exhausted_to
    target_asset_id: EXHAUST-DUCT-3F-A
  
  - type: controlled_by
    target_asset_id: PLC-3F-001
  
  - type: fed_by
    target_asset_id: WB-2F-001
  
  - type: feeds_to
    target_asset_id: CURING-OVEN-3F-001

# ========================================
# OT 數據標籤對應
# ========================================
data_tags:
  # 設備狀態
  - tag_id: DISP_3F_001.Status
    protocol: SECS_GEM
    equipment_id: DISP-3F-001
    variable_id: SVID_1
    data_type: string
    enum_values: [IDLE, RUNNING, ALARM, MAINTENANCE, MATERIAL_CHANGE]
    update_rate_ms: 1000
  
  # 生產數據
  - tag_id: DISP_3F_001.UPH
    protocol: SECS_GEM
    equipment_id: DISP-3F-001
    variable_id: SVID_10
    data_type: integer
    unit: units/hour
    range: [0, 8000]
    update_rate_ms: 5000
  
  - tag_id: DISP_3F_001.TotalCount
    protocol: SECS_GEM
    equipment_id: DISP-3F-001
    variable_id: SVID_11
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: DISP_3F_001.GoodCount
    protocol: SECS_GEM
    equipment_id: DISP-3F-001
    variable_id: SVID_12
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: DISP_3F_001.RejectCount
    protocol: SECS_GEM
    equipment_id: DISP-3F-001
    variable_id: SVID_13
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  # 製程參數
  - tag_id: DISP_3F_001.DispensingWeight
    protocol: SECS_GEM
    equipment_id: DISP-3F-001
    variable_id: SVID_20
    data_type: float
    unit: mg
    range: [0.5, 50]
    update_rate_ms: 2000
    description: 點膠重量
  
  - tag_id: DISP_3F_001.DispensingPressure
    protocol: SECS_GEM
    equipment_id: DISP-3F-001
    variable_id: SVID_21
    data_type: float
    unit: bar
    range: [0, 10]
    update_rate_ms: 2000
    description: 點膠壓力
  
  - tag_id: DISP_3F_001.MaterialViscosity
    protocol: SECS_GEM
    equipment_id: DISP-3F-001
    variable_id: SVID_22
    data_type: float
    unit: cps
    range: [1000, 100000]
    update_rate_ms: 60000
    description: 膠水黏度
  
  - tag_id: DISP_3F_001.MaterialLevel
    protocol: SECS_GEM
    equipment_id: DISP-3F-001
    variable_id: SVID_23
    data_type: float
    unit: percent
    range: [0, 100]
    update_rate_ms: 10000
    description: 膠水液位
  
  # 能源數據
  - tag_id: DISP_3F_001.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.DISP_3F_001.Power
    data_type: float
    unit: kW
    range: [0, 8]
    update_rate_ms: 5000
  
  # 告警
  - tag_id: DISP_3F_001.Alarm
    protocol: SECS_GEM
    equipment_id: DISP-3F-001
    event_id: ALID_ALL
    data_type: string
    update_rate_ms: 500

# ========================================
# 行為模型
# ========================================
behavior_model:
  type: PythonScript
  path: /models/dispenser_performance.py
  
  inputs:
    - uph
    - dispensing_weight
    - dispensing_pressure
    - material_viscosity
    - material_level
  
  outputs:
    - oee
    - quality_rate
    - material_consumption_rate
    - predicted_material_runout_time
    - dispensing_consistency_score
  
  update_frequency_hz: 0.1
  
  algorithms:
    - name: material_consumption_prediction
      description: 預測膠水用量和補充時間
      method: linear_regression
      features: [uph, dispensing_weight, material_level]
    
    - name: dispensing_quality_monitoring
      description: 監控點膠品質一致性
      method: statistical_process_control
      parameters: [dispensing_weight]
      control_limits: [ucl, lcl, target]

# ========================================
# Omniverse 整合
# ========================================
omniverse:
  scene_path: /World/Harvatek/Floor_3/Encapsulation_Area/DISP_3F_001
  
  data_bindings:
    - usd_attribute: /DISP_3F_001/StatusLight/Color
      data_tag: DISP_3F_001.Status
      mapping:
        IDLE: [0.5, 0.5, 0.5, 1.0]
        RUNNING: [0.0, 1.0, 0.0, 1.0]
        ALARM: [1.0, 0.0, 0.0, 1.0]
        MAINTENANCE: [1.0, 1.0, 0.0, 1.0]
        MATERIAL_CHANGE: [1.0, 0.5, 0.0, 1.0]
    
    - usd_attribute: /DISP_3F_001/Display/UPH
      data_tag: DISP_3F_001.UPH
      format: "{value} UPH"
    
    - usd_attribute: /DISP_3F_001/Display/MaterialLevel
      data_tag: DISP_3F_001.MaterialLevel
      format: "Material: {value:.0f}%"
    
    - usd_attribute: /DISP_3F_001/MaterialTank/Level
      data_tag: DISP_3F_001.MaterialLevel
      mapping_type: scale
      axis: [1, 1, 1]
      scale_factor: 0.01
  
  animations:
    - name: dispensing_head_motion
      usd_prim: /DISP_3F_001/DispensingHead
      type: translation
      axis: [0, 0, 1]
      amplitude: 0.06
      frequency_source: DISP_3F_001.UPH
      frequency_multiplier: 0.001
      enabled_when: DISP_3F_001.Status == RUNNING
    
    - name: material_flow
      usd_prim: /DISP_3F_001/MaterialFlow
      type: particle_system
      particle_count_source: DISP_3F_001.UPH
      particle_size: 0.001
      color: [1.0, 1.0, 0.8, 0.8]
      enabled_when: DISP_3F_001.Status == RUNNING

# ========================================
# 維護資訊
# ========================================
maintenance:
  maintenance_strategy: preventive
  last_pm_date: 2024-10-01
  next_pm_date: 2025-01-01
  pm_interval_days: 90
  mtbf_hours: 4000
  mttr_hours: 2.5
  
  maintenance_procedures:
    - id: PM-DISP-001
      name: 針頭清潔
      frequency_days: 7
      estimated_duration_hours: 0.5
      required_skills: [dispensing_maintenance]
      required_parts: [cleaning_solution]
    
    - id: PM-DISP-002
      name: 針頭更換
      frequency_days: 30
      estimated_duration_hours: 1.0
      required_skills: [dispensing_maintenance]
      required_parts: [dispensing_needle]
    
    - id: PM-DISP-003
      name: 壓力系統檢查
      frequency_days: 90
      estimated_duration_hours: 2.0
      required_skills: [pneumatic_system]
      required_parts: []
    
    - id: PM-DISP-004
      name: 全面保養
      frequency_days: 180
      estimated_duration_hours: 6.0
      required_skills: [dispensing_maintenance, mechanical_maintenance]
      required_parts: [seals, filters, needles]
  
  spare_parts:
    - part_id: SP-DISP-001
      name: 點膠針頭
      quantity_on_hand: 50
      reorder_point: 20
      lead_time_days: 7
      unit_cost_usd: 30
    
    - part_id: SP-DISP-002
      name: 密封圈組
      quantity_on_hand: 10
      reorder_point: 5
      lead_time_days: 14
      unit_cost_usd: 150
    
    - part_id: SP-DISP-003
      name: 過濾器
      quantity_on_hand: 20
      reorder_point: 10
      lead_time_days: 7
      unit_cost_usd: 80

# ========================================
# 材料管理
# ========================================
material_management:
  materials:
    - material_id: MAT-EPOXY-001
      name: 環氧樹脂 (透明)
      type: epoxy_resin
      viscosity_cps: 15000
      shelf_life_days: 180
      storage_temperature_c: [15, 25]
      current_inventory_kg: 50
      reorder_point_kg: 20
      lead_time_days: 14
      unit_cost_usd_per_kg: 120
    
    - material_id: MAT-SILICONE-001
      name: 矽膠 (透明)
      type: silicone
      viscosity_cps: 8000
      shelf_life_days: 365
      storage_temperature_c: [15, 25]
      current_inventory_kg: 30
      reorder_point_kg: 15
      lead_time_days: 21
      unit_cost_usd_per_kg: 200
  
  material_consumption:
    average_consumption_kg_per_day: 2.5
    material_waste_rate_percent: 5

# ========================================
# MES 整合
# ========================================
mes_integration:
  equipment_id: DISP-3F-001
  work_center: ENCAP-AREA
  resource_type: DISPENSER
  capacity_uph: 6000
  
  recipes:
    - recipe_id: RCP-DISP-LED-3528-CLEAR
      product_type: LED-3528
      material_id: MAT-EPOXY-001
      dispensing_weight_mg: 5.0
      dispensing_pressure_bar: 4.5
      dispensing_time_ms: 200
    
    - recipe_id: RCP-DISP-LED-5050-CLEAR
      product_type: LED-5050
      material_id: MAT-EPOXY-001
      dispensing_weight_mg: 12.0
      dispensing_pressure_bar: 5.0
      dispensing_time_ms: 300
    
    - recipe_id: RCP-DISP-LED-3528-SILICONE
      product_type: LED-3528-HIGH-POWER
      material_id: MAT-SILICONE-001
      dispensing_weight_mg: 8.0
      dispensing_pressure_bar: 3.5
      dispensing_time_ms: 250

# ========================================
# 品質管理
# ========================================
quality_management:
  spc_enabled: true
  spc_parameters:
    - parameter: dispensing_weight
      control_chart_type: X-bar_R
      sample_size: 10
      sample_frequency_minutes: 30
      ucl: 5.3
      lcl: 4.7
      target: 5.0
      unit: mg
  
  inspection:
    inspection_frequency_percent: 10
    inspection_methods:
      - method: visual_inspection
        criteria: [shape, position, bubble]
      - method: weight_measurement
        tolerance_mg: 0.3

# ========================================
# 元數據
# ========================================
metadata:
  created_date: 2022-06-01
  created_by: engineer@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 1.6
  iadl_version: 3.1
  
  tags:
    - production_equipment
    - dispensing
    - encapsulation
    - critical_equipment
    - material_intensive
```

---

*文件未完,繼續下一部分...*



## 5. Tester (測試機)

### 5.1 設備概述

**功能**: 測試 LED 的光電特性,包括波長、亮度、電壓、電流等參數。

**關鍵參數**:
- 測試速度: 10,000-15,000 UPH
- 測試項目: 波長、光通量、色溫、電壓、電流
- 測試精度: ±0.5 nm (波長), ±3% (光通量)

### 5.2 IADL 定義

```yaml
# ========================================
# IADL Asset Definition - LED Tester
# ========================================

asset_id: TESTER-3F-001
asset_type: LEDTester
manufacturer: Chroma ATE
model: Model 8000
serial_number: CHROMA-2023-TESTER-001
description: LED 光電特性測試機

# ========================================
# 基本屬性
# ========================================
properties:
  installation_date: 2023-01-15
  warranty_expiry_date: 2028-01-15
  purchase_cost_usd: 280000
  
  location:
    building: Harvatek-Hsinchu
    floor: 3
    room: Testing-Area
    coordinates:
      x: 22.8
      y: 18.5
      z: 0.0
  
  specifications:
    testing_speed_uph: 15000
    test_items:
      - wavelength
      - luminous_flux
      - luminous_intensity
      - color_temperature
      - forward_voltage
      - forward_current
      - reverse_current
      - color_rendering_index
    wavelength_range_nm: [380, 780]
    wavelength_accuracy_nm: 0.5
    luminous_flux_range_lm: [0.1, 1000]
    luminous_flux_accuracy_percent: 3
    integrating_sphere_diameter_inch: 12
    power_consumption_kw: 6.0
    dimensions_mm:
      length: 1800
      width: 1200
      height: 1700
    weight_kg: 900

# ========================================
# 3D 模型定義
# ========================================
geometry:
  usd_reference: omniverse://harvatek/assets/equipment/led_tester_chroma_8000.usd
  transform:
    translation: [22.8, 18.5, 0.0]
    rotation: [0.0, 0.0, 270.0]
    scale: [1.0, 1.0, 1.0]
  
  lod_levels:
    - level: high
      usd_reference: omniverse://harvatek/assets/equipment/led_tester_chroma_8000_high.usd
      max_distance: 5.0
      triangle_count: 100000
    - level: medium
      usd_reference: omniverse://harvatek/assets/equipment/led_tester_chroma_8000_med.usd
      max_distance: 20.0
      triangle_count: 35000
    - level: low
      usd_reference: omniverse://harvatek/assets/equipment/led_tester_chroma_8000_low.usd
      max_distance: 100.0
      triangle_count: 3500

# ========================================
# 連接點定義
# ========================================
connection_points:
  - id: power_inlet
    type: electrical
    voltage: 220V
    current: 30A
    phases: 3
    position: [-0.9, -0.6, 0.3]
  
  - id: network_port
    type: network
    protocol: ethernet
    speed_gbps: 1.0
    position: [-0.9, 0.6, 0.3]

# ========================================
# 與其他資產的連接
# ========================================
connections:
  - type: powered_by
    target_asset_id: PANEL-3F-A-05
  
  - type: controlled_by
    target_asset_id: PLC-3F-001
  
  - type: monitored_by
    target_asset_id: SCADA-SYSTEM
  
  - type: fed_by
    target_asset_id: CURING-OVEN-3F-001
  
  - type: feeds_to
    target_asset_id: TAPING-3F-001

# ========================================
# OT 數據標籤對應
# ========================================
data_tags:
  # 設備狀態
  - tag_id: TESTER_3F_001.Status
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_1
    data_type: string
    enum_values: [IDLE, RUNNING, ALARM, MAINTENANCE, CALIBRATION]
    update_rate_ms: 1000
  
  # 生產數據
  - tag_id: TESTER_3F_001.UPH
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_10
    data_type: integer
    unit: units/hour
    range: [0, 18000]
    update_rate_ms: 5000
  
  - tag_id: TESTER_3F_001.TotalCount
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_11
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: TESTER_3F_001.PassCount
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_12
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: TESTER_3F_001.FailCount
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_13
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: TESTER_3F_001.YieldRate
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_14
    data_type: float
    unit: percent
    range: [0, 100]
    update_rate_ms: 5000
  
  # 測試數據 - 平均值
  - tag_id: TESTER_3F_001.AvgWavelength
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_20
    data_type: float
    unit: nm
    range: [380, 780]
    update_rate_ms: 10000
  
  - tag_id: TESTER_3F_001.AvgLuminousFlux
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_21
    data_type: float
    unit: lm
    range: [0.1, 1000]
    update_rate_ms: 10000
  
  - tag_id: TESTER_3F_001.AvgColorTemperature
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_22
    data_type: float
    unit: K
    range: [2000, 10000]
    update_rate_ms: 10000
  
  - tag_id: TESTER_3F_001.AvgForwardVoltage
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_23
    data_type: float
    unit: V
    range: [0, 5]
    update_rate_ms: 10000
  
  - tag_id: TESTER_3F_001.AvgForwardCurrent
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_24
    data_type: float
    unit: mA
    range: [0, 1000]
    update_rate_ms: 10000
  
  # 測試數據 - 標準差
  - tag_id: TESTER_3F_001.StdDevWavelength
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_30
    data_type: float
    unit: nm
    range: [0, 10]
    update_rate_ms: 10000
  
  - tag_id: TESTER_3F_001.StdDevLuminousFlux
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    variable_id: SVID_31
    data_type: float
    unit: lm
    range: [0, 50]
    update_rate_ms: 10000
  
  # 能源數據
  - tag_id: TESTER_3F_001.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.TESTER_3F_001.Power
    data_type: float
    unit: kW
    range: [0, 10]
    update_rate_ms: 5000
  
  # 告警
  - tag_id: TESTER_3F_001.Alarm
    protocol: SECS_GEM
    equipment_id: TESTER-3F-001
    event_id: ALID_ALL
    data_type: string
    update_rate_ms: 500

# ========================================
# 行為模型
# ========================================
behavior_model:
  type: PythonScript
  path: /models/led_tester_performance.py
  
  inputs:
    - uph
    - yield_rate
    - avg_wavelength
    - std_dev_wavelength
    - avg_luminous_flux
    - std_dev_luminous_flux
  
  outputs:
    - oee
    - test_reliability_score
    - process_capability_cpk
    - predicted_calibration_date
    - anomaly_score
  
  update_frequency_hz: 0.1
  
  algorithms:
    - name: test_reliability_analysis
      description: 分析測試結果的可靠性和一致性
      method: statistical_analysis
      features: [std_dev_wavelength, std_dev_luminous_flux]
    
    - name: calibration_prediction
      description: 預測校正需求
      method: time_series_analysis
      features: [avg_wavelength, avg_luminous_flux]
      prediction_horizon_days: 90

# ========================================
# Omniverse 整合
# ========================================
omniverse:
  scene_path: /World/Harvatek/Floor_3/Testing_Area/TESTER_3F_001
  
  data_bindings:
    - usd_attribute: /TESTER_3F_001/StatusLight/Color
      data_tag: TESTER_3F_001.Status
      mapping:
        IDLE: [0.5, 0.5, 0.5, 1.0]
        RUNNING: [0.0, 1.0, 0.0, 1.0]
        ALARM: [1.0, 0.0, 0.0, 1.0]
        MAINTENANCE: [1.0, 1.0, 0.0, 1.0]
        CALIBRATION: [0.0, 0.5, 1.0, 1.0]
    
    - usd_attribute: /TESTER_3F_001/Display/UPH
      data_tag: TESTER_3F_001.UPH
      format: "{value} UPH"
    
    - usd_attribute: /TESTER_3F_001/Display/YieldRate
      data_tag: TESTER_3F_001.YieldRate
      format: "Yield: {value:.1f}%"
    
    - usd_attribute: /TESTER_3F_001/Display/AvgWavelength
      data_tag: TESTER_3F_001.AvgWavelength
      format: "λ: {value:.1f} nm"
    
    - usd_attribute: /TESTER_3F_001/IntegratingSphere/EmissiveColor
      data_tag: TESTER_3F_001.AvgWavelength
      mapping_type: wavelength_to_rgb
      enabled_when: TESTER_3F_001.Status == RUNNING
  
  animations:
    - name: test_probe_motion
      usd_prim: /TESTER_3F_001/TestProbe
      type: translation
      axis: [0, 0, 1]
      amplitude: 0.04
      frequency_source: TESTER_3F_001.UPH
      frequency_multiplier: 0.001
      enabled_when: TESTER_3F_001.Status == RUNNING

# ========================================
# 維護資訊
# ========================================
maintenance:
  maintenance_strategy: preventive
  last_pm_date: 2024-10-01
  next_pm_date: 2025-01-01
  pm_interval_days: 90
  mtbf_hours: 7000
  mttr_hours: 3
  
  maintenance_procedures:
    - id: PM-TESTER-001
      name: 光學系統校正
      frequency_days: 90
      estimated_duration_hours: 3.0
      required_skills: [optical_calibration]
      required_parts: [calibration_standard]
    
    - id: PM-TESTER-002
      name: 積分球清潔
      frequency_days: 180
      estimated_duration_hours: 2.0
      required_skills: [optical_maintenance]
      required_parts: [cleaning_solution]
    
    - id: PM-TESTER-003
      name: 電源供應器檢查
      frequency_days: 180
      estimated_duration_hours: 1.5
      required_skills: [electrical_maintenance]
      required_parts: []
    
    - id: PM-TESTER-004
      name: 全面保養
      frequency_days: 365
      estimated_duration_hours: 8.0
      required_skills: [optical_calibration, electrical_maintenance, mechanical_maintenance]
      required_parts: [calibration_standard, filters, lamps]
  
  spare_parts:
    - part_id: SP-TESTER-001
      name: 校正標準光源
      quantity_on_hand: 2
      reorder_point: 1
      lead_time_days: 30
      unit_cost_usd: 8000
    
    - part_id: SP-TESTER-002
      name: 測試探針
      quantity_on_hand: 20
      reorder_point: 10
      lead_time_days: 14
      unit_cost_usd: 100
    
    - part_id: SP-TESTER-003
      name: 光譜儀燈泡
      quantity_on_hand: 5
      reorder_point: 2
      lead_time_days: 21
      unit_cost_usd: 1200

# ========================================
# 校正管理
# ========================================
calibration:
  calibration_required: true
  calibration_frequency_days: 90
  last_calibration_date: 2024-10-01
  next_calibration_date: 2024-12-30
  calibration_standard: NIST_traceable
  
  calibration_parameters:
    - parameter: wavelength
      standard_value_nm: 546.1
      tolerance_nm: 0.5
      last_calibration_error_nm: 0.2
    
    - parameter: luminous_flux
      standard_value_lm: 100.0
      tolerance_percent: 3
      last_calibration_error_percent: 1.5
  
  calibration_history:
    - date: 2024-10-01
      technician: John Doe
      result: PASS
      wavelength_error_nm: 0.2
      luminous_flux_error_percent: 1.5
      certificate_number: CAL-2024-10-001

# ========================================
# MES 整合
# ========================================
mes_integration:
  equipment_id: TESTER-3F-001
  work_center: TESTING-AREA
  resource_type: LED_TESTER
  capacity_uph: 15000
  
  recipes:
    - recipe_id: RCP-TEST-BLUE-LED
      product_type: BLUE-LED
      test_items: [wavelength, luminous_flux, forward_voltage]
      wavelength_spec:
        target_nm: 465
        tolerance_nm: 5
      luminous_flux_spec:
        min_lm: 0.5
        max_lm: 2.0
      forward_voltage_spec:
        min_v: 2.8
        max_v: 3.4
    
    - recipe_id: RCP-TEST-WHITE-LED
      product_type: WHITE-LED
      test_items: [color_temperature, luminous_flux, forward_voltage, cri]
      color_temperature_spec:
        target_k: 5000
        tolerance_k: 500
      luminous_flux_spec:
        min_lm: 10
        max_lm: 50
      forward_voltage_spec:
        min_v: 2.8
        max_v: 3.6
      cri_spec:
        min: 80

# ========================================
# 品質管理
# ========================================
quality_management:
  spc_enabled: true
  spc_parameters:
    - parameter: wavelength
      control_chart_type: X-bar_R
      sample_size: 25
      sample_frequency_minutes: 30
      ucl: 467
      lcl: 463
      target: 465
      unit: nm
    
    - parameter: luminous_flux
      control_chart_type: X-bar_R
      sample_size: 25
      sample_frequency_minutes: 30
      ucl: 1.2
      lcl: 0.8
      target: 1.0
      unit: lm
  
  data_logging:
    log_all_test_results: true
    retention_period_days: 1095  # 3 years
    export_format: csv
    export_frequency: daily

# ========================================
# 元數據
# ========================================
metadata:
  created_date: 2023-01-01
  created_by: engineer@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 1.4
  iadl_version: 3.1
  
  tags:
    - production_equipment
    - testing
    - quality_control
    - critical_equipment
    - calibration_required
```

---

## 6. Taping Machine (編帶包裝機)

### 6.1 設備概述

**功能**: 將測試合格的 LED 自動編帶包裝,便於運輸和 SMT 貼裝。

**關鍵參數**:
- 編帶速度: 8,000-12,000 UPH
- 包裝精度: ±0.5 mm
- 編帶寬度: 8mm, 12mm, 16mm

### 6.2 IADL 定義

```yaml
# ========================================
# IADL Asset Definition - Taping Machine
# ========================================

asset_id: TAPING-3F-001
asset_type: TapingMachine
manufacturer: ASM Pacific Technology
model: AD-Taper
serial_number: ASM-2022-TAPING-001
description: LED 自動編帶包裝機

# ========================================
# 基本屬性
# ========================================
properties:
  installation_date: 2022-08-20
  warranty_expiry_date: 2027-08-20
  purchase_cost_usd: 320000
  
  location:
    building: Harvatek-Hsinchu
    floor: 3
    room: Packaging-Area
    coordinates:
      x: 28.5
      y: 22.3
      z: 0.0
  
  specifications:
    taping_speed_uph: 12000
    placement_accuracy_mm: 0.5
    tape_width_options_mm: [8, 12, 16]
    reel_diameter_inch: 7
    component_size_range_mm: [1.0, 10.0]
    power_consumption_kw: 4.0
    compressed_air_pressure_bar: 6.0
    compressed_air_flow_lpm: 50
    vacuum_pressure_kpa: -80
    dimensions_mm:
      length: 1500
      width: 1100
      height: 1600
    weight_kg: 750

# ========================================
# 3D 模型定義
# ========================================
geometry:
  usd_reference: omniverse://harvatek/assets/equipment/taping_machine_asm_adtaper.usd
  transform:
    translation: [28.5, 22.3, 0.0]
    rotation: [0.0, 0.0, 90.0]
    scale: [1.0, 1.0, 1.0]
  
  lod_levels:
    - level: high
      usd_reference: omniverse://harvatek/assets/equipment/taping_machine_asm_adtaper_high.usd
      max_distance: 5.0
    - level: medium
      usd_reference: omniverse://harvatek/assets/equipment/taping_machine_asm_adtaper_med.usd
      max_distance: 20.0
    - level: low
      usd_reference: omniverse://harvatek/assets/equipment/taping_machine_asm_adtaper_low.usd
      max_distance: 100.0

# ========================================
# 連接點定義
# ========================================
connection_points:
  - id: power_inlet
    type: electrical
    voltage: 220V
    current: 20A
    position: [-0.75, -0.55, 0.3]
  
  - id: cda_inlet
    type: gas
    gas_type: compressed_air
    pressure_bar: 6.0
    flow_rate_lpm: 50
    position: [-0.75, 0.55, 0.3]
  
  - id: vacuum_inlet
    type: vacuum
    pressure_kpa: -80
    flow_rate_lpm: 80
    position: [-0.75, 0.55, 0.5]

# ========================================
# 與其他資產的連接
# ========================================
connections:
  - type: powered_by
    target_asset_id: PANEL-3F-A-08
  
  - type: supplied_by
    target_asset_id: CDA-SYSTEM-MAIN
    connection_point: cda_inlet
  
  - type: supplied_by
    target_asset_id: VACUUM-SYSTEM-3F
    connection_point: vacuum_inlet
  
  - type: controlled_by
    target_asset_id: PLC-3F-001
  
  - type: fed_by
    target_asset_id: TESTER-3F-001
  
  - type: feeds_to
    target_asset_id: WAREHOUSE

# ========================================
# OT 數據標籤對應
# ========================================
data_tags:
  # 設備狀態
  - tag_id: TAPING_3F_001.Status
    protocol: SECS_GEM
    equipment_id: TAPING-3F-001
    variable_id: SVID_1
    data_type: string
    enum_values: [IDLE, RUNNING, ALARM, MAINTENANCE, TAPE_CHANGE]
    update_rate_ms: 1000
  
  # 生產數據
  - tag_id: TAPING_3F_001.UPH
    protocol: SECS_GEM
    equipment_id: TAPING-3F-001
    variable_id: SVID_10
    data_type: integer
    unit: units/hour
    range: [0, 15000]
    update_rate_ms: 5000
  
  - tag_id: TAPING_3F_001.TotalCount
    protocol: SECS_GEM
    equipment_id: TAPING-3F-001
    variable_id: SVID_11
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: TAPING_3F_001.GoodCount
    protocol: SECS_GEM
    equipment_id: TAPING-3F-001
    variable_id: SVID_12
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: TAPING_3F_001.RejectCount
    protocol: SECS_GEM
    equipment_id: TAPING-3F-001
    variable_id: SVID_13
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  # 製程參數
  - tag_id: TAPING_3F_001.TapeRemaining
    protocol: SECS_GEM
    equipment_id: TAPING-3F-001
    variable_id: SVID_20
    data_type: float
    unit: meter
    range: [0, 1000]
    update_rate_ms: 10000
    description: 剩餘編帶長度
  
  - tag_id: TAPING_3F_001.VacuumPressure
    protocol: SECS_GEM
    equipment_id: TAPING-3F-001
    variable_id: SVID_21
    data_type: float
    unit: kPa
    range: [-100, 0]
    update_rate_ms: 5000
  
  - tag_id: TAPING_3F_001.PlacementAccuracy
    protocol: SECS_GEM
    equipment_id: TAPING-3F-001
    variable_id: SVID_22
    data_type: float
    unit: mm
    range: [0, 2]
    update_rate_ms: 10000
  
  # 能源數據
  - tag_id: TAPING_3F_001.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.TAPING_3F_001.Power
    data_type: float
    unit: kW
    range: [0, 8]
    update_rate_ms: 5000
  
  # 告警
  - tag_id: TAPING_3F_001.Alarm
    protocol: SECS_GEM
    equipment_id: TAPING-3F-001
    event_id: ALID_ALL
    data_type: string
    update_rate_ms: 500

# ========================================
# 行為模型
# ========================================
behavior_model:
  type: PythonScript
  path: /models/taping_machine_performance.py
  
  inputs:
    - uph
    - tape_remaining
    - vacuum_pressure
    - placement_accuracy
  
  outputs:
    - oee
    - quality_rate
    - predicted_tape_runout_time
    - material_efficiency
  
  update_frequency_hz: 0.1

# ========================================
# Omniverse 整合
# ========================================
omniverse:
  scene_path: /World/Harvatek/Floor_3/Packaging_Area/TAPING_3F_001
  
  data_bindings:
    - usd_attribute: /TAPING_3F_001/StatusLight/Color
      data_tag: TAPING_3F_001.Status
      mapping:
        IDLE: [0.5, 0.5, 0.5, 1.0]
        RUNNING: [0.0, 1.0, 0.0, 1.0]
        ALARM: [1.0, 0.0, 0.0, 1.0]
        MAINTENANCE: [1.0, 1.0, 0.0, 1.0]
        TAPE_CHANGE: [1.0, 0.5, 0.0, 1.0]
    
    - usd_attribute: /TAPING_3F_001/Display/UPH
      data_tag: TAPING_3F_001.UPH
      format: "{value} UPH"
    
    - usd_attribute: /TAPING_3F_001/Display/TapeRemaining
      data_tag: TAPING_3F_001.TapeRemaining
      format: "Tape: {value:.0f}m"
    
    - usd_attribute: /TAPING_3F_001/TapeReel/Rotation
      data_tag: TAPING_3F_001.UPH
      mapping_type: rotation
      axis: [0, 1, 0]
      speed_multiplier: 0.01
  
  animations:
    - name: tape_feeding
      usd_prim: /TAPING_3F_001/TapeFeed
      type: translation
      axis: [1, 0, 0]
      speed_source: TAPING_3F_001.UPH
      speed_multiplier: 0.001
      enabled_when: TAPING_3F_001.Status == RUNNING
    
    - name: picker_motion
      usd_prim: /TAPING_3F_001/Picker
      type: translation
      axis: [0, 0, 1]
      amplitude: 0.05
      frequency_source: TAPING_3F_001.UPH
      frequency_multiplier: 0.001
      enabled_when: TAPING_3F_001.Status == RUNNING

# ========================================
# 維護資訊
# ========================================
maintenance:
  maintenance_strategy: preventive
  last_pm_date: 2024-09-01
  next_pm_date: 2024-12-01
  pm_interval_days: 90
  mtbf_hours: 5500
  mttr_hours: 2.5
  
  maintenance_procedures:
    - id: PM-TAPING-001
      name: 真空系統檢查
      frequency_days: 30
      estimated_duration_hours: 1.0
    
    - id: PM-TAPING-002
      name: 機械臂校正
      frequency_days: 90
      estimated_duration_hours: 2.0
    
    - id: PM-TAPING-003
      name: 全面保養
      frequency_days: 180
      estimated_duration_hours: 6.0
  
  spare_parts:
    - part_id: SP-TAPING-001
      name: 真空吸嘴
      quantity_on_hand: 30
      reorder_point: 15
      lead_time_days: 14
      unit_cost_usd: 40
    
    - part_id: SP-TAPING-002
      name: 編帶捲軸
      quantity_on_hand: 10
      reorder_point: 5
      lead_time_days: 21
      unit_cost_usd: 200

# ========================================
# 材料管理
# ========================================
material_management:
  materials:
    - material_id: MAT-TAPE-8MM
      name: 編帶 (8mm)
      type: carrier_tape
      width_mm: 8
      length_m: 1000
      current_inventory_rolls: 50
      reorder_point_rolls: 20
      lead_time_days: 14
      unit_cost_usd_per_roll: 15
    
    - material_id: MAT-TAPE-12MM
      name: 編帶 (12mm)
      type: carrier_tape
      width_mm: 12
      length_m: 1000
      current_inventory_rolls: 40
      reorder_point_rolls: 15
      lead_time_days: 14
      unit_cost_usd_per_roll: 20
    
    - material_id: MAT-COVER-TAPE
      name: 上帶
      type: cover_tape
      width_mm: 16
      length_m: 1000
      current_inventory_rolls: 60
      reorder_point_rolls: 25
      lead_time_days: 14
      unit_cost_usd_per_roll: 10

# ========================================
# MES 整合
# ========================================
mes_integration:
  equipment_id: TAPING-3F-001
  work_center: PACKAGING-AREA
  resource_type: TAPING_MACHINE
  capacity_uph: 12000
  
  recipes:
    - recipe_id: RCP-TAPING-LED-3528
      product_type: LED-3528
      tape_width_mm: 8
      component_spacing_mm: 4
      components_per_reel: 5000
    
    - recipe_id: RCP-TAPING-LED-5050
      product_type: LED-5050
      tape_width_mm: 12
      component_spacing_mm: 8
      components_per_reel: 3000

# ========================================
# 元數據
# ========================================
metadata:
  created_date: 2022-08-01
  created_by: engineer@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 1.5
  iadl_version: 3.1
  
  tags:
    - production_equipment
    - packaging
    - taping
    - material_intensive
```

---

## 7. AOI System (自動光學檢測)

### 7.1 設備概述

**功能**: 使用高解析度相機和影像處理技術,自動檢測 LED 封裝的外觀缺陷。

**關鍵參數**:
- 檢測速度: 5,000-8,000 UPH
- 解析度: 5 μm
- 缺陷檢測: 劃傷、氣泡、污染、偏移

### 7.2 IADL 定義

```yaml
# ========================================
# IADL Asset Definition - AOI System
# ========================================

asset_id: AOI-3F-001
asset_type: AOISystem
manufacturer: Saki Corporation
model: BF-3Di
serial_number: SAKI-2023-AOI-001
description: 3D 自動光學檢測系統

# ========================================
# 基本屬性
# ========================================
properties:
  installation_date: 2023-04-10
  warranty_expiry_date: 2028-04-10
  purchase_cost_usd: 420000
  
  location:
    building: Harvatek-Hsinchu
    floor: 3
    room: Inspection-Area
    coordinates:
      x: 18.2
      y: 25.8
      z: 0.0
  
  specifications:
    inspection_speed_uph: 8000
    resolution_um: 5
    inspection_methods:
      - 2d_imaging
      - 3d_imaging
      - laser_profiling
    defect_types:
      - scratch
      - bubble
      - contamination
      - offset
      - missing_component
      - wrong_component
    camera_resolution_mp: 12
    lighting_types:
      - white_led
      - rgb_led
      - coaxial
      - ring_light
    power_consumption_kw: 5.0
    dimensions_mm:
      length: 1600
      width: 1100
      height: 1500
    weight_kg: 800

# ========================================
# 3D 模型定義
# ========================================
geometry:
  usd_reference: omniverse://harvatek/assets/equipment/aoi_saki_bf3di.usd
  transform:
    translation: [18.2, 25.8, 0.0]
    rotation: [0.0, 0.0, 180.0]
    scale: [1.0, 1.0, 1.0]
  
  lod_levels:
    - level: high
      usd_reference: omniverse://harvatek/assets/equipment/aoi_saki_bf3di_high.usd
      max_distance: 5.0
    - level: medium
      usd_reference: omniverse://harvatek/assets/equipment/aoi_saki_bf3di_med.usd
      max_distance: 20.0
    - level: low
      usd_reference: omniverse://harvatek/assets/equipment/aoi_saki_bf3di_low.usd
      max_distance: 100.0

# ========================================
# 連接點定義
# ========================================
connection_points:
  - id: power_inlet
    type: electrical
    voltage: 220V
    current: 25A
    position: [-0.8, -0.55, 0.3]
  
  - id: network_port
    type: network
    protocol: ethernet
    speed_gbps: 10.0
    position: [-0.8, 0.55, 0.3]

# ========================================
# 與其他資產的連接
# ========================================
connections:
  - type: powered_by
    target_asset_id: PANEL-3F-A-06
  
  - type: controlled_by
    target_asset_id: PLC-3F-001
  
  - type: monitored_by
    target_asset_id: SCADA-SYSTEM
  
  - type: inspects_from
    target_asset_id: DISP-3F-001

# ========================================
# OT 數據標籤對應
# ========================================
data_tags:
  # 設備狀態
  - tag_id: AOI_3F_001.Status
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    variable_id: SVID_1
    data_type: string
    enum_values: [IDLE, RUNNING, ALARM, MAINTENANCE, CALIBRATION]
    update_rate_ms: 1000
  
  # 生產數據
  - tag_id: AOI_3F_001.UPH
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    variable_id: SVID_10
    data_type: integer
    unit: units/hour
    range: [0, 10000]
    update_rate_ms: 5000
  
  - tag_id: AOI_3F_001.TotalCount
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    variable_id: SVID_11
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: AOI_3F_001.PassCount
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    variable_id: SVID_12
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: AOI_3F_001.FailCount
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    variable_id: SVID_13
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: AOI_3F_001.YieldRate
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    variable_id: SVID_14
    data_type: float
    unit: percent
    range: [0, 100]
    update_rate_ms: 5000
  
  # 缺陷統計
  - tag_id: AOI_3F_001.ScratchCount
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    variable_id: SVID_20
    data_type: integer
    unit: units
    update_rate_ms: 10000
  
  - tag_id: AOI_3F_001.BubbleCount
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    variable_id: SVID_21
    data_type: integer
    unit: units
    update_rate_ms: 10000
  
  - tag_id: AOI_3F_001.ContaminationCount
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    variable_id: SVID_22
    data_type: integer
    unit: units
    update_rate_ms: 10000
  
  - tag_id: AOI_3F_001.OffsetCount
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    variable_id: SVID_23
    data_type: integer
    unit: units
    update_rate_ms: 10000
  
  # 能源數據
  - tag_id: AOI_3F_001.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.AOI_3F_001.Power
    data_type: float
    unit: kW
    range: [0, 8]
    update_rate_ms: 5000
  
  # 告警
  - tag_id: AOI_3F_001.Alarm
    protocol: SECS_GEM
    equipment_id: AOI-3F-001
    event_id: ALID_ALL
    data_type: string
    update_rate_ms: 500

# ========================================
# 行為模型
# ========================================
behavior_model:
  type: PythonScript
  path: /models/aoi_performance.py
  
  inputs:
    - uph
    - yield_rate
    - scratch_count
    - bubble_count
    - contamination_count
    - offset_count
  
  outputs:
    - oee
    - defect_rate_by_type
    - inspection_reliability_score
    - false_positive_rate
    - false_negative_rate
  
  update_frequency_hz: 0.1
  
  algorithms:
    - name: defect_pattern_analysis
      description: 分析缺陷模式,識別製程問題
      method: machine_learning
      model_type: classification
      features: [defect_type, defect_location, defect_size]
    
    - name: inspection_accuracy_monitoring
      description: 監控檢測準確性
      method: statistical_analysis
      parameters: [false_positive_rate, false_negative_rate]

# ========================================
# Omniverse 整合
# ========================================
omniverse:
  scene_path: /World/Harvatek/Floor_3/Inspection_Area/AOI_3F_001
  
  data_bindings:
    - usd_attribute: /AOI_3F_001/StatusLight/Color
      data_tag: AOI_3F_001.Status
      mapping:
        IDLE: [0.5, 0.5, 0.5, 1.0]
        RUNNING: [0.0, 1.0, 0.0, 1.0]
        ALARM: [1.0, 0.0, 0.0, 1.0]
        MAINTENANCE: [1.0, 1.0, 0.0, 1.0]
        CALIBRATION: [0.0, 0.5, 1.0, 1.0]
    
    - usd_attribute: /AOI_3F_001/Display/UPH
      data_tag: AOI_3F_001.UPH
      format: "{value} UPH"
    
    - usd_attribute: /AOI_3F_001/Display/YieldRate
      data_tag: AOI_3F_001.YieldRate
      format: "Yield: {value:.1f}%"
    
    - usd_attribute: /AOI_3F_001/CameraLight/Intensity
      data_tag: AOI_3F_001.Status
      mapping:
        RUNNING: 1.0
        default: 0.0
  
  animations:
    - name: camera_scanning
      usd_prim: /AOI_3F_001/Camera
      type: translation
      axis: [1, 0, 0]
      amplitude: 0.2
      frequency_source: AOI_3F_001.UPH
      frequency_multiplier: 0.001
      enabled_when: AOI_3F_001.Status == RUNNING

# ========================================
# 維護資訊
# ========================================
maintenance:
  maintenance_strategy: preventive
  last_pm_date: 2024-10-01
  next_pm_date: 2025-01-01
  pm_interval_days: 90
  mtbf_hours: 6500
  mttr_hours: 3
  
  maintenance_procedures:
    - id: PM-AOI-001
      name: 鏡頭清潔
      frequency_days: 30
      estimated_duration_hours: 0.5
    
    - id: PM-AOI-002
      name: 照明系統檢查
      frequency_days: 90
      estimated_duration_hours: 1.5
    
    - id: PM-AOI-003
      name: 影像校正
      frequency_days: 90
      estimated_duration_hours: 2.0
    
    - id: PM-AOI-004
      name: 全面保養
      frequency_days: 365
      estimated_duration_hours: 8.0
  
  spare_parts:
    - part_id: SP-AOI-001
      name: 相機鏡頭
      quantity_on_hand: 2
      reorder_point: 1
      lead_time_days: 30
      unit_cost_usd: 8000
    
    - part_id: SP-AOI-002
      name: LED 照明模組
      quantity_on_hand: 5
      reorder_point: 2
      lead_time_days: 21
      unit_cost_usd: 1500

# ========================================
# MES 整合
# ========================================
mes_integration:
  equipment_id: AOI-3F-001
  work_center: INSPECTION-AREA
  resource_type: AOI_SYSTEM
  capacity_uph: 8000
  
  recipes:
    - recipe_id: RCP-AOI-LED-3528
      product_type: LED-3528
      inspection_items: [scratch, bubble, contamination, offset]
      defect_thresholds:
        scratch_size_um: 50
        bubble_size_um: 30
        contamination_size_um: 20
        offset_um: 100
    
    - recipe_id: RCP-AOI-LED-5050
      product_type: LED-5050
      inspection_items: [scratch, bubble, contamination, offset]
      defect_thresholds:
        scratch_size_um: 80
        bubble_size_um: 50
        contamination_size_um: 30
        offset_um: 150

# ========================================
# 品質管理
# ========================================
quality_management:
  image_storage:
    store_all_images: false
    store_defect_images: true
    retention_period_days: 90
    storage_path: /quality/aoi_images/
  
  defect_review:
    manual_review_required: true
    review_sample_rate_percent: 5
    review_all_fails: true

# ========================================
# 元數據
# ========================================
metadata:
  created_date: 2023-04-01
  created_by: engineer@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 1.3
  iadl_version: 3.1
  
  tags:
    - inspection_equipment
    - aoi
    - quality_control
    - critical_equipment
    - ai_enabled
```

---

## 8. X-Ray Inspection (X光檢測)

### 8.1 設備概述

**功能**: 使用 X 光透視技術,檢測 LED 封裝內部的缺陷,如空洞、裂紋、金線斷裂等。

**關鍵參數**:
- 檢測速度: 2,000-3,000 UPH
- 解析度: 1 μm
- 缺陷檢測: 空洞、裂紋、金線斷裂、晶片偏移

### 8.2 IADL 定義

```yaml
# ========================================
# IADL Asset Definition - X-Ray Inspection
# ========================================

asset_id: XRAY-3F-001
asset_type: XRayInspection
manufacturer: Nordson DAGE
model: XD7600NT
serial_number: NORDSON-2023-XRAY-001
description: X 光檢測系統,用於 LED 內部缺陷檢測

# ========================================
# 基本屬性
# ========================================
properties:
  installation_date: 2023-06-15
  warranty_expiry_date: 2028-06-15
  purchase_cost_usd: 580000
  
  location:
    building: Harvatek-Hsinchu
    floor: 3
    room: Inspection-Area
    coordinates:
      x: 18.2
      y: 28.5
      z: 0.0
  
  specifications:
    inspection_speed_uph: 3000
    resolution_um: 1
    magnification_range: [10, 2000]
    x_ray_tube_voltage_kv: 130
    x_ray_tube_power_w: 10
    detector_type: flat_panel
    detector_resolution_mp: 16
    inspection_methods:
      - 2d_xray
      - 3d_ct
      - oblique_xray
    defect_types:
      - void
      - crack
      - wire_break
      - die_shift
      - delamination
    power_consumption_kw: 8.0
    dimensions_mm:
      length: 1400
      width: 1200
      height: 1800
    weight_kg: 1200
    radiation_shielding: lead_lined

# ========================================
# 3D 模型定義
# ========================================
geometry:
  usd_reference: omniverse://harvatek/assets/equipment/xray_nordson_xd7600.usd
  transform:
    translation: [18.2, 28.5, 0.0]
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]
  
  lod_levels:
    - level: high
      usd_reference: omniverse://harvatek/assets/equipment/xray_nordson_xd7600_high.usd
      max_distance: 5.0
    - level: medium
      usd_reference: omniverse://harvatek/assets/equipment/xray_nordson_xd7600_med.usd
      max_distance: 20.0
    - level: low
      usd_reference: omniverse://harvatek/assets/equipment/xray_nordson_xd7600_low.usd
      max_distance: 100.0

# ========================================
# 連接點定義
# ========================================
connection_points:
  - id: power_inlet
    type: electrical
    voltage: 220V
    current: 40A
    phases: 3
    position: [-0.7, -0.6, 0.3]
  
  - id: network_port
    type: network
    protocol: ethernet
    speed_gbps: 10.0
    position: [-0.7, 0.6, 0.3]
  
  - id: cooling_water_inlet
    type: pipe
    pipe_type: cooling_water
    nominal_diameter: DN20
    position: [-0.7, 0.6, 0.5]
  
  - id: cooling_water_outlet
    type: pipe
    pipe_type: cooling_water_return
    nominal_diameter: DN20
    position: [-0.7, 0.6, 0.7]

# ========================================
# 與其他資產的連接
# ========================================
connections:
  - type: powered_by
    target_asset_id: PANEL-3F-A-07
  
  - type: supplied_by
    target_asset_id: COOLING-WATER-SYSTEM
    connection_point: cooling_water_inlet
  
  - type: controlled_by
    target_asset_id: PLC-3F-001
  
  - type: monitored_by
    target_asset_id: SCADA-SYSTEM
  
  - type: inspects_from
    target_asset_id: AOI-3F-001

# ========================================
# OT 數據標籤對應
# ========================================
data_tags:
  # 設備狀態
  - tag_id: XRAY_3F_001.Status
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_1
    data_type: string
    enum_values: [IDLE, RUNNING, ALARM, MAINTENANCE, WARMUP, CALIBRATION]
    update_rate_ms: 1000
  
  # 生產數據
  - tag_id: XRAY_3F_001.UPH
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_10
    data_type: integer
    unit: units/hour
    range: [0, 4000]
    update_rate_ms: 5000
  
  - tag_id: XRAY_3F_001.TotalCount
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_11
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: XRAY_3F_001.PassCount
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_12
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: XRAY_3F_001.FailCount
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_13
    data_type: integer
    unit: units
    update_rate_ms: 1000
  
  - tag_id: XRAY_3F_001.YieldRate
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_14
    data_type: float
    unit: percent
    range: [0, 100]
    update_rate_ms: 5000
  
  # 缺陷統計
  - tag_id: XRAY_3F_001.VoidCount
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_20
    data_type: integer
    unit: units
    update_rate_ms: 10000
  
  - tag_id: XRAY_3F_001.CrackCount
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_21
    data_type: integer
    unit: units
    update_rate_ms: 10000
  
  - tag_id: XRAY_3F_001.WireBreakCount
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_22
    data_type: integer
    unit: units
    update_rate_ms: 10000
  
  - tag_id: XRAY_3F_001.DieShiftCount
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_23
    data_type: integer
    unit: units
    update_rate_ms: 10000
  
  # X 光管參數
  - tag_id: XRAY_3F_001.TubeVoltage
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_30
    data_type: float
    unit: kV
    range: [0, 150]
    update_rate_ms: 5000
  
  - tag_id: XRAY_3F_001.TubeCurrent
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_31
    data_type: float
    unit: uA
    range: [0, 100]
    update_rate_ms: 5000
  
  - tag_id: XRAY_3F_001.TubeTemperature
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    variable_id: SVID_32
    data_type: float
    unit: celsius
    range: [0, 100]
    update_rate_ms: 10000
  
  # 能源數據
  - tag_id: XRAY_3F_001.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.XRAY_3F_001.Power
    data_type: float
    unit: kW
    range: [0, 12]
    update_rate_ms: 5000
  
  # 告警
  - tag_id: XRAY_3F_001.Alarm
    protocol: SECS_GEM
    equipment_id: XRAY-3F-001
    event_id: ALID_ALL
    data_type: string
    update_rate_ms: 500
  
  # 輻射安全
  - tag_id: XRAY_3F_001.RadiationLevel
    protocol: OPC_UA
    server: opc.tcp://safety-monitor:4840
    node_id: ns=2;s=RadiationMonitor.XRAY_3F_001.Level
    data_type: float
    unit: uSv/h
    range: [0, 10]
    update_rate_ms: 1000
    alarm_threshold: 1.0

# ========================================
# 行為模型
# ========================================
behavior_model:
  type: PythonScript
  path: /models/xray_performance.py
  
  inputs:
    - uph
    - yield_rate
    - void_count
    - crack_count
    - wire_break_count
    - tube_voltage
    - tube_current
    - tube_temperature
  
  outputs:
    - oee
    - defect_rate_by_type
    - inspection_reliability_score
    - tube_health_score
    - predicted_tube_replacement_date
  
  update_frequency_hz: 0.1
  
  algorithms:
    - name: defect_root_cause_analysis
      description: 分析缺陷根本原因
      method: machine_learning
      model_type: classification
      features: [defect_type, defect_location, process_parameters]
    
    - name: tube_health_monitoring
      description: 監控 X 光管健康狀態
      method: predictive_maintenance
      features: [tube_voltage, tube_current, tube_temperature, operating_hours]
      prediction_horizon_days: 90

# ========================================
# Omniverse 整合
# ========================================
omniverse:
  scene_path: /World/Harvatek/Floor_3/Inspection_Area/XRAY_3F_001
  
  data_bindings:
    - usd_attribute: /XRAY_3F_001/StatusLight/Color
      data_tag: XRAY_3F_001.Status
      mapping:
        IDLE: [0.5, 0.5, 0.5, 1.0]
        RUNNING: [0.0, 1.0, 0.0, 1.0]
        ALARM: [1.0, 0.0, 0.0, 1.0]
        MAINTENANCE: [1.0, 1.0, 0.0, 1.0]
        WARMUP: [1.0, 0.5, 0.0, 1.0]
        CALIBRATION: [0.0, 0.5, 1.0, 1.0]
    
    - usd_attribute: /XRAY_3F_001/Display/UPH
      data_tag: XRAY_3F_001.UPH
      format: "{value} UPH"
    
    - usd_attribute: /XRAY_3F_001/Display/YieldRate
      data_tag: XRAY_3F_001.YieldRate
      format: "Yield: {value:.1f}%"
    
    - usd_attribute: /XRAY_3F_001/Display/TubeVoltage
      data_tag: XRAY_3F_001.TubeVoltage
      format: "{value:.0f} kV"
    
    - usd_attribute: /XRAY_3F_001/RadiationWarning/Visibility
      data_tag: XRAY_3F_001.Status
      mapping:
        RUNNING: true
        default: false
  
  animations:
    - name: stage_rotation
      usd_prim: /XRAY_3F_001/RotationStage
      type: rotation
      axis: [0, 0, 1]
      speed_source: XRAY_3F_001.UPH
      speed_multiplier: 0.01
      enabled_when: XRAY_3F_001.Status == RUNNING

# ========================================
# 維護資訊
# ========================================
maintenance:
  maintenance_strategy: predictive
  last_pm_date: 2024-09-15
  next_pm_date: 2024-12-15
  pm_interval_days: 90
  mtbf_hours: 8000
  mttr_hours: 4
  
  maintenance_procedures:
    - id: PM-XRAY-001
      name: X 光管檢查
      frequency_days: 30
      estimated_duration_hours: 1.0
      required_skills: [xray_maintenance, radiation_safety]
    
    - id: PM-XRAY-002
      name: 冷卻系統檢查
      frequency_days: 90
      estimated_duration_hours: 2.0
      required_skills: [cooling_system]
    
    - id: PM-XRAY-003
      name: 影像校正
      frequency_days: 90
      estimated_duration_hours: 3.0
      required_skills: [xray_calibration]
    
    - id: PM-XRAY-004
      name: X 光管更換
      frequency_days: 730
      estimated_duration_hours: 8.0
      required_skills: [xray_maintenance, radiation_safety]
      required_parts: [xray_tube]
    
    - id: PM-XRAY-005
      name: 全面保養
      frequency_days: 365
      estimated_duration_hours: 12.0
      required_skills: [xray_maintenance, cooling_system, electrical_maintenance]
  
  spare_parts:
    - part_id: SP-XRAY-001
      name: X 光管
      quantity_on_hand: 1
      reorder_point: 1
      lead_time_days: 60
      unit_cost_usd: 80000
    
    - part_id: SP-XRAY-002
      name: 平板探測器
      quantity_on_hand: 1
      reorder_point: 0
      lead_time_days: 90
      unit_cost_usd: 120000
    
    - part_id: SP-XRAY-003
      name: 冷卻液
      quantity_on_hand: 20
      reorder_point: 10
      lead_time_days: 7
      unit_cost_usd: 50

# ========================================
# 安全資訊
# ========================================
safety:
  safety_rating: CE_IEC_60601
  radiation_license_required: true
  radiation_license_number: RAD-2023-001
  radiation_license_expiry: 2026-06-15
  
  radiation_safety:
    shielding_material: lead
    shielding_thickness_mm: 3
    maximum_radiation_outside_uSv_h: 1.0
    radiation_monitoring_required: true
    radiation_badge_required: true
  
  hazards:
    - type: radiation
      severity: high
      mitigation: Lead shielding, interlocks, radiation monitoring
    
    - type: electrical
      severity: high
      mitigation: Lockout/tagout procedures
  
  emergency_procedures:
    - type: radiation_alarm
      procedure: Evacuate area, notify radiation safety officer
    
    - type: equipment_malfunction
      procedure: Emergency stop, lockout power, notify supervisor
  
  training_requirements:
    - training: radiation_safety
      frequency_years: 1
      certification_required: true
    
    - training: equipment_operation
      frequency_years: 2
      certification_required: true

# ========================================
# MES 整合
# ========================================
mes_integration:
  equipment_id: XRAY-3F-001
  work_center: INSPECTION-AREA
  resource_type: XRAY_INSPECTION
  capacity_uph: 3000
  
  recipes:
    - recipe_id: RCP-XRAY-LED-HIGH-POWER
      product_type: LED-HIGH-POWER
      inspection_items: [void, crack, wire_break, die_shift]
      tube_voltage_kv: 100
      tube_current_ua: 50
      magnification: 50
      defect_thresholds:
        void_size_um: 50
        crack_length_um: 100
        wire_break_detection: true
        die_shift_um: 20
    
    - recipe_id: RCP-XRAY-MINI-LED
      product_type: MINI-LED
      inspection_items: [void, crack, die_shift]
      tube_voltage_kv: 80
      tube_current_ua: 40
      magnification: 100
      defect_thresholds:
        void_size_um: 20
        crack_length_um: 50
        die_shift_um: 10

# ========================================
# 品質管理
# ========================================
quality_management:
  image_storage:
    store_all_images: false
    store_defect_images: true
    retention_period_days: 365
    storage_path: /quality/xray_images/
  
  defect_review:
    manual_review_required: true
    review_sample_rate_percent: 10
    review_all_fails: true
  
  traceability:
    lot_tracking: true
    serial_tracking: true
    image_linked_to_serial: true

# ========================================
# 元數據
# ========================================
metadata:
  created_date: 2023-06-01
  created_by: engineer@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 1.2
  iadl_version: 3.1
  
  tags:
    - inspection_equipment
    - xray
    - quality_control
    - critical_equipment
    - radiation_equipment
    - ai_enabled
```

---

## 總結

本文件提供了 Harvatek LED 封裝廠主要生產設備的完整 IADL V3.1 定義,包括:

1. **Die Sorter (晶片分選機)** - 晶片分選和測試
2. **Die Bonder (固晶機)** - 晶片貼裝
3. **Wire Bonder (打線機)** - 金線連接
4. **Dispenser (點膠機)** - 環氧樹脂封裝
5. **Tester (測試機)** - 光電特性測試
6. **Taping Machine (編帶包裝機)** - 自動編帶包裝
7. **AOI System (自動光學檢測)** - 外觀缺陷檢測
8. **X-Ray Inspection (X光檢測)** - 內部缺陷檢測

每個設備定義包含:
- 基本屬性和規格
- 3D 模型整合 (OpenUSD)
- 連接點和資產關係
- OT 數據標籤對應 (SECS/GEM, OPC UA)
- 行為模型和預測性維護
- Omniverse 視覺化整合
- 維護資訊和備品管理
- MES/ERP 整合
- 品質管理和校正

這些 IADL 定義可直接用於:
- IADL Editor V3.1 進行資產建模
- NDH V3.1 進行數據整合
- Omniverse 進行 3D 視覺化
- 預測性維護系統
- 數位分身應用

---

**文件版本**: 1.0  
**最後更新**: 2025-10-12  
**作者**: IDTF V3.1 Team

