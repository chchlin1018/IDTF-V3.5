# MCP API 草案 (OpenAPI/GraphQL)

## 1. 概述

本文件為 IDTF (Industrial Digital Twin Framework) 中 MCP (Model Context Protocol) 的 API 草案，旨在明確 MCP 作為 **Agent 層** 與 NDH (Neutral Data Hub) 作為 **數據層** 的職責邊界，並定義 AI Agent 透過 MCP 與 NDH 之間互動的介面。MCP 負責 Agent 的註冊與發現、生命週期管理、高層級工作流編排、權限與審計 (ACL) 以及 Agent 間通訊協調。NDH 則專注於底層數據存儲與檢索、實時事件流處理、時序數據管理和 Connector 管理。此草案將涵蓋 OpenAPI (REST) 和 GraphQL 兩種可能的實現方式，以提供靈活且強大的數據存取和操作能力，並強調 MCP 透過 gRPC 或內部 REST API 與 NDH 進行通訊，避免直接訪問 NDH 的資料庫，並使用事件驅動架構。

## 2. MCP 與 NDH 的職責邊界

為確保架構的清晰性和可維護性，NDH 與 MCP 的職責邊界明確劃分如下：

| 層級 | 主要職責 |
|:---|:---|
| **MCP (Model Context Protocol)** | **Agent 層**：負責 AI Agent 的註冊與發現、生命週期管理、高層級工作流編排、權限與審計 (ACL)、以及 Agent 間的通訊協調。 |
| **NDH (Neutral Data Hub)** | **數據層**：負責底層數據存儲與檢索、MQTT/REST 通訊基礎設施、實時事件流處理、時序數據管理、以及 Connector 管理。 |

### 2.1. 整合方式

*   **清晰的介面定義**: MCP 透過 **gRPC** 或內部 **REST API** 與 NDH 進行通訊，避免直接存取 NDH 的資料庫。
*   **事件驅動架構**: MCP 與 NDH 之間的互動主要基於事件驅動架構 (Event Sourcing)，確保了兩者之間的鬆耦合和高可擴展性。

## 3. OpenAPI (REST) 草案 (MCP 與 NDH 介面)

### 3.1. NDH 數據查詢與操作

**基礎 URL**: `https://api.ndh.example.com/v1` (此為 NDH 暴露給 MCP 的內部介面)

**認證**: OAuth 2.0 (Client Credentials Grant)

**常用端點 (Endpoints)**:

*   **GET /assets**: 獲取 Asset Instance 列表。
    *   **參數**: `type` (過濾資產類型), `location` (基於 FDL 路徑過濾), `status` (過濾資產狀態), `page`, `limit`。
    *   **回應**: `Asset Instance` 物件陣列。

*   **GET /assets/{assetId}**: 獲取特定 Asset Instance 的詳細資訊。
    *   **回應**: 單一 `Asset Instance` 物件，包含其屬性、最新的 Asset Tag Instance 數據、USD 模型引用等。

*   **GET /assets/{assetId}/tags**: 獲取特定 Asset Instance 的 Asset Tag Instance 列表。
    *   **回應**: `Asset Tag Instance` 物件陣列。

*   **GET /assets/{assetId}/tags/{tagId}/data**: 獲取特定 Asset Tag Instance 的歷史數據。
    *   **參數**: `start_time`, `end_time`, `interval`, `aggregation`。
    *   **回應**: 時間序列數據點陣列。

*   **POST /assets/{assetId}/commands**: 向特定 Asset Instance 發送控制命令。
    *   **請求主體**: `command_type` (例如 `start`, `stop`, `set_value`), `parameters` (命令參數)。
    *   **回應**: 命令執行狀態。

*   **GET /events**: 獲取事件日誌。
    *   **參數**: `type`, `source`, `start_time`, `end_time`。
    *   **回應**: `Event Envelope` 物件陣列。

### 3.2. 錯誤碼 (Error Codes)

標準 HTTP 狀態碼結合自定義錯誤碼：

*   **400 Bad Request**: 請求參數無效 (例如 `NDH-001: InvalidParameter`)。
*   **401 Unauthorized**: 認證失敗 (例如 `NDH-002: InvalidCredentials`)。
*   **403 Forbidden**: 權限不足 (例如 `NDH-003: AccessDenied`)。
*   **404 Not Found**: 資源不存在 (例如 `NDH-004: AssetNotFound`)。
*   **429 Too Many Requests**: 請求頻率過高 (例如 `NDH-005: RateLimitExceeded`)。
*   **500 Internal Server Error**: 服務器內部錯誤 (例如 `NDH-006: InternalError`)。
*   **503 Service Unavailable**: 服務暫時不可用。

