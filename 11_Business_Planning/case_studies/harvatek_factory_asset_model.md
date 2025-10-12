# Harvatek LED 封裝廠完整資產模型

**基於 IDTF V3.1 和 IADL V3.1**

**版本**: 1.0  
**日期**: 2025-10-12  
**作者**: IDTF V3.1 Team

---

## 目錄

1. [資產架構總覽](#1-資產架構總覽)
2. [建築結構資產](#2-建築結構資產)
3. [生產設備資產](#3-生產設備資產)
4. [設施系統資產](#4-設施系統資產)
5. [感測器網路資產](#5-感測器網路資產)
6. [資產關係圖](#6-資產關係圖)
7. [資產統計](#7-資產統計)

---

## 1. 資產架構總覽

### 1.1 資產分類架構

```
Harvatek-Hsinchu (宏齊科技新竹廠)
│
├── Building (建築結構)
│   ├── B1 (地下 1 樓)
│   ├── 1F (1 樓)
│   ├── 2F (2 樓) - 主要生產區
│   ├── 3F (3 樓) - 主要生產區
│   ├── 4F (4 樓) - Mini/Micro LED 產線
│   └── 5F (5 樓)
│
├── Production Equipment (生產設備) - 340 台
│   ├── Die Sorter (晶片分選機) - 20 台
│   ├── Die Bonder (固晶機) - 50 台
│   ├── Wire Bonder (打線機) - 80 台
│   ├── Dispenser (點膠機) - 40 台
│   ├── Tester (測試機) - 100 台
│   ├── Taping Machine (編帶包裝機) - 30 台
│   └── Inspection (檢測設備) - 20 台
│       ├── AOI System - 15 台
│       └── X-Ray Inspection - 5 台
│
├── Facility Systems (設施系統) - 160 台
│   ├── HVAC (空調系統)
│   │   ├── Chiller (冰水主機) - 10 台
│   │   ├── AHU (空調箱) - 40 台
│   │   ├── FFU (風機過濾單元) - 1,000+ 台
│   │   └── Exhaust Fan (排風機) - 20 台
│   ├── Pure Water System (純水系統) - 5 台
│   ├── Nitrogen System (氮氣系統) - 3 台
│   ├── CDA System (壓縮空氣系統) - 8 台
│   ├── Vacuum System (真空系統) - 4 台
│   ├── Power System (電力系統)
│   │   ├── Transformer (變壓器) - 5 台
│   │   ├── Switchgear (配電盤) - 30 台
│   │   ├── UPS (不斷電系統) - 5 台
│   │   └── Generator (發電機) - 2 台
│   ├── Scrubber (廢氣處理) - 10 台
│   └── Wastewater Treatment (廢水處理) - 2 套
│
└── Sensor Network (感測器網路) - 1,500+ 點
    ├── Environmental Sensors (環境感測器)
    │   ├── Temperature (溫度) - 400 點
    │   ├── Humidity (濕度) - 400 點
    │   ├── Differential Pressure (壓差) - 200 點
    │   └── Particle Counter (潔淨度) - 100 點
    ├── Energy Sensors (能源感測器)
    │   ├── Power Meter (電力) - 200 點
    │   ├── Water Meter (水) - 50 點
    │   └── Gas Meter (氣體) - 50 點
    ├── Equipment Sensors (設備感測器)
    │   └── Vibration, Temperature, Current (振動、溫度、電流) - 100 點
    └── Safety Sensors (安全感測器)
        └── Smoke, Gas, Access Control (煙霧、氣體、門禁) - 100 點
```

### 1.2 資產命名規則

**格式**: `{AssetType}-{Floor}-{Area}-{Number}`

**範例**:
- `DS-2F-001`: Die Sorter, 2樓, 編號 001
- `WB-2F-A-001`: Wire Bonder, 2樓, A區, 編號 001
- `AHU-2F-A-01`: AHU, 2樓, A區, 編號 01
- `PANEL-2F-A-03`: 配電盤, 2樓, A區, 編號 03

---

## 2. 建築結構資產

### 2.1 建築物主體

```yaml
# ========================================
# IADL Asset Definition - Building
# ========================================

asset_id: BUILDING-HARVATEK-HSINCHU
asset_type: Building
description: 宏齊科技新竹科學園區 LED 封裝廠房

properties:
  address: 新竹縣寶山鄉創新一路 7 號
  completion_date: 2010-06-01
  total_floor_area_sqm: 15000
  floors:
    - id: B1
      name: 地下 1 樓
      floor_area_sqm: 2000
      ceiling_height_m: 3.5
      usage: [warehouse, mechanical_room]
    
    - id: 1F
      name: 1 樓
      floor_area_sqm: 3000
      ceiling_height_m: 4.0
      usage: [office, shipping_receiving, facility_room]
    
    - id: 2F
      name: 2 樓
      floor_area_sqm: 3000
      ceiling_height_m: 4.5
      usage: [production, cleanroom]
      cleanroom_class: ISO_7  # Class 10,000
    
    - id: 3F
      name: 3 樓
      floor_area_sqm: 3000
      ceiling_height_m: 4.5
      usage: [production, cleanroom]
      cleanroom_class: ISO_7  # Class 10,000
    
    - id: 4F
      name: 4 樓
      floor_area_sqm: 2500
      ceiling_height_m: 4.5
      usage: [production, cleanroom]
      cleanroom_class: ISO_6  # Class 1,000 (Mini/Micro LED)
    
    - id: 5F
      name: 5 樓
      floor_area_sqm: 1500
      ceiling_height_m: 3.5
      usage: [rd_lab, office]

# 3D 模型
geometry:
  usd_reference: omniverse://harvatek/assets/building/harvatek_hsinchu_building.usd
  transform:
    translation: [0.0, 0.0, 0.0]
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]
  
  lod_levels:
    - level: exterior
      usd_reference: omniverse://harvatek/assets/building/harvatek_hsinchu_exterior.usd
      description: 建築外觀
    
    - level: structure
      usd_reference: omniverse://harvatek/assets/building/harvatek_hsinchu_structure.usd
      description: 建築結構 (柱、梁、樓板)
    
    - level: interior
      usd_reference: omniverse://harvatek/assets/building/harvatek_hsinchu_interior.usd
      description: 室內隔間、門窗

# Omniverse 整合
omniverse:
  scene_path: /World/Harvatek/Building
  
  layers:
    - layer_name: Building_Exterior
      usd_reference: omniverse://harvatek/assets/building/harvatek_hsinchu_exterior.usd
      visible: true
    
    - layer_name: Building_Structure
      usd_reference: omniverse://harvatek/assets/building/harvatek_hsinchu_structure.usd
      visible: true
    
    - layer_name: Building_Interior
      usd_reference: omniverse://harvatek/assets/building/harvatek_hsinchu_interior.usd
      visible: true
    
    - layer_name: MEP_Systems
      usd_reference: omniverse://harvatek/assets/mep/harvatek_hsinchu_mep.usd
      visible: true
    
    - layer_name: Equipment
      usd_reference: omniverse://harvatek/assets/equipment/harvatek_hsinchu_equipment.usd
      visible: true

metadata:
  created_date: 2010-01-01
  created_by: architect@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 3.0
  iadl_version: 3.1
```

### 2.2 樓層和區域定義

#### 2.2.1 2樓 (2F) - 主要生產區

```yaml
# ========================================
# Floor 2F - Production Area
# ========================================

asset_id: FLOOR-2F
asset_type: Floor
parent_asset_id: BUILDING-HARVATEK-HSINCHU

properties:
  floor_number: 2
  floor_area_sqm: 3000
  ceiling_height_m: 4.5
  cleanroom_class: ISO_7
  
  areas:
    # 晶片分選區
    - id: DIE-SORTING-AREA
      name: 晶片分選區
      area_sqm: 400
      equipment_count: 20
      equipment_types: [DieSorter]
      temperature_setpoint_c: 23
      humidity_setpoint_rh: 50
      pressure_differential_pa: 10
    
    # 固晶區 A
    - id: DIE-BONDING-AREA-A
      name: 固晶區 A
      area_sqm: 500
      equipment_count: 25
      equipment_types: [DieBonder]
      temperature_setpoint_c: 23
      humidity_setpoint_rh: 50
      pressure_differential_pa: 10
    
    # 固晶區 B
    - id: DIE-BONDING-AREA-B
      name: 固晶區 B
      area_sqm: 500
      equipment_count: 25
      equipment_types: [DieBonder]
      temperature_setpoint_c: 23
      humidity_setpoint_rh: 50
      pressure_differential_pa: 10
    
    # 打線區 A
    - id: WIRE-BONDING-AREA-A
      name: 打線區 A
      area_sqm: 600
      equipment_count: 40
      equipment_types: [WireBonder]
      temperature_setpoint_c: 23
      humidity_setpoint_rh: 50
      pressure_differential_pa: 10
    
    # 打線區 B
    - id: WIRE-BONDING-AREA-B
      name: 打線區 B
      area_sqm: 600
      equipment_count: 40
      equipment_types: [WireBonder]
      temperature_setpoint_c: 23
      humidity_setpoint_rh: 50
      pressure_differential_pa: 10
    
    # 設施機房
    - id: AHU-ROOM-A
      name: 空調機房 A
      area_sqm: 200
      equipment_count: 10
      equipment_types: [AHU, Chiller]
    
    - id: AHU-ROOM-B
      name: 空調機房 B
      area_sqm: 200
      equipment_count: 10
      equipment_types: [AHU, Chiller]

# 3D 模型
geometry:
  usd_reference: omniverse://harvatek/assets/building/floor_2f.usd
  transform:
    translation: [0.0, 0.0, 4.0]  # 2樓高度
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]

# Omniverse 整合
omniverse:
  scene_path: /World/Harvatek/Floor_2
  
  visualization:
    - type: heatmap
      parameter: temperature
      data_source: environmental_sensors
      color_scale:
        min: 20
        max: 26
        min_color: [0.0, 0.0, 1.0, 0.5]
        max_color: [1.0, 0.0, 0.0, 0.5]
    
    - type: heatmap
      parameter: humidity
      data_source: environmental_sensors
      color_scale:
        min: 40
        max: 60
        min_color: [1.0, 1.0, 0.0, 0.5]
        max_color: [0.0, 1.0, 1.0, 0.5]
    
    - type: airflow
      data_source: hvac_system
      particle_system: true
      particle_color: [0.8, 0.8, 1.0, 0.3]

metadata:
  created_date: 2010-01-01
  created_by: facility@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 2.5
  iadl_version: 3.1
```

#### 2.2.2 3樓 (3F) - 主要生產區

```yaml
# ========================================
# Floor 3F - Production Area
# ========================================

asset_id: FLOOR-3F
asset_type: Floor
parent_asset_id: BUILDING-HARVATEK-HSINCHU

properties:
  floor_number: 3
  floor_area_sqm: 3000
  ceiling_height_m: 4.5
  cleanroom_class: ISO_7
  
  areas:
    # 封膠區
    - id: ENCAPSULATION-AREA
      name: 封膠區
      area_sqm: 600
      equipment_count: 40
      equipment_types: [Dispenser, CuringOven]
      temperature_setpoint_c: 23
      humidity_setpoint_rh: 50
      pressure_differential_pa: 10
    
    # 測試區
    - id: TESTING-AREA
      name: 測試區
      area_sqm: 1000
      equipment_count: 100
      equipment_types: [Tester]
      temperature_setpoint_c: 23
      humidity_setpoint_rh: 50
      pressure_differential_pa: 10
    
    # 包裝區
    - id: PACKAGING-AREA
      name: 包裝區
      area_sqm: 500
      equipment_count: 30
      equipment_types: [TapingMachine]
      temperature_setpoint_c: 23
      humidity_setpoint_rh: 50
      pressure_differential_pa: 5
    
    # 檢測區
    - id: INSPECTION-AREA
      name: 檢測區
      area_sqm: 400
      equipment_count: 20
      equipment_types: [AOISystem, XRayInspection]
      temperature_setpoint_c: 23
      humidity_setpoint_rh: 50
      pressure_differential_pa: 10
    
    # 設施機房
    - id: AHU-ROOM-C
      name: 空調機房 C
      area_sqm: 250
      equipment_count: 15
      equipment_types: [AHU, Chiller]
    
    - id: AHU-ROOM-D
      name: 空調機房 D
      area_sqm: 250
      equipment_count: 15
      equipment_types: [AHU, Chiller]

# 3D 模型
geometry:
  usd_reference: omniverse://harvatek/assets/building/floor_3f.usd
  transform:
    translation: [0.0, 0.0, 8.5]  # 3樓高度
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]

metadata:
  created_date: 2010-01-01
  created_by: facility@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 2.5
  iadl_version: 3.1
```

---

## 3. 生產設備資產

### 3.1 設備清單總覽

| 設備類型 | 數量 | 樓層 | 平均 UPH | 總產能 UPH |
|---------|------|------|----------|-----------|
| Die Sorter | 20 | 2F | 12,000 | 240,000 |
| Die Bonder | 50 | 2F | 8,000 | 400,000 |
| Wire Bonder | 80 | 2F | 10,000 | 800,000 |
| Dispenser | 40 | 3F | 6,000 | 240,000 |
| Tester | 100 | 3F | 15,000 | 1,500,000 |
| Taping Machine | 30 | 3F | 12,000 | 360,000 |
| AOI System | 15 | 3F | 8,000 | 120,000 |
| X-Ray Inspection | 5 | 3F | 3,000 | 15,000 |
| **總計** | **340** | - | - | - |

### 3.2 設備佈局

#### 3.2.1 2樓設備佈局

```yaml
# ========================================
# Equipment Layout - Floor 2F
# ========================================

floor: 2F
total_equipment: 170

equipment_groups:
  # 晶片分選機組
  - group_id: DS-GROUP-2F
    equipment_type: DieSorter
    area: DIE-SORTING-AREA
    equipment_list:
      - asset_id: DS-2F-001
        position: {x: 10.5, y: 8.3, z: 0.0}
        rotation: 90
      - asset_id: DS-2F-002
        position: {x: 10.5, y: 12.3, z: 0.0}
        rotation: 90
      # ... (共 20 台)
  
  # 固晶機組 A
  - group_id: DB-GROUP-2F-A
    equipment_type: DieBonder
    area: DIE-BONDING-AREA-A
    equipment_list:
      - asset_id: DB-2F-A-001
        position: {x: 15.2, y: 8.5, z: 0.0}
        rotation: 0
      - asset_id: DB-2F-A-002
        position: {x: 15.2, y: 11.5, z: 0.0}
        rotation: 0
      # ... (共 25 台)
  
  # 固晶機組 B
  - group_id: DB-GROUP-2F-B
    equipment_type: DieBonder
    area: DIE-BONDING-AREA-B
    equipment_list:
      - asset_id: DB-2F-B-001
        position: {x: 20.5, y: 8.5, z: 0.0}
        rotation: 0
      # ... (共 25 台)
  
  # 打線機組 A
  - group_id: WB-GROUP-2F-A
    equipment_type: WireBonder
    area: WIRE-BONDING-AREA-A
    equipment_list:
      - asset_id: WB-2F-A-001
        position: {x: 25.3, y: 15.7, z: 0.0}
        rotation: 180
      - asset_id: WB-2F-A-002
        position: {x: 25.3, y: 18.7, z: 0.0}
        rotation: 180
      # ... (共 40 台)
  
  # 打線機組 B
  - group_id: WB-GROUP-2F-B
    equipment_type: WireBonder
    area: WIRE-BONDING-AREA-B
    equipment_list:
      - asset_id: WB-2F-B-001
        position: {x: 30.5, y: 15.7, z: 0.0}
        rotation: 180
      # ... (共 40 台)
```

#### 3.2.2 3樓設備佈局

```yaml
# ========================================
# Equipment Layout - Floor 3F
# ========================================

floor: 3F
total_equipment: 185

equipment_groups:
  # 點膠機組
  - group_id: DISP-GROUP-3F
    equipment_type: Dispenser
    area: ENCAPSULATION-AREA
    equipment_list:
      - asset_id: DISP-3F-001
        position: {x: 12.5, y: 10.2, z: 0.0}
        rotation: 45
      - asset_id: DISP-3F-002
        position: {x: 12.5, y: 13.2, z: 0.0}
        rotation: 45
      # ... (共 40 台)
  
  # 測試機組
  - group_id: TESTER-GROUP-3F
    equipment_type: Tester
    area: TESTING-AREA
    equipment_list:
      - asset_id: TESTER-3F-001
        position: {x: 22.8, y: 18.5, z: 0.0}
        rotation: 270
      - asset_id: TESTER-3F-002
        position: {x: 22.8, y: 21.5, z: 0.0}
        rotation: 270
      # ... (共 100 台)
  
  # 編帶包裝機組
  - group_id: TAPING-GROUP-3F
    equipment_type: TapingMachine
    area: PACKAGING-AREA
    equipment_list:
      - asset_id: TAPING-3F-001
        position: {x: 28.5, y: 22.3, z: 0.0}
        rotation: 90
      - asset_id: TAPING-3F-002
        position: {x: 28.5, y: 25.3, z: 0.0}
        rotation: 90
      # ... (共 30 台)
  
  # AOI 檢測機組
  - group_id: AOI-GROUP-3F
    equipment_type: AOISystem
    area: INSPECTION-AREA
    equipment_list:
      - asset_id: AOI-3F-001
        position: {x: 18.2, y: 25.8, z: 0.0}
        rotation: 180
      - asset_id: AOI-3F-002
        position: {x: 18.2, y: 28.8, z: 0.0}
        rotation: 180
      # ... (共 15 台)
  
  # X-Ray 檢測機組
  - group_id: XRAY-GROUP-3F
    equipment_type: XRayInspection
    area: INSPECTION-AREA
    equipment_list:
      - asset_id: XRAY-3F-001
        position: {x: 18.2, y: 32.5, z: 0.0}
        rotation: 0
      - asset_id: XRAY-3F-002
        position: {x: 18.2, y: 35.5, z: 0.0}
        rotation: 0
      # ... (共 5 台)
```

### 3.3 生產線配置

```yaml
# ========================================
# Production Line Configuration
# ========================================

production_lines:
  # 標準 LED 產線 (LED-3528, LED-5050)
  - line_id: LINE-STD-01
    name: 標準 LED 產線 1
    product_types: [LED-3528, LED-5050]
    capacity_uph: 8000
    
    equipment_sequence:
      - step: 1
        process: die_sorting
        equipment: [DS-2F-001, DS-2F-002]
      
      - step: 2
        process: die_bonding
        equipment: [DB-2F-A-001, DB-2F-A-002, DB-2F-A-003]
      
      - step: 3
        process: wire_bonding
        equipment: [WB-2F-A-001, WB-2F-A-002, WB-2F-A-003, WB-2F-A-004]
      
      - step: 4
        process: encapsulation
        equipment: [DISP-3F-001, DISP-3F-002]
      
      - step: 5
        process: curing
        equipment: [CURING-OVEN-3F-001]
      
      - step: 6
        process: testing
        equipment: [TESTER-3F-001, TESTER-3F-002, TESTER-3F-003]
      
      - step: 7
        process: inspection
        equipment: [AOI-3F-001]
      
      - step: 8
        process: taping
        equipment: [TAPING-3F-001, TAPING-3F-002]
  
  # 高功率 LED 產線
  - line_id: LINE-HP-01
    name: 高功率 LED 產線 1
    product_types: [LED-HIGH-POWER]
    capacity_uph: 5000
    
    equipment_sequence:
      - step: 1
        process: die_sorting
        equipment: [DS-2F-003]
      
      - step: 2
        process: die_bonding
        equipment: [DB-2F-A-004, DB-2F-A-005]
      
      - step: 3
        process: wire_bonding
        equipment: [WB-2F-A-005, WB-2F-A-006]
      
      - step: 4
        process: encapsulation
        equipment: [DISP-3F-003]
      
      - step: 5
        process: curing
        equipment: [CURING-OVEN-3F-002]
      
      - step: 6
        process: testing
        equipment: [TESTER-3F-004, TESTER-3F-005]
      
      - step: 7
        process: xray_inspection
        equipment: [XRAY-3F-001]
      
      - step: 8
        process: taping
        equipment: [TAPING-3F-003]
  
  # Mini LED 產線 (4樓)
  - line_id: LINE-MINI-01
    name: Mini LED 產線 1
    product_types: [MINI-LED]
    capacity_uph: 3000
    floor: 4F
    
    equipment_sequence:
      - step: 1
        process: die_sorting
        equipment: [DS-4F-001]
      
      - step: 2
        process: mass_transfer
        equipment: [MT-4F-001]
      
      - step: 3
        process: inspection
        equipment: [AOI-4F-001, XRAY-4F-001]
      
      - step: 4
        process: testing
        equipment: [TESTER-4F-001]
      
      - step: 5
        process: taping
        equipment: [TAPING-4F-001]
```

---

## 4. 設施系統資產

### 4.1 HVAC 系統

#### 4.1.1 冰水主機

```yaml
# ========================================
# Chiller System
# ========================================

asset_id: CHILLER-01
asset_type: Chiller
manufacturer: TRANE
model: CVHE-500
description: 離心式冰水主機

properties:
  installation_date: 2019-06-01
  location:
    building: Harvatek-Hsinchu
    floor: B1
    room: Chiller-Room
    coordinates: {x: 5.0, y: 10.0, z: -3.5}
  
  specifications:
    cooling_capacity_rt: 500  # Refrigeration Tons
    cooling_capacity_kw: 1758
    cop: 6.5  # Coefficient of Performance
    refrigerant: R134a
    chilled_water_flow_lpm: 1500
    chilled_water_temp_supply_c: 7
    chilled_water_temp_return_c: 12
    cooling_water_flow_lpm: 1800
    power_consumption_kw: 270
    dimensions_mm: {length: 4000, width: 2000, height: 2500}
    weight_kg: 8000

# 連接點
connection_points:
  - id: chw_supply
    type: pipe
    pipe_type: chilled_water
    nominal_diameter: DN200
    position: [2.0, 0.0, 1.0]
    usd_prim_path: /CHILLER_01/Piping/CHW_Supply
  
  - id: chw_return
    type: pipe
    pipe_type: chilled_water_return
    nominal_diameter: DN200
    position: [2.0, 0.0, 1.5]
    usd_prim_path: /CHILLER_01/Piping/CHW_Return
  
  - id: cw_supply
    type: pipe
    pipe_type: cooling_water
    nominal_diameter: DN250
    position: [-2.0, 0.0, 1.0]
    usd_prim_path: /CHILLER_01/Piping/CW_Supply
  
  - id: cw_return
    type: pipe
    pipe_type: cooling_water_return
    nominal_diameter: DN250
    position: [-2.0, 0.0, 1.5]
    usd_prim_path: /CHILLER_01/Piping/CW_Return
  
  - id: power_inlet
    type: electrical
    voltage: 380V
    current: 500A
    phases: 3
    position: [-2.0, -1.0, 0.5]

# 與其他資產的連接
connections:
  - type: supplies_to
    target_asset_id: CHW-PUMP-01
    connection_point: chw_supply
  
  - type: supplied_by
    target_asset_id: CHW-PUMP-02
    connection_point: chw_return
  
  - type: supplied_by
    target_asset_id: COOLING-TOWER-01
    connection_point: cw_supply
  
  - type: supplies_to
    target_asset_id: COOLING-TOWER-01
    connection_point: cw_return
  
  - type: powered_by
    target_asset_id: PANEL-B1-FACILITY-01
  
  - type: controlled_by
    target_asset_id: BMS-SYSTEM

# OT 數據標籤
data_tags:
  - tag_id: CHILLER_01.Status
    protocol: BACnet
    device_id: 201
    object_type: BINARY_VALUE
    object_instance: 1
    data_type: boolean
    update_rate_ms: 5000
  
  - tag_id: CHILLER_01.CoolingLoad
    protocol: BACnet
    device_id: 201
    object_type: ANALOG_INPUT
    object_instance: 1
    data_type: float
    unit: RT
    range: [0, 550]
    update_rate_ms: 10000
  
  - tag_id: CHILLER_01.CHW_SupplyTemp
    protocol: BACnet
    device_id: 201
    object_type: ANALOG_INPUT
    object_instance: 2
    data_type: float
    unit: celsius
    range: [5, 15]
    update_rate_ms: 10000
  
  - tag_id: CHILLER_01.CHW_ReturnTemp
    protocol: BACnet
    device_id: 201
    object_type: ANALOG_INPUT
    object_instance: 3
    data_type: float
    unit: celsius
    range: [10, 20]
    update_rate_ms: 10000
  
  - tag_id: CHILLER_01.COP
    protocol: BACnet
    device_id: 201
    object_type: ANALOG_INPUT
    object_instance: 4
    data_type: float
    unit: dimensionless
    range: [4, 8]
    update_rate_ms: 10000
  
  - tag_id: CHILLER_01.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.CHILLER_01.Power
    data_type: float
    unit: kW
    range: [0, 350]
    update_rate_ms: 5000

# 3D 模型
geometry:
  usd_reference: omniverse://harvatek/assets/facility/chiller_trane_cvhe500.usd
  transform:
    translation: [5.0, 10.0, -3.5]
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]

# Omniverse 整合
omniverse:
  scene_path: /World/Harvatek/Floor_B1/Chiller_Room/CHILLER_01
  
  data_bindings:
    - usd_attribute: /CHILLER_01/StatusLight/Color
      data_tag: CHILLER_01.Status
      mapping:
        true: [0.0, 1.0, 0.0, 1.0]
        false: [0.5, 0.5, 0.5, 1.0]
    
    - usd_attribute: /CHILLER_01/Display/CoolingLoad
      data_tag: CHILLER_01.CoolingLoad
      format: "{value:.0f} RT"
    
    - usd_attribute: /CHILLER_01/Display/COP
      data_tag: CHILLER_01.COP
      format: "COP: {value:.2f}"

metadata:
  created_date: 2019-05-15
  created_by: facility@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 2.3
  iadl_version: 3.1
  
  tags:
    - facility_equipment
    - hvac
    - chiller
    - critical_facility
    - energy_intensive
```

### 4.2 純水系統

```yaml
# ========================================
# DI Water System
# ========================================

asset_id: DI-WATER-SYSTEM-MAIN
asset_type: DIWaterSystem
manufacturer: Millipore
model: Elix-Essential-150
description: 純水製造系統

properties:
  installation_date: 2020-03-01
  location:
    building: Harvatek-Hsinchu
    floor: B1
    room: Utility-Room
    coordinates: {x: 15.0, y: 10.0, z: -3.5}
  
  specifications:
    production_capacity_lph: 150  # Liters per hour
    water_quality: Type_II  # ASTM D1193
    resistivity_mohm_cm: 10
    toc_ppb: 30  # Total Organic Carbon
    bacteria_cfu_ml: 10  # Colony Forming Units
    power_consumption_kw: 3.5
    dimensions_mm: {length: 1200, width: 800, height: 1800}
    weight_kg: 400

# 連接點
connection_points:
  - id: raw_water_inlet
    type: pipe
    pipe_type: raw_water
    nominal_diameter: DN25
    position: [-0.6, -0.4, 0.3]
  
  - id: di_water_outlet
    type: pipe
    pipe_type: di_water
    nominal_diameter: DN25
    position: [0.6, 0.4, 0.5]
  
  - id: drain
    type: pipe
    pipe_type: drain
    nominal_diameter: DN40
    position: [-0.6, 0.4, 0.1]
  
  - id: power_inlet
    type: electrical
    voltage: 220V
    current: 20A
    position: [-0.6, -0.4, 0.5]

# 與其他資產的連接
connections:
  - type: supplied_by
    target_asset_id: RAW-WATER-SYSTEM
    connection_point: raw_water_inlet
  
  - type: supplies_to
    target_asset_id: DI-WATER-DISTRIBUTION-2F
    connection_point: di_water_outlet
  
  - type: supplies_to
    target_asset_id: DI-WATER-DISTRIBUTION-3F
    connection_point: di_water_outlet
  
  - type: powered_by
    target_asset_id: PANEL-B1-FACILITY-02
  
  - type: controlled_by
    target_asset_id: BMS-SYSTEM

# OT 數據標籤
data_tags:
  - tag_id: DI_WATER_MAIN.Status
    protocol: Modbus_TCP
    ip_address: 192.168.1.101
    register: 40001
    data_type: boolean
    update_rate_ms: 5000
  
  - tag_id: DI_WATER_MAIN.FlowRate
    protocol: Modbus_TCP
    ip_address: 192.168.1.101
    register: 40002
    data_type: float
    unit: lpm
    range: [0, 150]
    update_rate_ms: 10000
  
  - tag_id: DI_WATER_MAIN.Resistivity
    protocol: Modbus_TCP
    ip_address: 192.168.1.101
    register: 40003
    data_type: float
    unit: mohm_cm
    range: [0, 18.2]
    update_rate_ms: 10000
  
  - tag_id: DI_WATER_MAIN.TOC
    protocol: Modbus_TCP
    ip_address: 192.168.1.101
    register: 40004
    data_type: float
    unit: ppb
    range: [0, 100]
    update_rate_ms: 60000
  
  - tag_id: DI_WATER_MAIN.TankLevel
    protocol: Modbus_TCP
    ip_address: 192.168.1.101
    register: 40005
    data_type: float
    unit: percent
    range: [0, 100]
    update_rate_ms: 10000

# 3D 模型
geometry:
  usd_reference: omniverse://harvatek/assets/facility/di_water_millipore_elix150.usd
  transform:
    translation: [15.0, 10.0, -3.5]
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]

metadata:
  created_date: 2020-02-15
  created_by: facility@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 1.8
  iadl_version: 3.1
  
  tags:
    - facility_equipment
    - utility
    - di_water
    - critical_facility
```

---

*文件未完,繼續下一部分...*



### 4.3 氮氣系統

```yaml
# ========================================
# Nitrogen Generation System
# ========================================

asset_id: N2-SYSTEM-MAIN
asset_type: NitrogenGenerator
manufacturer: Parker Hannifin
model: MAXIGAS-2000
description: PSA 氮氣產生系統

properties:
  installation_date: 2020-01-15
  location:
    building: Harvatek-Hsinchu
    floor: B1
    room: Utility-Room
    coordinates: {x: 20.0, y: 10.0, z: -3.5}
  
  specifications:
    generation_capacity_nm3_h: 2000  # Normal cubic meters per hour
    nitrogen_purity_percent: 99.999
    output_pressure_bar: 7.0
    dew_point_c: -40
    power_consumption_kw: 85
    compressed_air_consumption_nm3_h: 2500
    dimensions_mm: {length: 3000, width: 2000, height: 2500}
    weight_kg: 3500

# 連接點
connection_points:
  - id: compressed_air_inlet
    type: pipe
    pipe_type: compressed_air
    nominal_diameter: DN80
    position: [-1.5, -1.0, 1.0]
  
  - id: n2_outlet_main
    type: pipe
    pipe_type: nitrogen
    nominal_diameter: DN50
    position: [1.5, 0.0, 1.5]
  
  - id: power_inlet
    type: electrical
    voltage: 380V
    current: 150A
    phases: 3
    position: [-1.5, -1.0, 0.5]

# 與其他資產的連接
connections:
  - type: supplied_by
    target_asset_id: CDA-SYSTEM-MAIN
    connection_point: compressed_air_inlet
  
  - type: supplies_to
    target_asset_id: N2-DISTRIBUTION-2F
    connection_point: n2_outlet_main
  
  - type: supplies_to
    target_asset_id: N2-DISTRIBUTION-3F
    connection_point: n2_outlet_main
  
  - type: powered_by
    target_asset_id: PANEL-B1-FACILITY-03
  
  - type: controlled_by
    target_asset_id: BMS-SYSTEM

# OT 數據標籤
data_tags:
  - tag_id: N2_SYSTEM_MAIN.Status
    protocol: Modbus_TCP
    ip_address: 192.168.1.102
    register: 40001
    data_type: boolean
    update_rate_ms: 5000
  
  - tag_id: N2_SYSTEM_MAIN.FlowRate
    protocol: Modbus_TCP
    ip_address: 192.168.1.102
    register: 40002
    data_type: float
    unit: nm3/h
    range: [0, 2500]
    update_rate_ms: 10000
  
  - tag_id: N2_SYSTEM_MAIN.Purity
    protocol: Modbus_TCP
    ip_address: 192.168.1.102
    register: 40003
    data_type: float
    unit: percent
    range: [99.9, 100.0]
    update_rate_ms: 60000
  
  - tag_id: N2_SYSTEM_MAIN.OutputPressure
    protocol: Modbus_TCP
    ip_address: 192.168.1.102
    register: 40004
    data_type: float
    unit: bar
    range: [0, 10]
    update_rate_ms: 10000
  
  - tag_id: N2_SYSTEM_MAIN.DewPoint
    protocol: Modbus_TCP
    ip_address: 192.168.1.102
    register: 40005
    data_type: float
    unit: celsius
    range: [-60, 0]
    update_rate_ms: 60000

# 3D 模型
geometry:
  usd_reference: omniverse://harvatek/assets/facility/n2_generator_parker_maxigas2000.usd
  transform:
    translation: [20.0, 10.0, -3.5]
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]

# Omniverse 整合
omniverse:
  scene_path: /World/Harvatek/Floor_B1/Utility_Room/N2_SYSTEM_MAIN
  
  data_bindings:
    - usd_attribute: /N2_SYSTEM_MAIN/StatusLight/Color
      data_tag: N2_SYSTEM_MAIN.Status
      mapping:
        true: [0.0, 1.0, 0.0, 1.0]
        false: [0.5, 0.5, 0.5, 1.0]
    
    - usd_attribute: /N2_SYSTEM_MAIN/Display/FlowRate
      data_tag: N2_SYSTEM_MAIN.FlowRate
      format: "{value:.0f} Nm³/h"
    
    - usd_attribute: /N2_SYSTEM_MAIN/Display/Purity
      data_tag: N2_SYSTEM_MAIN.Purity
      format: "{value:.3f}%"

metadata:
  created_date: 2020-01-01
  created_by: facility@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 1.9
  iadl_version: 3.1
  
  tags:
    - facility_equipment
    - utility
    - nitrogen
    - critical_facility
```

### 4.4 壓縮空氣系統

```yaml
# ========================================
# Compressed Air System
# ========================================

asset_id: CDA-SYSTEM-MAIN
asset_type: AirCompressor
manufacturer: Atlas Copco
model: GA-250-VSD
description: 變頻螺旋式空壓機

properties:
  installation_date: 2019-08-01
  location:
    building: Harvatek-Hsinchu
    floor: B1
    room: Compressor-Room
    coordinates: {x: 25.0, y: 10.0, z: -3.5}
  
  specifications:
    air_flow_capacity_m3_min: 42  # Cubic meters per minute
    working_pressure_bar: 8.0
    motor_power_kw: 250
    drive_type: variable_speed_drive
    cooling_method: air_cooled
    air_quality: ISO_8573-1_Class_1.4.1
    power_consumption_kw: 200  # Average at 75% load
    dimensions_mm: {length: 3500, width: 1800, height: 2200}
    weight_kg: 4500

# 連接點
connection_points:
  - id: air_outlet
    type: pipe
    pipe_type: compressed_air
    nominal_diameter: DN100
    position: [1.75, 0.0, 1.5]
  
  - id: power_inlet
    type: electrical
    voltage: 380V
    current: 450A
    phases: 3
    position: [-1.75, -0.9, 0.5]

# 與其他資產的連接
connections:
  - type: supplies_to
    target_asset_id: AIR-DRYER-01
    connection_point: air_outlet
  
  - type: supplies_to
    target_asset_id: N2-SYSTEM-MAIN
  
  - type: supplies_to
    target_asset_id: CDA-DISTRIBUTION-2F
  
  - type: supplies_to
    target_asset_id: CDA-DISTRIBUTION-3F
  
  - type: powered_by
    target_asset_id: PANEL-B1-FACILITY-04
  
  - type: controlled_by
    target_asset_id: BMS-SYSTEM

# OT 數據標籤
data_tags:
  - tag_id: CDA_SYSTEM_MAIN.Status
    protocol: Modbus_TCP
    ip_address: 192.168.1.103
    register: 40001
    data_type: boolean
    update_rate_ms: 5000
  
  - tag_id: CDA_SYSTEM_MAIN.FlowRate
    protocol: Modbus_TCP
    ip_address: 192.168.1.103
    register: 40002
    data_type: float
    unit: m3/min
    range: [0, 50]
    update_rate_ms: 10000
  
  - tag_id: CDA_SYSTEM_MAIN.OutputPressure
    protocol: Modbus_TCP
    ip_address: 192.168.1.103
    register: 40003
    data_type: float
    unit: bar
    range: [0, 10]
    update_rate_ms: 10000
  
  - tag_id: CDA_SYSTEM_MAIN.MotorLoad
    protocol: Modbus_TCP
    ip_address: 192.168.1.103
    register: 40004
    data_type: float
    unit: percent
    range: [0, 100]
    update_rate_ms: 10000
  
  - tag_id: CDA_SYSTEM_MAIN.PowerConsumption
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.CDA_SYSTEM_MAIN.Power
    data_type: float
    unit: kW
    range: [0, 300]
    update_rate_ms: 5000
  
  - tag_id: CDA_SYSTEM_MAIN.RunningHours
    protocol: Modbus_TCP
    ip_address: 192.168.1.103
    register: 40005
    data_type: integer
    unit: hours
    update_rate_ms: 3600000

# 3D 模型
geometry:
  usd_reference: omniverse://harvatek/assets/facility/compressor_atlas_ga250.usd
  transform:
    translation: [25.0, 10.0, -3.5]
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]

# Omniverse 整合
omniverse:
  scene_path: /World/Harvatek/Floor_B1/Compressor_Room/CDA_SYSTEM_MAIN
  
  data_bindings:
    - usd_attribute: /CDA_SYSTEM_MAIN/StatusLight/Color
      data_tag: CDA_SYSTEM_MAIN.Status
      mapping:
        true: [0.0, 1.0, 0.0, 1.0]
        false: [0.5, 0.5, 0.5, 1.0]
    
    - usd_attribute: /CDA_SYSTEM_MAIN/Display/FlowRate
      data_tag: CDA_SYSTEM_MAIN.FlowRate
      format: "{value:.1f} m³/min"
    
    - usd_attribute: /CDA_SYSTEM_MAIN/Display/MotorLoad
      data_tag: CDA_SYSTEM_MAIN.MotorLoad
      format: "Load: {value:.0f}%"

metadata:
  created_date: 2019-07-15
  created_by: facility@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 2.1
  iadl_version: 3.1
  
  tags:
    - facility_equipment
    - utility
    - compressed_air
    - critical_facility
    - energy_intensive
```

### 4.5 電力系統

#### 4.5.1 變壓器

```yaml
# ========================================
# Power Transformer
# ========================================

asset_id: TRANSFORMER-MAIN-01
asset_type: PowerTransformer
manufacturer: ABB
model: ONAN-2000
description: 油浸式變壓器

properties:
  installation_date: 2010-05-01
  location:
    building: Harvatek-Hsinchu
    floor: B1
    room: Electrical-Room
    coordinates: {x: 30.0, y: 5.0, z: -3.5}
  
  specifications:
    rated_capacity_kva: 2000
    primary_voltage_kv: 22.8
    secondary_voltage_v: 380
    frequency_hz: 60
    cooling_type: ONAN  # Oil Natural Air Natural
    vector_group: Dyn11
    impedance_percent: 6.0
    efficiency_percent: 98.5
    dimensions_mm: {length: 3000, width: 2000, height: 2500}
    weight_kg: 6000
    oil_volume_l: 1200

# 連接點
connection_points:
  - id: primary_input
    type: electrical
    voltage: 22.8kV
    current: 50A
    phases: 3
    position: [-1.5, 0.0, 2.0]
  
  - id: secondary_output
    type: electrical
    voltage: 380V
    current: 3040A
    phases: 3
    position: [1.5, 0.0, 1.5]

# 與其他資產的連接
connections:
  - type: supplied_by
    target_asset_id: UTILITY-GRID
    connection_point: primary_input
  
  - type: supplies_to
    target_asset_id: SWITCHGEAR-MAIN-01
    connection_point: secondary_output
  
  - type: monitored_by
    target_asset_id: BMS-SYSTEM

# OT 數據標籤
data_tags:
  - tag_id: TRANSFORMER_MAIN_01.PrimaryVoltage
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.TRANSFORMER_MAIN_01.PrimaryVoltage
    data_type: float
    unit: kV
    range: [20, 25]
    update_rate_ms: 10000
  
  - tag_id: TRANSFORMER_MAIN_01.SecondaryVoltage
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.TRANSFORMER_MAIN_01.SecondaryVoltage
    data_type: float
    unit: V
    range: [360, 400]
    update_rate_ms: 10000
  
  - tag_id: TRANSFORMER_MAIN_01.LoadCurrent
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.TRANSFORMER_MAIN_01.LoadCurrent
    data_type: float
    unit: A
    range: [0, 3500]
    update_rate_ms: 10000
  
  - tag_id: TRANSFORMER_MAIN_01.LoadPower
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=PowerMeter.TRANSFORMER_MAIN_01.LoadPower
    data_type: float
    unit: kW
    range: [0, 2200]
    update_rate_ms: 10000
  
  - tag_id: TRANSFORMER_MAIN_01.OilTemperature
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=TempSensor.TRANSFORMER_MAIN_01.OilTemp
    data_type: float
    unit: celsius
    range: [0, 120]
    update_rate_ms: 60000
    alarm_threshold: 90
  
  - tag_id: TRANSFORMER_MAIN_01.WindingTemperature
    protocol: OPC_UA
    server: opc.tcp://power-monitor:4840
    node_id: ns=2;s=TempSensor.TRANSFORMER_MAIN_01.WindingTemp
    data_type: float
    unit: celsius
    range: [0, 150]
    update_rate_ms: 60000
    alarm_threshold: 110

# 3D 模型
geometry:
  usd_reference: omniverse://harvatek/assets/facility/transformer_abb_onan2000.usd
  transform:
    translation: [30.0, 5.0, -3.5]
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]

# Omniverse 整合
omniverse:
  scene_path: /World/Harvatek/Floor_B1/Electrical_Room/TRANSFORMER_MAIN_01
  
  data_bindings:
    - usd_attribute: /TRANSFORMER_MAIN_01/Display/LoadPower
      data_tag: TRANSFORMER_MAIN_01.LoadPower
      format: "{value:.0f} kW"
    
    - usd_attribute: /TRANSFORMER_MAIN_01/Display/OilTemp
      data_tag: TRANSFORMER_MAIN_01.OilTemperature
      format: "{value:.0f}°C"
    
    - usd_attribute: /TRANSFORMER_MAIN_01/TemperatureIndicator/Color
      data_tag: TRANSFORMER_MAIN_01.OilTemperature
      mapping_type: gradient
      color_scale:
        - value: 0
          color: [0.0, 1.0, 0.0, 1.0]
        - value: 70
          color: [1.0, 1.0, 0.0, 1.0]
        - value: 90
          color: [1.0, 0.0, 0.0, 1.0]

metadata:
  created_date: 2010-04-15
  created_by: facility@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 2.8
  iadl_version: 3.1
  
  tags:
    - facility_equipment
    - electrical
    - transformer
    - critical_facility
```

#### 4.5.2 UPS 系統

```yaml
# ========================================
# UPS System
# ========================================

asset_id: UPS-MAIN-01
asset_type: UPS
manufacturer: Schneider Electric
model: Galaxy-VX-500
description: 三相在線式 UPS 系統

properties:
  installation_date: 2020-06-01
  location:
    building: Harvatek-Hsinchu
    floor: B1
    room: UPS-Room
    coordinates: {x: 35.0, y: 5.0, z: -3.5}
  
  specifications:
    rated_capacity_kva: 500
    rated_power_kw: 450
    input_voltage_v: 380
    output_voltage_v: 380
    frequency_hz: 60
    topology: online_double_conversion
    efficiency_percent: 96.5  # At 50% load
    battery_type: VRLA
    battery_backup_time_min: 15  # At full load
    battery_count: 40
    dimensions_mm: {length: 2000, width: 1000, height: 2000}
    weight_kg: 1800

# 連接點
connection_points:
  - id: ac_input
    type: electrical
    voltage: 380V
    current: 760A
    phases: 3
    position: [-1.0, -0.5, 1.5]
  
  - id: bypass_input
    type: electrical
    voltage: 380V
    current: 760A
    phases: 3
    position: [-1.0, 0.5, 1.5]
  
  - id: ac_output
    type: electrical
    voltage: 380V
    current: 760A
    phases: 3
    position: [1.0, 0.0, 1.5]

# 與其他資產的連接
connections:
  - type: supplied_by
    target_asset_id: SWITCHGEAR-MAIN-01
    connection_point: ac_input
  
  - type: supplied_by
    target_asset_id: SWITCHGEAR-MAIN-01
    connection_point: bypass_input
  
  - type: supplies_to
    target_asset_id: PANEL-2F-CRITICAL-01
    connection_point: ac_output
  
  - type: supplies_to
    target_asset_id: PANEL-3F-CRITICAL-01
    connection_point: ac_output
  
  - type: monitored_by
    target_asset_id: BMS-SYSTEM

# OT 數據標籤
data_tags:
  - tag_id: UPS_MAIN_01.Status
    protocol: SNMP
    ip_address: 192.168.1.110
    oid: 1.3.6.1.4.1.318.1.1.1.4.1.1.0
    data_type: integer
    enum_values: {2: ONLINE, 3: ON_BATTERY, 4: BYPASS, 5: FAULT}
    update_rate_ms: 5000
  
  - tag_id: UPS_MAIN_01.InputVoltage
    protocol: SNMP
    ip_address: 192.168.1.110
    oid: 1.3.6.1.4.1.318.1.1.1.3.2.1.0
    data_type: float
    unit: V
    range: [0, 500]
    update_rate_ms: 10000
  
  - tag_id: UPS_MAIN_01.OutputVoltage
    protocol: SNMP
    ip_address: 192.168.1.110
    oid: 1.3.6.1.4.1.318.1.1.1.4.2.1.0
    data_type: float
    unit: V
    range: [0, 500]
    update_rate_ms: 10000
  
  - tag_id: UPS_MAIN_01.OutputLoad
    protocol: SNMP
    ip_address: 192.168.1.110
    oid: 1.3.6.1.4.1.318.1.1.1.4.2.3.0
    data_type: float
    unit: percent
    range: [0, 100]
    update_rate_ms: 10000
  
  - tag_id: UPS_MAIN_01.BatteryCapacity
    protocol: SNMP
    ip_address: 192.168.1.110
    oid: 1.3.6.1.4.1.318.1.1.1.2.2.1.0
    data_type: float
    unit: percent
    range: [0, 100]
    update_rate_ms: 60000
  
  - tag_id: UPS_MAIN_01.BatteryTemperature
    protocol: SNMP
    ip_address: 192.168.1.110
    oid: 1.3.6.1.4.1.318.1.1.1.2.2.2.0
    data_type: float
    unit: celsius
    range: [0, 50]
    update_rate_ms: 60000
  
  - tag_id: UPS_MAIN_01.RemainingRuntime
    protocol: SNMP
    ip_address: 192.168.1.110
    oid: 1.3.6.1.4.1.318.1.1.1.2.2.3.0
    data_type: integer
    unit: minutes
    range: [0, 60]
    update_rate_ms: 60000

# 3D 模型
geometry:
  usd_reference: omniverse://harvatek/assets/facility/ups_schneider_galaxy_vx500.usd
  transform:
    translation: [35.0, 5.0, -3.5]
    rotation: [0.0, 0.0, 0.0]
    scale: [1.0, 1.0, 1.0]

# Omniverse 整合
omniverse:
  scene_path: /World/Harvatek/Floor_B1/UPS_Room/UPS_MAIN_01
  
  data_bindings:
    - usd_attribute: /UPS_MAIN_01/StatusLight/Color
      data_tag: UPS_MAIN_01.Status
      mapping:
        ONLINE: [0.0, 1.0, 0.0, 1.0]
        ON_BATTERY: [1.0, 1.0, 0.0, 1.0]
        BYPASS: [1.0, 0.5, 0.0, 1.0]
        FAULT: [1.0, 0.0, 0.0, 1.0]
    
    - usd_attribute: /UPS_MAIN_01/Display/OutputLoad
      data_tag: UPS_MAIN_01.OutputLoad
      format: "Load: {value:.0f}%"
    
    - usd_attribute: /UPS_MAIN_01/Display/BatteryCapacity
      data_tag: UPS_MAIN_01.BatteryCapacity
      format: "Battery: {value:.0f}%"
    
    - usd_attribute: /UPS_MAIN_01/BatteryIndicator/FillLevel
      data_tag: UPS_MAIN_01.BatteryCapacity
      mapping_type: scale
      axis: [1, 1, 1]
      scale_factor: 0.01

metadata:
  created_date: 2020-05-15
  created_by: facility@harvatek.com
  last_modified_date: 2025-10-12
  last_modified_by: michael.lin@harvatek.com
  version: 1.7
  iadl_version: 3.1
  
  tags:
    - facility_equipment
    - electrical
    - ups
    - critical_facility
    - backup_power
```

---

## 5. 感測器網路資產

### 5.1 環境感測器

```yaml
# ========================================
# Environmental Sensor Template
# ========================================

sensor_template:
  type: EnvironmentalSensor
  manufacturer: Vaisala
  
  sensor_types:
    # 溫度感測器
    - sensor_type: TemperatureSensor
      model: HMT330
      measurement_range: [-40, 60]
      accuracy: 0.2
      unit: celsius
      update_rate_ms: 10000
      quantity: 400
      
      locations:
        - floor: 2F
          areas: [DIE-SORTING-AREA, DIE-BONDING-AREA-A, DIE-BONDING-AREA-B, 
                  WIRE-BONDING-AREA-A, WIRE-BONDING-AREA-B]
          sensors_per_area: 40
        
        - floor: 3F
          areas: [ENCAPSULATION-AREA, TESTING-AREA, PACKAGING-AREA, INSPECTION-AREA]
          sensors_per_area: 40
      
      naming_convention: "TEMP-{floor}-{area}-{number:03d}"
      
      iadl_template:
        asset_id: "{sensor_id}"
        asset_type: TemperatureSensor
        properties:
          measurement_range_c: [-40, 60]
          accuracy_c: 0.2
        
        data_tags:
          - tag_id: "{sensor_id}.Temperature"
            protocol: OPC_UA
            server: opc.tcp://env-monitor:4840
            node_id: "ns=2;s=Sensor.{sensor_id}.Temperature"
            data_type: float
            unit: celsius
            update_rate_ms: 10000
        
        omniverse:
          scene_path: "/World/Harvatek/{floor}/{area}/Sensors/{sensor_id}"
          visualization:
            type: heatmap_contribution
            parameter: temperature
    
    # 濕度感測器
    - sensor_type: HumiditySensor
      model: HMT330
      measurement_range: [0, 100]
      accuracy: 1.5
      unit: percent_rh
      update_rate_ms: 10000
      quantity: 400
      
      locations:
        - floor: 2F
          areas: [DIE-SORTING-AREA, DIE-BONDING-AREA-A, DIE-BONDING-AREA-B, 
                  WIRE-BONDING-AREA-A, WIRE-BONDING-AREA-B]
          sensors_per_area: 40
        
        - floor: 3F
          areas: [ENCAPSULATION-AREA, TESTING-AREA, PACKAGING-AREA, INSPECTION-AREA]
          sensors_per_area: 40
      
      naming_convention: "HUMID-{floor}-{area}-{number:03d}"
    
    # 壓差感測器
    - sensor_type: DifferentialPressureSensor
      model: PTB330
      measurement_range: [-500, 500]
      accuracy: 0.5
      unit: pa
      update_rate_ms: 10000
      quantity: 200
      
      locations:
        - floor: 2F
          areas: [DIE-SORTING-AREA, DIE-BONDING-AREA-A, DIE-BONDING-AREA-B, 
                  WIRE-BONDING-AREA-A, WIRE-BONDING-AREA-B]
          sensors_per_area: 20
        
        - floor: 3F
          areas: [ENCAPSULATION-AREA, TESTING-AREA, PACKAGING-AREA, INSPECTION-AREA]
          sensors_per_area: 20
      
      naming_convention: "DP-{floor}-{area}-{number:03d}"
    
    # 潔淨度感測器
    - sensor_type: ParticleCounter
      model: AeroTrak-9510
      measurement_range: [0, 1000000]
      particle_sizes_um: [0.3, 0.5, 1.0, 5.0]
      unit: particles/m3
      update_rate_ms: 60000
      quantity: 100
      
      locations:
        - floor: 2F
          areas: [DIE-SORTING-AREA, DIE-BONDING-AREA-A, DIE-BONDING-AREA-B, 
                  WIRE-BONDING-AREA-A, WIRE-BONDING-AREA-B]
          sensors_per_area: 10
        
        - floor: 3F
          areas: [ENCAPSULATION-AREA, TESTING-AREA, PACKAGING-AREA, INSPECTION-AREA]
          sensors_per_area: 10
      
      naming_convention: "PC-{floor}-{area}-{number:03d}"
```

### 5.2 能源感測器

```yaml
# ========================================
# Energy Sensor Template
# ========================================

sensor_template:
  type: EnergySensor
  
  sensor_types:
    # 電力計
    - sensor_type: PowerMeter
      manufacturer: Schneider Electric
      model: PM5560
      measurement_parameters:
        - voltage
        - current
        - power
        - energy
        - power_factor
      communication: Modbus_TCP
      update_rate_ms: 5000
      quantity: 200
      
      locations:
        - description: 主要配電盤
          quantity: 10
        
        - description: 樓層配電盤
          quantity: 30
        
        - description: 區域配電盤
          quantity: 60
        
        - description: 設備專用配電盤
          quantity: 100
      
      naming_convention: "PM-{location}-{number:03d}"
      
      iadl_template:
        asset_id: "{sensor_id}"
        asset_type: PowerMeter
        
        data_tags:
          - tag_id: "{sensor_id}.Voltage"
            protocol: Modbus_TCP
            data_type: float
            unit: V
          
          - tag_id: "{sensor_id}.Current"
            protocol: Modbus_TCP
            data_type: float
            unit: A
          
          - tag_id: "{sensor_id}.Power"
            protocol: Modbus_TCP
            data_type: float
            unit: kW
          
          - tag_id: "{sensor_id}.Energy"
            protocol: Modbus_TCP
            data_type: float
            unit: kWh
          
          - tag_id: "{sensor_id}.PowerFactor"
            protocol: Modbus_TCP
            data_type: float
            unit: dimensionless
    
    # 水錶
    - sensor_type: WaterMeter
      manufacturer: Kamstrup
      model: MULTICAL-21
      measurement_parameters:
        - flow_rate
        - volume
        - temperature
      communication: M-Bus
      update_rate_ms: 10000
      quantity: 50
      
      locations:
        - description: 純水系統
          quantity: 10
        
        - description: 冷卻水系統
          quantity: 15
        
        - description: 生活用水
          quantity: 15
        
        - description: 廢水處理
          quantity: 10
      
      naming_convention: "WM-{location}-{number:03d}"
    
    # 氣體流量計
    - sensor_type: GasFlowMeter
      manufacturer: Brooks Instrument
      model: SLA5800
      measurement_parameters:
        - flow_rate
        - volume
        - pressure
        - temperature
      communication: Modbus_TCP
      update_rate_ms: 10000
      quantity: 50
      
      locations:
        - description: 氮氣系統
          quantity: 20
        
        - description: 壓縮空氣系統
          quantity: 20
        
        - description: 真空系統
          quantity: 10
      
      naming_convention: "FM-{gas_type}-{location}-{number:03d}"
```

---

## 6. 資產關係圖

### 6.1 生產流程資產關係

```yaml
# ========================================
# Production Flow Asset Relationships
# ========================================

production_flow:
  name: LED 標準生產流程
  
  stages:
    - stage: 1
      name: 晶片分選
      equipment_type: DieSorter
      input_material: LED_WAFER
      output_material: SORTED_LED_CHIP
      
      relationships:
        - type: feeds_to
          target_stage: 2
          material_flow: SORTED_LED_CHIP
    
    - stage: 2
      name: 固晶
      equipment_type: DieBonder
      input_material: SORTED_LED_CHIP
      output_material: DIE_BONDED_UNIT
      
      relationships:
        - type: fed_by
          source_stage: 1
        - type: feeds_to
          target_stage: 3
          material_flow: DIE_BONDED_UNIT
    
    - stage: 3
      name: 打線
      equipment_type: WireBonder
      input_material: DIE_BONDED_UNIT
      output_material: WIRE_BONDED_UNIT
      
      relationships:
        - type: fed_by
          source_stage: 2
        - type: feeds_to
          target_stage: 4
          material_flow: WIRE_BONDED_UNIT
    
    - stage: 4
      name: 封膠
      equipment_type: Dispenser
      input_material: WIRE_BONDED_UNIT
      output_material: ENCAPSULATED_UNIT
      
      relationships:
        - type: fed_by
          source_stage: 3
        - type: feeds_to
          target_stage: 5
          material_flow: ENCAPSULATED_UNIT
    
    - stage: 5
      name: 固化
      equipment_type: CuringOven
      input_material: ENCAPSULATED_UNIT
      output_material: CURED_UNIT
      
      relationships:
        - type: fed_by
          source_stage: 4
        - type: feeds_to
          target_stage: 6
          material_flow: CURED_UNIT
    
    - stage: 6
      name: 測試
      equipment_type: Tester
      input_material: CURED_UNIT
      output_material: TESTED_UNIT
      
      relationships:
        - type: fed_by
          source_stage: 5
        - type: feeds_to
          target_stage: 7
          material_flow: TESTED_UNIT
    
    - stage: 7
      name: 檢測
      equipment_type: [AOISystem, XRayInspection]
      input_material: TESTED_UNIT
      output_material: INSPECTED_UNIT
      
      relationships:
        - type: fed_by
          source_stage: 6
        - type: feeds_to
          target_stage: 8
          material_flow: INSPECTED_UNIT
    
    - stage: 8
      name: 編帶包裝
      equipment_type: TapingMachine
      input_material: INSPECTED_UNIT
      output_material: PACKAGED_LED
      
      relationships:
        - type: fed_by
          source_stage: 7
        - type: feeds_to
          target: WAREHOUSE
          material_flow: PACKAGED_LED
```

### 6.2 設施系統資產關係

```yaml
# ========================================
# Facility System Asset Relationships
# ========================================

facility_systems:
  # 冰水系統
  - system: chilled_water_system
    name: 冰水系統
    
    assets:
      - asset_id: CHILLER-01
        asset_type: Chiller
        role: primary_cooling
        
        connections:
          - type: supplies_to
            target_asset_id: CHW-PUMP-PRIMARY-01
            medium: chilled_water
      
      - asset_id: CHW-PUMP-PRIMARY-01
        asset_type: Pump
        role: primary_circulation
        
        connections:
          - type: supplied_by
            source_asset_id: CHILLER-01
          - type: supplies_to
            target_asset_id: CHW-HEADER-MAIN
      
      - asset_id: CHW-HEADER-MAIN
        asset_type: PipeHeader
        role: distribution_header
        
        connections:
          - type: supplied_by
            source_asset_id: CHW-PUMP-PRIMARY-01
          - type: supplies_to
            target_asset_id: CHW-PUMP-SECONDARY-2F-01
          - type: supplies_to
            target_asset_id: CHW-PUMP-SECONDARY-3F-01
      
      - asset_id: CHW-PUMP-SECONDARY-2F-01
        asset_type: Pump
        role: secondary_circulation
        floor: 2F
        
        connections:
          - type: supplied_by
            source_asset_id: CHW-HEADER-MAIN
          - type: supplies_to
            target_asset_id: AHU-2F-A-01
          - type: supplies_to
            target_asset_id: AHU-2F-A-02
          # ... (連接到所有 2F AHU)
      
      - asset_id: AHU-2F-A-01
        asset_type: AirHandlingUnit
        role: air_conditioning
        floor: 2F
        area: A
        
        connections:
          - type: supplied_by
            source_asset_id: CHW-PUMP-SECONDARY-2F-01
            medium: chilled_water
          - type: supplies_to
            target_asset_id: CLEANROOM-2F-A
            medium: conditioned_air
  
  # 氮氣系統
  - system: nitrogen_system
    name: 氮氣系統
    
    assets:
      - asset_id: CDA-SYSTEM-MAIN
        asset_type: AirCompressor
        role: compressed_air_supply
        
        connections:
          - type: supplies_to
            target_asset_id: N2-SYSTEM-MAIN
            medium: compressed_air
      
      - asset_id: N2-SYSTEM-MAIN
        asset_type: NitrogenGenerator
        role: nitrogen_generation
        
        connections:
          - type: supplied_by
            source_asset_id: CDA-SYSTEM-MAIN
          - type: supplies_to
            target_asset_id: N2-DISTRIBUTION-2F
            medium: nitrogen
          - type: supplies_to
            target_asset_id: N2-DISTRIBUTION-3F
            medium: nitrogen
      
      - asset_id: N2-DISTRIBUTION-2F
        asset_type: PipeDistribution
        role: nitrogen_distribution
        floor: 2F
        
        connections:
          - type: supplied_by
            source_asset_id: N2-SYSTEM-MAIN
          - type: supplies_to
            target_asset_id: WB-2F-A-001
          - type: supplies_to
            target_asset_id: WB-2F-A-002
          # ... (連接到所有需要氮氣的設備)
  
  # 電力系統
  - system: power_system
    name: 電力系統
    
    assets:
      - asset_id: UTILITY-GRID
        asset_type: UtilityGrid
        role: primary_power_source
        
        connections:
          - type: supplies_to
            target_asset_id: TRANSFORMER-MAIN-01
            medium: electricity_22.8kV
      
      - asset_id: TRANSFORMER-MAIN-01
        asset_type: PowerTransformer
        role: voltage_transformation
        
        connections:
          - type: supplied_by
            source_asset_id: UTILITY-GRID
          - type: supplies_to
            target_asset_id: SWITCHGEAR-MAIN-01
            medium: electricity_380V
      
      - asset_id: SWITCHGEAR-MAIN-01
        asset_type: Switchgear
        role: main_distribution
        
        connections:
          - type: supplied_by
            source_asset_id: TRANSFORMER-MAIN-01
          - type: supplies_to
            target_asset_id: UPS-MAIN-01
          - type: supplies_to
            target_asset_id: PANEL-2F-A-01
          - type: supplies_to
            target_asset_id: PANEL-3F-A-01
          # ... (連接到所有配電盤)
      
      - asset_id: UPS-MAIN-01
        asset_type: UPS
        role: backup_power
        
        connections:
          - type: supplied_by
            source_asset_id: SWITCHGEAR-MAIN-01
          - type: supplies_to
            target_asset_id: PANEL-2F-CRITICAL-01
          - type: supplies_to
            target_asset_id: PANEL-3F-CRITICAL-01
      
      - asset_id: PANEL-2F-A-01
        asset_type: DistributionPanel
        role: floor_distribution
        floor: 2F
        area: A
        
        connections:
          - type: supplied_by
            source_asset_id: SWITCHGEAR-MAIN-01
          - type: supplies_to
            target_asset_id: DS-2F-001
          - type: supplies_to
            target_asset_id: DS-2F-002
          # ... (連接到所有 2F-A 區設備)
```

---

## 7. 資產統計

### 7.1 資產數量統計

```yaml
# ========================================
# Asset Count Summary
# ========================================

asset_summary:
  total_assets: 2000+
  
  by_category:
    building:
      buildings: 1
      floors: 6
      areas: 20
    
    production_equipment:
      die_sorter: 20
      die_bonder: 50
      wire_bonder: 80
      dispenser: 40
      tester: 100
      taping_machine: 30
      aoi_system: 15
      xray_inspection: 5
      total: 340
    
    facility_equipment:
      hvac:
        chiller: 10
        ahu: 40
        ffu: 1000+
        exhaust_fan: 20
      utility:
        di_water_system: 5
        nitrogen_generator: 3
        air_compressor: 8
        vacuum_pump: 4
      electrical:
        transformer: 5
        switchgear: 30
        ups: 5
        generator: 2
        distribution_panel: 100+
      waste_treatment:
        scrubber: 10
        wastewater_treatment: 2
      total: 1240+
    
    sensors:
      environmental:
        temperature: 400
        humidity: 400
        differential_pressure: 200
        particle_counter: 100
      energy:
        power_meter: 200
        water_meter: 50
        gas_flow_meter: 50
      equipment:
        vibration: 50
        temperature: 50
      safety:
        smoke_detector: 50
        gas_detector: 30
        access_control: 20
      total: 1600+
```

### 7.2 資產價值統計

```yaml
# ========================================
# Asset Value Summary
# ========================================

asset_value:
  total_value_usd: 150000000
  
  by_category:
    building:
      value_usd: 50000000
      percentage: 33.3
    
    production_equipment:
      die_sorter: 17000000  # 20 × 850,000
      die_bonder: 32500000  # 50 × 650,000
      wire_bonder: 36000000  # 80 × 450,000
      dispenser: 15200000  # 40 × 380,000
      tester: 28000000  # 100 × 280,000
      taping_machine: 9600000  # 30 × 320,000
      aoi_system: 6300000  # 15 × 420,000
      xray_inspection: 2900000  # 5 × 580,000
      total: 147500000
      percentage: 98.3
    
    facility_equipment:
      hvac: 8000000
      utility: 3500000
      electrical: 4500000
      waste_treatment: 2000000
      total: 18000000
      percentage: 12.0
```

### 7.3 資產能耗統計

```yaml
# ========================================
# Asset Energy Consumption Summary
# ========================================

energy_consumption:
  total_power_kw: 5000
  annual_energy_kwh: 35000000  # 5000 kW × 7000 hours/year
  annual_energy_cost_usd: 10500000  # 0.3 USD/kWh
  
  by_category:
    production_equipment:
      power_kw: 2500
      percentage: 50.0
      equipment_breakdown:
        die_sorter: 170  # 20 × 8.5
        die_bonder: 325  # 50 × 6.5
        wire_bonder: 440  # 80 × 5.5
        dispenser: 180  # 40 × 4.5
        tester: 600  # 100 × 6.0
        taping_machine: 120  # 30 × 4.0
        aoi_system: 75  # 15 × 5.0
        xray_inspection: 40  # 5 × 8.0
        others: 550
    
    facility_equipment:
      power_kw: 2500
      percentage: 50.0
      equipment_breakdown:
        hvac: 1500  # Chillers, AHUs, FFUs
        compressed_air: 200  # Compressors
        nitrogen: 85  # N2 generator
        di_water: 18  # DI water systems
        lighting: 300
        others: 397
```

---

## 總結

本文件建立了 Harvatek LED 封裝廠的完整資產模型,基於 IDTF V3.1 和 IADL V3.1 標準,涵蓋:

1. **建築結構**: 1 棟建築,6 個樓層,20 個生產和設施區域
2. **生產設備**: 340 台設備,8 種設備類型,3 條生產線
3. **設施系統**: 1,240+ 台設施設備,涵蓋 HVAC、公用系統、電力、廢物處理
4. **感測器網路**: 1,600+ 個感測器,涵蓋環境、能源、設備、安全監控
5. **資產關係**: 完整的生產流程和設施系統連接關係
6. **3D 整合**: 所有資產均包含 OpenUSD 3D 模型定義
7. **數據整合**: 所有設備和感測器均定義 OT 數據標籤
8. **Omniverse 視覺化**: 所有資產均包含實時數據綁定和視覺化定義

此資產模型可直接用於:
- IADL Editor V3.1 進行資產建模和編輯
- NDH V3.1 進行數據整合和分析
- Omniverse 進行 3D 視覺化和數位分身應用
- 預測性維護和智能優化系統
- 能源管理和成本分析

**總資產價值**: USD 150M  
**年能源消耗**: 35,000 MWh  
**年能源成本**: USD 10.5M

---

**文件版本**: 1.0  
**最後更新**: 2025-10-12  
**作者**: IDTF V3.1 Team

