# NDH 資產管理技術概述

**文件編號**: IDTF-V3.5-06-03
**版本**: 1.0
**日期**: 2025-10-16
**作者**: Manus AI

---

## 1. 簡介

本文件旨在詳細闡述 IDTF (Industrial Digital Twin Framework) 中 NDH (Neutral Data Hub) 如何生成與管理 **Asset Instance**，以及這些 Asset Instance 如何與 **IADL (Industrial Asset Description Language)**、**FDL (Factory Design Language)**、**Omniverse 整合**、**USD 模型連結**，以及 **Asset Tag Instance** 與實際 IoT 數據連動的關係。同時，也將探討 NDH 為應對大規模工業部署所採用的叢集架構。

## 2. NDH 中 Asset Instance 的生成與管理

在 NDH 中，**Asset Instance** 是指一個具體的、可操作的數位分身實例，它代表了物理世界中的一個真實資產（例如，一台特定的泵、一個特定的感測器、一條特定的生產線）。這個 Asset Instance 不僅包含了資產的靜態描述，還整合了其動態的運行數據和行為。

### 2.1. FDL 驅動的 Asset Instance 部署

NDH 中 Asset Instance 的生成主要由 FDL 驅動。FDL 作為工廠的「藍圖」，聲明式地定義了工廠的物理佈局、邏輯分區以及其中部署的資產實例的數量和類型。NDH 的 FDL 解析器會讀取這些定義，並據此在 NDH 內部數據模型中**自動實例化 (Instantiate)** 對應的 Asset Instance。這確保了數位分身與物理世界資產的一致性。

### 2.2. Asset Instance 的構成要素

每個 Asset Instance 包含以下關鍵資訊：

*   **唯一識別符 (Unique Identifier)**：每個 Instance 都有一個全局唯一的 ID (例如 `DS_2F_A_001`, `PUMP_001`)，通常根據 FDL 中的命名規則生成。
*   **資產類型 (Asset Type)**：引用其所基於的 IADL `AssetType` 定義，這提供了資產的藍圖和通用屬性。
*   **靜態屬性 (Static Properties)**：來自 IADL 定義的固定屬性，例如製造商、型號、安裝日期、額定功率等。
*   **空間資訊 (Spatial Information)**：來自 FDL 定義的物理位置 (`origin`) 和方向 (`orientation`)，用於在 3D 空間中定位資產。
*   **層次結構關係 (Hierarchical Relationships)**：它在整個資產層次結構中的位置，例如它屬於哪個 `Area`、哪個 `Building`，以及它包含哪些 `Component` 或被哪些 `Component` 包含。這反映了 ISA-95 的設備階層。
*   **數據點綁定 (Data Point Bindings)**：每個 Asset Instance 都會與一個或多個 **Asset Tag Instance** 綁定，每個 Asset Tag Instance 管理一個特定的 IoT Tag 資料源。
*   **3D 模型引用 (3D Model Reference)**：來自 IADL 定義的 3D 模型文件路徑，用於在 Omniverse USD Scene Graph 中視覺化該資產。

### 2.3. Asset Instance 的生命週期與動態性

Asset Instance 具有動態性，能夠反映物理資產的實時狀態和行為：

*   **實時數據更新**：透過其綁定的 Asset Tag Instance，Asset Instance 持續接收來自物理設備的實時遙測數據。
*   **狀態變化**：Asset Instance 的狀態會根據實時數據和預定義的邏輯進行更新。
*   **事件觸發**：當 Asset Instance 的數據或狀態發生特定變化時，可以觸發事件，進而啟動 AI Agent 進行分析或控制。

## 3. Asset Instance 與 Asset Tag Instance 的關係

**Asset Tag Instance** 是 NDH 中負責管理單一 IoT Tag 資料源的實例。每個 Asset Instance 可能擁有一至多個 Asset Tag Instance，這完全按照其 IADL `AssetType` 中 `data_tags` 的定義。

*   **IADL 定義多個資料點**：在 IADL 文件中，一個 `AssetType` 可以定義多個 `data_tags`，每個 `data_tag` 代表一個 IoT Tag 資料源（例如溫度、壓力、轉速）。
*   **Asset Tag Instance 的實例化**：當 NDH 實例化一個 Asset Instance 時，它會遍歷其 IADL 定義中的每個 `data_tag`，並為每個 `data_tag` 創建一個獨立的 Asset Tag Instance。
*   **Asset Tag Instance 的職責**：每個 Asset Tag Instance 專注於一項任務：根據 `source_mapping` 的定義，連接到指定的後端資料源（如 OPC UA 伺服器、MQTT Broker 或時序資料庫），並負責該特定 Tag 數據的讀取、寫入和訂閱。它將抽象的 `tag_id` 與後端的實際數據點進行解耦。

