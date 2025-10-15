# Factory Asset Library 實作細節

**版本**: 1.1  
**日期**: 2025-10-15  
**作者**: Manus AI

---

## 1. 概述

本文件旨在詳細闡述 **Factory Asset Library** 在 **NDH (Neutral Data Hub) Runtime** 中的實作細節。Factory Asset Library 是 IDTF V3.5 的核心組件，提供一個集中式的儲存庫，用於管理工業資產的整個生命週期，從設計、部署到運營和維護。本次更新將整合 IADL 和 FDL Spec 修正建議中提出的新概念，特別是 ISA-95 階層、多維度視圖、豐富數據模型和生命週期管理。

### 1.1 目標

-   **統一儲存**: 為資產定義 (IADL)、資產實例 (FDL)、即時數據、維護記錄和相關文件提供單一的儲存和管理平台。
-   **版本控制**: 對資產定義和實例進行嚴格的版本控制，確保變更的可追溯性。
-   **高效查詢**: 提供高效能的 API，以支援 IADL Editor、FDL Editor、數位分身應用和第三方系統的數據查詢需求。
-   **高可用性**: 確保在生產環境中的高可用性和數據可靠性。
-   **標準符合性**: 支援 ISA-95 設備階層模型，並能儲存 IADL 和 FDL 中定義的豐富屬性。
-   **生命週期管理**: 能夠追蹤和管理資產定義與實例的生命週期狀態。

### 1.2 技術棧

| 組件 | 技術選型 | 理由 |
|---|---|---|
| **數據庫** | PostgreSQL | 功能強大、可靠性高，並支援 JSONB，適合儲存結構化和半結構化的資產數據，特別是 IADL 和 FDL 的彈性數據模型。 |
| **時序數據庫** | InfluxDB | 專為時序數據設計，提供高效的數據寫入和查詢效能，適用於儲存感測器數據。 |
| **文件儲存** | MinIO | 與 S3 相容的物件儲存，提供可擴展、高可用的文件儲存解決方案。 |
| **API 服務** | FastAPI (Python) | 高效能的非同步 Web 框架，可快速開發 RESTful API。 |
| **訊息佇列** | RabbitMQ | 用於處理非同步任務，例如數據更新通知和事件觸發。 |

---

## 2. 數據庫結構設計

### 2.1 PostgreSQL 數據庫

#### `asset_definitions` 表

儲存 IADL 資產定義，包含 ISA-95 階層、邏輯分組和生命週期狀態等擴展屬性。

```sql
CREATE TABLE asset_definitions (
    id SERIAL PRIMARY KEY,
    asset_type VARCHAR(255) UNIQUE NOT NULL,
    version VARCHAR(50) NOT NULL,
    definition JSONB NOT NULL, -- 儲存完整的 IADL YAML 內容，包含所有擴展屬性、ISA-95 階層、邏輯分組和生命週期狀態定義
    isa95_hierarchy_level VARCHAR(50), -- ISA-95 階層級別 (e.g., equipment_type)
    lifecycle_status VARCHAR(50), -- 資產定義的生命週期狀態 (e.g., design_in_progress)
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
    definition JSONB NOT NULL, -- 儲存完整的 IADL YAML 內容
    author VARCHAR(255),
    changes TEXT,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    FOREIGN KEY (asset_type) REFERENCES asset_definitions(asset_type)
);
```

#### `asset_instances` 表

儲存 FDL 資產實例，包含 ISA-95 階層路徑、邏輯分組引用和實例生命週期狀態等擴展屬性。

