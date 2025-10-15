# Maintenance-Advisor Agent 設計文件

**文件編號**: IDTF-V3.6-06-05
**版本**: 1.0
**日期**: 2025-10-16
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)

---

## 1. 簡介

本文件旨在詳細闡述 `MaintenanceAdvisorAgent` 的設計，包括其功能、輸入輸出、決策邏輯以及與 NDH (Neutral Data Hub) 的互動方式。`MaintenanceAdvisorAgent` 作為 IDTF V3.6 MCP / AI Agent 擴充設計藍圖中的一個參考 Agent，其核心目標是透過監控資產狀態、分析歷史數據和預測潛在故障，為工廠維護提供智慧建議，從而提升設備可靠性、降低維護成本並減少停機時間。

## 2. 功能概述

`MaintenanceAdvisorAgent` 將提供以下核心功能：

*   **資產狀態監控**: 持續監控 NDH 中關鍵資產的實時運行數據，例如振動、溫度、壓力、電流等。
*   **異常檢測**: 透過機器學習模型或預設規則，識別資產運行數據中的異常模式，預警潛在故障。
*   **故障診斷**: 根據異常模式和歷史故障數據，診斷可能的故障類型和根本原因。
*   **維護建議**: 基於診斷結果、資產維護策略和備件庫存，生成最佳維護建議，例如：
    *   預防性維護排程調整
    *   預測性維護工單建議
    *   備件採購建議
    *   操作參數調整建議
*   **維護工單整合**: 將維護建議轉換為結構化的維護工單，並可與外部 CMMS/EAM 系統整合。
*   **性能評估與學習**: 監控維護建議的執行效果，並根據實際結果調整和優化其決策模型。

## 3. 輸入與輸出

### 3.1 輸入 (Input)

`MaintenanceAdvisorAgent` 將從 NDH 接收以下類型的數據作為輸入：

*   **實時資產遙測數據**: 來自 Data Acquisition 層的傳感器數據，例如：
    *   `ndh/telemetry/{asset_id}/vibration`
    *   `ndh/telemetry/{asset_id}/temperature`
    *   `ndh/telemetry/{asset_id}/pressure`
    *   `ndh/telemetry/{asset_id}/current`
*   **資產狀態機事件**: 來自 NDH 的狀態機事件，例如：
    *   `ndh/events/state_machine/{asset_id}/status_change`
    *   `ndh/events/state_machine/{asset_id}/alarm_triggered`
*   **歷史數據**: 透過 NDH API 查詢的歷史運行數據、維護記錄、故障模式和備件庫存資訊。
*   **IADL/FDL 資產元數據**: 透過 NDH API 獲取資產的詳細定義和實例化資訊，包括其工程屬性、維護屬性、ISA-95 階層歸屬等。

### 3.2 輸出 (Output)

`MaintenanceAdvisorAgent` 將其決策和運行狀態回寫至 NDH，主要透過以下方式：

*   **維護建議 (ndh/actions)**: 發布結構化的維護建議到 `ndh/actions` topic，例如：
    ```json
    {
      "agent_id": "MaintenanceAdvisorAgent",
      "action_type": "maintenance_recommendation",
      "timestamp": "2025-10-16T10:00:00Z",
      "target_asset_id": "Pump_001",
      "recommendation": {
        "type": "predictive_maintenance_order",
        "description": "Vibration anomaly detected on Pump_001. Recommend inspection and bearing replacement within 7 days.",
        "priority": "high",
        "estimated_downtime_hours": 4,
        "required_parts": [
          {"part_id": "Bearing_XYZ", "quantity": 2}
        ],
        "triggering_event": {"event_id": "VibrationAnomaly_P001_20251016"}
      }
    }
    ```
*   **Agent 運行日誌 (ndh/logs)**: 發布 Agent 的運行日誌、診斷結果和錯誤訊息到 `ndh/logs` topic，例如：
    ```json
    {
      "agent_id": "MaintenanceAdvisorAgent",
      "timestamp": "2025-10-16T09:55:00Z",
      "log_level": "INFO",
      "message": "Anomaly detected for Pump_001: high frequency vibration on motor bearing.",
      "details": {"model_confidence": 0.92, "sensor_data_snapshot": {...}}
    }
    ```
*   **資產狀態更新 (IADL State Memory)**: 更新 IADL Schema 中 `agent_state` 或 `maintenance_status` 等相關屬性，以反映 Agent 的診斷結果或維護排程。

## 4. 決策邏輯

`MaintenanceAdvisorAgent` 的決策邏輯將結合多種技術，包括基於規則的推理、統計分析和機器學習模型。

### 4.1 數據預處理與特徵工程

*   **數據清洗**: 處理傳感器數據中的缺失值、異常值和噪聲。
*   **特徵提取**: 從原始遙測數據中提取有意義的特徵，例如：
    *   振動數據的頻譜分析 (FFT)
    *   溫度趨勢、變化率
    *   運行時間、啟停次數

### 4.2 異常檢測模型

*   **閾值規則**: 對於簡單的異常，設定靜態或動態閾值。
*   **統計模型**: 例如控制圖 (Control Charts)、EWMA (指數加權移動平均) 等。
*   **機器學習模型**: 
    *   **無監督學習**: 如 Isolation Forest, One-Class SVM, Autoencoders 用於檢測未知的異常模式。
    *   **監督學習**: 如果有足夠的標註異常數據，可使用分類模型 (如 Random Forest, XGBoost) 進行異常分類。

### 4.3 故障診斷與預測

*   **模式識別**: 將檢測到的異常模式與已知的故障模式庫進行匹配。
*   **預測性模型**: 透過時間序列預測模型 (如 LSTM, Prophet) 預測資產的 RUL (Remaining Useful Life) 或下一次故障時間。
*   **因果推理**: 結合資產的物理模型和專家知識，推斷故障的根本原因。

### 4.4 維護策略與優化

*   **維護策略規則**: 根據資產的重要性、故障模式的嚴重性和 RUL 預測，應用預防性、預測性或事後維護策略。
*   **成本效益分析**: 考慮維護成本、停機成本和備件成本，優化維護排程。
*   **資源排程**: 考慮維護人員可用性、工具和備件庫存，生成可行的維護工單。

## 5. 與 NDH 的互動方式

`MaintenanceAdvisorAgent` 將透過 NDH 的 MQTT Broker 和 API 進行互動。

*   **訂閱 MQTT Topics**: 監聽 `ndh/telemetry/#` 和 `ndh/events/state_machine/#` 以獲取實時數據和事件。
*   **發布 MQTT Topics**: 將維護建議發布到 `ndh/actions`，將運行日誌發布到 `ndh/logs`。
*   **查詢 NDH API**: 透過 REST/GraphQL API 查詢歷史數據、資產元數據 (IADL/FDL) 和其他 Agent 的狀態。
*   **更新 IADL State Memory**: 透過 NDH API 更新資產的維護相關屬性，例如 `maintenance_status`, `next_maintenance_date` 等。

## 6. 結論

`MaintenanceAdvisorAgent` 的設計旨在為 IDTF 平台提供強大的智慧維護能力。透過與 NDH 的緊密整合，它能夠實現實時監控、預測性分析和智慧維護建議，從而顯著提升工廠的運營效率和資產可靠性。未來的實作將專注於模型的精確性、決策的自動化程度以及與外部維護管理系統的無縫整合。
