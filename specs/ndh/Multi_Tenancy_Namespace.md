# Multi-Tenancy & Namespace 規範

## 1. 概述

本文件定義了 NDH (Neutral Data Hub) 的多租戶 (Multi-Tenancy) 和命名空間 (Namespace) 規範，旨在確保 NDH 能夠安全、高效地服務於多個獨立的組織或部門，同時提供數據隔離和資源管理的能力。在複雜的工業環境中，多租戶支持是實現數據共享、協同工作和安全隔離的關鍵。

## 2. 租戶階層 (Tenant Hierarchy)

NDH 支援多層次的租戶階層，以反映企業的組織結構，並實現權限的繼承和數據的邏輯隔離。

```
Enterprise Tenant (例如: Harvatek Group)
└── Business Unit Tenant (例如: LED Packaging Division)
    └── Site Tenant (例如: Factory A, Factory B)
        └── Department/Team Tenant (例如: Production Line 1 Team, Maintenance Team)
```

*   **Enterprise Tenant**: 最高層級，代表整個企業。擁有全局配置和管理權限。
*   **Business Unit Tenant**: 代表企業內部的業務單元。繼承 Enterprise Tenant 的部分配置，並可進行業務單元級的自定義。
*   **Site Tenant**: 代表一個物理工廠或站點。擁有站點內所有資產和數據的管理權限。
*   **Department/Team Tenant**: 最細粒度，代表工廠內部的部門或團隊。擁有對特定資產群組或數據產品的存取權限。

## 3. Topic/Schema 命名規範與隔離

命名空間是實現數據邏輯隔離的核心機制。所有 NDH 內部資源（包括 MQTT Topic、數據 Schema、數據產品等）都將納入租戶命名空間。

### 3.1. Topic 命名規範

基於現有的 Topic 命名規範，引入租戶 ID 作為頂層命名空間。

**格式**: `tenants/{tenant_id}/ndh/site/{site_id}/area/{area_id}/asset/{asset_id}/telemetry/{tag_id}`

*   `tenant_id`: 租戶的唯一識別符 (例如 `harvatek_group`, `led_packaging_div`)。

**示例**:

*   `tenants/harvatek_group/ndh/site/factory_a/area/production_zone_b/asset/pump_001/telemetry/temperature`

### 3.2. Schema 命名空間

所有數據 Schema (JSON Schema) 都將根據其所屬的租戶進行命名空間隔離，並透過 URI 進行引用。

**格式**: `https://schemas.ndh.example.com/tenants/{tenant_id}/schemas/{schema_name}_v{version}.json`

**示例**:

*   `https://schemas.ndh.example.com/tenants/harvatek_group/schemas/asset_telemetry_power_v1.json`

## 4. Broker ACL (Access Control List)

NDH 的 MQTT Broker 將實施嚴格的 ACL 策略，以強制執行租戶之間的數據隔離和權限控制。

*   **基於租戶 ID 的存取控制**: 每個租戶只能存取其所屬命名空間下的 Topic。例如，`harvatek_group` 租戶下的使用者或 Agent 只能訂閱 `tenants/harvatek_group/#` 的 Topic。
*   **角色與權限**: 結合 RBAC (Role-Based Access Control) 模型，為不同角色定義對特定 Topic 的讀取 (subscribe) 和寫入 (publish) 權限。
*   **跨租戶共享 (可選)**: 在嚴格審批和配置下，允許有限的跨租戶數據共享，例如上層 Enterprise Tenant 可以監控下層所有 Site Tenant 的匯總數據。

## 5. 資料庫隔離策略 (Database Isolation Strategy)

NDH 核心數據庫（例如用於儲存 Asset Instance Hierarchy、Data Product 目錄、審計日誌等）將採用以下一種或多種隔離策略：

1.  **Schema-per-Tenant**: 為每個租戶創建獨立的數據庫 Schema。這是最常見且隔離性最強的策略，每個租戶的數據在邏輯上完全分離。
    *   **優點**: 數據隔離性高，便於備份和恢復。
    *   **缺點**: 資源消耗較大，管理複雜度較高。
2.  **Row-Level Security (RLS)**: 在單一數據庫或 Schema 中，透過數據庫層級的策略強制執行行級安全。每個數據行都包含一個 `tenant_id` 字段，數據庫會根據當前使用者的 `tenant_id` 自動過濾可見數據。
    *   **優點**: 資源利用率高，管理相對簡單。
    *   **缺點**: 實作複雜度較高，性能可能受影響，隔離性略低於 Schema-per-Tenant。
3.  **Dedicated Database**: 對於對數據隔離和性能要求極高的租戶，可以提供獨立的物理數據庫實例。

## 6. 資源管理與配額 (Resource Management & Quotas)

NDH 將實施資源管理和配額機制，以防止單一租戶消耗過多資源，影響其他租戶的服務。

*   **數據儲存配額**: 限制每個租戶可以儲存的數據量。
*   **API 請求速率限制**: 限制每個租戶的 API 請求頻率。
*   **事件吞吐量配額**: 限制每個租戶可以發佈或訂閱的事件數量。
*   **計算資源配額**: 對於運行 Agent 或數據處理任務，分配 CPU、記憶體等資源配額。

透過這些多租戶和命名空間規範，NDH 旨在構建一個安全、可擴展且高性能的工業數位分身平台，滿足不同企業和部門的複雜需求。


## 7. 非功能需求 (Non-Functional Requirements, NFR) 與決策門檻

本規範所定義的多租戶與命名空間機制需滿足以下非功能需求：

*   **延遲 (Latency)**：同廠查詢/事件 p95 < 300ms；跨廠/雲邊 p95 < 800ms。
*   **吞吐 (Throughput)**：≥ 100k tags / 1k EPS（每廠），可線性擴張。
*   **可用性 (Availability)**：NDH 控制面 99.9%，告警通道 99.99%。
*   **一致性 (Consistency)**：事件重放恢復狀態一致（需定義 3 條測例）。
*   **安全 (Security)**：高風險命令「人機共治 + 限幅/限頻 + 全鏈路審計」。
*   **可維運 (Maintainability)**：SLO 儀表（Latency/Traffic/Errors/Saturation）；事故 Runbook（角色/時限/動作）。

