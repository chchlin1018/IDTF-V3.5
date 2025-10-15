# FDL (Factory Design Language) 設計規範

**文件版本**: 1.0  
**發布日期**: 2025-10-12  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)  
**狀態**: 正式發布

---

## 1. 簡介

### 1.1. 目的

本文件旨在提供 **FDL (Factory Design Language)** 的完整設計規範。FDL 是一種專為描述工廠級資產佈局和實例化配置而設計的宣告式語言。它作為 IADL (Industrial Asset Description Language) 的補充，專注於回答「**哪裡**需要**多少個**什麼樣的資產？」這個核心問題。

### 1.2. 設計原則

- **宣告式 (Declarative)**：專注於描述「什麼」，而非「如何做」。
- **可讀性 (Human-Readable)**：採用 YAML 格式，語法簡潔，易於人類閱讀和編寫。
- **機器友善 (Machine-Friendly)**：結構清晰，易於程式解析和處理。
- **關注點分離 (Separation of Concerns)**：嚴格區分 IADL (資產藍圖) 和 FDL (實例化計畫)。
- **可擴展性 (Extensible)**：支援自訂元數據和屬性，以適應不同的工業場景。

### 1.3. 與 IADL 的關係

| 特性 | IADL (資產藍圖) | FDL (工廠部署計畫) |
| :--- | :--- | :--- |
| **目的** | **定義 (Definition)** | **聲明 (Declaration) & 實例化** |
| **核心問題** | 「一個資產**是什麼**？」 | 「**哪裡**需要**多少個**什麼樣的資產？」 |
| **內容** | 屬性、行為、事件、資料介面 | 實例數量、唯一 ID、位置、初始參數 |
| **比喻** | 汽車的設計圖紙 | 停車場的車輛清單與停放位置圖 |

---

## 2. 語法規範

FDL 採用 **YAML** 作為其基礎語法。所有 FDL 檔案都應使用 `.fdl.yml` 或 `.fdl` 作為副檔名。

### 2.1. 頂層結構

一個 FDL 檔案必須包含以下頂層鍵：

| 鍵 | 類型 | 是否必須 | 描述 |
| :--- | :--- | :--- | :--- |
| `fdl_version` | String | 是 | FDL 規範的版本號，例如 `"1.0"`。 |
| `factory_id` | String | 是 | 工廠的唯一識別碼。 |
| `metadata` | Object | 否 | 關於此 FDL 檔案的元數據。 |
| `areas` | Array | 是 | 工廠區域的列表，定義了資產的物理或邏輯分組。 |
| `relationships` | Array | 否 | 定義資產實例之間的連接關係。 |

**語法結構圖**：

![FDL 語法結構圖](../images/fdl-syntax-structure.png)

### 2.2. `metadata` 區塊

`metadata` 區塊用於存放關於 FDL 檔案本身的資訊。

| 鍵 | 類型 | 描述 |
| :--- | :--- | :--- |
| `name` | String | 工廠的名稱。 |
| `description` | String | 工廠的詳細描述。 |
| `author` | String | FDL 檔案的作者。 |
| `created_at` | Datetime | 建立日期。 |
| `updated_at` | Datetime | 最後更新日期。 |

**範例**：
```yaml
metadata:
  name: "Semiconductor Fab 1"
  description: "Main semiconductor fabrication plant in Hsinchu."
  author: "Michael Lin"
  created_at: 2025-10-12T10:00:00Z
```

### 2.3. `areas` 區塊

`areas` 區塊是 FDL 的核心，它定義了工廠的物理或邏輯佈局。它是一個包含多個 `Area` 物件的陣列。

#### `Area` 物件結構

| 鍵 | 類型 | 是否必須 | 描述 |
| :--- | :--- | :--- | :--- |
| `area_id` | String | 是 | 區域的唯一識別碼，在工廠內必須唯一。 |
| `name` | String | 否 | 區域的易讀名稱。 |
| `parent_area` | String | 否 | 指向父區域的 `area_id`，用於建立階層結構。 |
| `metadata` | Object | 否 | 關於此區域的元數據 (例如，樓層、GPS 座標)。 |
| `instances` | Array | 是 | 在此區域內要實例化的資產列表。 |

