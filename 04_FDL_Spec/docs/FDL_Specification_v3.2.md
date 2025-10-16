# FDL (Factory Design Language) 規範

**版本**: 3.2
**發布日期**: 2025-10-16

---

## 1. 版本變更 (v3.1 → v3.2)

### 1.1. Breaking Changes

*   **術語統一**: 根據最新定義，`Asset Servant` 已更名為 `Asset Tag Instance`，`Asset 的實體 Instance` 已更名為 `Asset Instance`。
*   **FDL 實例化增強**: FDL 在實例化 Asset Instance 時，會考慮 IADL V3.2 中定義的 `secondary_hierarchy` 和 `lifecycle_events`，並允許在 FDL 層級進行配置或覆寫。

### 1.2. 新功能

*   **Asset Instance 次要階層配置**: 允許在 FDL 中為 Asset Instance 指定其所屬的邏輯階層路徑。
*   **Asset Instance 生命週期事件配置**: 允許在 FDL 中為 Asset Instance 配置其生命週期事件的初始狀態或行為。

---

## 2. FDL 概述

FDL (Factory Design Language) 是 IDTF 生態系統中用於定義工廠佈局和配置的聲明式語言。FDL 填補了從資產建模 (IADL) 到工廠部署 (NDH) 之間的關鍵缺口，提供了一個標準化、可重用且易於維護的工廠設計描述方式。

### 2.1. FDL 在 IDTF 生態系統中的角色

```mermaid
graph TD
    A[IADL (Asset Blueprint)] --> B[FDL (Factory Layout)];
    B --> C[NDH (Deployment Execution)];
    C --> D[Omniverse (Visualization)];

    subgraph IDTF V3.2 生態系統
        A
        B
        C
        D
    end
```

### 2.2. FDL 與 IADL 的關係

| 維度 | IADL | FDL |
|------|------|-----|
| **關注點** | 資產是什麼 (What) | 工廠如何佈局 (How) |
| **粒度** | 單一資產類型 | 整座工廠 |
| **內容** | 3D 模型、數據標籤、行為 | 數量、位置、連接關係、Asset Instance 的次要階層與生命週期事件配置 |
| **重用性** | 跨專案重用 | 跨工廠複製 |
| **生命週期** | 設計階段 | 設計到部署 |

---

## 3. FDL 語法規範 (v3.2)

### 3.1. 基本結構

FDL 採用 YAML 格式，具有清晰的階層結構：

```yaml
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
      instances: <Asset Instance 列表>
      relationships: <關係列表>
  
  parameters:
    <全域參數定義>
```

### 3.2. Asset Instance 實例化增強

在 FDL 中實例化 Asset Instance 時，可以配置其次要階層路徑和生命週期事件行為。這些配置會覆寫或補充 IADL 中定義的預設值。

```yaml
layout:
    - area: "Production_Zone_A"
      instances:
        - type: "DieSorter_v1.0"  # 引用 IADL 資產類型
          count: 10
          naming_prefix: "DS_2F_A_"
          # ... 其他 FDL 佈局配置 ...
          
          # NEW in v3.2: FDL 層級可配置 Asset Instance 的次要階層路徑
          secondary_hierarchy_paths:
            - type: "ProcessArea"
              path: "/Fab/Lithography"
            - type: "MaintenanceGroup"
              path: "/GroupA/LithographyEquipment"

          # NEW in v3.2: FDL 層級可配置 Asset Instance 的生命週期事件行為
          lifecycle_event_configs:
            - event_name: "PowerOn"
              initial_state: "ReadyForProduction"
            - event_name: "StartProduction"
              initial_state: "Running"
              action: "NotifyMES"
            - event_name: "EndShift"
              initial_state: "Idle"
```

---

## 4. FDL 在 IDTF 生態系統中的角色

FDL 作為 IDTF 生態系統的核心組件，其主要職責是將 IADL 定義的資產藍圖，根據實際工廠的佈局和配置需求，實例化為具體的 Asset Instance。這些 Asset Instance 包含了資產的物理位置、數量、相互關係，以及其在多維度階層和生命週期事件中的特定行為。

FDL 的設計使得工廠的數位分身能夠精確地反映物理世界的狀態，為 NDH 的部署執行、Omniverse 的視覺化以及上層 AI Agent 的智能應用提供了堅實的基礎。

FDL 的這種聲明式、階層化的定義方式，也使得它能夠作為不同工廠 Layout 設計軟體之間的橋樑，實現標準化的佈局映射和數據互操作性。

