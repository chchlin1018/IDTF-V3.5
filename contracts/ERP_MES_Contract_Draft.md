# ERP/MES 整合合約草案

## 1. 概述

本文件定義了 IDTF (Industrial Digital Twin Framework) 與 ERP (Enterprise Resource Planning) / MES (Manufacturing Execution System) 系統之間整合的合約草案。旨在建立標準化的介面和數據交換機制，以實現生產計劃、工單管理、物料追溯、人員排程等運營數據與 NDH (Neutral Data Hub) 的順暢流動，並支援雙向同步。

## 2. 數據模型與介面 (Schema Draft)

### 2.1. ERP/MES 到 NDH (輸入)

ERP/MES 系統應提供以下數據，用於更新 NDH 中 Asset Instance 的狀態、觸發事件或提供上下文信息。

**ERP/MES 數據匯出 Schema (GraphQL SDL 草案)**:

```graphql
# ERP/MES 系統匯出的頂層物件
type ErpMesExport {
  exportId: ID!
  timestamp: String!
  sourceSystem: String! # 例如: SAP ERP, Siemens Opcenter MES
  data: ErpMesData!
}

# ERP/MES 數據類型
type ErpMesData {
  productionOrders: [ProductionOrder!]
  materialMovements: [MaterialMovement!]
  personnelAssignments: [PersonnelAssignment!]
  equipmentStatuses: [EquipmentStatus!]
}

# 生產工單
type ProductionOrder {
  orderId: ID!
  productId: ID!
  quantity: Int!
  unit: String!
  status: String! # 例如: Planned, Released, InProgress, Completed, Closed
  startTime: String
  endTime: String
  assetInstanceId: ID # 關聯的 Asset Instance
  # ... 其他工單屬性
}

# 物料移動
type MaterialMovement {
  movementId: ID!
materialId: ID!
  quantity: Float!
  unit: String!
  movementType: String! # 例如: Consumption, Production, Transfer
  timestamp: String!
  sourceLocation: String
  targetLocation: String
  assetInstanceId: ID # 關聯的 Asset Instance
  # ... 其他物料追溯屬性
}

# 人員排程/作業
type PersonnelAssignment {
  assignmentId: ID!
  personnelId: ID!
  role: String!
  startTime: String!
  endTime: String
  assetInstanceId: ID # 關聯的 Asset Instance
  # ... 其他人員相關屬性
}

# 設備狀態 (來自 MES)
type EquipmentStatus {
  assetInstanceId: ID!
  status: String! # 例如: Running, Idle, Fault, Maintenance
  reasonCode: String
  timestamp: String!
  # ... 其他狀態屬性
}

scalar JSON # 自定義 JSON 類型
```

### 2.2. NDH 到 ERP/MES (輸出)

NDH 可能需要將實時生產數據、設備狀態或事件回寫到 ERP/MES 系統，以更新生產進度、觸發物料補給或記錄設備停機。

**NDH 回寫 Schema (GraphQL SDL 草案)**:

```graphql
type Mutation {
  updateProductionOrderStatus(
    orderId: ID!,
    status: String!,
    actualQuantity: Int
    actualEndTime: String
  ): ErpMesUpdateResult

  updateEquipmentStatus(
    assetInstanceId: ID!,
    status: String!,
    reasonCode: String
    timestamp: String!
  ): ErpMesUpdateResult

  recordMaterialConsumption(
    assetInstanceId: ID!,
    materialId: ID!,
    quantity: Float!,
    unit: String!
    timestamp: String!
  ): ErpMesUpdateResult
}

type ErpMesUpdateResult {
  success: Boolean!
  message: String
}
```

## 3. 錯誤碼 (Error Codes)

ERP/MES 整合應使用標準 HTTP 狀態碼結合自定義錯誤碼，例如：

*   **400 Bad Request**: `ERPMES-001: InvalidInputData`
*   **401 Unauthorized**: `ERPMES-002: AuthenticationFailed`
*   **404 Not Found**: `ERPMES-003: OrderNotFound`
*   **500 Internal Server Error**: `ERPMES-004: ErpMesSystemError`

## 4. 錯誤補償策略 (Error Compensation Strategy)

為確保數據一致性和系統韌性，應實施以下錯誤補償策略：

*   **冪等鍵 (Idempotency Keys)**: 每個數據交換請求應包含一個唯一的冪等鍵，確保重複提交同一請求不會導致重複處理。
*   **重試佇列 (Retry Queues)**: 對於臨時性錯誤，失敗的請求應被放入重試佇列，並在經過指數退避 (exponential backoff) 後重新嘗試。
*   **死信佇列 (Dead-Letter Queues, DLQ)**: 經過多次重試仍失敗的請求應被移至 DLQ，以便人工審查和處理，防止數據丟失。
*   **對賬批 (Reconciliation Batches)**: 定期運行批處理作業，比對 ERP/MES 和 NDH 之間的數據，識別並修正不一致。

## 5. 驗收標準 (Acceptance Criteria)

提供三條閉環的時序圖，以驗證 ERP/MES 與 NDH 之間數據流的正確性和完整性：

1.  **設計到 IADL (Design to IADL)**: CAD/PLM 系統匯出資產設計數據，NDH 接收並生成 IADL `AssetType` 定義。
2.  **採購到 FDL 狀態 (Procurement to FDL State)**: ERP 系統更新資產採購狀態，NDH 接收並更新 Asset Instance 的 FDL 狀態。
3.  **工單到設備狀態/回寫 (Work Order to Equipment Status/Write-back)**: MES 系統生成工單，NDH 更新 Asset Instance 狀態，並將維護記錄回寫到 ERP/MES 系統。

本合約草案為 ERP/MES 與 IDTF 的整合提供了框架，旨在實現運營數據與數位分身的無縫連接。


## 6. 非功能需求 (Non-Functional Requirements, NFR) 與決策門檻

本合約所定義的 ERP/MES 整合需滿足以下非功能需求：

*   **延遲 (Latency)**：同廠查詢/事件 p95 < 300ms；跨廠/雲邊 p95 < 800ms。
*   **吞吐 (Throughput)**：≥ 100k tags / 1k EPS（每廠），可線性擴張。
*   **可用性 (Availability)**：NDH 控制面 99.9%，告警通道 99.99%。
*   **一致性 (Consistency)**：事件重放恢復狀態一致（需定義 3 條測例）。
*   **安全 (Security)**：高風險命令「人機共治 + 限幅/限頻 + 全鏈路審計」。
*   **可維運 (Maintainability)**：SLO 儀表（Latency/Traffic/Errors/Saturation）；事故 Runbook（角色/時限/動作）。

