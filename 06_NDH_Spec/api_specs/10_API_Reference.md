# API 參考

**文件編號**: IDTF-V3.3-10  
**版本**: 2.0  
**日期**: 2025-10-12  
**作者**: C.C. Lin

## NDH API

### 部署 API

\`\`\`http
POST /api/v1/deploy
Content-Type: application/json
Authorization: Bearer <token>

{
  "fdl": "...",
  "environment": "production",
  "dry_run": false
}
\`\`\`

### 查詢 API

\`\`\`http
GET /api/v1/assets/{asset_id}
Authorization: Bearer <token>
\`\`\`

### 數據 API

\`\`\`http
GET /api/v1/data/{asset_id}/timeseries?start=<timestamp>&end=<timestamp>
Authorization: Bearer <token>
\`\`\`

## Factory Designer API

### 資產庫 API

\`\`\`http
POST /api/v1/assets/import
Content-Type: multipart/form-data

file: <iadl_file>
\`\`\`

### 佈局 API

\`\`\`http
POST /api/v1/layouts
Content-Type: application/json

{
  "name": "Factory Layout 1",
  "fdl": "..."
}
\`\`\`

## WebSocket API

### 即時狀態訂閱

\`\`\`javascript
const ws = new WebSocket('wss://ndh.example.com/ws/status');
ws.onmessage = (event) => {
  const status = JSON.parse(event.data);
  console.log(status);
};
\`\`\`

---

**參考**: [NDH 架構](06_NDH_Architecture.md) | [Factory Designer](04_Factory_Designer.md)
