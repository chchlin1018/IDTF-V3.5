# IDTF 技術白皮書 (第二版)

**Industrial Digital Twins Framework - Technical Whitepaper**

**版本**: 2.0  
**日期**: 2025 年 1 月  
**作者**: IDTF 技術團隊  

## 4. 範本庫系統

### 4.1 兩層範本庫架構

```
Layer 2: Factory Design Template Library (工廠設計範本庫)
         ↓ 使用
Layer 1: Asset Template Library (資產範本庫)
```

### 4.2 Asset Template Library

**定義**: 標準化的工業資產範本庫

**範本數量**: 15+ 個核心範本

**範本類別**:

1. **泵 (Pumps)**:
   - 離心泵 (Centrifugal Pump)
   - 容積泵 (Positive Displacement Pump)
   - 真空泵 (Vacuum Pump)

2. **閥門 (Valves)**:
   - 控制閥 (Control Valve)
   - 開關閥 (On/Off Valve)
   - 安全閥 (Safety Valve)

3. **容器 (Vessels)**:
   - 壓力容器 (Pressure Vessel)
   - 儲槽 (Storage Tank)
   - 反應器 (Reactor)

4. **換熱器 (Heat Exchangers)**:
   - 管殼式換熱器 (Shell & Tube)
   - 板式換熱器 (Plate Heat Exchanger)
   - 空冷器 (Air Cooler)

5. **分析儀表 (Instruments)**:
   - 流量計 (Flow Meter)
   - 壓力變送器 (Pressure Transmitter)
   - 溫度變送器 (Temperature Transmitter)
   - 液位計 (Level Transmitter)

**範本結構**:
```yaml
template:
  id: "centrifugal_pump_v1"
  name: "Centrifugal Pump Template"
  version: "1.0"
  category: "pumps"
  
  parameters:
    - name: "flow_rate"
      type: "number"
      unit: "m3/h"
      required: true
    
    - name: "head"
      type: "number"
      unit: "m"
      required: true
  
  plc_tags_template:
    - name: "{asset_id}_RUN"
      data_type: "BOOL"
      description: "Running status"
    
    - name: "{asset_id}_FLOW"
      data_type: "REAL"
      unit: "m3/h"
      description: "Flow rate"
  
  scada_tags_template:
    - name: "{asset_name}_Status"
      alarm_enabled: true
      logging_enabled: true
  
  alarms:
    - name: "Low Flow"
      condition: "{asset_id}_FLOW < {flow_rate} * 0.5"
      priority: "HIGH"
  
  model_3d_template:
    usd_base_path: "/templates/pumps/centrifugal/"
```

### 4.3 Factory Design Template Library

**定義**: 行業區分的完整工廠設計範本庫

**範本數量**: 132 個範本 (11 個行業)

**行業分類**:

| 行業 | 工廠類型 | 規模變體 | 範本數 |
|------|----------|----------|--------|
| **半導體** | Fab 12吋, Fab 8吋, 後段封測 | 小/中/大 | 9 |
| **能源** | LNG, 太陽能, 風力, 儲能 | 小/中/大 | 12 |
| **LED** | 封裝 (SMD/Mini/Micro), 晶片 | 小/中/大 | 12 |
| **數據中心** | AI, 超大規模, 邊緣 | 小/中/大 | 9 |
| **化工** | 煉油, 石化, 精細化工 | 小/中/大 | 9 |
| **製藥** | 原料藥, 製劑, 生物製劑 | 小/中/大 | 9 |
| **食品飲料** | 啤酒, 乳品, 飲料 | 小/中/大 | 9 |
| **汽車** | 電池, 組裝, 塗裝 | 小/中/大 | 9 |
| **鋼鐵金屬** | 鋼鐵, 鋁冶煉, 金屬加工 | 小/中/大 | 9 |
| **紙漿造紙** | 紙漿, 造紙, 包裝 | 小/中/大 | 9 |
| **Smart City** | 交通, 能源, 建築, 水務等 | 小/中/大 | 36 |
| **總計** | | | **132** |

**範本結構**:
```yaml
factory_template:
  id: "led_packaging_medium_v1"
  name: "LED Packaging Factory (Medium)"
  version: "1.0"
  industry: "led"
  scale: "medium"
  
  overview:
    capacity: "50M units/month"
    equipment_count: 800
    monitoring_points: 15000
    floor_area: "10,000 m²"
  
  systems:
    - id: "die_bonding"
      name: "Die Bonding System"
      equipment_count: 200
      assets:
        - type: "die_bonder"
          template: "led/die_bonder"
          count: 200
    
    - id: "wire_bonding"
      name: "Wire Bonding System"
      equipment_count: 150
      assets:
        - type: "wire_bonder"
          template: "led/wire_bonder"
          count: 150
    
    - id: "packaging"
      name: "Packaging System"
      equipment_count: 100
      assets:
        - type: "molding_machine"
          template: "led/molding_machine"
          count: 100
  
  layout:
    zones:
      - name: "Clean Room"
        area: "8,000 m²"
        class: "ISO 7"
      
      - name: "Equipment Area"
        area: "1,500 m²"
      
      - name: "Warehouse"
        area: "500 m²"
  
  integration:
    erp: "SAP"
    mes: "Siemens Opcenter"
    scada: "Wonderware InTouch"
    plc: "Siemens S7-1500"
  
  kpis:
    oee_target: 0.75