```sql
CREATE TABLE asset_instances (
    id SERIAL PRIMARY KEY,
    instance_id VARCHAR(255) UNIQUE NOT NULL,
    asset_type VARCHAR(255) NOT NULL,
    location VARCHAR(255), -- 物理位置
    instance_data JSONB, -- 儲存 FDL YAML 內容，包含所有實例特有屬性、ISA-95 階層路徑、邏輯分組引用和生命週期狀態
    isa95_hierarchy_path TEXT, -- 資產實例的完整 ISA-95 階層路徑 (e.g., Enterprise/SiteA/Area1/ProductionUnit/Pump_001)
    operational_status VARCHAR(50), -- 資產實例的運營狀態 (e.g., running, stopped, under_maintenance)
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW(),
    FOREIGN KEY (asset_type) REFERENCES asset_definitions(asset_type)
);
```

#### `maintenance_records` 表

儲存維護記錄，可與資產實例的生命週期事件關聯。

```sql
CREATE TABLE maintenance_records (
    id SERIAL PRIMARY KEY,
    instance_id VARCHAR(255) NOT NULL,
    record_type VARCHAR(100), -- e.g., preventive, corrective, inspection
    description TEXT,
    author VARCHAR(255),
    record_data JSONB, -- 儲存詳細的維護內容，可包含生命週期事件觸發資訊
    created_at TIMESTAMPTZ DEFAULT NOW(),
    FOREIGN KEY (instance_id) REFERENCES asset_instances(instance_id)
);
```

#### `asset_logical_groups` 表 (新增)

儲存資產的邏輯分組定義，用於支援 IADL 中的次要階層。

```sql
CREATE TABLE asset_logical_groups (
    id SERIAL PRIMARY KEY,
    group_type VARCHAR(100) NOT NULL, -- e.g., discipline, maintenance, safety
    group_name VARCHAR(255) NOT NULL,
    description TEXT,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW(),
    UNIQUE (group_type, group_name)
);
```

#### `asset_instance_logical_group_map` 表 (新增)

映射資產實例與邏輯分組的關係。

```sql
CREATE TABLE asset_instance_logical_group_map (
    instance_id VARCHAR(255) NOT NULL,
    logical_group_id INT NOT NULL,
    PRIMARY KEY (instance_id, logical_group_id),
    FOREIGN KEY (instance_id) REFERENCES asset_instances(instance_id),
    FOREIGN KEY (logical_group_id) REFERENCES asset_logical_groups(id)
);
```

### 2.2 InfluxDB 數據庫

用於儲存資產的時序數據 (感測器讀數等)。

-   **Measurement**: `asset_data`
-   **Tags**:
    -   `instance_id`: 資產實例 ID
    -   `tag_name`: 數據標籤名稱
    -   `isa95_hierarchy_path`: 資產實例的 ISA-95 階層路徑 (用於快速過濾和聚合)
    -   `logical_group_type`: 邏輯分組類型 (e.g., discipline)
    -   `logical_group_name`: 邏輯分組名稱 (e.g., mechanical)
-   **Fields**:
    -   `value`: 數據值 (float, int, bool, string)

### 2.3 MinIO 物件儲存

用於儲存與資產相關的文件。

-   **Bucket**: `asset-documents`
-   **Object Naming Convention**: `{instance_id}/{document_type}/{file_name}` (可根據 ISA-95 階層或邏輯分組進行組織)

---

## 3. API 設計

Factory Asset Library API 的設計與 `IADL_Editor_Complete_Architecture.md` 中定義的 `FactoryAssetLibrary` 類別一致。以下是更詳細的 API 端點定義，並考慮了新的數據模型。

**Base URL**: `/api/v1/asset_library`

### 3.1 資產定義管理

-   **`POST /definitions`**: 創建新的資產定義。請求體包含完整的 IADL YAML 內容，API 將解析並儲存 ISA-95 階層、生命週期狀態等元數據。
-   **`GET /definitions`**: 列出所有資產定義 (可選用 `isa95_level`, `lifecycle_status` 過濾)。
-   **`GET /definitions/{asset_type}`**: 獲取特定資產定義的最新版本。
-   **`PUT /definitions/{asset_type}`**: 更新資產定義 (創建新版本)。API 將處理版本控制和元數據更新。
-   **`GET /definitions/{asset_type}/versions`**: 列出特定資產定義的所有歷史版本。