## 4. GraphQL SDL (Schema Definition Language) 草案 (MCP 與 NDH 介面)

GraphQL 提供了更靈活的查詢能力，允許 Agent 精確地獲取所需的數據，避免過度獲取 (over-fetching) 或不足獲取 (under-fetching)。

### 4.1. 類型定義 (Type Definitions)

### 3.1. NDH 數據查詢與操作

**基礎 URL**: `https://api.ndh.example.com/v1`

**認證**: OAuth 2.0 (Client Credentials Grant)

**常用端點 (Endpoints)**:

*   **GET /assets**: 獲取 Asset Instance 列表。
    *   **參數**: `type` (過濾資產類型), `location` (基於 FDL 路徑過濾), `status` (過濾資產狀態), `page`, `limit`。
    *   **回應**: `Asset Instance` 物件陣列。

*   **GET /assets/{assetId}**: 獲取特定 Asset Instance 的詳細資訊。
    *   **回應**: 單一 `Asset Instance` 物件，包含其屬性、最新的 Asset Tag Instance 數據、USD 模型引用等。

*   **GET /assets/{assetId}/tags**: 獲取特定 Asset Instance 的 Asset Tag Instance 列表。
    *   **回應**: `Asset Tag Instance` 物件陣列。

*   **GET /assets/{assetId}/tags/{tagId}/data**: 獲取特定 Asset Tag Instance 的歷史數據。
    *   **參數**: `start_time`, `end_time`, `interval`, `aggregation`。
    *   **回應**: 時間序列數據點陣列。

*   **POST /assets/{assetId}/commands**: 向特定 Asset Instance 發送控制命令。
    *   **請求主體**: `command_type` (例如 `start`, `stop`, `set_value`), `parameters` (命令參數)。
    *   **回應**: 命令執行狀態。

*   **GET /events**: 獲取事件日誌。
    *   **參數**: `type`, `source`, `start_time`, `end_time`。
    *   **回應**: `Event Envelope` 物件陣列。

### 3.2. 錯誤碼 (Error Codes)

標準 HTTP 狀態碼結合自定義錯誤碼：

*   **400 Bad Request**: 請求參數無效 (例如 `NDH-001: InvalidParameter`)。
*   **401 Unauthorized**: 認證失敗 (例如 `NDH-002: InvalidCredentials`)。
*   **403 Forbidden**: 權限不足 (例如 `NDH-003: AccessDenied`)。
*   **404 Not Found**: 資源不存在 (例如 `NDH-004: AssetNotFound`)。
*   **429 Too Many Requests**: 請求頻率過高 (例如 `NDH-005: RateLimitExceeded`)。
*   **500 Internal Server Error**: 服務器內部錯誤 (例如 `NDH-006: InternalError`)。
*   **503 Service Unavailable**: 服務暫時不可用。

## 3. GraphQL SDL (Schema Definition Language) 草案 (MCP 與 NDH 介面)

GraphQL 提供了更靈活的查詢能力，允許 Agent 精確地獲取所需的數據，避免過度獲取 (over-fetching) 或不足獲取 (under-fetching)。

### 3.3. 類型定義 (Type Definitions)

```graphql
# Asset Instance 的核心資訊
type AssetInstance {
  id: ID!
  assetId: String!
  assetType: String!
  name: String
  description: String
  location: String # FDL 路徑
  status: String
  properties: [Property!] # 靜態屬性
  tags: [AssetTagInstance!] # 關聯的 Asset Tag Instances
  usdModelRef: String # Omniverse USD 模型引用
  parent: AssetInstance
  children: [AssetInstance!]
}

# 資產屬性 (來自 IADL)
type Property {
  name: String!
  value: String
  unit: String
}

# Asset Tag Instance (原 Asset Servant)
type AssetTagInstance {
  id: ID!
  tagId: String!
  description: String
  dataType: String
  unit: String
  currentValue: String # 最新實時值
  timestamp: String # 最新實時值的時間戳
  sourceMapping: String # 後端數據源映射
}

# 事件信封
type EventEnvelope {
  id: ID!
  source: String!
  type: String!
  time: String!
  data: JSON # 實際事件數據
}

# 時間序列數據點
type TimeSeriesDataPoint {
  timestamp: String!
  value: String!
}
```

### 3.4. 查詢 (Queries)

