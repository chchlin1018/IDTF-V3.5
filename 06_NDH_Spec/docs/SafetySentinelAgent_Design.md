# Safety-Sentinel Agent 設計文件

**文件編號**: IDTF-V3.6-06-06
**版本**: 1.0
**日期**: 2025-10-16
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)

---

## 1. 簡介

本文件旨在詳細闡述 `SafetySentinelAgent` 的設計，包括其功能、輸入輸出、決策邏輯以及與 NDH (Neutral Data Hub) 的互動方式。`SafetySentinelAgent` 作為 IDTF V3.6 MCP / AI Agent 擴充設計藍圖中的一個參考 Agent，其核心目標是透過持續監控工廠環境和設備運行狀態，識別潛在的安全風險，預警危險情況，並在必要時觸發安全響應措施，從而保障人員安全和設備穩定運行。

## 2. 功能概述

`SafetySentinelAgent` 將提供以下核心功能：

*   **環境安全監控**: 監控工廠環境中的關鍵安全參數，例如：
    *   有毒氣體濃度 (CO, H2S, NH3 等)
    *   可燃氣體濃度 (CH4, LPG 等)
    *   煙霧/火災探測
    *   緊急停止按鈕狀態
    *   安全門/圍欄狀態
*   **設備安全監控**: 監控設備運行中的安全相關參數，例如：
    *   超溫、超壓、超速
    *   異常振動、洩漏
    *   安全聯鎖狀態
*   **人員安全監控**: 結合視覺識別或穿戴設備數據，監控人員是否進入危險區域、是否佩戴安全設備等。
*   **風險評估與預警**: 根據監控數據和預設的安全規則/模型，評估當前風險等級，並在風險超標時發出多級預警。
*   **安全響應建議**: 在檢測到危險情況時，提供自動化的安全響應建議，例如：
    *   發出聲光警報
    *   觸發緊急停機
    *   隔離危險區域
    *   通知相關安全人員
*   **安全事件記錄與分析**: 記錄所有安全事件的詳細資訊，並進行根本原因分析，以持續改進安全策略。

## 3. 輸入與輸出

### 3.1 輸入 (Input)

`SafetySentinelAgent` 將從 NDH 接收以下類型的數據作為輸入：

*   **實時環境傳感器數據**: 來自 Data Acquisition 層的傳感器數據，例如：
    *   `ndh/telemetry/{sensor_id}/gas_concentration`
    *   `ndh/telemetry/{sensor_id}/smoke_detection`
*   **實時設備遙測數據**: 來自 Data Acquisition 層的設備運行數據，例如：
    *   `ndh/telemetry/{asset_id}/temperature` (超溫)
    *   `ndh/telemetry/{asset_id}/pressure` (超壓)
    *   `ndh/telemetry/{asset_id}/interlock_status`
*   **資產狀態機事件**: 來自 NDH 的狀態機事件，例如：
    *   `ndh/events/state_machine/{asset_id}/alarm_triggered`
    *   `ndh/events/state_machine/{asset_id}/emergency_stop_activated`
*   **IADL/FDL 資產元數據**: 透過 NDH API 查詢的資產詳細定義和實例化資訊，包括其安全等級、危險區域定義、安全聯鎖邏輯等。
*   **安全規範與規則**: 預設的安全操作規程 (SOP)、風險評估矩陣和法規要求。

### 3.2 輸出 (Output)

`SafetySentinelAgent` 將其決策和運行狀態回寫至 NDH，主要透過以下方式：

*   **安全預警/響應指令 (ndh/actions)**: 發布結構化的安全預警或響應指令到 `ndh/actions` topic，例如：
    ```json
    {
      "agent_id": "SafetySentinelAgent",
      "action_type": "safety_alert",
      "timestamp": "2025-10-16T11:00:00Z",
      "risk_level": "critical",
      "description": "High concentration of methane detected in Zone A. Recommend immediate evacuation and emergency shutdown of gas supply.",
      "triggering_sensor_id": "GasSensor_005",
      "recommended_actions": [
        {"action": "activate_alarm", "target": "ZoneA_Alarm"},
        {"action": "emergency_shutdown", "target": "GasSupplyValve_001"},
        {"action": "notify_personnel", "role": "SafetyManager"}
      ]
    }
    ```
