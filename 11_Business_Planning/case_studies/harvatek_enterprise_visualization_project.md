# Harvatek Corporation Enterprise Visualization 專案

**基於 NVIDIA Omniverse 和 IDTF V3.1 架構**

**版本**: 1.0  
**日期**: 2025-10-12  
**專案類型**: Enterprise Visualization Platform

---

## 目錄

1. [專案概述](#1-專案概述)
2. [視覺化需求分析](#2-視覺化需求分析)
3. [技術架構設計](#3-技術架構設計)
4. [3D 視覺化平台](#4-3d-視覺化平台)
5. [即時數據視覺化](#5-即時數據視覺化)
6. [AR/VR 應用](#6-arvr-應用)
7. [決策支援系統](#7-決策支援系統)
8. [實施計畫](#8-實施計畫)
9. [投資與效益](#9-投資與效益)
10. [附錄](#10-附錄)

---

## 1. 專案概述

### 1.1 專案背景

Harvatek Corporation 作為全球領先的 LED 封裝製造商,擁有先進的生產設施和大量的 OT/IT 數據。為了充分發揮 Digital Twins 系統的價值,需要建立一個企業級視覺化平台,將複雜的數據轉化為直觀、可操作的視覺化資訊,支援各層級的決策制定。

### 1.2 專案目標

建立一個全方位的 Enterprise Visualization 平台,實現:

1. **3D 數位分身視覺化**:完整的工廠 3D 模型,即時反映實際運作狀態
2. **即時數據視覺化**:多維度、多層級的數據儀表板和分析工具
3. **沉浸式體驗**:AR/VR 應用支援遠程巡檢、培訓和協作
4. **決策支援**:高階主管駕駛艙,提供戰略級洞察
5. **協作平台**:跨部門、跨地域的視覺化協作環境

### 1.3 專案範圍

**視覺化對象**:
- 建築物:1 棟,6 層樓
- 生產設備:340 台
- 設施系統:1,240+ 台
- 感測器:1,600+ 點
- 數據流:每日 1.09 億筆數據點

**使用者群體**:
- 高階主管:10 人
- 中階管理:50 人
- 工程師:100 人
- 操作員:200 人
- 外部協作者:20 人

**視覺化類型**:
- 3D 即時視覺化
- 2D 儀表板和報表
- AR 擴增實境
- VR 虛擬實境
- 移動端應用

### 1.4 核心價值

1. **提升決策品質**:視覺化使複雜數據一目了然,支援快速、精準決策
2. **加速問題解決**:即時視覺化幫助快速識別和定位問題
3. **促進協作**:共享的視覺化環境促進跨部門協作
4. **降低培訓成本**:VR 培訓降低實體培訓成本和風險
5. **提升客戶信心**:展示先進的數位化能力,提升客戶信心

---

## 2. 視覺化需求分析

### 2.1 使用者角色與需求

#### 2.1.1 高階主管 (C-Level)

**主要需求**:
- 整體營運績效概覽
- 關鍵指標趨勢分析
- 異常告警和風險預警
- 跨廠區比較分析
- 戰略決策支援

**視覺化需求**:
- 高階主管駕駛艙 (Executive Dashboard)
- KPI 儀表板 (OEE, 良率, 能耗, 成本)
- 趨勢圖表和預測分析
- 3D 工廠全景視圖
- 移動端即時監控

**使用場景**:
- 每日早會查看營運狀況
- 月度/季度績效檢討
- 客戶參訪展示
- 投資決策評估

#### 2.1.2 廠務經理 (Plant Manager)

**主要需求**:
- 生產線即時狀態
- 設備效能監控
- 產能利用率分析
- 異常事件追蹤
- 資源調度優化

**視覺化需求**:
- 生產線 3D 視覺化
- 設備狀態熱圖
- 產能甘特圖
- 異常事件時間軸
- WIP 流動視覺化

**使用場景**:
- 每日生產調度
- 設備異常處理
- 產能平衡調整
- 生產會議報告

#### 2.1.3 維護工程師 (Maintenance Engineer)

**主要需求**:
- 設備健康度監控
- 維護工單管理
- 備品庫存狀態
- 維護歷史追蹤
- 故障診斷支援

**視覺化需求**:
- 設備 3D 模型和健康度
- 維護工單看板
- 備品庫存儀表板
- 故障樹分析圖
- AR 維護指引

**使用場景**:
- 預防性維護規劃
- 故障診斷和排除
- 備品採購決策
- 維護知識管理

#### 2.1.4 能源管理師 (Energy Manager)

**主要需求**:
- 能源消耗監控
- 能源成本分析
- 節能機會識別
- 碳排放追蹤
- 能源效率優化

**視覺化需求**:
- 能源消耗熱圖
- 成本分析圖表
- 能源流向桑基圖
- 碳排放儀表板
- 節能機會排行榜

**使用場景**:
- 每日能源監控
- 月度能源報告
- 節能專案評估
- ESG 報告製作

#### 2.1.5 品質工程師 (Quality Engineer)

**主要需求**:
- 品質指標監控
- 缺陷分析
- SPC 管制圖
- 品質追溯
- 改善專案追蹤

**視覺化需求**:
- 良率趨勢圖
- 柏拉圖 (Pareto Chart)
- SPC 管制圖
- 缺陷熱圖
- 品質追溯樹

**使用場景**:
- 每日品質監控
- 缺陷分析會議
- 客戶品質報告
- 改善專案管理

### 2.2 視覺化需求矩陣

| 使用者角色 | 3D 視覺化 | 2D 儀表板 | AR/VR | 移動端 | 協作功能 | 優先級 |
|-----------|----------|----------|-------|--------|---------|--------|
| 高階主管 | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | P0 |
| 廠務經理 | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | P0 |
| 維護工程師 | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | P1 |
| 能源管理師 | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐ | ⭐⭐⭐ | ⭐⭐ | P1 |
| 品質工程師 | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | P1 |
| 操作員 | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐ | P2 |

### 2.3 視覺化功能需求

#### 2.3.1 3D 視覺化需求

**基礎功能**:
- ✅ 完整工廠 3D 模型
- ✅ 多樓層切換和導航
- ✅ 設備 3D 模型和動畫
- ✅ 即時數據綁定和更新
- ✅ 視角控制 (自由視角、預設視角)
- ✅ 搜尋和定位功能

**進階功能**:
- ✅ 物理模擬 (流體、粒子)
- ✅ 熱圖疊加 (溫度、濕度、能耗)
- ✅ 數據流視覺化
- ✅ 歷史回放功能
- ✅ What-if 模擬
- ✅ 多人協作標註

**效能需求**:
- 幀率:≥ 60 FPS
- 載入時間:< 10 秒
- 數據延遲:< 5 秒
- 支援使用者:100+ 同時線上

#### 2.3.2 2D 儀表板需求

**儀表板類型**:
1. **高階主管駕駛艙**
2. **生產監控儀表板**
3. **設備效能儀表板**
4. **能源管理儀表板**
5. **品質管理儀表板**
6. **維護管理儀表板**
7. **告警管理儀表板**

**圖表類型**:
- 折線圖 (趨勢分析)
- 柱狀圖 (比較分析)
- 圓餅圖 (佔比分析)
- 散點圖 (相關性分析)
- 熱圖 (分布分析)
- 桑基圖 (流向分析)
- 雷達圖 (多維度評估)
- 甘特圖 (時程管理)

**互動功能**:
- 鑽取 (Drill-down)
- 篩選 (Filter)
- 時間範圍選擇
- 匯出報表 (PDF, Excel)
- 訂閱和排程
- 分享和協作

#### 2.3.3 AR/VR 需求

**AR 應用場景**:
1. **設備維護指引**:AR 疊加維護步驟和注意事項
2. **遠程專家支援**:AR 視訊通話和標註
3. **培訓輔助**:AR 疊加操作說明
4. **品質檢驗**:AR 疊加檢驗標準和結果

**VR 應用場景**:
1. **虛擬工廠巡檢**:VR 沉浸式工廠巡檢
2. **安全培訓**:VR 模擬緊急情況和應對
3. **設備操作培訓**:VR 模擬設備操作
4. **設施規劃**:VR 模擬新設備佈局

**設備需求**:
- AR 眼鏡:Microsoft HoloLens 2 或 Magic Leap 2
- VR 頭盔:Meta Quest Pro 或 HTC Vive Pro 2
- 移動設備:iPad Pro 或 Android 平板

---

## 3. 技術架構設計

### 3.1 整體架構

```
┌─────────────────────────────────────────────────────────────────┐
│                    使用者介面層 (UI Layer)                       │
│  • Web Portal  • Mobile App  • AR/VR App  • Large Display       │
└─────────────────────────────────────────────────────────────────┘
                              ▲
                              │
┌─────────────────────────────────────────────────────────────────┐
│              視覺化渲染層 (Visualization Rendering)              │
│  ┌──────────────────────┐  ┌──────────────────────────────┐    │
│  │  3D Rendering        │  │  2D Charting & Dashboard     │    │
│  │  • NVIDIA Omniverse  │  │  • D3.js / Chart.js          │    │
│  │  • USD Composer      │  │  • Plotly / ECharts          │    │
│  │  • RTX Renderer      │  │  • Custom WebGL              │    │
│  └──────────────────────┘  └──────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────┘
                              ▲
                              │
┌─────────────────────────────────────────────────────────────────┐
│           視覺化服務層 (Visualization Services)                  │
│  • Scene Management  • Data Binding  • Rendering Pipeline       │
│  • Animation Control  • Camera Control  • Collaboration         │
└─────────────────────────────────────────────────────────────────┘
                              ▲
                              │
┌─────────────────────────────────────────────────────────────────┐
│              數據處理層 (Data Processing Layer)                  │
│  • Real-time Aggregation  • Historical Analysis                 │
│  • Spatial Indexing  • Time-series Optimization                 │
└─────────────────────────────────────────────────────────────────┘
                              ▲
                              │
┌─────────────────────────────────────────────────────────────────┐
│              數據中樞層 (NDH - Neutral Data Hub)                 │
│  • Kafka  • InfluxDB  • PostgreSQL  • Redis Cache               │
└─────────────────────────────────────────────────────────────────┘
```

### 3.2 核心技術棧

#### 3.2.1 3D 視覺化技術

**NVIDIA Omniverse Platform**:
- **USD (Universal Scene Description)**:場景描述格式
- **RTX Renderer**:即時光線追蹤渲染
- **PhysX**:物理模擬引擎
- **Omniverse Kit**:應用開發框架
- **Nucleus Server**:協作伺服器

**替代/補充方案**:
- **Three.js**:Web 3D 圖形庫
- **Babylon.js**:遊戲引擎級 Web 3D
- **Unity**:跨平台 3D 引擎
- **Unreal Engine**:高品質 3D 渲染引擎

#### 3.2.2 2D 視覺化技術

**圖表庫**:
- **D3.js**:數據驅動文檔,高度客製化
- **Chart.js**:簡單易用的圖表庫
- **ECharts**:百度開源,功能豐富
- **Plotly**:科學計算視覺化
- **Highcharts**:商業圖表庫

**儀表板框架**:
- **Grafana**:開源監控儀表板
- **Kibana**:Elasticsearch 視覺化
- **Superset**:Apache 開源 BI 平台
- **Tableau**:商業 BI 工具
- **Power BI**:Microsoft BI 平台

**自訂開發**:
- **React**:前端框架
- **Vue.js**:漸進式框架
- **Angular**:企業級框架
- **WebGL**:底層 3D 圖形 API

#### 3.2.3 AR/VR 技術

**AR 平台**:
- **ARKit** (iOS):Apple AR 框架
- **ARCore** (Android):Google AR 框架
- **Vuforia**:企業級 AR 平台
- **8th Wall**:Web AR 平台

**VR 平台**:
- **Unity XR**:跨平台 VR 開發
- **Unreal Engine VR**:高品質 VR
- **WebXR**:Web VR/AR 標準
- **NVIDIA CloudXR**:雲端 VR 串流

**設備 SDK**:
- **HoloLens 2 SDK**:Microsoft AR 眼鏡
- **Meta Quest SDK**:Meta VR 頭盔
- **Magic Leap SDK**:Magic Leap AR 眼鏡

#### 3.2.4 後端技術

**API 服務**:
- **FastAPI** (Python):高效能 API 框架
- **Node.js + Express**:JavaScript 後端
- **GraphQL**:靈活的查詢語言

**即時通訊**:
- **WebSocket**:雙向即時通訊
- **Socket.io**:WebSocket 封裝
- **MQTT**:輕量級訊息協定

**快取**:
- **Redis**:記憶體快取
- **Memcached**:分散式快取

**訊息佇列**:
- **Kafka**:事件串流平台
- **RabbitMQ**:訊息佇列

### 3.3 部署架構

#### 3.3.1 雲端部署 (推薦)

```
┌─────────────────────────────────────────────────────────────┐
│                      使用者存取層                            │
│  • Web Browser  • Mobile App  • VR Headset                  │
└─────────────────────────────────────────────────────────────┘
                         ▲
                         │ HTTPS / WebSocket
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    CDN + Load Balancer                       │
│  • Cloudflare / AWS CloudFront                              │
│  • NGINX / AWS ALB                                          │
└─────────────────────────────────────────────────────────────┘
                         ▲
                         │
        ┌────────────────┴────────────────┐
        ▼                                 ▼
┌──────────────────┐            ┌──────────────────┐
│  Web Frontend    │            │  API Gateway     │
│  • React App     │            │  • FastAPI       │
│  • Static Assets │            │  • GraphQL       │
│  (S3 + CloudFront)│            │  (ECS / EKS)    │
└──────────────────┘            └──────────────────┘
                                         ▲
                                         │
        ┌────────────────────────────────┼────────────────┐
        ▼                                ▼                ▼
┌──────────────┐            ┌──────────────┐  ┌──────────────┐
│ Omniverse    │            │ Visualization │  │ Data Service │
│ Streaming    │            │ Service       │  │ • InfluxDB   │
│ (CloudXR)    │            │ • Scene Mgmt  │  │ • PostgreSQL │
│ (EC2 GPU)    │            │ • Data Binding│  │ • Redis      │
└──────────────┘            └──────────────┘  └──────────────┘
```

**雲端服務選擇**:
- **AWS**:EC2 (GPU), ECS, EKS, S3, RDS, ElastiCache
- **Azure**:Azure VM (GPU), AKS, Blob Storage, Azure Database
- **GCP**:Compute Engine (GPU), GKE, Cloud Storage, Cloud SQL

#### 3.3.2 混合部署

```
┌─────────────── 雲端 (Cloud) ───────────────┐
│                                            │
│  • Web Frontend (CDN)                      │
│  • API Gateway                             │
│  • 歷史數據分析                             │
│  • 報表生成                                 │
│                                            │
└────────────────────────────────────────────┘
                    ▲
                    │ VPN / Direct Connect
                    ▼
┌─────────────── 本地 (On-Premise) ──────────┐
│                                            │
│  • Omniverse Server (GPU Cluster)         │
│  • Real-time Data Processing              │
│  • NDH (Kafka, InfluxDB, PostgreSQL)      │
│  • OT Data Connectors                     │
│                                            │
└────────────────────────────────────────────┘
```

**優勢**:
- 敏感數據保留在本地
- 即時數據處理低延遲
- 雲端提供彈性擴展
- 降低雲端成本

---

## 4. 3D 視覺化平台

### 4.1 NVIDIA Omniverse 平台設計

#### 4.1.1 場景架構

**場景層次結構**:

```
Harvatek_Factory.usd (根場景)
├── Building.usd (建築主體)
│   ├── Floor_B1.usd (地下一樓)
│   ├── Floor_1F.usd (一樓)
│   ├── Floor_2F.usd (二樓)
│   │   ├── Area_2F_A.usd (2F-A 區)
│   │   │   ├── WireBonder_001.usd
│   │   │   ├── WireBonder_002.usd
│   │   │   └── ...
│   │   ├── Area_2F_B.usd (2F-B 區)
│   │   └── Area_2F_C.usd (2F-C 區)
│   ├── Floor_3F.usd (三樓)
│   ├── Floor_4F.usd (四樓)
│   └── Floor_5F.usd (五樓)
├── Equipment.usd (設備集合)
│   ├── Production_Equipment.usd
│   │   ├── DieSorter.usd (設備類型模板)
│   │   ├── DieBonder.usd
│   │   ├── WireBonder.usd
│   │   └── ...
│   └── Facility_Equipment.usd
│       ├── AHU.usd
│       ├── Chiller.usd
│       └── ...
├── Environment.usd (環境效果)
│   ├── Lighting.usd
│   ├── Particles.usd (粒子效果)
│   └── Weather.usd
├── Data_Overlays.usd (數據疊加層)
│   ├── Heatmaps.usd
│   ├── Data_Labels.usd
│   └── Annotations.usd
└── UI_Elements.usd (UI 元素)
    ├── Cameras.usd (預設視角)
    ├── Markers.usd (標記點)
    └── Navigation.usd (導航輔助)
```

#### 4.1.2 LOD (Level of Detail) 策略

**LOD 層級定義**:

| LOD 層級 | 使用場景 | 多邊形數 | 紋理解析度 | 範例 |
|---------|---------|---------|-----------|------|
| LOD 0 | 近距離檢視 (< 5m) | 100,000+ | 4K | 設備細節檢視 |
| LOD 1 | 中距離檢視 (5-20m) | 10,000 | 2K | 區域檢視 |
| LOD 2 | 遠距離檢視 (20-50m) | 1,000 | 1K | 樓層檢視 |
| LOD 3 | 全景檢視 (> 50m) | 100 | 512px | 整廠檢視 |

**自動 LOD 切換**:
```python
# Omniverse Kit Extension - LOD Manager
import omni.kit.commands
from pxr import Usd, UsdGeom

class LODManager:
    def __init__(self, stage):
        self.stage = stage
        self.camera_position = (0, 0, 0)
        
    def update_lod(self):
        """根據相機位置更新 LOD"""
        for prim in self.stage.Traverse():
            if prim.IsA(UsdGeom.Mesh):
                distance = self.calculate_distance(prim)
                lod_level = self.get_lod_level(distance)
                self.set_lod(prim, lod_level)
    
    def calculate_distance(self, prim):
        """計算相機到物體的距離"""
        # 實現距離計算
        pass
    
    def get_lod_level(self, distance):
        """根據距離決定 LOD 層級"""
        if distance < 5:
            return 0
        elif distance < 20:
            return 1
        elif distance < 50:
            return 2
        else:
            return 3
    
    def set_lod(self, prim, lod_level):
        """設定物體的 LOD 層級"""
        # 實現 LOD 切換
        pass
```

#### 4.1.3 即時數據綁定

**數據綁定架構**:

```python
# Omniverse Kit Extension - Data Binding
import omni.kit.app
import asyncio
import aiohttp
from pxr import Usd, UsdGeom, Gf

class DataBinding:
    def __init__(self, stage, api_endpoint):
        self.stage = stage
        self.api_endpoint = api_endpoint
        self.update_interval = 5  # 秒
        
    async def start_data_sync(self):
        """啟動數據同步"""
        while True:
            await self.fetch_and_update_data()
            await asyncio.sleep(self.update_interval)
    
    async def fetch_and_update_data(self):
        """從 API 獲取數據並更新場景"""
        async with aiohttp.ClientSession() as session:
            # 獲取設備狀態數據
            async with session.get(f"{self.api_endpoint}/equipment/status") as resp:
                data = await resp.json()
                self.update_equipment_status(data)
            
            # 獲取環境數據
            async with session.get(f"{self.api_endpoint}/environmental/sensors") as resp:
                data = await resp.json()
                self.update_environmental_data(data)
    
    def update_equipment_status(self, data):
        """更新設備狀態視覺化"""
        for equipment in data:
            equipment_id = equipment['equipment_id']
            status = equipment['status']
            
            # 找到對應的 USD Prim
            prim_path = f"/World/Equipment/{equipment_id}"
            prim = self.stage.GetPrimAtPath(prim_path)
            
            if prim.IsValid():
                # 更新顏色 (根據狀態)
                color = self.get_status_color(status)
                self.set_prim_color(prim, color)
                
                # 更新標籤
                self.update_label(prim, equipment)
    
    def get_status_color(self, status):
        """根據狀態返回顏色"""
        colors = {
            'RUNNING': (0.0, 1.0, 0.0),  # 綠色
            'IDLE': (1.0, 1.0, 0.0),     # 黃色
            'ALARM': (1.0, 0.0, 0.0),    # 紅色
            'OFFLINE': (0.5, 0.5, 0.5)   # 灰色
        }
        return colors.get(status, (1.0, 1.0, 1.0))
    
    def set_prim_color(self, prim, color):
        """設定 Prim 的顏色"""
        if prim.IsA(UsdGeom.Gprim):
            gprim = UsdGeom.Gprim(prim)
            gprim.GetDisplayColorAttr().Set([Gf.Vec3f(*color)])
    
    def update_label(self, prim, data):
        """更新數據標籤"""
        # 實現標籤更新
        pass
    
    def update_environmental_data(self, data):
        """更新環境數據視覺化 (熱圖)"""
        # 實現環境數據熱圖
        pass
```

#### 4.1.4 視覺化效果

**設備狀態視覺化**:

```python
# 設備狀態視覺化配置
EQUIPMENT_VISUALIZATION = {
    'RUNNING': {
        'color': (0.0, 1.0, 0.0),  # 綠色
        'emissive': 0.2,
        'animation': 'rotate',  # 旋轉動畫
        'particle': None
    },
    'IDLE': {
        'color': (1.0, 1.0, 0.0),  # 黃色
        'emissive': 0.1,
        'animation': 'pulse',  # 脈衝動畫
        'particle': None
    },
    'ALARM': {
        'color': (1.0, 0.0, 0.0),  # 紅色
        'emissive': 0.5,
        'animation': 'blink',  # 閃爍動畫
        'particle': 'warning'  # 警告粒子
    },
    'OFFLINE': {
        'color': (0.5, 0.5, 0.5),  # 灰色
        'emissive': 0.0,
        'animation': None,
        'particle': None
    }
}
```

**環境熱圖視覺化**:

```python
# 溫度熱圖生成
import numpy as np
from scipy.interpolate import griddata

class HeatmapGenerator:
    def __init__(self, floor_bounds):
        self.floor_bounds = floor_bounds  # (min_x, max_x, min_y, max_y)
        self.resolution = 100  # 網格解析度
        
    def generate_temperature_heatmap(self, sensor_data):
        """生成溫度熱圖"""
        # 提取感測器位置和溫度
        points = [(s['x'], s['y']) for s in sensor_data]
        values = [s['temperature'] for s in sensor_data]
        
        # 建立網格
        grid_x, grid_y = np.mgrid[
            self.floor_bounds[0]:self.floor_bounds[1]:complex(0, self.resolution),
            self.floor_bounds[2]:self.floor_bounds[3]:complex(0, self.resolution)
        ]
        
        # 插值
        grid_temp = griddata(points, values, (grid_x, grid_y), method='cubic')
        
        # 轉換為顏色
        heatmap = self.temperature_to_color(grid_temp)
        
        return heatmap
    
    def temperature_to_color(self, temperatures):
        """溫度轉顏色 (藍->綠->黃->紅)"""
        # 正規化溫度到 0-1
        temp_min, temp_max = 20, 30  # °C
        normalized = (temperatures - temp_min) / (temp_max - temp_min)
        normalized = np.clip(normalized, 0, 1)
        
        # 使用色彩映射
        # 藍色 (20°C) -> 綠色 (23°C) -> 黃色 (26°C) -> 紅色 (30°C)
        colors = np.zeros((*normalized.shape, 3))
        
        # R 通道
        colors[:, :, 0] = normalized
        # G 通道
        colors[:, :, 1] = 1 - abs(normalized - 0.5) * 2
        # B 通道
        colors[:, :, 2] = 1 - normalized
        
        return colors
```

### 4.2 預設視角和導航

#### 4.2.1 預設視角定義

```python
# 預設視角配置
PRESET_CAMERAS = {
    'overview': {
        'name': '整廠鳥瞰',
        'position': (0, 100, 0),
        'target': (0, 0, 0),
        'fov': 60,
        'description': '從上方俯瞰整個工廠'
    },
    'floor_2f': {
        'name': '2F 生產區',
        'position': (0, 20, 50),
        'target': (0, 0, 0),
        'fov': 75,
        'description': '2F 生產區域全景'
    },
    'floor_3f': {
        'name': '3F 生產區',
        'position': (0, 30, 50),
        'target': (0, 10, 0),
        'fov': 75,
        'description': '3F 生產區域全景'
    },
    'area_2f_a': {
        'name': '2F-A Wire Bonding',
        'position': (-30, 5, 20),
        'target': (-30, 0, 0),
        'fov': 90,
        'description': '2F-A 區 Wire Bonder 詳細檢視'
    },
    'facility_b1': {
        'name': 'B1 設施機房',
        'position': (0, -5, 30),
        'target': (0, -10, 0),
        'fov': 80,
        'description': 'B1 Chiller 和 Transformer 機房'
    }
}
```

#### 4.2.2 智能導航

```python
class SmartNavigation:
    def __init__(self, stage):
        self.stage = stage
        self.current_camera = None
        
    def navigate_to_equipment(self, equipment_id):
        """導航到指定設備"""
        prim = self.stage.GetPrimAtPath(f"/World/Equipment/{equipment_id}")
        if prim.IsValid():
            # 獲取設備位置
            xform = UsdGeom.Xformable(prim)
            transform = xform.ComputeLocalToWorldTransform(Usd.TimeCode.Default())
            position = transform.ExtractTranslation()
            
            # 計算相機位置 (設備前方 5 米,高度 2 米)
            camera_position = position + Gf.Vec3d(0, 2, 5)
            
            # 移動相機
            self.move_camera_to(camera_position, position)
    
    def navigate_to_alarm(self, alarm_id):
        """導航到告警位置"""
        # 從 API 獲取告警資訊
        alarm_data = self.fetch_alarm_data(alarm_id)
        equipment_id = alarm_data['equipment_id']
        
        # 導航到設備
        self.navigate_to_equipment(equipment_id)
        
        # 高亮顯示
        self.highlight_equipment(equipment_id)
    
    def move_camera_to(self, position, target, duration=2.0):
        """平滑移動相機"""
        # 實現相機平滑移動動畫
        pass
    
    def highlight_equipment(self, equipment_id):
        """高亮顯示設備"""
        # 實現高亮效果
        pass
```

### 4.3 協作功能

#### 4.3.1 多人協作

**Omniverse Nucleus 協作**:
- 多使用者同時檢視同一場景
- 即時標註和註解
- 語音/文字通訊
- 權限管理 (檢視/編輯/管理)

```python
# 協作標註系統
class CollaborationAnnotation:
    def __init__(self, nucleus_server):
        self.nucleus_server = nucleus_server
        self.annotations = []
        
    def add_annotation(self, user, position, text, type='note'):
        """新增標註"""
        annotation = {
            'id': self.generate_id(),
            'user': user,
            'position': position,
            'text': text,
            'type': type,  # note, issue, question
            'timestamp': datetime.now(),
            'status': 'open'
        }
        
        self.annotations.append(annotation)
        self.sync_to_nucleus(annotation)
        
        return annotation
    
    def resolve_annotation(self, annotation_id, user):
        """解決標註"""
        annotation = self.get_annotation(annotation_id)
        if annotation:
            annotation['status'] = 'resolved'
            annotation['resolved_by'] = user
            annotation['resolved_at'] = datetime.now()
            self.sync_to_nucleus(annotation)
    
    def sync_to_nucleus(self, annotation):
        """同步到 Nucleus 伺服器"""
        # 實現同步邏輯
        pass
```

---

## 5. 即時數據視覺化

### 5.1 儀表板設計

#### 5.1.1 高階主管駕駛艙

**佈局設計**:

```
┌─────────────────────────────────────────────────────────────┐
│  Harvatek Executive Dashboard          [日期] [時間範圍▼]   │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │
│  │   OEE    │  │  良率    │  │  產能    │  │  能耗    │   │
│  │   85%    │  │  97%     │  │  95%     │  │ -12%     │   │
│  │  ▲ +5%   │  │  ▲ +3%   │  │  ▲ +2%   │  │  ▼ YoY   │   │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │
│                                                              │
│  ┌────────────────────────────┐  ┌────────────────────────┐│
│  │  OEE 趨勢 (30 天)          │  │  產能利用率 (本週)     ││
│  │  [折線圖]                  │  │  [柱狀圖]              ││
│  │                            │  │                        ││
│  └────────────────────────────┘  └────────────────────────┘│
│                                                              │
│  ┌────────────────────────────┐  ┌────────────────────────┐│
│  │  告警統計 (24小時)         │  │  能源成本 (本月)       ││
│  │  [圓餅圖]                  │  │  [桑基圖]              ││
│  │                            │  │                        ││
│  └────────────────────────────┘  └────────────────────────┘│
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  3D 工廠視圖                                         │  │
│  │  [Omniverse 嵌入式視圖]                              │  │
│  │                                                       │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

**實現代碼** (React + D3.js):

```jsx
// ExecutiveDashboard.jsx
import React, { useState, useEffect } from 'react';
import { LineChart, BarChart, PieChart, SankeyChart } from './charts';
import { OmniverseViewer } from './OmniverseViewer';
import { MetricCard } from './MetricCard';
import { fetchDashboardData } from './api';

const ExecutiveDashboard = () => {
  const [data, setData] = useState(null);
  const [timeRange, setTimeRange] = useState('30d');
  
  useEffect(() => {
    const loadData = async () => {
      const dashboardData = await fetchDashboardData(timeRange);
      setData(dashboardData);
    };
    
    loadData();
    
    // 每 30 秒更新一次
    const interval = setInterval(loadData, 30000);
    return () => clearInterval(interval);
  }, [timeRange]);
  
  if (!data) return <div>Loading...</div>;
  
  return (
    <div className="executive-dashboard">
      <header>
        <h1>Harvatek Executive Dashboard</h1>
        <div className="controls">
          <span>{new Date().toLocaleDateString()}</span>
          <select value={timeRange} onChange={(e) => setTimeRange(e.target.value)}>
            <option value="7d">過去 7 天</option>
            <option value="30d">過去 30 天</option>
            <option value="90d">過去 90 天</option>
          </select>
        </div>
      </header>
      
      <div className="metrics-row">
        <MetricCard
          title="OEE"
          value={data.oee.current}
          change={data.oee.change}
          format="percentage"
        />
        <MetricCard
          title="良率"
          value={data.yield.current}
          change={data.yield.change}
          format="percentage"
        />
        <MetricCard
          title="產能"
          value={data.capacity.current}
          change={data.capacity.change}
          format="percentage"
        />
        <MetricCard
          title="能耗"
          value={data.energy.current}
          change={data.energy.change}
          format="percentage"
          inverse={true}
        />
      </div>
      
      <div className="charts-grid">
        <div className="chart-container">
          <h3>OEE 趨勢 (30 天)</h3>
          <LineChart data={data.oee_trend} />
        </div>
        
        <div className="chart-container">
          <h3>產能利用率 (本週)</h3>
          <BarChart data={data.capacity_utilization} />
        </div>
        
        <div className="chart-container">
          <h3>告警統計 (24小時)</h3>
          <PieChart data={data.alarm_stats} />
        </div>
        
        <div className="chart-container">
          <h3>能源成本 (本月)</h3>
          <SankeyChart data={data.energy_flow} />
        </div>
      </div>
      
      <div className="omniverse-container">
        <h3>3D 工廠視圖</h3>
        <OmniverseViewer 
          scene="Harvatek_Factory.usd"
          camera="overview"
        />
      </div>
    </div>
  );
};

export default ExecutiveDashboard;
```

#### 5.1.2 生產監控儀表板

**佈局設計**:

```
┌─────────────────────────────────────────────────────────────┐
│  生產監控儀表板                    [2F▼] [即時更新: 5秒前]   │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  生產線狀態 (2F)                                     │  │
│  │  ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐ ┌────┐         │  │
│  │  │ DS │→│ DB │→│ WB │→│ DP │→│ TS │→│ TP │         │  │
│  │  │ ✓  │ │ ✓  │ │ ⚠  │ │ ✓  │ │ ✓  │ │ ✓  │         │  │
│  │  └────┘ └────┘ └────┘ └────┘ └────┘ └────┘         │  │
│  │  DS: Die Sorter, DB: Die Bonder, WB: Wire Bonder    │  │
│  │  DP: Dispenser, TS: Tester, TP: Taping Machine      │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  ┌────────────────────┐  ┌────────────────────────────────┐│
│  │  設備狀態分布      │  │  設備 OEE 排行 (Top 10)        ││
│  │  ┌──────────────┐ │  │  ┌──────────────────────────┐ ││
│  │  │ 運行中: 85%  │ │  │  │ WB-001  ████████ 92%     │ ││
│  │  │ 閒置:   10%  │ │  │  │ WB-002  ███████  88%     │ ││
│  │  │ 告警:    3%  │ │  │  │ DB-001  ███████  87%     │ ││
│  │  │ 離線:    2%  │ │  │  │ ...                      │ ││
│  │  └──────────────┘ │  │  └──────────────────────────┘ ││
│  └────────────────────┘  └────────────────────────────────┘│
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  設備熱圖 (2F)                                       │  │
│  │  [3D 熱圖視圖 - 顏色表示 OEE]                        │  │
│  │                                                       │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │  即時告警                                            │  │
│  │  ⚠ WB-023  Wire Bonding 異常  2F-A  [5分鐘前]       │  │
│  │  ⚠ TS-045  測試失敗率過高    3F-B  [12分鐘前]       │  │
│  │  ℹ DB-012  維護提醒          2F-C  [1小時前]        │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 圖表庫實現

#### 5.2.1 OEE 趨勢圖 (D3.js)

```javascript
// OEETrendChart.js
import * as d3 from 'd3';

class OEETrendChart {
  constructor(containerId, data) {
    this.container = d3.select(`#${containerId}`);
    this.data = data;
    this.margin = { top: 20, right: 30, bottom: 30, left: 50 };
    this.width = 800 - this.margin.left - this.margin.right;
    this.height = 400 - this.margin.top - this.margin.bottom;
    
    this.init();
  }
  
  init() {
    // 建立 SVG
    this.svg = this.container.append('svg')
      .attr('width', this.width + this.margin.left + this.margin.right)
      .attr('height', this.height + this.margin.top + this.margin.bottom)
      .append('g')
      .attr('transform', `translate(${this.margin.left},${this.margin.top})`);
    
    // 建立比例尺
    this.xScale = d3.scaleTime()
      .domain(d3.extent(this.data, d => new Date(d.date)))
      .range([0, this.width]);
    
    this.yScale = d3.scaleLinear()
      .domain([0, 100])
      .range([this.height, 0]);
    
    // 建立軸
    this.xAxis = d3.axisBottom(this.xScale);
    this.yAxis = d3.axisLeft(this.yScale);
    
    this.svg.append('g')
      .attr('class', 'x-axis')
      .attr('transform', `translate(0,${this.height})`)
      .call(this.xAxis);
    
    this.svg.append('g')
      .attr('class', 'y-axis')
      .call(this.yAxis);
    
    // 建立折線生成器
    this.line = d3.line()
      .x(d => this.xScale(new Date(d.date)))
      .y(d => this.yScale(d.oee))
      .curve(d3.curveMonotoneX);
    
    // 繪製折線
    this.path = this.svg.append('path')
      .datum(this.data)
      .attr('class', 'line')
      .attr('fill', 'none')
      .attr('stroke', '#0066CC')
      .attr('stroke-width', 2)
      .attr('d', this.line);
    
    // 新增目標線
    this.svg.append('line')
      .attr('class', 'target-line')
      .attr('x1', 0)
      .attr('x2', this.width)
      .attr('y1', this.yScale(85))
      .attr('y2', this.yScale(85))
      .attr('stroke', '#00CC66')
      .attr('stroke-width', 2)
      .attr('stroke-dasharray', '5,5');
    
    // 新增標籤
    this.svg.append('text')
      .attr('x', this.width - 100)
      .attr('y', this.yScale(85) - 10)
      .attr('fill', '#00CC66')
      .text('目標: 85%');
  }
  
  update(newData) {
    this.data = newData;
    
    // 更新比例尺
    this.xScale.domain(d3.extent(this.data, d => new Date(d.date)));
    
    // 更新軸
    this.svg.select('.x-axis').call(this.xAxis);
    
    // 更新折線
    this.path
      .datum(this.data)
      .transition()
      .duration(750)
      .attr('d', this.line);
  }
}

export default OEETrendChart;
```

#### 5.2.2 設備狀態熱圖 (Three.js)

```javascript
// EquipmentHeatmap.js
import * as THREE from 'three';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls';

class EquipmentHeatmap {
  constructor(containerId, equipmentData) {
    this.container = document.getElementById(containerId);
    this.equipmentData = equipmentData;
    
    this.init();
    this.createHeatmap();
    this.animate();
  }
  
  init() {
    // 建立場景
    this.scene = new THREE.Scene();
    this.scene.background = new THREE.Color(0xf5f5f5);
    
    // 建立相機
    this.camera = new THREE.PerspectiveCamera(
      75,
      this.container.clientWidth / this.container.clientHeight,
      0.1,
      1000
    );
    this.camera.position.set(0, 50, 50);
    
    // 建立渲染器
    this.renderer = new THREE.WebGLRenderer({ antialias: true });
    this.renderer.setSize(this.container.clientWidth, this.container.clientHeight);
    this.container.appendChild(this.renderer.domElement);
    
    // 建立控制器
    this.controls = new OrbitControls(this.camera, this.renderer.domElement);
    
    // 新增光源
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.6);
    this.scene.add(ambientLight);
    
    const directionalLight = new THREE.DirectionalLight(0xffffff, 0.4);
    directionalLight.position.set(10, 10, 5);
    this.scene.add(directionalLight);
  }
  
  createHeatmap() {
    // 建立地板
    const floorGeometry = new THREE.PlaneGeometry(100, 100);
    const floorMaterial = new THREE.MeshBasicMaterial({ 
      color: 0xcccccc,
      side: THREE.DoubleSide 
    });
    const floor = new THREE.Mesh(floorGeometry, floorMaterial);
    floor.rotation.x = -Math.PI / 2;
    this.scene.add(floor);
    
    // 建立設備方塊
    this.equipmentMeshes = [];
    
    this.equipmentData.forEach(equipment => {
      const geometry = new THREE.BoxGeometry(2, 2, 2);
      const color = this.getOEEColor(equipment.oee);
      const material = new THREE.MeshStandardMaterial({ color });
      
      const mesh = new THREE.Mesh(geometry, material);
      mesh.position.set(equipment.x, 1, equipment.y);
      mesh.userData = equipment;
      
      this.scene.add(mesh);
      this.equipmentMeshes.push(mesh);
      
      // 新增標籤
      this.addLabel(mesh, equipment.id, equipment.oee);
    });
  }
  
  getOEEColor(oee) {
    // OEE 轉顏色 (紅->黃->綠)
    if (oee >= 85) {
      return 0x00CC66;  // 綠色
    } else if (oee >= 75) {
      return 0xFFCC00;  // 黃色
    } else {
      return 0xFF6600;  // 紅色
    }
  }
  
  addLabel(mesh, id, oee) {
    // 使用 CSS2DRenderer 新增標籤
    // 實現省略
  }
  
  update(newEquipmentData) {
    this.equipmentData = newEquipmentData;
    
    this.equipmentMeshes.forEach((mesh, index) => {
      const equipment = this.equipmentData[index];
      const color = this.getOEEColor(equipment.oee);
      mesh.material.color.setHex(color);
      mesh.userData = equipment;
    });
  }
  
  animate() {
    requestAnimationFrame(() => this.animate());
    this.controls.update();
    this.renderer.render(this.scene, this.camera);
  }
}

export default EquipmentHeatmap;
```

---

## 6. AR/VR 應用

### 6.1 AR 維護指引

#### 6.1.1 應用場景

**設備維護 AR 輔助**:
1. 維護工程師配戴 HoloLens 2
2. 掃描設備 QR Code 或使用語音指令
3. AR 疊加顯示:
   - 設備 3D 模型和內部結構
   - 維護步驟和注意事項
   - 即時數據和健康度
   - 備品位置和庫存
4. 完成維護後記錄和拍照

#### 6.1.2 技術實現 (Unity + HoloLens 2)

```csharp
// ARMaintenanceGuide.cs
using UnityEngine;
using Microsoft.MixedReality.Toolkit;
using Microsoft.MixedReality.Toolkit.UI;

public class ARMaintenanceGuide : MonoBehaviour
{
    [SerializeField] private GameObject equipmentModelPrefab;
    [SerializeField] private GameObject stepIndicatorPrefab;
    
    private GameObject currentEquipmentModel;
    private MaintenanceProcedure currentProcedure;
    private int currentStep = 0;
    
    public async void LoadEquipment(string equipmentId)
    {
        // 從 API 獲取設備資訊
        var equipmentData = await FetchEquipmentData(equipmentId);
        
        // 載入 3D 模型
        currentEquipmentModel = Instantiate(equipmentModelPrefab);
        // 設定模型位置 (相對於實際設備)
        currentEquipmentModel.transform.position = GetEquipmentPosition();
        
        // 載入維護程序
        currentProcedure = await FetchMaintenanceProcedure(equipmentId);
        
        // 顯示第一步
        ShowStep(0);
    }
    
    private void ShowStep(int stepIndex)
    {
        if (stepIndex >= currentProcedure.Steps.Count)
        {
            // 維護完成
            ShowCompletionDialog();
            return;
        }
        
        var step = currentProcedure.Steps[stepIndex];
        
        // 顯示步驟指示器
        var indicator = Instantiate(stepIndicatorPrefab);
        indicator.transform.position = step.TargetPosition;
        
        // 顯示步驟說明
        ShowStepDescription(step.Description);
        
        // 高亮顯示目標零件
        HighlightPart(step.TargetPartId);
        
        // 如果有動畫,播放動畫
        if (!string.IsNullOrEmpty(step.AnimationName))
        {
            PlayAnimation(step.AnimationName);
        }
    }
    
    public void NextStep()
    {
        currentStep++;
        ShowStep(currentStep);
    }
    
    public void PreviousStep()
    {
        if (currentStep > 0)
        {
            currentStep--;
            ShowStep(currentStep);
        }
    }
    
    private void ShowStepDescription(string description)
    {
        // 顯示文字說明
        // 實現省略
    }
    
    private void HighlightPart(string partId)
    {
        // 高亮顯示零件
        // 實現省略
    }
    
    private void PlayAnimation(string animationName)
    {
        // 播放動畫
        // 實現省略
    }
    
    private void ShowCompletionDialog()
    {
        // 顯示完成對話框
        // 實現省略
    }
}
```

### 6.2 VR 工廠巡檢

#### 6.2.1 應用場景

**遠程工廠巡檢**:
1. 管理者配戴 Meta Quest Pro
2. 進入虛擬工廠環境
3. 自由移動和檢視:
   - 查看設備即時狀態
   - 檢視生產數據
   - 識別異常和告警
   - 與現場人員語音通訊
4. 標註問題和下達指令

#### 6.2.2 技術實現 (Unity + Meta Quest)

```csharp
// VRFactoryInspection.cs
using UnityEngine;
using UnityEngine.XR;
using System.Collections.Generic;

public class VRFactoryInspection : MonoBehaviour
{
    [SerializeField] private Transform vrCamera;
    [SerializeField] private GameObject dataPanel Prefab;
    [SerializeField] private GameObject annotationPrefab;
    
    private Dictionary<string, GameObject> equipmentObjects = new Dictionary<string, GameObject>();
    private List<Annotation> annotations = new List<Annotation>();
    
    void Start()
    {
        // 載入工廠場景
        LoadFactoryScene();
        
        // 啟動數據同步
        StartDataSync();
        
        // 設定控制器
        SetupControllers();
    }
    
    private async void LoadFactoryScene()
    {
        // 從 Omniverse 載入 USD 場景
        // 或使用預先轉換的 Unity 場景
        
        // 載入設備物件
        var equipmentList = await FetchEquipmentList();
        foreach (var equipment in equipmentList)
        {
            var obj = Instantiate(GetEquipmentPrefab(equipment.Type));
            obj.transform.position = equipment.Position;
            obj.name = equipment.Id;
            
            equipmentObjects[equipment.Id] = obj;
        }
    }
    
    private async void StartDataSync()
    {
        // 每 5 秒同步一次數據
        while (true)
        {
            await Task.Delay(5000);
            
            var statusData = await FetchEquipmentStatus();
            UpdateEquipmentStatus(statusData);
        }
    }
    
    private void UpdateEquipmentStatus(List<EquipmentStatus> statusData)
    {
        foreach (var status in statusData)
        {
            if (equipmentObjects.TryGetValue(status.EquipmentId, out var obj))
            {
                // 更新顏色
                var renderer = obj.GetComponent<Renderer>();
                renderer.material.color = GetStatusColor(status.Status);
                
                // 更新數據面板 (如果正在檢視)
                if (IsViewing(obj))
                {
                    UpdateDataPanel(status);
                }
            }
        }
    }
    
    private void SetupControllers()
    {
        // 設定 VR 控制器
        // 左手:移動和旋轉
        // 右手:選擇和互動
        
        // 實現省略
    }
    
    public void OnEquipmentSelected(GameObject equipment)
    {
        // 顯示設備數據面板
        var panel = Instantiate(dataPanelPrefab);
        panel.transform.position = equipment.transform.position + Vector3.up * 2;
        panel.transform.LookAt(vrCamera);
        
        // 載入設備數據
        var equipmentId = equipment.name;
        LoadEquipmentData(panel, equipmentId);
    }
    
    private async void LoadEquipmentData(GameObject panel, string equipmentId)
    {
        var data = await FetchEquipmentDetailedData(equipmentId);
        
        // 更新面板內容
        var dataDisplay = panel.GetComponent<DataPanelDisplay>();
        dataDisplay.UpdateData(data);
    }
    
    public void CreateAnnotation(Vector3 position, string text)
    {
        var annotation = Instantiate(annotationPrefab);
        annotation.transform.position = position;
        
        var annotationData = new Annotation
        {
            Id = System.Guid.NewGuid().ToString(),
            Position = position,
            Text = text,
            CreatedBy = GetCurrentUser(),
            CreatedAt = System.DateTime.Now
        };
        
        annotations.Add(annotationData);
        
        // 同步到伺服器
        SyncAnnotation(annotationData);
    }
    
    private Color GetStatusColor(string status)
    {
        switch (status)
        {
            case "RUNNING":
                return Color.green;
            case "IDLE":
                return Color.yellow;
            case "ALARM":
                return Color.red;
            case "OFFLINE":
                return Color.gray;
            default:
                return Color.white;
        }
    }
}
```

### 6.3 AR/VR 培訓系統

#### 6.3.1 VR 安全培訓

**緊急情況模擬**:
1. 火災應對訓練
2. 化學品洩漏處理
3. 設備故障應急
4. 人員疏散演練

```csharp
// VRSafetyTraining.cs
using UnityEngine;
using System.Collections;

public class VRSafetyTraining : MonoBehaviour
{
    [SerializeField] private GameObject fireEffectPrefab;
    [SerializeField] private GameObject smokeEffectPrefab;
    [SerializeField] private AudioClip alarmSound;
    
    private TrainingScenario currentScenario;
    private float trainingStartTime;
    private List<TrainingAction> userActions = new List<TrainingAction>();
    
    public void StartTraining(string scenarioId)
    {
        currentScenario = LoadScenario(scenarioId);
        trainingStartTime = Time.time;
        
        // 開始情境
        StartCoroutine(RunScenario());
    }
    
    private IEnumerator RunScenario()
    {
        foreach (var event in currentScenario.Events)
        {
            yield return new WaitForSeconds(event.DelaySeconds);
            
            TriggerEvent(event);
        }
        
        // 評估表現
        EvaluatePerformance();
    }
    
    private void TriggerEvent(ScenarioEvent event)
    {
        switch (event.Type)
        {
            case "Fire":
                StartFire(event.Location);
                break;
            case "Alarm":
                PlayAlarm();
                break;
            case "Smoke":
                GenerateSmoke(event.Location);
                break;
        }
    }
    
    private void StartFire(Vector3 location)
    {
        var fire = Instantiate(fireEffectPrefab);
        fire.transform.position = location;
        
        // 記錄事件
        LogEvent("Fire started at " + location);
    }
    
    public void UserAction(string actionType, Vector3 location)
    {
        var action = new TrainingAction
        {
            Type = actionType,
            Location = location,
            Timestamp = Time.time - trainingStartTime
        };
        
        userActions.Add(action);
        
        // 檢查是否正確
        CheckAction(action);
    }
    
    private void CheckAction(TrainingAction action)
    {
        // 檢查動作是否正確
        // 給予即時回饋
        
        // 實現省略
    }
    
    private void EvaluatePerformance()
    {
        // 評估訓練表現
        // 生成報告
        
        var score = CalculateScore();
        ShowResults(score);
    }
    
    private float CalculateScore()
    {
        // 計算分數
        // 基於:反應時間、動作正確性、完成度
        
        // 實現省略
        return 0f;
    }
}
```

---

## 7. 決策支援系統

### 7.1 高階主管駕駛艙

#### 7.1.1 移動端應用 (React Native)

```jsx
// ExecutiveMobileApp.jsx
import React, { useState, useEffect } from 'react';
import { View, Text, ScrollView, RefreshControl } from 'react-native';
import { LineChart, BarChart, PieChart } from 'react-native-chart-kit';
import { fetchExecutiveData } from './api';

const ExecutiveMobileApp = () => {
  const [data, setData] = useState(null);
  const [refreshing, setRefreshing] = useState(false);
  
  useEffect(() => {
    loadData();
  }, []);
  
  const loadData = async () => {
    setRefreshing(true);
    const executiveData = await fetchExecutiveData();
    setData(executiveData);
    setRefreshing(false);
  };
  
  if (!data) return <Text>Loading...</Text>;
  
  return (
    <ScrollView
      refreshControl={
        <RefreshControl refreshing={refreshing} onRefresh={loadData} />
      }
    >
      <View style={styles.header}>
        <Text style={styles.title}>Harvatek Executive Dashboard</Text>
        <Text style={styles.subtitle}>{new Date().toLocaleDateString()}</Text>
      </View>
      
      <View style={styles.metricsContainer}>
        <MetricCard
          title="OEE"
          value={data.oee.current}
          change={data.oee.change}
          color="#0066CC"
        />
        <MetricCard
          title="良率"
          value={data.yield.current}
          change={data.yield.change}
          color="#00CC66"
        />
        <MetricCard
          title="產能"
          value={data.capacity.current}
          change={data.capacity.change}
          color="#0088DD"
        />
        <MetricCard
          title="能耗"
          value={data.energy.current}
          change={data.energy.change}
          color="#FF6600"
          inverse={true}
        />
      </View>
      
      <View style={styles.chartContainer}>
        <Text style={styles.chartTitle}>OEE 趨勢 (7 天)</Text>
        <LineChart
          data={{
            labels: data.oee_trend.labels,
            datasets: [{ data: data.oee_trend.values }]
          }}
          width={350}
          height={220}
          chartConfig={chartConfig}
        />
      </View>
      
      <View style={styles.chartContainer}>
        <Text style={styles.chartTitle}>告警統計</Text>
        <PieChart
          data={data.alarm_stats}
          width={350}
          height={220}
          chartConfig={chartConfig}
          accessor="count"
          backgroundColor="transparent"
        />
      </View>
      
      <View style={styles.alertsContainer}>
        <Text style={styles.sectionTitle}>即時告警</Text>
        {data.recent_alarms.map(alarm => (
          <AlertCard key={alarm.id} alarm={alarm} />
        ))}
      </View>
    </ScrollView>
  );
};

const MetricCard = ({ title, value, change, color, inverse = false }) => {
  const isPositive = inverse ? change < 0 : change > 0;
  const arrow = isPositive ? '▲' : '▼';
  const changeColor = isPositive ? '#00CC66' : '#FF6600';
  
  return (
    <View style={[styles.metricCard, { borderLeftColor: color }]}>
      <Text style={styles.metricTitle}>{title}</Text>
      <Text style={styles.metricValue}>{value}%</Text>
      <Text style={[styles.metricChange, { color: changeColor }]}>
        {arrow} {Math.abs(change)}%
      </Text>
    </View>
  );
};

const AlertCard = ({ alarm }) => {
  const severityColor = {
    'CRITICAL': '#FF0000',
    'WARNING': '#FF6600',
    'INFO': '#0066CC'
  }[alarm.severity];
  
  return (
    <View style={[styles.alertCard, { borderLeftColor: severityColor }]}>
      <Text style={styles.alertTitle}>{alarm.title}</Text>
      <Text style={styles.alertDesc}>{alarm.description}</Text>
      <Text style={styles.alertTime}>{alarm.time}</Text>
    </View>
  );
};

const chartConfig = {
  backgroundColor: '#ffffff',
  backgroundGradientFrom: '#ffffff',
  backgroundGradientTo: '#ffffff',
  decimalPlaces: 0,
  color: (opacity = 1) => `rgba(0, 102, 204, ${opacity})`,
  style: {
    borderRadius: 16
  }
};

const styles = {
  // 樣式定義省略
};

export default ExecutiveMobileApp;
```

### 7.2 What-if 分析

#### 7.2.1 產能模擬

```python
# capacity_simulation.py
import numpy as np
from dataclasses import dataclass
from typing import List, Dict

@dataclass
class Equipment:
    id: str
    type: str
    uph: int  # Units Per Hour
    availability: float  # 0-1
    performance: float  # 0-1
    quality: float  # 0-1
    
    @property
    def oee(self):
        return self.availability * self.performance * self.quality
    
    @property
    def effective_uph(self):
        return self.uph * self.oee

class ProductionLineSimulator:
    def __init__(self, equipment_list: List[Equipment]):
        self.equipment_list = equipment_list
        
    def calculate_line_capacity(self):
        """計算生產線產能 (瓶頸設備決定)"""
        bottleneck_uph = min(eq.effective_uph for eq in self.equipment_list)
        return bottleneck_uph
    
    def identify_bottleneck(self):
        """識別瓶頸設備"""
        bottleneck = min(self.equipment_list, key=lambda eq: eq.effective_uph)
        return bottleneck
    
    def simulate_improvement(self, equipment_id: str, improvement_type: str, value: float):
        """模擬改善效果"""
        # 找到設備
        equipment = next(eq for eq in self.equipment_list if eq.id == equipment_id)
        
        # 記錄原始值
        original_capacity = self.calculate_line_capacity()
        
        # 應用改善
        if improvement_type == 'availability':
            original_value = equipment.availability
            equipment.availability = min(1.0, equipment.availability + value)
        elif improvement_type == 'performance':
            original_value = equipment.performance
            equipment.performance = min(1.0, equipment.performance + value)
        elif improvement_type == 'quality':
            original_value = equipment.quality
            equipment.quality = min(1.0, equipment.quality + value)
        
        # 計算新產能
        new_capacity = self.calculate_line_capacity()
        
        # 恢復原始值
        if improvement_type == 'availability':
            equipment.availability = original_value
        elif improvement_type == 'performance':
            equipment.performance = original_value
        elif improvement_type == 'quality':
            equipment.quality = original_value
        
        # 計算改善效果
        improvement = new_capacity - original_capacity
        improvement_pct = (improvement / original_capacity) * 100
        
        return {
            'equipment_id': equipment_id,
            'improvement_type': improvement_type,
            'value': value,
            'original_capacity': original_capacity,
            'new_capacity': new_capacity,
            'improvement': improvement,
            'improvement_pct': improvement_pct
        }
    
    def simulate_multiple_scenarios(self):
        """模擬多種情境"""
        scenarios = []
        
        # 情境 1: 改善瓶頸設備可用率 5%
        bottleneck = self.identify_bottleneck()
        scenarios.append(
            self.simulate_improvement(bottleneck.id, 'availability', 0.05)
        )
        
        # 情境 2: 改善瓶頸設備效能 5%
        scenarios.append(
            self.simulate_improvement(bottleneck.id, 'performance', 0.05)
        )
        
        # 情境 3: 改善瓶頸設備良率 2%
        scenarios.append(
            self.simulate_improvement(bottleneck.id, 'quality', 0.02)
        )
        
        # 情境 4: 全面改善所有設備可用率 3%
        total_improvement = 0
        for eq in self.equipment_list:
            result = self.simulate_improvement(eq.id, 'availability', 0.03)
            total_improvement += result['improvement']
        
        scenarios.append({
            'scenario': 'Improve all equipment availability by 3%',
            'total_improvement': total_improvement,
            'improvement_pct': (total_improvement / self.calculate_line_capacity()) * 100
        })
        
        return scenarios

# 使用範例
equipment_list = [
    Equipment('DS-001', 'DieSorter', 12000, 0.95, 0.92, 0.98),
    Equipment('DB-001', 'DieBonder', 8000, 0.93, 0.90, 0.97),
    Equipment('WB-001', 'WireBonder', 10000, 0.90, 0.88, 0.96),  # 瓶頸
    Equipment('DP-001', 'Dispenser', 6000, 0.94, 0.91, 0.98),
    Equipment('TS-001', 'Tester', 15000, 0.96, 0.93, 0.99),
]

simulator = ProductionLineSimulator(equipment_list)

# 識別瓶頸
bottleneck = simulator.identify_bottleneck()
print(f"Bottleneck: {bottleneck.id} ({bottleneck.effective_uph} UPH)")

# 模擬改善情境
scenarios = simulator.simulate_multiple_scenarios()
for scenario in scenarios:
    print(scenario)
```

### 7.3 預測性分析

#### 7.3.1 設備故障預測

```python
# equipment_failure_prediction.py
import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.preprocessing import StandardScaler
import joblib

class EquipmentFailurePredictor:
    def __init__(self):
        self.model = None
        self.scaler = StandardScaler()
        self.feature_columns = [
            'vibration_x', 'vibration_y', 'vibration_z',
            'temperature', 'current', 'voltage',
            'runtime_hours', 'cycle_count',
            'last_maintenance_days'
        ]
        
    def prepare_features(self, data):
        """準備特徵"""
        features = data[self.feature_columns].copy()
        
        # 新增衍生特徵
        features['vibration_magnitude'] = np.sqrt(
            data['vibration_x']**2 + 
            data['vibration_y']**2 + 
            data['vibration_z']**2
        )
        features['power'] = data['current'] * data['voltage']
        features['temp_current_ratio'] = data['temperature'] / (data['current'] + 1)
        
        return features
    
    def train(self, training_data, labels):
        """訓練模型"""
        features = self.prepare_features(training_data)
        
        # 標準化
        features_scaled = self.scaler.fit_transform(features)
        
        # 訓練隨機森林
        self.model = RandomForestClassifier(
            n_estimators=100,
            max_depth=10,
            random_state=42
        )
        self.model.fit(features_scaled, labels)
        
        # 特徵重要性
        feature_importance = pd.DataFrame({
            'feature': features.columns,
            'importance': self.model.feature_importances_
        }).sort_values('importance', ascending=False)
        
        return feature_importance
    
    def predict(self, equipment_data):
        """預測故障機率"""
        features = self.prepare_features(equipment_data)
        features_scaled = self.scaler.transform(features)
        
        # 預測機率
        failure_prob = self.model.predict_proba(features_scaled)[:, 1]
        
        # 預測類別
        failure_pred = self.model.predict(features_scaled)
        
        return {
            'failure_probability': failure_prob,
            'failure_prediction': failure_pred,
            'risk_level': self.get_risk_level(failure_prob)
        }
    
    def get_risk_level(self, probability):
        """根據機率判定風險等級"""
        if probability < 0.2:
            return 'LOW'
        elif probability < 0.5:
            return 'MEDIUM'
        elif probability < 0.8:
            return 'HIGH'
        else:
            return 'CRITICAL'
    
    def save_model(self, filepath):
        """儲存模型"""
        joblib.dump({
            'model': self.model,
            'scaler': self.scaler,
            'feature_columns': self.feature_columns
        }, filepath)
    
    def load_model(self, filepath):
        """載入模型"""
        saved = joblib.load(filepath)
        self.model = saved['model']
        self.scaler = saved['scaler']
        self.feature_columns = saved['feature_columns']

# 使用範例
predictor = EquipmentFailurePredictor()

# 載入訓練數據
training_data = pd.read_csv('equipment_sensor_data.csv')
labels = pd.read_csv('equipment_failures.csv')['failure']

# 訓練模型
feature_importance = predictor.train(training_data, labels)
print("Feature Importance:")
print(feature_importance)

# 儲存模型
predictor.save_model('failure_prediction_model.pkl')

# 預測
current_data = pd.DataFrame({
    'vibration_x': [0.5],
    'vibration_y': [0.3],
    'vibration_z': [0.4],
    'temperature': [75.0],
    'current': [12.5],
    'voltage': [220.0],
    'runtime_hours': [5000],
    'cycle_count': [1000000],
    'last_maintenance_days': [90]
})

prediction = predictor.predict(current_data)
print(f"Failure Probability: {prediction['failure_probability'][0]:.2%}")
print(f"Risk Level: {prediction['risk_level'][0]}")
```

---

## 8. 實施計畫

### 8.1 實施階段

#### 階段 1: 規劃與設計 (Month 1-2)

**主要工作**:
1. 需求訪談和分析
2. 使用者故事和用例設計
3. 技術架構設計
4. UI/UX 設計
5. 3D 資產準備計畫

**交付成果**:
- 需求規格文件
- 技術架構文件
- UI/UX 設計稿
- 專案計畫書

#### 階段 2: 基礎平台建置 (Month 3-4)

**主要工作**:
1. Omniverse 平台部署
2. 後端 API 開發
3. 數據庫設計和建置
4. 基礎 3D 場景建立
5. 開發環境設定

**交付成果**:
- Omniverse 平台運行
- API 服務上線
- 基礎 3D 場景
- 開發環境就緒

#### 階段 3: 3D 視覺化開發 (Month 5-7)

**主要工作**:
1. 完整工廠 3D 模型建立
2. 設備 3D 模型整合
3. 即時數據綁定開發
4. LOD 優化
5. 視覺化效果開發

**交付成果**:
- 完整 3D 工廠場景
- 340 台設備 3D 模型
- 即時數據綁定功能
- 效能優化完成

#### 階段 4: 2D 儀表板開發 (Month 6-8)

**主要工作**:
1. 高階主管駕駛艙開發
2. 生產監控儀表板開發
3. 設備效能儀表板開發
4. 能源管理儀表板開發
5. 品質管理儀表板開發

**交付成果**:
- 5+ 主要儀表板
- 30+ 圖表和視覺化
- 移動端應用

#### 階段 5: AR/VR 應用開發 (Month 7-9)

**主要工作**:
1. AR 維護指引開發
2. VR 工廠巡檢開發
3. VR 安全培訓開發
4. 設備採購和部署

**交付成果**:
- AR 維護應用
- VR 巡檢應用
- VR 培訓應用
- 10+ AR/VR 設備

#### 階段 6: 整合測試 (Month 10-11)

**主要工作**:
1. 功能測試
2. 效能測試
3. 使用者驗收測試
4. 安全性測試
5. 問題修復

**交付成果**:
- 測試報告
- 問題修復完成
- 驗收通過

#### 階段 7: 部署與培訓 (Month 12)

**主要工作**:
1. 生產環境部署
2. 使用者培訓
3. 文件交付
4. 上線支援

**交付成果**:
- 系統上線
- 使用者培訓完成
- 完整文件交付
- 專案驗收

### 8.2 時程規劃

```
Month:    1  2  3  4  5  6  7  8  9 10 11 12
          |--|--|--|--|--|--|--|--|--|--|--|--|
規劃設計  [====]
基礎平台        [====]
3D 視覺化           [========]
2D 儀表板              [========]
AR/VR 應用                [========]
整合測試                        [====]
部署培訓                           [==]

里程碑:   M1  M2  M3  M4  M5  M6  M7
```

**里程碑**:
- M1 (Month 2): 設計完成
- M2 (Month 4): 基礎平台上線
- M3 (Month 7): 3D 視覺化完成
- M4 (Month 8): 2D 儀表板完成
- M5 (Month 9): AR/VR 應用完成
- M6 (Month 11): 整合測試通過
- M7 (Month 12): 專案驗收

---

## 9. 投資與效益

### 9.1 投資成本

#### 9.1.1 硬體成本

| 項目 | 規格 | 數量 | 單價 (USD) | 總價 (USD) |
|------|------|------|-----------|-----------|
| GPU 伺服器 | 4× NVIDIA A100, 512GB RAM | 3 | 150,000 | 450,000 |
| 儲存系統 | 100TB NVMe SSD | 1 | 200,000 | 200,000 |
| HoloLens 2 | AR 眼鏡 | 10 | 3,500 | 35,000 |
| Meta Quest Pro | VR 頭盔 | 10 | 1,000 | 10,000 |
| iPad Pro | 移動設備 | 20 | 1,000 | 20,000 |
| 大型顯示器 | 86" 4K | 5 | 5,000 | 25,000 |
| 網路設備 | 10Gbps Switch | 3 | 15,000 | 45,000 |
| **小計** | - | - | - | **785,000** |

#### 9.1.2 軟體授權

| 項目 | 類型 | 數量 | 單價 (USD) | 總價 (USD) |
|------|------|------|-----------|-----------|
| NVIDIA Omniverse Enterprise | 年訂閱 | 20 Users | 20,000 | 400,000 |
| Unity Pro | 年訂閱 | 5 Licenses | 2,000 | 10,000 |
| Grafana Enterprise | 年訂閱 | 1 套 | 30,000 | 30,000 |
| **小計** | - | - | - | **440,000** |

#### 9.1.3 開發成本

| 項目 | 人月 | 單價 (USD) | 總價 (USD) |
|------|------|-----------|-----------|
| 專案管理 | 12 | 15,000 | 180,000 |
| 系統架構師 | 12 | 18,000 | 216,000 |
| 3D 建模師 | 24 | 10,000 | 240,000 |
| 前端開發工程師 | 36 | 10,000 | 360,000 |
| 後端開發工程師 | 24 | 10,000 | 240,000 |
| AR/VR 開發工程師 | 18 | 12,000 | 216,000 |
| UI/UX 設計師 | 12 | 8,000 | 96,000 |
| QA 測試工程師 | 12 | 8,000 | 96,000 |
| **小計** | - | - | **1,644,000** |

#### 9.1.4 總投資成本

| 類別 | 金額 (USD) | 佔比 |
|------|-----------|------|
| 硬體成本 | 785,000 | 27.0% |
| 軟體授權 | 440,000 | 15.1% |
| 開發成本 | 1,644,000 | 56.5% |
| 應急準備金 (5%) | 143,450 | 4.9% |
| **總計** | **3,012,450** | **100%** |

**約 USD 3.0M**

### 9.2 效益分析

#### 9.2.1 量化效益

**1. 決策效率提升**

- 高階主管決策時間縮短 30%
- 問題識別時間縮短 50%
- 年節省時間價值: **USD 500,000**

**2. 維護效率提升**

- AR 輔助維護時間縮短 25%
- 維護錯誤率降低 40%
- 年節省維護成本: **USD 300,000**

**3. 培訓成本降低**

- VR 培訓取代部分實體培訓
- 培訓時間縮短 30%
- 年節省培訓成本: **USD 200,000**

**4. 遠程協作效益**

- 減少出差次數 50%
- 提升協作效率 30%
- 年節省差旅成本: **USD 150,000**

**5. 客戶信心提升**

- 訂單轉換率提升 10%
- 年增加營收: **USD 2,000,000**

**年度總效益**: **USD 3,150,000**

#### 9.2.2 質化效益

1. **提升品牌形象**:展示先進的數位化能力
2. **吸引優秀人才**:創新的工作環境吸引人才
3. **知識管理**:視覺化知識庫促進知識傳承
4. **創新文化**:培養數據驅動和視覺化思維
5. **競爭優勢**:建立技術領先地位

### 9.3 ROI 計算

**投資回收期**:
```
總投資 = USD 3.0M
年效益 = USD 3.15M
年營運成本 = USD 0.5M (軟體訂閱 + 維護)
年淨效益 = 3.15M - 0.5M = USD 2.65M

投資回收期 = 3.0M / 2.65M = 1.13 年 ≈ 13.6 個月
```

**5 年 NPV** (折現率 10%):
```
Year 0: -3.0M
Year 1: +2.65M × 0.909 = +2.41M
Year 2: +2.65M × 0.826 = +2.19M
Year 3: +2.65M × 0.751 = +1.99M
Year 4: +2.65M × 0.683 = +1.81M
Year 5: +2.65M × 0.621 = +1.65M

NPV = -3.0M + 2.41M + 2.19M + 1.99M + 1.81M + 1.65M = USD 7.05M
```

**IRR**: **88.3%**

**ROI 總結**:

| 指標 | 數值 | 評價 |
|------|------|------|
| 投資回收期 | 13.6 個月 | 優秀 |
| 5 年 NPV | USD 7.05M | 優秀 |
| IRR | 88.3% | 優秀 |
| BCR | 3.35 | 優秀 |

---

## 10. 附錄

### 10.1 技術規格

#### 10.1.1 系統需求

**伺服器端**:
- GPU: NVIDIA A100 或以上
- CPU: 64 Core 或以上
- RAM: 512GB 或以上
- 儲存: 100TB NVMe SSD
- 網路: 10Gbps

**客戶端**:
- Web Browser: Chrome 90+, Firefox 88+, Safari 14+
- 移動設備: iOS 14+, Android 10+
- AR 設備: HoloLens 2, Magic Leap 2
- VR 設備: Meta Quest Pro, HTC Vive Pro 2

#### 10.1.2 效能指標

| 指標 | 目標值 |
|------|--------|
| 3D 場景幀率 | ≥ 60 FPS |
| 3D 場景載入時間 | < 10 秒 |
| 數據更新延遲 | < 5 秒 |
| 儀表板載入時間 | < 3 秒 |
| API 回應時間 | < 500 ms |
| 系統可用率 | ≥ 99.5% |
| 同時線上使用者 | 100+ |

### 10.2 參考資料

1. NVIDIA Omniverse Documentation: https://docs.omniverse.nvidia.com/
2. USD (Universal Scene Description): https://graphics.pixar.com/usd/docs/index.html
3. Three.js Documentation: https://threejs.org/docs/
4. D3.js Documentation: https://d3js.org/
5. Unity XR Documentation: https://docs.unity3d.com/Manual/XR.html
6. HoloLens 2 Development: https://docs.microsoft.com/en-us/hololens/
7. Meta Quest Development: https://developer.oculus.com/

### 10.3 聯絡資訊

**專案負責人**: Michael Lin  
**Email**: michael.lin@harvatek.com  
**電話**: +886-3-578-xxxx

---

**文件版本**: 1.0  
**最後更新**: 2025-10-12  
**作者**: IDTF V3.1 Team  
**核准**: Harvatek Management Team

---

**© 2025 Harvatek Corporation. All Rights Reserved.**

