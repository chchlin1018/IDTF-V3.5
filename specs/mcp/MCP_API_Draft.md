# MCP API 草案 (OpenAPI/GraphQL)

## 1. 概述

本文件為 IDTF (Industrial Digital Twin Framework) 中 MCP (Model Context Protocol) 的 API 草案，旨在定義 AI Agent 與 NDH (Neutral Data Hub) 之間互動的介面。此草案將涵蓋 OpenAPI (REST) 和 GraphQL 兩種可能的實現方式，以提供靈活且強大的數據存取和操作能力。

## 2. OpenAPI (REST) 草案

### 2.1. NDH 數據查詢與操作

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

### 2.2. 錯誤碼 (Error Codes)

標準 HTTP 狀態碼結合自定義錯誤碼：

*   **400 Bad Request**: 請求參數無效 (例如 `NDH-001: InvalidParameter`)。
*   **401 Unauthorized**: 認證失敗 (例如 `NDH-002: InvalidCredentials`)。
*   **403 Forbidden**: 權限不足 (例如 `NDH-003: AccessDenied`)。
*   **404 Not Found**: 資源不存在 (例如 `NDH-004: AssetNotFound`)。
*   **429 Too Many Requests**: 請求頻率過高 (例如 `NDH-005: RateLimitExceeded`)。
*   **500 Internal Server Error**: 服務器內部錯誤 (例如 `NDH-006: InternalError`)。
*   **503 Service Unavailable**: 服務暫時不可用。

## 3. GraphQL SDL (Schema Definition Language) 草案

GraphQL 提供了更靈活的查詢能力，允許 Agent 精確地獲取所需的數據，避免過度獲取 (over-fetching) 或不足獲取 (under-fetching)。

### 3.1. 類型定義 (Type Definitions)

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

### 3.2. 查詢 (Queries)

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

### 3.3. 變更 (Mutations)

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