## 4. IADL、FDL、Omniverse 整合與 USD 模型連結

### 4.1. IADL 與 FDL 的協同作用

*   **FDL (Factory Design Language)**：定義了工廠的物理佈局和資產的組織結構。FDL 中的 `AssetInstance` 條目是 NDH 創建 Asset Instance 的直接依據，提供了 Instance 的數量、命名規則和空間位置。
*   **IADL (Industrial Asset Description Language)**：定義了資產的「藍圖」或「類型」。每個 NDH Asset Instance 都是一個特定 IADL `AssetType` 的具體化。IADL 提供了 Asset Instance 的靜態屬性、數據點 (`data_tags`) 和 3D 模型引用等詳細定義。

### 4.2. Omniverse 整合與 USD 模型連結

*   **USD Model Reference**：IADL 不僅定義了資產的數據點和屬性，還包含了其 3D 模型資訊，例如 `geometry.model_file`。這些引用通常指向 USD (Universal Scene Description) 格式的模型文件。
*   **NDH USD Integration Service**：當 Asset Instance 被創建時，NDH 的 USD Integration Service 會利用 IADL 中定義的 USD Model Reference，將 Asset Instance 在 Omniverse USD Scene Graph 中進行視覺化。這使得每個 Asset Instance 在虛擬世界中都有其對應的 3D 表示，實現了物理與虛擬的精確對映。
*   **實時數據同步**：透過 Asset Tag Instance 獲取的實時數據，會透過 USD Integration Service 同步到 Omniverse USD Scene Graph 中對應 Asset Instance 的自定義屬性上，實現數據驅動的 3D 視覺化和模擬。

## 5. Asset Instance 的階層結構管理

FDL 不僅定義了單個資產的部署，更定義了資產之間的物理和邏輯階層關係（例如工廠 -> 建築 -> 區域 -> 設備）。NDH 會根據 FDL 的這些定義，構建一個反映 ISA-95 設備階層的 **Asset Instance 階層樹**。這個階層結構在 NDH 內部進行管理，並可透過 API 進行查詢和遍歷，為上層應用提供結構化的資產視圖。

## 6. NDH 與 MCP 的職責邊界與整合

### 6.1. 職責邊界

為確保架構的清晰性和可維護性，NDH 與 MCP (Manus Control Plane) 的職責邊界明確劃分如下：

| 層級 | 主要職責 |
|:---|:---|
| **MCP (Manus Control Plane)** | **Agent 層**：負責 AI Agent 的生命週期管理、高層級工作流編排、權限與審計 (ACL)、以及 Agent 間的通訊協調。 |
| **NDH (Neutral Data Hub)** | **數據層**：負責底層數據存儲與檢索、MQTT/REST 通訊基礎設施、實時事件流處理、時序數據管理、以及 Connector 管理。 |

### 6.2. 整合方式

*   **清晰的介面定義**: MCP 透過 **gRPC** 或內部 **REST API** 與 NDH 進行通訊，避免直接存取 NDH 的資料庫。
*   **事件驅動架構**: MCP 與 NDH 之間的互動主要基於事件驅動架構 (Event Sourcing)，確保了兩者之間的鬆耦合和高可擴展性。

## 7. 架構優化策略

為了應對大規模工業部署的挑戰，IDTF/NDH 採用了一系列架構優化策略，確保系統的高效能、可擴展性和穩定性。

### 7.1. 分散式架構

NDH 和 MCP 都採用分散式叢集架構，以實現高可用性和水平擴展。

#### 7.1.1. NDH 叢集

*   **基於 Kubernetes 自動擴展**：NDH 核心服務部署在 Kubernetes 叢集上，利用其強大的容器編排能力。
*   **Horizontal Pod Autoscaler (HPA)**：根據 CPU 利用率、記憶體使用量或自定義指標自動擴展 NDH Pod 數量，以應對負載變化。
*   **分區策略 (Sharding by Factory/Area)**：NDH 的數據和處理負載可以根據工廠 ID 或地理區域進行分區，確保數據的局部性和高效處理。

#### 7.1.2. MCP 叢集

*   **無狀態設計**：MCP 的服務設計為無狀態，方便水平擴展和故障恢復。
*   **Load Balancer 分流**：透過負載均衡器將請求均勻分發到 MCP 叢集中的多個實例。
*   **Agent 親和性路由**：在某些情況下，可以實現 Agent 親和性路由，確保特定 Agent 的請求始終由同一 MCP 實例處理，以優化性能或保持會話狀態。

### 7.2. 資料分層

為了優化數據存儲成本和查詢性能，NDH 實施了多層次的數據存儲策略：

