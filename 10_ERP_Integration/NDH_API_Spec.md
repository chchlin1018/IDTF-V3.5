# NDH API 規格書 (ERP 整合擴展)

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件定義了 NDH (Neutral Data Hub) 的 API 規格,特別是為了與 ERP (Enterprise Resource Planning) 整合而新增的擴展。

## 2. API 端點

### 2.1 ERP 事件 API

#### `POST /api/v1/erp/events`

用於接收來自 ERP 的事件,並將其分派到 Message Queue。

**Request Body**:

```json
{
  "eventType": "ProductionOrderCreated",
  "timestamp": "2025-10-14T11:00:00Z",
  "payload": {
    "productionOrderId": "PO-67890",
    "productId": "PROD-ABC",
    "quantity": 5000,
    "equipmentId": "EQ-002",
    "startDate": "2025-10-15T08:00:00Z",
    "endDate": "2025-10-16T17:00:00Z"
  }
}
```

**Response**:
- `202 Accepted`: 事件已成功接收並排入佇列
- `400 Bad Request`: 請求格式錯誤
- `500 Internal Server Error`: 伺服器內部錯誤

---

### 2.2 資產 ERP 資訊 API

#### `GET /api/v1/assets/{assetId}/erp`

查詢特定資產的 ERP 相關資訊。

**Response**:

```json
{
  "assetId": "EQ-002",
  "erp": {
    "equipment_id": "10001234",
    "cost_center": "CC-FAB1-ASSEMBLY",
    "work_center": "WC-DB-01",
    "asset_class": "MACH-ASSEMBLY"
  }
}
```

#### `PUT /api/v1/assets/{assetId}/erp`

更新特定資產的 ERP 相關資訊 (通常由 ERP Connector 使用)。

**Request Body**:

```json
{
  "cost_center": "CC-FAB1-ASSEMBLY-NEW"
}
```

---

### 2.3 KPI 查詢 API

#### `GET /api/v1/assets/{assetId}/kpis?name={kpiName}&start={startTime}&end={endTime}`

查詢特定資產的 KPI 數據。

**Query Parameters**:
- `name`: KPI 名稱 (oee, mtbf, mttr, availability, performance, quality)
- `start`: 開始時間 (ISO 8601 格式)
- `end`: 結束時間 (ISO 8601 格式)

**Response**:

```json
{
  "assetId": "EQ-002",
  "kpi": "oee",
  "value": 0.85,
  "timestamp": "2025-10-14T11:00:00Z",
  "breakdown": {
    "availability": 0.95,
    "performance": 0.92,
    "quality": 0.97
  }
}
```

---

### 2.4 生產確認 API

#### `POST /api/v1/production/confirmations`

用於將生產確認數據發送到 ERP。

**Request Body**:

```json
{
  "productionOrderId": "PO-67890",
  "equipmentId": "EQ-002",
  "confirmedQuantity": 4850,
  "scrapQuantity": 150,
  "actualStartTime": "2025-10-15T08:15:00Z",
  "actualEndTime": "2025-10-16T16:45:00Z",
  "laborHours": 16.5,
  "materialConsumption": [
    {
      "materialId": "MAT-001",
      "quantity": 5000,
      "unit": "pcs"
    }
  ],
  "energyConsumption": {
    "electricity": 1250.5,
    "unit": "kWh"
  }
}
```

**Response**:
- `201 Created`: 生產確認已成功創建並發送到 ERP
- `400 Bad Request`: 請求格式錯誤
- `500 Internal Server Error`: 伺服器內部錯誤

---

## 3. 訊息佇列主題 (Message Queue Topics)

### 3.1 從 ERP 到 NDH

- `erp.productionorder.created`: 生產工單創建事件
- `erp.productionorder.released`: 生產工單釋放事件
- `erp.equipment.master.updated`: 設備主數據更新事件

### 3.2 從 NDH 到 ERP

- `ndh.production.confirmation`: 生產確認事件
- `ndh.equipment.kpis`: 設備 KPI 事件
- `ndh.equipment.status.changed`: 設備狀態變更事件

---

## 4. 數據模型

### 4.1 生產工單 (Production Order)

```json
{
  "productionOrderId": "PO-67890",
  "productId": "PROD-ABC",
  "quantity": 5000,
  "equipmentId": "EQ-002",
  "startDate": "2025-10-15T08:00:00Z",
  "endDate": "2025-10-16T17:00:00Z",
  "status": "Released"
}
```

### 4.2 生產確認 (Production Confirmation)

```json
{
  "confirmationId": "CONF-12345",
  "productionOrderId": "PO-67890",
  "equipmentId": "EQ-002",
  "confirmedQuantity": 4850,
  "scrapQuantity": 150,
  "actualStartTime": "2025-10-15T08:15:00Z",
  "actualEndTime": "2025-10-16T16:45:00Z",
  "laborHours": 16.5,
  "materialConsumption": [...],
  "energyConsumption": {...}
}
```

### 4.3 設備 KPI

```json
{
  "assetId": "EQ-002",
  "kpi": "oee",
  "value": 0.85,
  "timestamp": "2025-10-14T11:00:00Z",
  "breakdown": {
    "availability": 0.95,
    "performance": 0.92,
    "quality": 0.97
  }
}
```

---

## 5. 錯誤處理

所有 API 端點都遵循統一的錯誤響應格式:

```json
{
  "error": {
    "code": "INVALID_REQUEST",
    "message": "Invalid production order ID format",
    "details": {
      "field": "productionOrderId",
      "value": "INVALID"
    }
  }
}
```

**錯誤代碼**:
- `INVALID_REQUEST`: 請求格式錯誤
- `RESOURCE_NOT_FOUND`: 資源不存在
- `INTERNAL_ERROR`: 伺服器內部錯誤
- `UNAUTHORIZED`: 未授權訪問

---

