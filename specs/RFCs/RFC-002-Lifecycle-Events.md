# RFC-002: Lifecycle Events for Asset Instances

**狀態**: 已採納 (Accepted)
**發布日期**: 2025-10-16

## 1. 簡介

本 RFC 提出在 IDTF IADL 和 FDL 規範中引入「生命週期事件 (Lifecycle Events)」的概念，以標準化 Asset Instance 的狀態管理和事件驅動行為。在工業場景中，資產會經歷多種狀態轉換（例如，從「停止」到「運行」，從「正常」到「維護」），這些狀態轉換通常由特定的事件觸發。此功能旨在提供一個統一的機制來定義和管理這些事件及其對資產狀態的影響。

## 2. 問題陳述

現有 IDTF 架構缺乏對 Asset Instance 生命週期狀態的標準化定義和管理機制。資產狀態的變化通常由底層設備或上層應用以非結構化的方式報告，導致：

*   **狀態定義不一致**：不同資產或系統對同一狀態有不同命名或理解。
*   **事件處理複雜**：上層應用難以統一處理資產的狀態變化事件。
*   **可追溯性差**：難以追溯資產狀態變化的原因和歷史。
*   **自動化困難**：基於資產狀態的自動化邏輯難以實現和維護。

## 3. 提案

建議在 IADL 和 FDL 規範中引入 `lifecycle_events` 字段，允許為 Asset Instance 定義其生命週期事件、狀態轉移以及可選的觸發行為。

### 3.1. IADL 規範的修改

在 IADL `AssetType` 定義中，新增 `lifecycle_events` 區塊，用於聲明該資產類型可能經歷的生命週期事件、其描述以及可能的狀態轉移。這定義了資產類型層級的標準事件行為。

```yaml
asset_type: <資產類型名稱>
...

# NEW: 生命週期事件定義
lifecycle_events:
  - event_name: <事件名稱> # 例如: PowerOn, StartProduction, Maintenance
    description: <事件描述>
    transitions:
      - from: <起始狀態>
        to: <目標狀態>
      # 可以定義多個轉移，例如從多個狀態轉移到同一目標狀態
```

### 3.2. FDL 規範的修改

在 FDL 的 `layout.instances` 區塊中，為每個 Asset Instance 新增 `lifecycle_event_configs` 字段，用於配置該 Asset Instance 特定生命週期事件的初始狀態或觸發行為。這允許在部署時對資產的事件行為進行客製化。

```yaml
layout:
    - area: "Production_Zone_A"
      instances:
        - type: "DieSorter_v1.0"
          # ... 其他 FDL 佈局配置 ...
          
          # NEW: FDL 層級可配置 Asset Instance 的生命週期事件行為
          lifecycle_event_configs:
            - event_name: "PowerOn"
              initial_state: "ReadyForProduction" # 覆寫 IADL 中定義的預設初始狀態
            - event_name: "StartProduction"
              initial_state: "Running"
              action: "NotifyMES" # 定義事件觸發時的額外動作 (例如發送通知到 MES)
            - event_name: "EndShift"
              initial_state: "Idle"
```

## 4. 影響

*   **優點**：
    *   標準化資產狀態管理，提高數據一致性。
    *   簡化事件驅動架構的實作，上層應用可以統一處理資產事件。
    *   提高資產狀態變化的可追溯性和可審計性。
    *   為基於資產狀態的自動化和智能決策提供堅實基礎。
*   **缺點**：
    *   增加了 IADL 和 FDL 規範的複雜性。
    *   NDH 實作需要更新以支援生命週期事件的處理和狀態機管理。

## 5. 驗收標準

*   IADL 和 FDL 規範文件已更新，包含 `lifecycle_events` 字段的定義。
*   NDH 能夠根據 IADL 和 FDL 中定義的 `lifecycle_events` 正確管理 Asset Instance 的狀態和事件轉移。
*   提供 API 能夠觸發資產事件並查詢資產的當前狀態。

## 6. 未來工作

*   定義 `action` 字段的語法和可支援的動作類型（例如，發送通知、觸發工作流）。
*   探索如何將生命週期事件與事件一致性規範進一步整合。

