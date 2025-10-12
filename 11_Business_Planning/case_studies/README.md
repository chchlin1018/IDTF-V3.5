# Harvatek LED 封裝廠 Digital Twins 系統設計

**基於 IDTF V3.1 架構**

## 專案概述

本專案為 Harvatek Corporation LED 封裝廠設計完整的 Digital Twins 系統整合方案,應用 IDTF V3.1 (Industrial Digital Twin Framework) 架構和 IADL V3.1 (Industrial Asset Definition Language) 框架,整合 OT/IT 數據,實現智能製造和數位轉型。

### 廠房規模

- **建築**: 1 棟,6 層樓
- **生產設備**: 340 台
- **設施設備**: 1,240+ 台
- **感測器**: 1,600+ 點
- **總資產**: 2,000+ 項
- **總資產價值**: USD 150M

### 系統架構

- **OT 整合**: SECS/GEM, OPC UA, Modbus TCP, BACnet, SNMP
- **IT 整合**: SAP ERP, MES, PLM, WMS, QMS
- **數據中樞**: Kafka + InfluxDB + PostgreSQL (NDH V3.1)
- **視覺化**: NVIDIA Omniverse
- **數據量**: 109M+ points/day

## 文件結構

### 1. 系統設計總覽
**檔案**: `Harvatek_Digital_Twin_System_Design.md`

初步的系統設計文件,包含:
- 公司背景和廠房概況
- IDTF V3.1 整體架構設計
- IADL 資產定義範例
- NDH 數據整合方案架構
- 實施路徑圖
- ROI 分析

### 2. LED 封裝設備 IADL 定義
**檔案**: `harvatek_led_equipment_iadl_definitions.md` (93 KB)

詳細的 LED 封裝設備 IADL V3.1 定義,包含:

**設備類型** (8 種):
1. **Die Sorter** (晶片分選機) - 12,000 UPH
2. **Die Bonder** (固晶機) - 8,000 UPH
3. **Wire Bonder** (打線機) - 10,000 UPH
4. **Dispenser** (點膠機) - 6,000 UPH
5. **Tester** (測試機) - 15,000 UPH
6. **Taping Machine** (編帶包裝機) - 12,000 UPH
7. **AOI System** (自動光學檢測) - 8,000 UPH
8. **X-Ray Inspection** (X光檢測) - 3,000 UPH

**每個設備定義包含**:
- 📐 3D 模型整合 (OpenUSD 格式,含 LOD)
- 🔌 連接點定義 (電力、氣體、管線)
- 📊 OT 數據標籤 (SECS/GEM, OPC UA)
- 🤖 行為模型和預測性維護
- 🎨 Omniverse 視覺化整合
- 🔧 維護程序和備品管理
- 🏭 MES/ERP 整合
- ✅ 品質管理和校正

### 3. 廠房完整資產模型
**檔案**: `harvatek_factory_asset_model.md` (61 KB)

完整的廠房資產模型,包含:

**建築結構**:
- 建築物主體定義
- 6 個樓層詳細定義 (B1, 1F-5F)
- 2F 和 3F 生產區域劃分
- 環境參數設定

**生產設備**:
- 340 台設備清單和佈局
- 設備座標定義
- 3 條生產線配置
- 產能統計

**設施系統**:
- HVAC 系統 (Chiller, AHU, FFU)
- 純水系統 (DI Water)
- 氮氣系統 (Nitrogen)
- 壓縮空氣系統 (CDA)
- 電力系統 (Transformer, UPS, Switchgear)

**感測器網路**:
- 環境感測器 (溫度、濕度、壓差、潔淨度)
- 能源感測器 (電力、水、氣體)
- 設備感測器 (振動、溫度、電流)
- 安全感測器 (煙霧、氣體、門禁)

**資產統計**:
- 總資產: 2,000+ 項
- 總價值: USD 150M
- 年能耗: 35,000 MWh
- 年能源成本: USD 10.5M

### 4. NDH 資料整合方案
**檔案**: `harvatek_ndh_data_integration.md` (114 KB)

完整的 NDH (Neutral Data Hub) 資料整合方案,包含:

