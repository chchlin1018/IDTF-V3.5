# IADL (Industrial Asset Description Language) 規範

**版本**: 3.2
**發布日期**: 2025-10-16

---

## 1. 版本變更 (v3.1 → v3.2)

### 1.1. Breaking Changes

*   **術語統一**: 根據最新定義，`Asset Servant` 已更名為 `Asset Tag Instance`，`Asset 的實體 Instance` 已更名為 `Asset Instance`。
*   **新增 `secondary_hierarchy`**: 允許資產屬於主物理階層之外的多個邏輯階層。
*   **新增 `lifecycle_events`**: 標準化資產生命週期事件的定義和狀態轉移。

### 1.2. 新功能

*   引入了次要階層 (`secondary_hierarchy`) 的概念，以支援更靈活的資產分組和查詢。
*   引入了生命週期事件 (`lifecycle_events`) 的概念，以標準化資產狀態管理。

---

## 2. IADL 概述

IADL 是用於定義工業資產「藍圖」的標準語言。它描述了資產的靜態屬性、動態數據點、3D 模型、行為以及與其他系統的互動。

## 3. IADL 語法規範 (v3.2)

```yaml
asset_type: <資產類型名稱>
version: <版本號>
description: <資產描述>

properties:
  # 靜態屬性
  - name: <屬性名稱>
    type: <數據類型>
    value: <靜態值>

# NEW in v3.2: 次要階層定義
secondary_hierarchy:
  - name: <階層名稱> # 例如: ProcessArea, MaintenanceGroup
    description: <階層描述>

data_tags:
  # 動態數據點
  - tag_id: <標籤ID>
    description: <標籤描述>
    data_type: <數據類型>
    unit: <單位>
    source_mapping: <數據源映射>

# NEW in v3.2: 生命週期事件定義
lifecycle_events:
  - event_name: <事件名稱> # 例如: PowerOn, StartProduction, Maintenance
    description: <事件描述>
    transitions:
      - from: <起始狀態>
        to: <目標狀態>

3d_models:
  - format: <模型格式> # 例如: glb, usd
    path: <模型文件路徑>

behaviors:
  # 資產行為與方法
  - name: <行為名稱>
    type: <行為類型>
    parameters: <參數列表>
```

### 3.1. 次要階層 (`secondary_hierarchy`)

資產除了存在於 FDL 定義的物理階層（例如 `Building/Floor/Area`）中，還可以屬於一個或多個邏輯階層。這使得可以從不同維度對資產進行分組和查詢。

**範例**:
```yaml
secondary_hierarchy:
  - name: "ProcessArea"
    description: "按製程區域劃分"
  - name: "MaintenanceGroup"
    description: "按維護責任分組"
```
在 FDL 中，可以將 Asset Instance 指派到這些階層中的特定路徑。

### 3.2. 生命週期事件 (`lifecycle_events`)

此部分定義了資產在其生命週期中可能經歷的事件以及相應的狀態轉移。這為實現標準化的資產狀態管理和事件驅動架構提供了基礎。

**範例**:
```yaml
lifecycle_events:
  - event_name: "PowerOn"
    description: "資產上電"
    transitions:
      - from: "Off"
        to: "Ready"
  - event_name: "StartProduction"
    description: "開始生產"
    transitions:
      - from: "Ready"
        to: "Running"
  - event_name: "StopProduction"
    description: "停止生產"
    transitions:
      - from: "Running"
        to: "Idle"
```

