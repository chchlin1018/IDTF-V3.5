# NDH MCP Server 整合設計文件

**版本**: IDTF V3.5  
**日期**: 2025年10月14日  
**狀態**: 設計階段

---

## 目錄

1. [概述](#概述)
2. [MCP 協議簡介](#mcp-協議簡介)
3. [NDH 作為 MCP Server 的架構設計](#ndh-作為-mcp-server-的架構設計)
4. [MCP 功能映射](#mcp-功能映射)
5. [實作細節](#實作細節)
6. [安全性與授權](#安全性與授權)
7. [部署架構](#部署架構)
8. [使用場景](#使用場景)
9. [實作路線圖](#實作路線圖)
10. [附錄](#附錄)

---

## 1. 概述

### 1.1 目標

將 **NDH (Neutral Data Hub)** 整合為 **MCP (Model Context Protocol) Server**，使 AI 助理（如 Claude、ChatGPT、Manus 等）能夠直接與工業數位分身系統互動，實現：

- **智能查詢**：AI 助理可以查詢工廠資產狀態、歷史數據、即時數據
- **智能操作**：AI 助理可以執行工廠操作（在授權範圍內）
- **智能分析**：AI 助理可以分析工廠數據、生成報告、提供建議
- **智能設計**：AI 助理可以協助設計工廠佈局、優化資產配置

### 1.2 價值主張

**對使用者**：
- 使用自然語言與數位分身系統互動
- 快速獲取工廠洞察和分析
- 降低系統使用門檻

**對開發者**：
- 標準化的 AI 整合介面
- 無需為每個 AI 平台單獨開發整合
- 自動生成工具描述和文檔

**對系統**：
- 擴展 NDH 的應用場景
- 提升系統的智能化水平
- 增強生態系統的開放性

### 1.3 設計原則

1. **安全第一**：所有操作都需要明確的授權和審計
2. **標準遵循**：完全遵循 MCP 協議規範
3. **向後相容**：不影響現有 NDH 功能
4. **可擴展性**：支援未來新增功能
5. **效能優化**：最小化延遲，支援高並發

---

## 2. MCP 協議簡介

### 2.1 MCP 核心概念

**Model Context Protocol (MCP)** 是一個開放協議，用於在 LLM 應用程式與外部數據源和工具之間建立標準化連接。

**核心角色**：
- **Host**：LLM 應用程式（如 Claude Desktop、Manus）
- **Client**：Host 內部的連接器
- **Server**：提供上下文和能力的服務（NDH 將扮演此角色）

**通訊協議**：
- 基於 **JSON-RPC 2.0** 訊息格式
- 支援 **stateful connections**（有狀態連接）
- 支援 **capability negotiation**（能力協商）

### 2.2 MCP Server 提供的功能

#### 2.2.1 Resources（資源）

**定義**：上下文和數據，供使用者或 AI 模型使用

**NDH 對應**：
- 工廠資產列表
- 資產即時狀態
- 資產歷史數據
- 工廠佈局資訊
- IADL/FDL 定義

#### 2.2.2 Tools（工具）

**定義**：AI 模型可以執行的函數

**NDH 對應**：
- 查詢資產狀態
- 設置資產參數
- 執行資產命令
- 查詢歷史數據
- 生成報告
- 分析數據趨勢

#### 2.2.3 Prompts（提示模板）

**定義**：為使用者提供的模板化訊息和工作流程

**NDH 對應**：
- 工廠巡檢提示
- 異常分析提示
- 效能優化提示
- 預測性維護提示

### 2.3 MCP Client 提供的功能（NDH 可選實作）

#### 2.3.1 Sampling（採樣）

**定義**：Server 發起的 LLM 互動

**NDH 使用場景**：
- 自動生成資產描述
- 自動分析異常原因
- 自動生成維護建議

#### 2.3.2 Roots（根目錄）

**定義**：Server 查詢操作邊界

**NDH 使用場景**：
- 查詢工廠邊界
- 查詢資產範圍

---

## 3. NDH 作為 MCP Server 的架構設計

### 3.1 整體架構

```
┌─────────────────────────────────────────────────────────────┐
│                        AI Host                               │
│  (Claude Desktop, Manus, ChatGPT, etc.)                     │
└────────────────────────┬────────────────────────────────────┘
                         │ MCP Protocol (JSON-RPC 2.0)
                         │ Transport: STDIO / HTTP+SSE
                         ↓
┌─────────────────────────────────────────────────────────────┐
│                   NDH MCP Server                             │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  MCP Protocol Handler                                 │  │
│  │  - JSON-RPC 2.0 Parser                               │  │
│  │  - Capability Negotiation                            │  │
│  │  - Session Management                                │  │
│  └──────────────────────────────────────────────────────┘  │
│                         ↓                                    │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  MCP Feature Adapters                                 │  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐          │  │
│  │  │Resources │  │  Tools   │  │ Prompts  │          │  │
│  │  │ Adapter  │  │ Adapter  │  │ Adapter  │          │  │
│  │  └──────────┘  └──────────┘  └──────────┘          │  │
│  └──────────────────────────────────────────────────────┘  │
│                         ↓                                    │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  Authorization & Security Layer                       │  │
│  │  - OAuth 2.0 / API Key Authentication               │  │
│  │  - Role-Based Access Control (RBAC)                 │  │
│  │  - Operation Audit Logging                          │  │
│  └──────────────────────────────────────────────────────┘  │
│                         ↓                                    │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  NDH Core API Layer                                   │  │
│  │  - Asset Management API                              │  │
│  │  - Data Query API                                    │  │
│  │  - Command Execution API                             │  │
│  │  - Analytics API                                     │  │
│  └──────────────────────────────────────────────────────┘  │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ↓
┌─────────────────────────────────────────────────────────────┐
│              Existing NDH Components                         │
│  - Asset Servants                                            │
│  - Time-Series Database                                      │
│  - PostgreSQL                                                │
│  - Protocol Adapters (OPC UA, MQTT, Modbus)                │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 組件說明

#### 3.2.1 MCP Protocol Handler

**職責**：
- 處理 JSON-RPC 2.0 訊息的解析和序列化
- 管理 MCP 連接的生命週期（初始化、能力協商、關閉）
- 處理 MCP 標準方法（initialize, initialized, ping, etc.）

**實作技術**：
- **Python**: 使用 `mcp` Python SDK
- **TypeScript**: 使用 `@modelcontextprotocol/sdk` TypeScript SDK

**關鍵方法**：
```python
# 初始化
async def handle_initialize(params: InitializeParams) -> InitializeResult

# 能力協商
async def handle_capabilities() -> ServerCapabilities

# 列出資源
async def handle_list_resources() -> ListResourcesResult

# 讀取資源
async def handle_read_resource(uri: str) -> ReadResourceResult

# 列出工具
async def handle_list_tools() -> ListToolsResult

# 調用工具
async def handle_call_tool(name: str, arguments: dict) -> CallToolResult

# 列出提示
async def handle_list_prompts() -> ListPromptsResult

# 獲取提示
async def handle_get_prompt(name: str, arguments: dict) -> GetPromptResult
```

#### 3.2.2 MCP Feature Adapters

**Resources Adapter**：
- 將 NDH 的資產、數據、配置映射為 MCP Resources
- 支援動態資源發現
- 支援資源訂閱和更新通知

**Tools Adapter**：
- 將 NDH 的 API 操作映射為 MCP Tools
- 自動生成工具描述（JSON Schema）
- 處理工具調用的參數驗證和執行

**Prompts Adapter**：
- 提供預定義的提示模板
- 支援動態參數替換
- 支援多語言提示

#### 3.2.3 Authorization & Security Layer

**認證機制**：
- **API Key**: 用於簡單的服務間認證
- **OAuth 2.0**: 用於使用者授權
- **JWT**: 用於無狀態認證

**授權機制**：
- **RBAC**: 基於角色的存取控制
  - `viewer`: 只能查詢資源和數據
  - `operator`: 可以執行操作命令
  - `admin`: 完全控制權限

**審計日誌**：
- 記錄所有 MCP 操作
- 記錄使用者、時間、操作、參數、結果
- 支援審計查詢和報告

#### 3.2.4 NDH Core API Layer

**現有 API 的封裝**：
- 不修改現有 NDH API
- 在 MCP Server 層進行適配和轉換
- 確保向後相容

---

## 4. MCP 功能映射

### 4.1 Resources 映射

#### 4.1.1 工廠資產列表

**Resource URI**: `ndh://factory/{factory_id}/assets`

**描述**: 列出工廠中的所有資產

**MIME Type**: `application/json`

**範例**:
```json
{
  "uri": "ndh://factory/harvatek-led/assets",
  "name": "Harvatek LED Factory Assets",
  "description": "List of all assets in Harvatek LED factory",
  "mimeType": "application/json"
}
```

**內容**:
```json
{
  "factory_id": "harvatek-led",
  "total_assets": 2847,
  "assets": [
    {
      "id": "asset-001",
      "name": "Conveyor Belt 1",
      "type": "conveyor",
      "location": "Floor 1, Zone A",
      "status": "running"
    },
    ...
  ]
}
```

#### 4.1.2 資產即時狀態

**Resource URI**: `ndh://factory/{factory_id}/asset/{asset_id}/status`

**描述**: 獲取資產的即時狀態

**MIME Type**: `application/json`

**範例**:
```json
{
  "uri": "ndh://factory/harvatek-led/asset/asset-001/status",
  "name": "Conveyor Belt 1 Status",
  "description": "Real-time status of Conveyor Belt 1",
  "mimeType": "application/json"
}
```

**內容**:
```json
{
  "asset_id": "asset-001",
  "timestamp": "2025-10-14T12:00:00Z",
  "status": "running",
  "data_points": {
    "speed": 1.5,
    "temperature": 35.2,
    "vibration": 0.02
  }
}
```

#### 4.1.3 資產歷史數據

**Resource URI**: `ndh://factory/{factory_id}/asset/{asset_id}/history?start={start}&end={end}`

**描述**: 獲取資產的歷史數據

**MIME Type**: `application/json`

#### 4.1.4 工廠佈局

**Resource URI**: `ndh://factory/{factory_id}/layout`

**描述**: 獲取工廠的 FDL 佈局定義

**MIME Type**: `application/yaml`

#### 4.1.5 資產定義

**Resource URI**: `ndh://factory/{factory_id}/asset/{asset_id}/definition`

**描述**: 獲取資產的 IADL 定義

**MIME Type**: `application/yaml`

### 4.2 Tools 映射

#### 4.2.1 查詢資產狀態

**Tool Name**: `query_asset_status`

**描述**: 查詢指定資產的即時狀態

**輸入 Schema**:
```json
{
  "type": "object",
  "properties": {
    "factory_id": {
      "type": "string",
      "description": "工廠 ID"
    },
    "asset_id": {
      "type": "string",
      "description": "資產 ID"
    }
  },
  "required": ["factory_id", "asset_id"]
}
```

**輸出範例**:
```json
{
  "content": [
    {
      "type": "text",
      "text": "Conveyor Belt 1 is currently running at 1.5 m/s. Temperature: 35.2°C, Vibration: 0.02 mm/s."
    }
  ]
}
```

#### 4.2.2 設置資產參數

**Tool Name**: `set_asset_parameter`

**描述**: 設置資產的參數值

**輸入 Schema**:
```json
{
  "type": "object",
  "properties": {
    "factory_id": {
      "type": "string",
      "description": "工廠 ID"
    },
    "asset_id": {
      "type": "string",
      "description": "資產 ID"
    },
    "parameter": {
      "type": "string",
      "description": "參數名稱"
    },
    "value": {
      "type": "number",
      "description": "參數值"
    }
  },
  "required": ["factory_id", "asset_id", "parameter", "value"]
}
```

**輸出範例**:
```json
{
  "content": [
    {
      "type": "text",
      "text": "Successfully set speed to 2.0 m/s for Conveyor Belt 1."
    }
  ]
}
```

#### 4.2.3 查詢歷史數據

**Tool Name**: `query_historical_data`

**描述**: 查詢資產的歷史數據

**輸入 Schema**:
```json
{
  "type": "object",
  "properties": {
    "factory_id": {
      "type": "string",
      "description": "工廠 ID"
    },
    "asset_id": {
      "type": "string",
      "description": "資產 ID"
    },
    "data_point": {
      "type": "string",
      "description": "數據點名稱"
    },
    "start_time": {
      "type": "string",
      "format": "date-time",
      "description": "開始時間 (ISO 8601)"
    },
    "end_time": {
      "type": "string",
      "format": "date-time",
      "description": "結束時間 (ISO 8601)"
    },
    "aggregation": {
      "type": "string",
      "enum": ["raw", "avg", "min", "max", "sum"],
      "description": "聚合方式"
    },
    "interval": {
      "type": "string",
      "description": "聚合間隔 (e.g., '1m', '1h', '1d')"
    }
  },
  "required": ["factory_id", "asset_id", "data_point", "start_time", "end_time"]
}
```

#### 4.2.4 執行資產命令

**Tool Name**: `execute_asset_command`

**描述**: 執行資產的命令（如啟動、停止、重置）

**輸入 Schema**:
```json
{
  "type": "object",
  "properties": {
    "factory_id": {
      "type": "string",
      "description": "工廠 ID"
    },
    "asset_id": {
      "type": "string",
      "description": "資產 ID"
    },
    "command": {
      "type": "string",
      "enum": ["start", "stop", "reset", "pause", "resume"],
      "description": "命令名稱"
    }
  },
  "required": ["factory_id", "asset_id", "command"]
}
```

#### 4.2.5 分析數據趨勢

**Tool Name**: `analyze_data_trend`

**描述**: 分析資產數據的趨勢

**輸入 Schema**:
```json
{
  "type": "object",
  "properties": {
    "factory_id": {
      "type": "string",
      "description": "工廠 ID"
    },
    "asset_id": {
      "type": "string",
      "description": "資產 ID"
    },
    "data_point": {
      "type": "string",
      "description": "數據點名稱"
    },
    "time_range": {
      "type": "string",
      "enum": ["1h", "24h", "7d", "30d"],
      "description": "時間範圍"
    }
  },
  "required": ["factory_id", "asset_id", "data_point", "time_range"]
}
```

**輸出範例**:
```json
{
  "content": [
    {
      "type": "text",
      "text": "Temperature trend for Conveyor Belt 1 over the past 24 hours:\n- Average: 35.5°C\n- Min: 32.1°C\n- Max: 38.9°C\n- Trend: Increasing by 0.5°C per hour\n- Alert: Temperature is approaching the warning threshold of 40°C."
    }
  ]
}
```

#### 4.2.6 生成報告

**Tool Name**: `generate_report`

**描述**: 生成工廠或資產的報告

**輸入 Schema**:
```json
{
  "type": "object",
  "properties": {
    "factory_id": {
      "type": "string",
      "description": "工廠 ID"
    },
    "report_type": {
      "type": "string",
      "enum": ["daily_summary", "weekly_summary", "asset_health", "anomaly_detection"],
      "description": "報告類型"
    },
    "start_date": {
      "type": "string",
      "format": "date",
      "description": "開始日期"
    },
    "end_date": {
      "type": "string",
      "format": "date",
      "description": "結束日期"
    }
  },
  "required": ["factory_id", "report_type"]
}
```

#### 4.2.7 搜尋資產

**Tool Name**: `search_assets`

**描述**: 根據條件搜尋資產

**輸入 Schema**:
```json
{
  "type": "object",
  "properties": {
    "factory_id": {
      "type": "string",
      "description": "工廠 ID"
    },
    "query": {
      "type": "string",
      "description": "搜尋關鍵字"
    },
    "asset_type": {
      "type": "string",
      "description": "資產類型過濾"
    },
    "location": {
      "type": "string",
      "description": "位置過濾"
    },
    "status": {
      "type": "string",
      "enum": ["running", "stopped", "error", "maintenance"],
      "description": "狀態過濾"
    }
  },
  "required": ["factory_id"]
}
```

### 4.3 Prompts 映射

#### 4.3.1 工廠巡檢

**Prompt Name**: `factory_inspection`

**描述**: 協助進行工廠巡檢

**參數**:
- `factory_id`: 工廠 ID

**提示模板**:
```
You are conducting a factory inspection for {factory_id}. Please:

1. Check the status of all critical assets
2. Identify any assets with abnormal readings
3. Review recent alerts and warnings
4. Provide a summary of the factory's overall health

Use the available tools to gather information and provide a comprehensive inspection report.
```

#### 4.3.2 異常分析

**Prompt Name**: `anomaly_analysis`

**描述**: 分析工廠異常

**參數**:
- `factory_id`: 工廠 ID
- `asset_id`: 資產 ID（可選）
- `time_range`: 時間範圍（可選）

**提示模板**:
```
Analyze anomalies in {factory_id} {asset_id ? "for asset " + asset_id : ""} {time_range ? "over the past " + time_range : ""}.

Please:
1. Identify any unusual patterns or outliers in the data
2. Determine potential root causes
3. Assess the severity and impact
4. Recommend corrective actions

Use the available tools to gather historical data and provide a detailed analysis.
```

#### 4.3.3 效能優化

**Prompt Name**: `performance_optimization`

**描述**: 提供效能優化建議

**參數**:
- `factory_id`: 工廠 ID

**提示模板**:
```
Analyze the performance of {factory_id} and provide optimization recommendations.

Please:
1. Review key performance indicators (KPIs)
2. Identify bottlenecks and inefficiencies
3. Compare current performance with historical baselines
4. Suggest specific optimization strategies

Use the available tools to gather performance data and provide actionable recommendations.
```

#### 4.3.4 預測性維護

**Prompt Name**: `predictive_maintenance`

**描述**: 提供預測性維護建議

**參數**:
- `factory_id`: 工廠 ID
- `asset_id`: 資產 ID（可選）

**提示模板**:
```
Perform predictive maintenance analysis for {factory_id} {asset_id ? "focusing on asset " + asset_id : ""}.

Please:
1. Analyze historical failure patterns
2. Identify assets at risk of failure
3. Estimate time to failure for critical components
4. Recommend maintenance schedules and priorities

Use the available tools to gather historical data and provide a predictive maintenance plan.
```

---

## 5. 實作細節

### 5.1 技術棧選擇

#### 5.1.1 Python 實作（推薦）

**優勢**：
- NDH 現有組件主要使用 Python
- MCP Python SDK 成熟且易用
- 豐富的數據處理和分析庫

**核心依賴**：
```python
# MCP SDK
mcp>=1.0.0

# Web 框架（用於 HTTP+SSE transport）
fastapi>=0.100.0
uvicorn>=0.23.0
sse-starlette>=1.6.0

# 認證和授權
python-jose[cryptography]>=3.3.0
passlib[bcrypt]>=1.7.4

# NDH 現有依賴
# ...
```

**專案結構**：
```
ndh-mcp-server/
├── src/
│   ├── mcp_server/
│   │   ├── __init__.py
│   │   ├── server.py              # MCP Server 主入口
│   │   ├── protocol_handler.py    # JSON-RPC 處理
│   │   ├── adapters/
│   │   │   ├── __init__.py
│   │   │   ├── resources.py       # Resources Adapter
│   │   │   ├── tools.py           # Tools Adapter
│   │   │   └── prompts.py         # Prompts Adapter
│   │   ├── auth/
│   │   │   ├── __init__.py
│   │   │   ├── authentication.py  # 認證邏輯
│   │   │   └── authorization.py   # 授權邏輯
│   │   └── utils/
│   │       ├── __init__.py
│   │       └── logging.py         # 日誌工具
│   └── ndh_api/
│       └── client.py              # NDH API 客戶端
├── tests/
│   ├── test_resources.py
│   ├── test_tools.py
│   └── test_prompts.py
├── config/
│   ├── server_config.yaml
│   └── prompts.yaml
├── pyproject.toml
└── README.md
```

#### 5.1.2 TypeScript 實作（可選）

**優勢**：
- 更好的類型安全
- 與前端整合更容易
- MCP TypeScript SDK 功能完整

**核心依賴**：
```json
{
  "dependencies": {
    "@modelcontextprotocol/sdk": "^1.0.0",
    "express": "^4.18.0",
    "express-sse": "^0.5.0",
    "jsonwebtoken": "^9.0.0"
  }
}
```

### 5.2 Transport 實作

#### 5.2.1 STDIO Transport

**適用場景**：
- 本地開發和測試
- Claude Desktop 等桌面應用整合

**實作**：
```python
import asyncio
from mcp.server.stdio import stdio_server
from mcp_server.server import create_mcp_server

async def main():
    server = create_mcp_server()
    async with stdio_server() as (read_stream, write_stream):
        await server.run(
            read_stream,
            write_stream,
            server.create_initialization_options()
        )

if __name__ == "__main__":
    asyncio.run(main())
```

**啟動命令**：
```bash
python -m ndh_mcp_server.stdio
```

#### 5.2.2 HTTP+SSE Transport

**適用場景**：
- 遠端訪問
- Web 應用整合
- 多用戶並發

**實作**：
```python
from fastapi import FastAPI, Request
from sse_starlette.sse import EventSourceResponse
from mcp_server.server import create_mcp_server

app = FastAPI()
mcp_server = create_mcp_server()

@app.post("/mcp")
async def mcp_endpoint(request: Request):
    """處理 MCP JSON-RPC 請求"""
    body = await request.json()
    response = await mcp_server.handle_request(body)
    return response

@app.get("/mcp/sse")
async def mcp_sse_endpoint(request: Request):
    """處理 MCP SSE 連接"""
    async def event_generator():
        async for event in mcp_server.event_stream():
            yield event
    
    return EventSourceResponse(event_generator())

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8080)
```

**啟動命令**：
```bash
uvicorn ndh_mcp_server.http:app --host 0.0.0.0 --port 8080
```

### 5.3 Resources Adapter 實作

```python
from mcp.server import Server
from mcp.types import Resource, ReadResourceResult

class ResourcesAdapter:
    def __init__(self, ndh_client):
        self.ndh_client = ndh_client
    
    async def list_resources(self) -> list[Resource]:
        """列出所有可用資源"""
        resources = []
        
        # 列出所有工廠
        factories = await self.ndh_client.list_factories()
        
        for factory in factories:
            # 工廠資產列表資源
            resources.append(Resource(
                uri=f"ndh://factory/{factory['id']}/assets",
                name=f"{factory['name']} Assets",
                description=f"List of all assets in {factory['name']}",
                mimeType="application/json"
            ))
            
            # 工廠佈局資源
            resources.append(Resource(
                uri=f"ndh://factory/{factory['id']}/layout",
                name=f"{factory['name']} Layout",
                description=f"FDL layout definition for {factory['name']}",
                mimeType="application/yaml"
            ))
            
            # 列出工廠中的所有資產
            assets = await self.ndh_client.list_assets(factory['id'])
            
            for asset in assets:
                # 資產即時狀態資源
                resources.append(Resource(
                    uri=f"ndh://factory/{factory['id']}/asset/{asset['id']}/status",
                    name=f"{asset['name']} Status",
                    description=f"Real-time status of {asset['name']}",
                    mimeType="application/json"
                ))
                
                # 資產定義資源
                resources.append(Resource(
                    uri=f"ndh://factory/{factory['id']}/asset/{asset['id']}/definition",
                    name=f"{asset['name']} Definition",
                    description=f"IADL definition for {asset['name']}",
                    mimeType="application/yaml"
                ))
        
        return resources
    
    async def read_resource(self, uri: str) -> ReadResourceResult:
        """讀取資源內容"""
        # 解析 URI
        parts = uri.replace("ndh://", "").split("/")
        
        if parts[0] == "factory":
            factory_id = parts[1]
            
            if parts[2] == "assets":
                # 返回工廠資產列表
                assets = await self.ndh_client.list_assets(factory_id)
                return ReadResourceResult(
                    contents=[{
                        "uri": uri,
                        "mimeType": "application/json",
                        "text": json.dumps(assets, indent=2)
                    }]
                )
            
            elif parts[2] == "layout":
                # 返回工廠佈局
                layout = await self.ndh_client.get_factory_layout(factory_id)
                return ReadResourceResult(
                    contents=[{
                        "uri": uri,
                        "mimeType": "application/yaml",
                        "text": layout
                    }]
                )
            
            elif parts[2] == "asset":
                asset_id = parts[3]
                
                if parts[4] == "status":
                    # 返回資產即時狀態
                    status = await self.ndh_client.get_asset_status(factory_id, asset_id)
                    return ReadResourceResult(
                        contents=[{
                            "uri": uri,
                            "mimeType": "application/json",
                            "text": json.dumps(status, indent=2)
                        }]
                    )
                
                elif parts[4] == "definition":
                    # 返回資產定義
                    definition = await self.ndh_client.get_asset_definition(factory_id, asset_id)
                    return ReadResourceResult(
                        contents=[{
                            "uri": uri,
                            "mimeType": "application/yaml",
                            "text": definition
                        }]
                    )
        
        raise ValueError(f"Unknown resource URI: {uri}")
```

### 5.4 Tools Adapter 實作

```python
from mcp.server import Server
from mcp.types import Tool, CallToolResult

class ToolsAdapter:
    def __init__(self, ndh_client):
        self.ndh_client = ndh_client
    
    def list_tools(self) -> list[Tool]:
        """列出所有可用工具"""
        return [
            Tool(
                name="query_asset_status",
                description="Query the real-time status of an asset",
                inputSchema={
                    "type": "object",
                    "properties": {
                        "factory_id": {
                            "type": "string",
                            "description": "Factory ID"
                        },
                        "asset_id": {
                            "type": "string",
                            "description": "Asset ID"
                        }
                    },
                    "required": ["factory_id", "asset_id"]
                }
            ),
            Tool(
                name="set_asset_parameter",
                description="Set a parameter value for an asset",
                inputSchema={
                    "type": "object",
                    "properties": {
                        "factory_id": {
                            "type": "string",
                            "description": "Factory ID"
                        },
                        "asset_id": {
                            "type": "string",
                            "description": "Asset ID"
                        },
                        "parameter": {
                            "type": "string",
                            "description": "Parameter name"
                        },
                        "value": {
                            "type": "number",
                            "description": "Parameter value"
                        }
                    },
                    "required": ["factory_id", "asset_id", "parameter", "value"]
                }
            ),
            Tool(
                name="query_historical_data",
                description="Query historical data for an asset",
                inputSchema={
                    "type": "object",
                    "properties": {
                        "factory_id": {"type": "string"},
                        "asset_id": {"type": "string"},
                        "data_point": {"type": "string"},
                        "start_time": {"type": "string", "format": "date-time"},
                        "end_time": {"type": "string", "format": "date-time"},
                        "aggregation": {
                            "type": "string",
                            "enum": ["raw", "avg", "min", "max", "sum"]
                        },
                        "interval": {"type": "string"}
                    },
                    "required": ["factory_id", "asset_id", "data_point", "start_time", "end_time"]
                }
            ),
            # ... 其他工具
        ]
    
    async def call_tool(self, name: str, arguments: dict) -> CallToolResult:
        """調用工具"""
        if name == "query_asset_status":
            status = await self.ndh_client.get_asset_status(
                arguments["factory_id"],
                arguments["asset_id"]
            )
            
            # 格式化輸出
            text = f"{status['name']} is currently {status['status']}.\n"
            for key, value in status['data_points'].items():
                text += f"{key}: {value}\n"
            
            return CallToolResult(
                content=[{
                    "type": "text",
                    "text": text
                }]
            )
        
        elif name == "set_asset_parameter":
            await self.ndh_client.set_asset_parameter(
                arguments["factory_id"],
                arguments["asset_id"],
                arguments["parameter"],
                arguments["value"]
            )
            
            return CallToolResult(
                content=[{
                    "type": "text",
                    "text": f"Successfully set {arguments['parameter']} to {arguments['value']}"
                }]
            )
        
        elif name == "query_historical_data":
            data = await self.ndh_client.query_historical_data(
                factory_id=arguments["factory_id"],
                asset_id=arguments["asset_id"],
                data_point=arguments["data_point"],
                start_time=arguments["start_time"],
                end_time=arguments["end_time"],
                aggregation=arguments.get("aggregation", "raw"),
                interval=arguments.get("interval")
            )
            
            # 格式化輸出（可以返回 JSON 或文本）
            return CallToolResult(
                content=[{
                    "type": "text",
                    "text": json.dumps(data, indent=2)
                }]
            )
        
        else:
            raise ValueError(f"Unknown tool: {name}")
```

### 5.5 Prompts Adapter 實作

```python
from mcp.server import Server
from mcp.types import Prompt, GetPromptResult, PromptMessage

class PromptsAdapter:
    def __init__(self, prompts_config):
        self.prompts_config = prompts_config
    
    def list_prompts(self) -> list[Prompt]:
        """列出所有可用提示"""
        return [
            Prompt(
                name="factory_inspection",
                description="Assist with factory inspection",
                arguments=[
                    {
                        "name": "factory_id",
                        "description": "Factory ID",
                        "required": True
                    }
                ]
            ),
            Prompt(
                name="anomaly_analysis",
                description="Analyze factory anomalies",
                arguments=[
                    {
                        "name": "factory_id",
                        "description": "Factory ID",
                        "required": True
                    },
                    {
                        "name": "asset_id",
                        "description": "Asset ID (optional)",
                        "required": False
                    },
                    {
                        "name": "time_range",
                        "description": "Time range (optional)",
                        "required": False
                    }
                ]
            ),
            # ... 其他提示
        ]
    
    async def get_prompt(self, name: str, arguments: dict) -> GetPromptResult:
        """獲取提示內容"""
        if name == "factory_inspection":
            factory_id = arguments["factory_id"]
            
            prompt_text = f"""You are conducting a factory inspection for {factory_id}. Please:

1. Check the status of all critical assets
2. Identify any assets with abnormal readings
3. Review recent alerts and warnings
4. Provide a summary of the factory's overall health

Use the available tools to gather information and provide a comprehensive inspection report."""
            
            return GetPromptResult(
                messages=[
                    PromptMessage(
                        role="user",
                        content={
                            "type": "text",
                            "text": prompt_text
                        }
                    )
                ]
            )
        
        elif name == "anomaly_analysis":
            factory_id = arguments["factory_id"]
            asset_id = arguments.get("asset_id")
            time_range = arguments.get("time_range")
            
            prompt_text = f"""Analyze anomalies in {factory_id}"""
            if asset_id:
                prompt_text += f" for asset {asset_id}"
            if time_range:
                prompt_text += f" over the past {time_range}"
            
            prompt_text += """

Please:
1. Identify any unusual patterns or outliers in the data
2. Determine potential root causes
3. Assess the severity and impact
4. Recommend corrective actions

Use the available tools to gather historical data and provide a detailed analysis."""
            
            return GetPromptResult(
                messages=[
                    PromptMessage(
                        role="user",
                        content={
                            "type": "text",
                            "text": prompt_text
                        }
                    )
                ]
            )
        
        else:
            raise ValueError(f"Unknown prompt: {name}")
```

---

## 6. 安全性與授權

### 6.1 認證機制

#### 6.1.1 API Key 認證

**適用場景**：服務間認證

**實作**：
```python
from fastapi import Header, HTTPException

async def verify_api_key(x_api_key: str = Header(...)):
    """驗證 API Key"""
    if x_api_key not in valid_api_keys:
        raise HTTPException(status_code=401, detail="Invalid API Key")
    return x_api_key
```

**配置**：
```yaml
# config/server_config.yaml
authentication:
  api_keys:
    - key: "ndh_mcp_key_12345"
      name: "Claude Desktop"
      permissions: ["viewer"]
    - key: "ndh_mcp_key_67890"
      name: "Admin Tool"
      permissions: ["admin"]
```

#### 6.1.2 OAuth 2.0 認證

**適用場景**：使用者授權

**流程**：
1. 使用者在 AI Host 中發起連接
2. AI Host 重定向到 NDH OAuth 授權頁面
3. 使用者登入並授權
4. NDH 返回 Authorization Code
5. AI Host 交換 Access Token
6. 使用 Access Token 訪問 MCP Server

**實作**：
```python
from fastapi import Depends
from fastapi.security import OAuth2AuthorizationCodeBearer

oauth2_scheme = OAuth2AuthorizationCodeBearer(
    authorizationUrl="https://ndh.example.com/oauth/authorize",
    tokenUrl="https://ndh.example.com/oauth/token"
)

async def get_current_user(token: str = Depends(oauth2_scheme)):
    """獲取當前使用者"""
    user = await verify_token(token)
    if not user:
        raise HTTPException(status_code=401, detail="Invalid token")
    return user
```

### 6.2 授權機制

#### 6.2.1 角色定義

```yaml
# config/rbac.yaml
roles:
  viewer:
    description: "Read-only access"
    permissions:
      - "resources:read"
      - "tools:query_asset_status"
      - "tools:query_historical_data"
      - "tools:search_assets"
      - "tools:analyze_data_trend"
      - "prompts:*"
  
  operator:
    description: "Operational access"
    inherits: ["viewer"]
    permissions:
      - "tools:set_asset_parameter"
      - "tools:execute_asset_command"
  
  admin:
    description: "Full access"
    inherits: ["operator"]
    permissions:
      - "tools:*"
      - "resources:*"
```

#### 6.2.2 權限檢查

```python
class AuthorizationMiddleware:
    def __init__(self, rbac_config):
        self.rbac_config = rbac_config
    
    async def check_permission(self, user, action, resource):
        """檢查使用者權限"""
        user_roles = user.get("roles", [])
        
        for role in user_roles:
            permissions = self.get_role_permissions(role)
            
            # 檢查是否有通配符權限
            if f"{action}:*" in permissions:
                return True
            
            # 檢查是否有具體權限
            if f"{action}:{resource}" in permissions:
                return True
        
        return False
    
    def get_role_permissions(self, role):
        """獲取角色的所有權限（包括繼承）"""
        permissions = set()
        role_config = self.rbac_config["roles"].get(role, {})
        
        # 添加直接權限
        permissions.update(role_config.get("permissions", []))
        
        # 添加繼承的權限
        for inherited_role in role_config.get("inherits", []):
            permissions.update(self.get_role_permissions(inherited_role))
        
        return permissions
```

### 6.3 審計日誌

```python
class AuditLogger:
    def __init__(self, db_client):
        self.db_client = db_client
    
    async def log_operation(self, user, operation, resource, arguments, result, error=None):
        """記錄操作日誌"""
        log_entry = {
            "timestamp": datetime.utcnow(),
            "user_id": user.get("id"),
            "user_name": user.get("name"),
            "operation": operation,
            "resource": resource,
            "arguments": arguments,
            "result": result,
            "error": error,
            "ip_address": user.get("ip_address"),
            "user_agent": user.get("user_agent")
        }
        
        await self.db_client.insert("audit_logs", log_entry)
    
    async def query_audit_logs(self, filters):
        """查詢審計日誌"""
        return await self.db_client.query("audit_logs", filters)
```

**審計日誌範例**：
```json
{
  "timestamp": "2025-10-14T12:00:00Z",
  "user_id": "user-123",
  "user_name": "john.doe@example.com",
  "operation": "tools:call",
  "resource": "set_asset_parameter",
  "arguments": {
    "factory_id": "harvatek-led",
    "asset_id": "asset-001",
    "parameter": "speed",
    "value": 2.0
  },
  "result": "success",
  "error": null,
  "ip_address": "192.168.1.100",
  "user_agent": "Claude Desktop/1.0"
}
```

---

## 7. 部署架構

### 7.1 獨立部署

```
┌─────────────────────────────────────────────────────────────┐
│                        AI Host                               │
│  (Claude Desktop, Manus, ChatGPT, etc.)                     │
└────────────────────────┬────────────────────────────────────┘
                         │ MCP Protocol
                         │ Transport: HTTP+SSE
                         ↓
┌─────────────────────────────────────────────────────────────┐
│                   NDH MCP Server                             │
│  (Standalone Service)                                        │
│  - Port: 8080                                                │
│  - Authentication: OAuth 2.0 / API Key                      │
└────────────────────────┬────────────────────────────────────┘
                         │ HTTP API
                         ↓
┌─────────────────────────────────────────────────────────────┐
│                   NDH Master Node                            │
│  - Asset Management API                                      │
│  - Data Query API                                            │
│  - Command Execution API                                     │
└─────────────────────────────────────────────────────────────┘
```

**部署命令**：
```bash
# Docker Compose
docker-compose up -d ndh-mcp-server

# Kubernetes
kubectl apply -f ndh-mcp-server-deployment.yaml
```

**Docker Compose 配置**：
```yaml
# docker-compose.yml
version: '3.8'

services:
  ndh-mcp-server:
    image: idtf/ndh-mcp-server:latest
    ports:
      - "8080:8080"
    environment:
      - NDH_API_URL=http://ndh-master:3000
      - AUTH_METHOD=oauth2
      - OAUTH_CLIENT_ID=ndh-mcp-server
      - OAUTH_CLIENT_SECRET=${OAUTH_CLIENT_SECRET}
    depends_on:
      - ndh-master
    networks:
      - idtf-network

networks:
  idtf-network:
    external: true
```

### 7.2 嵌入式部署

```
┌─────────────────────────────────────────────────────────────┐
│                   NDH Master Node                            │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  NDH Core Services                                    │  │
│  │  - Asset Management                                   │  │
│  │  - Data Query                                         │  │
│  │  - Command Execution                                  │  │
│  └──────────────────────────────────────────────────────┘  │
│                         ↑                                    │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  NDH MCP Server (Embedded)                            │  │
│  │  - Port: 8080                                         │  │
│  │  - Direct API Access                                  │  │
│  └──────────────────────────────────────────────────────┘  │
└────────────────────────┬────────────────────────────────────┘
                         │ MCP Protocol
                         │ Transport: HTTP+SSE
                         ↓
┌─────────────────────────────────────────────────────────────┐
│                        AI Host                               │
│  (Claude Desktop, Manus, ChatGPT, etc.)                     │
└─────────────────────────────────────────────────────────────┘
```

**優勢**：
- 更低的延遲
- 更簡單的部署
- 更少的網路跳轉

**實作**：
```python
# 在 NDH Master 中嵌入 MCP Server
from ndh.master import NDHMaster
from ndh_mcp_server import create_mcp_server

class NDHMasterWithMCP(NDHMaster):
    def __init__(self, config):
        super().__init__(config)
        
        # 創建 MCP Server
        self.mcp_server = create_mcp_server(ndh_client=self.api_client)
    
    async def start(self):
        # 啟動 NDH Master
        await super().start()
        
        # 啟動 MCP Server
        await self.mcp_server.start()
```

### 7.3 Kubernetes 部署

```yaml
# ndh-mcp-server-deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: ndh-mcp-server
  namespace: idtf
spec:
  replicas: 3
  selector:
    matchLabels:
      app: ndh-mcp-server
  template:
    metadata:
      labels:
        app: ndh-mcp-server
    spec:
      containers:
      - name: ndh-mcp-server
        image: idtf/ndh-mcp-server:latest
        ports:
        - containerPort: 8080
        env:
        - name: NDH_API_URL
          value: "http://ndh-master:3000"
        - name: AUTH_METHOD
          value: "oauth2"
        - name: OAUTH_CLIENT_ID
          valueFrom:
            secretKeyRef:
              name: ndh-mcp-oauth
              key: client-id
        - name: OAUTH_CLIENT_SECRET
          valueFrom:
            secretKeyRef:
              name: ndh-mcp-oauth
              key: client-secret
        resources:
          requests:
            memory: "256Mi"
            cpu: "250m"
          limits:
            memory: "512Mi"
            cpu: "500m"
        livenessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 10
        readinessProbe:
          httpGet:
            path: /ready
            port: 8080
          initialDelaySeconds: 10
          periodSeconds: 5
---
apiVersion: v1
kind: Service
metadata:
  name: ndh-mcp-server
  namespace: idtf
spec:
  selector:
    app: ndh-mcp-server
  ports:
  - protocol: TCP
    port: 80
    targetPort: 8080
  type: LoadBalancer
---
apiVersion: networking.k8s.io/v1
kind: Ingress
metadata:
  name: ndh-mcp-server
  namespace: idtf
  annotations:
    cert-manager.io/cluster-issuer: "letsencrypt-prod"
spec:
  tls:
  - hosts:
    - mcp.ndh.example.com
    secretName: ndh-mcp-tls
  rules:
  - host: mcp.ndh.example.com
    http:
      paths:
      - path: /
        pathType: Prefix
        backend:
          service:
            name: ndh-mcp-server
            port:
              number: 80
```

---

## 8. 使用場景

### 8.1 場景 1：智能工廠巡檢

**使用者**：工廠管理員

**對話範例**：

**使用者**：請幫我巡檢宏齊 LED 工廠，檢查所有關鍵設備的狀態。

**AI 助理**（使用 MCP）：
1. 調用 `search_assets` 工具，搜尋所有關鍵資產
2. 對每個關鍵資產調用 `query_asset_status` 工具
3. 分析結果並生成報告

**AI 助理回應**：
```
我已完成宏齊 LED 工廠的巡檢，以下是報告：

✅ 正常運行的設備 (45/50):
- Conveyor Belt 1-10: 全部正常運行
- LED Mounting Machine 1-15: 全部正常運行
- ...

⚠️ 需要關注的設備 (3/50):
- Conveyor Belt 11: 溫度偏高 (38.5°C，接近警告閾值 40°C)
- LED Testing Machine 3: 振動異常 (0.05 mm/s，超過正常範圍)
- Packaging Machine 2: 速度降低 (1.2 m/s，低於設定值 1.5 m/s)

❌ 故障設備 (2/50):
- Conveyor Belt 12: 已停機，錯誤代碼 E001
- LED Mounting Machine 16: 通訊中斷

建議：
1. 立即檢查 Conveyor Belt 12 的錯誤代碼 E001
2. 檢查 LED Mounting Machine 16 的網路連接
3. 監控 Conveyor Belt 11 的溫度變化
4. 安排 LED Testing Machine 3 的維護檢查
```

### 8.2 場景 2：異常分析與診斷

**使用者**：維護工程師

**對話範例**：

**使用者**：Conveyor Belt 11 的溫度為什麼一直在上升？請分析過去 24 小時的數據。

**AI 助理**（使用 MCP）：
1. 調用 `query_historical_data` 工具，獲取過去 24 小時的溫度數據
2. 調用 `analyze_data_trend` 工具，分析溫度趨勢
3. 調用 `query_historical_data` 工具，獲取相關的其他參數（速度、負載等）
4. 分析相關性並生成診斷報告

**AI 助理回應**：
```
我已分析 Conveyor Belt 11 過去 24 小時的數據，發現以下問題：

📊 溫度趨勢：
- 起始溫度：32.1°C (00:00)
- 當前溫度：38.5°C (12:00)
- 平均上升速率：0.27°C/小時
- 預計 6 小時後將達到警告閾值 40°C

🔍 根本原因分析：
1. **負載增加**：過去 24 小時內，輸送帶的負載從 60% 增加到 85%
2. **速度提升**：速度從 1.3 m/s 提升到 1.6 m/s
3. **環境溫度**：工廠環境溫度從 25°C 上升到 28°C

📈 相關性分析：
- 溫度與負載的相關係數：0.89（強正相關）
- 溫度與速度的相關係數：0.76（中等正相關）
- 溫度與環境溫度的相關係數：0.45（弱正相關）

💡 建議措施：
1. **短期**：降低輸送帶速度至 1.4 m/s，預計可降低溫度 2-3°C
2. **中期**：優化負載分配，將部分負載轉移到其他輸送帶
3. **長期**：考慮升級輸送帶的冷卻系統或增加散熱風扇

⚠️ 預警：
如果不採取措施，預計在 6 小時後溫度將達到 40°C，可能觸發自動停機保護。
```

### 8.3 場景 3：效能優化建議

**使用者**：生產經理

**對話範例**：

**使用者**：我們的 LED 封裝產線效率最近下降了，請幫我分析原因並提供優化建議。

**AI 助理**（使用 MCP）：
1. 使用 `performance_optimization` 提示模板
2. 調用 `query_historical_data` 工具，獲取產線的 KPI 數據
3. 調用 `search_assets` 工具，找出產線中的瓶頸設備
4. 調用 `analyze_data_trend` 工具，分析效率下降的趨勢
5. 生成優化建議

**AI 助理回應**：
```
我已完成 LED 封裝產線的效能分析，以下是報告：

📊 關鍵效能指標 (KPI)：
- 當前產能：850 units/hour（目標：1000 units/hour）
- 設備綜合效率 (OEE)：72%（目標：85%）
- 停機時間：12%（目標：< 5%）
- 良品率：96%（目標：98%）

🔍 瓶頸分析：
1. **LED Mounting Machine 5**：
   - 平均週期時間：4.5 秒（其他機器：3.8 秒）
   - 停機次數：15 次/天（其他機器：5 次/天）
   - 判斷：主要瓶頸

2. **LED Testing Machine 7**：
   - 測試時間：2.8 秒（其他機器：2.2 秒）
   - 誤判率：3%（其他機器：1%）
   - 判斷：次要瓶頸

3. **Packaging Machine 3**：
   - 速度：1.2 m/s（其他機器：1.5 m/s）
   - 判斷：輕微瓶頸

📈 趨勢分析：
- 效率下降始於 2 週前
- 下降速率：2% per week
- 主要原因：LED Mounting Machine 5 的停機次數增加

💡 優化建議：

**立即措施**（預計提升 5-8%）：
1. 對 LED Mounting Machine 5 進行深度維護和校準
2. 更換 LED Testing Machine 7 的測試探頭
3. 將 Packaging Machine 3 的速度提升至 1.5 m/s

**短期措施**（預計提升 10-15%）：
1. 優化產線平衡，將部分負載從 LED Mounting Machine 5 轉移
2. 實施預測性維護，減少計劃外停機
3. 培訓操作員，減少人為錯誤

**長期措施**（預計提升 20-25%）：
1. 考慮更換或升級 LED Mounting Machine 5
2. 引入自動化檢測系統，取代 LED Testing Machine 7
3. 實施 AI 驅動的產線調度優化

💰 投資回報分析：
- 立即措施成本：$5,000，預計年收益：$50,000
- 短期措施成本：$20,000，預計年收益：$150,000
- 長期措施成本：$200,000，預計年收益：$500,000

建議優先實施立即措施和短期措施，長期措施可在下一財年預算中考慮。
```

### 8.4 場景 4：預測性維護

**使用者**：維護主管

**對話範例**：

**使用者**：請分析我們工廠中哪些設備需要進行預測性維護，並給出優先級。

**AI 助理**（使用 MCP）：
1. 使用 `predictive_maintenance` 提示模板
2. 調用 `search_assets` 工具，列出所有設備
3. 對每個設備調用 `query_historical_data` 工具，獲取關鍵參數的歷史數據
4. 調用 `analyze_data_trend` 工具，分析故障預兆
5. 生成預測性維護計畫

**AI 助理回應**：
```
我已完成預測性維護分析，以下是優先級排序的維護建議：

🔴 高優先級（建議 1 週內維護）：

1. **LED Mounting Machine 5**
   - 故障風險：85%
   - 預計故障時間：7-10 天
   - 關鍵指標：
     * 振動增加 40%（過去 2 週）
     * 溫度波動異常
     * 停機次數增加 3 倍
   - 建議措施：
     * 更換軸承
     * 檢查並潤滑傳動系統
     * 校準定位系統
   - 預計維護成本：$3,000
   - 預計停機成本（如不維護）：$15,000

2. **Conveyor Belt 12**
   - 故障風險：75%
   - 預計故障時間：10-14 天
   - 關鍵指標：
     * 電機電流異常增加
     * 速度不穩定
     * 異常噪音
   - 建議措施：
     * 檢查電機和驅動器
     * 更換皮帶
     * 調整張力
   - 預計維護成本：$2,000
   - 預計停機成本（如不維護）：$10,000

🟡 中優先級（建議 2-4 週內維護）：

3. **LED Testing Machine 7**
   - 故障風險：55%
   - 預計故障時間：20-30 天
   - 關鍵指標：
     * 測試準確度下降
     * 誤判率增加
   - 建議措施：
     * 校準測試設備
     * 更換老化的感測器
   - 預計維護成本：$1,500

4. **Packaging Machine 3**
   - 故障風險：45%
   - 預計故障時間：30-45 天
   - 關鍵指標：
     * 速度逐漸降低
     * 偶發性卡料
   - 建議措施：
     * 清潔和潤滑
     * 更換磨損部件
   - 預計維護成本：$1,000

🟢 低優先級（建議 1-2 個月內維護）：

5-10. 其他設備...

📅 建議維護排程：

**本週**：
- LED Mounting Machine 5（週三，預計 4 小時）

**下週**：
- Conveyor Belt 12（週二，預計 3 小時）

**第 3 週**：
- LED Testing Machine 7（週四，預計 2 小時）

**第 4 週**：
- Packaging Machine 3（週一，預計 2 小時）

💰 總成本與收益：
- 預測性維護總成本：$7,500
- 避免的停機成本：$25,000+
- 淨收益：$17,500+
- ROI：233%

建議立即安排高優先級設備的維護，以避免計劃外停機和生產損失。
```

### 8.5 場景 5：自然語言查詢

**使用者**：新員工

**對話範例**：

**使用者**：我是新來的，請告訴我工廠裡有哪些輸送帶，它們現在的狀態如何？

**AI 助理**（使用 MCP）：
1. 調用 `search_assets` 工具，搜尋類型為 "conveyor" 的資產
2. 對每個輸送帶調用 `query_asset_status` 工具
3. 以友好的方式呈現結果

**AI 助理回應**：
```
歡迎加入！我們工廠共有 15 條輸送帶，讓我為您介紹它們的當前狀態：

✅ **正常運行的輸送帶 (12條)**：

**1樓 A區**：
- Conveyor Belt 1: 正常運行，速度 1.5 m/s，溫度 35°C
- Conveyor Belt 2: 正常運行，速度 1.5 m/s，溫度 34°C
- Conveyor Belt 3: 正常運行，速度 1.6 m/s，溫度 36°C

**1樓 B區**：
- Conveyor Belt 4-6: 全部正常運行

**2樓**：
- Conveyor Belt 7-10: 全部正常運行

⚠️ **需要關注的輸送帶 (2條)**：

- **Conveyor Belt 11** (1樓 C區): 
  * 狀態：運行中
  * 問題：溫度偏高 (38.5°C)
  * 說明：接近警告閾值，已安排監控

- **Conveyor Belt 13** (2樓): 
  * 狀態：運行中
  * 問題：速度略低 (1.3 m/s，設定值 1.5 m/s)
  * 說明：已通知維護團隊檢查

❌ **停機的輸送帶 (1條)**：

- **Conveyor Belt 12** (1樓 C區): 
  * 狀態：已停機
  * 原因：錯誤代碼 E001（電機過載）
  * 說明：維護團隊正在處理，預計 2 小時內恢復

如果您需要了解某條輸送帶的更多詳細資訊，請隨時告訴我！
```

---

## 9. 實作路線圖

### 9.1 階段 1：基礎架構（1-2 個月）

**目標**：建立 MCP Server 的基礎架構

**任務**：
- [ ] 設計 MCP Server 架構
- [ ] 實作 MCP Protocol Handler
- [ ] 實作 STDIO Transport
- [ ] 實作 HTTP+SSE Transport
- [ ] 建立基本的認證機制（API Key）
- [ ] 建立基本的授權機制（RBAC）
- [ ] 建立審計日誌系統

**交付物**：
- MCP Server 基礎框架
- 認證和授權系統
- 審計日誌系統

### 9.2 階段 2：Resources 實作（1 個月）

**目標**：實作 Resources 功能

**任務**：
- [ ] 實作 Resources Adapter
- [ ] 映射工廠資產列表資源
- [ ] 映射資產即時狀態資源
- [ ] 映射資產歷史數據資源
- [ ] 映射工廠佈局資源
- [ ] 映射資產定義資源
- [ ] 實作資源訂閱和更新通知

**交付物**：
- 完整的 Resources 功能
- Resources API 文檔

### 9.3 階段 3：Tools 實作（2 個月）

**目標**：實作 Tools 功能

**任務**：
- [ ] 實作 Tools Adapter
- [ ] 實作 `query_asset_status` 工具
- [ ] 實作 `set_asset_parameter` 工具
- [ ] 實作 `query_historical_data` 工具
- [ ] 實作 `execute_asset_command` 工具
- [ ] 實作 `analyze_data_trend` 工具
- [ ] 實作 `generate_report` 工具
- [ ] 實作 `search_assets` 工具
- [ ] 自動生成工具描述（JSON Schema）

**交付物**：
- 完整的 Tools 功能
- Tools API 文檔

### 9.4 階段 4：Prompts 實作（1 個月）

**目標**：實作 Prompts 功能

**任務**：
- [ ] 實作 Prompts Adapter
- [ ] 設計並實作 `factory_inspection` 提示
- [ ] 設計並實作 `anomaly_analysis` 提示
- [ ] 設計並實作 `performance_optimization` 提示
- [ ] 設計並實作 `predictive_maintenance` 提示
- [ ] 支援多語言提示

**交付物**：
- 完整的 Prompts 功能
- Prompts 庫

### 9.5 階段 5：測試與優化（1 個月）

**目標**：全面測試和優化

**任務**：
- [ ] 單元測試（覆蓋率 > 80%）
- [ ] 整合測試
- [ ] 效能測試和優化
- [ ] 安全性測試
- [ ] 與 Claude Desktop 整合測試
- [ ] 與 Manus 整合測試
- [ ] 文檔完善

**交付物**：
- 測試報告
- 效能基準
- 完整文檔

### 9.6 階段 6：部署與發布（1 個月）

**目標**：部署到生產環境

**任務**：
- [ ] 準備 Docker 映像
- [ ] 準備 Kubernetes 部署配置
- [ ] 建立 CI/CD 流程
- [ ] 生產環境部署
- [ ] 監控和告警配置
- [ ] 使用者培訓
- [ ] 正式發布

**交付物**：
- 生產就緒的 MCP Server
- 部署文檔
- 使用者手冊

---

## 10. 附錄

### 10.1 MCP 協議參考

**官方文檔**：
- 規格：https://modelcontextprotocol.io/specification/latest
- Python SDK：https://github.com/modelcontextprotocol/python-sdk
- TypeScript SDK：https://github.com/modelcontextprotocol/typescript-sdk

**關鍵概念**：
- JSON-RPC 2.0
- Resources
- Tools
- Prompts
- Sampling
- Roots

### 10.2 NDH API 參考

**核心 API**：
- Asset Management API
- Data Query API
- Command Execution API
- Analytics API

**認證**：
- API Key
- OAuth 2.0
- JWT

### 10.3 範例程式碼

完整的範例程式碼請參考：
- GitHub: https://github.com/idtf/ndh-mcp-server
- 文檔: https://docs.idtf.io/ndh-mcp-server

### 10.4 常見問題 (FAQ)

**Q1: MCP Server 會影響 NDH 的效能嗎？**

A1: 不會。MCP Server 是一個獨立的服務層，通過 NDH 的現有 API 進行通訊。它不會修改 NDH 的核心邏輯，也不會增加額外的負載。

**Q2: 如何確保 MCP Server 的安全性？**

A2: MCP Server 實作了多層安全機制：
- 認證：API Key / OAuth 2.0
- 授權：RBAC
- 審計：完整的操作日誌
- 加密：HTTPS / TLS

**Q3: MCP Server 支援哪些 AI 平台？**

A3: MCP Server 遵循標準的 MCP 協議，理論上支援所有實作了 MCP 的 AI 平台，包括：
- Claude Desktop
- Manus
- ChatGPT (未來)
- 其他 MCP 相容平台

**Q4: 如何擴展 MCP Server 的功能？**

A4: MCP Server 的設計是高度模組化的，您可以：
- 添加新的 Resources
- 添加新的 Tools
- 添加新的 Prompts
- 自定義認證和授權邏輯

**Q5: MCP Server 的效能如何？**

A5: MCP Server 的效能取決於 NDH 的效能。在典型的工業場景中：
- 查詢延遲：< 100ms
- 命令執行延遲：< 500ms
- 並發連接：> 1000

---

## 總結

將 NDH 整合為 MCP Server 是一個戰略性的決策，它將：

1. **提升使用者體驗**：使用者可以使用自然語言與數位分身系統互動
2. **降低使用門檻**：無需學習複雜的 API 和介面
3. **增強智能化**：AI 助理可以提供智能分析和建議
4. **擴展應用場景**：開啟新的使用場景和商業模式
5. **增強生態系統**：與 AI 生態系統深度整合

這個設計文件提供了完整的架構、實作細節和路線圖，為 NDH MCP Server 的開發提供了清晰的指引。

---

**文件版本**: 1.0  
**最後更新**: 2025年10月14日  
**作者**: IDTF 技術團隊  
**聯絡**: tech@idtf.io

