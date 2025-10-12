# 競品分析研究筆記

## 1. Unified Namespace (UNS) - HiveMQ

**來源**: https://www.hivemq.com/mqtt/unified-namespace-uns-essentials-iiot-industry-40/

### 核心概念

Unified Namespace (UNS) 是一種新穎的解決方案,允許從各種工業物聯網 (IIoT) 系統收集資料,為其添加上下文,並將其轉換為其他系統可以理解的格式。

### 主要特點

1. **統一的資料命名空間**: 標準化的方式來組織和命名資料
2. **基於 MQTT 和 Sparkplug**: 使用 MQTT 作為通訊協定,Sparkplug 作為資料規範
3. **語義資料層次結構**: 提供有意義的資料組織方式
4. **事件驅動架構**: 發布-訂閱模式

### 整合能力

HiveMQ UNS 支援整合:
- SCADA 系統
- Historian (歷史資料庫)
- MES (製造執行系統)
- ERP (企業資源規劃)
- 進階分析系統

### 技術架構

- **核心**: MQTT Broker (HiveMQ)
- **資料模型**: MQTT Sparkplug
- **部署**: 邊緣到雲端
- **擴展性**: 高度可擴展

### 與 NDH 的相似性

1. ✅ 都是資料中樞概念
2. ✅ 都支援多種工業系統整合
3. ✅ 都使用事件驅動架構
4. ✅ 都支援 MQTT/Sparkplug

### 與 NDH 的差異

1. UNS 主要基於 MQTT,NDH 支援多種協定
2. UNS 專注於命名空間,NDH 提供更廣泛的資料管理
3. HiveMQ 是商業產品,NDH 是開源專案

---




## 2. Kepware KEPServerEX - PTC

**來源**: https://www.ptc.com/en/products/kepware/kepserverex

### 核心概念

KEPServerEX 是值得信賴的工業連接平台,可將幾乎任何設備的專有資料轉換為標準化、應用程式就緒的資料。在幾分鐘內橋接 OT 和 IT,無需硬體升級或臃腫的軟體。

### 主要特點

1. **設備連接**: 超過 150 個預建設備驅動程式
2. **OT 資料聚合**: 可靠、可擴展的工業連接平台
3. **資料轉換**: 將原始專有資料轉換為可操作的洞察
4. **安全的 OT-IT 整合**: 強大的協定轉換
5. **靈活定價**: 按需購買驅動程式

### 技術架構

- **平台**: Microsoft Windows
- **核心**: OPC Server
- **驅動程式**: 150+ 設備驅動程式 (à la carte)
- **協定**: OPC UA, OPC DA, MQTT, REST API
- **部署**: 本地部署

### 產品線

1. **KEPServerEX**: Windows 平台,小型專案
2. **Kepware+**: 企業級,集中管理
3. **Kepware Edge**: Linux 平台,容器化部署

### 使用案例

- AI & ML
- Industrial IoT
- Automation
- M2M Communication
- MES, ERP, SCADA, Historians 整合

### 定價模式

- **Manufacturing Suite**: 100+ 驅動程式套裝
- **À la carte**: 單獨購買所需驅動程式
- **Demo**: 2 小時免費試用,無限次重啟

### 與 NDH 的相似性

1. ✅ 都是工業連接平台
2. ✅ 都支援多種設備和協定
3. ✅ 都提供資料轉換功能
4. ✅ 都支援 OPC UA

### 與 NDH 的差異

1. **商業模式**: KEPServerEX 是商業產品,NDH 是開源
2. **平台**: KEPServerEX 主要在 Windows,NDH 跨平台
3. **焦點**: KEPServerEX 專注於設備連接,NDH 是資料中樞
4. **定價**: KEPServerEX 按驅動程式收費,NDH 免費
5. **架構**: KEPServerEX 是 OPC Server,NDH 是 Broker 架構

---




## 3. Ignition - Inductive Automation

**來源**: https://inductiveautomation.com/ignition/

### 核心概念

Ignition 是一個無限制的工業平台,用於企業整合、工業應用程式等。作為伺服器軟體,它充當工廠車間所有設備的中心樞紐,實現完整的系統整合。

### 主要特點

1. **無限制授權**: 一個伺服器授權,無限 Tags、無限用戶、無限客戶端
2. **全面整合**: SCADA, IIoT, MES 等多種工業應用
3. **跨平台**: 基於 SQL, Python, OPC UA, MQTT 等開放技術
4. **Web 部署**: 伺服器為中心的 Web 部署模型
5. **快速安裝**: 3 分鐘安裝完成

### 技術架構

- **核心**: 伺服器軟體 (Java 基礎)
- **協定**: OPC UA, MQTT, SQL, REST API
- **開發**: Ignition Designer (圖形化開發環境)
- **部署**: Web-based, 支援桌面、工業顯示器、行動裝置
- **模組化**: 軟體模組系統

