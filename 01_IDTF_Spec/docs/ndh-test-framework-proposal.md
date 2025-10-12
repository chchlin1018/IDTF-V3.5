# Pure Python NDH 可運行測試架構技術建議書

**文件版本**: 1.0  
**發布日期**: 2025-10-12  
**作者**: Manus AI  
**參考資料**: [1, 2, 3]

---

## 1. 緒論

### 1.1 背景

**NDH (Neutral Data Hub)** 是一個功能強大的開源工業資料整合中間件平台，其核心價值在於提供中立、靈活且低成本的資料交換能力 [1]。隨著 NDH 功能的擴展和應用的深入，建立一個獨立、輕量且易於使用的測試框架變得至關重要。現有的測試雖然包含在 `NDH-Win-Test1` 專案中，但與主體程式碼耦合較緊，且可能需要完整的開發環境 [3]。

### 1.2 目的與範疇

本建議書旨在設計一個**獨立的、純 Python 的 NDH 可運行測試架構**。此框架的主要目標是：

- **獨立性**: 作為一個獨立的專案存在，可以對任何遠端的 NDH 服務實例進行黑箱測試，無需存取其原始碼。
- **純 Python**: 僅依賴標準 Python 函式庫和可透過 `pip` 安裝的套件，無需安裝如 Docker、Kafka、Redis 等重量級依賴。
- **易用性**: 提供清晰的結構、簡單的配置和一鍵運行的能力，讓開發者、測試人員甚至技術支援工程師都能快速上手。
- **可擴展性**: 具備良好的擴展能力，可以輕鬆添加新的測試案例和功能模組。

本建議書將詳細闡述此測試框架的設計目標、系統架構、核心組件、實施細節及測試場景。

---

## 2. 設計目標與原則

本測試框架的設計遵循以下核心目標與原則：

| 設計目標 | 說明 |
|---|---|
| **黑箱測試** | 框架將 NDH 視為一個黑箱，僅透過其公開的 API 和 WebSocket 介面進行互動和驗證，確保測試的客觀性和通用性。 |
| **零依賴部署** | 測試框架本身不應需要一個完整的 NDH 運行環境。它應該能夠在任何安裝了 Python 的機器上運行，遠端連接到一個正在運行的 NDH 服務。 |
| **配置驅動** | 所有的環境特定參數（如 NDH 主機地址、端口）都應透過外部設定檔進行管理，而非硬編碼在程式碼中。 |
| **全面的測試覆蓋** | 測試場景應涵蓋 NDH 的核心功能，包括 RESTful API、WebSocket 訂閱、連接器互動以及端到端的資料流。 |
| **清晰的報告** | 測試結果應以標準化的格式（如 `pytest` 報告）呈現，清晰地展示成功、失敗的案例及詳細的錯誤訊息。 |

---

## 3. 建議架構

### 3.1 專案結構

建議採用以下模組化的專案結構，以確保程式碼的清晰度和可維護性：

```
ndh-test-framework/
├── tests/                      # 測試案例目錄
│   ├── __init__.py
│   ├── smoke/                  # 煙霧測試 (基礎連通性)
│   │   └── test_health_check.py
│   ├── functional/             # 功能測試
│   │   ├── test_api_data.py    # 資料 API 測試
│   │   └── test_websockets.py  # WebSocket 測試
│   └── integration/            # 整合測試
│       └── test_data_flow.py   # 端到端資料流測試
├── client/                     # NDH 客戶端
│   ├── __init__.py
│   └── ndh_client.py           # 封裝 API 和 WebSocket 互動
├── config/                     # 設定目錄
│   └── test_config.yaml        # 測試設定檔
├── fixtures/                   # 測試資料與輔助檔案
│   └── sample_iadl.json        # 用於測試的 IADL 檔案
├── reports/                    # 測試報告輸出目錄
├── requirements.txt            # Python 依賴
└── pytest.ini                  # Pytest 設定檔
```

### 3.2 核心組件

#### 3.2.1 NDH 客戶端 (`client/ndh_client.py`)

這是測試框架的核心，它是一個類別，封裝了與 NDH 服務進行互動的所有底層邏輯。這種設計將通訊細節與測試邏輯分離，使得測試案例更簡潔、更易讀。

**主要職責**:
- 發送 HTTP 請求到 NDH 的 RESTful API 端點。
- 建立和管理 WebSocket 連接，處理訂閱和訊息接收。
- 處理認證、錯誤和重試邏輯。

**建議方法**:
```python
import httpx
import websockets

class NDHClient:
    def __init__(self, base_url: str, ws_url: str):
        self.http_client = httpx.AsyncClient(base_url=base_url)
        self.ws_url = ws_url

    async def get_health(self) -> dict:
        # 呼叫 GET /health
        pass

    async def push_data(self, payload: dict) -> dict:
        # 呼叫 POST /api/v1/data/push
        pass

    async def query_data(self, query: dict) -> dict:
        # 呼叫 POST /api/v1/data/query
        pass

    async def subscribe_to_topics(self, topics: list, duration: int):
        # 建立 WebSocket 連接並訂閱主題
        # 在指定時間內接收訊息並返回
        pass
```

#### 3.2.2 測試執行器 (`pytest`)

`pytest` 是一個成熟、功能強大且易於擴展的 Python 測試框架。它支援：

- **Fixtures**: 用於管理測試資源和狀態（例如，初始化 `NDHClient`）。
- **參數化**: 輕鬆地對同一測試使用不同的輸入資料運行。
- **豐富的斷言**: 提供詳細的錯誤報告。
- **插件生態**: 大量的插件支援，如 `pytest-asyncio` 用於測試非同步程式碼，`pytest-html` 用於生成 HTML 報告。

