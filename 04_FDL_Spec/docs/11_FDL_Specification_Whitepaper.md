# FDL (Factory Design Language) 規範設計白皮書

**版本**: 3.2  
**發布日期**: 2025-10-16  
**作者**: Manus AI  
**組織**: IDTF Consortium

---

## 2. FDL 概述

### 設計理念

FDL 的設計基於以下核心理念:

1. **聲明式語法**: 描述「是什麼」而非「怎麼做」
2. **階層結構**: 支援建築物、樓層、區域的階層組織
3. **Asset Instance 引用**: 引用 IADL 定義的資產藍圖，並實例化為 Asset Instance。
4. **關係定義**: 明確定義 Asset Instance 之間的連接和依賴。
5. **參數化**: 支援參數化配置,提高重用性

### FDL 在 IDTF 生態系統中的角色

```
┌─────────────────────────────────────────────────────────┐
│                    IDTF V3.2 生態系統                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  IADL (Asset Blueprint)                                  │
│    ↓                                                     │
│  FDL (Factory Layout) ← 本白皮書的焦點                   │
│    ↓                                                     │
│  NDH (Deployment Execution)                              │
│    ↓                                                     │
│  Omniverse (Visualization)                               │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### FDL 與 IADL 的關係

| 維度 | IADL | FDL |
|------|------|-----|
| **關注點** | 資產是什麼 (What) | 工廠如何佈局 (How) |
| **粒度** | 單一資產類型 | 整座工廠 |
| **內容** | 3D 模型、數據標籤、行為 | 數量、位置、連接關係、Asset Instance 的次要階層與生命週期事件配置 |
| **重用性** | 跨專案重用 | 跨工廠複製 |
| **生命週期** | 設計階段 | 設計到部署 |

---

## 3. FDL 語法規範

### 基本結構

FDL 採用 YAML 格式,具有清晰的階層結構:

```yaml
# FDL 基本結構
factory_design:
  metadata:
    name: <工廠名稱>
    version: <版本號>
    author: <作者>
    date: <日期>
    description: <描述>
  
  buildings:
    - building_id: <建築物 ID>
      name: <建築物名稱>
      location: <地理位置>
      floors: <樓層列表>
  
  layout:
    - area: <區域名稱>
      building: <所屬建築物>
      floor: <所屬樓層>
      zone_type: <區域類型>
      instances: <Asset Instance 列表>
      relationships: <關係列表>
  
  utilities:
    - type: <公用系統類型>
      specifications: <規格>
      distribution: <分配方式>
  
  parameters:
    <全域參數定義>
```

### 元數據 (Metadata)

定義工廠的基本資訊:

```yaml
metadata:
  name: "Harvatek LED Packaging Factory"
  version: "1.0.0"
  author: "C.C. Lin"
  date: "2025-10-12"
  description: "LED封裝廠完整佈局設計"
  tags:
    - "LED"
    - "Semiconductor"
    - "High-Tech"
  industry: "Electronics Manufacturing"
  location:
    country: "Taiwan"
    city: "Hsinchu"
    address: "No. 123, Tech Road"
    coordinates:
      latitude: 24.8138
      longitude: 120.9675
```

### 建築物定義 (Buildings)

定義工廠的建築結構:

```yaml
buildings:
  - building_id: "MainBuilding"
    name: "主廠房"
    type: "Production"
    construction_year: 2020
    total_area: 50000  # 平方米
    floors:
      - floor_id: "B1"
        level: -1
        height: 4.5
        area: 10000
        purpose: "Utility & Storage"
      - floor_id: "1F"
        level: 1
        height: 5.0
        area: 10000
        purpose: "Production & Office"
      - floor_id: "2F"
        level: 2
        height: 4.0
        area: 10000
        purpose: "Production - Standard LED"
      - floor_id: "3F"
        level: 3
        height: 4.0
        area: 10000
        purpose: "Production - High Power LED"
      - floor_id: "4F"
        level: 4
        height: 4.0
        area: 5000
        purpose: "R&D & Quality Lab"
      - floor_id: "5F"
        level: 5
        height: 3.5
        area: 5000
        purpose: "Office & Meeting"
```

### 佈局定義 (Layout)

這是 FDL 的核心部分,定義 Asset Instance 的佈局:

```yaml
layout:
    - area: "Production_Zone_A"
      building: "MainBuilding"
      floor: "2F"
      zone_type: "CleanRoom"
      cleanliness_class: "ISO_Class_7"
      temperature: 23  # 攝氏度
      humidity: 45     # 相對濕度 %
    
    instances:
      # 晶片分選機
      - type: "DieSorter_v1.0"  # 引用 IADL 資產類型
        count: 10
        naming_prefix: "DS_2F_A_"
        naming_pattern: "{prefix}{index:03d}"  # DS_2F_A_001
        layout_pattern: "grid"
        grid_config:
          rows: 2
          columns: 5
          spacing_x: 3.0  # 米
          spacing_y: 4.0  # 米
        origin:
          x: 10.0
          y: 20.0
          z: 0.0
        orientation: 0  # 度 (0=北, 90=東, 180=南, 270=西)
        instance_params:
          sorting_speed: 12000  # UPH
          bin_count: 8
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessArea"
            path: "/Fab/Lithography"
        lifecycle_event_configs:
          - event_name: "PowerOn"
            initial_state: "Ready"
          - event_name: "StartProduction"
            initial_state: "Running"
      
      # 固晶機
      - type: "DieBonder_v1.0"
        count: 8
        naming_prefix: "DB_2F_A_"
        layout_pattern: "linear"
        linear_config:
          direction: "horizontal"
          spacing: 3.5
        origin:
          x: 10.0
          y: 30.0
          z: 0.0
        instance_params:
          bonding_speed: 8000  # UPH
          accuracy: 0.001  # mm
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessArea"
            path: "/Bonding"
      
      # 打線機
      - type: "WireBonder_v1.0"
        count: 12
        naming_prefix: "WB_2F_A_"
        layout_pattern: "grid"
        grid_config:
          rows: 3
          columns: 4
          spacing_x: 2.5
          spacing_y: 3.0
        origin:
          x: 10.0
          y: 40.0
          z: 0.0
        instance_params:
          wire_diameter: 0.025  # mm
          bonding_speed: 10000  # UPH
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessArea"
            path: "/WireBonding"
    
    relationships:
      # 物料流關係
      - type: "material_flow"
        from: "DS_2F_A_001"
        to: "DB_2F_A_001"
        properties:
          transport_method: "AGV"
          cycle_time: 120  # 秒
          batch_size: 100
      
      - type: "material_flow"
        from: "DB_2F_A_001"
        to: "WB_2F_A_001"
        properties:
          transport_method: "Conveyor"
          speed: 0.5  # m/s
      
      # 數據連接關係
      - type: "data_connection"
        from: "DS_2F_A_*"  # 萬用字元,匹配所有 Die Sorter
        to: "MES_Server"
        properties:
          protocol: "SECS/GEM"
          update_interval: 1  # 秒
      
      # 電力供應關係
      - type: "power_supply"
        from: "Transformer_2F_A"
        to: "DS_2F_A_*"
        properties:
          voltage: 220  # V
          phase: 3
          capacity: 15  # kW per machine
```

### 公用系統 (Utilities)

定義工廠的公用系統配置:

```yaml
utilities:
  # 電力系統
  - type: "electrical"
    specifications:
      total_capacity: 5000  # kW
      voltage_levels:
        - 22000  # V (高壓輸入)
        - 380   # V (三相)
        - 220   # V (單相)
      transformers:
        - id: "Transformer_Main"
          capacity: 2000  # kVA
          location: "B1_Electrical_Room"
        - id: "Transformer_2F"
          capacity: 1500  # kVA
          location: "2F_Electrical_Room"
    distribution:
      - floor: "2F"
        zones: ["Production_Zone_A", "Production_Zone_B"]
        capacity: 1500  # kW
      - floor: "3F"
        zones: ["Production_Zone_C", "Production_Zone_D"]
        capacity: 1500  # kW
  
  # HVAC 系統
  - type: "hvac"
    specifications:
      total_cooling_capacity: 2000  # RT
      air_handling_units:
        - id: "AHU_2F_A"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_A"]
        - id: "AHU_2F_B"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_B"]
      chillers:
        - id: "Chiller_01"
          capacity: 1000  # RT
          location: "B1_Mechanical_Room"
          type: "Water-Cooled"
    distribution:
      - floor: "2F"
        temperature_setpoint: 23  # °C
        humidity_setpoint: 45     # %RH
        air_changes_per_hour: 20
  
  # 純水系統
  - type: "di_water"
    specifications:
      production_capacity: 100  # m³/day
      resistivity: 18.2  # MΩ·cm
      storage_tanks:
        - id: "DI_Tank_01"
          capacity: 50  # m³
          location: "B1_Utility_Room"
    distribution:
      - floor: "2F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
      - floor: "3F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
  
  # 氮氣系統
  - type: "nitrogen"
    specifications:
      purity: 99.999  # %
      production_method: "PSA"
      capacity: 500  # Nm³/hr
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
  
  # 壓縮空氣系統
  - type: "compressed_air"
    specifications:
      pressure: 7.0  # bar
      capacity: 1000  # Nm³/hr
      dew_point: -40  # °C
      compressors:
        - id: "Compressor_01"
          capacity: 500  # Nm³/hr
          type: "Screw"
          location: "B1_Compressor_Room"
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
```

### 參數定義 (Parameters)

定義全域參數和可配置項:

```yaml
parameters:
  # 全域設定
  global:
    timezone: "Asia/Taipei"
    working_hours: "24/7"
    shifts: 3
    language: "zh-TW"
  
  # 生產參數
  production:
    target_oee: 85  # %
    cycle_time_target: 120  # 秒
    batch_size_standard: 1000
    quality_target: 99.5  # %
  
  # 環境參數
  environment:
    cleanroom:
      temperature_range: [22, 24]  # °C
      humidity_range: [40, 50]     # %RH
      pressure_differential: 15    # Pa
    office:
      temperature_range: [23, 26]  # °C
      humidity_range: [40, 60]     # %RH
  
  # 安全參數
  safety:
    emergency_exits: 8
    fire_extinguishers: 50
    smoke_detectors: 200
    evacuation_time_target: 300  # 秒
  
  # 能源參數
  energy:
    peak_demand_limit: 4500  # kW
    power_factor_target: 0.95
    energy_efficiency_target: 0.85
```

---

## 廠房類型與設計模式

### 1. 半導體廠房 (Semiconductor Fab)

#### 特徵

- **高度潔淨**: ISO Class 1-5 無塵室
- **精密環控**: 溫度 ±0.1°C, 濕度 ±1%
- **複雜 MEP**: 超純水、特殊氣體、化學品供應
- **高能耗**: 單位面積能耗極高
- **嚴格 ESD**: 靜電防護要求

#### FDL 設計模式

```yaml
# 台積電 12 吋晶圓廠範例
factory_design:
  metadata:
    name: "TSMC 12-inch Wafer Fab"
    facility_type: "Semiconductor"
    technology_node: "5nm"
  
  buildings:
    - building_id: "Fab_Building"
      type: "Cleanroom_Facility"
      total_area: 100000  # m²
      floors:
        - floor_id: "B2"
          purpose: "Utility_Basement"
        - floor_id: "B1"
          purpose: "Sub_Fab"  # 設備下層,管線空間
        - floor_id: "1F"
          purpose: "Fab_Floor"  # 主要製程區
        - floor_id: "2F"
          purpose: "Fan_Filter_Unit"  # FFU 層
  
  layout:
    - area: "Lithography_Bay"
      building: "Fab_Building"
      floor: "1F"
      zone_type: "CleanRoom"
      cleanliness_class: "ISO_Class_2"
      temperature: 23.0
      temperature_tolerance: 0.1
      humidity: 45
      
      # FDL 層級可配置 Asset Instance 的次要階層路徑
      secondary_hierarchy_paths:
        - type: "ProcessArea"
          path: "/Fab/Lithography"

      # FDL 層級可配置 Asset Instance 的生命週期事件行為
      lifecycle_event_configs:
        - event_name: "StartShift"
          initial_state: "ReadyForProduction"
        - event_name: "EndShift"
          initial_state: "Idle"

---

## 2. FDL 概述

### 設計理念

FDL 的設計基於以下核心理念:

1. **聲明式語法**: 描述「是什麼」而非「怎麼做」
2. **階層結構**: 支援建築物、樓層、區域的階層組織
3. **Asset Instance 引用**: 引用 IADL 定義的資產藍圖，並實例化為 Asset Instance。
4. **關係定義**: 明確定義 Asset Instance 之間的連接和依賴。
5. **參數化**: 支援參數化配置,提高重用性

### FDL 在 IDTF 生態系統中的角色