### 三大核心能力

1. **Connect**: 連接所有設備和資料庫
2. **Design**: 建立任何類型的工業應用
3. **Deploy**: Web 部署到任何裝置

### 定價模式

- **單一伺服器授權**: 固定價格
- **無限制**: Tags, 用戶, 客戶端, 設計器
- **免費試用**: 無時間限制的試用版

### 使用案例

- SCADA 系統
- MES (製造執行系統)
- IIoT 平台
- 數位轉型專案
- 跨行業應用 (食品飲料、製造、能源等)

### 客戶案例

- Sierra Nevada Brewing Co.
- Chobani Greek Yogurt
- 全球各行業

### 與 NDH 的相似性

1. ✅ 都是工業整合平台
2. ✅ 都支援 OPC UA 和 MQTT
3. ✅ 都提供中心化資料管理
4. ✅ 都支援跨平台

### 與 NDH 的差異

1. **定位**: Ignition 是 SCADA/MES 平台,NDH 是資料中樞
2. **授權**: Ignition 是商業授權,NDH 是開源
3. **焦點**: Ignition 專注於應用開發,NDH 專注於資料整合
4. **UI**: Ignition 提供圖形化設計器,NDH 是程式化配置
5. **價格**: Ignition 單一伺服器授權,NDH 免費

---




## 4. HighByte Intelligence Hub

**來源**: https://www.highbyte.com/intelligence-hub

### 核心概念

HighByte Intelligence Hub 是專為工業資料建模、編排和治理而設計的 Industrial DataOps 軟體解決方案。在邊緣部署,存取、建模、轉換和準備工廠車間資料以供雲端分析。

### 主要特點

1. **資料建模 (Data Modeling)**: 
   - 用智慧資料模型表示機器、產品、流程和系統
   - 將數千個工業資料點與其他系統資訊合併,添加元資料
   - 模板化輸入和實例,快速建模數百個常見資產

2. **資料編排 (Data Orchestration)**:
   - 在連接之間建立資料流 (間隔、事件或可呼叫)
   - Store-and-forward 緩衝機制
   - Pipeline Debug 比較有效負載、統計和狀態

3. **邊緣部署 (Edge Deployment)**:
   - 支援輕量級硬體平台 (單板電腦、工業交換機、IoT 閘道)
   - Docker 映像部署

4. **Industrial AI**:
   - 透過 Industrial MCP Server 存取情境化工業資料
   - 支援 AI 代理工作流程
   - 整合 Amazon Bedrock, Azure AI, Google Gemini, OpenAI

5. **MQTT Broker**:
   - 嵌入式 MQTT broker (v3.1.1 和 v5)
   - 支援 JSON 和 Sparkplug 有效負載
   - 快速建立本地 Unified Namespace (UNS)

6. **Namespaces**:
   - 視覺化組織資料集及其關係
   - 設計和治理 Unified Namespace (UNS)
   - 將資料源和建模資料集編目到邏輯層次結構

7. **REST Data Server**:
   - 作為工業資料的 API 閘道
   - 安全地公開連接、模型、實例和管道
   - "Industrial Data API Builder"

### 技術架構

- **核心**: Industrial DataOps 平台
- **部署**: 邊緣 (Edge-native)
- **介面**: No-code / Low-code
- **協定**: MQTT, REST API, OPC UA, Modbus, 等
- **資料格式**: JSON, Sparkplug
- **容器化**: Docker 支援

### 定價模式

- **免費試用**: 2 小時執行時間限制
- **評估版**: $4,500
- **專業授權**: $17,500 起
- **企業選項**: 需報價

### 使用案例

- Industry 4.0 數位轉型
- Unified Namespace (UNS) 建置
- 工業資料 API
- AI/ML 資料準備
- 邊緣資料處理

### 與 NDH 的相似性

1. ✅ 都是工業資料中樞
2. ✅ 都支援資料建模和轉換
3. ✅ 都支援 MQTT 和 Sparkplug
4. ✅ 都提供 REST API
5. ✅ 都支援 Unified Namespace

### 與 NDH 的差異

1. **商業模式**: HighByte 是商業產品 ($17,500+),NDH 是開源
2. **焦點**: HighByte 專注於 DataOps,NDH 是通用資料中樞
3. **介面**: HighByte 提供 No-code UI,NDH 是程式化配置
4. **AI 整合**: HighByte 內建 AI/MCP 支援,NDH 需要額外整合
5. **部署**: HighByte 專注於邊緣,NDH 更靈活

### HighByte 的獨特優勢

- **Industrial DataOps**: 專門為工業資料設計的 DataOps 方法論
- **No-code**: 圖形化介面,降低技術門檻
- **MCP Server**: 原生支援 AI 代理工作流程
- **Namespaces**: 視覺化 UNS 設計和治理

---