#### 3.2.3 設定管理器 (`config/test_config.yaml`)

使用 YAML 格式的設定檔來管理所有可變的環境參數。

**範例設定**:
```yaml
ndh_service:
  host: "localhost"
  port: 8000
  protocol: "http"

test_parameters:
  default_timeout: 10
  websocket_subscription_duration: 5 # seconds
```

---

## 4. 實施細節

### 4.1 依賴套件

`requirements.txt` 檔案將包含所有必要的 Python 套件。

```
# requirements.txt

# 測試框架
pytest
pytest-asyncio  # 支援非同步測試
pytest-html     # 產生 HTML 報告

# HTTP & WebSocket 客戶端
httpx           # 現代化的非同步 HTTP 客戶端
websockets      # WebSocket 客戶端

# 設定與資料處理
pyyaml          # 讀取 YAML 設定檔
```

### 4.2 快速開始指南

1.  **安裝依賴**: `pip install -r requirements.txt`
2.  **配置環境**: 修改 `config/test_config.yaml`，填寫目標 NDH 服務的地址。
3.  **執行測試**: 在專案根目錄下運行 `pytest` 命令。
4.  **查看報告**: 在 `reports/` 目錄下查看生成的 `report.html`。

### 4.3 測試案例範例

#### 煙霧測試 (`tests/smoke/test_health_check.py`)

```python
import pytest
from client.ndh_client import NDHClient

@pytest.mark.asyncio
async def test_health_check(ndh_client: NDHClient):
    """測試 NDH 服務是否正在運行並可訪問"""
    response = await ndh_client.get_health()
    assert response["status"] == "running"
    assert response["service"] == "NDH - Neutral Data Hub"
```

#### 功能測試 (`tests/functional/test_api_data.py`)

```python
import pytest
from client.ndh_client import NDHClient
from datetime import datetime

@pytest.mark.asyncio
async def test_push_and_query_data(ndh_client: NDHClient):
    """測試資料推送和查詢功能"""
    asset_id = f"TEST_ASSET_{datetime.now().timestamp()}"
    payload = {
        "asset_id": asset_id,
        "values": {"temperature": 25.5}
    }

    # 1. 推送資料
    push_response = await ndh_client.push_data(payload)
    assert push_response["status"] == "success"

    # 2. 查詢資料
    query_response = await ndh_client.query_data({"asset_id": asset_id})
    assert len(query_response["data"]) == 1
    assert query_response["data"][0]["values"]["temperature"] == 25.5
```

#### 整合測試 (`tests/integration/test_data_flow.py`)

此測試驗證一個端到端的資料流，例如透過 API 推送資料，然後透過 WebSocket 接收到該資料的通知。

```python
import pytest
import asyncio
from client.ndh_client import NDHClient

@pytest.mark.asyncio
async def test_api_to_websocket_flow(ndh_client: NDHClient):
    """測試從 API 推送到 WebSocket 接收的完整資料流"""
    asset_id = "FLOW_TEST_ASSET_01"
    topic = "temperature_alerts"

    # 1. 啟動 WebSocket 訂閱
    # subscribe_to_topics 是一個非同步任務，會在背景監聽
    ws_task = asyncio.create_task(
        ndh_client.subscribe_to_topics([topic], duration=5)
    )

    # 給予 WebSocket 連接和訂閱足夠的時間
    await asyncio.sleep(1)

    # 2. 透過 API 推送觸發事件的資料
    payload = {
        "asset_id": asset_id,
        "topic": topic,
        "values": {"temperature": 99.9}
    }
    await ndh_client.push_data(payload)

    # 3. 等待 WebSocket 任務完成並獲取結果
    received_messages = await ws_task

    # 4. 驗證結果
    assert len(received_messages) > 0
    found = any(msg["asset_id"] == asset_id and msg["values"]["temperature"] == 99.9 for msg in received_messages)
    assert found, "未在 WebSocket 上收到預期的訊息"
```

---

## 5. 結論與效益

本建議書提出了一個獨立、輕量且功能全面的 **Pure Python NDH 測試框架**。採用此框架將帶來以下顯著效益：

- **提升軟體品質**: 透過自動化、可重複的測試，確保 NDH 服務的穩定性和可靠性。
- **加速開發迭代**: 開發人員可以快速驗證其變更是否影響現有功能，實現快速的持續整合 (CI)。
- **降低測試門檻**: 無需複雜的環境設定，讓更多團隊成員（如 QA、技術支援）能夠參與到測試過程中。
- **改善文件和範例**: 測試案例本身就是 NDH API 最好的使用範例和動態文件。

我們相信，投資建設這樣一個測試框架，將為 NDH 專案的長期健康發展和社群的壯大提供堅實的基礎。

---

## 6. 參考資料

[1] Lin, C. C. (2025). *idtf-v3: IDTF V3.1 完整技術白皮書*. GitHub. [https://github.com/chchlin1018/idtf-v3](https://github.com/chchlin1018/idtf-v3)

[2] Lin, C. C. (2025). *idtf-documentation: IDTF 完整文件、程式碼、範例和視覺化*. GitHub. [https://github.com/chchlin1018/idtf-documentation](https://github.com/chchlin1018/idtf-documentation)

[3] Lin, C. C. (2025). *NDH-Win-Test1: NDH 測試開發框架*. GitHub. [https://github.com/chchlin1018/NDH-Win-Test1](https://github.com/chchlin1018/NDH-Win-Test1)

