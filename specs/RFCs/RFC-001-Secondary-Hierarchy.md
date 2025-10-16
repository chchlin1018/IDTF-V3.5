# RFC-001: Secondary Hierarchy for Asset Instances

**狀態**: 已採納 (Accepted)
**發布日期**: 2025-10-16

## 1. 簡介

本 RFC 提出在 IDTF IADL 和 FDL 規範中引入「次要階層 (Secondary Hierarchy)」的概念，以增強 Asset Instance 的靈活分組和多維度視圖。傳統上，Asset Instance 主要根據物理佈局（由 FDL 定義）形成一個主階層。然而，在實際工業應用中，資產往往需要根據不同的業務邏輯（例如，按製程區域、維護班組、成本中心等）進行分組和管理。此功能旨在解決這一需求。

## 2. 問題陳述

現有 IDTF 架構主要依賴由 FDL 定義的物理階層來組織 Asset Instance。這在物理佈局視圖下是有效的，但當需要從其他業務維度（如邏輯製程、維護責任、安全區域等）來查詢、分析或管理資產時，缺乏原生支援，導致需要額外的、非標準化的機制來實現，增加了複雜性和數據不一致的風險。

## 3. 提案

建議在 IADL 和 FDL 規範中引入 `secondary_hierarchy` 字段，允許為 Asset Instance 定義一個或多個邏輯階層路徑。

### 3.1. IADL 規範的修改

在 IADL `AssetType` 定義中，新增 `secondary_hierarchy` 區塊，用於聲明該資產類型可能支援的次要階層類型和其描述。這是一個元數據層次的定義，表明該資產可以被歸類到哪些邏輯維度。

```yaml
asset_type: <資產類型名稱>
...

# NEW: 次要階層定義
secondary_hierarchy:
  - name: <階層名稱> # 例如: ProcessArea, MaintenanceGroup
    description: <階層描述>
```

### 3.2. FDL 規範的修改

在 FDL 的 `layout.instances` 區塊中，為每個 Asset Instance 新增 `secondary_hierarchy_paths` 字段，用於指定該 Asset Instance 所屬的具體次要階層路徑。這允許在部署時靈活地將資產分配到不同的邏輯群組中。

```yaml
layout:
    - area: "Production_Zone_A"
      instances:
        - type: "DieSorter_v1.0"
          # ... 其他 FDL 佈局配置 ...
          
          # NEW: FDL 層級可配置 Asset Instance 的次要階層路徑
          secondary_hierarchy_paths:
            - type: "ProcessArea"
              path: "/Fab/Lithography"
            - type: "MaintenanceGroup"
              path: "/GroupA/LithographyEquipment"
```

## 4. 影響

*   **優點**：
    *   增強資產的查詢和分組靈活性，支援多維度視圖。
    *   簡化上層應用開發，無需自行維護資產的邏輯分組信息。
    *   提高數據一致性，將邏輯分組信息納入標準規範。
    *   更好地支援基於業務邏輯的權限管理和數據產品化。
*   **缺點**：
    *   增加了 IADL 和 FDL 規範的複雜性。
    *   NDH 實作需要更新以支援 Asset Instance 的多個階層索引。

## 5. 驗收標準

*   IADL 和 FDL 規範文件已更新，包含 `secondary_hierarchy` 字段的定義。
*   NDH 能夠根據 FDL 中定義的 `secondary_hierarchy_paths` 正確構建和管理 Asset Instance 的邏輯階層。
*   提供 API 能夠基於次要階層路徑查詢 Asset Instance。

## 6. 未來工作

*   定義次要階層路徑的命名規範和驗證規則。
*   探索如何將次要階層與 Logical Groups 規範進一步整合。

