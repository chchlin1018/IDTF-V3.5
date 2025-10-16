# 審計與變更管控 (Audit & Change Control)

## 1. 概述

本文件定義了 NDH (Neutral Data Hub) 的審計 (Audit) 和變更管控 (Change Control) 規範，旨在確保所有對 NDH 系統、數據和配置的修改都可追溯、可驗證且符合安全和合規性要求。在工業環境中，對數據和系統變更的完整審計日誌是故障排除、安全分析和法規遵循的關鍵。

## 2. 審計日誌 (Audit Log)

所有對 NDH 系統的關鍵操作都必須生成詳細的審計日誌。審計日誌應包含以下核心信息：

1.  **操作者 (Who)**: 執行操作的使用者、Agent 或系統進程的唯一識別符。
2.  **操作時間 (When)**: 操作發生的精確時間，使用 UTC 和 ISO 8601 格式。
3.  **操作類型 (What)**: 執行了什麼操作，例如 `create_asset_instance`、`update_asset_property`、`send_command`、`login` 等。
4.  **操作對象 (Where)**: 操作影響的資源，例如 Asset Instance ID、Logical Group ID、Data Product ID、配置參數等。
5.  **操作結果 (Result)**: 操作是成功還是失敗，以及任何相關的錯誤信息。
6.  **上下文信息 (Context)**: 操作的來源 IP、會話 ID、相關的 `correlationid` 或 `causationid` 等。
7.  **變更詳情 (Details)**: 對於數據或配置的修改操作，應記錄變更前和變更後的值。

**審計日誌示例**:

```json
{
  "timestamp": "2025-10-26T17:00:00.123Z",
  "operator": {
    "type": "user",
    "id": "user_alice",
    "role": "admin"
  },
  "action": "update_asset_property",
  "resource": {
    "type": "AssetInstance",
    "id": "urn:ndh:asset:factory_a:pump_001",
    "property_name": "setpoint_pressure"
  },
  "details": {
    "old_value": "5.0",
    "new_value": "5.5",
    "unit": "bar"
  },
  "result": "success",
  "context": {
    "source_ip": "192.168.1.100",
    "session_id": "sess_xyz123",
    "correlation_id": "uuid_abc"
  }
}
```

## 3. 變更管控 (Change Control)

NDH 實施嚴格的變更管控流程，特別是對於影響生產運營或數據完整性的關鍵配置和數據。

### 3.1. 配置版本控制 (Configuration Versioning)

*   所有 NDH 的核心配置（例如 FDL 文件、IADL 文件、Agent Manifest、數據產品定義）都將儲存在版本控制系統中（例如 Git）。
*   任何配置的修改都必須經過版本控制，並附有明確的提交信息。

### 3.2. 審批流程 (Approval Workflow)

*   對於高風險的變更（例如修改 Asset Instance 的控制參數、部署新的 Agent、修改數據產品 Schema），NDH 將強制執行多級審批流程。
*   審批流程可以整合到現有的 CI/CD (持續集成/持續部署) 管線中，確保變更在部署到生產環境之前經過充分審查。
*   Agent Manifest 中的 `approvalFlow` 字段用於定義 Agent 部署或關鍵操作的審批類型（自動或人機共治）。

### 3.3. 變更回滾 (Rollback Capability)

*   NDH 系統應具備在變更引入問題時，快速回滾到先前穩定狀態的能力。
*   這透過版本控制的配置和事件日誌的重放機制來實現。

### 3.4. 變更通知 (Change Notification)

*   關鍵變更發生時，相關的利益關係者（例如系統管理員、數據產品所有者、Agent 開發者）應透過通知機制（例如郵件、即時通訊）收到通知。

## 4. 安全與合規性 (Security & Compliance)

*   **不可篡改性**: 審計日誌一旦生成，應確保其不可被篡改。這可以透過使用區塊鏈技術或加密簽名來實現。
*   **訪問控制**: 只有經過授權的使用者或系統才能訪問審計日誌。基於角色的訪問控制 (RBAC) 應應用於審計日誌的讀取和管理。
*   **保留策略**: 審計日誌應根據法規和企業政策進行長期保留。
*   **隱私保護**: 審計日誌應避免記錄敏感的個人身份信息 (PII)，或對其進行匿名化處理。

## 5. 驗收標準 (Acceptance Criteria)

*   **審計日誌完整性**: 確保所有關鍵操作都生成審計日誌，並包含所有必要的字段。
*   **變更可追溯性**: 能夠追溯任何系統配置或數據變更的來源、時間和內容。
*   **高風險命令審批**: 驗證高風險命令（例如 Agent Manifest 中定義的）是否成功觸發了人機共治的審批流程。

透過實施這些審計和變更管控規範，NDH 將為工業數位分身平台提供一個堅固的基礎，確保其在高度監管和安全要求的環境下可靠運行。