**核心組件**:
- **Kafka Cluster**: 3 Brokers, 100,000+ events/sec
- **InfluxDB Cluster**: 2 節點,時序數據儲存
- **PostgreSQL Cluster**: Primary + Replica,關聯數據管理

**數據源整合**:
- **SECS/GEM**: 340 台設備 → 88.1M points/day
- **OPC UA**: 200 個電力計 → 8.6M points/day
- **Modbus TCP**: 150 點公用系統 → 1.3M points/day
- **BACnet**: 160 台 HVAC + 1,100 感測器 → 10.4M points/day
- **SNMP**: 5 台 UPS → 0.3M points/day
- **SAP ERP**: 生產訂單、物料、庫存、維護工單
- **MES**: 生產批次、設備狀態、品質數據、配方

**Kafka 數據流**:
- 10+ Kafka Topics
- Kafka Streams 實時處理
- 事件驅動架構

**數據庫設計**:
- **InfluxDB**: 6 個 Measurements,4 個 Retention Policies
- **PostgreSQL**: 15+ Tables (資產、維護、生產、品質、告警)

**Connector 配置**:
- SECS/GEM Connector (Python 實現)
- OPC UA Connector (Python 實現)
- Modbus TCP Connector (Python 實現)
- BACnet Connector (Python 實現)

**實時數據處理**:
- Kafka Streams 應用 (OEE 計算、能源聚合、告警檢測)
- 實時數據處理服務
- 告警規則引擎
- 告警通知服務

**RESTful API**:
- 設備 API
- 設施 API
- 環境 API
- 告警 API
- 生產 API
- 分析 API

**Kubernetes 部署**:
- 完整的容器化部署配置
- Prometheus + Grafana 監控

### 5. 實施路徑圖與 ROI 分析
**檔案**: `harvatek_implementation_roadmap_roi.md` (31 KB)

詳細的實施計畫和投資回報分析,包含:

**實施路徑圖** (18 個月,6 個階段):
1. **階段 1**: 基礎建設 (Month 1-3) - USD 1.55M
2. **階段 2**: 試點導入 (Month 4-6) - USD 0.80M
3. **階段 3**: 2F 全面導入 (Month 7-9) - USD 1.15M
4. **階段 4**: 3F 全面導入 (Month 10-12) - USD 1.50M
5. **階段 5**: 全廠整合 (Month 13-15) - USD 1.50M
6. **階段 6**: 優化與擴展 (Month 16-18) - USD 0.50M

**投資成本**:
- **CAPEX**: USD 6.5M
- **年度 OPEX**: USD 2.0M

**年度效益** (USD 14.6M):
- 生產效率提升: USD 6.65M (OEE +10%)
- 維護成本降低: USD 1.87M (故障率 -60%)
- 能源成本降低: USD 1.58M (能耗 -15%)
- 品質提升: USD 2.50M (良率 +5%)
- 庫存優化: USD 1.00M (庫存 -25%)
- 人力成本優化: USD 1.00M (減少 20 人)

**ROI 分析**:
- **簡單回收期**: 6.2 個月
- **折現回收期**: 6.8 個月
- **5 年 NPV**: USD 41.2M
- **IRR**: 193.8%
- **BCR**: 7.34

**風險評估**:
- 技術風險
- 專案管理風險
- 組織變革風險
- 安全風險

**成功關鍵因素**:
- 高層支持
- 跨部門協作
- 技術能力
- 變革管理
- 品質保證

## 技術架構

### IDTF V3.1 架構層次

