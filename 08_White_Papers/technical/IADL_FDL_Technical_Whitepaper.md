# IADL & FDL 技術白皮書
## 工業資產建模與工廠設計語言完整技術規範

**版本**: 3.5  
**發布日期**: 2025-01-14  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih Cheng Lin, Michael Lin)  
**組織**: MacroVision Systems / IDTF Consortium

---

## 文件摘要

本白皮書提供 IDTF (Industrial Digital Twin Framework) V3.5 生態系統中兩個核心語言的完整技術規範：

- **IADL (Industrial Asset Description Language)**：工業資產描述語言
- **FDL (Factory Design Language)**：工廠設計語言

以及對應的視覺化編輯工具：

- **IADL Editor**：資產建模工具
- **FDL Editor (Factory Designer)**：工廠設計工具

本文件涵蓋技術架構、語法規範、功能特性、介面設計、實際範例和最佳實踐，適合架構師、工程師、產品經理和技術決策者閱讀。

---

## 目錄

1. [引言與背景](#1-引言與背景)
2. [IADL 技術規範](#2-iadl-技術規範)
3. [IADL Editor 架構與功能](#3-iadl-editor-架構與功能)
4. [FDL 技術規範](#4-fdl-技術規範)
5. [FDL Editor (Factory Designer) 架構與功能](#5-fdl-editor-factory-designer-架構與功能)
6. [IADL 與 FDL 的協同工作流程](#6-iadl-與-fdl-的協同工作流程)
7. [實際應用案例](#7-實際應用案例)
8. [最佳實踐與設計模式](#8-最佳實踐與設計模式)
9. [技術優勢與競爭力分析](#9-技術優勢與競爭力分析)
10. [未來發展路線圖](#10-未來發展路線圖)

---

## 1. 引言與背景

### 1.1 工業數位分身的挑戰

在工業 4.0 時代，企業面臨著數位化轉型的迫切需求。然而，傳統的工業數位分身解決方案存在以下挑戰：

1. **供應商鎖定**：依賴特定廠商的專有格式和工具
2. **高昂成本**：商業軟體授權費用高達數十萬美元
3. **數據孤島**：IT/OT 系統無法有效整合
4. **缺乏標準**：不同系統之間無法互操作
5. **複雜度高**：需要專業團隊才能建置和維護

### 1.2 IDTF 的解決方案

IDTF V3.5 提供了一個**開源、中立、可負擔**的工業數位分身框架，核心理念包括：

- **開源中立**：無供應商鎖定，完全開放的技術標準
- **成本民主化**：相較商業方案節省 70% 成本
- **全生命週期**：從設計、建造到運維的完整覆蓋
- **IT/OT 融合**：統一整合企業應用和現場設備

### 1.3 IADL 與 FDL 的定位

在 IDTF 生態系統中，IADL 和 FDL 扮演著關鍵角色：

```
┌─────────────────────────────────────────────────────────┐
│                    IDTF V3.5 生態系統                    │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  設計階段 (Design Time - Offline)                        │
│  ┌─────────────┐         ┌─────────────┐               │
│  │    IADL     │ ──────→ │     FDL     │               │
│  │ 資產是什麼   │         │ 工廠如何佈局 │               │
│  └─────────────┘         └─────────────┘               │
│         ↓                        ↓                      │
│  ┌─────────────┐         ┌─────────────┐               │
│  │ IADL Editor │         │FDL Editor   │               │
│  │ 視覺化建模   │         │(Factory     │               │
│  │             │         │ Designer)   │               │
│  └─────────────┘         └─────────────┘               │
│                                 ↓                       │
│  執行階段 (Runtime - Online)                             │
│  ┌──────────────────────────────────┐                  │
│  │            NDH                    │                  │
│  │    (Neutral Data Hub)             │                  │
│  │  - 部署 FDL 定義的工廠佈局          │                  │
│  │  - 載入 IADL 定義的資產藍圖         │                  │
│  │  - 批次實例化 Asset Tag Instances       │                  │
│  │  - 即時數據採集與同步              │                  │
│  └──────────────────────────────────┘                  │
│                                 ↓                       │
│  ┌──────────────────────────────────┐                  │
│  │         Omniverse                 │                  │
│  │  - 3D 視覺化與協同                 │                  │
│  │  - 即時渲染與模擬                  │                  │
│  └──────────────────────────────────┘                  │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

**關鍵分工**：

| 組件 | 職責 | 階段 | 輸出 |
|------|------|------|------|
| **IADL** | 定義資產藍圖（What） | 設計階段 | .iadl 文件 |
| **IADL Editor** | 視覺化資產建模工具 | 設計階段 | .iadl 文件 |
| **FDL** | 定義工廠佈局（How） | 設計階段 | .fdl 文件 |
| **FDL Editor** | 視覺化工廠設計工具 | 設計階段 | .fdl 文件 + MEP 設計 |
| **NDH** | 執行時數據中樞 | 執行階段 | Asset Tag Instances + 即時數據 |
| **Omniverse** | 3D 視覺化與協同 | 設計+執行 | USD 場景 |

---

## 2. IADL 技術規範

### 2.1 IADL 概述

**IADL (Industrial Asset Description Language)** 是一種專為工業自動化領域設計的聲明式描述語言，用於定義工業資產的**藍圖**。

**核心理念**：
- **一次建模，到處使用**：定義一次，可在多個工廠專案中重用
- **供應商中立**：不依賴特定廠商的格式
- **全面描述**：涵蓋 3D 模型、數據標籤、行為邏輯、連接關係

### 2.2 IADL 語法結構

IADL 採用基於 YAML 的聲明式語法，具有清晰的階層結構：

#### 2.2.1 基本結構

```yaml
# IADL 基本結構
asset_definition:
  metadata:
    id: <資產唯一識別符>
    name: <資產名稱>
    version: <版本號>
    category: <資產類別>
    manufacturer: <製造商>
    description: <描述>
  
  geometry:
    model_uri: <3D 模型路徑>
    bounding_box: <邊界框>
    collision_mesh: <碰撞網格>
  
  properties:
    - name: <屬性名稱>
      type: <數據類型>
      default: <預設值>
      unit: <單位>
      range: <範圍>
  
  data_tags:
    - name: <標籤名稱>
      protocol: <通訊協議>
      address: <地址>
      data_type: <數據類型>
      access: <讀寫權限>
  
  behaviors:
    - name: <行為名稱>
      type: <行為類型>
      inputs: <輸入參數>
      outputs: <輸出參數>
      logic: <行為邏輯>
  
  connection_points:
    - name: <連接點名稱>
      type: <連接類型>
      position: <位置>
      compatible_types: <相容類型>
```

#### 2.2.2 完整範例：標準泵浦 (Standard Pump)

```yaml
asset_definition:
  metadata:
    id: "StandardPump_v1.0"
    name: "Standard Industrial Pump"
    version: "1.0.0"
    category: "Pump"
    manufacturer: "Generic"
    description: "標準工業用泵浦，適用於水循環系統"
    tags:
      - "HVAC"
      - "Utilities"
      - "Mechanical"
  
  geometry:
    model_uri: "models/pump_standard.usd"
    bounding_box:
      min: [-0.3, -0.3, 0.0]
      max: [0.3, 0.3, 0.6]
    collision_mesh: "models/pump_standard_collision.obj"
    visual_mesh: "models/pump_standard_visual.obj"
  
  properties:
    - name: "rated_power"
      type: "float"
      default: 15.0
      unit: "kW"
      range: [5.0, 30.0]
      description: "額定功率"
    
    - name: "rated_flow"
      type: "float"
      default: 100.0
      unit: "m³/h"
      range: [50.0, 200.0]
      description: "額定流量"
    
    - name: "rated_head"
      type: "float"
      default: 50.0
      unit: "m"
      range: [20.0, 100.0]
      description: "額定揚程"
    
    - name: "efficiency"
      type: "float"
      default: 0.85
      unit: "%"
      range: [0.7, 0.95]
      description: "效率"
    
    - name: "status"
      type: "enum"
      default: "Stopped"
      values: ["Running", "Stopped", "Fault", "Maintenance"]
      description: "運行狀態"
  
  data_tags:
    # OPC UA 標籤
    - name: "power_consumption"
      protocol: "OPC_UA"
      address: "ns=2;s=Pump.PowerConsumption"
      data_type: "float"
      access: "read"
      update_rate: 1000  # ms
      description: "實際功耗"
    
    - name: "flow_rate"
      protocol: "OPC_UA"
      address: "ns=2;s=Pump.FlowRate"
      data_type: "float"
      access: "read"
      update_rate: 1000
      description: "實際流量"
    
    - name: "outlet_pressure"
      protocol: "OPC_UA"
      address: "ns=2;s=Pump.OutletPressure"
      data_type: "float"
      access: "read"
      update_rate: 1000
      description: "出口壓力"
    
    - name: "motor_temperature"
      protocol: "OPC_UA"
      address: "ns=2;s=Pump.MotorTemperature"
      data_type: "float"
      access: "read"
      update_rate: 5000
      description: "馬達溫度"
    
    - name: "run_command"
      protocol: "OPC_UA"
      address: "ns=2;s=Pump.RunCommand"
      data_type: "boolean"
      access: "write"
      description: "啟動命令"
    
    - name: "stop_command"
      protocol: "OPC_UA"
      address: "ns=2;s=Pump.StopCommand"
      data_type: "boolean"
      access: "write"
      description: "停止命令"
  
  behaviors:
    - name: "Start"
      type: "command"
      inputs:
        - name: "target_flow"
          type: "float"
          unit: "m³/h"
      outputs:
        - name: "success"
          type: "boolean"
      logic: |
        if status == "Stopped":
          write_tag("run_command", true)
          wait_for_status("Running", timeout=30s)
          return true
        else:
          return false
    
    - name: "Stop"
      type: "command"
      outputs:
        - name: "success"
          type: "boolean"
      logic: |
        if status == "Running":
          write_tag("stop_command", true)
          wait_for_status("Stopped", timeout=30s)
          return true
        else:
          return false
    
    - name: "AdjustFlow"
      type: "command"
      inputs:
        - name: "target_flow"
          type: "float"
          unit: "m³/h"
      outputs:
        - name: "actual_flow"
          type: "float"
      logic: |
        # 調整變頻器頻率以達到目標流量
        frequency = calculate_frequency(target_flow)
        write_tag("vfd_frequency", frequency)
        wait(5s)
        return read_tag("flow_rate")
  
  connection_points:
    - name: "inlet"
      type: "pipe"
      position: [-0.2, 0.0, 0.3]
      direction: "in"
      compatible_types: ["pipe_DN50", "pipe_DN80", "pipe_DN100"]
      properties:
        nominal_diameter: 50  # mm
        connection_standard: "ANSI B16.5"
    
    - name: "outlet"
      type: "pipe"
      position: [0.2, 0.0, 0.3]
      direction: "out"
      compatible_types: ["pipe_DN50", "pipe_DN80", "pipe_DN100"]
      properties:
        nominal_diameter: 50  # mm
        connection_standard: "ANSI B16.5"
    
    - name: "power_input"
      type: "electrical"
      position: [0.0, -0.25, 0.4]
      direction: "in"
      compatible_types: ["3phase_380V", "3phase_440V"]
      properties:
        voltage: 380  # V
        phases: 3
        frequency: 50  # Hz
  
  maintenance:
    schedule:
      - type: "routine_inspection"
        interval: 720  # hours
        tasks:
          - "檢查軸承潤滑"
          - "檢查密封件"
          - "測量振動"
      
      - type: "major_overhaul"
        interval: 8760  # hours (1 year)
        tasks:
          - "更換軸承"
          - "更換密封件"
          - "檢查葉輪"
          - "馬達絕緣測試"
    
    spare_parts:
      - name: "軸承"
        part_number: "SKF-6308"
        quantity: 2
      - name: "機械密封"
        part_number: "MS-50"
        quantity: 1
```

### 2.3 IADL 數據類型系統

IADL 支援豐富的數據類型：

#### 2.3.1 基本類型

| 類型 | 說明 | 範例 |
|------|------|------|
| `string` | 字串 | "Pump_001" |
| `int` | 整數 | 42 |
| `float` | 浮點數 | 3.14 |
| `boolean` | 布林值 | true, false |
| `enum` | 列舉 | ["Running", "Stopped"] |
| `datetime` | 日期時間 | "2025-01-14T10:30:00Z" |

#### 2.3.2 複合類型

```yaml
# 3D 向量
struct Vector3:
  x: float
  y: float
  z: float

# 位置與旋轉
struct Transform:
  position: Vector3
  rotation: Vector3  # Euler angles
  scale: Vector3

# 邊界框
struct BoundingBox:
  min: Vector3
  max: Vector3

# 顏色
struct Color:
  r: float  # 0.0 - 1.0
  g: float
  b: float
  a: float  # alpha
```

#### 2.3.3 協議支援

IADL 支援多種工業通訊協議：

| 協議 | 用途 | 範例地址 |
|------|------|----------|
| **OPC UA** | 工業標準 | `ns=2;s=Device.Temperature` |
| **Modbus TCP** | PLC 通訊 | `40001` (Holding Register) |
| **MQTT** | IoT 設備 | `factory/zone1/sensor/temp` |
| **BACnet** | 樓宇自動化 | `Device:123,AI:1` |
| **S7** | Siemens PLC | `DB1.DBD0` |
| **EtherNet/IP** | Rockwell | `Program:MainProgram.Tag1` |

### 2.4 IADL 行為模型

IADL 支援三種行為類型：

#### 2.4.1 命令 (Command)

同步執行的操作，有明確的輸入和輸出：

```yaml
behaviors:
  - name: "MoveTo"
    type: "command"
    inputs:
      - name: "target_position"
        type: "Vector3"
      - name: "speed"
        type: "float"
        unit: "m/s"
        default: 1.0
    outputs:
      - name: "success"
        type: "boolean"
      - name: "actual_position"
        type: "Vector3"
    timeout: 60  # seconds
    logic: |
      # 移動邏輯
      send_command("move", target_position, speed)
      wait_for_completion()
      return (success, get_current_position())
```

#### 2.4.2 事件 (Event)

異步觸發的事件：

```yaml
behaviors:
  - name: "OnTemperatureExceeded"
    type: "event"
    trigger:
      condition: "motor_temperature > 80.0"
      debounce: 5000  # ms
    outputs:
      - name: "temperature"
        type: "float"
      - name: "timestamp"
        type: "datetime"
    actions:
      - type: "alarm"
        severity: "high"
        message: "馬達溫度過高"
      - type: "notify"
        recipients: ["operator@factory.com"]
```

#### 2.4.3 狀態機 (State Machine)

定義資產的狀態轉換：

```yaml
behaviors:
  - name: "OperationStateMachine"
    type: "state_machine"
    initial_state: "Idle"
    states:
      - name: "Idle"
        on_enter:
          - write_tag("status_led", "green")
        transitions:
          - event: "start_command"
            target: "Starting"
            condition: "no_faults"
      
      - name: "Starting"
        on_enter:
          - write_tag("run_command", true)
          - write_tag("status_led", "yellow")
        timeout: 30  # seconds
        transitions:
          - event: "speed_reached"
            target: "Running"
          - event: "timeout"
            target: "Fault"
      
      - name: "Running"
        on_enter:
          - write_tag("status_led", "green_blinking")
        transitions:
          - event: "stop_command"
            target: "Stopping"
          - event: "fault_detected"
            target: "Fault"
      
      - name: "Stopping"
        on_enter:
          - write_tag("stop_command", true)
        timeout: 30
        transitions:
          - event: "speed_zero"
            target: "Idle"
          - event: "timeout"
            target: "Fault"
      
      - name: "Fault"
        on_enter:
          - write_tag("status_led", "red")
          - trigger_alarm("設備故障")
        transitions:
          - event: "reset_command"
            target: "Idle"
            condition: "faults_cleared"
```

### 2.5 IADL 資產類別

IADL 定義了標準的資產類別層次：

```
Asset (抽象基類)
├── MechanicalAsset (機械資產)
│   ├── Pump (泵浦)
│   ├── Valve (閥門)
│   ├── Fan (風扇)
│   ├── Compressor (壓縮機)
│   └── Motor (馬達)
│
├── ElectricalAsset (電氣資產)
│   ├── Transformer (變壓器)
│   ├── Switchgear (開關設備)
│   ├── UPS (不斷電系統)
│   └── Generator (發電機)
│
├── InstrumentationAsset (儀表資產)
│   ├── Sensor (感測器)
│   │   ├── TemperatureSensor
│   │   ├── PressureSensor
│   │   └── FlowSensor
│   ├── Actuator (執行器)
│   └── Controller (控制器)
│
├── ProcessAsset (製程資產)
│   ├── Reactor (反應器)
│   ├── HeatExchanger (熱交換器)
│   ├── Distillation Column (蒸餾塔)
│   └── Tank (儲槽)
│
└── StructuralAsset (結構資產)
    ├── Pipe (管道)
    ├── Duct (風管)
    ├── Cable Tray (電纜架)
    └── Support Structure (支撐結構)
```

---

## 3. IADL Editor 架構與功能

### 3.1 IADL Editor 概述

**IADL Editor** 是一個基於 Qt6 的桌面應用程式，提供視覺化的資產建模介面，讓工程師無需手動編寫 YAML 代碼即可定義工業資產。

### 3.2 技術架構

```
┌─────────────────────────────────────────────────────────┐
│                   IADL Editor                            │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │  UI Layer    │  │ Business     │  │ Data Layer   │  │
│  │  (Qt6 QML)   │  │ Logic Layer  │  │              │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
│         │                  │                  │         │
│         ↓                  ↓                  ↓         │
│  ┌──────────────────────────────────────────────────┐  │
│  │            核心模組                               │  │
│  ├──────────────────────────────────────────────────┤  │
│  │ • Asset Template Manager (資產模板管理器)         │  │
│  │ • Property Editor (屬性編輯器)                    │  │
│  │ • Data Tag Configurator (數據標籤配置器)          │  │
│  │ • Behavior Designer (行為設計器)                  │  │
│  │ • 3D Model Viewer (3D 模型查看器)                 │  │
│  │ • IADL Parser & Validator (IADL 解析與驗證器)     │  │
│  │ • Export Engine (匯出引擎)                        │  │
│  └──────────────────────────────────────────────────┘  │
│                         │                               │
│                         ↓                               │
│  ┌──────────────────────────────────────────────────┐  │
│  │            外部整合                               │  │
│  ├──────────────────────────────────────────────────┤  │
│  │ • USD (Omniverse 3D 模型)                         │  │
│  │ • Git (版本控制)                                  │  │
│  │ • Factory Designer (工廠設計工具)                 │  │
│  │ • NDH (部署到執行環境)                            │  │
│  └──────────────────────────────────────────────────┘  │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### 3.3 核心功能

#### 3.3.1 資產模板庫

IADL Editor 內建豐富的資產模板庫：

**標準資產模板**：
- 泵浦 (Pump)：離心泵、螺桿泵、齒輪泵
- 閥門 (Valve)：球閥、蝶閥、閘閥、調節閥
- 感測器 (Sensor)：溫度、壓力、流量、液位
- 馬達 (Motor)：交流馬達、伺服馬達、步進馬達
- 儲槽 (Tank)：常壓槽、壓力槽、攪拌槽

**行業專用模板**：
- 半導體：CVD 設備、蝕刻機、CMP 設備
- LED 封裝：固晶機、焊線機、封裝機
- 發電廠：鍋爐、汽輪機、發電機、冷卻塔
- 數據中心：UPS、PDU、CRAC、冷水主機

#### 3.3.2 視覺化介面設計

**主介面佈局**：

```
┌─────────────────────────────────────────────────────────┐
│ File  Edit  View  Tools  Help                   [_][□][X]│
├─────────────────────────────────────────────────────────┤
│ ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐        │
│ │  New    │ │  Open   │ │  Save   │ │ Validate│        │
│ └─────────┘ └─────────┘ └─────────┘ └─────────┘        │
├──────────┬──────────────────────────────────┬───────────┤
│          │                                  │           │
│ Asset    │      3D Model Viewer             │ Property  │
│ Template │                                  │ Editor    │
│ Library  │      [3D Pump Model]             │           │
│          │                                  │ Name:     │
│ ├─Pump   │                                  │ [Pump_01] │
│ │├─Centri│                                  │           │
│ ││fugal   │                                  │ Category: │
│ │└─Screw  │                                  │ [Pump ▼]  │
│ ├─Valve   │                                  │           │
│ ├─Sensor  │                                  │ Power:    │
│ ├─Motor   │                                  │ [15.0 kW] │
│ └─Tank    │                                  │           │
│          │                                  │ Flow:     │
│          │                                  │ [100 m³/h]│
│          │                                  │           │
├──────────┴──────────────────────────────────┴───────────┤
│ Tabs: [Metadata] [Geometry] [Properties] [Data Tags]    │
│       [Behaviors] [Connection Points] [Maintenance]      │
├─────────────────────────────────────────────────────────┤
│ ┌─────────────────────────────────────────────────────┐ │
│ │ Data Tags Configuration                             │ │
│ ├─────────────────────────────────────────────────────┤ │
│ │ Tag Name        │ Protocol │ Address      │ Type    │ │
│ ├─────────────────┼──────────┼──────────────┼─────────┤ │
│ │ power_consumption│ OPC UA  │ ns=2;s=...   │ float   │ │
│ │ flow_rate       │ OPC UA   │ ns=2;s=...   │ float   │ │
│ │ outlet_pressure │ OPC UA   │ ns=2;s=...   │ float   │ │
│ └─────────────────┴──────────┴──────────────┴─────────┘ │
│ [Add Tag] [Edit Tag] [Delete Tag]                       │
├─────────────────────────────────────────────────────────┤
│ Status: Ready | Validation: ✓ Passed                    │
└─────────────────────────────────────────────────────────┘
```

#### 3.3.3 屬性編輯器

支援多種屬性類型的視覺化編輯：

**數值屬性**：
- 滑桿控制（帶範圍限制）
- 數值輸入框（帶單位轉換）
- 科學記號支援

**列舉屬性**：
- 下拉選單
- 單選按鈕組
- 標籤雲

**複合屬性**：
- 樹狀結構編輯器
- 表格編輯器
- JSON 編輯器（帶語法高亮）

#### 3.3.4 數據標籤配置器

**支援的協議**：

1. **OPC UA**：
   - 瀏覽伺服器節點樹
   - 拖放式標籤配置
   - 自動類型推斷

2. **Modbus TCP**：
   - 暫存器地址計算器
   - 功能碼選擇器
   - 數據格式轉換

3. **MQTT**：
   - 主題訂閱管理
   - QoS 設定
   - Payload 格式定義

**配置介面**：

```
┌─────────────────────────────────────────────────────────┐
│ Data Tag Configuration                                   │
├─────────────────────────────────────────────────────────┤
│ Tag Name: [power_consumption________________]            │
│                                                          │
│ Protocol: [OPC UA ▼]                                     │
│                                                          │
│ ┌─ OPC UA Settings ─────────────────────────────────┐   │
│ │ Server URL: [opc.tcp://192.168.1.100:4840_______] │   │
│ │                                                    │   │
│ │ Node ID: [ns=2;s=Pump.PowerConsumption__________] │   │
│ │          [Browse...]                               │   │
│ │                                                    │   │
│ │ ☑ Use Security                                     │   │
│ │   Security Policy: [Basic256Sha256 ▼]             │   │
│ │   Security Mode:   [SignAndEncrypt ▼]             │   │
│ │                                                    │   │
│ │ Update Rate: [1000] ms                             │   │
│ │ Deadband:    [0.1_] (absolute)                     │   │
│ └────────────────────────────────────────────────────┘   │
│                                                          │
│ Data Type: [float ▼]                                     │
│ Access:    [● Read  ○ Write  ○ Read/Write]               │
│                                                          │
│ Unit: [kW__]  Scaling: [1.0___]  Offset: [0.0___]        │
│                                                          │
│ [Test Connection] [Save] [Cancel]                        │
└─────────────────────────────────────────────────────────┘
```

#### 3.3.5 行為設計器

**視覺化狀態機設計器**：

```
┌─────────────────────────────────────────────────────────┐
│ Behavior Designer - State Machine                        │
├─────────────────────────────────────────────────────────┤
│ [Toolbox]                                                │
│ ┌─────────┐                                              │
│ │ ○ State │                                              │
│ │ → Trans │                                              │
│ │ ⚡ Event │                                              │
│ └─────────┘                                              │
│                                                          │
│ Canvas:                                                  │
│ ┌────────────────────────────────────────────────────┐  │
│ │                                                    │  │
│ │    ┌──────┐  start_command  ┌──────────┐          │  │
│ │    │ Idle │ ───────────────→ │ Starting │          │  │
│ │    └──────┘                  └──────────┘          │  │
│ │       ↑                            │                │  │
│ │       │                            │ speed_reached  │  │
│ │       │                            ↓                │  │
│ │       │                      ┌──────────┐          │  │
│ │       │  stop_command        │ Running  │          │  │
│ │       │ ←─────────────────── └──────────┘          │  │
│ │       │                            │                │  │
│ │       │                            │ fault_detected │  │
│ │       │                            ↓                │  │
│ │       │                      ┌──────────┐          │  │
│ │       │  reset_command       │  Fault   │          │  │
│ │       └───────────────────── └──────────┘          │  │
│ │                                                    │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ [Properties]                                             │
│ Selected: Running State                                  │
│ On Enter Actions:                                        │
│   1. write_tag("status_led", "green_blinking")           │
│   [Add Action]                                           │
│                                                          │
│ Transitions:                                             │
│   • stop_command → Stopping                              │
│   • fault_detected → Fault                               │
│   [Add Transition]                                       │
└─────────────────────────────────────────────────────────┘
```

#### 3.3.6 3D 模型整合

**支援的 3D 格式**：
- USD/USDA (Omniverse 原生格式)
- GLTF/GLB (Web 3D 標準)
- FBX (Autodesk 格式)
- OBJ (通用格式)
- STEP/IGES (CAD 格式)

**3D 查看器功能**：
- 軌道旋轉、平移、縮放
- 連接點視覺化標記
- 邊界框顯示
- 碰撞網格預覽
- 材質編輯

### 3.4 驗證與匯出

#### 3.4.1 即時驗證

IADL Editor 提供即時語法和語義驗證：

**語法驗證**：
- YAML 格式正確性
- 必填欄位檢查
- 數據類型匹配

**語義驗證**：
- 屬性範圍檢查
- 連接點相容性驗證
- 行為邏輯一致性檢查
- 循環依賴檢測

**驗證結果顯示**：

```
┌─────────────────────────────────────────────────────────┐
│ Validation Results                                       │
├─────────────────────────────────────────────────────────┤
│ ✓ Syntax: Passed                                         │
│ ✓ Metadata: Complete                                     │
│ ✓ Geometry: Valid USD file                               │
│ ✓ Properties: 5 properties defined                       │
│ ✓ Data Tags: 6 tags configured                           │
│ ⚠ Behaviors: 1 warning                                   │
│   └─ "Start" behavior timeout not specified (using 60s)  │
│ ✓ Connection Points: 3 points defined                    │
│                                                          │
│ Overall Status: ✓ Valid with 1 warning                   │
│                                                          │
│ [View Details] [Export Anyway] [Fix Warnings]            │
└─────────────────────────────────────────────────────────┘
```

#### 3.4.2 匯出選項

**匯出格式**：
1. **IADL YAML**：標準格式，用於 NDH 部署
2. **JSON**：用於 Web 應用程式
3. **USD**：用於 Omniverse 視覺化
4. **Documentation**：自動生成 Markdown 文檔

**匯出目標**：
- 本地檔案系統
- Git 儲存庫（自動提交）
- Factory Designer 資產庫
- NDH 資產註冊表

---

## 4. FDL 技術規範

### 4.1 FDL 概述

**FDL (Factory Design Language)** 是用於定義工廠佈局和配置的聲明式語言。它描述：
- 資產實例的數量和位置
- 資產之間的連接關係
- 公用系統的配置
- 區域和樓層的組織

**核心理念**：
- **一次設計，多處部署**：工廠設計可以複製到不同地點
- **參數化配置**：支援變數和模板，提高重用性
- **自動化部署**：FDL 可直接部署到 NDH，自動實例化資產

### 4.2 FDL 語法結構

#### 4.2.1 基本結構

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
      floors: <樓層列表>
  
  layout:
    - area: <區域名稱>
      building: <所屬建築物>
      floor: <所屬樓層>
      instances: <資產實例列表>
      relationships: <關係列表>
  
  utilities:
    - type: <公用系統類型>
      specifications: <規格>
  
  parameters:
    <全域參數定義>
```

#### 4.2.2 完整範例：LED 封裝廠

```yaml
factory_design:
  metadata:
    name: "Harvatek LED Packaging Factory"
    version: "2.0.0"
    author: "C.C. Lin"
    date: "2025-01-14"
    description: "宏齊科技 LED 封裝廠完整佈局設計"
    industry: "Electronics Manufacturing"
    location:
      country: "Taiwan"
      city: "Hsinchu"
      coordinates:
        latitude: 24.8138
        longitude: 120.9675
  
  buildings:
    - building_id: "MainBuilding"
      name: "主廠房"
      type: "Production"
      construction_year: 2020
      total_area: 50000  # m²
      floors:
        - floor_id: "1F"
          level: 1
          height: 5.0  # m
          area: 10000
          purpose: "Production & Office"
          cleanroom_class: "ISO 7"
        
        - floor_id: "2F"
          level: 2
          height: 4.0
          area: 10000
          purpose: "Production - Standard LED"
          cleanroom_class: "ISO 6"
        
        - floor_id: "3F"
          level: 3
          height: 4.0
          area: 10000
          purpose: "Production - High Power LED"
          cleanroom_class: "ISO 5"
  
  parameters:
    # 全域參數
    standard_pump_power: 15.0  # kW
    standard_pump_flow: 100.0  # m³/h
    chilled_water_supply_temp: 7.0  # °C
    chilled_water_return_temp: 12.0  # °C
    compressed_air_pressure: 7.0  # bar
  
  layout:
    # 1F - 冷卻水系統
    - area: "ChilledWaterPlant"
      building: "MainBuilding"
      floor: "1F"
      zone_type: "Utility"
      location: "x:0, y:0, z:0"
      
      instances:
        # 冷水主機
        - type: "WaterChiller"
          count: 3
          naming_prefix: "CH_"
          location_pattern: "grid"
          grid_params:
            rows: 1
            columns: 3
            spacing_x: 5.0
            spacing_y: 0.0
            origin: "x:10, y:20, z:0"
          initial_params:
            cooling_capacity: 1000  # kW
            chilled_water_flow: 172  # m³/h
            supply_temp: ${chilled_water_supply_temp}
            return_temp: ${chilled_water_return_temp}
            refrigerant: "R134a"
        
        # 冷卻水泵
        - type: "StandardPump"
          count: 6  # 3 運轉 + 3 備用
          naming_prefix: "CHWP_"
          location_pattern: "grid"
          grid_params:
            rows: 2
            columns: 3
            spacing_x: 3.0
            spacing_y: 2.0
            origin: "x:10, y:30, z:0"
          initial_params:
            rated_power: ${standard_pump_power}
            rated_flow: ${standard_pump_flow}
            rated_head: 50.0
            status: "Stopped"
          tags:
            duty: ["duty", "duty", "duty", "standby", "standby", "standby"]
        
        # 冷卻水塔
        - type: "CoolingTower"
          count: 3
          naming_prefix: "CT_"
          location_pattern: "line"
          line_params:
            start: "x:50, y:20, z:0"
            end: "x:50, y:35, z:0"
          initial_params:
            cooling_capacity: 1200  # kW
            water_flow: 206  # m³/h
            fan_power: 30  # kW
        
        # 膨脹槽
        - type: "ExpansionTank"
          count: 1
          naming_prefix: "ET_"
          location: "x:25, y:25, z:0"
          initial_params:
            volume: 5000  # L
            pressure: 2.0  # bar
      
      relationships:
        # 冷水主機 → 冷卻水泵
        - from_pattern: "CH_*"
          to_pattern: "CHWP_*"
          type: "pipe"
          properties:
            fluid: "chilled_water"
            diameter: 150  # mm
            material: "stainless_steel"
            insulation: true
        
        # 冷卻水泵 → 冷卻水塔
        - from_pattern: "CHWP_*"
          to_pattern: "CT_*"
          type: "pipe"
          properties:
            fluid: "cooling_water"
            diameter: 150
            material: "carbon_steel"
        
        # 冷卻水塔 → 冷水主機
        - from_pattern: "CT_*"
          to_pattern: "CH_*"
          type: "pipe"
          properties:
            fluid: "cooling_water"
            diameter: 150
            material: "carbon_steel"
    
    # 2F - 標準 LED 生產線
    - area: "StandardLEDProduction"
      building: "MainBuilding"
      floor: "2F"
      zone_type: "Production"
      cleanroom_class: "ISO 6"
      
      instances:
        # 固晶機
        - type: "DieBonder"
          count: 10
          naming_prefix: "DB_"
          location_pattern: "grid"
          grid_params:
            rows: 2
            columns: 5
            spacing_x: 4.0
            spacing_y: 3.0
            origin: "x:10, y:10, z:0"
          initial_params:
            accuracy: 0.01  # mm
            speed: 3000  # UPH (units per hour)
            bond_force: 50  # gf
        
        # 焊線機
        - type: "WireBonder"
          count: 20
          naming_prefix: "WB_"
          location_pattern: "grid"
          grid_params:
            rows: 4
            columns: 5
            spacing_x: 3.0
            spacing_y: 2.5
            origin: "x:30, y:10, z:0"
          initial_params:
            wire_diameter: 0.025  # mm
            loop_height: 0.15  # mm
            speed: 5000  # UPH
        
        # 封裝機
        - type: "Encapsulator"
          count: 5
          naming_prefix: "EP_"
          location_pattern: "line"
          line_params:
            start: "x:60, y:10, z:0"
            end: "x:60, y:25, z:0"
          initial_params:
            dispensing_accuracy: 0.05  # mm
            curing_temp: 150  # °C
            curing_time: 120  # seconds
        
        # 烤箱
        - type: "CuringOven"
          count: 3
          naming_prefix: "OV_"
          location_pattern: "line"
          line_params:
            start: "x:70, y:10, z:0"
            end: "x:70, y:20, z:0"
          initial_params:
            max_temp: 200  # °C
            capacity: 1000  # units
            heating_rate: 5  # °C/min
      
      relationships:
        # 固晶機 → 焊線機
        - from_pattern: "DB_*"
          to_pattern: "WB_*"
          type: "material_flow"
          properties:
            transport_method: "AGV"
            cycle_time: 300  # seconds
        
        # 焊線機 → 封裝機
        - from_pattern: "WB_*"
          to_pattern: "EP_*"
          type: "material_flow"
          properties:
            transport_method: "conveyor"
        
        # 封裝機 → 烤箱
        - from_pattern: "EP_*"
          to_pattern: "OV_*"
          type: "material_flow"
          properties:
            transport_method: "conveyor"
    
    # 3F - 高功率 LED 生產線
    - area: "HighPowerLEDProduction"
      building: "MainBuilding"
      floor: "3F"
      zone_type: "Production"
      cleanroom_class: "ISO 5"
      
      instances:
        # 高精度固晶機
        - type: "HighPrecisionDieBonder"
          count: 5
          naming_prefix: "HPDB_"
          location_pattern: "grid"
          grid_params:
            rows: 1
            columns: 5
            spacing_x: 5.0
            spacing_y: 0.0
            origin: "x:10, y:15, z:0"
          initial_params:
            accuracy: 0.005  # mm
            speed: 1500  # UPH
            bond_force: 100  # gf
            vision_system: true
        
        # 雷射焊線機
        - type: "LaserWireBonder"
          count: 10
          naming_prefix: "LWB_"
          location_pattern: "grid"
          grid_params:
            rows: 2
            columns: 5
            spacing_x: 4.0
            spacing_y: 3.0
            origin: "x:30, y:10, z:0"
          initial_params:
            laser_power: 50  # W
            wire_diameter: 0.050  # mm
            speed: 2000  # UPH
  
  utilities:
    # 冷卻水系統
    - type: "chilled_water"
      specifications:
        supply_temp: ${chilled_water_supply_temp}
        return_temp: ${chilled_water_return_temp}
        total_capacity: 3000  # kW
        pressure: 4.0  # bar
      distribution:
        main_header:
          diameter: 200  # mm
          material: "stainless_steel"
        branch_headers:
          - floor: "1F"
            diameter: 150
          - floor: "2F"
            diameter: 150
          - floor: "3F"
            diameter: 100
    
    # 壓縮空氣系統
    - type: "compressed_air"
      specifications:
        pressure: ${compressed_air_pressure}
        dew_point: -40  # °C
        filtration: "0.01 micron"
        total_flow: 1000  # Nm³/h
      distribution:
        compressor_room: "1F"
        main_header:
          diameter: 100
          material: "aluminum"
    
    # 純水系統
    - type: "pure_water"
      specifications:
        resistivity: 18.2  # MΩ·cm
        TOC: 5  # ppb
        particle_size: 0.2  # micron
        total_flow: 50  # m³/h
      distribution:
        purification_plant: "1F"
        main_header:
          diameter: 80
          material: "PVDF"
    
    # 氮氣系統
    - type: "nitrogen"
      specifications:
        purity: 99.999  # %
        pressure: 5.0  # bar
        dew_point: -60  # °C
        total_flow: 500  # Nm³/h
      distribution:
        generator_room: "1F"
        main_header:
          diameter: 50
          material: "stainless_steel"
```

### 4.3 FDL 佈局模式

FDL 支援多種資產佈局模式：

#### 4.3.1 網格佈局 (Grid)

```yaml
location_pattern: "grid"
grid_params:
  rows: 3
  columns: 4
  spacing_x: 5.0  # m
  spacing_y: 4.0  # m
  origin: "x:10, y:20, z:0"
```

生成 3×4 = 12 個資產實例，均勻分佈在網格上。

#### 4.3.2 線性佈局 (Line)

```yaml
location_pattern: "line"
line_params:
  start: "x:10, y:20, z:0"
  end: "x:50, y:20, z:0"
```

資產沿著直線均勻分佈。

#### 4.3.3 圓形佈局 (Circle)

```yaml
location_pattern: "circle"
circle_params:
  center: "x:30, y:30, z:0"
  radius: 10.0  # m
  start_angle: 0  # degrees
  end_angle: 360
```

資產沿著圓周分佈。

#### 4.3.4 自定義佈局 (Custom)

```yaml
location_pattern: "custom"
custom_locations:
  - "x:10.5, y:20.3, z:0.0"
  - "x:15.2, y:22.1, z:0.0"
  - "x:18.7, y:19.8, z:0.0"
```

手動指定每個資產的精確位置。

### 4.4 FDL 關係定義

FDL 支援多種資產間的關係類型：

#### 4.4.1 管道連接 (Pipe)

```yaml
relationships:
  - from: "Pump_001"
    to: "Tank_001"
    type: "pipe"
    properties:
      fluid: "water"
      diameter: 50  # mm
      material: "stainless_steel"
      insulation: true
      length: 15.5  # m (自動計算或手動指定)
```

#### 4.4.2 電氣連接 (Electrical)

```yaml
relationships:
  - from: "Transformer_001"
    to: "Motor_001"
    type: "electrical"
    properties:
      voltage: 380  # V
      current: 50  # A
      cable_type: "NYY 3x16mm²"
      length: 25.0  # m
```

#### 4.4.3 物料流 (Material Flow)

```yaml
relationships:
  - from: "DieBonder_001"
    to: "WireBonder_001"
    type: "material_flow"
    properties:
      transport_method: "AGV"
      cycle_time: 300  # seconds
      batch_size: 100  # units
```

#### 4.4.4 控制信號 (Control Signal)

```yaml
relationships:
  - from: "PLC_001"
    to: "Pump_001"
    type: "control_signal"
    properties:
      protocol: "EtherNet/IP"
      update_rate: 100  # ms
```

### 4.5 FDL 參數化與模板

FDL 支援參數化配置，提高重用性：

#### 4.5.1 全域參數

```yaml
parameters:
  # 公用系統參數
  chilled_water_supply_temp: 7.0
  chilled_water_return_temp: 12.0
  compressed_air_pressure: 7.0
  
  # 設備參數
  standard_pump_power: 15.0
  standard_pump_flow: 100.0
  
  # 佈局參數
  equipment_spacing_x: 5.0
  equipment_spacing_y: 4.0
```

#### 4.5.2 參數引用

```yaml
initial_params:
  supply_temp: ${chilled_water_supply_temp}
  rated_power: ${standard_pump_power}

grid_params:
  spacing_x: ${equipment_spacing_x}
  spacing_y: ${equipment_spacing_y}
```

#### 4.5.3 條件配置

```yaml
instances:
  - type: "Pump"
    count: ${if production_mode == "high" then 6 else 4}
    initial_params:
      power: ${if region == "US" then 20.0 else 15.0}
```

---

## 5. FDL Editor (Factory Designer) 架構與功能

### 5.1 Factory Designer 概述

**Factory Designer** (FDL Editor) 是一個基於 Web 的工廠設計工具，提供視覺化的工廠佈局設計介面，整合了 MEP 設計、衝突檢測、ISO 圖面生成等高級功能。

### 5.2 技術架構

```
┌─────────────────────────────────────────────────────────┐
│              Factory Designer (FDL Editor)               │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │  Frontend    │  │  Backend     │  │  Services    │  │
│  │  (React +    │  │  (Node.js +  │  │              │  │
│  │   Three.js)  │  │   FastAPI)   │  │              │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
│         │                  │                  │         │
│         ↓                  ↓                  ↓         │
│  ┌──────────────────────────────────────────────────┐  │
│  │            核心模組                               │  │
│  ├──────────────────────────────────────────────────┤  │
│  │ • Asset Library Manager (資產庫管理器)            │  │
│  │ • 3D Layout Designer (3D 佈局設計器)              │  │
│  │ • MEP Design Engine (MEP 設計引擎)                │  │
│  │ • Conflict Detection Engine (衝突檢測引擎)        │  │
│  │ • ISO Drawing Generator (ISO 圖面生成器)          │  │
│  │ • Fluid Analysis Engine (流體分析引擎)            │  │
│  │ • FDL Parser & Validator (FDL 解析與驗證器)       │  │
│  │ • Deployment Engine (部署引擎)                    │  │
│  └──────────────────────────────────────────────────┘  │
│                         │                               │
│                         ↓                               │
│  ┌──────────────────────────────────────────────────┐  │
│  │            外部整合                               │  │
│  ├──────────────────────────────────────────────────┤  │
│  │ • IADL Editor (資產定義導入)                      │  │
│  │ • NDH (部署到執行環境)                            │  │
│  │ • Omniverse (3D 視覺化與協同)                     │  │
│  │ • Git (版本控制)                                  │  │
│  │ • CAD Tools (AutoCAD, Revit, etc.)                │  │
│  └──────────────────────────────────────────────────┘  │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### 5.3 核心功能

#### 5.3.1 3D 佈局設計器

**主介面佈局**：

```
┌─────────────────────────────────────────────────────────┐
│ File  Edit  View  Design  Tools  Deploy  Help   [_][□][X]│
├─────────────────────────────────────────────────────────┤
│ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐        │
│ │ New │ │Open │ │Save │ │Undo │ │Redo │ │Check│        │
│ └─────┘ └─────┘ └─────┘ └─────┘ └─────┘ └─────┘        │
├──────────┬──────────────────────────────────┬───────────┤
│          │                                  │           │
│ Asset    │      3D Viewport                 │ Property  │
│ Library  │                                  │ Inspector │
│          │      [3D Factory Layout]         │           │
│ Search:  │                                  │ Selected: │
│ [____]   │                                  │ Pump_001  │
│          │                                  │           │
│ ├─HVAC   │      ┌─────┐                     │ Type:     │
│ │├─Pump  │      │     │   ┌─────┐           │ Standard  │
│ ││└─Std  │      │  P1 │───│ T1  │           │ Pump      │
│ │├─Chiller│     │     │   │     │           │           │
│ │└─Tower │      └─────┘   └─────┘           │ Location: │
│ ├─Elec   │                                  │ X: 10.5   │
│ ├─Process│      Tools:                      │ Y: 20.3   │
│ └─Struct │      [Select][Move][Rotate]      │ Z: 0.0    │
│          │      [Connect][Measure]          │           │
│          │                                  │ Status:   │
│          │      Layers:                     │ [Running] │
│          │      ☑ Equipment                 │           │
│          │      ☑ Piping                    │ [Edit...] │
│          │      ☑ Electrical                │           │
│          │      ☐ Structure                 │           │
├──────────┴──────────────────────────────────┴───────────┤
│ Tabs: [Layout] [MEP Design] [Conflict Check] [ISO Draw] │
├─────────────────────────────────────────────────────────┤
│ Status: 125 assets placed | 0 conflicts | Last saved: 2m│
└─────────────────────────────────────────────────────────┘
```

**3D 視口功能**：
- 軌道旋轉、平移、縮放
- 多視圖（俯視、側視、透視）
- 圖層管理（設備、管道、電氣、結構）
- 測量工具（距離、角度、面積）
- 截面視圖
- 渲染模式（線框、著色、真實）

#### 5.3.2 MEP 設計引擎

**參數化管道設計**：

Factory Designer 提供智能化的 MEP 設計功能：

**自動佈線**：
- 輸入起點和終點
- 自動計算最佳路徑（避開障礙物）
- 自動添加彎頭、三通、閥門
- 符合設計規範（最小彎曲半徑、坡度等）

**元件選型**：
- 根據流量自動計算管徑
- 根據壓降選擇泵浦
- 根據負載選擇電纜規格

**範例：冷卻水管道設計**：

```
┌─────────────────────────────────────────────────────────┐
│ MEP Design - Chilled Water Piping                        │
├─────────────────────────────────────────────────────────┤
│ From: Chiller_001 (Outlet)                               │
│ To:   AHU_001 (Inlet)                                    │
│                                                          │
│ Design Parameters:                                       │
│ ┌────────────────────────────────────────────────────┐  │
│ │ Flow Rate:    [100__] m³/h                         │  │
│ │ Fluid:        [Chilled Water ▼]                    │  │
│ │ Supply Temp:  [7.0__] °C                           │  │
│ │ Return Temp:  [12.0_] °C                           │  │
│ │ Max Velocity: [2.5__] m/s                          │  │
│ │ Max Pressure Drop: [50__] kPa                      │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ [Calculate] → Recommended Pipe Diameter: DN80            │
│                                                          │
│ Routing Options:                                         │
│ ○ Shortest Path                                          │
│ ● Avoid Obstacles (with clearance: [500_] mm)            │
│ ○ Follow Existing Pipe Rack                              │
│                                                          │
│ Components to Add:                                       │
│ ☑ Isolation Valves (at start and end)                    │
│ ☑ Balancing Valve                                        │
│ ☑ Pressure Gauge                                         │
│ ☑ Temperature Sensor                                     │
│ ☑ Drain Valve                                            │
│                                                          │
│ [Generate Route] [Preview] [Apply]                       │
└─────────────────────────────────────────────────────────┘
```

**自動生成結果**：
- 管道路徑（3D 模型）
- 材料清單（BOM）
- 壓降計算報告
- 施工圖（ISO 圖面）

#### 5.3.3 衝突檢測引擎

**3D 碰撞檢測**：

Factory Designer 使用 **GJK (Gilbert-Johnson-Keerthi)** 和 **EPA (Expanding Polytope Algorithm)** 演算法進行精確的 3D 碰撞檢測。

**檢測類型**：
1. **硬衝突 (Hard Clash)**：實體幾何重疊
2. **軟衝突 (Soft Clash)**：違反間隙要求（如維護空間）
3. **時間衝突 (4D Clash)**：施工階段的時間衝突

**衝突檢測介面**：

```
┌─────────────────────────────────────────────────────────┐
│ Conflict Detection                                       │
├─────────────────────────────────────────────────────────┤
│ Detection Settings:                                      │
│ ┌────────────────────────────────────────────────────┐  │
│ │ Clash Type:                                        │  │
│ │ ☑ Hard Clash (geometry overlap)                    │  │
│ │ ☑ Soft Clash (clearance violation)                 │  │
│ │   Min Clearance: [500_] mm                         │  │
│ │ ☐ 4D Clash (schedule conflict)                     │  │
│ │                                                    │  │
│ │ Layers to Check:                                   │  │
│ │ ☑ Equipment vs Equipment                           │  │
│ │ ☑ Equipment vs Piping                              │  │
│ │ ☑ Piping vs Electrical                             │  │
│ │ ☑ All vs Structure                                 │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ [Run Detection]                                          │
│                                                          │
│ Results: 12 conflicts found                              │
│ ┌────────────────────────────────────────────────────┐  │
│ │ ID │ Type │ Object 1    │ Object 2    │ Severity  │  │
│ ├────┼──────┼─────────────┼─────────────┼───────────┤  │
│ │ 1  │ Hard │ Pipe_DN100  │ Beam_B12    │ Critical  │  │
│ │ 2  │ Soft │ Pump_001    │ Wall_W3     │ High      │  │
│ │ 3  │ Hard │ Cable_C45   │ Pipe_DN50   │ Critical  │  │
│ │ 4  │ Soft │ Chiller_001 │ Door_D7     │ Medium    │  │
│ └────┴──────┴─────────────┴─────────────┴───────────┘  │
│                                                          │
│ [Zoom to Conflict] [Generate Report] [Export]            │
└─────────────────────────────────────────────────────────┘
```

**衝突視覺化**：
- 紅色高亮顯示衝突區域
- 透明度調整以查看內部衝突
- 自動相機定位到衝突位置
- 衝突詳細信息標註

**衝突解決建議**：
- 自動建議替代路徑
- 調整設備位置
- 修改管道標高
- 重新選型（更小尺寸）

#### 5.3.4 ISO 圖面生成器

**自動生成 ISO 圖面**：

從 3D 模型自動生成符合 ISO 標準的 2D 工程圖：

**支援的圖面類型**：
1. **平面圖 (Plan View)**：俯視圖
2. **立面圖 (Elevation)**：側視圖
3. **剖面圖 (Section)**：截面視圖
4. **等角圖 (Isometric)**：管道等角圖
5. **詳圖 (Detail)**：局部放大圖

**ISO 圖面生成介面**：

```
┌─────────────────────────────────────────────────────────┐
│ ISO Drawing Generator                                    │
├─────────────────────────────────────────────────────────┤
│ Drawing Type: [Piping Isometric ▼]                       │
│                                                          │
│ Scope:                                                   │
│ ○ Entire Factory                                         │
│ ● Selected System: [Chilled Water System ▼]              │
│ ○ Custom Selection                                       │
│                                                          │
│ Drawing Settings:                                        │
│ ┌────────────────────────────────────────────────────┐  │
│ │ Scale:        [1:50 ▼]                             │  │
│ │ Paper Size:   [A1 ▼]                               │  │
│ │ Orientation:  [Landscape ▼]                        │  │
│ │                                                    │  │
│ │ Annotations:                                       │  │
│ │ ☑ Dimensions                                       │  │
│ │ ☑ Elevations                                       │  │
│ │ ☑ Component Labels                                 │  │
│ │ ☑ Material Specifications                          │  │
│ │ ☑ Weld Symbols                                     │  │
│ │                                                    │  │
│ │ Title Block:                                       │  │
│ │ Project:  [Harvatek LED Factory_______________]    │  │
│ │ Drawing:  [CHW System Isometric_______________]    │  │
│ │ Number:   [P-001______]  Rev: [A__]               │  │
│ │ Date:     [2025-01-14_]  By: [C.C. Lin_______]    │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ [Generate Preview] [Export DWG] [Export PDF]             │
│                                                          │
│ Preview:                                                 │
│ ┌────────────────────────────────────────────────────┐  │
│ │                                                    │  │
│ │         [2D Isometric Drawing Preview]             │  │
│ │                                                    │  │
│ │   CH_001 ──┬── DN150 ──┬── CHWP_001               │  │
│ │            │           │                           │  │
│ │            └── DN150 ──┴── CHWP_002               │  │
│ │                                                    │  │
│ └────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

**自動標註功能**：
- 管徑標註
- 標高標註
- 坡度標註
- 元件編號
- 材料規格
- 焊接符號
- 尺寸鏈

**匯出格式**：
- DWG (AutoCAD)
- DXF (通用 CAD)
- PDF (可列印)
- SVG (向量圖)

#### 5.3.5 流體分析引擎

**CFD 模擬整合**：

Factory Designer 整合 OpenFOAM 等 CFD 引擎，提供流體分析功能：

**分析類型**：
1. **管網水力計算**：壓降、流量分配
2. **HVAC 氣流分析**：風速、溫度分佈
3. **潔淨室氣流模擬**：粒子軌跡、換氣次數
4. **熱傳分析**：溫度場、熱負荷

**流體分析介面**：

```
┌─────────────────────────────────────────────────────────┐
│ Fluid Analysis - Chilled Water Network                   │
├─────────────────────────────────────────────────────────┤
│ Analysis Type: [Hydraulic Calculation ▼]                 │
│                                                          │
│ Boundary Conditions:                                     │
│ ┌────────────────────────────────────────────────────┐  │
│ │ Inlet (Chiller_001):                               │  │
│ │   Flow Rate:  [300__] m³/h                         │  │
│ │   Pressure:   [400__] kPa                          │  │
│ │   Temperature:[7.0__] °C                           │  │
│ │                                                    │  │
│ │ Outlets (AHU_001 ~ AHU_010):                       │  │
│ │   Total Demand: [300__] m³/h                       │  │
│ │   Distribution: [Auto-balance ▼]                   │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ Pipe Properties:                                         │
│ ┌────────────────────────────────────────────────────┐  │
│ │ Material:     [Stainless Steel ▼]                  │  │
│ │ Roughness:    [0.045] mm                           │  │
│ │ Insulation:   [☑] Thickness: [25__] mm             │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ [Run Simulation]                                         │
│                                                          │
│ Results:                                                 │
│ ┌────────────────────────────────────────────────────┐  │
│ │ Total Pressure Drop:  45.2 kPa                     │  │
│ │ Max Velocity:         2.3 m/s (at DN50 branch)     │  │
│ │ Min Velocity:         0.8 m/s (at DN150 header)    │  │
│ │ Pump Head Required:   50.5 m                       │  │
│ │ Pump Power:           16.2 kW                      │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ Visualization:                                           │
│ [Pressure Contour] [Velocity Vectors] [Flow Animation]   │
│                                                          │
│ [Export Report] [Optimize Design]                        │
└─────────────────────────────────────────────────────────┘
```

**視覺化結果**：
- 壓力等值線圖
- 速度向量場
- 溫度雲圖
- 流線動畫

### 5.4 協同設計功能

#### 5.4.1 多用戶即時協同

Factory Designer 支援多用戶即時協同設計：

**協同功能**：
- 即時游標位置顯示
- 用戶操作廣播
- 衝突鎖定（防止同時編輯同一物件）
- 聊天和標註

**協同介面**：

```
┌─────────────────────────────────────────────────────────┐
│ Collaboration Panel                                      │
├─────────────────────────────────────────────────────────┤
│ Active Users (3):                                        │
│ ┌────────────────────────────────────────────────────┐  │
│ │ ● Michael Lin (You)       - Editing: Pump_001     │  │
│ │ ● John Chen               - Viewing: Floor 2F     │  │
│ │ ● Sarah Wang              - Editing: Piping       │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ Chat:                                                    │
│ ┌────────────────────────────────────────────────────┐  │
│ │ [10:30] John: 2F 的管道路徑需要調整                │  │
│ │ [10:32] You: 好的，我看一下                       │  │
│ │ [10:35] Sarah: 我已經完成 3F 的電氣設計            │  │
│ └────────────────────────────────────────────────────┘  │
│ [Type message...____________________________] [Send]     │
│                                                          │
│ Annotations (5):                                         │
│ ┌────────────────────────────────────────────────────┐  │
│ │ 📍 Pump_001: 需要確認功率規格 (Michael, 2h ago)   │  │
│ │ 📍 Pipe_DN100: 與樑柱衝突 (Sarah, 1h ago)         │  │
│ │ 📍 Chiller_002: 維護空間不足 (John, 30m ago)      │  │
│ └────────────────────────────────────────────────────┘  │
│ [Add Annotation]                                         │
│                                                          │
│ Version History:                                         │
│ ┌────────────────────────────────────────────────────┐  │
│ │ v1.5 - 2025-01-14 11:00 - Michael Lin              │  │
│ │ v1.4 - 2025-01-14 10:30 - Sarah Wang               │  │
│ │ v1.3 - 2025-01-14 09:45 - John Chen                │  │
│ └────────────────────────────────────────────────────┘  │
│ [View History] [Restore Version]                         │
└─────────────────────────────────────────────────────────┘
```

#### 5.4.2 Omniverse 整合

Factory Designer 與 NVIDIA Omniverse 深度整合：

**雙向同步**：
- FDL → USD：將工廠設計匯出為 USD 場景
- USD → FDL：從 Omniverse 場景導入資產位置

**即時渲染**：
- 使用 Omniverse RTX 進行照片級渲染
- 支援光線追蹤和全局照明
- VR/AR 預覽

**協同設計**：
- 多用戶在 Omniverse 中同時編輯
- 變更即時同步回 Factory Designer

### 5.5 部署功能

#### 5.5.1 一鍵部署到 NDH

Factory Designer 可以將 FDL 設計直接部署到 NDH：

**部署流程**：

```
┌─────────────────────────────────────────────────────────┐
│ Deploy to NDH                                            │
├─────────────────────────────────────────────────────────┤
│ Target NDH Instance:                                     │
│ [https://ndh.harvatek.com_______________________] [Test] │
│                                                          │
│ Deployment Options:                                      │
│ ┌────────────────────────────────────────────────────┐  │
│ │ ☑ Deploy Factory Layout (FDL)                      │  │
│ │ ☑ Load Asset Definitions (IADL)                    │  │
│ │ ☑ Instantiate Asset Tag Instances                       │  │
│ │ ☑ Configure Data Tag Subscriptions                 │  │
│ │ ☑ Register to Naming Service                       │  │
│ │ ☐ Start Data Collection (manual start later)       │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ Validation:                                              │
│ ┌────────────────────────────────────────────────────┐  │
│ │ ✓ FDL Syntax: Valid                                │  │
│ │ ✓ IADL References: All resolved                    │  │
│ │ ✓ NDH Connectivity: OK                             │  │
│ │ ✓ Asset Count: 125 assets to deploy               │  │
│ │ ⚠ Warnings: 2 assets have no data tags configured  │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ [Preview Deployment] [Deploy] [Cancel]                   │
│                                                          │
│ Deployment Progress:                                     │
│ ┌────────────────────────────────────────────────────┐  │
│ │ [████████████████████████████░░░░░░░░] 75%         │  │
│ │                                                    │  │
│ │ Current Step: Instantiating Asset Tag Instances...      │  │
│ │ 94 / 125 assets deployed                           │  │
│ │                                                    │  │
│ │ Estimated Time Remaining: 2 minutes                │  │
│ └────────────────────────────────────────────────────┘  │
│                                                          │
│ [View Logs] [Pause] [Cancel Deployment]                  │
└─────────────────────────────────────────────────────────┘
```

**部署後驗證**：
- 檢查所有資產是否成功實例化
- 驗證數據標籤連接
- 測試行為邏輯
- 生成部署報告

---

## 6. IADL 與 FDL 的協同工作流程

### 6.1 完整的設計到部署流程

```
┌─────────────────────────────────────────────────────────┐
│          IDTF 完整工作流程                               │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  階段 1: 資產建模 (Asset Modeling)                       │
│  ┌────────────────────────────────────────────────────┐ │
│  │ 使用 IADL Editor 定義資產藍圖:                      │ │
│  │ • 3D 模型 (USD)                                    │ │
│  │ • 屬性定義                                         │ │
│  │ • 數據標籤 (OPC UA, Modbus, MQTT)                  │ │
│  │ • 行為邏輯 (Commands, Events, State Machines)      │ │
│  │ • 連接點 (Pipes, Electrical, Control)              │ │
│  │                                                    │ │
│  │ 輸出: StandardPump_v1.0.iadl                       │ │
│  └────────────────────────────────────────────────────┘ │
│                         ↓                               │
│  階段 2: 工廠設計 (Factory Design)                       │
│  ┌────────────────────────────────────────────────────┐ │
│  │ 使用 Factory Designer 設計工廠佈局:                 │ │
│  │ • 導入 IADL 資產到資產庫                           │ │
│  │ • 3D 佈局設計 (拖放、網格、線性)                    │ │
│  │ • MEP 設計 (自動佈線、元件選型)                     │ │
│  │ • 衝突檢測 (3D 碰撞、間隙檢查)                      │ │
│  │ • ISO 圖面生成 (平面圖、等角圖)                     │ │
│  │ • 流體分析 (水力計算、CFD 模擬)                     │ │
│  │                                                    │ │
│  │ 輸出: Harvatek_Factory_v2.0.fdl                    │ │
│  └────────────────────────────────────────────────────┘ │
│                         ↓                               │
│  階段 3: 部署到 NDH (Deployment)                         │
│  ┌────────────────────────────────────────────────────┐ │
│  │ Factory Designer 一鍵部署到 NDH:                    │ │
│  │ • 解析 FDL 文件                                    │ │
│  │ • 載入 IADL 資產定義                               │ │
│  │ • 批次實例化 Asset Tag Instances (125 個)               │ │
│  │ • 配置數據標籤訂閱 (OPC UA, Modbus)                │ │
│  │ • 註冊到命名服務 (Naming Service)                  │ │
│  │ • 啟動即時數據採集                                 │ │
│  └────────────────────────────────────────────────────┘ │
│                         ↓                               │
│  階段 4: 執行時運行 (Runtime Operation)                  │
│  ┌────────────────────────────────────────────────────┐ │
│  │ NDH 執行時管理:                                    │ │
│  │ • Asset Tag Instances 即時數據採集                      │ │
│  │ • MES/ERP/SCADA 雙向同步                           │ │
│  │ • 即時監控與告警                                   │ │
│  │ • 行為邏輯執行 (Start/Stop/AdjustFlow)             │ │
│  │ • 審計日誌記錄                                     │ │
│  │ • 與 Omniverse 即時協同                            │ │
│  └────────────────────────────────────────────────────┘ │
│                         ↓                               │
│  階段 5: 視覺化與協同 (Visualization & Collaboration)    │
│  ┌────────────────────────────────────────────────────┐ │
│  │ Omniverse 3D 視覺化:                               │ │
│  │ • USD 場景即時渲染                                 │ │
│  │ • 數位分身狀態即時更新                             │ │
│  │ • 多用戶協同設計                                   │ │
│  │ • VR/AR 沉浸式體驗                                 │ │
│  └────────────────────────────────────────────────────┘ │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### 6.2 數據流與責任分工

| 階段 | 工具 | 輸入 | 輸出 | 職責 |
|------|------|------|------|------|
| **資產建模** | IADL Editor | 3D 模型 (USD), 規格 | .iadl 文件 | 定義資產「是什麼」 |
| **工廠設計** | Factory Designer | .iadl 文件 | .fdl 文件 | 定義工廠「如何佈局」 |
| **部署** | Factory Designer + NDH | .fdl + .iadl | Asset Tag Instances | 自動化部署 |
| **執行** | NDH | 現場數據 (OPC UA, Modbus) | 即時數據流 | 執行時數據管理 |
| **視覺化** | Omniverse | USD 場景 + 即時數據 | 3D 渲染 | 視覺化與協同 |

---

## 7. 實際應用案例

### 7.1 案例一：宏齊科技 LED 封裝廠

**專案背景**：
- 客戶：宏齊科技 (Harvatek)
- 產業：LED 封裝製造
- 規模：5 層樓，50,000 m²
- 設備數量：500+ 台
- 專案週期：6 個月

**使用的 IDTF 組件**：
- IADL Editor：定義 20+ 種資產類型（固晶機、焊線機、封裝機、烤箱、泵浦、冷水主機等）
- Factory Designer：設計 5 層樓的完整佈局，包含 MEP 系統
- NDH：部署 500+ Asset Tag Instances，整合 MES/ERP
- Omniverse：3D 視覺化與多用戶協同

**專案成果**：
- ✅ 設計時間減少 **67%**（從 12 週 → 4 週）
- ✅ 設計錯誤減少 **80%**（衝突檢測自動化）
- ✅ 施工變更減少 **60%**（3D 預覽和模擬）
- ✅ ROI **193.8%**，回收期 **6.2 個月**
- ✅ 年度效益 **$14.6M**

**技術亮點**：
1. **IADL 資產重用**：20 種資產定義可在未來專案中重用
2. **FDL 參數化設計**：冷卻水系統參數化，易於調整
3. **MEP 自動化**：管道自動佈線節省 80% 設計時間
4. **衝突檢測**：發現並解決 150+ 個設計衝突
5. **即時協同**：台灣、美國、歐洲團隊同時設計

### 7.2 案例二：NVIDIA GPU 數據中心

**專案背景**：
- 客戶：某大型雲端服務商
- 產業：數據中心
- 規模：10,000 台 GPU 伺服器
- 功率密度：50 kW/rack
- 專案週期：9 個月

**使用的 IDTF 組件**：
- IADL Editor：定義數據中心專用資產（GPU 伺服器、液冷系統、UPS、PDU、CRAC）
- Factory Designer：設計高密度機櫃佈局和液冷系統
- NDH：整合 DCIM、BMS、SCADA
- Omniverse：數位分身視覺化

**專案成果**：
- ✅ PUE (Power Usage Effectiveness) **1.15**（業界領先）
- ✅ 冷卻效率提升 **40%**（液冷 + CFD 優化）
- ✅ 部署時間減少 **50%**（自動化部署）
- ✅ 能源成本節省 **$2.5M/年**

**技術亮點**：
1. **液冷系統設計**：使用 FDL 設計複雜的液冷管路
2. **CFD 模擬**：優化機房氣流，消除熱點
3. **即時監控**：10,000+ 數據點即時採集
4. **預測性維護**：AI 模型預測設備故障

### 7.3 案例三：台積電半導體廠

**專案背景**：
- 客戶：台積電 (TSMC)
- 產業：半導體製造
- 規模：Fab 18 新廠房
- 潔淨室等級：ISO 3
- 專案週期：12 個月

**使用的 IDTF 組件**：
- IADL Editor：定義半導體設備（CVD、蝕刻機、CMP、光刻機）
- Factory Designer：設計潔淨室佈局和 FMCS (Facility Monitoring and Control System)
- NDH：整合 MES、SCADA、BMS
- Omniverse：虛擬工廠漫遊

**專案成果**：
- ✅ 符合 SEMI S2 安全標準
- ✅ 潔淨室粒子濃度 **< 10 particles/m³** (0.1 μm)
- ✅ 設備稼動率 **> 95%**
- ✅ 施工零衝突（衝突檢測 100% 覆蓋）

**技術亮點**：
1. **超高精度佈局**：設備定位精度 ±1mm
2. **潔淨室氣流模擬**：CFD 模擬確保粒子控制
3. **FMCS 整合**：監控 5,000+ 個環境參數
4. **數位分身驗證**：虛擬調試節省 3 個月

---

## 8. 最佳實踐與設計模式

### 8.1 IADL 設計最佳實踐

#### 8.1.1 資產命名規範

**推薦命名格式**：

```
<Category>_<Type>_<Variant>_v<Version>

範例:
- Pump_Centrifugal_Standard_v1.0
- Sensor_Temperature_PT100_v2.1
- Motor_AC_15kW_v1.0
```

#### 8.1.2 屬性設計原則

1. **使用標準單位**：SI 單位優先（m, kg, s, K, Pa）
2. **定義合理範圍**：防止無效值
3. **提供預設值**：加速配置
4. **添加描述**：提高可讀性

```yaml
properties:
  - name: "rated_power"
    type: "float"
    default: 15.0
    unit: "kW"
    range: [5.0, 30.0]
    description: "額定功率，根據實際負載選擇"
```

#### 8.1.3 數據標籤設計原則

1. **標準化地址格式**：使用一致的命名空間
2. **分組相關標籤**：便於管理
3. **設定合理更新率**：平衡性能和實時性
4. **使用死區 (Deadband)**：減少無效更新

```yaml
data_tags:
  # 過程變數 (PV) - 高頻更新
  - name: "flow_rate"
    protocol: "OPC_UA"
    address: "ns=2;s=Pump.PV.FlowRate"
    update_rate: 1000  # 1 秒
    deadband: 0.1  # 絕對死區
  
  # 狀態變數 - 中頻更新
  - name: "status"
    protocol: "OPC_UA"
    address: "ns=2;s=Pump.Status"
    update_rate: 5000  # 5 秒
  
  # 配置參數 - 低頻更新
  - name: "rated_power"
    protocol: "OPC_UA"
    address: "ns=2;s=Pump.Config.RatedPower"
    update_rate: 60000  # 60 秒
```

#### 8.1.4 行為設計原則

1. **單一職責**：每個行為只做一件事
2. **明確輸入輸出**：定義清晰的介面
3. **錯誤處理**：返回明確的錯誤訊息
4. **超時設定**：防止無限等待

```yaml
behaviors:
  - name: "Start"
    type: "command"
    description: "啟動泵浦並達到目標流量"
    inputs:
      - name: "target_flow"
        type: "float"
        unit: "m³/h"
        range: [0, 200]
    outputs:
      - name: "success"
        type: "boolean"
      - name: "actual_flow"
        type: "float"
        unit: "m³/h"
      - name: "error_message"
        type: "string"
    timeout: 60  # seconds
    logic: |
      try:
        if status != "Stopped":
          return (false, 0.0, "Pump is not in Stopped state")
        
        write_tag("run_command", true)
        wait_for_status("Running", timeout=30s)
        
        # 調整到目標流量
        adjust_flow(target_flow)
        wait(5s)
        
        actual = read_tag("flow_rate")
        return (true, actual, "")
      
      except TimeoutError:
        return (false, 0.0, "Timeout waiting for pump to start")
      except Exception as e:
        return (false, 0.0, str(e))
```

### 8.2 FDL 設計最佳實踐

#### 8.2.1 模組化設計

將大型工廠分解為多個模組：

```yaml
# 主 FDL 文件
factory_design:
  metadata:
    name: "Main Factory"
  
  imports:
    - "modules/chilled_water_system.fdl"
    - "modules/compressed_air_system.fdl"
    - "modules/production_line_2f.fdl"
    - "modules/production_line_3f.fdl"
  
  layout:
    # 引用模組
    - $ref: "modules/chilled_water_system.fdl#/layout/ChilledWaterPlant"
    - $ref: "modules/production_line_2f.fdl#/layout/StandardLEDProduction"
```

#### 8.2.2 參數化與重用

使用參數提高重用性：

```yaml
parameters:
  # 全域參數
  pump_power: 15.0
  pump_count: 6
  
  # 區域參數
  floor_2f:
    equipment_spacing: 4.0
    die_bonder_count: 10
  
  floor_3f:
    equipment_spacing: 5.0
    die_bonder_count: 5

layout:
  - area: "StandardLEDProduction"
    floor: "2F"
    instances:
      - type: "DieBonder"
        count: ${floor_2f.die_bonder_count}
        grid_params:
          spacing_x: ${floor_2f.equipment_spacing}
```

#### 8.2.3 命名規範

**推薦命名格式**：

```
<Type>_<Number>

範例:
- Pump_001, Pump_002, ...
- CH_001 (Chiller_001)
- CHWP_001 (Chilled Water Pump_001)
- CT_001 (Cooling Tower_001)
```

**使用前綴縮寫**：

| 設備類型 | 前綴 | 範例 |
|----------|------|------|
| Chiller | CH | CH_001 |
| Pump | P | P_001 |
| Chilled Water Pump | CHWP | CHWP_001 |
| Cooling Tower | CT | CT_001 |
| Air Handling Unit | AHU | AHU_001 |
| Fan Coil Unit | FCU | FCU_001 |
| Transformer | TR | TR_001 |
| Switchgear | SG | SG_001 |

#### 8.2.4 關係定義最佳實踐

1. **明確連接類型**：pipe, electrical, control_signal
2. **添加屬性**：diameter, material, voltage, protocol
3. **使用模式匹配**：批次定義關係

```yaml
relationships:
  # 使用模式匹配批次定義
  - from_pattern: "CH_*"
    to_pattern: "CHWP_*"
    type: "pipe"
    properties:
      fluid: "chilled_water"
      diameter: 150
      material: "stainless_steel"
      insulation: true
  
  # 明確定義單一連接
  - from: "CHWP_001"
    to: "AHU_001"
    type: "pipe"
    properties:
      fluid: "chilled_water"
      diameter: 80
      material: "stainless_steel"
      insulation: true
      length: 45.5  # 手動指定長度
```

### 8.3 版本控制最佳實踐

#### 8.3.1 語義化版本號

使用 **Semantic Versioning (SemVer)**：

```
<Major>.<Minor>.<Patch>

範例:
- 1.0.0: 初始版本
- 1.1.0: 新增功能（向後相容）
- 1.1.1: 錯誤修正（向後相容）
- 2.0.0: 重大變更（不向後相容）
```

#### 8.3.2 Git 工作流程

**推薦的 Git 分支策略**：

```
main (生產環境)
├── develop (開發環境)
│   ├── feature/new-asset-type
│   ├── feature/fdl-optimization
│   └── bugfix/data-tag-error
└── release/v2.0.0
```

**提交訊息規範**：

```
<type>(<scope>): <subject>

type: feat, fix, docs, style, refactor, test, chore
scope: iadl, fdl, ndh, omniverse
subject: 簡短描述

範例:
feat(iadl): Add high-precision die bonder asset definition
fix(fdl): Correct chilled water pipe diameter calculation
docs(iadl): Update data tag configuration guide
```

---

## 9. 技術優勢與競爭力分析

### 9.1 與競爭對手的對比

| 維度 | IDTF (IADL + FDL) | Azure Digital Twins | Siemens Xcelerator | AWS IoT TwinMaker |
|------|-------------------|---------------------|-------------------|-------------------|
| **開源性** | ✅ 完全開源 | ❌ 專有 | ❌ 專有 | ❌ 專有 |
| **供應商鎖定** | ✅ 無鎖定 | ❌ Azure 鎖定 | ❌ Siemens 鎖定 | ❌ AWS 鎖定 |
| **成本** | ✅ 低（節省 70%） | ❌ 高 | ❌ 極高 | ❌ 高 |
| **IT/OT 整合** | ✅ 原生支援 | ⚠️ 有限 | ✅ 強 | ⚠️ 有限 |
| **MEP 設計** | ✅ 原生支援 | ❌ 無 | ✅ 有（需額外授權） | ❌ 無 |
| **衝突檢測** | ✅ 原生支援 | ❌ 無 | ✅ 有（需額外授權） | ❌ 無 |
| **ISO 圖面生成** | ✅ 原生支援 | ❌ 無 | ✅ 有（需額外授權） | ❌ 無 |
| **流體分析** | ✅ 原生支援 | ❌ 無 | ✅ 有（需額外授權） | ❌ 無 |
| **Omniverse 整合** | ✅ 深度整合 | ⚠️ 有限 | ⚠️ 有限 | ❌ 無 |
| **可抽換 TSDB** | ✅ 4+ 種選擇 | ❌ Azure 專用 | ❌ Siemens 專用 | ❌ AWS 專用 |
| **學習曲線** | ✅ 低（YAML 語法） | ⚠️ 中 | ❌ 高 | ⚠️ 中 |
| **社群支援** | ✅ 開源社群 | ⚠️ 商業支援 | ⚠️ 商業支援 | ⚠️ 商業支援 |

### 9.2 核心技術優勢

#### 9.2.1 關注點分離 (Separation of Concerns)

**IADL + FDL 的雙層架構**：

- **IADL**：定義「資產是什麼」（What）
  - 3D 模型、屬性、數據標籤、行為邏輯
  - 可在多個專案中重用
  - 由設備專家維護

- **FDL**：定義「工廠如何佈局」（How）
  - 資產數量、位置、連接關係
  - 可複製到不同地點
  - 由工廠設計師維護

**優勢**：
- ✅ 提高重用性：資產定義一次，多處使用
- ✅ 降低維護成本：修改資產定義自動影響所有工廠
- ✅ 專業分工：不同角色專注於不同層次

#### 9.2.2 一次建模，到處使用 (Model Once, Use Everywhere)

**IADL 資產定義的重用**：

```
StandardPump_v1.0.iadl
├── 專案 A: Harvatek LED Factory (使用 50 次)
├── 專案 B: TSMC Fab 18 (使用 120 次)
├── 專案 C: NVIDIA Data Center (使用 80 次)
└── 專案 D: LNG Power Plant (使用 30 次)

總重用次數: 280 次
節省設計時間: 280 × 4 小時 = 1,120 小時
```

#### 9.2.3 自動化程度高

**95% 自動化部署**：

| 步驟 | 傳統方式 | IDTF 方式 | 自動化程度 |
|------|----------|-----------|-----------|
| 資產建模 | 手動編寫代碼 | IADL Editor 視覺化 | 80% |
| 工廠設計 | 手動繪圖 | Factory Designer 3D | 90% |
| MEP 設計 | 手動佈線 | 自動佈線 | 95% |
| 衝突檢測 | 手動檢查 | 自動檢測 | 100% |
| ISO 圖面 | 手動繪製 | 自動生成 | 100% |
| 部署到 NDH | 手動配置 | 一鍵部署 | 100% |

#### 9.2.4 供應商中立

**可抽換的技術堆疊**：

| 組件 | 可選方案 |
|------|----------|
| **時序數據庫** | InfluxDB, TDengine, TimescaleDB, QuestDB |
| **關聯數據庫** | PostgreSQL, MySQL, SQL Server |
| **文檔數據庫** | MongoDB, Elasticsearch |
| **對象存儲** | MinIO, AWS S3, Azure Blob |
| **消息隊列** | RabbitMQ, Kafka, MQTT |
| **3D 引擎** | Omniverse, Three.js, Unity |
| **雲平台** | AWS, Azure, GCP, 私有雲 |

---

## 10. 未來發展路線圖

### 10.1 短期計劃（6-12 個月）

#### 10.1.1 IADL 增強

- ✅ **IADL V3.6**：
  - 支援更多工業協議（EtherCAT, PROFINET, CC-Link）
  - 增強行為邏輯（支援 Python 腳本）
  - 資產繼承機制（面向對象設計）

- ✅ **IADL Editor 增強**：
  - 行為邏輯視覺化編程（類似 Node-RED）
  - AI 輔助資產建模（從 CAD 自動生成 IADL）
  - 資產市場（Asset Marketplace）

#### 10.1.2 FDL 增強

- ✅ **FDL V3.6**：
  - 支援 4D 施工排程（時間維度）
  - 支援成本估算（BOM + 人工）
  - 支援能源模擬（年度能耗預測）

- ✅ **Factory Designer 增強**：
  - AI 輔助佈局優化（遺傳算法）
  - VR/AR 設計模式
  - 多物理場模擬（熱、聲、光）

### 10.2 中期計劃（1-2 年）

#### 10.2.1 AI/ML 整合

- ✅ **AI 輔助設計**：
  - 自動生成最佳工廠佈局
  - 自動選擇設備規格
  - 自動優化管道路徑

- ✅ **預測性維護**：
  - 基於數位分身的故障預測
  - 剩餘壽命估算（RUL）
  - 維護排程優化

#### 10.2.2 標準化推動

- ✅ **IADL 成為行業標準**：
  - 與 IEC、ISO、SEMI 合作
  - 推動 IADL 成為國際標準
  - 建立認證體系

- ✅ **開源社群建設**：
  - IADL/FDL GitHub 組織
  - 定期發布新版本
  - 社群貢獻獎勵計劃

### 10.3 長期願景（3-5 年）

#### 10.3.1 全球數位分身網路

- ✅ **跨工廠協同**：
  - 全球多個工廠的數位分身互聯
  - 供應鏈數位分身
  - 全球優化調度

#### 10.3.2 認知數位分身

- ✅ **自主決策**：
  - 數位分身具備自主決策能力
  - 基於強化學習的優化
  - 人機協同決策

---

## 結論

IADL 和 FDL 作為 IDTF V3.5 生態系統的核心語言，提供了一個**開源、中立、可負擔**的工業數位分身解決方案。通過**關注點分離**的設計理念，IADL 專注於資產建模，FDL 專注於工廠設計，兩者協同工作，實現了從設計到部署的完整自動化流程。

配合 IADL Editor 和 Factory Designer 兩個視覺化工具，以及 NDH 執行時數據中樞和 Omniverse 3D 視覺化平台，IDTF 提供了一個**全生命週期、IT/OT 融合、高度自動化**的工業數位分身框架。

在宏齊科技、NVIDIA 數據中心、台積電半導體廠等實際專案中，IDTF 已經證明了其**技術先進性**和**商業價值**，相較於 Azure、Siemens、AWS 等競爭對手，IDTF 在**成本、開放性、自動化程度**方面具有顯著優勢。

未來，IDTF 將持續演進，整合 AI/ML 技術，推動 IADL 成為行業標準，建設開源社群，最終實現**全球數位分身網路**和**認知數位分身**的願景。

---

## 附錄

### 附錄 A：IADL 語法速查表

```yaml
# 基本結構
asset_definition:
  metadata:
    id: <string>
    name: <string>
    version: <string>
    category: <string>
  
  geometry:
    model_uri: <string>
    bounding_box: {min: [x,y,z], max: [x,y,z]}
  
  properties:
    - name: <string>
      type: <string|int|float|boolean|enum>
      default: <value>
      unit: <string>
      range: [min, max]
  
  data_tags:
    - name: <string>
      protocol: <OPC_UA|Modbus|MQTT|BACnet|S7|EtherNetIP>
      address: <string>
      data_type: <string>
      access: <read|write|read_write>
  
  behaviors:
    - name: <string>
      type: <command|event|state_machine>
      inputs: [...]
      outputs: [...]
      logic: <string>
  
  connection_points:
    - name: <string>
      type: <pipe|electrical|control>
      position: [x, y, z]
      direction: <in|out|bidirectional>
```

### 附錄 B：FDL 語法速查表

```yaml
# 基本結構
factory_design:
  metadata:
    name: <string>
    version: <string>
  
  buildings:
    - building_id: <string>
      floors: [...]
  
  parameters:
    <key>: <value>
  
  layout:
    - area: <string>
      building: <string>
      floor: <string>
      instances:
        - type: <IADL asset type>
          count: <int>
          naming_prefix: <string>
          location_pattern: <grid|line|circle|custom>
          grid_params: {...}
          initial_params: {...}
      relationships:
        - from: <string>
          to: <string>
          type: <pipe|electrical|material_flow|control_signal>
          properties: {...}
```

### 附錄 C：參考資源

**官方文檔**：
- IDTF 官網：https://idtf.org
- IADL 規範：https://github.com/idtf/iadl-spec
- FDL 規範：https://github.com/idtf/fdl-spec
- NDH 文檔：https://github.com/idtf/ndh-docs

**社群資源**：
- IDTF 論壇：https://forum.idtf.org
- Discord 頻道：https://discord.gg/idtf
- YouTube 教學：https://youtube.com/@idtf

**相關標準**：
- IEC 62541 (OPC UA)
- ISO 16739 (IFC)
- ISO 15926 (Process Plant Data)
- SEMI E120 (CIM Framework)

---

**文件版本**: 3.5  
**最後更新**: 2025-01-14  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih Cheng Lin, Michael Lin)  
**組織**: MacroVision Systems / IDTF Consortium  
**授權**: CC BY-SA 4.0

---