```
┌─────────────────────────────────────────────────────────┐
│                    IDTF V3.2 生態系統                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  IADL (Asset Blueprint)                                  │
│    ↓                                                     │
│  FDL (Factory Layout) ← 本白皮書的焦點                   │
│    ↓                                                     │
│  NDH (Deployment Execution)                              │
│    ↓                                                     │
│  Omniverse (Visualization)                               │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### FDL 與 IADL 的關係

| 維度 | IADL | FDL |
|------|------|-----|
| **關注點** | 資產是什麼 (What) | 工廠如何佈局 (How) |
| **粒度** | 單一資產類型 | 整座工廠 |
| **內容** | 3D 模型、數據標籤、行為 | 數量、位置、連接關係、Asset Instance 的次要階層與生命週期事件配置 |
| **重用性** | 跨專案重用 | 跨工廠複製 |
| **生命週期** | 設計階段 | 設計到部署 |

---

## 3. FDL 語法規範

### 基本結構

FDL 採用 YAML 格式,具有清晰的階層結構:

```yaml
# FDL 基本結構
factory_design:
  metadata:
    name: <工廠名稱>
    version: <版本號>
    author: <作者>
    date: <日期>
    description: <描述>
  
  buildings:
    - building_id: <建築物 ID>
      name: <建築物名稱>
      location: <地理位置>
      floors: <樓層列表>
  
  layout:
    - area: <區域名稱>
      building: <所屬建築物>
      floor: <所屬樓層>
      instances: <Asset Instance 列表>
      relationships: <關係列表>
  
  utilities:
    - type: <公用系統類型>
      specifications: <規格>
      distribution: <分配方式>
  
  parameters:
    <全域參數定義>
```

### 元數據 (Metadata)

定義工廠的基本資訊:

```yaml
metadata:
  name: "Harvatek LED Packaging Factory"
  version: "1.0.0"
  author: "C.C. Lin"
  date: "2025-10-12"
  description: "LED封裝廠完整佈局設計"
  tags:
    - "LED"
    - "Semiconductor"
    - "High-Tech"
  industry: "Electronics Manufacturing"
  location:
    country: "Taiwan"
    city: "Hsinchu"
    address: "No. 123, Tech Road"
    coordinates:
      latitude: 24.8138
      longitude: 120.9675
```

### 建築物定義 (Buildings)

定義工廠的建築結構:

```yaml
buildings:
  - building_id: "MainBuilding"
    name: "主廠房"
    type: "Production"
    construction_year: 2020
    total_area: 50000  # 平方米
    floors:
      - floor_id: "B1"
        level: -1
        height: 4.5
        area: 10000
        purpose: "Utility & Storage"
      - floor_id: "1F"
        level: 1
        height: 5.0
        area: 10000
        purpose: "Production & Office"
      - floor_id: "2F"
        level: 2
        height: 4.0
        area: 10000
        purpose: "Production - Standard LED"
      - floor_id: "3F"
        level: 3
        height: 4.0
        area: 10000
        purpose: "Production - High Power LED"
      - floor_id: "4F"
        level: 4
        height: 4.0
        area: 5000
        purpose: "R&D & Quality Lab"
      - floor_id: "5F"
        level: 5
        height: 3.5
        area: 5000
        purpose: "Office & Meeting"
```

### 佈局定義 (Layout)

這是 FDL 的核心部分,定義 Asset Instance 的佈局:

```yaml
layout:
  - area: "Production_Zone_A"
    building: "MainBuilding"
    floor: "2F"
    zone_type: "CleanRoom"
    cleanliness_class: "ISO_Class_7"
    temperature: 23  # 攝氏度
    humidity: 45     # 相對濕度 %
    
    instances:
      # 晶片分選機
      - type: "DieSorter_v1.0"  # 引用 IADL 資產類型
        count: 10
        naming_prefix: "DS_2F_A_"
        naming_pattern: "{prefix}{index:03d}"  # DS_2F_A_001
        layout_pattern: "grid"
        grid_config:
          rows: 2
          columns: 5
          spacing_x: 3.0  # 米
          spacing_y: 4.0  # 米
        origin:
          x: 10.0
          y: 20.0
          z: 0.0
        orientation: 0  # 度 (0=北, 90=東, 180=南, 270=西)
        instance_params:
          sorting_speed: 12000  # UPH
          bin_count: 8
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessArea"
            path: "/Sorting"
        lifecycle_event_configs:
          - event_name: "PowerOn"
            initial_state: "Ready"
          - event_name: "StartProduction"
            initial_state: "Running"
      
      # 固晶機
      - type: "DieBonder_v1.0"
        count: 8
        naming_prefix: "DB_2F_A_"
        layout_pattern: "linear"
        linear_config:
          direction: "horizontal"
          spacing: 3.5
        origin:
          x: 10.0
          y: 30.0
          z: 0.0
        instance_params:
          bonding_speed: 8000  # UPH
          accuracy: 0.001  # mm
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessArea"
            path: "/Bonding"
      
      # 打線機
      - type: "WireBonder_v1.0"
        count: 12
        naming_prefix: "WB_2F_A_"
        layout_pattern: "grid"
        grid_config:
          rows: 3
          columns: 4
          spacing_x: 2.5
          spacing_y: 3.0
        origin:
          x: 10.0
          y: 40.0
          z: 0.0
        instance_params:
          wire_diameter: 0.025  # mm
          bonding_speed: 10000  # UPH
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessArea"
            path: "/WireBonding"
    
    relationships:
      # 物料流關係
      - type: "material_flow"
        from: "DS_2F_A_001"
        to: "DB_2F_A_001"
        properties:
          transport_method: "AGV"
          cycle_time: 120  # 秒
          batch_size: 100
      
      - type: "material_flow"
        from: "DB_2F_A_001"
        to: "WB_2F_A_001"
        properties:
          transport_method: "Conveyor"
          speed: 0.5  # m/s
      
      # 數據連接關係
      - type: "data_connection"
        from: "DS_2F_A_*"  # 萬用字元,匹配所有 Die Sorter
        to: "MES_Server"
        properties:
          protocol: "SECS/GEM"
          update_interval: 1  # 秒
      
      # 電力供應關係
      - type: "power_supply"
        from: "Transformer_2F_A"
        to: "DS_2F_A_*"
        properties:
          voltage: 220  # V
          phase: 3
          capacity: 15  # kW per machine
```

### 公用系統 (Utilities)

定義工廠的公用系統配置:

```yaml
utilities:
  # 電力系統
  - type: "electrical"
    specifications:
      total_capacity: 5000  # kW
      voltage_levels:
        - 22000  # V (高壓輸入)
        - 380   # V (三相)
        - 220   # V (單相)
      transformers:
        - id: "Transformer_Main"
          capacity: 2000  # kVA
          location: "B1_Electrical_Room"
        - id: "Transformer_2F"
          capacity: 1500  # kVA
          location: "2F_Electrical_Room"
    distribution:
      - floor: "2F"
        zones: ["Production_Zone_A", "Production_Zone_B"]
        capacity: 1500  # kW
      - floor: "3F"
        zones: ["Production_Zone_C", "Production_Zone_D"]
        capacity: 1500  # kW
  
  # HVAC 系統
  - type: "hvac"
    specifications:
      total_cooling_capacity: 2000  # RT
      air_handling_units:
        - id: "AHU_2F_A"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_A"]
        - id: "AHU_2F_B"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_B"]
      chillers:
        - id: "Chiller_01"
          capacity: 1000  # RT
          location: "B1_Mechanical_Room"
          type: "Water-Cooled"
    distribution:
      - floor: "2F"
        temperature_setpoint: 23  # °C
        humidity_setpoint: 45     # %RH
        air_changes_per_hour: 20
  
  # 純水系統
  - type: "di_water"
    specifications:
      production_capacity: 100  # m³/day
      resistivity: 18.2  # MΩ·cm
      storage_tanks:
        - id: "DI_Tank_01"
          capacity: 50  # m³
          location: "B1_Utility_Room"
    distribution:
      - floor: "2F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
      - floor: "3F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
  
  # 氮氣系統
  - type: "nitrogen"
    specifications:
      purity: 99.999  # %
      production_method: "PSA"
      capacity: 500  # Nm³/hr
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
  
  # 壓縮空氣系統
  - type: "compressed_air"
    specifications:
      pressure: 7.0  # bar
      capacity: 1000  # Nm³/hr
      dew_point: -40  # °C
      compressors:
        - id: "Compressor_01"
          capacity: 500  # Nm³/hr
          type: "Screw"
          location: "B1_Compressor_Room"
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
```

### 參數定義 (Parameters)

定義全域參數和可配置項:

```yaml
parameters:
  # 全域設定
  global:
    timezone: "Asia/Taipei"
    working_hours: "24/7"
    shifts: 3
    language: "zh-TW"
  
  # 生產參數
  production:
    target_oee: 85  # %
    cycle_time_target: 120  # 秒
    batch_size_standard: 1000
    quality_target: 99.5  # %
  
  # 環境參數
  environment:
    cleanroom:
      temperature_range: [22, 24]  # °C
      humidity_range: [40, 50]     # %RH
      pressure_differential: 15    # Pa
    office:
      temperature_range: [23, 26]  # °C
      humidity_range: [40, 60]     # %RH
  
  # 安全參數
  safety:
    emergency_exits: 8
    fire_extinguishers: 50
    smoke_detectors: 200
    evacuation_time_target: 300  # 秒
  
  # 能源參數
  energy:
    peak_demand_limit: 4500  # kW
    power_factor_target: 0.95
    energy_efficiency_target: 0.85
```

---

## 廠房類型與設計模式

### 1. 半導體廠房 (Semiconductor Fab)

#### 特徵

- **高度潔淨**: ISO Class 1-5 無塵室
- **精密環控**: 溫度 ±0.1°C, 濕度 ±1%
- **複雜 MEP**: 超純水、特殊氣體、化學品供應
- **高能耗**: 單位面積能耗極高
- **嚴格 ESD**: 靜電防護要求

#### FDL 設計模式

```yaml
# 台積電 12 吋晶圓廠範例
factory_design:
  metadata:
    name: "TSMC 12-inch Wafer Fab"
    facility_type: "Semiconductor"
    technology_node: "5nm"
  
  buildings:
    - building_id: "Fab_Building"
      type: "Cleanroom_Facility"
      total_area: 100000  # m²
      floors:
        - floor_id: "B2"
          purpose: "Utility_Basement"
        - floor_id: "B1"
          purpose: "Sub_Fab"  # 設備下層,管線空間
        - floor_id: "1F"
          purpose: "Fab_Floor"  # 主要製程區
        - floor_id: "2F"
          purpose: "Fan_Filter_Unit"  # FFU 層
  
  layout:
    - area: "Lithography_Bay"
      building: "Fab_Building"
      floor: "1F"
      zone_type: "CleanRoom"
      cleanliness_class: "ISO_Class_2"
      temperature: 23.0
      temperature_tolerance: 0.1
      humidity: 45
      
      # FDL 層級可配置 Asset Instance 的次要階層路徑
      secondary_hierarchy_paths:
        - type: "ProcessArea"
            path: "/Fab/Lithography"

      # FDL 層級可配置 Asset Instance 的生命週期事件行為
      lifecycle_event_configs:
        - event_name: "StartShift"
          initial_state: "ReadyForProduction"
        - event_name: "EndShift"
          initial_state: "Idle"

---

## 2. FDL 概述

### 設計理念

FDL 的設計基於以下核心理念:

1. **聲明式語法**: 描述「是什麼」而非「怎麼做」
2. **階層結構**: 支援建築物、樓層、區域的階層組織
3. **Asset Instance 引用**: 引用 IADL 定義的資產藍圖，並實例化為 Asset Instance。
4. **關係定義**: 明確定義 Asset Instance 之間的連接和依賴。
5. **參數化**: 支援參數化配置,提高重用性

### FDL 在 IDTF 生態系統中的角色

```
┌─────────────────────────────────────────────────────────┐
│                    IDTF V3.2 生態系統                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  IADL (資產藍圖)                                         │
│    ↓                                                     │
│  FDL (工廠佈局) ← 本白皮書的焦點                         │
│    ↓                                                     │
│  NDH (執行部署)                                          │
│    ↓                                                     │
│  Omniverse (視覺化)                                      │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### FDL 與 IADL 的關係

| 維度 | IADL | FDL |
|------|------|-----|
| **關注點** | 資產是什麼 (What) | 工廠如何佈局 (How) |
| **粒度** | 單一資產類型 | 整座工廠 |
| **內容** | 3D 模型、數據標籤、行為 | 數量、位置、連接關係、Asset Instance 的次要階層與生命週期事件配置 |
| **重用性** | 跨專案重用 | 跨工廠複製 |
| **生命週期** | 設計階段 | 設計到部署 |

---

## 3. FDL 語法規範

### 基本結構

FDL 採用 YAML 格式,具有清晰的階層結構:

```yaml
# FDL 基本結構
factory_design:
  metadata:
    name: <工廠名稱>
    version: <版本號>
    author: <作者>
    date: <日期>
    description: <描述>
  
  buildings:
    - building_id: <建築物 ID>
      name: <建築物名稱>
      location: <地理位置>
      floors: <樓層列表>
  
  layout:
    - area: <區域名稱>
      building: <所屬建築物>
      floor: <所屬樓層>
      zone_type: <區域類型>
      instances: <Asset Instance 列表>
      relationships: <關係列表>
  
  utilities:
    - type: <公用系統類型>
      specifications: <規格>
      distribution: <分配方式>
  
  parameters:
    <全域參數定義>
