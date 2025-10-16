# 事件規範 (Event Specification)

## 1. 概述

本文件定義了 IDTF (Industrial Digital Twin Framework) 中事件的結構、傳輸、一致性策略和時間規範。事件是 NDH (Neutral Data Hub) 內部以及與外部系統之間數據流和狀態變化的核心驅動機制。標準化的事件規範對於確保數據的可靠性、可追溯性和系統的互操作性至關重要。

## 2. 事件信封 (Event Envelope)

所有在 NDH 系統中流動的事件都必須遵循統一的事件信封結構。這個信封提供了事件的元數據，使得事件能夠被正確地路由、處理和審計。

```json
{
  "id": "urn:uuid:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx",
  "source": "/ndh/site/factory_a/area/production_zone_b/asset/pump_001",
  "type": "com.ndh.asset.telemetry.update",
  "specversion": "1.0",
  "time": "2025-10-26T14:30:00.123Z",
  "datacontenttype": "application/json",
  "dataschema": "https://ndh.example.com/schemas/asset_telemetry_update_v1.json",
  "correlationid": "urn:uuid:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx",
  "causationid": "urn:uuid:xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx",
  "data": {
    // 實際事件數據，結構由 dataschema 定義
  }
}
```

**字段說明:**

*   **`id` (string, UUID)**: 事件的唯一識別符。用於事件的去重和追蹤。
*   **`source` (string, URI)**: 事件的來源。通常是一個 NDH 內部資產的唯一路徑，或外部系統的識別符。遵循 CloudEvents 規範。
*   **`type` (string)**: 事件的類型。描述事件發生的動作或狀態變化，例如 `com.ndh.asset.telemetry.update`、`com.ndh.asset.status.changed`、`com.ndh.agent.command.executed`。遵循 CloudEvents 規範。
*   **`specversion` (string)**: CloudEvents 規範版本，目前為 `1.0`。
*   **`time` (string, ISO 8601)**: 事件發生的時間，UTC 格式。精確到毫秒。
*   **`datacontenttype` (string)**: `data` 字段的內容類型，例如 `application/json`、`application/xml`。
*   **`dataschema` (string, URI)**: `data` 字段內容的 JSON Schema 定義的 URI。這使得事件數據可以被自動驗證和理解。
*   **`correlationid` (string, UUID, 可選)**: 用於追蹤跨多個服務的請求流。所有屬於同一邏輯操作的事件應共享相同的 `correlationid`。
*   **`causationid` (string, UUID, 可選)**: 指向直接導致此事件發生的前一個事件的 `id`。用於構建事件因果鏈。
*   **`data` (object)**: 實際的事件數據負載 (payload)。其結構由 `dataschema` 字段指定。

## 3. 一致性策略 (Consistency Strategy)

NDH 採用 **最終一致性 (Eventual Consistency)** 模型，並透過事件日誌和重放機制來確保數據的一致性和可追溯性。

1.  **事件日誌 (Event Log)**:
    *   所有進入 NDH 的事件（包括來自設備的遙測數據、來自 Agent 的指令、來自外部系統的狀態更新）都將被寫入一個不可變的、有序的事件日誌（例如基於 Apache Kafka）。
    *   事件日誌是系統的單一事實來源 (Single Source of Truth)。

2.  **事件重放 (Event Replay)**:
    *   在系統故障恢復、數據校正或新的服務部署時，NDH 能夠從事件日誌中重放歷史事件，以重建系統狀態或初始化新的服務狀態。
    *   這確保了即使在分散式環境中，不同服務也能最終達到一致的狀態。

3.  **冪等性 (Idempotency)**:
    *   所有事件處理器都應設計為冪等性，即多次處理同一個事件（基於 `id` 字段）應產生相同的結果，避免重複操作導致的錯誤。

4.  **因果一致性 (Causal Consistency)**:
    *   透過 `correlationid` 和 `causationid` 字段，NDH 能夠追蹤事件之間的因果關係，確保事件處理的邏輯順序和正確性，即使在亂序到達的情況下也能重建正確的狀態。

## 4. 時間規範 (Time Specification)

時間在工業系統中至關重要，NDH 對事件時間戳有嚴格的規範。

1.  **統一時間基準 (UTC)**:
    *   所有事件的 `time` 字段必須使用 UTC (Coordinated Universal Time) 時間，並遵循 ISO 8601 格式 (例如 `YYYY-MM-DDTHH:mm:ss.sssZ`)。這消除了時區差異帶來的混淆。

2.  **數據點時間戳 (Data Point Timestamp)**:
    *   對於遙測數據，`data` 負載內部應包含一個或多個時間戳，指示數據點實際採集的時間。這個時間戳應盡可能接近數據採集源，並同樣使用 UTC 和 ISO 8601 格式。
    *   `event.time` 字段表示事件被 NDH 接收或處理的時間，而 `data` 內部的時間戳表示數據點的原始生成時間。兩者可能存在微小差異，但都應精確記錄。

3.  **時鐘同步 (Clock Synchronization)**:
    *   NDH 叢集中的所有節點以及連接的邊緣設備都應實施 NTP (Network Time Protocol) 或 PTP (Precision Time Protocol) 進行時鐘同步，以確保時間戳的準確性和一致性。

這些規範共同構成了 IDTF 事件處理的基礎，確保了系統的穩定性、數據的可靠性和可追溯性，為構建高效、智能的工業數位分身提供了堅實的基礎。
