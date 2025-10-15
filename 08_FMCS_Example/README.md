

# 半導體廠房 FMCS 設計文件

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)  
**日期**: 2025年10月14日  
**版本**: 1.1 (新增 USD 3D 模型定義)

---

## 1. 簡介

本文件旨在運用 IDTF (Industrial Digital Twin Framework) 的核心技術 - IADL (Industrial Asset Description Language) 和 FDL (Factory Design Language),為一座先進的半導體廠房設計一套完整的 FMCS (Facility Monitoring and Control System, 設施監控系統)。

### 1.1 設計目標

- **標準化**: 建立統一的 FMCS 資產模型,涵蓋所有廠務系統。
- **模組化**: 將複雜的 FMCS 系統分解為獨立、可重用的模組。
- **可擴展性**: 支援未來新系統的快速整合。
- **可維護性**: 透過 IADL 和 FDL 簡化系統的維護和升級。
- **數位分身整合**: 為 NDH (Neutral Data Hub) 和 Omniverse 視覺化提供基礎。

### 1.2 適用範圍

本設計涵蓋半導體廠房的主要廠務系統,包括:
- **HVAC**: 暖通空調系統
- **PCW**: 製程冷卻水系統
- **UPW**: 超純水系統
- **WWT**: 廢水處理系統
- **GAS**: 特氣供應系統
- **POWER**: 電力監控系統

---

## 2. FMCS 系統架構與資產階層

### 2.1 系統架構

FMCS 系統採用分層架構,確保各層職責清晰,易於管理。

```
┌─────────────────────────────────────────────────────────┐
│                  應用層 (Applications)                     │
│  - 能源管理系統 (EMS)                                    │
│  - 告警管理系統 (AMS)                                    │
│  - 維護管理系統 (CMMS)                                   │
│  - 數位分身儀表板 (Digital Twin Dashboard)               │
└────────────────────┬────────────────────────────────────────┘
                     ↓ (NDH API)
┌─────────────────────────────────────────────────────────┐
│                  NDH (Neutral Data Hub)                    │
│  - Asset Servant (虛擬分身物件)                          │
│  - 時序資料庫 (TDengine/PI)                              │
│  - 元數據資料庫 (PostgreSQL)                             │
└────────────────────┬────────────────────────────────────────┘
                     ↓ (IADL/FDL)
┌─────────────────────────────────────────────────────────┐
│                FMCS 系統層 (IADL 定義)                   │
│  - HVAC System                                           │
│  - PCW System                                            │
│  - UPW System                                            │
│  - WWT System                                            │
│  - GAS System                                            │
│  - POWER System                                          │
└────────────────────┬────────────────────────────────────────┘
                     ↓ (實體設備)
┌─────────────────────────────────────────────────────────┐
│                  現場設備層 (Physical Layer)               │
│  - Chiller, Pump, MAU, AHU, Sensor, Valve, Scrubber     │
└─────────────────────────────────────────────────────────┘
```

### 2.2 資產階層設計 (Asset Hierarchy)

FMCS 的資產階層採用由上而下的設計,從廠房、系統、子系統到設備,逐層細化。

```
Fab1 (半導體廠房)
├── HVAC_System (暖通空調系統)
│   ├── Chiller_Subsystem (冰水主機子系統)
│   │   ├── Equipment: Chiller-01, Chiller-02
│   │   ├── Equipment: Chiller_Pump-01, Chiller_Pump-02
│   │   └── Sensor: CHW_Supply_Temp, CHW_Return_Temp
│   ├── MAU_Subsystem (外氣空調箱子系統)
│   │   ├── Equipment: MAU-01, MAU-02
│   │   └── Sensor: Outdoor_Air_Temp, Outdoor_Air_Humidity
│   └── FFU_Subsystem (風機濾網機組子系統)
│       ├── Equipment: FFU-ZoneA-001, FFU-ZoneA-002
│       └── Sensor: Cleanroom_Pressure, Cleanroom_Particle_Count
├── PCW_System (製程冷卻水系統)
│   ├── PCW_Pump_Subsystem (泵浦子系統)
│   │   ├── Equipment: PCW_Pump-01, PCW_Pump-02
│   │   └── Sensor: PCW_Supply_Pressure, PCW_Return_Pressure
│   └── Heat_Exchanger_Subsystem (熱交換器子系統)
│       ├── Equipment: HEX-01, HEX-02
│       └── Sensor: PCW_Temp_In, PCW_Temp_Out
├── UPW_System (超純水系統)
│   ├── RO_Subsystem (逆滲透子系統)
│   │   ├── Equipment: RO_Unit-01
│   │   └── Sensor: RO_Conductivity, RO_TOC
│   └── EDI_Subsystem (電去離子子系統)
│       ├── Equipment: EDI_Unit-01
│       └── Sensor: EDI_Resistivity
├── WWT_System (廢水處理系統)
│   ├── Collection_Subsystem (收集子系統)
│   │   ├── Equipment: Lift_Station-01
│   │   └── Sensor: Sump_Level
│   └── Treatment_Subsystem (處理子系統)
│       ├── Equipment: MBR-01 (薄膜生物反應器)
│       └── Sensor: pH_Sensor, COD_Sensor
├── GAS_System (特氣供應系統)
│   ├── Bulk_Gas_Subsystem (大宗氣體子系統)
│   │   ├── Equipment: N2_Tank, O2_Tank
│   │   └── Sensor: N2_Tank_Pressure, O2_Tank_Level
│   └── Specialty_Gas_Subsystem (特殊氣體子系統)
│       ├── Equipment: Gas_Cabinet-NH3, Gas_Cabinet-SiH4
│       └── Sensor: NH3_Gas_Detector, SiH4_Gas_Detector
└── POWER_System (電力監控系統)
    ├── Transformer_Subsystem (變壓器子系統)
    │   ├── Equipment: TR-01, TR-02
    │   └── Sensor: TR_Oil_Temp, TR_Load
    └── Switchgear_Subsystem (開關設備子系統)
        ├── Equipment: SWGR-01, SWGR-02
        └── Sensor: Breaker_Status, Bus_Voltage
```