```

### 元數據 (Metadata)

定義工廠的基本資訊:

```yaml
metadata:
  name: "Harvatek LED Packaging Factory"
  version: "1.0.0"
  author: "C.C. Lin"
  date: "2025-10-12"
  description: "LED封裝廠完整佈局設計"
  tags:
    - "LED"
    - "Semiconductor"
    - "High-Tech"
  industry: "Electronics Manufacturing"
  location:
    country: "Taiwan"
    city: "Hsinchu"
    address: "No. 123, Tech Road"
    coordinates:
      latitude: 24.8138
      longitude: 120.9675
```

### 建築物定義 (Buildings)

定義工廠的建築結構:

```yaml
buildings:
  - building_id: "MainBuilding"
    name: "主廠房"
    type: "Production"
    construction_year: 2020
    total_area: 50000  # 平方米
    floors:
      - floor_id: "B1"
        level: -1
        height: 4.5
        area: 10000
        purpose: "Utility & Storage"
      - floor_id: "1F"
        level: 1
        height: 5.0
        area: 10000
        purpose: "Production & Office"
      - floor_id: "2F"
        level: 2
        height: 4.0
        area: 10000
        purpose: "Production - Standard LED"
      - floor_id: "3F"
        level: 3
        height: 4.0
        area: 10000
        purpose: "Production - High Power LED"
      - floor_id: "4F"
        level: 4
        height: 4.0
        area: 5000
        purpose: "R&D & Quality Lab"
      - floor_id: "5F"
        level: 5
        height: 3.5
        area: 5000
        purpose: "Office & Meeting"
```

### 佈局定義 (Layout)

這是 FDL 的核心部分,定義 Asset Instance 的佈局:

```yaml
layout:
  - area: "Production_Zone_A"
    building: "MainBuilding"
    floor: "2F"
    zone_type: "CleanRoom"
    cleanliness_class: "ISO_Class_7"
    temperature: 23  # 攝氏度
    humidity: 45     # 相對濕度 %
    
    instances:
      # 晶片分選機
      - type: "DieSorter_v1.0"  # 引用 IADL 資產類型
        count: 10
        naming_prefix: "DS_2F_A_"
        naming_pattern: "{prefix}{index:03d}"  # DS_2F_A_001
        layout_pattern: "grid"
        grid_config:
          rows: 2
          columns: 5
          spacing_x: 3.0  # 米
          spacing_y: 4.0  # 米
        origin:
          x: 10.0
          y: 20.0
          z: 0.0
        orientation: 0  # 度 (0=北, 90=東, 180=南, 270=西)
        instance_params:
          sorting_speed: 12000  # UPH
          bin_count: 8
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessStep"
            path: "/Sorting"
        lifecycle_event_configs:
          - event_name: "PowerOn"
            initial_state: "Ready"
          - event_name: "StartProduction"
            initial_state: "Running"
      
      # 固晶機
      - type: "DieBonder_v1.0"
        count: 8
        naming_prefix: "DB_2F_A_"
        layout_pattern: "linear"
        linear_config:
          direction: "horizontal"
          spacing: 3.5
        origin:
          x: 10.0
          y: 30.0
          z: 0.0
        instance_params:
          bonding_speed: 8000  # UPH
          accuracy: 0.001  # mm
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessStep"
            path: "/Bonding"
      
      # 打線機
      - type: "WireBonder_v1.0"
        count: 12
        naming_prefix: "WB_2F_A_"
        layout_pattern: "grid"
        grid_config:
          rows: 3
          columns: 4
          spacing_x: 2.5
          spacing_y: 3.0
        origin:
          x: 10.0
          y: 40.0
          z: 0.0
        instance_params:
          wire_diameter: 0.025  # mm
          bonding_speed: 10000  # UPH
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessStep"
            path: "/WireBonding"
    
    relationships:
      # 物料流關係
      - type: "material_flow"
        from: "DS_2F_A_001"
        to: "DB_2F_A_001"
        properties:
          transport_method: "AGV"
          cycle_time: 120  # 秒
          batch_size: 100
      
      - type: "material_flow"
        from: "DB_2F_A_001"
        to: "WB_2F_A_001"
        properties:
          transport_method: "Conveyor"
          speed: 0.5  # m/s
      
      # 數據連接關係
      - type: "data_connection"
        from: "DS_2F_A_*"  # 萬用字元,匹配所有 Die Sorter
        to: "MES_Server"
        properties:
          protocol: "SECS/GEM"
          update_interval: 1  # 秒
      
      # 電力供應關係
      - type: "power_supply"
        from: "Transformer_2F_A"
        to: "DS_2F_A_*"
        properties:
          voltage: 220  # V
          phase: 3
          capacity: 15  # kW per machine
```

### 公用系統 (Utilities)

定義工廠的公用系統配置:

```yaml
utilities:
  # 電力系統
  - type: "electrical"
    specifications:
      total_capacity: 5000  # kW
      voltage_levels:
        - 22000  # V (高壓輸入)
        - 380   # V (三相)
        - 220   # V (單相)
      transformers:
        - id: "Transformer_Main"
          capacity: 2000  # kVA
          location: "B1_Electrical_Room"
        - id: "Transformer_2F"
          capacity: 1500  # kVA
          location: "2F_Electrical_Room"
    distribution:
      - floor: "2F"
        zones: ["Production_Zone_A", "Production_Zone_B"]
        capacity: 1500  # kW
      - floor: "3F"
        zones: ["Production_Zone_C", "Production_Zone_D"]
        capacity: 1500  # kW
  
  # HVAC 系統
  - type: "hvac"
    specifications:
      total_cooling_capacity: 2000  # RT
      air_handling_units:
        - id: "AHU_2F_A"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_A"]
        - id: "AHU_2F_B"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_B"]
      chillers:
        - id: "Chiller_01"
          capacity: 1000  # RT
          location: "B1_Mechanical_Room"
          type: "Water-Cooled"
    distribution:
      - floor: "2F"
        temperature_setpoint: 23  # °C
        humidity_setpoint: 45     # %RH
        air_changes_per_hour: 20
  
  # 純水系統
  - type: "di_water"
    specifications:
      production_capacity: 100  # m³/day
      resistivity: 18.2  # MΩ·cm
      storage_tanks:
        - id: "DI_Tank_01"
          capacity: 50  # m³
          location: "B1_Utility_Room"
    distribution:
      - floor: "2F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
      - floor: "3F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
  
  # 氮氣系統
  - type: "nitrogen"
    specifications:
      purity: 99.999  # %
      production_method: "PSA"
      capacity: 500  # Nm³/hr
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
  
  # 壓縮空氣系統
  - type: "compressed_air"
    specifications:
      pressure: 7.0  # bar
      capacity: 1000  # Nm³/hr
      dew_point: -40  # °C
      compressors:
        - id: "Compressor_01"
          capacity: 500  # Nm³/hr
          type: "Screw"
          location: "B1_Compressor_Room"
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
```

### 參數定義 (Parameters)

定義全域參數和可配置項:

```yaml
parameters:
  # 全域設定
  global:
    timezone: "Asia/Taipei"
    working_hours: "24/7"
    shifts: 3
    language: "zh-TW"
  
  # 生產參數
  production:
    target_oee: 85  # %
    cycle_time_target: 120  # 秒
    batch_size_standard: 1000
    quality_target: 99.5  # %
  
  # 環境參數
  environment:
    cleanroom:
      temperature_range: [22, 24]  # °C
      humidity_range: [40, 50]     # %RH
      pressure_differential: 15    # Pa
    office:
      temperature_range: [23, 26]  # °C
      humidity_range: [40, 60]     # %RH
  
  # 安全參數
  safety:
    emergency_exits: 8
    fire_extinguishers: 50
    smoke_detectors: 200
    evacuation_time_target: 300  # 秒
  
  # 能源參數
  energy:
    peak_demand_limit: 4500  # kW
    power_factor_target: 0.95
    energy_efficiency_target: 0.85
```

---

## 廠房類型與設計模式

### 1. 半導體廠房 (Semiconductor Fab)

#### 特徵

- **高度潔淨**: ISO Class 1-5 無塵室
- **精密環控**: 溫度 ±0.1°C, 濕度 ±1%
- **複雜 MEP**: 超純水、特殊氣體、化學品供應
- **高能耗**: 單位面積能耗極高
- **嚴格 ESD**: 靜電防護要求

#### FDL 設計模式

```yaml
# 台積電 12 吋晶圓廠範例
factory_design:
  metadata:
    name: "TSMC 12-inch Wafer Fab"
    facility_type: "Semiconductor"
    technology_node: "5nm"
  
  buildings:
    - building_id: "Fab_Building"
      type: "Cleanroom_Facility"
      total_area: 100000  # m²
      floors:
        - floor_id: "B2"
          purpose: "Utility_Basement"
        - floor_id: "B1"
          purpose: "Sub_Fab"  # 設備下層,管線空間
        - floor_id: "1F"
          purpose: "Fab_Floor"  # 主要製程區
        - floor_id: "2F"
          purpose: "Fan_Filter_Unit"  # FFU 層
  
  layout:
    - area: "Lithography_Bay"
      building: "Fab_Building"
      floor: "1F"
      zone_type: "CleanRoom"
      cleanliness_class: "ISO_Class_2"
      temperature: 23.0
      temperature_tolerance: 0.1
      humidity: 45
      
      # FDL 層級可配置 Asset Instance 的次要階層路徑
      secondary_hierarchy_paths:
        - type: "ProcessArea"
          path: "/Fab/Lithography"

      # FDL 層級可配置 Asset Instance 的生命週期事件行為
      lifecycle_event_configs:
        - event_name: "StartShift"
          initial_state: "ReadyForProduction"
        - event_name: "EndShift"
          initial_state: "Idle"

---

## 2. FDL 概述

### 設計理念

FDL 的設計基於以下核心理念:

1. **聲明式語法**: 描述「是什麼」而非「怎麼做」
2. **階層結構**: 支援建築物、樓層、區域的階層組織
3. **Asset Instance 引用**: 引用 IADL 定義的資產藍圖，並實例化為 Asset Instance。
4. **關係定義**: 明確定義 Asset Instance 之間的連接和依賴。
5. **參數化**: 支援參數化配置,提高重用性

### FDL 在 IDTF 生態系統中的角色

```
┌─────────────────────────────────────────────────────────┐
│                    IDTF V3.2 生態系統                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  IADL (資產藍圖)                                         │
│    ↓                                                     │
│  FDL (工廠佈局) ← 本白皮書的焦點                         │
│    ↓                                                     │
│  NDH (執行部署)                                          │
│    ↓                                                     │
│  Omniverse (視覺化)                                      │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### FDL 與 IADL 的關係

| 維度 | IADL | FDL |
|------|------|-----|
| **關注點** | 資產是什麼 (What) | 工廠如何佈局 (How) |
| **粒度** | 單一資產類型 | 整座工廠 |
| **內容** | 3D 模型、數據標籤、行為 | 數量、位置、連接關係、Asset Instance 的次要階層與生命週期事件配置 |
| **重用性** | 跨專案重用 | 跨工廠複製 |
| **生命週期** | 設計階段 | 設計到部署 |

---

## 3. FDL 語法規範

### 基本結構

FDL 採用 YAML 格式,具有清晰的階層結構:

```yaml
# FDL 基本結構
factory_design:
  metadata:
    name: <工廠名稱>
    version: <版本號>
    author: <作者>
    date: <日期>
    description: <描述>
  
  buildings:
    - building_id: <建築物 ID>
      name: <建築物名稱>
      location: <地理位置>
      floors: <樓層列表>
  
  layout:
    - area: <區域名稱>
      building: <所屬建築物>
      floor: <所屬樓層>
      zone_type: <區域類型>
      instances: <Asset Instance 列表>
      relationships: <關係列表>
  
  utilities:
    - type: <公用系統類型>
      specifications: <規格>
      distribution: <分配方式>
  
  parameters:
    <全域參數定義>
```

### 元數據 (Metadata)

定義工廠的基本資訊:

```yaml
metadata:
  name: "Harvatek LED Packaging Factory"
  version: "1.0.0"
  author: "C.C. Lin"
  date: "2025-10-12"
  description: "LED封裝廠完整佈局設計"
  tags:
    - "LED"
    - "Semiconductor"
    - "High-Tech"
  industry: "Electronics Manufacturing"
  location:
    country: "Taiwan"
    city: "Hsinchu"
    address: "No. 123, Tech Road"
    coordinates:
      latitude: 24.8138
      longitude: 120.9675
```

### 建築物定義 (Buildings)

定義工廠的建築結構:

```yaml
buildings:
  - building_id: "MainBuilding"
    name: "主廠房"
    type: "Production"
    construction_year: 2020
    total_area: 50000  # 平方米
    floors:
      - floor_id: "B1"
        level: -1
        height: 4.5
        area: 10000
        purpose: "Utility & Storage"
      - floor_id: "1F"
        level: 1
        height: 5.0
        area: 10000
        purpose: "Production & Office"
      - floor_id: "2F"
        level: 2
        height: 4.0
        area: 10000
        purpose: "Production - Standard LED"
      - floor_id: "3F"
        level: 3
        height: 4.0
        area: 10000
        purpose: "Production - High Power LED"
      - floor_id: "4F"
        level: 4
        height: 4.0
        area: 5000
        purpose: "R&D & Quality Lab"
      - floor_id: "5F"
        level: 5
        height: 3.5
        area: 5000
        purpose: "Office & Meeting"
