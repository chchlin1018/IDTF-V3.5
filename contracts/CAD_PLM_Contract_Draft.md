# CAD/PLM 整合合約草案

## 1. 概述

本文件定義了 IDTF (Industrial Digital Twin Framework) 與 CAD/PLM (Computer-Aided Design/Product Lifecycle Management) 系統之間整合的合約草案。旨在建立標準化的介面和數據交換機制，以實現設計數據（例如 3D 模型、工程屬性、BOM）到 NDH (Neutral Data Hub) 的順暢流動，並支援雙向同步。

## 2. 數據模型與介面 (Schema Draft)

### 2.1. CAD/PLM 到 NDH (輸入)

CAD/PLM 系統應提供以下數據，用於生成或更新 NDH 中的 FDL (Factory Design Language) 和 IADL (Industrial Asset Description Language) 定義。

**CAD/PLM 數據匯出 Schema (GraphQL SDL 草案)**:

```graphql
# CAD/PLM 系統匯出的頂層物件
type CadPlmExport {
  exportId: ID!
  timestamp: String!
  sourceSystem: String! # 例如: Siemens Teamcenter, Dassault SOLIDWORKS
  project: Project!
}

# 專案資訊
type Project {
  projectId: ID!
  name: String!
  description: String
  version: String
  assets: [PlmAsset!] # 專案包含的資產列表
}

# PLM 資產定義 (對應 IADL AssetType)
type PlmAsset {
  partNumber: ID! # 資產的唯一部件號 (對應 IADL AssetType.id)
  name: String!
  description: String
  revision: String # 版本
  assetType: String # 例如: Pump, Motor, Valve
  manufacturer: String
  model: String
  properties: [PlmProperty!] # 靜態屬性
  cadModels: [CadModel!] # 相關的 CAD 模型引用
  billOfMaterials: [BomItem!] # 物料清單 (BOM)
  # ... 其他 IADL 相關屬性
}

# PLM 屬性
type PlmProperty {
  name: String!
  value: String!
  unit: String
}

# CAD 模型資訊 (對應 USD Model Reference)
type CadModel {
  modelId: ID!
  format: String! # 例如: STEP, JT, glTF, USD
  uri: String! # 模型的存儲位置 (例如 S3 URL, PLM 內部路徑)
  version: String
  # ... 其他 3D 模型相關元數據
}

# 物料清單項目 (對應 FDL 中的 Component 或 Nested Asset)
type BomItem {
  partNumber: ID! # 子部件號
  quantity: Int!
  unit: String
  # ... 其他 BOM 屬性
}

# 工廠佈局資訊 (對應 FDL FactoryDesign)
type FactoryLayout {
  layoutId: ID!
  name: String!
  description: String
  version: String
  areas: [LayoutArea!] # 佈局區域
}

# 佈局區域
type LayoutArea {
  areaId: ID!
  name: String!
  type: String! # 例如: ProductionZone, StorageArea
  position: Position! # 空間位置
  orientation: Orientation! # 方向
  assets: [LayoutAsset!] # 區域內的資產實例
}

# 佈局資產實例 (對應 FDL AssetInstance)
type LayoutAsset {
  instanceId: ID! # 資產實例 ID
  partNumber: ID! # 引用 PlmAsset 的部件號
  position: Position!
  orientation: Orientation!
  # ... 其他 FDL AssetInstance 相關屬性
}

# 空間位置
type Position {
  x: Float!
  y: Float!
  z: Float!
}

# 方向 (歐拉角或四元數)
type Orientation {
  roll: Float
  pitch: Float
  yaw: Float
  # 或
  # x: Float, y: Float, z: Float, w: Float
}

scalar JSON # 自定義 JSON 類型
```

### 2.2. NDH 到 CAD/PLM (輸出)

NDH 可能需要將運行時數據或配置回寫到 PLM 系統，例如更新資產的實際運行壽命、維護記錄或配置變更。

**NDH 回寫 Schema (GraphQL SDL 草案)**:

```graphql
type Mutation {
  updatePlmAssetStatus(
    partNumber: ID!,
    revision: String,
    status: String! # 例如: InService, OutOfService, UnderMaintenance
    lastMaintenanceDate: String
    runHours: Float
  ): PlmUpdateResult
}

type PlmUpdateResult {
  success: Boolean!
  message: String
}
```

## 3. 錯誤碼 (Error Codes)

CAD/PLM 整合應使用標準 HTTP 狀態碼結合自定義錯誤碼，例如：

*   **400 Bad Request**: `CADPLM-001: InvalidInputData`
*   **401 Unauthorized**: `CADPLM-002: AuthenticationFailed`
*   **404 Not Found**: `CADPLM-003: PartNumberNotFound`
*   **500 Internal Server Error**: `CADPLM-004: PlmSystemError`

## 4. 錯誤補償策略 (Error Compensation Strategy)

為確保數據一致性和系統韌性，應實施以下錯誤補償策略：

*   **冪等鍵 (Idempotency Keys)**: 每個數據交換請求應包含一個唯一的冪等鍵，確保重複提交同一請求不會導致重複處理。
*   **重試佇列 (Retry Queues)**: 對於臨時性錯誤，失敗的請求應被放入重試佇列，並在經過指數退避 (exponential backoff) 後重新嘗試。
*   **死信佇列 (Dead-Letter Queues, DLQ)**: 經過多次重試仍失敗的請求應被移至 DLQ，以便人工審查和處理，防止數據丟失。
*   **對賬批 (Reconciliation Batches)**: 定期運行批處理作業，比對 CAD/PLM 和 NDH 之間的數據，識別並修正不一致。

## 5. 驗收標準 (Acceptance Criteria)

提供三條閉環的時序圖，以驗證 CAD/PLM 與 NDH 之間數據流的正確性和完整性：

1.  **設計到 IADL (Design to IADL)**: CAD/PLM 系統匯出資產設計數據，NDH 接收並生成 IADL `AssetType` 定義。
2.  **採購到 FDL 狀態 (Procurement to FDL State)**: PLM 系統更新資產採購狀態，NDH 接收並更新 Asset Instance 的 FDL 狀態。
3.  **工單到設備狀態/回寫 (Work Order to Equipment Status/Write-back)**: MES/CMMS 系統生成工單，NDH 更新 Asset Instance 狀態，並將維護記錄回寫到 PLM 系統。

本合約草案為 CAD/PLM 與 IDTF 的整合提供了框架，旨在實現設計、製造和運營數據的無縫連接。
