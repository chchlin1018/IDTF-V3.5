# IDTF V3.6 —— MCP / AI Agent 擴充設計藍圖 (Draft)

**文件編號**: IDTF-V3.6-01-01
**版本**: 1.0
**日期**: 2025-10-16
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)

---

## 1. 目標與範圍

IDTF V3.6 的核心目標是在現有的 NDH (Neutral Data Hub) 之上，引入 **MCP (Multi-Agent Control Plane)** 與 **Agent SDK**，以構建一個開放、可擴展的 AI Agent 生態系統。這將允許多個 AI Agent 和基於規則的 Agent 能夠訂閱實時事件、執行決策並向工業設備或系統下達指令，從而實現更智能、更自主的工廠運營。

### 1.1 核心目標

*   **啟用多 Agent 協作**: 建立一個框架，使不同的 AI Agent 能夠透過標準化的通訊協議進行互動和協作。
*   **實時決策與控制**: 賦予 Agent 根據實時數據進行分析、決策並觸發控制動作的能力。
*   **提升工廠智能化水平**: 透過 Agent 的引入，實現能源優化、預測性維護、安全監控等高級智能應用。

### 1.2 範圍

本設計藍圖涵蓋以下關鍵領域：

*   **MCP Control Plane 設計**: 定義 MCP 的核心架構、組件和功能。
*   **Agent SDK 規範**: 規劃 Agent 開發所需的 SDK 功能和介面。
*   **Reference Agents 設計**: 設計三個具有代表性的參考 Agent，以驗證框架的有效性和價值。

### 1.3 供應商中立原則

IDTF V3.6 將嚴格遵循供應商中立原則，確保框架的開放性和互操作性：

*   **主要介面**: 以 **MQTT** (包含 SparkplugB 命名風格) 和 **REST/GraphQL** 作為 Agent 與 NDH/MCP 之間的主要通訊介面。
*   **語意底座**: 以 **IADL (Industrial Asset Definition Language)** 和 **FDL (Factory Design Language)** 作為統一的語意底座，確保數據和狀態的標準化表示。

### 1.4 Reference Agents

為驗證 IDTF V3.6 的價值和可行性，將優先實作以下三個可驗證價值的 Reference Agents：

*   **Energy-Optimizer Agent**: 負責監控能源消耗，並根據生產計畫和能源價格進行優化，以降低能耗。
*   **Maintenance-Advisor Agent**: 負責監控設備狀態，預測潛在故障，並提供維護建議。
*   **Safety-Sentinel Agent**: 負責監控工廠環境和設備安全，預警危險情況，並觸發安全響應。

## 2. MCP (Multi-Agent Control Plane) 設計

MCP Control Plane 是 IDTF V3.6 的核心組件，負責管理和協調 AI Agent 的生命週期和通訊。其架構設計旨在提供高可用性、可擴展性和安全性。

### 2.1 架構概述

MCP Control Plane 將作為 NDH 之上的獨立服務層，與 NDH 的 MQTT Broker 緊密整合。它將提供 Agent 註冊、發現、事件分發、狀態管理、ACL (Access Control List) 和審計等核心功能。

```mermaid
graph TD
    subgraph Apps/Dashboards
        A[Apps / Dashboards] -->|WS/HTTP| SL(Service Layer)
    end

    subgraph Service Layer
        SL -->|REST / GraphQL / WS| PL(Processing Layer)
    end

    subgraph Processing Layer
        PL -->|Stream / Batch / AI| NDH(NDH Data Hub)
    end

    subgraph NDH Data Hub
        NDH -->|Kafka/TSDB/Postgres/Redis + Event Bus| DA(Data Acquisition)
    end

    subgraph Data Acquisition
        DA -->|OPC UA / MQTT / ...| MCPCP(MCP Control Plane)
        DA -->|ndh/# telemetry / events / cmds| AR(Agent Runtimes)
    end

    subgraph MCP Control Plane
        MCPCP <--> AR
        MCPCP -->|agents/# (MQTT Topics)| AR
    end

    subgraph Agent Runtimes
        AR -->|py/go/node SDK + policies| NDH
    end
```

### 2.2 核心組件與功能

*   **Agent Registry**: 負責 Agent 的註冊、發現和元數據管理。Agent 在啟動時向 Registry 註冊其身份、能力和通訊端點。
*   **Event Dispatcher**: 負責將來自 NDH 或其他 Agent 的事件分發到訂閱的 Agent。支援基於 Topic 和內容的路由。
*   **State Manager**: 負責接收 Agent 發布的狀態更新，並將其持久化到 IADL State Memory 中。提供狀態查詢和歷史追溯功能。
*   **ACL & Policy Engine**: 實施基於角色或屬性的訪問控制，確保 Agent 只能訪問其被授權的數據和資源，並執行被允許的操作。
*   **Scheduler**: 負責 Agent 任務的排程和協調，特別是對於需要定時執行或依賴特定條件觸發的 Agent。
*   **Audit Log**: 記錄所有 Agent 的活動、決策和通訊，以便進行審計和故障排除。

### 2.3 與 NDH 的整合