*   **Hot Data (最近1天)**：
    *   **存儲介質**：使用 Redis 或 In-Memory Cache 進行存儲。
    *   **查詢特性**：提供次秒級 (sub-second) 的極速查詢響應，適用於實時監控和控制。
*   **Warm Data (1天-30天)**：
    *   **存儲介質**：採用專為時序數據設計的數據庫，如 TimescaleDB 或 InfluxDB。
    *   **查詢特性**：提供秒級的查詢響應，適用於短期歷史數據分析和趨勢分析。
*   **Cold Data (30天以上)**：
    *   **存儲介質**：歸檔到成本效益高的對象存儲服務，如 Amazon S3 或其他兼容 S3 的對象存儲。
    *   **查詢特性**：主要用於長期歸檔、合規性要求和離線大數據分析，查詢延遲較高但成本極低。

### 7.3. 事件流優化

NDH 採用 Apache Kafka 作為核心事件骨幹，確保事件處理的高吞吐量、低延遲和高可靠性：

*   **Apache Kafka 作為事件骨幹**：所有來自 Asset Tag Instance 的實時數據和系統事件都透過 Kafka 進行傳輸和持久化。
*   **分區策略**：根據 Asset Instance ID 或地理位置對 Kafka Topic 進行分區，確保相關事件在同一分區內有序處理，提高並行處理能力。
*   **批次處理**：數據在寫入數據庫或進行其他處理之前，會進行批次積累，減少 I/O 操作次數，提高寫入效率。
*   **背壓機制**：實施流量控制和背壓機制，防止上游數據源產生過多數據導致下游系統過載，確保系統的穩定性。

### 7.4. 效能基準測試

為了驗證和保障 NDH/MCP 系統的性能，將進行嚴格的效能基準測試，並設定明確的目標指標。以下是一個模擬 Agent 負載測試的腳本範例和目標指標：

```python
# 效能測試腳本 (Locust)
import asyncio
from locust import User, task, between

class AgentLoadTest(User):
    wait_time = between(0.1, 1)
    
    @task
    def subscribe_events(self):
        # 模擬 Agent 訂閱事件
        self.client.get("/mcp/events/subscribe?filter=temperature>80")
    
    @task(3)
    def publish_action(self):
        # 模擬 Agent 發布動作
        self.client.post("/mcp/actions", json={
            "type": "adjust_setpoint",
            "target": "valve-123",
            "value": 75.5
        })
```

**目標指標**：

*   **P99 延遲**：同廠查詢/事件 < 100ms
*   **吞吐量**：> 10,000 operations/second
*   **CPU 使用率**：< 70%

### 7.5. NDH 的叢集架構以應對大規模部署

在大型工業場景中，Asset Instance 和 Asset Tag Instance 的數量可能達到百萬級別。為了處理如此龐大的數據量和高併發的數據流，NDH 採用了**分散式叢集架構**。這包括：

*   **數據採集層的分散式部署**：多個 Data Acquisition 節點並行採集數據。
*   **NDH Data Hub 的可擴展性**：利用 Kafka、TSDB (如 TDengine)、Postgres 等分散式數據技術，確保數據的吞吐量和儲存能力。
*   **Asset Tag Instance 的分散式管理**：Asset Tag Instance 可以分散部署在多個節點上，實現負載均衡和高可用性。
*   **MCP Control Plane 的叢集化**：確保 AI Agent 的管理和排程在高負載下依然穩定可靠。

## 8. 結論

NDH 透過 FDL 和 IADL 的精確定義，結合 Asset Instance 和 Asset Tag Instance 的概念，構建了一個強大、靈活且可擴展的數位分身資產管理框架。其與 Omniverse USD Scene Graph 的深度整合，以及為大規模部署設計的叢集架構，共同為實現工業數位化轉型和智能製造提供了堅實的基礎。


## 9. 非功能需求 (Non-Functional Requirements, NFR) 與決策門檻

本概述所定義的 NDH 資產管理機制需滿足以下非功能需求：

*   **延遲 (Latency)**：同廠查詢/事件 p95 < 300ms；跨廠/雲邊 p95 < 800ms。
*   **吞吐 (Throughput)**：≥ 100k tags / 1k EPS（每廠），可線性擴張。
*   **可用性 (Availability)**：NDH 控制面 99.9%，告警通道 99.99%。
*   **一致性 (Consistency)**：事件重放恢復狀態一致（需定義 3 條測例）。
*   **安全 (Security)**：高風險命令「人機共治 + 限幅/限頻 + 全鏈路審計」。
*   **可維運 (Maintainability)**：SLO 儀表（Latency/Traffic/Errors/Saturation）；事故 Runbook（角色/時限/動作）。