### 3.2 資產實例管理

-   **`POST /instances`**: 創建新的資產實例。請求體包含完整的 FDL YAML 內容，API 將解析並儲存 ISA-95 階層路徑、運營狀態、邏輯分組引用等元數據。
-   **`GET /instances`**: 列出所有資產實例 (可選用 `asset_type`, `isa95_path_prefix`, `operational_status`, `logical_group_type`, `logical_group_name` 過濾)。
-   **`GET /instances/{instance_id}`**: 獲取特定資產實例的詳細資訊。
-   **`PUT /instances/{instance_id}`**: 更新資產實例。API 將處理實例數據和元數據的更新，包括生命週期狀態的變更。

### 3.3 即時與歷史數據

-   **`GET /instances/{instance_id}/realtime`**: 獲取資產的即時數據 (從 InfluxDB 查詢最新值)。
-   **`GET /instances/{instance_id}/history`**: 獲取資產的歷史數據 (從 InfluxDB 查詢時間範圍內的數據)。可選用 `tag_name` 過濾。

### 3.4 維護記錄管理

-   **`POST /instances/{instance_id}/maintenance`**: 為資產創建新的維護記錄。可包含生命週期事件觸發資訊。
-   **`GET /instances/{instance_id}/maintenance`**: 列出資產的所有維護記錄。

### 3.5 文件管理

-   **`POST /instances/{instance_id}/documents`**: 上傳與資產相關的文件。API 將處理文件儲存到 MinIO 並更新資產實例的 `document_links` 屬性。
-   **`GET /instances/{instance_id}/documents`**: 列出資產的所有文件。
-   **`GET /documents/{document_id}`**: 下載特定文件。

### 3.6 邏輯分組管理 (新增)

-   **`POST /logical_groups`**: 創建新的邏輯分組。
-   **`GET /logical_groups`**: 列出所有邏輯分組。
-   **`POST /instances/{instance_id}/logical_groups/{group_id}`**: 將資產實例添加到邏輯分組。
-   **`DELETE /instances/{instance_id}/logical_groups/{group_id}`**: 將資產實例從邏輯分組移除。

---

## 4. 實作考量

### 4.1 數據一致性

-   在更新資產定義時，需要確保所有關聯的資產實例都能夠平滑地過渡到新版本。可以考慮使用一個背景任務來處理資產實例的升級，並記錄升級的狀態。
-   ISA-95 階層路徑和邏輯分組引用應在資產實例創建和更新時進行驗證，確保其有效性。

### 4.2 效能

-   對於頻繁查詢的 API 端點，應該實作快取機制 (例如使用 Redis)。
-   數據庫查詢應該被優化，特別是對於大型數據集的查詢，例如使用索引來加速 ISA-95 階層路徑和邏輯分組的查詢。
-   InfluxDB 的查詢應針對時間範圍和標籤進行優化。

### 4.3 安全性

-   所有 API 端點都應該透過 API Key 或 OAuth 2.0 進行保護。
-   對於文件的上傳和下載，應該進行嚴格的權限檢查。
-   數據模型中的敏感資訊應加密儲存或進行適當的存取控制。

### 4.4 可擴展性

-   數據庫結構應支援未來 IADL 和 FDL 規範的擴展，例如增加新的屬性類型或階層層級。
-   API 應設計為可版本化，以便在不影響現有客戶的情況下引入新功能。

---

## 5. 結論

這個實作細節文件為 Factory Asset Library 的開發提供了清晰的藍圖。透過結合 PostgreSQL、InfluxDB 和 MinIO，並深度整合 ISA-95 階層、多維度視圖、豐富數據模型和生命週期管理，我們可以建立一個功能強大、可擴展且可靠的資產管理平台，為 IDTF V3.5 的成功奠定基礎，並使其更能適應複雜的工業數位分身應用場景。