```
┌─────────────────────────────────────────────────────────────┐
│              應用層 (Application Layer)                      │
│  • 設備監控  • 預測性維護  • 能源管理  • 生產優化           │
│  • 品質管理  • 決策支援    • VR/AR 應用                      │
└─────────────────────────────────────────────────────────────┘
                              ▲
                              │
┌─────────────────────────────────────────────────────────────┐
│           視覺化層 (Visualization Layer)                     │
│              NVIDIA Omniverse Platform                       │
│  • 3D 場景渲染  • 即時數據綁定  • 物理模擬  • 協作平台      │
└─────────────────────────────────────────────────────────────┘
                              ▲
                              │
┌─────────────────────────────────────────────────────────────┐
│            服務層 (Service Layer)                            │
│  • RESTful API  • GraphQL API  • WebSocket  • gRPC          │
└─────────────────────────────────────────────────────────────┘
                              ▲
                              │
┌─────────────────────────────────────────────────────────────┐
│          數據處理層 (Data Processing Layer)                  │
│  • Kafka Streams  • Spark  • Flink  • AI/ML Models         │
└─────────────────────────────────────────────────────────────┘
                              ▲
                              │
┌─────────────────────────────────────────────────────────────┐
│         數據中樞層 (NDH - Neutral Data Hub)                  │
│  • Kafka (Event Streaming)                                  │
│  • InfluxDB (Time Series)                                   │
│  • PostgreSQL (Relational)                                  │
└─────────────────────────────────────────────────────────────┘
                              ▲
                              │
┌─────────────────────────────────────────────────────────────┐
│          數據採集層 (Data Acquisition Layer)                 │
│  • SECS/GEM Connector  • OPC UA Connector                   │
│  • Modbus Connector    • BACnet Connector                   │
│  • SNMP Connector      • REST API Connector                 │
└─────────────────────────────────────────────────────────────┘
                              ▲
                              │
┌─────────────────────────────────────────────────────────────┐
│            資產層 (Asset Layer)                              │
│  • 生產設備 (340 台)  • 設施系統 (1,240+ 台)                │
│  • 感測器 (1,600+ 點)  • IT 系統 (ERP, MES, PLM...)         │
└─────────────────────────────────────────────────────────────┘
```

## 核心技術

- **IDTF V3.1**: Industrial Digital Twin Framework
- **IADL V3.1**: Industrial Asset Definition Language
- **OpenUSD**: Universal Scene Description (3D 資產格式)
- **NVIDIA Omniverse**: 3D 協作和模擬平台
- **Apache Kafka**: 分散式事件串流平台
- **InfluxDB**: 時序數據庫
- **PostgreSQL**: 關聯式數據庫
- **Kubernetes**: 容器編排平台
- **Python**: 主要開發語言
- **FastAPI**: RESTful API 框架

## 預期效益

### 量化效益

| 效益項目 | 年效益 (USD) | 提升幅度 |
|---------|-------------|---------|
| 生產效率提升 | 6,650,000 | OEE +10% |
| 維護成本降低 | 1,870,000 | 故障率 -60% |
| 能源成本降低 | 1,575,000 | 能耗 -15% |
| 品質提升 | 2,500,000 | 良率 +5% |
| 庫存優化 | 1,000,000 | 庫存 -25% |
| 人力成本優化 | 1,000,000 | 減少 20 人 |
| **年度總效益** | **14,595,000** | - |

### 質化效益

- **決策支援**: 即時可視化,數據驅動決策
- **競爭優勢**: 成為產業標竿,提升客戶信心
- **永續發展**: 碳排放降低,ESG 評級提升
- **知識管理**: 知識累積,經驗傳承

### ROI 指標

| 指標 | 數值 | 評價 |
|------|------|------|
| 簡單回收期 | 6.2 個月 | 極佳 |
| 折現回收期 | 6.8 個月 | 極佳 |
| 5 年 NPV | USD 41.2M | 極佳 |
| IRR | 193.8% | 極佳 |
| BCR | 7.34 | 極佳 |

## 專案資訊

- **專案名稱**: Harvatek LED 封裝廠 Digital Twins 系統
- **客戶**: Harvatek Corporation (宏齊科技)
- **地點**: 新竹科學園區
- **期程**: 18 個月
- **預算**: USD 6.5M
- **預期效益**: USD 14.6M/年
- **投資回收期**: 6.2 個月

## 版本資訊

- **版本**: 1.0
- **日期**: 2025-10-12
- **作者**: IDTF V3.1 Team
- **狀態**: 設計完成,待核准

## 授權

本專案文件為 Harvatek Corporation 專有,未經授權不得複製或散佈。

## 聯絡資訊

如有任何問題或建議,請聯繫:

**專案負責人**: Michael Lin  
**Email**: michael.lin@harvatek.com  
**電話**: +886-3-578-xxxx

---

**© 2025 Harvatek Corporation. All Rights Reserved.**

