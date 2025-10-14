# NDH API 規格書 (MES 整合擴展)

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 2.0

---

## 1. 簡介

本文件定義了 NDH (Neutral Data Hub) 的 API 規格,特別是為了與 MES (Manufacturing Execution System) 整合而新增的擴展。

## 2. API 端點

### 2.1 MES 事件 API

#### `POST /api/v1/mes/events`

用於接收來自 MES 的事件,並將其分派到 Message Queue。

- **Request Body**:

```json
{
  "eventType": "WorkOrderStarted",
  "timestamp": "2025-10-14T10:30:00Z",
  "payload": {
    "workOrderId": "WO-12345",
    "productId": "PROD-XYZ",
    "quantity": 1000,
    "equipmentId": "EQ-001"
  }
}
```

- **Response**:
  - `202 Accepted`: 事件已成功接收並排入佇列。
  - `400 Bad Request`: 請求格式錯誤。
  - `500 Internal Server Error`: 伺服器內部錯誤。

### 2.2 資產 API 擴展

#### `GET /api/v1/assets/{assetId}/production`

查詢特定資產的生產相關資訊。

- **Response**:

```json
{
  "assetId": "EQ-001",
  "production": {
    "current_work_order": "WO-12345",
    "current_product_id": "PROD-XYZ",
    "wip_status": "Processing",
    "throughput": 1000
  }
}
```

#### `PUT /api/v1/assets/{assetId}/production`

更新特定資產的生產相關資訊 (通常由 Asset Servant 內部使用)。

- **Request Body**:

```json
{
  "current_work_order": "WO-54321"
}
```

---