```

### 佈局定義 (Layout)

這是 FDL 的核心部分,定義 Asset Instance 的佈局:

```yaml
layout:
  - area: "Production_Zone_A"
    building: "MainBuilding"
    floor: "2F"
    zone_type: "CleanRoom"
    cleanliness_class: "ISO_Class_7"
    temperature: 23  # 攝氏度
    humidity: 45     # 相對濕度 %
    
    instances:
      # 晶片分選機
      - type: "DieSorter_v1.0"  # 引用 IADL 資產類型
        count: 10
        naming_prefix: "DS_2F_A_"
        naming_pattern: "{prefix}{index:03d}"  # DS_2F_A_001
        layout_pattern: "grid"
        grid_config:
          rows: 2
          columns: 5
          spacing_x: 3.0  # 米
          spacing_y: 4.0  # 米
        origin:
          x: 10.0
          y: 20.0
          z: 0.0
        orientation: 0  # 度 (0=北, 90=東, 180=南, 270=西)
        instance_params:
          sorting_speed: 12000  # UPH
          bin_count: 8
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessStep"
            path: "/Sorting"
        lifecycle_event_configs:
          - event_name: "PowerOn"
            initial_state: "Ready"
          - event_name: "StartProduction"
            initial_state: "Running"
      
      # 固晶機
      - type: "DieBonder_v1.0"
        count: 8
        naming_prefix: "DB_2F_A_"
        layout_pattern: "linear"
        linear_config:
          direction: "horizontal"
          spacing: 3.5
        origin:
          x: 10.0
          y: 30.0
          z: 0.0
        instance_params:
          bonding_speed: 8000  # UPH
          accuracy: 0.001  # mm
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessStep"
            path: "/Bonding"
      
      # 打線機
      - type: "WireBonder_v1.0"
        count: 12
        naming_prefix: "WB_2F_A_"
        layout_pattern: "grid"
        grid_config:
          rows: 3
          columns: 4
          spacing_x: 2.5
          spacing_y: 3.0
        origin:
          x: 10.0
          y: 40.0
          z: 0.0
        instance_params:
          wire_diameter: 0.025  # mm
          bonding_speed: 10000  # UPH
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessStep"
            path: "/WireBonding"
    
    relationships:
      # 物料流關係
      - type: "material_flow"
        from: "DS_2F_A_001"
        to: "DB_2F_A_001"
        properties:
          transport_method: "AGV"
          cycle_time: 120  # 秒
          batch_size: 100
      
      - type: "material_flow"
        from: "DB_2F_A_001"
        to: "WB_2F_A_001"
        properties:
          transport_method: "Conveyor"
          speed: 0.5  # m/s
      
      # 數據連接關係
      - type: "data_connection"
        from: "DS_2F_A_*"  # 萬用字元,匹配所有 Die Sorter
        to: "MES_Server"
        properties:
          protocol: "SECS/GEM"
          update_interval: 1  # 秒
      
      # 電力供應關係
      - type: "power_supply"
        from: "Transformer_2F_A"
        to: "DS_2F_A_*"
        properties:
          voltage: 220  # V
          phase: 3
          capacity: 15  # kW per machine
```

### 公用系統 (Utilities)

定義工廠的公用系統配置:

```yaml
utilities:
  # 電力系統
  - type: "electrical"
    specifications:
      total_capacity: 5000  # kW
      voltage_levels:
        - 22000  # V (高壓輸入)
        - 380   # V (三相)
        - 220   # V (單相)
      transformers:
        - id: "Transformer_Main"
          capacity: 2000  # kVA
          location: "B1_Electrical_Room"
        - id: "Transformer_2F"
          capacity: 1500  # kVA
          location: "2F_Electrical_Room"
    distribution:
      - floor: "2F"
        zones: ["Production_Zone_A", "Production_Zone_B"]
        capacity: 1500  # kW
      - floor: "3F"
        zones: ["Production_Zone_C", "Production_Zone_D"]
        capacity: 1500  # kW
  
  # HVAC 系統
  - type: "hvac"
    specifications:
      total_cooling_capacity: 2000  # RT
      air_handling_units:
        - id: "AHU_2F_A"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_A"]
        - id: "AHU_2F_B"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_B"]
      chillers:
        - id: "Chiller_01"
          capacity: 1000  # RT
          location: "B1_Mechanical_Room"
          type: "Water-Cooled"
    distribution:
      - floor: "2F"
        temperature_setpoint: 23  # °C
        humidity_setpoint: 45     # %RH
        air_changes_per_hour: 20
  
  # 純水系統
  - type: "di_water"
    specifications:
      production_capacity: 100  # m³/day
      resistivity: 18.2  # MΩ·cm
      storage_tanks:
        - id: "DI_Tank_01"
          capacity: 50  # m³
          location: "B1_Utility_Room"
    distribution:
      - floor: "2F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
      - floor: "3F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
  
  # 氮氣系統
  - type: "nitrogen"
    specifications:
      purity: 99.999  # %
      production_method: "PSA"
      capacity: 500  # Nm³/hr
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
  
  # 壓縮空氣系統
  - type: "compressed_air"
    specifications:
      pressure: 7.0  # bar
      capacity: 1000  # Nm³/hr
      dew_point: -40  # °C
      compressors:
        - id: "Compressor_01"
          capacity: 500  # Nm³/hr
          type: "Screw"
          location: "B1_Compressor_Room"
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
```

### 參數定義 (Parameters)

定義全域參數和可配置項:

```yaml
parameters:
  # 全域設定
  global:
    timezone: "Asia/Taipei"
    working_hours: "24/7"
    shifts: 3
    language: "zh-TW"
  
  # 生產參數
  production:
    target_oee: 85  # %
    cycle_time_target: 120  # 秒
    batch_size_standard: 1000
    quality_target: 99.5  # %
  
  # 環境參數
  environment:
    cleanroom:
      temperature_range: [22, 24]  # °C
      humidity_range: [40, 50]     # %RH
      pressure_differential: 15    # Pa
    office:
      temperature_range: [23, 26]  # °C
      humidity_range: [40, 60]     # %RH
  
  # 安全參數
  safety:
    emergency_exits: 8
    fire_extinguishers: 50
    smoke_detectors: 200
    evacuation_time_target: 300  # 秒
  
  # 能源參數
  energy:
    peak_demand_limit: 4500  # kW
    power_factor_target: 0.95
    energy_efficiency_target: 0.85
```

---

## 廠房類型與設計模式

### 1. 半導體廠房 (Semiconductor Fab)

#### 特徵

- **高度潔淨**: ISO Class 1-5 無塵室
- **精密環控**: 溫度 ±0.1°C, 濕度 ±1%
- **複雜 MEP**: 超純水、特殊氣體、化學品供應
- **高能耗**: 單位面積能耗極高
- **嚴格 ESD**: 靜電防護要求

#### FDL 設計模式

```yaml
# 台積電 12 吋晶圓廠範例
factory_design:
  metadata:
    name: "TSMC 12-inch Wafer Fab"
    facility_type: "Semiconductor"
    technology_node: "5nm"
  
  buildings:
    - building_id: "Fab_Building"
      type: "Cleanroom_Facility"
      total_area: 100000  # m²
      floors:
        - floor_id: "B2"
          purpose: "Utility_Basement"
        - floor_id: "B1"
          purpose: "Sub_Fab"  # 設備下層,管線空間
        - floor_id: "1F"
          purpose: "Fab_Floor"  # 主要製程區
        - floor_id: "2F"
          purpose: "Fan_Filter_Unit"  # FFU 層
  
  layout:
    - area: "Lithography_Bay"
      building: "Fab_Building"
      floor: "1F"
      zone_type: "CleanRoom"
      cleanliness_class: "ISO_Class_2"
      temperature: 23.0
      temperature_tolerance: 0.1
      humidity: 45
      
      # FDL 層級可配置 Asset Instance 的次要階層路徑
      secondary_hierarchy_paths:
        - type: "ProcessArea"
          path: "/Fab/Lithography"

      # FDL 層級可配置 Asset Instance 的生命週期事件行為
      lifecycle_event_configs:
        - event_name: "StartShift"
          initial_state: "ReadyForProduction"
        - event_name: "EndShift"
          initial_state: "Idle"

---

## 2. FDL 概述

### 設計理念

FDL 的設計基於以下核心理念:

1. **聲明式語法**: 描述「是什麼」而非「怎麼做」
2. **階層結構**: 支援建築物、樓層、區域的階層組織
3. **Asset Instance 引用**: 引用 IADL 定義的資產藍圖，並實例化為 Asset Instance。
4. **關係定義**: 明確定義 Asset Instance 之間的連接和依賴。
5. **參數化**: 支援參數化配置,提高重用性

### FDL 在 IDTF 生態系統中的角色

```
┌─────────────────────────────────────────────────────────┐
│                    IDTF V3.2 生態系統                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  IADL (資產藍圖)                                         │
│    ↓                                                     │
│  FDL (工廠佈局) ← 本白皮書的焦點                         │
│    ↓                                                     │
│  NDH (執行部署)                                          │
│    ↓                                                     │
│  Omniverse (視覺化)                                      │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### FDL 與 IADL 的關係

| 維度 | IADL | FDL |
|------|------|-----|
| **關注點** | 資產是什麼 (What) | 工廠如何佈局 (How) |
| **粒度** | 單一資產類型 | 整座工廠 |
| **內容** | 3D 模型、數據標籤、行為 | 數量、位置、連接關係、Asset Instance 的次要階層與生命週期事件配置 |
| **重用性** | 跨專案重用 | 跨工廠複製 |
| **生命週期** | 設計階段 | 設計到部署 |

---

## 3. FDL 語法規範

### 基本結構

FDL 採用 YAML 格式,具有清晰的階層結構:

```yaml
# FDL 基本結構
factory_design:
  metadata:
    name: <工廠名稱>
    version: <版本號>
    author: <作者>
    date: <日期>
    description: <描述>
  
  buildings:
    - building_id: <建築物 ID>
      name: <建築物名稱>
      location: <地理位置>
      floors: <樓層列表>
  
  layout:
    - area: <區域名稱>
      building: <所屬建築物>
      floor: <所屬樓層>
      zone_type: <區域類型>
      instances: <Asset Instance 列表>
      relationships: <關係列表>
  
  utilities:
    - type: <公用系統類型>
      specifications: <規格>
      distribution: <分配方式>
  
  parameters:
    <全域參數定義>
```

### 元數據 (Metadata)

定義工廠的基本資訊:

```yaml
metadata:
  name: "Harvatek LED Packaging Factory"
  version: "1.0.0"
  author: "C.C. Lin"
  date: "2025-10-12"
  description: "LED封裝廠完整佈局設計"
  tags:
    - "LED"
    - "Semiconductor"
    - "High-Tech"
  industry: "Electronics Manufacturing"
  location:
    country: "Taiwan"
    city: "Hsinchu"
    address: "No. 123, Tech Road"
    coordinates:
      latitude: 24.8138
      longitude: 120.9675
```

### 建築物定義 (Buildings)

定義工廠的建築結構:

```yaml
buildings:
  - building_id: "MainBuilding"
    name: "主廠房"
    type: "Production"
    construction_year: 2020
    total_area: 50000  # 平方米
    floors:
      - floor_id: "B1"
        level: -1
        height: 4.5
        area: 10000
        purpose: "Utility & Storage"
      - floor_id: "1F"
        level: 1
        height: 5.0
        area: 10000
        purpose: "Production & Office"
      - floor_id: "2F"
        level: 2
        height: 4.0
        area: 10000
        purpose: "Production - Standard LED"
      - floor_id: "3F"
        level: 3
        height: 4.0
        area: 10000
        purpose: "Production - High Power LED"
      - floor_id: "4F"
        level: 4
        height: 4.0
        area: 5000
        purpose: "R&D & Quality Lab"
      - floor_id: "5F"
        level: 5
        height: 3.5
        area: 5000
        purpose: "Office & Meeting"
