# Topic 命名規範

## 1. 概述

本文件定義了 IDTF (Industrial Digital Twin Framework) 中事件和命令的 Topic 命名規範。統一的 Topic 命名規範對於確保 NDH (Neutral Data Hub) 內部以及與 AI Agent 和外部系統之間數據流的清晰性、可管理性和互操作性至關重要。它遵循 MQTT Topic 的最佳實踐，並融入了 IDTF 的資產階層結構。

## 2. 核心原則

1.  **階層性 (Hierarchy)**: Topic 應反映 NDH 的資產階層結構，從宏觀到微觀逐步細化。
2.  **清晰性 (Clarity)**: Topic 名稱應清晰、簡潔，易於理解其所代表的數據或命令。
3.  **一致性 (Consistency)**: 所有 Topic 應遵循相同的命名模式，避免混亂。
4.  **可擴展性 (Extensibility)**: 命名規範應允許未來添加新的資產類型、事件類型或命令。
5.  **語義化 (Semantic)**: Topic 應包含足夠的語義信息，以便消費者能夠判斷其相關性。

## 3. NDH Topic 命名規範

NDH 的 Topic 命名規範主要分為以下幾類：

### 3.1. 遙測數據 (Telemetry Data)

用於發佈來自 Asset Instance 的實時感測器數據、狀態或其他遙測信息。

**格式**: `ndh/site/{site_id}/area/{area_id}/asset/{asset_id}/telemetry/{tag_id}`

*   `ndh`: 根命名空間，表示這是 NDH 系統的 Topic。
*   `site_id`: FDL 中定義的 Site ID (例如 `factory_a`, `main_building`)。
*   `area_id`: FDL 中定義的 Area ID (例如 `production_zone_b`, `floor_2f`)。
*   `asset_id`: Asset Instance 的唯一識別符 (例如 `pump_001`, `cncmachine_002`)。
*   `telemetry`: 表示這是一個遙測數據 Topic。
*   `tag_id`: Asset Tag Instance 的唯一識別符 (例如 `temperature`, `pressure`, `rpm`, `oee`)。

**示例**:

*   `ndh/site/factory_a/area/production_zone_b/asset/pump_001/telemetry/temperature`
*   `ndh/site/factory_a/area/assembly_line_1/asset/robot_arm_003/telemetry/joint_angle_1`

### 3.2. 事件 (Events)

用於發佈系統級事件、資產生命週期事件、警報或通知。

**格式**: `ndh/events/{domain}/{event_type}`

*   `domain`: 事件所屬的領域 (例如 `asset`, `system`, `security`, `mes`)。
*   `event_type`: 事件的具體類型 (例如 `status_changed`, `alarm_triggered`, `maintenance_scheduled`)。

**示例**:

*   `ndh/events/asset/status_changed` (事件數據中包含哪個 Asset Instance 的狀態變化)
*   `ndh/events/system/gateway_offline`
*   `ndh/events/security/access_violation`

### 3.3. 命令 (Commands)

用於向 Asset Instance 或 Agent 發送控制命令。

**格式**: `ndh/cmd/{asset_id}/{verb}` (針對 Asset Instance)
**格式**: `ndh/cmd/agent/{agent_id}/{verb}` (針對 Agent)

*   `asset_id`: Asset Instance 的唯一識別符。
*   `agent_id`: Agent 的唯一識別符。
*   `verb`: 命令的動詞 (例如 `start`, `stop`, `set_value`, `reset`)。

**示例**:

*   `ndh/cmd/pump_001/start`
*   `ndh/cmd/cncmachine_002/set_speed` (命令數據中包含速度值)
*   `ndh/cmd/agent/energy_optimizer/recalibrate`

### 3.4. Agent 內部通訊 (Agent Internal Communication)

用於 Agent 之間或 Agent 與 NDH 控制面之間的特定通訊。

**格式**: `agents/{agent_name}/inbox`
**格式**: `agents/{agent_name}/outbox`

*   `agent_name`: Agent 的唯一名稱。
*   `inbox`: Agent 接收命令或消息的 Topic。
*   `outbox`: Agent 發佈自身狀態或處理結果的 Topic。

**示例**:

*   `agents/energy_optimizer/inbox`
*   `agents/maintenance_advisor/outbox`

## 4. 通配符使用

NDH 支援 MQTT 的通配符訂閱，允許 Agent 或應用程式訂閱多個相關 Topic。

*   `+`: 單層通配符。例如 `ndh/site/+/area/production_zone_b/asset/pump_001/telemetry/temperature` 可以訂閱所有站點中 `production_zone_b` 區域 `pump_001` 的溫度數據。
*   `#`: 多層通配符。例如 `ndh/site/factory_a/area/#` 可以訂閱 `factory_a` 中所有區域的所有遙測數據和事件。

## 5. 最佳實踐

*   **避免使用空格和特殊字符**: Topic 應只包含小寫字母、數字、連字符 `-` 和下劃線 `_`。
*   **保持簡潔**: 雖然階層性很重要，但 Topic 不應過長或過於複雜。
*   **避免以 `/` 開頭或結尾**: 這會導致歧義。
*   **使用有意義的 ID**: `site_id`, `area_id`, `asset_id`, `tag_id` 應使用在 FDL 和 IADL 中定義的實際 ID。

遵循這些命名規範將有助於構建一個清晰、可管理且高效的 IDTF 生態系統。


## 6. 非功能需求 (Non-Functional Requirements, NFR) 與決策門檻

本規範所定義的 Topic 命名規範需滿足以下非功能需求：

*   **延遲 (Latency)**：同廠查詢/事件 p95 < 300ms；跨廠/雲邊 p95 < 800ms。
*   **吞吐 (Throughput)**：≥ 100k tags / 1k EPS（每廠），可線性擴張。
*   **可用性 (Availability)**：NDH 控制面 99.9%，告警通道 99.99%。
*   **一致性 (Consistency)**：事件重放恢復狀態一致（需定義 3 條測例）。
*   **安全 (Security)**：高風險命令「人機共治 + 限幅/限頻 + 全鏈路審計」。
*   **可維運 (Maintainability)**：SLO 儀表（Latency/Traffic/Errors/Saturation）；事故 Runbook（角色/時限/動作）。

