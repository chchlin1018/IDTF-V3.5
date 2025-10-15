# IADL/FDL JSON Schema 與 Conformance Test 規範

**版本**: 1.0
**日期**: 2025-10-15
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)

---

## 1. 概述

本文件旨在定義 IDTF (Industrial Digital Twin Framework) 中 IADL (Industrial Asset Definition Language) 和 FDL (Factory Design Language) 的 JSON Schema 規範，並闡述如何利用這些 Schema 進行語言規範、範例驗證以及 Conformance Test。透過標準化的 JSON Schema，IDTF 旨在提供一個機器可讀、可驗證的語言定義，確保資產模型的語法正確性、一致性及互操作性，並為自動化工具和生態系統的發展奠定基礎。

## 2. IADL JSON Schema 規範

IADL 用於定義工業資產的類型 (AssetType) 及其相關屬性、行為、接口和關係。其 JSON Schema 應反映這些核心概念。

### 2.1 核心結構

IADL 的頂層結構應包含 `AssetType` 的定義，每個 `AssetType` 應具有唯一的識別符、名稱、描述，並可包含多個 `ComponentType` 和 `Interface`。

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Industrial Asset Definition Language (IADL) Schema",
  "description": "Schema for defining industrial asset types and their characteristics.",
  "type": "object",
  "properties": {
    "iadlVersion": {
      "type": "string",
      "description": "Version of the IADL specification."
    },
    "assetTypes": {
      "type": "array",
      "description": "List of industrial asset type definitions.",
      "items": {
        "$ref": "#/definitions/AssetType"
      }
    }
  },
  "required": ["iadlVersion", "assetTypes"],
  "definitions": {
    "AssetType": {
      "type": "object",
      "properties": {
        "id": {
          "type": "string",
          "description": "Unique identifier for the asset type (e.g., 'Pump_Centrifugal_V1').",
          "pattern": "^[A-Za-z0-9_\\-]+$"
        },
        "name": {
          "type": "string",
          "description": "Human-readable name of the asset type."
        },
        "description": {
          "type": "string",
          "description": "Detailed description of the asset type."
        },
        "baseType": {
          "type": "string",
          "description": "Optional: ID of the base asset type this type inherits from."
        },
        "properties": {
          "type": "array",
          "description": "List of properties (attributes) for this asset type.",
          "items": {
            "$ref": "#/definitions/Property"
          }
        },
        "components": {
          "type": "array",
          "description": "List of component types that make up this asset type.",
          "items": {
            "$ref": "#/definitions/ComponentReference"
          }
        },
        "interfaces": {
          "type": "array",
          "description": "List of interfaces this asset type exposes.",
          "items": {
            "$ref": "#/definitions/Interface"
          }
        },
        "telemetry": {
          "type": "array",
          "description": "List of telemetry points for this asset type.",
          "items": {
            "$ref": "#/definitions/TelemetryPoint"
          }
        },
        "commands": {
          "type": "array",
          "description": "List of commands that can be executed on this asset type.",
          "items": {
            "$ref": "#/definitions/Command"
          }
        }
      },
      "required": ["id", "name"],
      "additionalProperties": false
    },
    "Property": {
      "type": "object",
      "properties": {
        "name": {
          "type": "string",
          "description": "Name of the property."
        },
        "dataType": {
          "type": "string",
          "enum": ["string", "number", "boolean", "integer", "array", "object"],
          "description": "Data type of the property."
        },
        "unit": {
          "type": "string",
          "description": "Optional: Unit of measurement for the property (e.g., 'm/s', '°C')."
        },
        "description": {
          "type": "string",
          "description": "Description of the property."
        },
        "defaultValue": {
          "description": "Optional: Default value for the property."
        }
      },
      "required": ["name", "dataType"],
      "additionalProperties": false
    },
    "ComponentReference": {
      "type": "object",
      "properties": {
        "componentTypeId": {
          "type": "string",
          "description": "ID of the referenced component type."
        },
        "quantity": {
          "type": "integer",
          "minimum": 1,
          "description": "Number of instances of this component type within the asset."
        },
        "name": {
          "type": "string",
          "description": "Optional: Specific name for this component instance within the asset."
        }
      },
      "required": ["componentTypeId"],
      "additionalProperties": false
    },
    "Interface": {
      "type": "object",
      "properties": {
        "id": {
          "type": "string",
          "description": "Unique identifier for the interface."
        },
        "type": {
          "type": "string",
          "enum": ["OPC_UA", "MQTT", "REST", "Modbus"],
          "description": "Type of the interface (e.g., OPC_UA, MQTT, REST)."
        },
        "endpoint": {
          "type": "string",
          "description": "Connection endpoint for the interface."
        },
        "mapping": {
          "type": "object",
          "description": "Mapping rules for data points to/from the interface."
        }
      },
      "required": ["id", "type", "endpoint"],
      "additionalProperties": false
    },
    "TelemetryPoint": {
      "type": "object",
      "properties": {
        "name": {
          "type": "string",
          "description": "Name of the telemetry point."
        },
        "dataType": {
          "type": "string",
          "enum": ["string", "number", "boolean", "integer"],
          "description": "Data type of the telemetry value."
        },
        "unit": {
          "type": "string",
          "description": "Optional: Unit of measurement."
        },
        "description": {
          "type": "string",
          "description": "Description of the telemetry point."
        }
      },
      "required": ["name", "dataType"],
      "additionalProperties": false
    },
    "Command": {
      "type": "object",
      "properties": {
        "name": {
          "type": "string",
          "description": "Name of the command."
        },
        "parameters": {
          "type": "array",
          "description": "List of parameters for the command.",
          "items": {
            "$ref": "#/definitions/Property"
          }
        },
        "description": {
          "type": "string",
          "description": "Description of the command."
        }
      },
      "required": ["name"],
      "additionalProperties": false
    }
  }
}
```

### 2.2 IADL 範例

以下是一個簡單的 IADL 範例，定義了一個「CentrifugalPump」資產類型：

```json
{
  "iadlVersion": "1.0.0",
  "assetTypes": [
    {
      "id": "CentrifugalPump_V1",
      "name": "Centrifugal Pump",
      "description": "A standard centrifugal pump for fluid transfer.",
      "properties": [
        {
          "name": "MaxFlowRate",
          "dataType": "number",
          "unit": "m3/h",
          "description": "Maximum fluid flow rate."
        },
        {
          "name": "RatedPower",
          "dataType": "number",
          "unit": "kW",
          "description": "Rated electrical power."
        }
      ],
      "components": [
        {
          "componentTypeId": "Motor_AC_V2",
          "quantity": 1,
          "name": "MainMotor"
        }
      ],
      "telemetry": [
        {
          "name": "CurrentFlowRate",
          "dataType": "number",
          "unit": "m3/h",
          "description": "Current fluid flow rate."
        },
        {
          "name": "MotorTemperature",
          "dataType": "number",
          "unit": "°C",
          "description": "Motor winding temperature."
        }
      ],
      "commands": [
        {
          "name": "StartPump",
          "description": "Starts the pump."
        },
        {
          "name": "StopPump",
          "description": "Stops the pump."
        }
      ]
    }
  ]
}
```

## 3. FDL JSON Schema 規範

FDL 用於定義工廠的物理佈局、資產實例 (AssetInstance) 及其之間的階層關係。它將 IADL 定義的資產類型實例化到具體的工廠環境中。

### 3.1 核心結構

FDL 的頂層結構應包含 `Factory` 的定義，其中包含 `Site`、`Area`、`ProcessCell`、`Unit` 等階層元素，並引用 IADL 定義的 `AssetType` 來創建 `AssetInstance`。

```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Factory Design Language (FDL) Schema",
  "description": "Schema for defining factory layout and asset instances.",
  "type": "object",
  "properties": {
    "fdlVersion": {
      "type": "string",
      "description": "Version of the FDL specification."
    },
    "factory": {
      "$ref": "#/definitions/Factory"
    }
  },
  "required": ["fdlVersion", "factory"],
  "definitions": {
    "Factory": {
      "type": "object",
      "properties": {
        "id": {
          "type": "string",
          "description": "Unique identifier for the factory."
        },
        "name": {
          "type": "string",
          "description": "Human-readable name of the factory."
        },
        "description": {
          "type": "string",
          "description": "Detailed description of the factory."
        },
        "sites": {
          "type": "array",
          "description": "List of sites within the factory.",
          "items": {
            "$ref": "#/definitions/Site"
          }
        }
      },
      "required": ["id", "name"],
      "additionalProperties": false
    },
    "Site": {
      "type": "object",
      "properties": {
        "id": {
          "type": "string",
          "description": "Unique identifier for the site."
        },
        "name": {
          "type": "string",
          "description": "Human-readable name of the site."
        },
        "areas": {
          "type": "array",
          "description": "List of areas within the site.",
          "items": {
            "$ref": "#/definitions/Area"
          }
        }
      },
      "required": ["id", "name"],
      "additionalProperties": false
    },
    "Area": {
      "type": "object",
      "properties": {
        "id": {
          "type": "string",
          "description": "Unique identifier for the area."
        },
        "name": {
          "type": "string",
          "description": "Human-readable name of the area."
        },
        "processCells": {
          "type": "array",
          "description": "List of process cells within the area.",
          "items": {
            "$ref": "#/definitions/ProcessCell"
          }
        }
      },
      "required": ["id", "name"],
      "additionalProperties": false
    },
    "ProcessCell": {
      "type": "object",
      "properties": {
        "id": {
          "type": "string",
          "description": "Unique identifier for the process cell."
        },
        "name": {
          "type": "string",
          "description": "Human-readable name of the process cell."
        },
        "units": {
          "type": "array",
          "description": "List of units within the process cell.",
          "items": {
            "$ref": "#/definitions/Unit"
          }
        }
      },
      "required": ["id", "name"],
      "additionalProperties": false
    },
    "Unit": {
      "type": "object",
      "properties": {
        "id": {
          "type": "string",
          "description": "Unique identifier for the unit."
        },
        "name": {
          "type": "string",
          "description": "Human-readable name of the unit."
        },
        "assetInstances": {
          "type": "array",
          "description": "List of asset instances within the unit.",
          "items": {
            "$ref": "#/definitions/AssetInstance"
          }
        }
      },
      "required": ["id", "name"],
      "additionalProperties": false
    },
    "AssetInstance": {
      "type": "object",
      "properties": {
        "id": {
          "type": "string",
          "description": "Unique identifier for this specific asset instance."
        },
        "name": {
          "type": "string",
          "description": "Human-readable name for this asset instance."
        },
        "assetTypeId": {
          "type": "string",
          "description": "ID of the IADL AssetType this instance is based on."
        },
        "serialNumber": {
          "type": "string",
          "description": "Serial number of the physical asset."
        },
        "location": {
          "type": "object",
          "description": "Physical location details.",
          "properties": {
            "x": {"type": "number"},
            "y": {"type": "number"},
            "z": {"type": "number"},
            "room": {"type": "string"}
          }
        },
        "properties": {
          "type": "object",
          "description": "Instance-specific property values (overrides or additions to AssetType properties)."
        },
        "operationalStatus": {
          "type": "string",
          "enum": ["Running", "Stopped", "Idle", "Faulted", "Under_Maintenance", "Commissioning", "Decommissioned"],
          "description": "Current operational status of the asset instance."
        },
        "componentInstances": {
          "type": "array",
          "description": "List of component instances within this asset instance.",
          "items": {
            "$ref": "#/definitions/ComponentInstance"
          }
        }
      },
      "required": ["id", "name", "assetTypeId"],
      "additionalProperties": false
    },
    "ComponentInstance": {
      "type": "object",
      "properties": {
        "id": {
          "type": "string",
          "description": "Unique identifier for this specific component instance."
        },
        "name": {
          "type": "string",
          "description": "Human-readable name for this component instance."
        },
        "componentTypeId": {
          "type": "string",
          "description": "ID of the IADL ComponentType this instance is based on."
        },
        "serialNumber": {
          "type": "string",
          "description": "Serial number of the physical component."
        }
      },
      "required": ["id", "name", "componentTypeId"],
      "additionalProperties": false
    }
  }
}
```

### 3.2 FDL 範例

以下是一個簡單的 FDL 範例，實例化了一個工廠和其中的泵：

```json
{
  "fdlVersion": "1.0.0",
  "factory": {
    "id": "Factory_A",
    "name": "Main Production Plant A",
    "description": "Primary manufacturing facility.",
    "sites": [
      {
        "id": "Site_01",
        "name": "Site One",
        "areas": [
          {
            "id": "Area_Assembly",
            "name": "Assembly Line 1",
            "processCells": [
              {
                "id": "ProcessCell_PumpAssembly",
                "name": "Pump Assembly Process Cell",
                "units": [
                  {
                    "id": "Unit_Mixing",
                    "name": "Mixing Unit",
                    "assetInstances": [
                      {
                        "id": "Pump_001",
                        "name": "Mixing Pump 1",
                        "assetTypeId": "CentrifugalPump_V1",
                        "serialNumber": "SN-PUMP-001-XYZ",
                        "location": {
                          "x": 10.5,
                          "y": 20.1,
                          "z": 5.0,
                          "room": "Mixing Room A"
                        },
                        "operationalStatus": "Running"
                      }
                    ]
                  }
                ]
              }
            ]
          }
        ]
      }
    ]
  }
}
```

## 4. Conformance Test 規範

Conformance Test 旨在驗證 IADL 和 FDL 文件是否符合其定義的 JSON Schema 規範，並確保其語義一致性。

### 4.1 語法驗證 (Syntactic Conformance)

*   **目的**：驗證 IADL/FDL 文件是否符合其對應的 JSON Schema 語法規則。
*   **方法**：使用標準的 JSON Schema 驗證工具 (例如 `jsonschema` 函式庫) 對 IADL/FDL JSON 文件進行驗證。任何不符合 Schema 的文件都應被標記為無效。
*   **測試項目**：
    *   所有 `required` 字段是否存在。
    *   所有字段的 `type` 是否正確。
    *   `pattern` 規則是否遵守 (例如 `id` 的命名規範)。
    *   `enum` 限制是否遵守 (例如 `dataType` 或 `operationalStatus`)。
    *   `minimum`、`maximum` 等數值限制是否遵守。

### 4.2 語義驗證 (Semantic Conformance)

*   **目的**：驗證 IADL/FDL 文件中的引用關係和邏輯一致性，這些是 JSON Schema 無法直接驗證的。
*   **方法**：開發自定義的驗證邏輯或工具，檢查跨文件或跨實體的引用完整性。
*   **測試項目**：
    *   **IADL 內部引用**：
        *   `AssetType` 中的 `baseType` 引用是否指向一個存在的 `AssetType`。
        *   `ComponentReference` 中的 `componentTypeId` 是否指向一個存在的 `ComponentType`。
    *   **FDL 對 IADL 的引用**：
        *   `AssetInstance` 中的 `assetTypeId` 是否指向一個存在的 IADL `AssetType`。
        *   `ComponentInstance` 中的 `componentTypeId` 是否指向一個存在的 IADL `ComponentType`。
    *   **階層一致性**：FDL 中定義的資產階層是否符合邏輯，例如 `Unit` 只能包含 `AssetInstance`，不能包含 `Site`。
    *   **唯一性約束**：在同一層級下，`id` 字段是否唯一。

### 4.3 Conformance Test 流程

1.  **定義測試集**：創建一系列有效的 (Positive) 和無效的 (Negative) IADL/FDL 範例文件，涵蓋各種邊界條件和錯誤場景。
2.  **語法驗證**：對所有範例文件執行 JSON Schema 語法驗證。
3.  **語義驗證**：對通過語法驗證的有效範例文件執行自定義語義驗證。
4.  **結果報告**：生成詳細的測試報告，指出哪些文件通過或未通過驗證，並提供錯誤信息。

## 5. 版本控制與發佈

IADL/FDL 的 JSON Schema、範例文件和 Conformance Test 腳本應納入版本控制系統 (如 Git)，並遵循語義化版本控制 (Semantic Versioning)。

*   **儲存庫結構**：
    ```
    idtf-v3.5/
    ├── 01_IDTF_Spec/
    │   ├── docs/
    │   │   └── IADL_FDL_JSON_Schema_and_Conformance.md
    │   └── schemas/
    │       ├── iadl_schema.json
    │       └── fdl_schema.json
    │   └── examples/
    │       ├── iadl_pump_example.json
    │       └── fdl_factory_example.json
    │   └── conformance_tests/
    │       ├── iadl_conformance.py
    │       └── fdl_conformance.py
    ```
*   **發佈**：每次 Schema 或規範更新時，應發佈新的版本，並更新 CHANGELOG。可以考慮將 Schema 發佈到公共的 Schema 註冊中心或透過 OpenAPI 規範生成工具發佈。

## 6. 結論

透過建立嚴謹的 IADL/FDL JSON Schema 和 Conformance Test 規範，IDTF 將能夠提供一個堅實的基礎，確保數位分身數據模型的一致性、互操作性和可驗證性。這不僅有助於自動化工具的開發，也將促進 IDTF 生態系統的健康發展，為工業數位轉型提供更可靠的標準。
