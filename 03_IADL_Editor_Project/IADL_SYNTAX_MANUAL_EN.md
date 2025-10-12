# IADL Syntax Manual

## Table of Contents

1. [Introduction](#introduction)
2. [Basic Syntax](#basic-syntax)
3. [Asset Definition](#asset-definition)
4. [Property Definition](#property-definition)
5. [Relationship Definition](#relationship-definition)
6. [Hierarchical Structure](#hierarchical-structure)
7. [Data Types](#data-types)
8. [Comments](#comments)
9. [Keyword Reference](#keyword-reference)
10. [Complete Examples](#complete-examples)
11. [Best Practices](#best-practices)

---

## Introduction

IADL (Industrial Asset Description Language) is a description language specifically designed for industrial asset management. It provides concise yet powerful syntax for defining industrial assets, properties, relationships, and hierarchical structures. The design goal of IADL is to enable engineers to quickly create and maintain industrial asset models while maintaining good readability and maintainability.

### Design Philosophy

IADL adopts declarative syntax, emphasizing the structure and relationships of assets rather than implementation details. The language design follows these principles: simplicity (minimizing syntax elements), readability (natural language-like expression), extensibility (supporting custom properties and types), interoperability (supporting import/export with multiple industrial systems), and hierarchy (supporting complex asset organizational structures).

### Application Scenarios

IADL is suitable for various industrial application scenarios, including SCADA system tag management, factory equipment asset modeling, Building Information Model (BIM) equipment description, power system network topology definition, Manufacturing Execution System (MES) resource management, and Industrial Internet of Things (IIoT) device description.

---

## Basic Syntax

### Syntax Structure

An IADL document consists of a series of asset definitions. Each asset definition contains asset type, asset name, property list, and optional child assets. The basic syntax structure is as follows:

```
AssetType AssetName {
    property: value
    property: value
    ...
}
```

### Naming Rules

Asset names and property names follow these rules: must start with a letter or underscore, can contain letters, numbers, and underscores, case-sensitive, recommended to use camelCase or snake_case naming conventions.

### Reserved Words

The following are IADL reserved words and should not be used as asset names or property names: Asset, Equipment, Device, Sensor, Actuator, Controller, System, Network, Connection, Relationship, Parent, Child, Type, Name, ID, Description, Value, Unit, Status, Quality, Timestamp.

---

## Asset Definition

### Basic Asset Definition

Assets are the core concept of IADL, representing any physical or logical component in an industrial environment. A basic asset definition contains asset type and asset name.

**Syntax**:
```
AssetType AssetName {
    // Property definitions
}
```

**Example**:
```iadl
Equipment Tank1 {
    type: "Storage Tank"
    capacity: 10000
    unit: "liters"
}
```

### Asset Types

IADL supports multiple predefined asset types, each with specific purposes and properties.

#### Equipment

Represents physical equipment such as motors, pumps, valves, etc.

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

#### Device

Represents control or monitoring devices such as PLCs, sensors, actuators, etc.

```iadl
Device PLC1 {
    type: "Programmable Logic Controller"
    manufacturer: "Siemens"
    model: "S7-1500"
    ipAddress: "192.168.1.100"
    firmwareVersion: "2.9.3"
}
```

#### Sensor

Represents measurement devices such as temperature sensors, pressure sensors, etc.

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

#### Actuator

Represents devices that perform actions, such as valves, motor controllers, etc.

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

#### Controller

Represents control systems such as DCS, SCADA servers, etc.

```iadl
Controller DCS1 {
    type: "Distributed Control System"
    manufacturer: "Honeywell"
    model: "Experion PKS"
    serverAddress: "192.168.1.10"
    redundancy: true
}
```

#### System

Represents logical systems or subsystems such as cooling systems, power supply systems, etc.

```iadl
System CoolingSystem {
    type: "Cooling System"
    description: "Main cooling system for reactor"
    operatingMode: "Continuous"
    designCapacity: 5000
    capacityUnit: "kW"
}
```

#### Network

Represents communication networks such as Ethernet, Fieldbus, etc.

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

## Property Definition

### Basic Properties

Properties are used to describe the characteristics and state of assets. Each property consists of a property name and property value, separated by a colon.

**Syntax**:
```
propertyName: propertyValue
```

**Example**:
```iadl
Equipment Motor1 {
    type: "AC Motor"
    power: 30
    voltage: 380
    frequency: 50
    speed: 1500
}
```

### Common Properties

The following are commonly used properties in IADL and their descriptions:

#### Identification Properties

- **id**: Unique identifier
- **name**: Asset name
- **type**: Asset type
- **description**: Description

```iadl
Equipment Compressor1 {
    id: "COMP-001"
    name: "Main Air Compressor"
    type: "Screw Compressor"
    description: "Primary compressed air supply for production line"
}
```

#### Technical Properties

- **manufacturer**: Manufacturer
- **model**: Model
- **serialNumber**: Serial number
- **capacity**: Capacity
- **power**: Power
- **voltage**: Voltage
- **current**: Current
- **pressure**: Pressure
- **temperature**: Temperature
- **flow**: Flow rate

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

#### Status Properties

- **status**: Status (Running, Stopped, Fault, Maintenance)
- **enabled**: Enabled state (true/false)
- **quality**: Data quality (Good, Bad, Uncertain)
- **value**: Current value
- **timestamp**: Timestamp

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

#### Location Properties

- **location**: Location
- **building**: Building
- **floor**: Floor
- **room**: Room
- **coordinates**: Coordinates

```iadl
Equipment Boiler1 {
    location: "Plant A"
    building: "Utility Building"
    floor: "Ground Floor"
    room: "Boiler Room 1"
    coordinates: "23.5,121.0,10.5"
}
```

#### Maintenance Properties

- **installDate**: Installation date
- **commissionDate**: Commission date
- **lastMaintenanceDate**: Last maintenance date
- **nextMaintenanceDate**: Next maintenance date
- **maintenanceInterval**: Maintenance interval
- **warranty**: Warranty period

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

### Custom Properties

IADL supports custom properties, allowing users to define additional properties based on specific requirements. Custom property naming should follow naming rules and use descriptive names.

```iadl
Equipment CustomEquipment1 {
    // Standard properties
    type: "Custom Equipment"
    
    // Custom properties
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

## Relationship Definition

### Parent-Child Relationships

IADL supports hierarchical parent-child relationships to represent containment or composition relationships between assets. Child assets are defined within the parent asset's braces.

**Syntax**:
```
ParentAsset ParentName {
    property: value
    
    ChildAsset ChildName {
        property: value
    }
}
```

**Example**:
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

### Connection Relationships

Connection relationships represent communication, electrical, or mechanical connections between assets.

**Syntax**:
```
Connection ConnectionName {
    from: "SourceAsset"
    to: "TargetAsset"
    type: "ConnectionType"
    properties...
}
```

**Example**:
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

### Logical Relationships

Logical relationships represent logical dependencies or control relationships between assets.

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

## Hierarchical Structure

### Multi-level Hierarchy

IADL supports hierarchical structures of arbitrary depth to represent complex industrial system organizations.

**Example**:
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

### References

IADL supports asset references, allowing the same asset to be referenced in different locations, avoiding duplicate definitions.

**Syntax**:
```
reference: "AssetPath"
```

**Example**:
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
        // Or use ID reference
        controllerID: "PLC-001"
    }
}
```

---

## Data Types

### Basic Data Types

IADL supports the following basic data types:

#### String

Used to represent text data, enclosed in double quotes.

```iadl
Equipment Example1 {
    name: "Equipment Name"
    description: "This is a description"
    manufacturer: "Company Name"
}
```

#### Number

Used to represent integers or floating-point numbers.

```iadl
Equipment Example2 {
    power: 30
    voltage: 380.5
    efficiency: 0.95
    temperature: -10.5
}
```

#### Boolean

Used to represent true or false, using `true` or `false`.

```iadl
Equipment Example3 {
    enabled: true
    running: false
    alarm: false
    maintenance: true
}
```

#### DateTime

Used to represent date and time, using ISO 8601 format.

```iadl
Equipment Example4 {
    installDate: "2024-01-15"
    commissionDate: "2024-01-20T10:30:00Z"
    lastMaintenanceDate: "2024-09-01T14:00:00+08:00"
}
```

### Composite Data Types

#### Array

Used to represent a list of multiple values, enclosed in square brackets, with values separated by commas.

```iadl
Equipment Example5 {
    tags: ["Production", "Critical", "High Priority"]
    alarmLevels: [10, 20, 30, 40]
    operators: ["John", "Mary", "Bob"]
}
```

#### Object

Used to represent structured data, enclosed in braces.

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

### Unit Representation

IADL recommends using separate properties to represent units to maintain data structure.

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

## Comments

### Single-line Comments

Use double slashes `//` to start a single-line comment, which continues to the end of the line.

```iadl
// This is a single-line comment
Equipment Pump1 {
    type: "Centrifugal Pump"  // This is also a comment
    power: 15  // Power 15 kW
}
```

### Multi-line Comments

Use `/*` to start and `*/` to end multi-line comments.

```iadl
/*
 * This is a multi-line comment
 * Can span multiple lines
 * Used for detailed explanations
 */
Equipment ComplexEquipment {
    /*
     * The following are the main parameters of the equipment
     * These parameters are determined during the design phase
     */
    type: "Complex Equipment"
    capacity: 1000
}
```

### Documentation Comments

It is recommended to add documentation comments before each major asset definition, explaining the asset's purpose, function, and notes.

```iadl
/**
 * Main Production Line Control System
 * 
 * Functions:
 * - Control all equipment on the production line
 * - Monitor production process
 * - Record production data
 * 
 * Notes:
 * - Requires regular program backup
 * - Production must be stopped during maintenance
 * 
 * @author: John Doe
 * @date: 2024-10-09
 * @version: 1.0
 */
System ProductionLineControl {
    type: "Control System"
    // ... Property definitions
}
```

---

## Keyword Reference

### Asset Type Keywords

| Keyword | Description | Purpose |
|---------|-------------|---------|
| Asset | Generic Asset | Represents any type of asset |
| Equipment | Equipment | Represents physical equipment |
| Device | Device | Represents control or monitoring device |
| Sensor | Sensor | Represents measurement device |
| Actuator | Actuator | Represents device that performs actions |
| Controller | Controller | Represents control system |
| System | System | Represents logical system or subsystem |
| Network | Network | Represents communication network |
| Connection | Connection | Represents connection between assets |
| Relationship | Relationship | Represents logical relationship between assets |

### Property Keywords

| Keyword | Data Type | Description |
|---------|-----------|-------------|
| id | String | Unique identifier |
| name | String | Name |
| type | String | Type |
| description | String | Description |
| manufacturer | String | Manufacturer |
| model | String | Model |
| serialNumber | String | Serial number |
| status | String | Status |
| enabled | Boolean | Enabled state |
| value | Number | Value |
| unit | String | Unit |
| quality | String | Quality |
| timestamp | DateTime | Timestamp |
| location | String | Location |
| capacity | Number | Capacity |
| power | Number | Power |
| voltage | Number | Voltage |
| current | Number | Current |
| pressure | Number | Pressure |
| temperature | Number | Temperature |
| flow | Number | Flow rate |

### Relationship Keywords

| Keyword | Description |
|---------|-------------|
| from | Connection source |
| to | Connection target |
| parent | Parent asset |
| child | Child asset |
| controller | Controller |
| controlled | Controlled object |
| monitor | Monitor |
| monitored | Monitored object |
| reference | Asset reference |

---

## Complete Examples

### Example 1: Simple Pump System

```iadl
/**
 * Cooling Water Pump System
 * Used for factory cooling water circulation
 */
System CoolingWaterSystem {
    type: "Cooling Water System"
    description: "Main cooling water circulation system"
    location: "Plant A, Utility Building"
    
    // Cooling Tower
    Equipment CoolingTower1 {
        type: "Cooling Tower"
        capacity: 500
        capacityUnit: "m³"
        manufacturer: "Baltimore Aircoil"
        model: "VTI-1500"
        installDate: "2023-05-15"
        
        // Temperature Sensor
        Sensor TowerTempSensor {
            type: "Temperature Sensor"
            measurementType: "Temperature"
            range: "0-100"
            unit: "°C"
            accuracy: "±0.5"
            currentValue: 28.5
            status: "Good"
        }
        
        // Level Sensor
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
    
    // Circulation Pump
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
        
        // Pump Motor
        Device PumpMotor1 {
            type: "AC Motor"
            power: 22
            powerUnit: "kW"
            voltage: 380
            current: 42
            speed: 1480
            speedUnit: "rpm"
            efficiency: 0.92
            
            // Motor Temperature Sensor
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
            
            // Motor Vibration Sensor
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
        
        // Outlet Pressure Sensor
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
        
        // Flow Sensor
        Sensor FlowSensor {
            type: "Flow Sensor"
            measurementType: "Flow"
            range: "0-300"
            unit: "m³/h"
            accuracy: "±1.0"
            currentValue: 195.5
            status: "Good"
        }
        
        // Variable Frequency Drive
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
    
    // Control Valve
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
        
        // Valve Position Sensor
        Sensor ValvePositionSensor {
            type: "Position Sensor"
            measurementType: "Position"
            range: "0-100"
            unit: "%"
            currentValue: 75.2
            status: "Good"
        }
    }
    
    // PLC Controller
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

// Network Connection
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

// Control Relationship
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

### Example 2: Production Line System

```iadl
/**
 * Automated Production Line
 * Contains multiple workstations and robots
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
    
    // Station 1: Loading Station
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
    
    // Station 2: Assembly Station
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
    
    // Station 3: Inspection Station
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
    
    // Station 4: Packaging Station
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
    
    // Main Controller
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
    
    // HMI Operator Panel
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

// Industrial Network
Network ProductionNetwork {
    type: "Industrial Ethernet"
    protocol: "PROFINET"
    topology: "Star"
    bandwidth: "1 Gbps"
    redundancy: true
    
    // Network Switch
    Device NetworkSwitch {
        type: "Industrial Switch"
        manufacturer: "Siemens"
        model: "SCALANCE X308-2"
        ports: 8
        managementIP: "192.168.1.254"
        status: "Running"
    }
}

// Safety System
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

### Example 3: Power Distribution System

```iadl
/**
 * Factory Power Distribution System
 * Includes transformers, distribution panels, and loads
 */
System PowerDistributionSystem {
    type: "Power Distribution System"
    description: "Main power distribution for factory"
    location: "Factory A, Substation"
    voltage: 22000
    voltageUnit: "V"
    frequency: 60
    frequencyUnit: "Hz"
    
    // Main Transformer
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
        
        // Transformer Temperature Monitoring
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
        
        // Transformer Protection Relay
        Device ProtectionRelay {
            type: "Protection Relay"
            manufacturer: "ABB"
            model: "REF615"
            functions: ["Overcurrent", "Differential", "Earth Fault"]
            status: "Normal"
        }
    }
    
    // Main Switchboard
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
        
        // Main Circuit Breaker
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
            
            // Current Sensors
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
        
        // Voltage Monitor
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
        
        // Power Meter
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
    
    // Distribution Circuit 1: Production Line
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
    
    // Distribution Circuit 2: HVAC System
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
    
    // Distribution Circuit 3: Lighting System
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
    
    // Backup Generator
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
    
    // Automatic Transfer Switch
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
    
    // Uninterruptible Power Supply
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

## Best Practices

### Naming Conventions

Use descriptive names that clearly express the asset's function or location. For example, use `CoolingTowerPump1` instead of `Pump1`, use `ProductionLineRobot` instead of `Robot`.

For multiple assets of the same type, use numeric suffixes to distinguish them. For example, `Pump1`, `Pump2`, `Pump3`, or use location information such as `NorthPump`, `SouthPump`.

Use consistent naming style. It is recommended to use PascalCase for asset names and camelCase for property names.

### Structure Organization

Organize assets according to logical hierarchy, from systems to subsystems, then to equipment and devices. Group related assets under the same parent asset. For example, place all sensors under the monitored equipment.

Use System to organize large structures, use Equipment to represent physical equipment, and use Device to represent control or monitoring components.

### Property Definition

Provide sufficient identification information for each asset, including id, name, type, and description. Record important technical parameters such as manufacturer, model, serialNumber, capacity, etc.

Include status information such as status, enabled, quality for monitoring and diagnostics. Record maintenance information such as installDate, lastMaintenanceDate, nextMaintenanceDate to support maintenance management.

Provide units for numeric properties using separate properties (such as powerUnit) or include unit information in the property name.

### Comment Usage

Add documentation comments before each major asset definition, explaining its purpose and function. Use single-line comments to explain complex properties or special configurations. Use multi-line comments to provide detailed explanations or notes.

Record author, date, and version information, especially for important system definitions.

### Relationship Definition

Clearly define connection relationships between assets, including communication, electrical, and mechanical connections. Use logical relationships to represent control, monitoring, and interlock relationships.

Provide sufficient technical details for connections and relationships, such as protocol, bandwidth, control mode, etc.

### Modular Design

Break down large systems into multiple modules or subsystems, each focusing on specific functions. Use references to avoid duplicate definitions and improve maintainability.

Consider creating reusable templates or standard definitions for common equipment types.

### Version Control

Use version control systems (such as Git) to manage IADL files and track change history. Record version information and change logs at the beginning of files.

Create new versions for major changes and keep old versions for reference.

### Validation and Testing

Regularly validate the syntax correctness of IADL files using IADL Editor's syntax checking features. Test import/export functionality to ensure data can be correctly converted.

Create test cases to validate complex hierarchical structures and relationship definitions.

### Documentation Maintenance

Keep IADL files synchronized with actual systems and update changes promptly. Document important design decisions and assumptions for future maintenance.

Provide usage instructions and examples to help other engineers understand and use IADL definitions.

---

## Appendix

### Common Errors

**Missing Required Properties**: Ensure each asset has basic identification information such as id, name, and type.

**Inconsistent Naming**: Maintain consistent naming style and avoid mixing different naming conventions.

**Overly Deep Hierarchy**: Avoid creating overly complex hierarchical structures; typically 3-5 levels are sufficient.

**Missing Units**: Provide explicit units for numeric properties to avoid confusion.

**Duplicate Definitions**: Use references to avoid duplicate definitions of the same asset.

### Tools and Resources

**IADL Editor**: Official IADL editor providing syntax highlighting, auto-completion, and syntax checking features.

**GitHub Repository**: https://github.com/chchlin1018/IADL-Editor-Test1

**Example Files**: More examples can be found in the examples directory of IADL Editor.

### Contact

For questions or suggestions, please contact us through GitHub Issues:
https://github.com/chchlin1018/IADL-Editor-Test1/issues

---

**Version**: 1.0  
**Date**: 2024-10-09  
**Author**: Manus AI Assistant  
**License**: MIT License

---

*This manual is automatically generated by Manus AI Assistant*