### 2.4. `instances` 區塊

`instances` 區塊定義了要在此區域內創建的資產實例。它是一個包含多個 `Instance` 物件的陣列。

#### `Instance` 物件結構

| 鍵 | 類型 | 是否必須 | 描述 |
| :--- | :--- | :--- | :--- |
| `type` | String | 是 | 要實例化的資產類型，必須對應到一個已知的 IADL 資產藍圖。 |
| `count` | Integer | 否 (預設 1) | 要創建的實例數量。 |
| `naming_prefix` | String | 否 | 實例的命名規則。NDH 會自動生成 ID，例如 `Pump_001`, `Pump_002`。 |
| `initial_params` | Object | 否 | 覆寫 IADL 中定義的預設屬性值。 |
| `location` | Object | 否 | 資產的物理位置 (例如，x, y, z 座標)。 |

**範例**：
```yaml
instances:
  - type: StandardPump
    count: 5
    naming_prefix: "Pump_"
    initial_params:
      power: 15
      status: "Stopped"
    location:
      x: 10.5
      y: 20.2
      z: 0.0
```

### 2.5. `relationships` 區塊

`relationships` 區塊定義了資產實例之間的連接關係。

#### `Relationship` 物件結構

| 鍵 | 類型 | 是否必須 | 描述 |
| :--- | :--- | :--- | :--- |
| `from` | String | 是 | 連接的來源資產實例的完整 ID (格式：`area_id.instance_id`)。 |
| `to` | String | 是 | 連接的目標資產實例的完整 ID。 |
| `type` | String | 是 | 關係的類型，例如 `connectedTo`, `controls`, `feeds`。 |
| `properties` | Object | 否 | 關於此連接關係的屬性 (例如，管線直徑、電纜類型)。 |

**範例**：
```yaml
relationships:
  - from: "ProductionFloor1.Pump_001"
    to: "ProductionFloor1.CoolingTower_001"
    type: "connectedTo"
    properties:
      pipeline_diameter: "DN100"
```

---

## 3. 完整範例

```yaml
fdl_version: "1.0"
factory_id: "FAB01"

metadata:
  name: "Semiconductor Fab 1"
  author: "Michael Lin"
  updated_at: 2025-10-12T12:00:00Z

areas:
  - area_id: "CleanRoom_A"
    name: "Class 100 Clean Room A"
    instances:
      - type: "AirHandlingUnit"
        count: 2
        naming_prefix: "AHU_"
        initial_params:
          fan_speed: 1800 # RPM

      - type: "TemperatureSensor"
        count: 10
        naming_prefix: "TempSensor_"
        location:
          distribution: "grid"
          rows: 2
          cols: 5

  - area_id: "UtilityArea"
    name: "Utility and Power Area"
    instances:
      - type: "Chiller"
        count: 1
        naming_prefix: "Chiller_"
        initial_params:
          capacity: 500 # Tons

relationships:
  - from: "UtilityArea.Chiller_001"
    to: "CleanRoom_A.AHU_001"
    type: "feeds"
    properties:
      medium: "Chilled Water"

  - from: "UtilityArea.Chiller_001"
    to: "CleanRoom_A.AHU_002"
    type: "feeds"
    properties:
      medium: "Chilled Water"
```

---

## 4. 最佳實踐

1.  **保持 IADL 和 FDL 分離**：不要在 FDL 中定義資產的內在屬性，只定義實例化的參數。
2.  **使用有意義的 ID**：`factory_id` 和 `area_id` 應該簡潔且有意義。
3.  **善用階層結構**：透過 `parent_area` 建立清晰的工廠佈局階層。
4.  **模組化 FDL**：對於大型工廠，可以將 FDL 拆分成多個檔案，例如每個區域一個檔案，然後由主 FDL 檔案引用。
5.  **版本控制**：將 FDL 檔案納入 Git 等版本控制系統，追蹤工廠佈局的變更歷史。

---

## 5. 附錄：保留關鍵字

- `fdl_version`
- `factory_id`
- `metadata`
- `areas`
- `relationships`
- `area_id`
- `instances`
- `type`
- `count`
- `naming_prefix`
- `initial_params`
- `location`
- `from`
- `to`
- `properties`

