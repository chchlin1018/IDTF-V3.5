# NDH 與 NVIDIA Omniverse 整合指南

**版本**: 1.0.0
**作者**: Manus AI
**日期**: 2025-10-10

---

## 目錄

1.  [簡介](#1-簡介)
    *   [1.1 目的](#11-目的)
    *   [1.2 主要功能](#12-主要功能)
    *   [1.3 技術棧](#13-技術棧)
2.  [環境準備](#2-環境準備)
    *   [2.1 硬體要求](#21-硬體要求)
    *   [2.2 軟體要求](#22-軟體要求)
3.  [安裝與配置](#3-安裝與配置)
    *   [3.1 安裝 NDH](#31-安裝-ndh)
    *   [3.2 安裝 Omniverse Connector](#32-安裝-omniverse-connector)
    *   [3.3 配置 Omniverse Connector](#33-配置-omniverse-connector)
4.  [快速開始](#4-快速開始)
    *   [4.1 運行基本同步範例](#41-運行基本同步範例)
    *   [4.2 驗證結果](#42-驗證結果)
5.  [建立您的第一個數位分身](#5-建立您的第一個數位分身)
    *   [5.1 準備 3D 模型](#51-準備-3d-模型)
    *   [5.2 建立 USD 場景](#52-建立-usd-場景)
    *   [5.3 運行數位分身工廠範例](#53-運行數位分身工廠範例)
6.  [進階功能](#6-進階功能)
    *   [6.1 歷史資料回放](#61-歷史資料回放)
    *   [6.2 多人協作](#62-多人協作)
    *   [6.3 資料過濾與優化](#63-資料過濾與優化)
7.  [API 參考](#7-api-參考)
    *   [7.1 OmniverseConnector](#71-omniverseconnector)
    *   [7.2 OmniverseConnectorConfig](#72-omniverseconnectorconfig)
8.  [故障排除](#8-故障排除)
9.  [參考資料](#9-參考資料)

---

## 1. 簡介

### 1.1 目的

本指南旨在幫助開發者和工程師將 **NDH (Neutral Data Hub)** 與 **NVIDIA Omniverse** 平台整合,建立一個即時運行、多人協作的工業數位分身 (Industrial Digital Twins) 平台。通過本指南,您將學會如何:

*   安裝和配置 NDH Omniverse Connector
*   將來自工業現場的即時資料同步到 Omniverse
*   在 Omniverse 中建立和視覺化您的數位分身
*   利用多人協作功能與團隊成員共享和互動

### 1.2 主要功能

*   **即時資料同步**: 將來自 Kafka 的 IADL 資料即時轉換為 OpenUSD 屬性更新。
*   **場景自動創建**: 如果 USD Prim 不存在,連接器會自動創建。
*   **狀態視覺化**: 根據資產狀態 (如告警、運行) 自動更新 3D 模型的顏色。
*   **高效能**: 支援批次更新和資料過濾,減少網路負載和渲染開銷。
*   **可擴展**: 易於擴展以支援自定義的資料映射和視覺化邏輯。

### 1.3 技術棧

| 組件 | 技術 |
|---|---|
| 資料中樞 | NDH (Neutral Data Hub) |
| 事件流 | Apache Kafka |
| 資料標準 | IADL (Industrial Asset Data Language) |
| 3D 協作平台 | NVIDIA Omniverse |
| 場景描述 | OpenUSD (Universal Scene Description) |
| 連接器語言 | Python |

---

## 2. 環境準備

### 2.1 硬體要求

*   **開發機**: 任何支援 Python 的現代電腦。
*   **Omniverse Nucleus/App**: NVIDIA RTX™ 顯示卡 (建議 RTX 3070 或更高)。

### 2.2 軟體要求

1.  **NDH 環境**:
    *   已安裝並運行的 NDH 實例。
    *   已安裝並運行的 Kafka 叢集。

2.  **NVIDIA Omniverse 環境**:
    *   **Omniverse Launcher**: 從 [NVIDIA 官網](https://www.nvidia.com/en-us/omniverse/download/) 下載並安裝。
    *   **Nucleus**: 安裝並運行 Omniverse Nucleus (用於資料庫和協作)。
    *   **Omniverse App**: 安裝至少一個 Omniverse 應用程式,例如 `Omniverse View` 或 `Omniverse Code`。

3.  **Python 環境**:
    *   Python 3.8 或更高版本。
    *   `pip` 套件管理器。

---

## 3. 安裝與配置

### 3.1 安裝 NDH

請參考 NDH 的主文件進行安裝和配置。

### 3.2 安裝 Omniverse Connector

1.  **克隆 NDH 倉庫**:

    ```bash
    git clone https://github.com/chchlin1018/NDH-Win-Test1.git
    cd NDH-Win-Test1
    ```

2.  **安裝 Python 依賴**:

    ```bash
    pip install -r requirements.txt
    ```

    **注意**: `usd-core` 和 `omni.client` 可能需要手動安裝。

    *   **OpenUSD**: `pip install usd-core`
    *   **Omniverse Client Library**: 請參考 Omniverse 開發者文件進行安裝。

### 3.3 配置 Omniverse Connector

連接器的配置在範例程式碼中完成,例如 `examples/omniverse_integration/basic_omniverse_sync.py`:

```python
config = OmniverseConnectorConfig(
    # Kafka 配置
    kafka_bootstrap_servers="localhost:9092",
    kafka_topics=["asset.telemetry", "asset.events"],
    kafka_group_id="omniverse-connector-example",
    
    # Nucleus 配置
    nucleus_url="omniverse://localhost",
    
    # Stage 配置
    stage_url="omniverse://localhost/Projects/Factory/DigitalTwin.usd",
)
```

**主要配置參數**:

*   `kafka_bootstrap_servers`: Kafka 服務器地址。
*   `kafka_topics`: 要訂閱的 Kafka 主題列表。
*   `nucleus_url`: Omniverse Nucleus 服務器地址。
*   `stage_url`: 要同步的 USD Stage 的 URL。

---

## 4. 快速開始

### 4.1 運行基本同步範例

1.  **啟動 Omniverse**: 確保您的 Nucleus 和一個 Omniverse App (如 View) 正在運行。

2.  **發送測試資料到 Kafka**: 使用任何 Kafka 工具發送一條 IADL 格式的訊息到 `asset.telemetry` 主題:

    ```json
    {
      "asset_id": "/Factory/Line1/CNC_01",
      "timestamp": "2025-10-10T10:00:00Z",
      "properties": {
        "temperature": 45.5,
        "speed": 1200
      },
      "state": "RUNNING"
    }
    ```

3.  **運行範例程式碼**:

    ```bash
    python examples/omniverse_integration/basic_omniverse_sync.py
    ```

### 4.2 驗證結果

1.  **在 Omniverse View 中打開 Stage**:
    *   導航到 `omniverse://localhost/Projects/Factory/`。
    *   打開 `DigitalTwin.usd` 文件。

2.  **檢查 Prim 屬性**:
    *   在 Stage 視窗中,找到 `/World/Factory/Line1/CNC_01`。
    *   在屬性 (Property) 面板中,您應該能看到 `custom:temperature` 和 `custom:speed` 屬性,其值已更新。
    *   模型的顏色應該變為綠色,表示 `RUNNING` 狀態。

---

## 5. 建立您的第一個數位分身

### 5.1 準備 3D 模型

*   準備好您的工業設備的 3D 模型 (格式: USD, FBX, OBJ 等)。
*   將模型上傳到 Nucleus 的一個目錄中,例如 `omniverse://localhost/Assets/Models/`。

### 5.2 建立 USD 場景

使用 `digital_twins_factory.py` 範例來自動創建場景結構:

```python
class DigitalTwinsFactory:
    # ...
    async def _create_factory_scene(self):
        # ...
        # 引用 3D 模型
        cnc01_prim = stage.GetPrimAtPath("/World/Factory/Line1/CNC_01")
        cnc01_prim.GetReferences().AddReference("omniverse://localhost/Assets/Models/cnc_machine.usd")
        # ...
```

### 5.3 運行數位分身工廠範例

```bash
python examples/omniverse_integration/digital_twins_factory.py
```

這個範例會:
1.  在 USD 中創建一個工廠場景。
2.  為機器創建 Prim 和自定義屬性。
3.  開始監聽 Kafka 並將資料同步到對應的機器 Prim。

---

## 6. 進階功能

### 6.1 歷史資料回放

(待實作)

### 6.2 多人協作

Omniverse 的 Live Sync 功能是預設啟用的。當多個使用者同時打開同一個 USD Stage 時,任何由 NDH Connector 寫入的更新都會即時同步到所有用戶的視圖中。

### 6.3 資料過濾與優化

在 `OmniverseConnectorConfig` 中啟用資料過濾可以顯著提高效能:

```python
config = OmniverseConnectorConfig(
    # ...
    enable_filtering=True,
    min_update_interval=0.1,  # 每個屬性每 100ms 最多更新一次
    value_change_threshold=0.01  # 只有當值變化超過 1% 時才更新
)
```

---

## 7. API 參考

### 7.1 OmniverseConnector

`ndh.connectors.omniverse.OmniverseConnector`

**主要方法**:

*   `__init__(config: OmniverseConnectorConfig)`: 初始化連接器。
*   `async connect()`: 連接到 Kafka 和 Nucleus。
*   `async start()`: 開始監聽和處理事件。
*   `async disconnect()`: 斷開連接。
*   `get_stats() -> Dict[str, int]`: 獲取統計資訊。

### 7.2 OmniverseConnectorConfig

`ndh.connectors.omniverse.OmniverseConnectorConfig`

一個 dataclass,包含所有配置參數。詳情請參考原始碼。

---

## 8. 故障排除

| 問題 | 可能原因 | 解決方案 |
|---|---|---|
| 連接 Nucleus 失敗 | Nucleus 未運行或網路問題 | 確保 Nucleus 正在運行,並檢查防火牆設置。 |
| 無法訂閱 Kafka 主題 | Kafka 未運行或主題不存在 | 確保 Kafka 正在運行,並已創建所需的主題。 |
| USD Stage 中沒有更新 | 資料格式錯誤或映射問題 | 檢查 Kafka 中的 IADL 資料格式是否正確,並檢查日誌中是否有轉換錯誤。 |
| Omniverse App 崩潰 | 驅動程式或版本不相容 | 更新 NVIDIA 顯示卡驅動程式和 Omniverse App 到最新版本。 |

---

## 9. 參考資料

*   [NVIDIA Omniverse 文件](https://docs.omniverse.nvidia.com/)
*   [OpenUSD 文件](https://openusd.org/release/index.html)
*   [Apache Kafka 文件](https://kafka.apache.org/documentation/)
*   [NDH GitHub 倉庫](https://github.com/chchlin1018/NDH-Win-Test1)