## 5. ThingWorx IIoT Platform - PTC

**來源**: https://www.ptc.com/en/products/thingworx

### 核心概念

ThingWorx 是一個完整的工業物聯網 (IIoT) 平台,專為工業環境設計。它將來自產品、人員和流程的資料無縫連接,將原始資訊轉換為強大的可操作智慧。

### 主要特點

1. **連接 (Connect)**: 快速連接工業資產,直接從設備、應用程式和企業系統獲取資料
2. **建立 (Build)**: 使用預建應用程式和開發工具,從試點到企業級解決方案
3. **分析 (Analyze)**: AI 驅動的分析和洞察
4. **管理 (Manage)**: 端到端管理能力

### 業務價值

- **最大化收入**: 提高營運效率
- **加速創新**: 快速實施 IIoT 解決方案
- **降低成本**: 減少停機時間和維護成本
- **提高品質**: 改善產品和流程品質
- **最大化靈活性**: 適應不同的使用案例

### 技術架構

- **平台**: 完整的 IIoT 平台
- **AI 整合**: ThingWorx 10 重新定義 AI 時代的智慧製造
- **應用程式**: 預建的企業應用程式
- **開發工具**: 完整的開發環境
- **社群**: ThingWorx Community (教學、指南、影片)

### 解決方案領域

1. **製造 (Manufacturing)**: 連接操作員和工程師與工廠車間資產
2. **服務 (Service)**: 連接服務團隊與現場資產和資料
3. **工程 (Engineering)**: 基於角色的應用程式,連接企業系統
4. **問題解決**: 識別、優先排序和監控問題解決機會

### 市場地位

- **領導者**: 在全球 IIoT 市場被認可為技術領導者
- **完整平台**: 提供端到端的 IIoT 能力
- **成熟產品**: 多年的創新和發展

### 與 NDH 的相似性

1. ✅ 都是工業資料平台
2. ✅ 都支援資料連接和整合
3. ✅ 都提供分析能力

### 與 NDH 的差異

1. **定位**: ThingWorx 是完整的 IIoT 平台,NDH 是資料中樞
2. **商業模式**: ThingWorx 是高價商業產品,NDH 是開源
3. **焦點**: ThingWorx 涵蓋製造、服務、工程全流程,NDH 專注於資料整合
4. **AI**: ThingWorx 內建 AI 能力,NDH 需要額外整合
5. **應用程式**: ThingWorx 提供預建應用程式,NDH 是基礎設施
6. **目標市場**: ThingWorx 面向大型企業,NDH 更靈活

---

## 6. AWS IoT SiteWise

**來源**: 搜尋結果

### 核心概念

AWS IoT SiteWise 是一個託管服務,可輕鬆大規模收集、儲存、組織和監控來自工業設備的資料。

### 主要特點

1. **託管服務**: 完全託管的雲端服務
2. **大規模**: 支援大規模工業設備資料收集
3. **資料建模**: 建模、分析和視覺化工業設備資料
4. **MQTT 整合**: 透過 MQTT 訊息發送資料到資產屬性

### 技術架構

- **雲端**: AWS 雲端服務
- **協定**: MQTT, OPC UA
- **整合**: AWS IoT Core, AWS IoT Analytics
- **儲存**: AWS 雲端儲存

### 與 NDH 的相似性

1. ✅ 都是工業資料管理平台
2. ✅ 都支援 MQTT 和 OPC UA

### 與 NDH 的差異

1. **部署**: AWS IoT SiteWise 是雲端服務,NDH 可本地部署
2. **商業模式**: AWS 按使用量計費,NDH 開源免費
3. **供應商鎖定**: AWS 綁定 AWS 生態系統,NDH 開放

---

## 7. Azure Industrial IoT

**來源**: 搜尋結果

### 核心概念

Azure Industrial IoT 是 Microsoft 的工業物聯網解決方案,包括 OPC Publisher 和可選的 Azure Industrial IoT Platform 配套 Web 服務。

### 主要特點

1. **OPC UA 支援**: 發現和操作 OPC UA 啟用的工業資產
2. **Azure IoT Operations**: 工業轉型加速器
3. **自適應雲端**: Azure 的自適應雲端方法
4. **市場地位**: 2025 Gartner Magic Quadrant 領導者

### 技術架構

- **雲端**: Azure 雲端服務
- **協定**: OPC UA (IEC 62541)
- **整合**: Azure IoT Hub, Azure IoT Analytics
- **開源**: GitHub 上的開源專案

### 與 NDH 的相似性

1. ✅ 都支援 OPC UA
2. ✅ 都有開源元件

### 與 NDH 的差異

1. **部署**: Azure 是雲端服務,NDH 可本地部署
2. **商業模式**: Azure 按使用量計費,NDH 開源免費
3. **供應商鎖定**: Azure 綁定 Microsoft 生態系統,NDH 開放

---