```

### 佈局定義 (Layout)

這是 FDL 的核心部分,定義 Asset Instance 的佈局:

```yaml
layout:
  - area: "Production_Zone_A"
    building: "MainBuilding"
    floor: "2F"
    zone_type: "CleanRoom"
    cleanliness_class: "ISO_Class_7"
    temperature: 23  # 攝氏度
    humidity: 45     # 相對濕度 %
    
    instances:
      # 晶片分選機
      - type: "DieSorter_v1.0"  # 引用 IADL 資產類型
        count: 10
        naming_prefix: "DS_2F_A_"
        naming_pattern: "{prefix}{index:03d}"  # DS_2F_A_001
        layout_pattern: "grid"
        grid_config:
          rows: 2
          columns: 5
          spacing_x: 3.0  # 米
          spacing_y: 4.0  # 米
        origin:
          x: 10.0
          y: 20.0
          z: 0.0
        orientation: 0  # 度 (0=北, 90=東, 180=南, 270=西)
        instance_params:
          sorting_speed: 12000  # UPH
          bin_count: 8
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessStep"
            path: "/Sorting"
        lifecycle_event_configs:
          - event_name: "PowerOn"
            initial_state: "Ready"
          - event_name: "StartProduction"
            initial_state: "Running"
      
      # 固晶機
      - type: "DieBonder_v1.0"
        count: 8
        naming_prefix: "DB_2F_A_"
        layout_pattern: "linear"
        linear_config:
          direction: "horizontal"
          spacing: 3.5
        origin:
          x: 10.0
          y: 30.0
          z: 0.0
        instance_params:
          bonding_speed: 8000  # UPH
          accuracy: 0.001  # mm
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessStep"
            path: "/Bonding"
      
      # 打線機
      - type: "WireBonder_v1.0"
        count: 12
        naming_prefix: "WB_2F_A_"
        layout_pattern: "grid"
        grid_config:
          rows: 3
          columns: 4
          spacing_x: 2.5
          spacing_y: 3.0
        origin:
          x: 10.0
          y: 40.0
          z: 0.0
        instance_params:
          wire_diameter: 0.025  # mm
          bonding_speed: 10000  # UPH
          status: "Idle"
        secondary_hierarchy_paths:
          - type: "ProcessStep"
            path: "/WireBonding"
    
    relationships:
      # 物料流關係
      - type: "material_flow"
        from: "DS_2F_A_001"
        to: "DB_2F_A_001"
        properties:
          transport_method: "AGV"
          cycle_time: 120  # 秒
          batch_size: 100
      
      - type: "material_flow"
        from: "DB_2F_A_001"
        to: "WB_2F_A_001"
        properties:
          transport_method: "Conveyor"
          speed: 0.5  # m/s
      
      # 數據連接關係
      - type: "data_connection"
        from: "DS_2F_A_*"  # 萬用字元,匹配所有 Die Sorter
        to: "MES_Server"
        properties:
          protocol: "SECS/GEM"
          update_interval: 1  # 秒
      
      # 電力供應關係
      - type: "power_supply"
        from: "Transformer_2F_A"
        to: "DS_2F_A_*"
        properties:
          voltage: 220  # V
          phase: 3
          capacity: 15  # kW per machine
```

### 公用系統 (Utilities)

定義工廠的公用系統配置:

```yaml
utilities:
  # 電力系統
  - type: "electrical"
    specifications:
      total_capacity: 5000  # kW
      voltage_levels:
        - 22000  # V (高壓輸入)
        - 380   # V (三相)
        - 220   # V (單相)
      transformers:
        - id: "Transformer_Main"
          capacity: 2000  # kVA
          location: "B1_Electrical_Room"
        - id: "Transformer_2F"
          capacity: 1500  # kVA
          location: "2F_Electrical_Room"
    distribution:
      - floor: "2F"
        zones: ["Production_Zone_A", "Production_Zone_B"]
        capacity: 1500  # kW
      - floor: "3F"
        zones: ["Production_Zone_C", "Production_Zone_D"]
        capacity: 1500  # kW
  
  # HVAC 系統
  - type: "hvac"
    specifications:
      total_cooling_capacity: 2000  # RT
      air_handling_units:
        - id: "AHU_2F_A"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_A"]
        - id: "AHU_2F_B"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_B"]
      chillers:
        - id: "Chiller_01"
          capacity: 1000  # RT
          location: "B1_Mechanical_Room"
          type: "Water-Cooled"
    distribution:
      - floor: "2F"
        temperature_setpoint: 23  # °C
        humidity_setpoint: 45     # %RH
        air_changes_per_hour: 20
  
  # 純水系統
  - type: "di_water"
    specifications:
      production_capacity: 100  # m³/day
      resistivity: 18.2  # MΩ·cm
      storage_tanks:
        - id: "DI_Tank_01"
          capacity: 50  # m³
          location: "B1_Utility_Room"
    distribution:
      - floor: "2F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
      - floor: "3F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
  
  # 氮氣系統
  - type: "nitrogen"
    specifications:
      purity: 99.999  # %
      production_method: "PSA"
      capacity: 500  # Nm³/hr
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
  
  # 壓縮空氣系統
  - type: "compressed_air"
    specifications:
      pressure: 7.0  # bar
      capacity: 1000  # Nm³/hr
      dew_point: -40  # °C
      compressors:
        - id: "Compressor_01"
          capacity: 500  # Nm³/hr
          type: "Screw"
          location: "B1_Compressor_Room"
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
```

### 參數定義 (Parameters)

定義全域參數和可配置項:

```yaml
parameters:
  # 全域設定
  global:
    timezone: "Asia/Taipei"
    working_hours: "24/7"
    shifts: 3
    language: "zh-TW"
  
  # 生產參數
  production:
    target_oee: 85  # %
    cycle_time_target: 120  # 秒
    batch_size_standard: 1000
    quality_target: 99.5  # %
  
  # 環境參數
  environment:
    cleanroom:
      temperature_range: [22, 24]  # °C
      humidity_range: [40, 50]     # %RH
      pressure_differential: 15    # Pa
    office:
      temperature_range: [23, 26]  # °C
      humidity_range: [40, 60]     # %RH
  
  # 安全參數
  safety:
    emergency_exits: 8
    fire_extinguishers: 50
    smoke_detectors: 200
    evacuation_time_target: 300  # 秒
  
  # 能源參數
  energy:
    peak_demand_limit: 4500  # kW
    power_factor_target: 0.95
    energy_efficiency_target: 0.85
```

---

## 廠房類型與設計模式

### 1. 半導體廠房 (Semiconductor Fab)

#### 特徵

- **高度潔淨**: ISO Class 1-5 無塵室
- **精密環控**: 溫度 ±0.1°C, 濕度 ±1%
- **複雜 MEP**: 超純水、特殊氣體、化學品供應
- **高能耗**: 單位面積能耗極高
- **嚴格 ESD**: 靜電防護要求

#### FDL 設計模式

```yaml
# 台積電 12 吋晶圓廠範例
factory_design:
  metadata:
    name: "TSMC 12-inch Wafer Fab"
    facility_type: "Semiconductor"
    technology_node: "5nm"
  
  buildings:
    - building_id: "Fab_Building"
      type: "Cleanroom_Facility"
      total_area: 100000  # m²
      floors:
        - floor_id: "B2"
          purpose: "Utility_Basement"
        - floor_id: "B1"
          purpose: "Sub_Fab"  # 設備下層,管線空間
        - floor_id: "1F"
          purpose: "Fab_Floor"  # 主要製程區
        - floor_id: "2F"
          purpose: "Fan_Filter_Unit"  # FFU 層
  
  layout:
    - area: "Lithography_Bay"
      building: "Fab_Building"
      floor: "1F"
      zone_type: "CleanRoom"
      cleanliness_class: "ISO_Class_2"
      temperature: 23.0
      temperature_tolerance: 0.1
      humidity: 45

---

## 目錄