*   **Agent 運行日誌 (ndh/logs)**: 發布 Agent 的運行日誌、風險評估結果和錯誤訊息到 `ndh/logs` topic，例如：
    ```json
    {
      "agent_id": "SafetySentinelAgent",
      "timestamp": "2025-10-16T10:58:00Z",
      "log_level": "WARNING",
      "message": "Methane concentration exceeding threshold (5% LEL) in Zone A.",
      "details": {"current_value": "6.2% LEL", "threshold": "5% LEL", "location": "Zone A"}
    }
    ```
*   **資產狀態更新 (IADL State Memory)**: 更新 IADL Schema 中 `agent_state` 或 `safety_status` 等相關屬性，以反映 Agent 的安全評估結果或觸發的安全措施。

## 4. 決策邏輯

`SafetySentinelAgent` 的決策邏輯將主要基於預設的安全規則、閾值監控和潛在的機器學習模型。

### 4.1 數據預處理與情境化

*   **數據清洗**: 處理傳感器數據中的異常值和噪聲。
*   **情境化**: 將實時數據與資產的地理位置、所屬區域、相關人員等情境資訊結合，以便進行更精確的風險評估。

### 4.2 安全規則引擎

*   **閾值觸發**: 監控關鍵安全參數，一旦超過預設的靜態或動態閾值，立即觸發警報。
*   **聯鎖邏輯**: 根據設備的安全聯鎖邏輯，判斷多個條件組合是否構成危險情況。
*   **SOP 規則**: 將標準操作規程 (SOP) 轉換為可執行的規則，確保操作符合安全要求。

### 4.3 風險評估模型

*   **基於規則的風險評估**: 根據觸發的事件和相關情境，結合預設的風險矩陣，評估風險等級 (低、中、高、危急)。
*   **機器學習異常檢測**: 對於複雜的設備運行模式，可使用無監督學習模型檢測異常行為，預警潛在故障導致的安全風險。

### 4.4 安全響應策略

*   **多級響應**: 根據風險等級和事件類型，觸發不同級別的安全響應，從輕微警告到緊急停機。
*   **自動化響應**: 在高風險情況下，自動觸發預設的安全措施，如關閉閥門、啟動排風系統、隔離電源等。
*   **通知與升級**: 自動通知相關安全人員、管理層，並根據事件的嚴重性進行升級。

## 5. 與 NDH 的互動方式

`SafetySentinelAgent` 將透過 NDH 的 MQTT Broker 和 API 進行互動。

*   **訂閱 MQTT Topics**: 監聽 `ndh/telemetry/#` 和 `ndh/events/state_machine/#` 以獲取實時安全相關數據和事件。
*   **發布 MQTT Topics**: 將安全預警/響應指令發布到 `ndh/actions`，將運行日誌發布到 `ndh/logs`。
*   **查詢 NDH API**: 透過 REST/GraphQL API 查詢歷史安全事件、資產元數據 (IADL/FDL) 和安全規範。
*   **更新 IADL State Memory**: 透過 NDH API 更新資產的 `safety_status` 或 `risk_level` 等相關屬性。

## 6. 結論

`SafetySentinelAgent` 的設計旨在為 IDTF 平台提供全面的工廠安全監控和響應能力。透過與 NDH 的緊密整合，它能夠實現實時風險評估、自動化安全預警和響應，從而顯著提升工廠的安全管理水平，保護人員和設備免受潛在危害。未來的實作將專注於安全規則的精確性、響應的即時性以及與現有安全系統的無縫整合。
