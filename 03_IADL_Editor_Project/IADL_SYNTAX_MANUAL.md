# IADL 語法手冊

## 目錄

1. [簡介](#簡介)
2. [基本語法](#基本語法)
3. [資產定義](#資產定義)
4. [屬性定義](#屬性定義)
5. [關係定義](#關係定義)
6. [階層結構](#階層結構)
7. [資料類型](#資料類型)
8. [註解](#註解)
9. [關鍵字參考](#關鍵字參考)
10. [完整範例](#完整範例)
11. [最佳實踐](#最佳實踐)

---

## 簡介

IADL (Industrial Asset Description Language) 是一種專為工業資產管理設計的描述語言。它提供了簡潔而強大的語法,用於定義工業資產、屬性、關係和階層結構。IADL 的設計目標是讓工程師能夠快速建立和維護工業資產模型,同時保持良好的可讀性和可維護性。

### 設計理念

IADL 採用宣告式語法,強調資產的結構和關係,而非實作細節。語言設計遵循以下原則:簡潔性 (最小化語法元素)、可讀性 (接近自然語言的表達)、可擴展性 (支援自訂屬性和類型)、互操作性 (支援多種工業系統的匯入/匯出) 以及階層性 (支援複雜的資產組織結構)。

### 應用場景

IADL 適用於多種工業應用場景,包括 SCADA 系統的標籤管理、工廠設備的資產建模、建築資訊模型 (BIM) 的設備描述、電力系統的網路拓撲定義、製造執行系統 (MES) 的資源管理,以及物聯網 (IIoT) 的裝置描述。

---

## 基本語法

### 語法結構

IADL 文件由一系列的資產定義組成。每個資產定義包含資產類型、資產名稱、屬性列表和可選的子資產。基本語法結構如下:

```
AssetType AssetName {
    property: value
    property: value
    ...
}
```

### 命名規則

資產名稱和屬性名稱遵循以下規則:必須以字母或底線開頭,可以包含字母、數字和底線,區分大小寫,建議使用駝峰式命名法 (camelCase) 或底線分隔命名法 (snake_case)。

### 保留字

以下是 IADL 的保留字,不應用作資產名稱或屬性名稱:Asset、Equipment、Device、Sensor、Actuator、Controller、System、Network、Connection、Relationship、Parent、Child、Type、Name、ID、Description、Value、Unit、Status、Quality、Timestamp。

---

## 資產定義

### 基本資產定義

資產是 IADL 的核心概念,代表工業環境中的任何實體或邏輯元件。基本資產定義包含資產類型和資產名稱。

**語法**:
```
AssetType AssetName {
    // 屬性定義
}
```

**範例**:
```iadl
Equipment Tank1 {
    type: "Storage Tank"
    capacity: 10000
    unit: "liters"
}
```

### 資產類型

IADL 支援多種預定義的資產類型,每種類型都有其特定的用途和屬性。

#### Equipment (設備)

代表實體設備,如馬達、幫浦、閥門等。

```iadl
Equipment Pump1 {
    type: "Centrifugal Pump"
    power: 15
    powerUnit: "kW"
    manufacturer: "ABB"
    model: "M3BP 160"
    serialNumber: "ABC123456"
    installDate: "2024-01-15"
}
```

#### Device (裝置)

代表控制或監控裝置,如 PLC、感測器、執行器等。

```iadl
Device PLC1 {
    type: "Programmable Logic Controller"
    manufacturer: "Siemens"
    model: "S7-1500"
    ipAddress: "192.168.1.100"
    firmwareVersion: "2.9.3"
}
```

#### Sensor (感測器)

代表測量裝置,如溫度感測器、壓力感測器等。

```iadl
Sensor TempSensor1 {
    type: "Temperature Sensor"
    measurementType: "Temperature"
    range: "0-100"
    unit: "°C"
    accuracy: "±0.5"
    outputType: "4-20mA"
}
```

#### Actuator (執行器)

代表執行動作的裝置,如閥門、馬達控制器等。

```iadl
Actuator Valve1 {
    type: "Control Valve"
    valveType: "Ball Valve"
    size: "2 inch"
    material: "Stainless Steel"
    actuatorType: "Pneumatic"
    controlSignal: "4-20mA"
}
```

#### Controller (控制器)

代表控制系統,如 DCS、SCADA 伺服器等。

```iadl
Controller DCS1 {
    type: "Distributed Control System"
    manufacturer: "Honeywell"
    model: "Experion PKS"
    serverAddress: "192.168.1.10"
    redundancy: true
}
```

#### System (系統)

代表邏輯系統或子系統,如冷卻系統、供電系統等。

```iadl
System CoolingSystem {
    type: "Cooling System"
    description: "Main cooling system for reactor"
    operatingMode: "Continuous"
    designCapacity: 5000
    capacityUnit: "kW"
}
```

#### Network (網路)

代表通訊網路,如 Ethernet、Fieldbus 等。

```iadl
Network ControlNetwork {
    type: "Industrial Ethernet"
    protocol: "PROFINET"
    topology: "Star"
    bandwidth: "1 Gbps"
    redundancy: true
}
```

---

## 屬性定義

### 基本屬性

屬性用於描述資產的特徵和狀態。每個屬性由屬性名稱和屬性值組成,使用冒號分隔。

**語法**:
```
propertyName: propertyValue
```

**範例**:
```iadl
Equipment Motor1 {
    type: "AC Motor"
    power: 30
    voltage: 380
    frequency: 50
    speed: 1500
}
```

### 常用屬性

以下是 IADL 中常用的屬性及其說明:

#### 識別屬性

- **id**: 唯一識別碼
- **name**: 資產名稱
- **type**: 資產類型
- **description**: 描述

```iadl
Equipment Compressor1 {
    id: "COMP-001"
    name: "Main Air Compressor"
    type: "Screw Compressor"
    description: "Primary compressed air supply for production line"
}
```

#### 技術屬性

- **manufacturer**: 製造商
- **model**: 型號
- **serialNumber**: 序號
- **capacity**: 容量
- **power**: 功率
- **voltage**: 電壓
- **current**: 電流
- **pressure**: 壓力
- **temperature**: 溫度
- **flow**: 流量

```iadl
Equipment HeatExchanger1 {
    manufacturer: "Alfa Laval"
    model: "M15-BFG"
    serialNumber: "HE2024001"
    capacity: 500
    capacityUnit: "kW"
    maxPressure: 10
    pressureUnit: "bar"
    maxTemperature: 150
    temperatureUnit: "°C"
}
```

#### 狀態屬性

- **status**: 狀態 (Running, Stopped, Fault, Maintenance)
- **enabled**: 啟用狀態 (true/false)
- **quality**: 資料品質 (Good, Bad, Uncertain)
- **value**: 當前值
- **timestamp**: 時間戳記

```iadl
Sensor PressureSensor1 {
    status: "Running"
    enabled: true
    quality: "Good"
    value: 5.2
    unit: "bar"
    timestamp: "2024-10-09T10:30:00Z"
}
```

#### 位置屬性

- **location**: 位置
- **building**: 建築物
- **floor**: 樓層
- **room**: 房間
- **coordinates**: 座標

```iadl
Equipment Boiler1 {
    location: "Plant A"
    building: "Utility Building"
    floor: "Ground Floor"
    room: "Boiler Room 1"
    coordinates: "23.5,121.0,10.5"
}
```

#### 維護屬性

- **installDate**: 安裝日期
- **commissionDate**: 啟用日期
- **lastMaintenanceDate**: 上次維護日期
- **nextMaintenanceDate**: 下次維護日期
- **maintenanceInterval**: 維護間隔
- **warranty**: 保固期

```iadl
Equipment Transformer1 {
    installDate: "2023-06-15"
    commissionDate: "2023-07-01"
    lastMaintenanceDate: "2024-09-01"
    nextMaintenanceDate: "2025-03-01"
    maintenanceInterval: "6 months"
    warranty: "5 years"
}
```

### 自訂屬性

IADL 支援自訂屬性,允許使用者根據特定需求定義額外的屬性。自訂屬性的命名應遵循命名規則,並建議使用描述性名稱。

```iadl
Equipment CustomEquipment1 {
    // 標準屬性
    type: "Custom Equipment"
    
    // 自訂屬性
    customField1: "Custom Value"
    customField2: 12345
    customField3: true
    operatingHours: 8760
    efficiency: 0.95
    maintenanceCost: 50000
    energyConsumption: 1500
}
```

---

## 關係定義

### 父子關係

IADL 支援階層式的父子關係,用於表示資產之間的包含或組成關係。子資產定義在父資產的大括號內。

**語法**:
```
ParentAsset ParentName {
    property: value
    
    ChildAsset ChildName {
        property: value
    }
}
```

**範例**:
```iadl
System ProductionLine1 {
    type: "Production Line"
    description: "Main production line"
    
    Equipment Station1 {
        type: "Assembly Station"
        position: 1
        
        Device Robot1 {
            type: "Industrial Robot"
            manufacturer: "KUKA"
            model: "KR 10 R1100"
        }
        
        Sensor VisionSensor1 {
            type: "Vision Sensor"
            resolution: "1920x1080"
        }
    }
    
    Equipment Station2 {
        type: "Testing Station"
        position: 2
    }
}
```

### 連接關係

連接關係用於表示資產之間的通訊、電氣或機械連接。

**語法**:
```
Connection ConnectionName {
    from: "SourceAsset"
    to: "TargetAsset"
    type: "ConnectionType"
    properties...
}
```

**範例**:
```iadl
Connection EthernetConnection1 {
    from: "PLC1"
    to: "SCADA_Server"
    type: "Ethernet"
    protocol: "TCP/IP"
    port: 502
    bandwidth: "100 Mbps"
}

Connection PowerConnection1 {
    from: "MainPanel"
    to: "Motor1"
    type: "Electrical"
    voltage: 380
    current: 50
    cableType: "3-Phase"
}

Connection PipeConnection1 {
    from: "Tank1"
    to: "Pump1"
    type: "Pipe"
    diameter: "4 inch"
    material: "Stainless Steel"
    pressure: 10
}
```

### 邏輯關係

邏輯關係用於表示資產之間的邏輯依賴或控制關係。

```iadl
Relationship ControlRelationship1 {
    type: "Controls"
    controller: "PLC1"
    controlled: "Valve1"
    controlMode: "Automatic"
    controlSignal: "4-20mA"
}

Relationship MonitoringRelationship1 {
    type: "Monitors"
    monitor: "SCADA_Server"
    monitored: "TempSensor1"
    updateRate: 1000
    updateUnit: "ms"
}

Relationship InterlockRelationship1 {
    type: "Interlock"
    source: "EmergencyStop"
    target: "ProductionLine1"
    condition: "Emergency"
    action: "Stop"
}
```

---

## 階層結構

### 多層次階層

IADL 支援任意深度的階層結構,用於表示複雜的工業系統組織。

**範例**:
```iadl
System Factory {
    type: "Manufacturing Facility"
    name: "Main Factory"
    location: "Taipei, Taiwan"
    
    System ProductionArea {
        type: "Production Area"
        name: "Production Area A"
        
        System ProductionLine1 {
            type: "Production Line"
            name: "Line 1"
            capacity: 1000
            capacityUnit: "units/hour"
            
            Equipment Station1 {
                type: "Assembly Station"
                position: 1
                
                Device Robot1 {
                    type: "Industrial Robot"
                    manufacturer: "ABB"
                    model: "IRB 6700"
                    payload: 150
                    reach: 3200
                }
                
                Sensor VisionSystem1 {
                    type: "Vision System"
                    resolution: "2048x2048"
                    frameRate: 60
                }
            }
            
            Equipment Station2 {
                type: "Testing Station"
                position: 2
                
                Device TestEquipment1 {
                    type: "Automated Test Equipment"
                    testType: "Functional Test"
                }
            }
            
            Equipment Station3 {
                type: "Packaging Station"
                position: 3
            }
        }
        
        System ProductionLine2 {
            type: "Production Line"
            name: "Line 2"
            capacity: 800
            capacityUnit: "units/hour"
        }
    }
    
    System UtilityArea {
        type: "Utility Area"
        name: "Utility Systems"
        
        System PowerSystem {
            type: "Power Distribution System"
            
            Equipment MainTransformer {
                type: "Transformer"
                capacity: 2000
                capacityUnit: "kVA"
                primaryVoltage: 22000
                secondaryVoltage: 380
            }
            
            Equipment MainPanel {
                type: "Distribution Panel"
                voltage: 380
                current: 3000
            }
        }
        
        System CompressedAirSystem {
            type: "Compressed Air System"
            
            Equipment Compressor1 {
                type: "Air Compressor"
                capacity: 100
                capacityUnit: "m³/min"
                pressure: 8
                pressureUnit: "bar"
            }
            
            Equipment AirDryer1 {
                type: "Air Dryer"
                dewPoint: -40
                dewPointUnit: "°C"
            }
        }
    }
}
```

### 參考和引用

IADL 支援資產參考,允許在不同位置引用同一個資產,避免重複定義。

**語法**:
```
reference: "AssetPath"
```

**範例**:
```iadl
System ControlSystem {
    Controller MainPLC {
        id: "PLC-001"
        type: "PLC"
        model: "Siemens S7-1500"
    }
}

System ProductionLine1 {
    Equipment Station1 {
        controller: reference("ControlSystem.MainPLC")
        // 或使用 ID 引用
        controllerID: "PLC-001"
    }
}
```

---

## 資料類型

### 基本資料類型

IADL 支援以下基本資料類型:

#### 字串 (String)

用於表示文字資料,使用雙引號包圍。

```iadl
Equipment Example1 {
    name: "Equipment Name"
    description: "This is a description"
    manufacturer: "Company Name"
}
```

#### 數值 (Number)

用於表示整數或浮點數。

```iadl
Equipment Example2 {
    power: 30
    voltage: 380.5
    efficiency: 0.95
    temperature: -10.5
}
```

#### 布林 (Boolean)

用於表示真或假,使用 `true` 或 `false`。

```iadl
Equipment Example3 {
    enabled: true
    running: false
    alarm: false
    maintenance: true
}
```

#### 日期時間 (DateTime)

用於表示日期和時間,使用 ISO 8601 格式。

```iadl
Equipment Example4 {
    installDate: "2024-01-15"
    commissionDate: "2024-01-20T10:30:00Z"
    lastMaintenanceDate: "2024-09-01T14:00:00+08:00"
}
```

### 複合資料類型

#### 陣列 (Array)

用於表示多個值的列表,使用方括號包圍,值之間用逗號分隔。

```iadl
Equipment Example5 {
    tags: ["Production", "Critical", "High Priority"]
    alarmLevels: [10, 20, 30, 40]
    operators: ["John", "Mary", "Bob"]
}
```

#### 物件 (Object)

用於表示結構化資料,使用大括號包圍。

```iadl
Equipment Example6 {
    dimensions: {
        length: 2.5
        width: 1.5
        height: 2.0
        unit: "meters"
    }
    
    location: {
        building: "Building A"
        floor: 2
        room: "Room 201"
        coordinates: {
            x: 100.5
            y: 200.3
            z: 10.0
        }
    }
}
```

### 單位表示

IADL 建議使用單獨的屬性來表示單位,以保持資料的結構化。

```iadl
Sensor FlowSensor1 {
    value: 150.5
    unit: "m³/h"
    
    range: {
        min: 0
        max: 500
        unit: "m³/h"
    }
    
    accuracy: {
        value: 1.0
        unit: "%"
    }
}
```

---

## 註解

### 單行註解

使用雙斜線 `//` 開始單行註解,註解內容延續到行尾。

```iadl
// 這是一個單行註解
Equipment Pump1 {
    type: "Centrifugal Pump"  // 這也是註解
    power: 15  // 功率 15 kW
}
```

### 多行註解

使用 `/*` 開始和 `*/` 結束多行註解。

```iadl
/*
 * 這是一個多行註解
 * 可以跨越多行
 * 用於詳細說明
 */
Equipment ComplexEquipment {
    /*
     * 以下是設備的主要參數
     * 這些參數在設計階段確定
     */
    type: "Complex Equipment"
    capacity: 1000
}
```

### 文件註解

建議在每個主要資產定義前加入文件註解,說明資產的用途、功能和注意事項。

```iadl
/**
 * 主要生產線控制系統
 * 
 * 功能:
 * - 控制生產線的所有設備
 * - 監控生產過程
 * - 記錄生產資料
 * 
 * 注意事項:
 * - 需要定期備份程式
 * - 維護時需要停止生產
 * 
 * @author: John Doe
 * @date: 2024-10-09
 * @version: 1.0
 */
System ProductionLineControl {
    type: "Control System"
    // ... 屬性定義
}
```

---

## 關鍵字參考

### 資產類型關鍵字

| 關鍵字 | 說明 | 用途 |
|--------|------|------|
| Asset | 通用資產 | 表示任何類型的資產 |
| Equipment | 設備 | 表示實體設備 |
| Device | 裝置 | 表示控制或監控裝置 |
| Sensor | 感測器 | 表示測量裝置 |
| Actuator | 執行器 | 表示執行動作的裝置 |
| Controller | 控制器 | 表示控制系統 |
| System | 系統 | 表示邏輯系統或子系統 |
| Network | 網路 | 表示通訊網路 |
| Connection | 連接 | 表示資產之間的連接 |
| Relationship | 關係 | 表示資產之間的邏輯關係 |

### 屬性關鍵字

| 關鍵字 | 資料類型 | 說明 |
|--------|----------|------|
| id | String | 唯一識別碼 |
| name | String | 名稱 |
| type | String | 類型 |
| description | String | 描述 |
| manufacturer | String | 製造商 |
| model | String | 型號 |
| serialNumber | String | 序號 |
| status | String | 狀態 |
| enabled | Boolean | 啟用狀態 |
| value | Number | 值 |
| unit | String | 單位 |
| quality | String | 品質 |
| timestamp | DateTime | 時間戳記 |
| location | String | 位置 |
| capacity | Number | 容量 |
| power | Number | 功率 |
| voltage | Number | 電壓 |
| current | Number | 電流 |
| pressure | Number | 壓力 |
| temperature | Number | 溫度 |
| flow | Number | 流量 |

### 關係關鍵字

| 關鍵字 | 說明 |
|--------|------|
| from | 連接起點 |
| to | 連接終點 |
| parent | 父資產 |
| child | 子資產 |
| controller | 控制器 |
| controlled | 被控制對象 |
| monitor | 監控器 |
| monitored | 被監控對象 |
| reference | 資產引用 |

---

## 完整範例

### 範例 1: 簡單的幫浦系統

```iadl
/**
 * 冷卻水幫浦系統
 * 用於工廠冷卻水循環
 */
System CoolingWaterSystem {
    type: "Cooling Water System"
    description: "Main cooling water circulation system"
    location: "Plant A, Utility Building"
    
    // 儲水槽
    Equipment CoolingTower1 {
        type: "Cooling Tower"
        capacity: 500
        capacityUnit: "m³"
        manufacturer: "Baltimore Aircoil"
        model: "VTI-1500"
        installDate: "2023-05-15"
        
        // 溫度感測器
        Sensor TowerTempSensor {
            type: "Temperature Sensor"
            measurementType: "Temperature"
            range: "0-100"
            unit: "°C"
            accuracy: "±0.5"
            currentValue: 28.5
            status: "Good"
        }
        
        // 液位感測器
        Sensor TowerLevelSensor {
            type: "Level Sensor"
            measurementType: "Level"
            range: "0-10"
            unit: "meters"
            accuracy: "±0.1"
            currentValue: 7.2
            status: "Good"
        }
    }
    
    // 循環幫浦
    Equipment CirculationPump1 {
        type: "Centrifugal Pump"
        manufacturer: "Grundfos"
        model: "CR 64-2"
        serialNumber: "GF2024001"
        power: 22
        powerUnit: "kW"
        voltage: 380
        voltageUnit: "V"
        frequency: 50
        frequencyUnit: "Hz"
        flow: 200
        flowUnit: "m³/h"
        head: 50
        headUnit: "meters"
        efficiency: 0.85
        installDate: "2023-05-20"
        commissionDate: "2023-06-01"
        status: "Running"
        enabled: true
        
        // 幫浦馬達
        Device PumpMotor1 {
            type: "AC Motor"
            power: 22
            powerUnit: "kW"
            voltage: 380
            current: 42
            speed: 1480
            speedUnit: "rpm"
            efficiency: 0.92
            
            // 馬達溫度感測器
            Sensor MotorTempSensor {
                type: "Temperature Sensor"
                measurementType: "Temperature"
                range: "0-150"
                unit: "°C"
                currentValue: 65.3
                alarmHigh: 80
                alarmLow: 0
                status: "Good"
            }
            
            // 馬達振動感測器
            Sensor MotorVibrationSensor {
                type: "Vibration Sensor"
                measurementType: "Vibration"
                range: "0-20"
                unit: "mm/s"
                currentValue: 2.1
                alarmHigh: 10
                status: "Good"
            }
        }
        
        // 出口壓力感測器
        Sensor OutletPressureSensor {
            type: "Pressure Sensor"
            measurementType: "Pressure"
            range: "0-10"
            unit: "bar"
            accuracy: "±0.1"
            currentValue: 5.2
            alarmHigh: 8
            alarmLow: 3
            status: "Good"
        }
        
        // 流量感測器
        Sensor FlowSensor {
            type: "Flow Sensor"
            measurementType: "Flow"
            range: "0-300"
            unit: "m³/h"
            accuracy: "±1.0"
            currentValue: 195.5
            status: "Good"
        }
        
        // 變頻器
        Device VFD1 {
            type: "Variable Frequency Drive"
            manufacturer: "ABB"
            model: "ACS880-01"
            power: 22
            powerUnit: "kW"
            inputVoltage: 380
            outputFrequency: 48.5
            outputFrequencyUnit: "Hz"
            controlMode: "Automatic"
            setpoint: 50
            status: "Running"
        }
    }
    
    // 控制閥
    Actuator ControlValve1 {
        type: "Control Valve"
        valveType: "Butterfly Valve"
        size: "6 inch"
        material: "Stainless Steel 316"
        actuatorType: "Electric"
        controlSignal: "4-20mA"
        position: 75
        positionUnit: "%"
        status: "Operating"
        
        // 閥位置感測器
        Sensor ValvePositionSensor {
            type: "Position Sensor"
            measurementType: "Position"
            range: "0-100"
            unit: "%"
            currentValue: 75.2
            status: "Good"
        }
    }
    
    // PLC 控制器
    Controller SystemPLC {
        type: "Programmable Logic Controller"
        manufacturer: "Siemens"
        model: "S7-1500"
        cpuModel: "CPU 1515-2 PN"
        ipAddress: "192.168.1.100"
        subnetMask: "255.255.255.0"
        gateway: "192.168.1.1"
        firmwareVersion: "2.9.3"
        programVersion: "1.5.2"
        scanTime: 10
        scanTimeUnit: "ms"
        status: "Running"
        enabled: true
    }
}

// 網路連接
Connection EthernetConnection1 {
    from: "CoolingWaterSystem.SystemPLC"
    to: "SCADA_Server"
    type: "Ethernet"
    protocol: "PROFINET"
    ipAddress: "192.168.1.10"
    port: 502
    bandwidth: "100 Mbps"
    status: "Connected"
}

// 控制關係
Relationship PumpControl {
    type: "Controls"
    controller: "CoolingWaterSystem.SystemPLC"
    controlled: "CoolingWaterSystem.CirculationPump1.VFD1"
    controlMode: "PID"
    controlVariable: "Flow"
    setpoint: 200
    setpointUnit: "m³/h"
}

Relationship ValveControl {
    type: "Controls"
    controller: "CoolingWaterSystem.SystemPLC"
    controlled: "CoolingWaterSystem.ControlValve1"
    controlMode: "Automatic"
    controlVariable: "Position"
}
```

### 範例 2: 生產線系統

```iadl
/**
 * 自動化生產線
 * 包含多個工作站和機器人
 */
System AutomatedProductionLine {
    type: "Production Line"
    name: "Line 1"
    description: "Automated assembly line for product manufacturing"
    location: "Factory A, Production Hall 1"
    capacity: 1000
    capacityUnit: "units/hour"
    operatingMode: "Continuous"
    status: "Running"
    
    // 工作站 1: 上料站
    Equipment LoadingStation {
        type: "Loading Station"
        position: 1
        description: "Material loading and preparation"
        
        Device LoadingRobot {
            type: "Industrial Robot"
            manufacturer: "KUKA"
            model: "KR 10 R1100"
            serialNumber: "KR2024001"
            payload: 10
            payloadUnit: "kg"
            reach: 1100
            reachUnit: "mm"
            axes: 6
            repeatability: 0.03
            repeatabilityUnit: "mm"
            status: "Running"
            
            Controller RobotController {
                type: "Robot Controller"
                manufacturer: "KUKA"
                model: "KR C4"
                ipAddress: "192.168.1.101"
                programVersion: "8.6.7"
            }
        }
        
        Sensor PartPresenceSensor {
            type: "Proximity Sensor"
            measurementType: "Presence"
            detectionRange: 10
            detectionRangeUnit: "mm"
            outputType: "Digital"
            status: "Good"
        }
        
        Device Conveyor1 {
            type: "Belt Conveyor"
            length: 2.0
            lengthUnit: "meters"
            width: 0.5
            widthUnit: "meters"
            speed: 0.5
            speedUnit: "m/s"
            motorPower: 0.75
            motorPowerUnit: "kW"
            status: "Running"
        }
    }
    
    // 工作站 2: 組裝站
    Equipment AssemblyStation {
        type: "Assembly Station"
        position: 2
        description: "Main assembly operations"
        
        Device AssemblyRobot1 {
            type: "Industrial Robot"
            manufacturer: "ABB"
            model: "IRB 1200"
            payload: 7
            payloadUnit: "kg"
            reach: 900
            reachUnit: "mm"
            status: "Running"
        }
        
        Device AssemblyRobot2 {
            type: "Industrial Robot"
            manufacturer: "ABB"
            model: "IRB 1200"
            payload: 7
            payloadUnit: "kg"
            reach: 900
            reachUnit: "mm"
            status: "Running"
        }
        
        Device ScrewDriver {
            type: "Electric Screwdriver"
            manufacturer: "Atlas Copco"
            model: "ETV ST61-25-10"
            torqueRange: "1-10"
            torqueRangeUnit: "Nm"
            accuracy: "±5"
            accuracyUnit: "%"
            status: "Ready"
        }
        
        Sensor VisionSystem {
            type: "Vision System"
            manufacturer: "Cognex"
            model: "In-Sight 7000"
            resolution: "800x600"
            frameRate: 60
            frameRateUnit: "fps"
            illuminationType: "LED Ring Light"
            status: "Running"
        }
    }
    
    // 工作站 3: 檢測站
    Equipment InspectionStation {
        type: "Inspection Station"
        position: 3
        description: "Quality inspection and testing"
        
        Device TestEquipment {
            type: "Automated Test Equipment"
            manufacturer: "National Instruments"
            model: "PXI-1045"
            testType: "Functional Test"
            testTime: 30
            testTimeUnit: "seconds"
            status: "Running"
        }
        
        Sensor VisionInspection {
            type: "Vision System"
            manufacturer: "Keyence"
            model: "CV-X200"
            resolution: "1920x1080"
            inspectionType: "Dimensional"
            accuracy: "±0.01"
            accuracyUnit: "mm"
            status: "Running"
        }
        
        Device RejectMechanism {
            type: "Pneumatic Pusher"
            actuatorType: "Pneumatic Cylinder"
            stroke: 100
            strokeUnit: "mm"
            force: 50
            forceUnit: "N"
            status: "Ready"
        }
    }
    
    // 工作站 4: 包裝站
    Equipment PackagingStation {
        type: "Packaging Station"
        position: 4
        description: "Product packaging and labeling"
        
        Device PackagingRobot {
            type: "Industrial Robot"
            manufacturer: "Fanuc"
            model: "M-10iD/12"
            payload: 12
            payloadUnit: "kg"
            reach: 1420
            reachUnit: "mm"
            status: "Running"
        }
        
        Device LabelPrinter {
            type: "Label Printer"
            manufacturer: "Zebra"
            model: "ZT410"
            printWidth: 104
            printWidthUnit: "mm"
            resolution: 203
            resolutionUnit: "dpi"
            status: "Ready"
        }
        
        Device BoxSealer {
            type: "Carton Sealer"
            sealingType: "Tape"
            speed: 20
            speedUnit: "boxes/min"
            status: "Running"
        }
    }
    
    // 主控制器
    Controller LineController {
        type: "Programmable Logic Controller"
        manufacturer: "Siemens"
        model: "S7-1500"
        cpuModel: "CPU 1516-3 PN/DP"
        ipAddress: "192.168.1.110"
        redundancy: true
        backupController: "192.168.1.111"
        firmwareVersion: "2.9.3"
        status: "Running"
    }
    
    // HMI 操作面板
    Device OperatorPanel {
        type: "HMI Panel"
        manufacturer: "Siemens"
        model: "TP1500 Comfort"
        screenSize: 15
        screenSizeUnit: "inch"
        resolution: "1024x768"
        ipAddress: "192.168.1.120"
        status: "Running"
    }
}

// 工業網路
Network ProductionNetwork {
    type: "Industrial Ethernet"
    protocol: "PROFINET"
    topology: "Star"
    bandwidth: "1 Gbps"
    redundancy: true
    
    // 網路交換器
    Device NetworkSwitch {
        type: "Industrial Switch"
        manufacturer: "Siemens"
        model: "SCALANCE X308-2"
        ports: 8
        managementIP: "192.168.1.254"
        status: "Running"
    }
}

// 安全系統
System SafetySystem {
    type: "Safety System"
    description: "Production line safety monitoring and control"
    safetyLevel: "SIL 2"
    
    Controller SafetyPLC {
        type: "Safety PLC"
        manufacturer: "Siemens"
        model: "S7-1500F"
        ipAddress: "192.168.1.130"
        status: "Running"
    }
    
    Sensor EmergencyStop1 {
        type: "Emergency Stop Button"
        location: "Loading Station"
        status: "Normal"
    }
    
    Sensor EmergencyStop2 {
        type: "Emergency Stop Button"
        location: "Assembly Station"
        status: "Normal"
    }
    
    Sensor LightCurtain1 {
        type: "Safety Light Curtain"
        manufacturer: "Sick"
        model: "C4000"
        protectionHeight: 1800
        protectionHeightUnit: "mm"
        range: 4
        rangeUnit: "meters"
        status: "Clear"
    }
}
```

### 範例 3: 電力系統

```iadl
/**
 * 工廠電力配電系統
 * 包含變壓器、配電盤和負載
 */
System PowerDistributionSystem {
    type: "Power Distribution System"
    description: "Main power distribution for factory"
    location: "Factory A, Substation"
    voltage: 22000
    voltageUnit: "V"
    frequency: 60
    frequencyUnit: "Hz"
    
    // 主變壓器
    Equipment MainTransformer {
        type: "Power Transformer"
        manufacturer: "Schneider Electric"
        model: "Trihal"
        serialNumber: "TR2024001"
        capacity: 2500
        capacityUnit: "kVA"
        primaryVoltage: 22000
        primaryVoltageUnit: "V"
        secondaryVoltage: 380
        secondaryVoltageUnit: "V"
        frequency: 60
        frequencyUnit: "Hz"
        phases: 3
        coolingType: "ONAN"
        impedance: 6.0
        impedanceUnit: "%"
        installDate: "2023-03-01"
        commissionDate: "2023-03-15"
        status: "Operating"
        
        // 變壓器溫度監測
        Sensor OilTempSensor {
            type: "Temperature Sensor"
            measurementType: "Oil Temperature"
            range: "0-150"
            unit: "°C"
            currentValue: 65.5
            alarmHigh: 95
            alarmCritical: 105
            status: "Good"
        }
        
        Sensor WindingTempSensor {
            type: "Temperature Sensor"
            measurementType: "Winding Temperature"
            range: "0-200"
            unit: "°C"
            currentValue: 78.2
            alarmHigh: 120
            alarmCritical: 140
            status: "Good"
        }
        
        // 變壓器保護繼電器
        Device ProtectionRelay {
            type: "Protection Relay"
            manufacturer: "ABB"
            model: "REF615"
            functions: ["Overcurrent", "Differential", "Earth Fault"]
            status: "Normal"
        }
    }
    
    // 主配電盤
    Equipment MainSwitchboard {
        type: "Main Distribution Board"
        manufacturer: "Siemens"
        model: "SIVACON S8"
        ratedCurrent: 4000
        ratedCurrentUnit: "A"
        voltage: 380
        voltageUnit: "V"
        busbarMaterial: "Copper"
        ipRating: "IP54"
        
        // 主斷路器
        Device MainCircuitBreaker {
            type: "Air Circuit Breaker"
            manufacturer: "Schneider Electric"
            model: "Masterpact MTZ2"
            ratedCurrent: 4000
            ratedCurrentUnit: "A"
            breakingCapacity: 100
            breakingCapacityUnit: "kA"
            tripUnit: "Micrologic 7.0"
            status: "Closed"
            
            // 電流感測器
            Sensor CurrentSensorL1 {
                type: "Current Transformer"
                phase: "L1"
                ratio: "4000/5"
                currentValue: 2850
                currentUnit: "A"
                status: "Good"
            }
            
            Sensor CurrentSensorL2 {
                type: "Current Transformer"
                phase: "L2"
                ratio: "4000/5"
                currentValue: 2920
                currentUnit: "A"
                status: "Good"
            }
            
            Sensor CurrentSensorL3 {
                type: "Current Transformer"
                phase: "L3"
                ratio: "4000/5"
                currentValue: 2780
                currentUnit: "A"
                status: "Good"
            }
        }
        
        // 電壓感測器
        Sensor VoltageMonitor {
            type: "Voltage Monitor"
            manufacturer: "Schneider Electric"
            model: "PM5560"
            voltageL1L2: 380.5
            voltageL2L3: 381.2
            voltageL3L1: 379.8
            voltageUnit: "V"
            frequency: 60.02
            frequencyUnit: "Hz"
            status: "Good"
        }
        
        // 功率表
        Sensor PowerMeter {
            type: "Power Meter"
            manufacturer: "Schneider Electric"
            model: "PM5560"
            activePower: 1850
            activePowerUnit: "kW"
            reactivePower: 420
            reactivePowerUnit: "kVAr"
            apparentPower: 1897
            apparentPowerUnit: "kVA"
            powerFactor: 0.975
            energy: 125680
            energyUnit: "kWh"
            status: "Good"
        }
    }
    
    // 配電迴路 1: 生產線
    Equipment ProductionFeeder {
        type: "Distribution Feeder"
        name: "Production Line Feeder"
        load: "Production Line 1"
        
        Device Breaker1 {
            type: "Molded Case Circuit Breaker"
            manufacturer: "ABB"
            model: "Tmax XT4"
            ratedCurrent: 630
            ratedCurrentUnit: "A"
            breakingCapacity: 50
            breakingCapacityUnit: "kA"
            status: "Closed"
        }
        
        Sensor CurrentMonitor1 {
            type: "Current Monitor"
            currentL1: 425
            currentL2: 438
            currentL3: 412
            currentUnit: "A"
            status: "Good"
        }
    }
    
    // 配電迴路 2: 空調系統
    Equipment HVACFeeder {
        type: "Distribution Feeder"
        name: "HVAC System Feeder"
        load: "HVAC System"
        
        Device Breaker2 {
            type: "Molded Case Circuit Breaker"
            manufacturer: "Schneider Electric"
            model: "NSX400F"
            ratedCurrent: 400
            ratedCurrentUnit: "A"
            breakingCapacity: 36
            breakingCapacityUnit: "kA"
            status: "Closed"
        }
        
        Sensor CurrentMonitor2 {
            type: "Current Monitor"
            currentL1: 285
            currentL2: 292
            currentL3: 278
            currentUnit: "A"
            status: "Good"
        }
    }
    
    // 配電迴路 3: 照明系統
    Equipment LightingFeeder {
        type: "Distribution Feeder"
        name: "Lighting System Feeder"
        load: "Lighting System"
        
        Device Breaker3 {
            type: "Miniature Circuit Breaker"
            manufacturer: "ABB"
            model: "S203-C100"
            ratedCurrent: 100
            ratedCurrentUnit: "A"
            poles: 3
            curve: "C"
            status: "Closed"
        }
        
        Sensor CurrentMonitor3 {
            type: "Current Monitor"
            currentL1: 65
            currentL2: 68
            currentL3: 62
            currentUnit: "A"
            status: "Good"
        }
    }
    
    // 備用發電機
    Equipment BackupGenerator {
        type: "Diesel Generator"
        manufacturer: "Cummins"
        model: "C2000 D5"
        capacity: 2000
        capacityUnit: "kVA"
        voltage: 380
        voltageUnit: "V"
        frequency: 60
        frequencyUnit: "Hz"
        fuelType: "Diesel"
        fuelTankCapacity: 1000
        fuelTankCapacityUnit: "liters"
        runningHours: 1250
        lastMaintenanceDate: "2024-08-15"
        nextMaintenanceDate: "2024-11-15"
        status: "Standby"
        
        Device GeneratorController {
            type: "Generator Controller"
            manufacturer: "ComAp"
            model: "InteliGen 200"
            autoStart: true
            autoTransfer: true
            status: "Ready"
        }
    }
    
    // 自動轉換開關
    Device AutomaticTransferSwitch {
        type: "Automatic Transfer Switch"
        manufacturer: "ASCO"
        model: "7000 Series"
        ratedCurrent: 4000
        ratedCurrentUnit: "A"
        transferTime: 10
        transferTimeUnit: "seconds"
        currentSource: "Utility"
        status: "Normal"
    }
    
    // 不斷電系統
    Equipment UPS {
        type: "Uninterruptible Power Supply"
        manufacturer: "Eaton"
        model: "9395"
        capacity: 550
        capacityUnit: "kVA"
        voltage: 380
        voltageUnit: "V"
        topology: "Double Conversion"
        batteryType: "VRLA"
        batteryCapacity: 200
        batteryCapacityUnit: "Ah"
        backupTime: 15
        backupTimeUnit: "minutes"
        efficiency: 0.96
        status: "Online"
        
        Sensor BatteryMonitor {
            type: "Battery Monitor"
            voltage: 480
            voltageUnit: "V"
            current: 0
            currentUnit: "A"
            temperature: 25
            temperatureUnit: "°C"
            chargeLevel: 100
            chargeLevelUnit: "%"
            status: "Good"
        }
    }
}
```

---

## 最佳實踐

### 命名慣例

使用描述性的名稱,清楚表達資產的功能或位置。例如,使用 `CoolingTowerPump1` 而不是 `Pump1`,使用 `ProductionLineRobot` 而不是 `Robot`。

對於同類型的多個資產,使用數字後綴區分。例如,`Pump1`、`Pump2`、`Pump3`,或使用位置資訊,如 `NorthPump`、`SouthPump`。

使用一致的命名風格,建議使用駝峰式命名法 (PascalCase) 用於資產名稱,駝峰式命名法 (camelCase) 用於屬性名稱。

### 結構組織

按照邏輯階層組織資產,從系統到子系統,再到設備和裝置。將相關的資產組織在同一個父資產下,例如,將所有感測器放在被監控的設備下。

使用系統 (System) 來組織大型結構,使用設備 (Equipment) 來表示實體設備,使用裝置 (Device) 來表示控制或監控元件。

### 屬性定義

為每個資產提供足夠的識別資訊,包括 id、name、type 和 description。記錄重要的技術參數,如 manufacturer、model、serialNumber、capacity 等。

包含狀態資訊,如 status、enabled、quality,以便監控和診斷。記錄維護資訊,如 installDate、lastMaintenanceDate、nextMaintenanceDate,以支援維護管理。

為數值屬性提供單位,使用單獨的屬性 (如 powerUnit) 或在屬性名稱中包含單位資訊。

### 註解使用

在每個主要資產定義前加入文件註解,說明其用途和功能。使用單行註解解釋複雜的屬性或特殊設定。使用多行註解提供詳細的說明或注意事項。

記錄作者、日期和版本資訊,特別是對於重要的系統定義。

### 關係定義

明確定義資產之間的連接關係,包括通訊、電氣和機械連接。使用邏輯關係表示控制、監控和互鎖關係。

為連接和關係提供足夠的技術細節,如協定、頻寬、控制模式等。

### 模組化設計

將大型系統分解為多個模組或子系統,每個模組專注於特定功能。使用參考和引用避免重複定義,提高可維護性。

考慮建立可重用的範本或標準定義,用於常見的設備類型。

### 版本控制

使用版本控制系統 (如 Git) 管理 IADL 檔案,追蹤變更歷史。在檔案開頭記錄版本資訊和變更日誌。

為重大變更建立新版本,保留舊版本以供參考。

### 驗證和測試

定期驗證 IADL 檔案的語法正確性,使用 IADL Editor 的語法檢查功能。測試匯入/匯出功能,確保資料能夠正確轉換。

建立測試案例,驗證複雜的階層結構和關係定義。

### 文件維護

保持 IADL 檔案與實際系統同步,及時更新變更。記錄重要的設計決策和假設,便於未來維護。

提供使用說明和範例,幫助其他工程師理解和使用 IADL 定義。

---

## 附錄

### 常見錯誤

**缺少必要屬性**: 確保每個資產都有基本的識別資訊,如 id、name 和 type。

**不一致的命名**: 保持命名風格的一致性,避免混用不同的命名慣例。

**過深的階層**: 避免建立過於複雜的階層結構,通常 3-5 層已足夠。

**缺少單位**: 為數值屬性提供明確的單位,避免混淆。

**重複定義**: 使用參考和引用避免重複定義相同的資產。

### 工具和資源

**IADL Editor**: 官方的 IADL 編輯器,提供語法高亮、自動完成和語法檢查功能。

**GitHub 儲存庫**: https://github.com/chchlin1018/IADL-Editor-Test1

**範例檔案**: 在 IADL Editor 的 examples 目錄中可以找到更多範例。

### 聯絡方式

如有問題或建議,請透過 GitHub Issues 聯絡我們:
https://github.com/chchlin1018/IADL-Editor-Test1/issues

---

**版本**: 1.0  
**日期**: 2024-10-09  
**作者**: Manus AI Assistant  
**授權**: MIT License

---

*本手冊由 Manus AI Assistant 自動生成*