1. [執行摘要](#執行摘要)
2. [FDL 概述](#fdl-概述)
3. [FDL 語法規範](#fdl-語法規範)
4. [廠房類型與設計模式](#廠房類型與設計模式)
5. [實際應用案例](#實際應用案例)
6. [最佳實踐指南](#最佳實踐指南)
7. [擴展與客製化](#擴展與客製化)
8. [總結與展望](#總結與展望)

---

## 執行摘要

**FDL (Factory Design Language)** 是 IDTF V3.3 生態系統中用於定義工廠佈局和配置的聲明式語言。FDL 填補了從資產建模(IADL)到工廠部署(NDH)之間的關鍵缺口,提供了一個標準化、可重用且易於維護的工廠設計描述方式。

本白皮書基於多個實際專案的經驗,包括:
- **半導體廠房** (台積電等)
- **LED 封裝廠** (宏齊科技)
- **LNG 發電廠**
- **NVIDIA GPU 數據中心**
- **Smart City 基礎設施**

歸納整理出完整的 FDL 規範,涵蓋語法定義、設計模式、應用案例和最佳實踐。

### 核心價值

1. **標準化**: 統一的工廠描述語言,適用於各種產業
2. **可重用性**: 一次定義,多處部署
3. **可維護性**: 版本控制,變更追蹤
4. **自動化**: 自動生成部署配置
5. **可擴展性**: 支援客製化和擴展

---

## FDL 概述

### 設計理念

FDL 的設計基於以下核心理念:

1. **聲明式語法**: 描述「是什麼」而非「怎麼做」
2. **階層結構**: 支援建築物、樓層、區域的階層組織
3. **資產引用**: 引用 IADL 定義的資產藍圖
4. **關係定義**: 明確定義資產之間的連接和依賴
5. **參數化**: 支援參數化配置,提高重用性

### FDL 在 IDTF 生態系統中的角色

```
┌─────────────────────────────────────────────────────────┐
│                    IDTF V3.3 生態系統                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  IADL (資產藍圖)                                         │
│    ↓                                                     │
│  FDL (工廠佈局) ← 本白皮書的焦點                         │
│    ↓                                                     │
│  NDH (執行部署)                                          │
│    ↓                                                     │
│  Omniverse (視覺化)                                      │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### FDL 與 IADL 的關係

| 維度 | IADL | FDL |
|------|------|-----|
| **關注點** | 資產是什麼 (What) | 工廠如何佈局 (How) |
| **粒度** | 單一資產類型 | 整座工廠 |
| **內容** | 3D 模型、數據標籤、行為、預設次要階層與生命週期事件 | 數量、位置、連接關係、Asset Instance 的次要階層與生命週期事件配置 |
| **重用性** | 跨專案重用 | 跨工廠複製 |
| **生命週期** | 設計階段 | 設計到部署 |

---

## FDL 語法規範

### 基本結構

FDL 採用 YAML 格式,具有清晰的階層結構:

```yaml
# FDL 基本結構
factory_design:
  metadata:
    name: <工廠名稱>
    version: <版本號>
    author: <作者>
    date: <日期>
    description: <描述>
  
  buildings:
    - building_id: <建築物 ID>
      name: <建築物名稱>
      location: <地理位置>
      floors: <樓層列表>
  
  layout:
    - area: <區域名稱>
      building: <所屬建築物>
      floor: <所屬樓層>
      zone_type: <區域類型>
      instances: <Asset Instance List>
      relationships: <關係列表>
  
  utilities:
    - type: <公用系統類型>
      specifications: <規格>
      distribution: <分配方式>
  
  parameters:
    <全域參數定義>
```

### 元數據 (Metadata)

定義工廠的基本資訊:

```yaml
metadata:
  name: "Harvatek LED Packaging Factory"
  version: "1.0.0"
  author: "C.C. Lin"
  date: "2025-10-12"
  description: "LED封裝廠完整佈局設計"
  tags:
    - "LED"
    - "Semiconductor"
    - "High-Tech"
  industry: "Electronics Manufacturing"
  location:
    country: "Taiwan"
    city: "Hsinchu"
    address: "No. 123, Tech Road"
    coordinates:
      latitude: 24.8138
      longitude: 120.9675
```

### 建築物定義 (Buildings)

定義工廠的建築結構:

```yaml
buildings:
  - building_id: "MainBuilding"
    name: "主廠房"
    type: "Production"
    construction_year: 2020
    total_area: 50000  # 平方米
    floors:
      - floor_id: "B1"
        level: -1
        height: 4.5
        area: 10000
        purpose: "Utility & Storage"
      - floor_id: "1F"
        level: 1
        height: 5.0
        area: 10000
        purpose: "Production & Office"
      - floor_id: "2F"
        level: 2
        height: 4.0
        area: 10000
        purpose: "Production - Standard LED"
      - floor_id: "3F"
        level: 3
        height: 4.0
        area: 10000
        purpose: "Production - High Power LED"
      - floor_id: "4F"
        level: 4
        height: 4.0
        area: 5000
        purpose: "R&D & Quality Lab"
      - floor_id: "5F"
        level: 5
        height: 3.5
        area: 5000
        purpose: "Office & Meeting"
```

### 佈局定義 (Layout)

這是 FDL 的核心部分,定義資產實例的佈局:

```yaml
layout:
  - area: "Production_Zone_A"
    building: "MainBuilding"
    floor: "2F"
    zone_type: "CleanRoom"
    cleanliness_class: "ISO_Class_7"
    temperature: 23  # 攝氏度
    humidity: 45     # 相對濕度 %
    
    instances:
      # 晶片分選機
      - type: "DieSorter_v1.0"  # 引用 IADL 資產類型
        count: 10
        naming_prefix: "DS_2F_A_"
        naming_pattern: "{prefix}{index:03d}"  # DS_2F_A_001
        layout_pattern: "grid"
        grid_config:
          rows: 2
          columns: 5
          spacing_x: 3.0  # 米
          spacing_y: 4.0  # 米
        origin:
          x: 10.0
          y: 20.0
          z: 0.0
        orientation: 0  # 度 (0=北, 90=東, 180=南, 270=西)
        instance_params:
          sorting_speed: 12000  # UPH
          bin_count: 8
          status: "Idle"
        maintenance:
          schedule: "weekly"
          last_maintenance: "2025-10-01"
      
      # 固晶機
      - type: "DieBonder_v1.0"
        count: 8
        naming_prefix: "DB_2F_A_"
        layout_pattern: "linear"
        linear_config:
          direction: "horizontal"
          spacing: 3.5
        origin:
          x: 10.0
          y: 30.0
          z: 0.0
        instance_params:
          bonding_speed: 8000  # UPH
          accuracy: 0.001  # mm
          status: "Idle"
      
      # 打線機
      - type: "WireBonder_v1.0"
        count: 12
        naming_prefix: "WB_2F_A_"
        layout_pattern: "grid"
        grid_config:
          rows: 3
          columns: 4
          spacing_x: 2.5
          spacing_y: 3.0
        origin:
          x: 10.0
          y: 40.0
          z: 0.0
        instance_params:
          wire_diameter: 0.025  # mm
          bonding_speed: 10000  # UPH
          status: "Idle"
    
    relationships:
      # 物料流關係
      - type: "material_flow"
        from: "DS_2F_A_001"
        to: "DB_2F_A_001"
        properties:
          transport_method: "AGV"
          cycle_time: 120  # 秒
          batch_size: 100
      
      - type: "material_flow"
        from: "DB_2F_A_001"
        to: "WB_2F_A_001"
        properties:
          transport_method: "Conveyor"
          speed: 0.5  # m/s
      
      # 數據連接關係
      - type: "data_connection"
        from: "DS_2F_A_*"  # 萬用字元,匹配所有 Die Sorter
        to: "MES_Server"
        properties:
          protocol: "SECS/GEM"
          update_interval: 1  # 秒
      
      # 電力供應關係
      - type: "power_supply"
        from: "Transformer_2F_A"
        to: "DS_2F_A_*"
        properties:
          voltage: 220  # V
          phase: 3
          capacity: 15  # kW per machine
```

### 公用系統 (Utilities)

定義工廠的公用系統配置:

```yaml
utilities:
  # 電力系統
  - type: "electrical"
    specifications:
      total_capacity: 5000  # kW
      voltage_levels:
        - 22000  # V (高壓輸入)
        - 380   # V (三相)
        - 220   # V (單相)
      transformers:
        - id: "Transformer_Main"
          capacity: 2000  # kVA
          location: "B1_Electrical_Room"
        - id: "Transformer_2F"
          capacity: 1500  # kVA
          location: "2F_Electrical_Room"
    distribution:
      - floor: "2F"
        zones: ["Production_Zone_A", "Production_Zone_B"]
        capacity: 1500  # kW
      - floor: "3F"
        zones: ["Production_Zone_C", "Production_Zone_D"]
        capacity: 1500  # kW
  
  # HVAC 系統
  - type: "hvac"
    specifications:
      total_cooling_capacity: 2000  # RT
      air_handling_units:
        - id: "AHU_2F_A"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_A"]
        - id: "AHU_2F_B"
          capacity: 50000  # CMH
          location: "2F_Ceiling"
          serving_zones: ["Production_Zone_B"]
      chillers:
        - id: "Chiller_01"
          capacity: 1000  # RT
          location: "B1_Mechanical_Room"
          type: "Water-Cooled"
    distribution:
      - floor: "2F"
        temperature_setpoint: 23  # °C
        humidity_setpoint: 45     # %RH
        air_changes_per_hour: 20
  
  # 純水系統
  - type: "di_water"
    specifications:
      production_capacity: 100  # m³/day
      resistivity: 18.2  # MΩ·cm
      storage_tanks:
        - id: "DI_Tank_01"
          capacity: 50  # m³
          location: "B1_Utility_Room"
    distribution:
      - floor: "2F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
      - floor: "3F"
        flow_rate: 20  # L/min
        pressure: 3.0  # bar
  
  # 氮氣系統
  - type: "nitrogen"
    specifications:
      purity: 99.999  # %
      production_method: "PSA"
      capacity: 500  # Nm³/hr
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 200  # Nm³/hr
  
  # 壓縮空氣系統
  - type: "compressed_air"
    specifications:
      pressure: 7.0  # bar
      capacity: 1000  # Nm³/hr
      dew_point: -40  # °C
      compressors:
        - id: "Compressor_01"
          capacity: 500  # Nm³/hr
          type: "Screw"
          location: "B1_Compressor_Room"
    distribution:
      - floor: "2F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
      - floor: "3F"
        pressure: 6.0  # bar
        flow_rate: 400  # Nm³/hr
```

### 參數定義 (Parameters)

定義全域參數和可配置項:

```yaml
parameters:
  # 全域設定
  global:
    timezone: "Asia/Taipei"
    working_hours: "24/7"
    shifts: 3
    language: "zh-TW"
  
  # 生產參數
  production:
    target_oee: 85  # %
    cycle_time_target: 120  # 秒
    batch_size_standard: 1000
    quality_target: 99.5  # %
  
  # 環境參數
  environment:
    cleanroom:
      temperature_range: [22, 24]  # °C
      humidity_range: [40, 50]     # %RH
      pressure_differential: 15    # Pa
    office:
      temperature_range: [23, 26]  # °C
      humidity_range: [40, 60]     # %RH
  
  # 安全參數
  safety:
    emergency_exits: 8
    fire_extinguishers: 50
    smoke_detectors: 200
    evacuation_time_target: 300  # 秒
  
  # 能源參數
  energy:
    peak_demand_limit: 4500  # kW
    power_factor_target: 0.95
    energy_efficiency_target: 0.85
```

---

## 廠房類型與設計模式

### 1. 半導體廠房 (Semiconductor Fab)

#### 特徵

- **高度潔淨**: ISO Class 1-5 無塵室
- **精密環控**: 溫度 ±0.1°C, 濕度 ±1%
- **複雜 MEP**: 超純水、特殊氣體、化學品供應
- **高能耗**: 單位面積能耗極高
- **嚴格 ESD**: 靜電防護要求

#### FDL 設計模式

```yaml
# 台積電 12 吋晶圓廠範例
factory_design:
  metadata:
    name: "TSMC 12-inch Wafer Fab"
    facility_type: "Semiconductor"
    technology_node: "5nm"
  
  buildings:
    - building_id: "Fab_Building"
      type: "Cleanroom_Facility"
      total_area: 100000  # m²
      floors:
        - floor_id: "B2"
          purpose: "Utility_Basement"
        - floor_id: "B1"
          purpose: "Sub_Fab"  # 設備下層,管線空間
        - floor_id: "1F"
          purpose: "Fab_Floor"  # 主要製程區
        - floor_id: "2F"
          purpose: "Fan_Filter_Unit"  # FFU 層
  
  layout:
    - area: "Lithography_Bay"
      building: "Fab_Building"
      floor: "1F"
      zone_type: "CleanRoom"
      cleanliness_class: "ISO_Class_2"
      temperature: 23.0
      temperature_tolerance: 0.1
      humidity: 45
      humidity_tolerance: 1
      vibration_limit: 0.1  # μm
      
      instances:
        - type: "EUV_Scanner"  # 極紫外光微影機
          count: 20
          naming_prefix: "EUV_"
          cost_per_unit: 150000000  # USD
          footprint:
            length: 15.0  # m
            width: 8.0    # m
            height: 4.0   # m
          power_consumption: 1000  # kW
          instance_params:
            wavelength: 13.5  # nm
            throughput: 170   # WPH (Wafers Per Hour)
  
  utilities:
    - type: "ultra_pure_water"
      specifications:
        resistivity: 18.2  # MΩ·cm
        toc: 1  # ppb (Total Organic Carbon)
        particles: 0  # >0.05μm per liter
        production_capacity: 1000  # m³/day
    
    - type: "process_gases"
      gases:
        - name: "Nitrogen"
          purity: 99.9999  # %
          flow_rate: 5000  # Nm³/hr
        - name: "Argon"
          purity: 99.999   # %
          flow_rate: 1000  # Nm³/hr
        - name: "Hydrogen"
          purity: 99.999   # %
          flow_rate: 500   # Nm³/hr
    
    - type: "chemical_distribution"
      chemicals:
        - name: "Sulfuric_Acid"
          concentration: 98  # %
          storage: 10000     # L
        - name: "Hydrofluoric_Acid"
          concentration: 49  # %
          storage: 5000      # L
```

### 2. LED 封裝廠 (LED Packaging)

#### 特徵

- **中度潔淨**: ISO Class 6-7 無塵室
- **高產能**: 高速自動化生產線
- **多樣化**: 多種產品混線生產
- **精密組裝**: μm 級精度要求
- **品質檢測**: 光學、電性全檢

#### FDL 設計模式

```yaml
# 宏齊科技 LED 封裝廠範例
factory_design:
  metadata:
    name: "Harvatek LED Packaging Factory"
    facility_type: "LED_Packaging"
    products:
      - "Standard_LED"
      - "High_Power_LED"
      - "Mini_LED"
  
  layout:
    - area: "Standard_LED_Line_1"
      building: "MainBuilding"
      floor: "2F"
      zone_type: "CleanRoom"
      cleanliness_class: "ISO_Class_7"
      
      instances:
        # 完整生產線配置
        - type: "DieSorter_v1.0"
          count: 10
          naming_prefix: "DS_L1_"
          throughput: 12000  # UPH
        
        - type: "DieBonder_v1.0"
          count: 8
          naming_prefix: "DB_L1_"
          throughput: 8000  # UPH
        
        - type: "WireBonder_v1.0"
          count: 12
          naming_prefix: "WB_L1_"
          throughput: 10000  # UPH
        
        - type: "Dispenser_v1.0"
          count: 6
          naming_prefix: "DP_L1_"
          throughput: 6000  # UPH
        
        - type: "Curing_Oven"
          count: 4
          naming_prefix: "CO_L1_"
          temperature: 150  # °C
          curing_time: 120  # 分鐘
        
        - type: "Tester_v1.0"
          count: 15
          naming_prefix: "TS_L1_"
          throughput: 15000  # UPH
          test_items:
            - "Forward_Voltage"
            - "Luminous_Intensity"
            - "Wavelength"
            - "Reverse_Current"
        
        - type: "TapingMachine_v1.0"
          count: 8
          naming_prefix: "TM_L1_"
          throughput: 12000  # UPH
      
      relationships:
        # 生產流程關係
        - type: "production_flow"
          sequence:
            - "DS_L1_*"  # Die Sorting
            - "DB_L1_*"  # Die Bonding
            - "WB_L1_*"  # Wire Bonding
            - "DP_L1_*"  # Dispensing
            - "CO_L1_*"  # Curing
            - "TS_L1_*"  # Testing
            - "TM_L1_*"  # Taping
          properties:
            cycle_time: 0.3  # 秒/件
            batch_size: 1000
            wip_target: 5000  # Work In Process
```

### 3. LNG 發電廠 (LNG Power Plant)

#### 特徵

- **大型設備**: 燃氣渦輪機、發電機組
- **安全關鍵**: LNG 儲存和氣化系統
- **高效能**: 聯合循環發電
- **環保要求**: 排放控制系統
- **24/7 運行**: 高可靠性要求

#### FDL 設計模式

```yaml
# LNG 聯合循環發電廠範例
factory_design:
  metadata:
    name: "LNG Combined Cycle Power Plant"
    facility_type: "Power_Generation"
    capacity: 1000  # MW
    fuel_type: "LNG"
  
  buildings:
    - building_id: "Power_Block"
      type: "Industrial"
      footprint: 50000  # m²
  
  layout:
    - area: "Gas_Turbine_Area"
      building: "Power_Block"
      floor: "Ground"
      
      instances:
        # 燃氣渦輪發電機組
        - type: "Gas_Turbine_Generator"
          count: 2
          naming_prefix: "GTG_"
          manufacturer: "GE"
          model: "9HA.02"
          specifications:
            power_output: 470  # MW
            efficiency: 41     # %
            fuel_consumption: 9800  # kg/hr at full load
            nox_emission: 15   # ppm
            dimensions:
              length: 25  # m
              width: 12   # m
              height: 8   # m
          instance_params:
            load: 0  # MW
            status: "Standby"
        
        # 蒸汽渦輪發電機組
        - type: "Steam_Turbine_Generator"
          count: 1
          naming_prefix: "STG_"
          specifications:
            power_output: 250  # MW
            steam_pressure: 120  # bar
            steam_temperature: 565  # °C
        
        # 餘熱回收蒸汽產生器
        - type: "HRSG"  # Heat Recovery Steam Generator
          count: 2
          naming_prefix: "HRSG_"
          specifications:
            steam_production: 180  # ton/hr
            efficiency: 95  # %
    
    - area: "LNG_Storage_Area"
      building: "Storage_Facility"
      floor: "Ground"
      safety_zone: 200  # m radius
      
      instances:
        # LNG 儲槽
        - type: "LNG_Storage_Tank"
          count: 2
          naming_prefix: "LNG_Tank_"
          specifications:
            capacity: 180000  # m³
            design_temperature: -162  # °C
            design_pressure: 0.25  # bar
            containment_type: "Full_Containment"
            insulation: "Perlite"
          safety_systems:
            - "Fire_Detection"
            - "Gas_Detection"
            - "Emergency_Shutdown"
            - "Deluge_System"
        
        # LNG 氣化器
        - type: "LNG_Vaporizer"
          count: 4
          naming_prefix: "VAP_"
          specifications:
            capacity: 50  # ton/hr
            type: "Open_Rack_Vaporizer"
            heating_medium: "Seawater"
  
  utilities:
    - type: "cooling_water"
      specifications:
        source: "Seawater"
        flow_rate: 50000  # m³/hr
        temperature_rise: 10  # °C
      distribution:
        - equipment: "GTG_*"
          flow_rate: 15000  # m³/hr per unit
        - equipment: "STG_*"
          flow_rate: 20000  # m³/hr
    
    - type: "electrical_grid"
      specifications:
        voltage: 345000  # V (345 kV)
        frequency: 60    # Hz
        connection_type: "Double_Circuit"
```

### 4. NVIDIA GPU 數據中心 (GPU Data Center)

#### 特徵

- **高密度**: 高功率密度機櫃
- **液冷系統**: 直接液冷或浸沒式冷卻
- **高速網路**: InfiniBand/Ethernet 高速互連
- **AI 運算**: GPU 叢集配置
- **能源效率**: PUE < 1.2 目標

#### FDL 設計模式

```yaml
# NVIDIA DGX SuperPOD 數據中心範例
factory_design:
  metadata:
    name: "NVIDIA AI Data Center"
    facility_type: "GPU_Data_Center"
    total_gpu_count: 1024
    ai_performance: 10  # ExaFLOPS (FP8)
  
  buildings:
    - building_id: "Data_Center"
      type: "Tier_IV_Data_Center"
      total_area: 5000  # m²
      redundancy: "2N"
  
  layout:
    - area: "GPU_Pod_A"
      building: "Data_Center"
      floor: "1F"
      zone_type: "White_Space"
      raised_floor_height: 1.2  # m
      
      instances:
        # DGX H100 系統
        - type: "DGX_H100"
          count: 32  # 每個 Pod
          naming_prefix: "DGX_A_"
          specifications:
            gpus_per_node: 8  # H100 80GB
            gpu_memory: 640   # GB total
            system_memory: 2048  # GB
            storage: 30  # TB NVMe
            network:
              - "8x 400Gb/s InfiniBand"
              - "2x 100Gb/s Ethernet"
            power_consumption: 10.2  # kW per node
            dimensions:
              height: 10  # U (rack units)
              width: 19   # inch
              depth: 35   # inch
          layout_pattern: "rack"
          rack_config:
            racks: 8
            nodes_per_rack: 4
            rack_spacing: 1.2  # m
        
        # 網路交換機
        - type: "InfiniBand_Switch"
          count: 4
          naming_prefix: "IB_Switch_A_"
          specifications:
            ports: 64
            speed: 400  # Gb/s per port
            latency: 130  # ns
            power_consumption: 2.5  # kW
        
        # 儲存系統
        - type: "NVMe_Storage_Array"
          count: 2
          naming_prefix: "Storage_A_"
          specifications:
            capacity: 1000  # TB
            throughput: 100  # GB/s
            iops: 10000000  # 10M IOPS
      
      relationships:
        # 網路拓撲
        - type: "network_connection"
          topology: "Fat_Tree"
          from: "DGX_A_*"
          to: "IB_Switch_A_*"
          properties:
            bandwidth: 400  # Gb/s
            redundancy: "Active-Active"
        
        # 儲存連接
        - type: "storage_connection"
          from: "DGX_A_*"
          to: "Storage_A_*"
          properties:
            protocol: "NVMe-oF"
            bandwidth: 100  # GB/s
  
  utilities:
    - type: "power_distribution"
      specifications:
        total_capacity: 2000  # kW per pod
        voltage: 480  # V
        ups_capacity: 2000  # kW
        ups_runtime: 15  # minutes
        backup_generator: true
        generator_capacity: 3000  # kW
    
    - type: "cooling_system"
      specifications:
        cooling_method: "Direct_Liquid_Cooling"
        coolant: "Water"
        supply_temperature: 18  # °C
        return_temperature: 28  # °C
        flow_rate: 500  # L/min per pod
        redundancy: "N+1"
        pue_target: 1.15
      distribution:
        - equipment: "DGX_A_*"
          cooling_capacity: 10  # kW per node
          coolant_flow: 15  # L/min per node
```

### 5. Smart City 基礎設施 (Smart City)

#### 特徵

- **分散式**: 多個地點分散部署
- **IoT 整合**: 大量感測器和裝置
- **公共服務**: 交通、照明、環境監測
- **數據驅動**: 即時數據分析和決策
- **可擴展**: 模組化擴展

#### FDL 設計模式

```yaml
# Smart City 基礎設施範例
factory_design:
  metadata:
    name: "Smart City Infrastructure"
    facility_type: "Smart_City"
    coverage_area: 100  # km²
    population: 500000
  
  buildings:
    - building_id: "City_Operations_Center"
      type: "Control_Center"
      location: "City_Center"
  
  layout:
    # 智慧交通系統
    - area: "Traffic_Management_Zone_1"
      zone_type: "Public_Infrastructure"
      coverage:
        district: "Downtown"
        area: 10  # km²
      
      instances:
        # 智慧紅綠燈
        - type: "Smart_Traffic_Light"
          count: 150
          naming_prefix: "TL_DT_"
          specifications:
            led_power: 50  # W
            camera_resolution: "4K"
            ai_processing: true
          features:
            - "Vehicle_Detection"
            - "Pedestrian_Detection"
            - "Adaptive_Timing"
            - "Emergency_Vehicle_Priority"
          connectivity:
            protocol: "5G"
            update_interval: 1  # 秒
        
        # 交通監控攝影機
        - type: "Traffic_Camera"
          count: 300
          naming_prefix: "TC_DT_"
          specifications:
            resolution: "4K"
            frame_rate: 30  # fps
            night_vision: true
            ptz: true  # Pan-Tilt-Zoom
          ai_capabilities:
            - "License_Plate_Recognition"
            - "Vehicle_Classification"
            - "Traffic_Flow_Analysis"
            - "Incident_Detection"
        
        # 環境感測器
        - type: "Environmental_Sensor"
          count: 100
          naming_prefix: "ES_DT_"
          measurements:
            - name: "PM2.5"
              unit: "μg/m³"
              range: [0, 500]
            - name: "Temperature"
              unit: "°C"
              range: [-20, 50]
            - name: "Humidity"
              unit: "%"
              range: [0, 100]
            - name: "Noise_Level"
              unit: "dB"
              range: [30, 120]
    
    # 智慧照明系統
    - area: "Street_Lighting_Zone_1"
      zone_type: "Public_Infrastructure"
      coverage:
        district: "Downtown"
        street_length: 50  # km
      
      instances:
        # 智慧路燈
        - type: "Smart_Street_Light"
          count: 2000
          naming_prefix: "SL_DT_"
          specifications:
            led_power: 150  # W
            luminous_flux: 15000  # lm
            color_temperature: 4000  # K
            dimming_range: [10, 100]  # %
          features:
            - "Motion_Detection"
            - "Adaptive_Dimming"
            - "Energy_Monitoring"
            - "Fault_Detection"
          connectivity:
            protocol: "LoRaWAN"
            update_interval: 60  # 秒
          energy_saving:
            default_level: 70  # %
            motion_level: 100  # %
            motion_timeout: 300  # 秒
    
    # 智慧停車系統
    - area: "Parking_Management_Zone_1"
      zone_type: "Public_Infrastructure"
      coverage:
        district: "Downtown"
        parking_spaces: 5000
      
      instances:
        # 停車感測器
        - type: "Parking_Sensor"
          count: 5000
          naming_prefix: "PS_DT_"
          specifications:
            detection_method: "Ultrasonic"
            accuracy: 99  # %
            battery_life: 5  # years
          connectivity:
            protocol: "LoRaWAN"
            update_interval: 30  # 秒
        
        # 停車場管理系統
        - type: "Parking_Management_System"
          count: 20
          naming_prefix: "PMS_DT_"
          features:
            - "License_Plate_Recognition"
            - "Payment_Processing"
            - "Guidance_Display"
            - "Mobile_App_Integration"
  
  utilities:
    # 通訊網路
    - type: "communication_network"
      specifications:
        technologies:
          - "5G"
          - "LoRaWAN"
          - "NB-IoT"
        coverage: 100  # %
        base_stations: 50
    
    # 數據中心
    - type: "edge_data_center"
      specifications:
        locations: 5
        total_capacity: 500  # kW
        storage: 1000  # TB
        ai_processing: true
    
    # 能源管理
    - type: "energy_management"
      specifications:
        renewable_energy: 30  # %
        energy_storage: 10  # MWh
        smart_grid: true
```

---

## 實際應用案例

### 案例 1: 宏齊科技 LED 封裝廠

#### 專案背景

- **公司**: 宏齊科技 (Harvatek Corporation)
- **產業**: LED 封裝
- **規模**: 340 台生產設備, 1,600+ 感測器
- **挑戰**: 多產品混線生產, 即時監控需求

#### FDL 應用

完整的 FDL 配置文件定義了:

1. **6 層建築結構**: B1 至 5F
2. **20 個生產區域**: 2F 和 3F 的詳細佈局
3. **340 台設備實例**: 8 種設備類型
4. **完整的公用系統**: HVAC, 純水, 氮氣, 壓縮空氣, 電力

#### 實施效益

- **部署時間**: 從 6 個月縮短到 2 個月
- **設計錯誤**: 減少 80%
- **變更管理**: 版本控制, 快速迭代
- **ROI**: 6.2 個月回收期

#### FDL 片段

```yaml
# Harvatek 2F 標準 LED 生產線
layout:
  - area: "Standard_LED_Line_1"
    building: "MainBuilding"
    floor: "2F"
    zone_type: "CleanRoom"
    cleanliness_class: "ISO_Class_7"
    production_capacity: 1000000  # units/day
    
    instances:
      - type: "DieSorter_v1.0"
        count: 10
        throughput_total: 120000  # UPH
      - type: "DieBonder_v1.0"
        count: 8
        throughput_total: 64000  # UPH
      # ... 其他設備
```

### 案例 2: 台積電 5nm 晶圓廠 (概念設計)

#### 專案背景

- **技術節點**: 5nm
- **產能**: 100,000 片/月 (12 吋晶圓)
- **投資**: USD 20B
- **挑戰**: 極高潔淨度, EUV 微影, 複雜 MEP

#### FDL 應用

```yaml
# TSMC 5nm Fab 微影區
layout:
  - area: "EUV_Lithography_Bay"
    building: "Fab_12"
    floor: "1F"
    zone_type: "CleanRoom"
    cleanliness_class: "ISO_Class_1"
    vibration_criteria: "VC-E"  # 最嚴格的振動標準
    
    instances:
      - type: "ASML_EUV_NXE3400C"
        count: 20
        cost_total: 3000000000  # USD 3B
        specifications:
          wavelength: 13.5  # nm
          throughput: 170  # WPH
          overlay: 1.5  # nm
          power: 1000  # kW per machine
        
        supporting_equipment:
          - type: "Reticle_Storage_Pod"
            count: 20
          - type: "Wafer_Sorter"
            count: 10
          - type: "Metrology_Station"
            count: 5
```

### 案例 3: LNG 發電廠

#### 專案背景

- **容量**: 1,000 MW
- **效率**: 60% (聯合循環)
- **投資**: USD 1B
- **挑戰**: LNG 安全, 高效率, 環保

#### FDL 應用

```yaml
# LNG 發電廠主要設備配置
layout:
  - area: "Power_Generation_Block"
    instances:
      - type: "GE_9HA_Gas_Turbine"
        count: 2
        power_output: 940  # MW total
        efficiency: 41  # %
      
      - type: "Steam_Turbine"
        count: 1
        power_output: 250  # MW
      
      - type: "HRSG"
        count: 2
        steam_production: 360  # ton/hr total
    
    relationships:
      - type: "thermal_flow"
        from: "GE_9HA_Gas_Turbine"
        to: "HRSG"
        properties:
          exhaust_temperature: 650  # °C
          exhaust_flow: 700  # kg/s
      
      - type: "steam_flow"
        from: "HRSG"
        to: "Steam_Turbine"
        properties:
          pressure: 120  # bar
          temperature: 565  # °C
```

---

## 最佳實踐指南

### 1. 命名規範

#### 資產命名模式

```yaml
# 推薦的命名模式
naming_pattern: "{type}_{location}_{sequence}"

# 範例
examples:
  - "DS_2F_A_001"  # Die Sorter, 2F, Zone A, #001
  - "WB_3F_B_012"  # Wire Bonder, 3F, Zone B, #012
  - "AHU_2F_001"   # AHU, 2F, #001

# 命名規則
rules:
  type:
    format: "2-4 字母縮寫"
    case: "大寫"
  location:
    format: "樓層_區域"
    examples: ["2F_A", "3F_B", "B1"]
  sequence:
    format: "3 位數字"
    padding: "前導零"
    range: "001-999"
```

#### 區域命名模式

```yaml
# 區域命名規範
area_naming:
  pattern: "{purpose}_{zone_type}_{identifier}"
  
  examples:
    - "Production_Zone_A"
    - "Cleanroom_Area_1"
    - "Utility_Room_B1"
  
  guidelines:
    - "使用描述性名稱"
    - "避免特殊字元"
    - "保持一致性"
    - "考慮國際化"
```

### 2. 模組化設計

#### 可重用模組

```yaml
# 定義可重用的生產線模組
modules:
  - module_id: "Standard_LED_Production_Line"
    version: "1.0"
    description: "標準 LED 生產線配置"
    
    equipment_list:
      - type: "DieSorter_v1.0"
        count: 10
      - type: "DieBonder_v1.0"
        count: 8
      - type: "WireBonder_v1.0"
        count: 12
      - type: "Dispenser_v1.0"
        count: 6
      - type: "Tester_v1.0"
        count: 15
      - type: "TapingMachine_v1.0"
        count: 8
    
    footprint:
      length: 50  # m
      width: 30   # m
    
    utilities_required:
      power: 500  # kW
      compressed_air: 100  # Nm³/hr
      nitrogen: 50  # Nm³/hr
      di_water: 5  # m³/day
    
    performance:
      throughput: 50000  # units/hr
      oee_target: 85  # %

# 在 FDL 中引用模組
layout:
  - area: "Production_Floor_2F"
    modules:
      - module_id: "Standard_LED_Production_Line"
        instance_name: "Line_1"
        location:
          x: 10
          y: 20
          z: 0
      
      - module_id: "Standard_LED_Production_Line"
        instance_name: "Line_2"
        location:
          x: 10
          y: 60
          z: 0
```

### 3. 參數化配置

#### 使用變數和參數

```yaml
# 定義全域變數
variables:
  cleanroom_temperature: &cleanroom_temp 23
  cleanroom_humidity: &cleanroom_humid 45
  production_shift_hours: &shift_hours 8

# 在配置中引用
layout:
  - area: "Zone_A"
    temperature: *cleanroom_temp
    humidity: *cleanroom_humid
    working_hours: *shift_hours
  
  - area: "Zone_B"
    temperature: *cleanroom_temp
    humidity: *cleanroom_humid
    working_hours: *shift_hours

# 使用參數化模板
templates:
  production_zone:
    parameters:
      - name: "zone_name"
        type: "string"
      - name: "equipment_count"
        type: "integer"
      - name: "temperature"
        type: "float"
        default: 23
    
    layout:
      area: "{{zone_name}}"
      temperature: "{{temperature}}"
      instances:
        - type: "Equipment_Type"
          count: "{{equipment_count}}"
```

### 4. 版本控制

#### 版本管理策略

```yaml
# FDL 版本資訊
metadata:
  version: "2.1.3"  # 主版本.次版本.修訂版本
  version_history:
    - version: "2.1.3"
      date: "2025-10-12"
      author: "C.C. Lin"
      changes:
        - "新增 Mini LED 生產線"
        - "更新 AHU 配置"
        - "修正設備命名"
    
    - version: "2.1.2"
      date: "2025-09-15"
      author: "C.C. Lin"
      changes:
        - "調整 2F 設備佈局"
        - "優化氮氣管路"
    
    - version: "2.1.0"
      date: "2025-08-01"
      author: "C.C. Lin"
      changes:
        - "新增 3F 高功率 LED 產線"
        - "擴充電力系統"

# 版本控制最佳實踐
version_control:
  guidelines:
    - "使用語義化版本 (Semantic Versioning)"
    - "主版本: 不相容的變更"
    - "次版本: 向後相容的新功能"
    - "修訂版本: 向後相容的錯誤修正"
    - "記錄所有變更"
    - "使用 Git 進行版本控制"
```

### 5. 驗證與測試

#### FDL 驗證規則

```yaml
# FDL 驗證配置
validation:
  # 語法驗證
  syntax:
    - "YAML 格式正確"
    - "必要欄位存在"
    - "資料類型正確"
  
  # 語義驗證
  semantic:
    - "資產類型存在於 IADL 庫"
    - "建築物和樓層引用正確"
    - "關係引用的資產存在"
    - "座標在合理範圍內"
  
  # 物理驗證
  physical:
    - "設備不重疊"
    - "設備在樓層範圍內"
    - "通道寬度足夠"
    - "承重符合要求"
  
  # 公用系統驗證
  utilities:
    - "電力容量足夠"
    - "冷卻容量足夠"
    - "管線路徑可行"
    - "壓力和流量符合要求"

# 測試案例
test_cases:
  - name: "基本語法測試"
    input: "test_basic.fdl"
    expected: "pass"
  
  - name: "大規模工廠測試"
    input: "test_large_factory.fdl"
    expected: "pass"
    performance:
      max_parse_time: 10  # 秒
      max_memory: 1000    # MB
  
  - name: "錯誤處理測試"
    input: "test_invalid.fdl"
    expected: "fail"
    error_type: "ValidationError"
```

### 6. 文檔化

#### FDL 文檔結構

```yaml
# 內嵌文檔
documentation:
  overview: |
    這是 Harvatek LED 封裝廠的完整 FDL 配置。
    包含 2F 和 3F 的所有生產設備和公用系統。
  
  sections:
    - name: "2F 標準 LED 產線"
      description: |
        2F 配置了 3 條標準 LED 生產線,
        年產能 10 億顆。
      equipment_count: 120
      daily_capacity: 3000000
    
    - name: "3F 高功率 LED 產線"
      description: |
        3F 配置了 2 條高功率 LED 生產線,
        年產能 5 億顆。
      equipment_count: 80
      daily_capacity: 1500000
  
  diagrams:
    - name: "2F Layout"
      file: "diagrams/2f_layout.png"
    - name: "Utility Distribution"
      file: "diagrams/utility_distribution.png"
  
  references:
    - "IADL Specification v3.1"
    - "NDH API Documentation v2.0"
    - "Factory Designer User Guide"
```

---

## 擴展與客製化

### 自定義資產類型

```yaml
# 定義客製化資產類型
custom_asset_types:
  - type_id: "Custom_LED_Sorter_v2.0"
    base_type: "DieSorter_v1.0"  # 繼承基礎類型
    custom_properties:
      ai_vision: true
      multi_bin: 16
      throughput: 15000  # UPH
    custom_behaviors:
      - "auto_calibration"
      - "predictive_maintenance"
```

### 產業特定擴展

```yaml
# 半導體產業擴展
semiconductor_extensions:
  wafer_size: 12  # inch
  technology_node: 5  # nm
  process_steps: 1000
  
  equipment_categories:
    - "Lithography"
    - "Etching"
    - "Deposition"
    - "CMP"
    - "Metrology"

# LED 產業擴展
led_extensions:
  product_types:
    - "Standard_LED"
    - "High_Power_LED"
    - "Mini_LED"
    - "Micro_LED"
  
  wavelength_range: [380, 780]  # nm
  luminous_intensity_range: [1, 10000]  # mcd
```

---

## 總結與展望

### 總結

FDL (Factory Design Language) 作為 IDTF V3.3 生態系統的核心組件,提供了一個標準化、可重用且易於維護的工廠設計描述方式。本白皮書基於多個實際專案的經驗,歸納整理出:

1. **完整的語法規範**: 涵蓋元數據、建築、佈局、公用系統和參數
2. **多種廠房類型**: 半導體、LED、發電廠、數據中心、智慧城市
3. **實際應用案例**: 宏齊科技、台積電、LNG 發電廠等
4. **最佳實踐指南**: 命名規範、模組化設計、版本控制、驗證測試

### 核心價值

- **標準化**: 統一的描述語言,跨產業適用
- **可重用性**: 模組化設計,快速複製
- **可維護性**: 版本控制,變更追蹤
- **自動化**: 與 Factory Designer 和 NDH 無縫整合
- **可擴展性**: 支援客製化和產業特定擴展

### 未來展望

1. **AI 輔助設計**: 使用 AI 自動生成和優化 FDL
2. **數位分身整合**: 與 Omniverse 深度整合,即時視覺化
3. **多廠管理**: 支援集團級多工廠統一管理
4. **雲端協作**: 雲端 FDL 編輯和版本控制
5. **標準化推廣**: 推動 FDL 成為產業標準

### 下一步行動

1. **實施 FDL**: 在新專案中採用 FDL
2. **建立模板庫**: 累積可重用的 FDL 模板
3. **工具開發**: 開發 FDL 編輯器和驗證工具
4. **社群建設**: 建立 FDL 使用者社群
5. **標準制定**: 參與產業標準制定

---

## 參考資料

1. IDTF V3.3 技術白皮書 - https://github.com/chchlin1018/idtf-v3.3
2. Factory Designer 設計文件 - IDTF-V3.3-04
3. IADL V3.1 規範 - IDTF-V3.3-02
4. NDH 架構設計 - IDTF-V3.3-06
5. Harvatek Digital Twins 專案 - https://github.com/chchlin1018/harvatek-digital-twin
6. YAML 1.2 規範 - https://yaml.org/spec/1.2/spec.html
7. Semantic Versioning 2.0.0 - https://semver.org/

---

**版權聲明**: © 2025 IDTF Consortium. 本文件採用 CC BY-SA 4.0 授權。

**聯絡資訊**:  
Email: chchlin1018@gmail.com  
GitHub: https://github.com/chchlin1018

---

**FDL - 讓工廠設計標準化、自動化、智能化!** 🏭🚀


_

---

## 9. 分散式部署與 Worker 分配

在大型或跨廠區的部署場景中，NDH (Neutral Data Hub) 通常會以分散式叢集的方式運行。為了優化效能、降低延遲並實現地理位置感知，FDL 提供了 `worker_assignment` 機制，允許在設計階段就為資產實例指定執行的 **Worker 節點**。

### 9.1 Worker 分配提示 (Assignment Hints)

通過在資產的 `instance_params` 中加入特定的 `hints`，可以指導 NDH 的調度器將 Asset Servant 部署到最合適的 Worker 節點上。

**核心提示欄位**：

| 欄位 | 類型 | 描述 | 範例 |
|---|---|---|---|
| `region` | string | 資產所在的地理區域或廠區。用於將 Servant 分配到對應的 Edge NDH 節點。 | `TW`, `US-West`, `FAB-A` |
| `building` | string | 資產所在的建築物。 | `Main-Building`, `Office-Tower` |
| `floor` | integer | 資產所在的樓層。 | `1`, `5`, `-1` |
| `worker_group` | string | 指定一個特定的 Worker 群組。可用於基於資產類型或重要性進行分組。 | `mep`, `production-critical`, `hvac` |
| `affinity` | object | 定義親和性規則，讓某些 Servants 盡量部署在同一個 Worker 上。 | `{ key: "robot_arm_group", value: "group1" }` |
| `anti_affinity` | object | 定義反親和性規則，確保某些 Servants 分散在不同的 Worker 上以提高可用性。 | `{ key: "safety_monitor", value: "true" }` |

### 9.2 FDL 範例

以下範例展示如何在 FDL 中使用 Worker 分配提示：

```yaml
factory_design:
  metadata:
    name: "Global Multi-Site Factory"

  ndh_cluster_config:  # 定義 NDH 叢集拓撲
    mode: "hybrid"
    edge_nodes:
      - id: "edge-tw"
        region: "TW"
      - id: "edge-us"
        region: "US"

    worker_groups:
      - name: "mep-workers"
        node_selector:
          role: "mep"
      - name: "prod-workers"
        node_selector:
          role: "production"

  layout:
    - area: "TW-FAB1-Floor1"
      instances:
        - type: "iadl://compressor_ga75.iadl"
          id: "comp-tw-001"
          instance_params:
            position: [10, 20, 0]
            hints:  # Worker 分配提示
              region: "TW"                # -> 分配到 edge-tw 節點
              building: "FAB1"
              floor: 1
              worker_group: "mep-workers" # -> 分配到 MEP 專用 Worker 群組

    - area: "US-FAB2-Floor2"
      instances:
        - type: "iadl://robot_arm_kr210.iadl"
          id: "robot-us-001"
          instance_params:
            position: [50, 60, 0]
            hints:
              region: "US"                # -> 分配到 edge-us 節點
              worker_group: "prod-workers"
              affinity:
                key: "robot_cell"
                value: "cell_A"

        - type: "iadl://robot_arm_kr210.iadl"
          id: "robot-us-002"
          instance_params:
            position: [52, 60, 0]
            hints:
              region: "US"
              worker_group: "prod-workers"
              affinity:
                key: "robot_cell"
                value: "cell_A" # -> 與 robot-us-001 部署在同一個 Worker 上
```

### 9.3 與 NDH 調度器的互動

1.  **FDL 解析**: NDH 在啟動時解析 FDL 文件。
2.  **讀取提示**: 調度器讀取每個資產實例的 `hints`。
3.  **匹配 Worker**: 調度器根據提示（`region`, `worker_group` 等）和 Worker 節點的標籤 (`labels`)，選擇最匹配的 Worker。
4.  **動態調度**: 如果沒有靜態提示，或目標 Worker 不可用，調度器會根據負載均衡策略動態選擇一個 Worker。

這種機制使得工廠設計者可以在不關心底層 Kubernetes 細節的情況下，對分散式系統的行為進行高層次的指導，從而實現設計意圖與運行時行為的一致性。

