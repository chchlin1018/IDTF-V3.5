# MCP 層設計文件

**文件編號**: IDTF-V3.5-06-03
**版本**: 1.0
**日期**: 2025-10-15
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)

---

## 1. 簡介

本文件旨在詳細闡述 IDTF V3.5 中 MCP (Multi-Control/Communication Protocol) 層的設計，特別是 MCP Broker 的架構、其與 AI Agent 的通訊機制，以及如何利用 IADL Schema 作為 Agent 狀態記憶 (State Memory) 的實作。MCP 層的引入旨在提升 NDH (Neutral Data Hub) 的上層應用能力，實現多個 AI Agent 之間透過事件與狀態主題的互通，從而支援更複雜的智慧工廠應用場景。

## 2. MCP Broker 架構與功能

MCP Broker 作為 NDH 上層的關鍵組件，其核心職責是協調和管理 AI Agent 之間的通訊與協作。它將作為一個輕量級、高效率的訊息中介，確保 Agent 能夠可靠地交換資訊並同步狀態。

### 2.1 架構概述

MCP Broker 將部署在 NDH 服務層之上，與 NDH 的 MQTT Broker 緊密整合。它將提供一套標準化的 API 和介面，供 AI Agent 註冊、發布事件、訂閱狀態更新，並查詢歷史狀態。

```mermaid
graph TD
    subgraph AI Agent Layer
        A[AI Agent 1] -->|事件/狀態| MCPB(MCP Broker)
        B[AI Agent 2] -->|事件/狀態| MCPB
        C[AI Agent N] -->|事件/狀態| MCPB
    end

    subgraph MCP Layer
        MCPB -->|MQTT Topics| NDH_MQTT[NDH MQTT Broker]
    end

    subgraph NDH Layer
        NDH_MQTT --> NDH_Core[NDH Core Services]
        NDH_Core --> IADL_DB[IADL State Memory (Database)]
    end

    NDH_Core <--> OT_IT[OT/IT Systems]
```

### 2.2 核心功能

*   **Agent 註冊與發現**: 允許 AI Agent 註冊其身份、能力和通訊端點，並發現其他可用的 Agent。
*   **事件路由**: 根據預定義的規則或訂閱關係，將事件從發布 Agent 路由到訂閱 Agent。
*   **狀態管理**: 接收 Agent 發布的狀態更新，並將其持久化到 IADL State Memory 中，同時通知相關訂閱者。
*   **歷史查詢**: 提供介面供 Agent 查詢特定資產或 Agent 的歷史狀態。
*   **安全與認證**: 確保只有經過授權的 Agent 才能參與通訊和狀態管理。

## 3. AI Agent 通訊機制

AI Agent 之間的通訊將主要透過 NDH MQTT Topics 進行，並可利用 LangGraph、AutoGen 或 CrewAI 等開源 Agent orchestration SDK 來實現複雜的協作邏輯。

### 3.1 NDH MQTT Topics 作為事件通道

NDH 內建的 MQTT Broker 將作為 MCP 層的核心訊息傳輸機制。Agent 將透過發布和訂閱特定的 MQTT Topics 來交換事件和狀態更新。這種方式具有輕量、高效、支援發布/訂閱模式的優點，非常適合工業物聯網環境。

**建議的 MQTT Topic 結構**:

*   `mcp/events/{agent_id}/{event_type}`: 用於 Agent 發布特定事件。
*   `mcp/status/{agent_id}/{asset_id}`: 用於 Agent 發布其對特定資產的狀態更新。
*   `mcp/commands/{agent_id}/{command_type}`: 用於 Agent 向其他 Agent 發送指令。

### 3.2 開源 Agent Orchestration SDK 整合

為了簡化 AI Agent 的開發和協作，建議整合以下開源 Agent orchestration SDK：

*   **LangGraph**: 適用於構建有狀態、多步驟的 Agent 工作流，能夠清晰定義 Agent 之間的狀態轉移和決策邏輯。
*   **AutoGen**: 支援多 Agent 對話和協作，特別適合需要多個 Agent 共同解決問題的場景。
*   **CrewAI**: 提供 Agent 角色定義、任務分配和協作框架，有助於構建結構化的 Agent 團隊。

這些 SDK 將負責 Agent 內部的邏輯編排、訊息解析和狀態同步，並透過 MCP Broker 提供的 MQTT 介面與其他 Agent 進行通訊。

## 4. IADL Schema 作為 Agent 狀態記憶 (State Memory)

IADL (Industrial Asset Definition Language) Schema 將被擴展，以作為 AI Agent 的統一狀態記憶模型。這確保了 Agent 狀態的結構化、標準化和可追溯性，並與底層的資產數據模型保持一致。

### 4.1 IADL Schema 擴展

IADL Schema 將新增用於描述 Agent 狀態的區塊，例如 `agent_state` 或 `agent_memory`。這些區塊可以包含：

*   **當前任務**: Agent 正在執行的任務 ID 或描述。
*   **目標**: Agent 的長期或短期目標。
*   **內部狀態**: Agent 內部的變數、模型參數或學習狀態。
*   **觀察結果**: Agent 從環境中獲取的最新觀察數據。
*   **決策歷史**: Agent 過去的決策記錄。
*   **信任度**: Agent 對於某些資訊或其他 Agent 的信任程度。

**IADL Schema 範例擴展**:

```yaml
asset_type: AI_Agent
model: PredictiveMaintenanceAgent_v1.0
manufacturer: IDTF_AI
metadata:
  category: Maintenance
  function: AnomalyDetection
agent_state:
  current_task: "MonitorPump_001_Vibration"
  task_status: "in_progress"
  last_observation_timestamp: "2025-10-15T10:30:00Z"
  observed_vibration_level: 5.2 # mm/s
  anomaly_detected: false
  confidence_score: 0.95
  related_asset_id: "Pump_001"
agent_memory:
  long_term_goals:
    - "Minimize downtime for critical assets"
  short_term_goals:
    - "Analyze Pump_001 data for 24 hours"
  knowledge_base_version: "KB_v2.1"
```

### 4.2 狀態記憶實作細節

*   **持久化**: Agent 的狀態將被持久化到 NDH 的數據庫中，確保即使 Agent 重啟也能恢復其工作狀態。這可以透過將 IADL Schema 映射到 NoSQL (如 MongoDB) 或關係型數據庫 (如 PostgreSQL) 實現。
*   **版本控制**: 每個 Agent 狀態的更新都應支援版本控制，以便追溯 Agent 的決策過程和行為演變。
*   **查詢與訂閱**: MCP Broker 將提供 API 供 Agent 查詢其他 Agent 的狀態，或訂閱特定 Agent 狀態的變化。
*   **一致性**: 透過 IADL Schema 的嚴格定義，確保不同 Agent 之間對狀態的理解和表示保持一致。

## 5. 結論

MCP 層的設計，特別是 MCP Broker 的引入，將顯著提升 IDTF 在 AI Agent 協作和智慧應用方面的能力。透過標準化的通訊機制 (NDH MQTT Topics) 和統一的狀態記憶模型 (IADL Schema)，IDTF 將能夠支持構建更智能、更靈活的工業數位分身解決方案，為工業 4.0 的發展奠定堅實基礎。

