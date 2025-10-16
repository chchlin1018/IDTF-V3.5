# Agent Manifest 規範

## 1. 概述

本文件定義了 IDTF (Industrial Digital Twin Framework) 中 AI Agent 的 Manifest 規範。Agent Manifest 是一個 YAML 格式的描述文件，用於聲明 Agent 的能力、配置、數據訂閱/發佈模式、風險策略以及所需的審批流程。這使得 NDH (Neutral Data Hub) 能夠理解、部署和管理各種 AI Agent，並確保其在工業環境中安全、合規地運行。

## 2. Manifest 結構

```yaml
apiVersion: ndh.agent/v1
kind: AgentManifest
metadata:
  name: energy-optimizer-agent
  description: "用於優化工廠能源消耗的 AI Agent"
  version: "1.0.0"
  author: "NDH Solutions Team"
  creationTimestamp: "2025-10-26T15:00:00Z"
spec:
  capabilities:
    - type: "data-subscription"
      description: "訂閱實時能源數據"
    - type: "command-issuance"
      description: "發送設備控制命令"
    - type: "prediction"
      description: "預測能源消耗趨勢"
  
  subscriptions:
    - topic: "ndh/site/factory_a/area/main_plant/asset/power_meter_001/telemetry/active_power"
      qos: 1
      dataSchema: "https://ndh.example.com/schemas/telemetry_power_v1.json"
    - topic: "ndh/site/factory_a/area/main_plant/asset/hvac_unit_001/telemetry/temperature_setpoint"
      qos: 1
      dataSchema: "https://ndh.example.com/schemas/telemetry_temperature_v1.json"

  publications:
    - topic: "ndh/site/factory_a/area/main_plant/asset/hvac_unit_001/cmd/set_temperature"
      qos: 1
      dataSchema: "https://ndh.example.com/schemas/command_set_temperature_v1.json"
      riskLevel: "high" # 命令發佈的風險等級

  riskStrategy:
    rateLimit:
      maxCommandsPerMinute: 5
      burst: 2
    frequencyLimit:
      minIntervalSeconds: 10 # 同一命令的最小發佈間隔
    commandBlacklist:
      - "ndh/site/factory_a/area/main_plant/asset/emergency_stop_001/cmd/activate"

  approvalFlow:
    type: "human-in-the-loop" # 自動 (auto) / 人機共治 (human-in-the-loop)
    approversGroup: "energy-managers"
    timeoutSeconds: 300

  resourceRequirements:
    cpu: "500m"
    memory: "512Mi"
    gpu: "0"

  runtimeConfig:
    environmentVariables:
      - name: "OPTIMIZATION_MODEL_VERSION"
        value: "v2.1"
```

## 3. 字段說明

### `apiVersion`, `kind`, `metadata`

*   **`apiVersion` (string)**: Manifest 規範的版本，例如 `ndh.agent/v1`。
*   **`kind` (string)**: 資源類型，固定為 `AgentManifest`。
*   **`metadata` (object)**: Agent 的元數據。
    *   `name` (string): Agent 的唯一名稱。
    *   `description` (string): Agent 的簡要描述。
    *   `version` (string): Agent 的版本號。
    *   `author` (string): Agent 的開發者或團隊。
    *   `creationTimestamp` (string, ISO 8601): Manifest 的創建時間。

### `spec`

*   **`capabilities` (array of objects)**: Agent 具備的能力列表。
    *   `type` (string): 能力類型，例如 `data-subscription`, `command-issuance`, `prediction`, `anomaly-detection` 等。
    *   `description` (string): 能力的詳細描述。

*   **`subscriptions` (array of objects)**: Agent 訂閱的數據 Topic 列表。
    *   `topic` (string): 遵循 NDH Topic 命名規範的 MQTT Topic。
    *   `qos` (integer): MQTT Quality of Service (0, 1, 2)。
    *   `dataSchema` (string, URI): 訂閱數據的 JSON Schema 定義的 URI，用於驗證接收到的數據。

*   **`publications` (array of objects)**: Agent 發佈的數據或命令 Topic 列表。
    *   `topic` (string): 遵循 NDH Topic 命名規範的 MQTT Topic。
    *   `qos` (integer): MQTT Quality of Service。
    *   `dataSchema` (string, URI): 發佈數據的 JSON Schema 定義的 URI，用於驗證發佈的數據。
    *   `riskLevel` (string, 可選): 發佈操作的風險等級 (`low`, `medium`, `high`, `critical`)。用於觸發不同的審批流程和安全策略。

*   **`riskStrategy` (object)**: Agent 執行操作時的安全和風險控制策略。
    *   `rateLimit` (object): 命令發佈的速率限制。
        *   `maxCommandsPerMinute` (integer): 每分鐘允許發送的最大命令數。
        *   `burst` (integer): 允許的瞬時命令峰值。
    *   `frequencyLimit` (object): 同一命令的最小發佈間隔。
        *   `minIntervalSeconds` (integer): 最小間隔秒數。
    *   `commandBlacklist` (array of strings): 禁止 Agent 發送的命令 Topic 列表。

*   **`approvalFlow` (object)**: Agent 執行高風險操作時所需的審批流程。
    *   `type` (string): 審批類型，`auto` (自動審批) 或 `human-in-the-loop` (人機共治，需要人工審批)。
    *   `approversGroup` (string, 可選): 需要審批的人員組或角色。
    *   `timeoutSeconds` (integer, 可選): 人工審批的超時時間。

*   **`resourceRequirements` (object)**: Agent 運行所需的計算資源。
    *   `cpu` (string): CPU 請求，例如 `500m` (0.5 CPU 核心)。
    *   `memory` (string): 記憶體請求，例如 `512Mi` (512 MiB)。
    *   `gpu` (integer): GPU 請求數量。

*   **`runtimeConfig` (object)**: Agent 運行時的配置信息。
    *   `environmentVariables` (array of objects): 運行時的環境變數列表。
        *   `name` (string): 環境變數名稱。
        *   `value` (string): 環境變數值。

## 4. 驗收標準 (Acceptance Criteria)

*   **Energy-Optimizer Agent**: 能夠訂閱電力數據，發佈優化指令，並具有中等風險等級，需要人機共治審批。
*   **Maintenance-Advisor Agent**: 能夠訂閱設備狀態和故障代碼，發佈維護建議，風險等級較低，自動審批。
*   **Safety-Sentinel Agent**: 能夠訂閱安全相關數據，發佈緊急停機命令，風險等級高，需要人機共治審批且有嚴格的速率限制。

Agent Manifest 規範為 NDH 平台提供了標準化的 Agent 描述方式，確保了 Agent 的安全部署、可管理性和互操作性。


## 5. 非功能需求 (Non-Functional Requirements, NFR) 與決策門檻

本規範所定義的 Agent Manifest 需滿足以下非功能需求：

*   **延遲 (Latency)**：同廠查詢/事件 p95 < 300ms；跨廠/雲邊 p95 < 800ms。
*   **吞吐 (Throughput)**：≥ 100k tags / 1k EPS（每廠），可線性擴張。
*   **可用性 (Availability)**：NDH 控制面 99.9%，告警通道 99.99%。
*   **一致性 (Consistency)**：事件重放恢復狀態一致（需定義 3 條測例）。
*   **安全 (Security)**：高風險命令「人機共治 + 限幅/限頻 + 全鏈路審計」。
*   **可維運 (Maintainability)**：SLO 儀表（Latency/Traffic/Errors/Saturation）；事故 Runbook（角色/時限/動作）。