*   **MQTT Broker**: MCP Control Plane 將利用 NDH 內建的 MQTT Broker 作為 Agent 之間以及 Agent 與 MCP 之間的主要通訊通道。專用 Topic Namespace `agents/#` 將用於 Agent 間的事件和指令交換。
*   **NDH API**: MCP Control Plane 將透過 NDH 的 REST/GraphQL API 存取資產元數據 (IADL/FDL)、歷史數據和配置資訊。
*   **IADL State Memory**: IADL Schema 將被擴展，作為 Agent 狀態的統一記憶模型，持久化在 NDH 的數據庫中。

## 3. Agent SDK 規範

Agent SDK 旨在簡化 AI Agent 的開發、部署和管理，提供一套標準化的介面和工具，使其能夠輕鬆地與 MCP Control Plane 和 NDH 互動。

### 3.1 核心功能

*   **Agent 註冊與生命週期管理**: 提供 API 供 Agent 註冊、啟動、停止和更新其狀態。
*   **MQTT 通訊介面**: 封裝 MQTT 客戶端，提供簡單易用的發布/訂閱介面，支援 `agents/#` 和 `ndh/#` Topic Namespace。
*   **NDH API 客戶端**: 提供用於存取 NDH REST/GraphQL API 的客戶端庫，方便 Agent 查詢資產數據、歷史數據和更新 IADL State Memory。
*   **IADL/FDL 數據模型抽象**: 提供數據模型抽象層，使 Agent 能夠以物件導向的方式操作 IADL/FDL 數據，而無需直接處理底層的 JSON 或 YAML 格式。
*   **狀態記憶管理**: 提供介面供 Agent 讀取和寫入其在 IADL State Memory 中的狀態。
*   **日誌與監控**: 整合標準日誌框架，並提供 Agent 運行狀態的監控介面。

### 3.2 支援語言與框架

Agent SDK 將支援多種主流程式語言，例如 Python、Go 和 Node.js，以滿足不同開發者的需求。同時，建議整合 LangGraph、AutoGen 或 CrewAI 等開源 Agent orchestration SDK，以簡化複雜 Agent 工作流的編排。

## 4. Reference Agents 設計

### 4.1 Energy-Optimizer Agent

*   **目標**: 透過監控能源消耗、生產計畫和能源價格，優化工廠能源使用，降低能耗和成本。
*   **輸入**: 實時能源數據 (電力、燃氣)、生產排程、能源價格、天氣預報、IADL/FDL 資產元數據。
*   **決策邏輯**: 結合時間序列預測、線性規劃或強化學習模型，生成最佳能源調度策略。
*   **輸出**: 能源調度指令 (如設備啟停、負載調整)、能源消耗預測、節能報告。
*   **與 NDH 互動**: 訂閱 `ndh/telemetry/energy/#`，發布 `ndh/actions/energy_optimization` 和 `ndh/logs/energy_optimizer`。

### 4.2 Maintenance-Advisor Agent

*   **目標**: 透過監控設備狀態、分析歷史數據和預測潛在故障，為工廠維護提供智慧建議。
*   **輸入**: 實時設備遙測數據 (振動、溫度、壓力)、歷史維護記錄、故障模式庫、備件庫存、IADL/FDL 資產元數據。
*   **決策邏輯**: 異常檢測 (閾值、統計、ML)、故障診斷、RUL (Remaining Useful Life) 預測、維護策略優化。
*   **輸出**: 維護建議 (如預防性/預測性工單)、故障診斷報告、備件採購建議。
*   **與 NDH 互動**: 訂閱 `ndh/telemetry/asset/#` 和 `ndh/events/state_machine/#`，發布 `ndh/actions/maintenance_recommendation` 和 `ndh/logs/maintenance_advisor`。

### 4.3 Safety-Sentinel Agent

*   **目標**: 透過持續監控工廠環境和設備運行狀態，識別潛在的安全風險，預警危險情況，並觸發安全響應措施。
*   **輸入**: 實時環境傳感器數據 (氣體濃度、煙霧)、設備安全聯鎖狀態、人員位置、安全規範、IADL/FDL 資產元數據。
*   **決策邏輯**: 安全規則引擎、閾值監控、異常行為檢測、風險評估模型。
*   **輸出**: 安全預警 (多級)、安全響應指令 (如緊急停機、隔離區域)、安全事件報告。
*   **與 NDH 互動**: 訂閱 `ndh/telemetry/safety/#` 和 `ndh/events/state_machine/#`，發布 `ndh/actions/safety_alert` 和 `ndh/logs/safety_sentinel`。

## 5. 結論

IDTF V3.6 的 MCP / AI Agent 擴充設計藍圖為構建智能、自主的工業數位分身解決方案奠定了基礎。透過引入 MCP Control Plane 和 Agent SDK，並實作三個 Reference Agents，IDTF 將能夠有效地整合 AI 能力，實現實時決策、優化運營和提升安全性，從而加速工業 4.0 的轉型。未來的開發將專注於完善各組件的實作細節、強化安全機制和提升 Agent 的協作能力。
