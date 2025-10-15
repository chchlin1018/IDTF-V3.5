# 主流工廠總線參考連接器規範

**版本**: 1.0
**日期**: 2025-10-15
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)

---

## 1. 概述

本文件旨在定義 IDTF (Industrial Digital Twin Framework) 中 NDH Connector Agent 針對主流工廠總線協議的參考連接器 (Reference Connector) 規範。參考連接器是可運行的概念驗證 (Proof of Concept, PoC) 實作,展示如何將 NDH 與常見的工業通訊協議和架構整合。這些參考連接器將涵蓋 OPC UA、ISA-95 Companion Specification、MQTT Sparkplug B 和 Unified Namespace (UNS) 等主流工廠總線,為開發者提供實際的整合範例和最佳實踐。

## 2. 參考連接器架構原則

所有參考連接器應遵循以下架構原則:

*   **統一 API 介面**:所有參考連接器必須實作 NDH Connector Agent 的標準 gRPC API 介面,確保與 NDH 的無縫整合。
*   **零配置部署**:盡可能實現自動發現和配置,減少手動設置的複雜度。
*   **只讀訪問**:預設情況下,連接器應僅從外部系統讀取數據,不應向外部系統寫入數據,以確保數據安全性。
*   **數據映射與轉換**:連接器應負責將外部協議的數據格式轉換為 NDH 的標準數據模型 (基於 IADL/FDL)。
*   **錯誤處理與重連**:連接器應具備健壯的錯誤處理機制和自動重連能力,確保數據流的穩定性。
*   **可擴展性**:連接器的設計應支援水平擴展,以處理大規模數據流。
*   **可觀測性**:連接器應提供豐富的日誌、指標和追蹤資訊,便於監控和故障排除。

## 3. OPC UA 參考連接器

### 3.1 概述

OPC UA (OPC Unified Architecture) 是工業自動化領域最廣泛採用的通訊協議之一,提供了平台無關、安全、可靠的數據交換機制。OPC UA 參考連接器旨在展示如何從 OPC UA 伺服器讀取數據並映射到 NDH 的資產模型。

### 3.2 功能特性

*   **伺服器發現**:支援透過 OPC UA Discovery Service 自動發現網路中的 OPC UA 伺服器。
*   **節點瀏覽**:能夠瀏覽 OPC UA 伺服器的地址空間 (Address Space),識別可用的節點 (Nodes)。
*   **訂閱與監控**:支援訂閱 OPC UA 節點的數據變化,並透過 MonitoredItem 機制接收即時更新。
*   **歷史數據讀取**:支援讀取 OPC UA 伺服器中的歷史數據。
*   **數據類型映射**:將 OPC UA 的數據類型 (如 Int32, Double, String, DateTime) 映射到 NDH 的標準數據類型。
*   **安全連接**:支援 OPC UA 的安全模式 (如 SignAndEncrypt),確保數據傳輸的機密性和完整性。

### 3.3 數據映射範例

| OPC UA 概念 | NDH 對映 (IADL/FDL) | 說明 |
|:---|:---|:---|
| OPC UA Server | FDL Site 或 Area | 一個 OPC UA 伺服器通常代表一個物理站點或區域。 |
| OPC UA Object (設備) | IADL AssetType / FDL AssetInstance | OPC UA 地址空間中代表設備的 Object 節點映射到 NDH 的資產實例。 |
| OPC UA Variable (數據點) | AssetInstance 的 Telemetry 或 Attribute | OPC UA Variable 節點映射到資產實例的遙測數據或屬性。 |
| OPC UA Method | AssetInstance 的 Command | OPC UA Method 可映射到資產實例的可執行命令 (如果支援寫入)。 |

### 3.4 實作技術

*   **程式語言**:Python (使用 `opcua` 或 `asyncua` 函式庫) 或 C# (使用 OPC Foundation 的 .NET SDK)。
*   **gRPC 框架**:使用標準的 gRPC 函式庫實作 NDH Connector Agent API。

## 4. ISA-95 Companion Specification 參考連接器

### 4.1 概述

ISA-95 Companion Specification for OPC UA 定義了如何在 OPC UA 地址空間中表示 ISA-95 的資訊模型 (如設備階層、生產訂單、物料定義等)。此參考連接器展示如何從符合 ISA-95 Companion Specification 的 OPC UA 伺服器讀取數據,並映射到 NDH 的 IADL/FDL 模型。

### 4.2 功能特性

*   **ISA-95 模型識別**:能夠識別 OPC UA 地址空間中符合 ISA-95 Companion Specification 的節點類型 (如 EquipmentType, ProcessCellType)。
*   **階層映射**:自動將 ISA-95 的設備階層 (Enterprise, Site, Area, ProcessCell, Unit, Equipment) 映射到 FDL 的對應階層。
*   **屬性映射**:將 ISA-95 定義的屬性 (如 ID, Description, Manufacturer) 映射到 IADL/FDL 的屬性。
*   **生產訂單與物料整合**:支援讀取生產訂單 (ProductionOrder) 和物料定義 (MaterialDefinition),並將其關聯到 NDH 的生產管理模型。

### 4.3 數據映射範例

| ISA-95 Companion Specification 概念 | NDH 對映 (IADL/FDL) | 說明 |
|:---|:---|:---|
| EquipmentType | IADL AssetType | ISA-95 的設備類型映射到 IADL 的資產類型。 |
| Equipment Instance | FDL AssetInstance | ISA-95 的設備實例映射到 FDL 的資產實例。 |
| ProcessCellType | FDL ProcessCell | ISA-95 的生產單元類型映射到 FDL 的 ProcessCell。 |
| ProductionOrder | NDH 生產訂單實體 | ISA-95 的生產訂單映射到 NDH 的生產管理模型中的訂單實體。 |

