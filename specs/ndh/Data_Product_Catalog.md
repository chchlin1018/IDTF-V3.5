# 數據產品目錄 (Data Product Catalog)

## 1. 概述

本文件定義了 NDH (Neutral Data Hub) 的數據產品目錄規範，旨在將 NDH 中整合的數據轉化為可發現、可理解、可信賴且易於使用的數據產品。數據產品化是實現數據價值的關鍵，它將數據視為一種產品，提供給內部和外部的數據消費者（例如 AI Agent、應用程式、數據分析師）。

## 2. 數據產品定義 (Data Product Definition)

每個數據產品都將透過一個標準化的元數據描述文件來定義，通常以 YAML 或 JSON 格式呈現。

```yaml
productId: "urn:ndh:dataproduct:factory_a_oee_dashboard_data"
name: "Factory A OEE Dashboard Data"
description: "提供工廠 A 各生產線的實時 OEE (整體設備效率) 數據，用於儀表板展示和分析。"
version: "1.0.0"
owner: "data_engineering_team"
contact: "data.team@example.com"

sourceSystems:
  - systemId: "mes_system_v2"
    dataEntities: ["production_order", "equipment_status"]
  - systemId: "scada_system_v3"
    dataEntities: ["asset_telemetry_oee"]

dataModel:
  type: "json_schema"
  schemaUri: "https://ndh.example.com/schemas/data_product_oee_v1.json"
  exampleData: |
    {
      "timestamp": "2025-10-26T14:30:00Z",
      "productionLineId": "PL-01",
      "oee": 0.85,
      "availability": 0.90,
      "performance": 0.95,
      "quality": 0.99
    }

accessMethods:
  - type: "mqtt_topic"
    topic: "tenants/harvatek_group/ndh/dataproducts/factory_a_oee_dashboard_data"
    qos: 1
  - type: "rest_api"
    endpoint: "https://api.ndh.example.com/v1/dataproducts/factory_a_oee_dashboard_data"
    authentication: "oauth2"
  - type: "graphql_query"
    queryName: "factoryAOEEData"

qualityMetrics:
  - metric: "data_freshness"
    value: "5s"
  - metric: "data_completeness"
    value: "99.9%"
  - metric: "data_accuracy"
    value: "99%"

termsOfUse:
  - "僅限內部使用"
  - "禁止轉售"

changeLog:
  - version: "1.0.0"
    date: "2025-10-26"
    description: "初始版本，提供工廠 A OEE 實時數據。"
```

**字段說明:**

*   **`productId` (string, URI)**: 數據產品的唯一識別符。
*   **`name` (string)**: 數據產品的顯示名稱。
*   **`description` (string)**: 數據產品的詳細描述。
*   **`version` (string)**: 數據產品的版本號。
*   **`owner` (string)**: 數據產品的負責人或團隊。
*   **`contact` (string)**: 數據產品的聯繫方式。
*   **`sourceSystems` (array of objects)**: 數據產品的來源系統和相關數據實體。
*   **`dataModel` (object)**: 數據產品的數據模型定義。
    *   `type` (string): 數據模型類型，例如 `json_schema`。
    *   `schemaUri` (string, URI): 數據模型的 JSON Schema 定義的 URI。
    *   `exampleData` (string): 數據產品的範例數據。
*   **`accessMethods` (array of objects)**: 數據產品的存取方式。
    *   `type` (string): 存取方式類型，例如 `mqtt_topic`、`rest_api`、`graphql_query`。
    *   `topic` / `endpoint` / `queryName` (string): 具體的存取點。
    *   `authentication` (string, 可選): 存取所需的認證方式。
*   **`qualityMetrics` (array of objects)**: 數據產品的質量指標。
    *   `metric` (string): 質量指標名稱，例如 `data_freshness`、`data_completeness`、`data_accuracy`。
    *   `value` (string): 指標值。
*   **`termsOfUse` (array of strings)**: 數據產品的使用條款。
*   **`changeLog` (array of objects)**: 數據產品的版本變更日誌。

## 3. 數據產品生命週期管理 (Data Product Lifecycle Management)

NDH 將提供工具和流程來管理數據產品的整個生命週期，包括：

*   **創建 (Creation)**: 數據產品的定義和註冊。
*   **發佈 (Publishing)**: 將數據產品發佈到目錄中，使其可被發現和存取。
*   **版本控制 (Versioning)**: 管理數據產品的不同版本，確保向後兼容性或明確的版本升級路徑。
*   **監控 (Monitoring)**: 監控數據產品的數據質量、存取模式和性能。
*   **退役 (Deprecation)**: 當數據產品不再需要時，進行退役處理，並通知相關消費者。

## 4. 數據產品發現與訂閱 (Data Product Discovery & Subscription)

NDH 將提供以下機制來幫助數據消費者發現和訂閱數據產品：

*   **數據產品目錄服務 (Catalog Service)**: 提供可搜索的介面，允許使用者根據關鍵字、所有者、來源系統等條件查詢數據產品。
*   **API 接口**: 透過 NDH 的 API (REST/GraphQL)，程式化地查詢數據產品元數據。
*   **自動通知**: 當新的數據產品發佈或現有產品更新時，自動通知相關的潛在消費者。

數據產品目錄是 NDH 實現數據民主化和數據驅動決策的基石，它將複雜的工業數據轉化為易於消費的價值。


## 5. 非功能需求 (Non-Functional Requirements, NFR) 與決策門檻

本規範所定義的數據產品目錄機制需滿足以下非功能需求：

*   **延遲 (Latency)**：同廠查詢/事件 p95 < 300ms；跨廠/雲邊 p95 < 800ms。
*   **吞吐 (Throughput)**：≥ 100k tags / 1k EPS（每廠），可線性擴張。
*   **可用性 (Availability)**：NDH 控制面 99.9%，告警通道 99.99%。
*   **一致性 (Consistency)**：事件重放恢復狀態一致（需定義 3 條測例）。
*   **安全 (Security)**：高風險命令「人機共治 + 限幅/限頻 + 全鏈路審計」。
*   **可維運 (Maintainability)**：SLO 儀表（Latency/Traffic/Errors/Saturation）；事故 Runbook（角色/時限/動作）。