---



---

## 3. IADL 定義

本章節提供 FMCS 各主要系統的 IADL (Industrial Asset Description Language) 定義。這些定義檔建立了資產的標準化模型,包含技術規格、數據標籤 (Data Tags) 和行為邏輯 (Behaviors)。

### 3.1 IADL 檔案結構

IADL 檔案以 `.iadl` 為副檔名,採用宣告式語法,描述資產的靜態屬性和動態行為。

- **hvac.iadl**: 定義暖通空調系統 (HVAC) 的相關資產。
- **upw.iadl**: 定義超純水系統 (UPW) 的相關資產。
- **pcw.iadl**: 定義製程冷卻水系統 (PCW) 的相關資產。
- **wwt.iadl**: 定義廢水處理系統 (WWT) 的相關資產。
- **gas.iadl**: 定義氣體供應系統 (GAS) 的相關資產。
- **power.iadl**: 定義電力監控系統 (POWER) 的相關資產。

### 3.2 HVAC 系統 IADL 範例 (`hvac.iadl`)

```yaml
/*
 * IADL Definition for HVAC System
 * Version: 1.1 (Added USD 3D Models)
 * Author: Manus AI
 */

System HVAC_System {
    description: "Heating, Ventilation, and Air Conditioning System"
    
    Subsystem Chiller_Subsystem {
        description: "Chilled Water Subsystem"
        
        Equipment Chiller-01 {
            type: "Centrifugal Chiller"
            manufacturer: "Trane"
            model: "CVHF1250"
            capacity: 1250 // tons
            
            // 3D Model (USD) - 新增!
            geometry: {
                usd_reference: "omniverse://fab1/assets/hvac/chiller_trane_cvhf1250.usd",
                dimensions: {
                    length: 5.2,
                    width: 2.4,
                    height: 2.8
                }
            }
            
            data_tags: [
                { tag_id: "chw_supply_temp", source: { type: "opc_ua", node_id: "ns=2;s=CH01.CHWST" } },
                { tag_id: "chw_return_temp", source: { type: "opc_ua", node_id: "ns=2;s=CH01.CHWRT" } },
                { tag_id: "power_consumption", source: { type: "modbus", address: 40001 } }
            ]
            
            behaviors: [
                {
                    type: "alarm_rule",
                    name: "high_chw_supply_temp_alarm",
                    condition: "chw_supply_temp > 7.0",
                    severity: "high",
                    message: "Chilled water supply temperature is too high"
                }
            ]
        }
    }
}
```

### 3.3 UPW 系統 IADL 範例 (`upw.iadl`)

```yaml
/*
 * IADL Definition for UPW System
 * Version: 1.0
 * Author: Manus AI
 */

System UPW_System {
    description: "Ultra-Pure Water System"
    
    Subsystem EDI_Subsystem {
        description: "Electrodeionization Subsystem"
        
        Equipment EDI_Unit-01 {
            type: "Electrodeionization Unit"
            manufacturer: "Suez"
            model: "E-Cell MK-7"
            
            data_tags: [
                { tag_id: "product_resistivity", source: { type: "opc_ua", node_id: "ns=2;s=EDI01.Res" } },
                { tag_id: "product_toc", source: { type: "opc_ua", node_id: "ns=2;s=EDI01.TOC" } }
            ]
            
            behaviors: [
                {
                    type: "alarm_rule",
                    name: "low_resistivity_alarm",
                    condition: "product_resistivity < 18.0",
                    severity: "critical",
                    message: "UPW product water resistivity is too low"
                }
            ]
        }
    }
}
```

