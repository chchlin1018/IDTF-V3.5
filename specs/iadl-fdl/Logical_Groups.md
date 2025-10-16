# Logical Groups 規範

## 1. 概述

本文件定義了 IDTF (Industrial Digital Twin Framework) 中 Logical Groups 的規格，旨在提供一種機制，允許使用者或 AI Agent 根據業務需求，動態地組織和查詢 Asset Instance。Logical Groups 實現了多維度資產視圖，支持複雜的查詢語意和精細的權限控制，以應對工業場景中不斷變化的需求。

## 2. 資料模型 (Data Model)

Logical Groups 的資料模型設計為靈活且可擴展，能夠表示靜態集合、動態過濾結果以及巢狀群組。

```json
{
  "id": "urn:ndh:logicalgroup:production_line_a_critical_assets",
  "name": "生產線 A 關鍵資產",
  "description": "生產線 A 中所有被標記為關鍵的資產，用於優先監控和維護",
  "type": "dynamic" | "static",
  "definition": {
    "asset_paths": [
      "/ndh/site/factory_a/area/production_line_a/asset/pump_001",
      "/ndh/site/factory_a/area/production_line_a/asset/robot_arm_002"
    ], # 靜態群組直接列出資產路徑
    "query": {
      "include": [
        "/ndh/site/factory_a/area/production_line_a/#"
      ], # 包含所有生產線 A 下的資產
      "exclude": [
        "/ndh/site/factory_a/area/production_line_a/asset/sensor_temp_*
      ], # 排除溫度感測器
      "filter": "assetType = 'Pump_v1.0' AND properties.criticality = 'high' AND tags.operational_status = 'running'",
      "nested_groups": [
        "urn:ndh:logicalgroup:energy_intensive_assets"
      ] # 引用其他 Logical Group
    }
  },
  "metadata": {
    "owner": "maintenance_team",
    "last_updated_by": "system",
    "last_updated_at": "2025-10-26T16:00:00Z"
  }
}
```

**字段說明:**

*   **`id` (string, URI)**: Logical Group 的唯一識別符。
*   **`name` (string)**: 群組的顯示名稱。
*   **`description` (string)**: 群組的詳細描述。
*   **`type` (string)**: 群組類型，可以是 `static` (靜態定義) 或 `dynamic` (動態查詢)。
*   **`definition` (object)**: 群組的具體定義。
    *   **`asset_paths` (array of strings, 僅限 `static` 類型)**: 靜態群組直接包含的 Asset Instance 唯一路徑列表。
    *   **`query` (object, 僅限 `dynamic` 類型)**: 動態群組的查詢定義。
        *   `include` (array of strings): 使用 FDL 路徑通配符 (例如 `#`, `*`) 指定要包含的資產範圍。
        *   `exclude` (array of strings, 可選): 使用 FDL 路徑通配符指定要排除的資產範圍。
        *   `filter` (string, 可選): 基於資產屬性 (properties) 或 Asset Tag Instance 數據 (tags) 的過濾表達式。支援邏輯運算符 (AND, OR, NOT) 和比較運算符 (=, !=, >, <, >=, <=, LIKE)。
        *   `nested_groups` (array of URIs, 可選): 引用其他 Logical Groups，實現巢狀和複合群組。
*   **`metadata` (object)**: 群組的元數據，用於審計和管理。
    *   `owner` (string): 群組的負責人或團隊。
    *   `last_updated_by` (string): 最後更新者。
    *   `last_updated_at` (string, ISO 8601): 最後更新時間。

## 3. 查詢語意 (Query Semantics)

NDH 提供強大的查詢引擎來處理 Logical Groups，支援多種查詢模式。

1.  **路徑查詢 (Path Query)**:
    *   直接透過 FDL 路徑進行查詢，支援通配符。例如 `ndh/site/factory_a/area/#`。
2.  **交集/聯集 (Intersection/Union)**:
    *   支援多個 Logical Groups 之間的交集 (AND) 和聯集 (OR) 操作，以創建更複雜的視圖。
    *   例如：`GET /logicalgroups/intersection?groups=group_a,group_b`。
3.  **基於屬性過濾 (Attribute-based Filtering)**:
    *   允許根據 Asset Instance 的靜態屬性 (`properties`) 或實時 Asset Tag Instance 數據 (`tags`) 進行過濾。
    *   例如：`filter=assetType = 'Motor_v2.0' AND properties.manufacturer = 'Siemens' AND tags.vibration > 5.0`。
4.  **動態視圖 (Dynamic Views)**:
    *   動態群組的查詢會在每次請求時實時評估，確保結果始終反映最新的資產狀態和屬性。

## 4. 權限模型 (Permission Model)

Logical Groups 整合了精細的權限模型，支援 RBAC (Role-Based Access Control) 和 ABAC (Attribute-Based Access Control)，確保數據的安全性和合規性。

1.  **群組級權限 (Group-level RBAC)**:
    *   可以為特定的角色 (Role) 授予對一個或多個 Logical Groups 的讀取、修改或管理權限。
    *   例如：`maintenance_team` 角色對 `production_line_a_critical_assets` 群組擁有讀取和修改權限。

2.  **資產級權限 (Asset-level ABAC)**:
    *   權限可以細化到單個 Asset Instance。例如，只有 `production_manager` 角色才能對 `pump_001` 發送命令。
    *   ABAC 允許根據資產的屬性 (例如 `properties.criticality = 'high'`) 動態授予權限。

3.  **欄位級權限 (Field-level RBAC/ABAC)**:
    *   最細粒度的權限控制，可以限制使用者或 Agent 對 Asset Instance 特定屬性或 Asset Tag Instance 數據的讀寫權限。
    *   例如：`operator` 角色只能讀取 `temperature` Tag，但不能修改 `setpoint` Tag。

4.  **審計欄位 (Audit Fields)**:
    *   所有對 Logical Groups 和 Asset Instance 的操作都將被記錄，包括操作者 (who)、操作時間 (when) 和操作原因 (why)。這些審計欄位將與事件日誌結合，提供完整的可追溯性。

## 5. 驗收標準 (Acceptance Criteria)

提供 10 種常見查詢樣式的語義與預期輸出示例，以驗證 Logical Groups 查詢語意的正確性。

**示例查詢樣式：**

1.  查詢所有類型為 `Pump_v1.0` 且位於 `factory_a` 的資產。
2.  查詢 `production_line_a` 中所有 `operational_status` 為 `running` 的關鍵資產。
3.  查詢同時屬於 `energy_intensive_assets` 和 `high_vibration_alerts` 的資產。
4.  查詢 `factory_a` 中所有除了 `sensor_temp_*` 以外的資產。
5.  查詢 `factory_a` 中所有具有 `manufacturer = 'Siemens'` 屬性的資產。
6.  查詢 `production_line_b` 中所有 Asset Tag Instance `pressure` 值大於 `10.0` 的資產。
7.  查詢某個特定 Asset Instance 的所有子資產。
8.  查詢某個特定 Asset Instance 的所有父資產。
9.  查詢所有由 `maintenance_team` 擁有的 Logical Groups。
10. 查詢所有包含 `robot_arm_002` 的 Logical Groups。

本規範為 NDH 中 Logical Groups 的設計和實作提供了基礎，旨在支持靈活、安全且高效的資產管理和數據查詢。
