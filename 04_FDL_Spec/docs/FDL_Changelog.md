# FDL 變更日誌

## v3.2 (2025-10-16)

### Breaking Changes

*   **術語統一**: `Asset Servant` 更名為 `Asset Tag Instance`，`Asset 的實體 Instance` 更名為 `Asset Instance`。

### 新功能與改進

*   **FDL 實例化增強**: FDL 在實例化 Asset Instance 時，會考慮 IADL V3.2 中定義的 `secondary_hierarchy` 和 `lifecycle_events`，並允許在 FDL 層級進行配置或覆寫。
*   **Asset Instance 次要階層配置**: 允許在 FDL 中為 Asset Instance 指定其所屬的邏輯階層路徑。
*   **Asset Instance 生命週期事件配置**: 允許在 FDL 中為 Asset Instance 配置其生命週期事件的初始狀態或行為。

## v3.1 (Previous Version)

*   初始版本發布。