---



## 4. FDL 設計

本章節使用 FDL (Factory Design Language) 來定義半導體廠房的實體佈局和資產實例化。FDL 檔案描述了 IADL 中定義的資產如何在廠房中被實例化、放置在何處,以及它們之間的連接關係。

### 4.1 FDL 檔案結構

FDL 檔案以 `.fdl` 為副檔名,採用 YAML 格式,定義了廠房的建築結構、區域劃分、資產實例和關係。

- **fab1.fdl**: 定義 Fab 1 廠房的完整佈局。

### 4.2 Fab 1 廠房 FDL 範例 (`fab1.fdl`)

```yaml
factory_design:
  metadata:
    name: "Semiconductor Fab 1"
    version: "1.0"
    author: "Manus AI"
    date: "2025-10-14"
    description: "Facility layout for a 12-inch semiconductor fab"

  buildings:
    - building_id: "FAB1"
      name: "Main Fab Building"
      location: "Hsinchu Science Park"
      floors:
        - floor_id: "L1"
          name: "Level 1 - Utility"
        - floor_id: "L2"
          name: "Level 2 - Cleanroom"

  layout:
    - area: "Utility_Area"
      building: "FAB1"
      floor: "L1"
      zone_type: "Utility"
      instances:
        - iadl_ref: "hvac.iadl#Chiller-01"
          instance_id: "CH-01"
          count: 2
          position: { x: 10, y: 20, z: 0 }
        - iadl_ref: "hvac.iadl#Chiller_Pump-01"
          instance_id: "CHWP-01"
          count: 2
          position: { x: 12, y: 22, z: 0 }
        - iadl_ref: "upw.iadl#RO_Unit-01"
          instance_id: "RO-01"
          count: 1
          position: { x: 30, y: 20, z: 0 }

    - area: "Cleanroom_ZoneA"
      building: "FAB1"
      floor: "L2"
      zone_type: "ISO_5"
      instances:
        - iadl_ref: "hvac.iadl#FFU-ZoneA-001"
          instance_id: "FFU-A"
          count: 500
          distribution: "grid"
          grid_params: { rows: 20, cols: 25, spacing: 2.4 }

  relationships:
    - from: "CHWP-01"
      to: "CH-01"
      type: "feeds"
      description: "Chilled water pump feeds chiller"
```

### 4.3 FDL 設計說明

- **metadata**: 定義了廠房的基本資訊,如名稱、版本和作者。
- **buildings**: 定義了廠房的建築結構,包括建築物和樓層。
- **layout**: 定義了廠房的區域劃分和資產實例化。
    - `iadl_ref`: 引用 IADL 檔案中定義的資產藍圖。
    - `instance_id`: 為資產實例分配唯一的 ID。
    - `count`: 指定該資產實例的數量。
    - `position` / `distribution`: 定義資產的空間位置或分佈方式。
- **relationships**: 定義資產實例之間的連接關係。

---



## 5. 結論

本文件成功地運用 IADL 和 FDL 為一座先進的半導體廠房設計了一套完整的 FMCS 系統。這個基於 IDTF 框架的設計方案帶來了以下核心價值:

- **設計標準化**: 透過 IADL,所有廠務設備都有了統一的數位化描述,涵蓋技術規格、數據標籤和行為邏輯。這為後續的系統整合、維護和升級奠定了堅實的基礎。

- **佈局模型化**: 透過 FDL,廠房的實體佈局和資產配置被精確地模型化。這使得工廠設計可以被版本控制、重複使用和自動化部署,大大提高了設計效率和準確性。

- **數位分身就緒 (Digital Twin Ready)**: 這套 IADL/FDL 設計文件是建立工廠數位分身的基石。NDH 可以直接解析這些文件,自動創建 Asset Servant 虛擬分身物件,並將其與 Omniverse 中的 3D 模型進行連結,實現設計、建造和營運的全生命週期數位化管理。

- **系統解耦與靈活性**: IADL 中的 `source` 定義將抽象的數據標籤 (如 `chw_supply_temp`) 與具體的通訊協議和地址 (如 `opc_ua`, `modbus`) 解耦。這意味著即使底層的 PLC 或感測器更換,只要更新 IADL 中的 `source` 即可,上層應用無需任何修改,大大提高了系統的靈活性和可維護性。

總而言之,這套基於 IADL 和 FDL 的 FMCS 設計方法論,不僅僅是技術文件的撰寫,更是實現工業 4.0 和數位化轉型的重要實踐。它將傳統的工廠設計提升到一個新的層次,為打造下一代智慧工廠提供了清晰、可行且高效的路徑。

