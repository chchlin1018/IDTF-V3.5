_# Factory Asset Library 實作細節

**版本**: 1.0  
**日期**: 2025-10-15  
**作者**: 林志錚 Michael Lin (Chih Cheng Lin)

---

## 1. 概述

本文件旨在詳細闡述 **Factory Asset Library** 在 **NDH (Neutral Data Hub) Runtime** 中的實作細節。Factory Asset Library 是 IDTF V3.5 的核心組件，提供一個集中式的儲存庫，用於管理工業資產的整個生命週期，從設計、部署到運營和維護。

### 1.1 目標

- **統一儲存**: 為資產定義 (IADL)、資產實例 (FDL)、即時數據、維護記錄和相關文件提供單一的儲存和管理平台。
- **版本控制**: 對資產定義和實例進行嚴格的版本控制，確保變更的可追溯性。
- **高效查詢**: 提供高效能的 API，以支援 IADL Editor、FDL Editor、數位分身應用和第三方系統的數據查詢需求。
- **高可用性**: 確保在生產環境中的高可用性和數據可靠性。

### 1.2 技術棧

| 組件 | 技術選型 | 理由 |
|---|---|---|
| **數據庫** | PostgreSQL | 功能強大、可靠性高，並支援 JSONB，適合儲存結構化和半結構化的資產數據。 |
| **時序數據庫** | InfluxDB | 專為時序數據設計，提供高效的數據寫入和查詢效能，適用於儲存感測器數據。 |
| **文件儲存** | MinIO | 與 S3 相容的物件儲存，提供可擴展、高可用的文件儲存解決方案。 |
| **API 服務** | FastAPI (Python) | 高效能的非同步 Web 框架，可快速開發 RESTful API。 |
| **訊息佇列** | RabbitMQ | 用於處理非同步任務，例如數據更新通知和事件觸發。 |

---

## 2. 數據庫結構設計

### 2.1 PostgreSQL 數據庫

#### `asset_definitions` 表

儲存 IADL 資產定義。

```sql
CREATE TABLE asset_definitions (
    id SERIAL PRIMARY KEY,
    asset_type VARCHAR(255) UNIQUE NOT NULL,
    version VARCHAR(50) NOT NULL,
    definition JSONB NOT NULL, -- 儲存完整的 IADL YAML 內容
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW()
);
```

#### `asset_definition_versions` 表

儲存資產定義的歷史版本。

```sql
CREATE TABLE asset_definition_versions (
    id SERIAL PRIMARY KEY,
    asset_type VARCHAR(255) NOT NULL,
    version VARCHAR(50) NOT NULL,
    definition JSONB NOT NULL,
    author VARCHAR(255),
    changes TEXT,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    FOREIGN KEY (asset_type) REFERENCES asset_definitions(asset_type)
);
```

#### `asset_instances` 表

儲存 FDL 資產實例。

```sql
CREATE TABLE asset_instances (
    id SERIAL PRIMARY KEY,
    instance_id VARCHAR(255) UNIQUE NOT NULL,
    asset_type VARCHAR(255) NOT NULL,
    location VARCHAR(255),
    instance_data JSONB, -- 儲存 FDL YAML 內容
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW(),
    FOREIGN KEY (asset_type) REFERENCES asset_definitions(asset_type)
);
```

#### `maintenance_records` 表

儲存維護記錄。

```sql
CREATE TABLE maintenance_records (
    id SERIAL PRIMARY KEY,
    instance_id VARCHAR(255) NOT NULL,
    record_type VARCHAR(100), -- e.g., preventive, corrective, inspection
    description TEXT,
    author VARCHAR(255),
    record_data JSONB, -- 儲存詳細的維護內容
    created_at TIMESTAMPTZ DEFAULT NOW(),
    FOREIGN KEY (instance_id) REFERENCES asset_instances(instance_id)
);
```

### 2.2 InfluxDB 數據庫

用於儲存資產的時序數據 (感測器讀數等)。

- **Measurement**: `asset_data`
- **Tags**:
    - `instance_id`: 資產實例 ID
    - `tag_name`: 數據標籤名稱
- **Fields**:
    - `value`: 數據值 (float, int, bool, string)

### 2.3 MinIO 物件儲存

用於儲存與資產相關的文件。

- **Bucket**: `asset-documents`
- **Object Naming Convention**: `{instance_id}/{document_type}/{file_name}`

---

## 3. API 設計

Factory Asset Library API 的設計與 `IADL_Editor_Complete_Architecture.md` 中定義的 `FactoryAssetLibrary` 類別一致。以下是更詳細的 API 端點定義。

**Base URL**: `/api/v1/asset_library`

### 3.1 資產定義管理

- **`POST /definitions`**: 創建新的資產定義。
- **`GET /definitions`**: 列出所有資產定義。
- **`GET /definitions/{asset_type}`**: 獲取特定資產定義的最新版本。
- **`PUT /definitions/{asset_type}`**: 更新資產定義 (創建新版本)。
- **`GET /definitions/{asset_type}/versions`**: 列出特定資產定義的所有歷史版本。

### 3.2 資產實例管理

- **`POST /instances`**: 創建新的資產實例。
- **`GET /instances`**: 列出所有資產實例 (可選用 `asset_type` 過濾)。
- **`GET /instances/{instance_id}`**: 獲取特定資產實例的詳細資訊。
- **`PUT /instances/{instance_id}`**: 更新資產實例。

### 3.3 即時與歷史數據

- **`GET /instances/{instance_id}/realtime`**: 獲取資產的即時數據 (從 InfluxDB 查詢最新值)。
- **`GET /instances/{instance_id}/history`**: 獲取資產的歷史數據 (從 InfluxDB 查詢時間範圍內的數據)。

### 3.4 維護記錄管理

- **`POST /instances/{instance_id}/maintenance`**: 為資產創建新的維護記錄。
- **`GET /instances/{instance_id}/maintenance`**: 列出資產的所有維護記錄。

### 3.5 文件管理

- **`POST /instances/{instance_id}/documents`**: 上傳與資產相關的文件。
- **`GET /instances/{instance_id}/documents`**: 列出資產的所有文件。
- **`GET /documents/{document_id}`**: 下載特定文件。

---

## 4. 實作考量

### 4.1 數據一致性

- 在更新資產定義時，需要確保所有關聯的資產實例都能夠平滑地過渡到新版本。
- 可以考慮使用一個背景任務來處理資產實例的升級，並記錄升級的狀態。

### 4.2 效能

- 對於頻繁查詢的 API 端點，應該實作快取機制 (例如使用 Redis)。
- 數據庫查詢應該被優化，特別是對於大型數據集的查詢。

### 4.3 安全性

- 所有 API 端點都應該透過 API Key 或 OAuth 2.0 進行保護。
- 對於文件的上傳和下載，應該進行嚴格的權限檢查。

---

## 5. 結論

這個實作細節文件為 Factory Asset Library 的開發提供了清晰的藍圖。透過結合 PostgreSQL、InfluxDB 和 MinIO，我們可以建立一個功能強大、可擴展且可靠的資產管理平台，為 IDTF V3.5 的成功奠定基礎。