```graphql
type Query {
  # 獲取單一 Asset Instance
  asset(id: ID!): AssetInstance
  assetByPath(location: String!): AssetInstance

  # 獲取 Asset Instance 列表
  assets(
    type: String
    location: String
    status: String
    first: Int = 10
    after: String
  ): [AssetInstance!]

  # 獲取特定 Asset Instance 的 Asset Tag Instance 數據
  assetTags(assetId: ID!): [AssetTagInstance!]
  assetTag(assetId: ID!, tagId: String!): AssetTagInstance

  # 獲取時間序列數據
  timeSeriesData(
    assetId: ID!
    tagId: String!
    startTime: String!
    endTime: String!
    interval: String
    aggregation: String
  ): [TimeSeriesDataPoint!]

  # 查詢事件日誌
  events(
    type: String
    source: String
    startTime: String
    endTime: String
    first: Int = 10
    after: String
  ): [EventEnvelope!]
}
```

### 3.5. 變更 (Mutations)

```graphql
type Mutation {
  # 向 Asset Instance 發送命令
  sendAssetCommand(
    assetId: ID!
    commandType: String!
    parameters: JSON
  ): CommandResult
}

type CommandResult {
  success: Boolean!
  message: String
  commandId: ID
}

scalar JSON # 自定義 JSON 類型
```

此草案為 MCP API 提供了初步的結構和功能定義，將作為未來詳細設計和實作的基礎。


## 4. 架構優化策略

為了確保 MCP 能夠高效、穩定地運作，並與 NDH 協同提供強大的數位分身服務，以下架構優化策略將被實施：

### 4.1. 分散式架構

MCP 服務將採用無狀態設計，並部署在 Kubernetes 叢集上，以實現高可用性和水平擴展：

*   **無狀態設計**：MCP 的服務設計為無狀態，方便水平擴展和故障恢復。
*   **Load Balancer 分流**：透過負載均衡器將請求均勻分發到 MCP 叢集中的多個實例。
*   **Agent 親和性路由**：在某些情況下，可以實現 Agent 親和性路由，確保特定 Agent 的請求始終由同一 MCP 實例處理，以優化性能或保持會話狀態。

### 4.2. 事件流優化

MCP 將利用 Apache Kafka 作為與 NDH 互動的事件骨幹，確保事件處理的高吞吐量、低延遲和高可靠性：

*   **Apache Kafka 作為事件骨幹**：所有來自 NDH 的實時數據和系統事件，以及 MCP 發送給 NDH 的控制指令，都透過 Kafka 進行傳輸和持久化。
*   **分區策略**：根據 Asset Instance ID 或地理位置對 Kafka Topic 進行分區，確保相關事件在同一分區內有序處理，提高並行處理能力。
*   **批次處理**：數據在寫入數據庫或進行其他處理之前，會進行批次積累，減少 I/O 操作次數，提高寫入效率。
*   **背壓機制**：實施流量控制和背壓機制，防止上游數據源產生過多數據導致下游系統過載，確保系統的穩定性。

### 4.3. 效能基準測試

為了驗證和保障 MCP 系統的性能，將進行嚴格的效能基準測試，並設定明確的目標指標。以下是一個模擬 Agent 負載測試的腳本範例和目標指標：

```python
# 效能測試腳本 (Locust)
import asyncio
from locust import User, task, between

class AgentLoadTest(User):
    wait_time = between(0.1, 1)
    
    @task
    def subscribe_events(self):
        # 模擬 Agent 訂閱事件
        self.client.get("/mcp/events/subscribe?filter=temperature>80")
    
    @task(3)
    def publish_action(self):
        # 模擬 Agent 發布動作
        self.client.post("/mcp/actions", json={
            "type": "adjust_setpoint",
            "target": "valve-123",
            "value": 75.5
        })
```

**目標指標**：

*   **P99 延遲**：同廠查詢/事件 < 100ms
*   **吞吐量**：> 10,000 operations/second
*   **CPU 使用率**：< 70%

## 5. 非功能需求 (Non-Functional Requirements, NFR) 與決策門檻

本 API 所定義的 MCP 介面需滿足以下非功能需求：

*   **延遲 (Latency)**：同廠查詢/事件 p95 < 300ms；跨廠/雲邊 p95 < 800ms。
*   **吞吐 (Throughput)**：≥ 100k tags / 1k EPS（每廠），可線性擴張。
*   **可用性 (Availability)**：NDH 控制面 99.9%，告警通道 99.99%。
*   **一致性 (Consistency)**：事件重放恢復狀態一致（需定義 3 條測例）。
*   **安全 (Security)**：高風險命令「人機共治 + 限幅/限頻 + 全鏈路審計」。
*   **可維運 (Maintainability)**：SLO 儀表（Latency/Traffic/Errors/Saturation）；事故 Runbook（角色/時限/動作）。