### 4.4 實作技術

*   **程式語言**:Python 或 C#,基於 OPC UA 參考連接器擴展。
*   **ISA-95 模型解析**:需要解析 OPC UA 地址空間中的 ISA-95 特定節點類型和屬性。

## 5. MQTT Sparkplug B 參考連接器

### 5.1 概述

MQTT Sparkplug B 是一個基於 MQTT 的輕量級、高效的工業 IoT 協議,特別適合邊緣設備和感測器的數據傳輸。Sparkplug B 定義了標準的主題命名空間和有效載荷格式 (使用 Google Protocol Buffers)。此參考連接器展示如何從 MQTT Broker 訂閱 Sparkplug B 主題,並將數據映射到 NDH。

### 5.2 功能特性

*   **MQTT Broker 連接**:支援連接到標準的 MQTT Broker (如 Mosquitto, HiveMQ)。
*   **Sparkplug B 主題訂閱**:訂閱 Sparkplug B 定義的主題 (如 `spBv1.0/group_id/NBIRTH/edge_node_id`)。
*   **Protobuf 解析**:解析 Sparkplug B 的 Protobuf 有效載荷,提取設備和指標資訊。
*   **設備生命週期管理**:處理 Sparkplug B 的 BIRTH 和 DEATH 訊息,追蹤設備的上線和離線狀態。
*   **指標映射**:將 Sparkplug B 的 Metric 映射到 NDH 資產實例的遙測數據。

### 5.3 數據映射範例

| Sparkplug B 概念 | NDH 對映 (IADL/FDL) | 說明 |
|:---|:---|:---|
| Edge Node | FDL AssetInstance (邊緣設備) | Sparkplug B 的 Edge Node 映射到 NDH 的邊緣設備資產實例。 |
| Device | FDL AssetInstance (子設備) | Sparkplug B 的 Device 映射到 NDH 的子設備資產實例。 |
| Metric | AssetInstance 的 Telemetry | Sparkplug B 的 Metric 映射到資產實例的遙測數據點。 |
| NBIRTH / DBIRTH | 資產實例狀態變更為 Online | BIRTH 訊息觸發資產實例狀態更新為上線。 |
| NDEATH / DDEATH | 資產實例狀態變更為 Offline | DEATH 訊息觸發資產實例狀態更新為離線。 |

### 5.4 實作技術

*   **程式語言**:Python (使用 `paho-mqtt` 函式庫和 `protobuf` 函式庫)。
*   **Sparkplug B 函式庫**:可使用開源的 Sparkplug B 函式庫簡化實作。

## 6. Unified Namespace (UNS) 參考連接器

### 6.1 概述

Unified Namespace (UNS) 是一種架構模式,旨在透過建立一個集中的、基於主題的命名空間,打破工業數據孤島,實現數據的自由流動。UNS 通常基於 MQTT 實現,並採用階層式的主題結構 (如 `enterprise/site/area/line/machine/metric`)。此參考連接器展示如何從 UNS 訂閱數據,並映射到 NDH 的階層模型。

### 6.2 功能特性

*   **UNS 主題訂閱**:支援訂閱符合 UNS 命名規範的 MQTT 主題。
*   **階層解析**:從 UNS 主題結構中自動解析出企業、站點、區域、生產線、設備等階層資訊。
*   **動態資產發現**:根據 UNS 主題動態發現和創建 NDH 中的資產實例。
*   **數據映射**:將 UNS 主題中的數據映射到對應資產實例的遙測數據或屬性。
*   **靈活的主題模式**:支援配置不同的 UNS 主題命名模式,以適應不同的 UNS 實作。

### 6.3 數據映射範例

| UNS 主題結構 | NDH 對映 (IADL/FDL) | 說明 |
|:---|:---|:---|
| `enterprise/site/area/line/machine/temperature` | FDL AssetInstance (machine) 的 Telemetry (temperature) | UNS 主題映射到對應階層的資產實例及其遙測數據點。 |
| `enterprise/site` | FDL Site | UNS 主題的前綴部分映射到 FDL 的站點。 |
| `enterprise/site/area` | FDL Area | UNS 主題的前綴部分映射到 FDL 的區域。 |

### 6.4 實作技術

*   **程式語言**:Python (使用 `paho-mqtt` 函式庫)。
*   **主題解析**:需要實作邏輯來解析 UNS 主題結構,並動態創建或更新 NDH 中的資產階層。

## 7. 參考連接器開發與部署

### 7.1 開發環境

*   所有參考連接器應提供完整的原始碼,並託管在公開的程式碼儲存庫 (如 GitHub)。
*   應提供清晰的 README 文件,說明連接器的功能、配置和使用方法。
*   應包含單元測試和整合測試,確保連接器的品質。

### 7.2 部署方式

*   **容器化**:所有參考連接器應提供 Docker 映像,便於快速部署和測試。
*   **配置管理**:應支援透過環境變數或配置檔案進行配置,例如 MQTT Broker 地址、OPC UA 伺服器端點等。
*   **文檔**:應提供詳細的部署指南,包括如何在不同環境 (本地、雲端) 中部署和運行連接器。

## 8. 結論

這些主流工廠總線的參考連接器將為 IDTF 的使用者和開發者提供實際的整合範例,展示 NDH 如何與現有的工業通訊協議和架構無縫整合。透過這些可運行的概念驗證,將大幅降低 IDTF 的學習曲線和實施門檻,加速其在台灣高科技工廠中的應用和推廣。這些參考連接器也將作為開發自定義連接器的基礎和範本,促進 IDTF 生態系統的發展。
