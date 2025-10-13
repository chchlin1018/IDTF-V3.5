# NDH (Neutral Data Hub) 架構建議書
**MacroVision Systems - Industrial Digital Twins Framework V3.5**

**作者**：林志錚 (Chih Cheng Lin, Michael Lin)  
**日期**：2025年10月14日  
**版本**：3.5  
**文件類型**：技術架構建議書

---

## 執行摘要

NDH (Neutral Data Hub) 是 IDTF V3.5 框架的核心組件之一，定位為**供應商中立的工業數據整合中樞**。NDH 旨在解決工業領域最關鍵的痛點：**IT/OT 數據孤島、供應商鎖定、高昂的整合成本和缺乏標準化**。

### 核心價值主張

**NDH 提供**：
- ✅ **供應商中立**：開源架構，零鎖定風險
- ✅ **IT/OT 融合**：統一整合 MES、ERP、SCADA、PLC 等系統
- ✅ **可抽換時序數據庫**：支援 InfluxDB、TDengine、TimescaleDB、QuestDB
- ✅ **全生命週期覆蓋**：從設計、建造到運維的端到端解決方案
- ✅ **低成本高效能**：相較專有方案節省 70% 成本

### 關鍵技術特性

1. **微服務架構**：模組化設計，可獨立擴展和維護
2. **統一數據模型**：基於 IADL 的標準化資產描述
3. **即時數據處理**：支援百萬級數據點/秒的高吞吐量
4. **即時數據處理**：支援百萬級數據點/秒的高吞吐量，即時異常檢測和告警
5. **開放 API**：RESTful API、GraphQL、WebSocket 全面支援

### 預期效益

**技術效益**：
- 數據整合時間 ↓ 80%
- 系統互操作性 ↑ 95%
- 部署自動化程度 ↑ 95%

**商業效益**：
- 整合成本 ↓ 70%
- 供應商風險 ↓ 100%（零鎖定）
- ROI：6-12 個月回收期

---

## 目錄

1. [引言](#1-引言)
2. [NDH 核心架構](#2-ndh-核心架構)
3. [分層架構設計](#3-分層架構設計)
4. [可抽換時序數據庫架構](#4-可抽換時序數據庫架構)
5. [企業應用整合（IT/OT 融合）](#5-企業應用整合itot-融合)
6. [核心服務模組](#6-核心服務模組)
7. [API 設計與介面規範](#7-api-設計與介面規範)
8. [數據流與互動流程](#8-數據流與互動流程)
9. [部署架構與擴展性](#9-部署架構與擴展性)
10. [安全性與合規性](#10-安全性與合規性)
11. [效能指標與優化](#11-效能指標與優化)
12. [實施路線圖](#12-實施路線圖)
13. [附錄](#13-附錄)

---

## 1. 引言

### 1.1 背景與動機

工業數位化轉型面臨的核心挑戰是**數據碎片化**和**系統孤島化**。企業內部存在大量異構系統：

**IT 層系統**：
- ERP（Enterprise Resource Planning）
- MES（Manufacturing Execution System）
- PLM（Product Lifecycle Management）
- WMS（Warehouse Management System）
- QMS（Quality Management System）

**OT 層系統**：
- SCADA（Supervisory Control and Data Acquisition）
- PLC（Programmable Logic Controller）
- DCS（Distributed Control System）
- HMI（Human-Machine Interface）
- 各類感測器和執行器

這些系統通常來自不同供應商，使用不同的通訊協議（OPC UA、Modbus、MQTT、Profinet 等），數據格式各異，導致：

1. **數據孤島**：IT 和 OT 系統無法有效溝通
2. **整合成本高**：每個系統對接需要客製化開發
3. **供應商鎖定**：依賴特定廠商的專有技術
4. **缺乏標準化**：無統一的數據模型和介面規範
5. **決策延遲**：CEO 無法即時獲得整體營運數據

### 1.2 NDH 的定位

NDH 定位為**中立的數據整合中樞**，扮演以下角色：

1. **數據橋接器**：連接 IT 和 OT 系統，實現雙向數據流
2. **標準化層**：將異構數據轉換為統一的 IADL 格式
3. **即時監控引擎**：提供即時數據採集、異常檢測和告警能力
4. **協作平台**：支援多用戶即時協同和版本控制
5. **開放生態**：提供標準 API，方便第三方整合

### 1.3 設計原則

NDH 的設計遵循以下核心原則：

1. **供應商中立**：開源架構，避免任何形式的鎖定
2. **可負擔性**：低成本，中小企業也能負擔
3. **可擴展性**：模組化設計，可根據需求靈活擴展
4. **高效能**：支援百萬級數據點/秒的吞吐量
5. **易用性**：提供直觀的 API 和管理介面
6. **安全性**：內建認證、授權和加密機制

---

## 2. NDH 核心架構

### 2.1 架構總覽

NDH 採用**分層式微服務架構**，由以下核心層組成：

```
┌─────────────────────────────────────────────────────────────────┐
│                    應用層 (Application Layer)                    │
│  Digital Twin Apps, Dashboards, Analytics, AI/ML, Omniverse     │
├─────────────────────────────────────────────────────────────────┤
│                      API 層 (API Layer)                          │
│  RESTful API, GraphQL, WebSocket, gRPC                          │
│  - /api/v1/assets, /api/v1/timeseries, /api/v1/analytics       │
├─────────────────────────────────────────────────────────────────┤
│                   服務層 (Service Layer) ⭐                      │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐      │
│  │ Asset    │ MEP      │ Conflict │ Flow     │ Collab   │      │
│  │ Servants │ Design   │ Detection│ Analysis │ Service  │      │
│  └──────────┴──────────┴──────────┴──────────┴──────────┘      │
├─────────────────────────────────────────────────────────────────┤
│              數據管理層 (Data Management Layer) ⭐               │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  時序數據庫抽象層 (TSDB Abstraction Layer)                │  │
│  │  - ITSDBWriter, ITSDBReader, ITSDBAdmin                  │  │
│  └──────────────────────────────────────────────────────────┘  │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐      │
│  │ InfluxDB │ TDengine │Timescale │ QuestDB  │ Others   │      │
│  │ Adapter  │ Adapter  │ Adapter  │ Adapter  │          │      │
│  └──────────┴──────────┴──────────┴──────────┴──────────┘      │
├─────────────────────────────────────────────────────────────────┤
│            企業應用整合層 (Enterprise Integration Layer)         │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐      │
│  │ MES      │ ERP      │ PLM      │ WMS      │ QMS      │      │
│  │ Connector│ Connector│ Connector│ Connector│ Connector│      │
│  └──────────┴──────────┴──────────┴──────────┴──────────┘      │
├─────────────────────────────────────────────────────────────────┤
│            OT 系統整合層 (OT Integration Layer)                  │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐      │
│  │ OPC UA   │ Modbus   │ MQTT     │ Profinet │ BACnet   │      │
│  │ Connector│ Connector│ Connector│ Connector│ Connector│      │
│  └──────────┴──────────┴──────────┴──────────┴──────────┘      │
├─────────────────────────────────────────────────────────────────┤
│                  基礎設施層 (Infrastructure Layer)               │
│  訊息佇列 (Redis/Kafka), 配置中心, 服務發現, 監控告警          │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 核心組件說明

#### 2.2.1 應用層
- **Digital Twin Apps**：數位分身應用程式
- **Dashboards**：視覺化儀表板
- **Analytics**：數據分析和報表
- **AI/ML**：機器學習和預測性維護
- **Omniverse**：3D 視覺化和協作平台

#### 2.2.2 API 層
- **RESTful API**：標準的 HTTP API，適合 CRUD 操作
- **GraphQL**：靈活的查詢語言，減少過度獲取
- **WebSocket**：即時雙向通訊
- **gRPC**：高效能的 RPC 框架（內部服務間通訊）

#### 2.2.3 服務層
- **Asset Servants**：資產執行時模型，管理資產生命週期
- **MEP Design Service**：機電管線設計服務
- **Conflict Detection Service**：衝突檢測服務
- **Flow Analysis Service**：流體分析服務
- **Collaboration Service**：即時協同服務

#### 2.2.4 數據管理層
- **時序數據庫抽象層**：統一的時序數據庫介面
- **適配器層**：支援多種時序數據庫（InfluxDB、TDengine、TimescaleDB、QuestDB）
- **IADL 解析器**：解析 IADL 資產描述檔案
- **數據轉換引擎**：異構數據格式轉換

#### 2.2.5 企業應用整合層
- **MES Connector**：製造執行系統連接器
- **ERP Connector**：企業資源規劃連接器
- **PLM Connector**：產品生命週期管理連接器
- **WMS Connector**：倉儲管理系統連接器
- **QMS Connector**：品質管理系統連接器

#### 2.2.6 OT 系統整合層
- **OPC UA Connector**：工業標準通訊協議
- **Modbus Connector**：工業現場總線協議
- **MQTT Connector**：輕量級物聯網協議
- **Profinet Connector**：工業乙太網協議
- **BACnet Connector**：樓宇自動化協議

#### 2.2.7 基礎設施層
- **訊息佇列**：Redis（輕量級）或 Kafka（高吞吐量）
- **配置中心**：集中管理配置（Consul/etcd）
- **服務發現**：動態服務註冊和發現
- **監控告警**：Prometheus + Grafana

---

## 3. 分層架構設計

### 3.1 數據管理層詳細設計

數據管理層是 NDH 的核心，負責所有數位分身相關數據的攝取、儲存、管理和標準化。

#### 3.1.1 IADL 解析器

**職責**：
- 解析符合 IADL 標準的資產描述檔案（YAML/JSON）
- 將 IADL 轉換為 NDH 內部數據結構
- 驗證 IADL 檔案的合法性和完整性

**核心功能**：
```python
class IADLParser:
    """IADL 解析器"""
    
    def parse_file(self, iadl_file_path: str) -> Asset:
        """解析 IADL 檔案"""
        pass
    
    def validate(self, iadl_data: dict) -> bool:
        """驗證 IADL 數據"""
        pass
    
    def to_internal_model(self, iadl_data: dict) -> Asset:
        """轉換為內部模型"""
        pass
```

#### 3.1.2 數據庫系統

**混合式儲存策略**：

1. **關係型數據庫**（PostgreSQL）：
   - 資產元數據（Asset Metadata）
   - 用戶和權限管理
   - 配置數據
   - 審計日誌

2. **時序數據庫**（可抽換）：
   - 感測器數據
   - 設備運行數據
   - 效能指標
   - 歷史趨勢

3. **文件型數據庫**（MongoDB）：
   - 非結構化數據
   - JSON 文檔
   - 日誌數據

4. **分散式檔案儲存**（MinIO/S3）：
   - 3D 模型檔案
   - 點雲數據
   - 圖片和影片
   - 備份檔案

#### 3.1.3 數據轉換引擎

**職責**：
- 異構數據格式轉換（OPC UA → IADL、Modbus → IADL）
- 數據清洗和標準化
- 數據聚合和降採樣

**轉換流程**：
```
原始數據 → 協議解析 → 數據映射 → IADL 格式 → 存儲
```

### 3.2 服務層詳細設計

**重要說明**：NDH 專注於**執行時（Runtime）**的數據整合和管理。設計階段（Design Time）的功能，如 MEP 設計、衝突檢測、ISO 圖面生成、流體分析等，屬於 **FDL Editor（工廠設計編輯器）** 的職責，不在 NDH 架構範圍內。

NDH 服務層的核心職責：
- ✅ Asset Servants（資產執行時模型）
- ✅ 數據採集與儲存（從 PLC/SCADA 採集數據）
- ✅ IT/OT 整合（MES/ERP/PLM 連接器）
- ✅ 即時監控與告警（異常檢測、事件觸發）
- ✅ API 服務（提供數據訪問介面）
- ✅ 即時協同服務（執行時協作）

服務層是 NDH 的核心功能實現，提供各種專業的工程分析與協作服務。

#### 3.2.1 Asset Servants（資產執行時模型）

**概念**：
Asset Servants 是資產在執行時的數位化身，負責：
- 管理資產的生命週期（創建、更新、刪除）
- 提供統一的 API 介面
- 處理資產的數據讀寫
- 執行資產的行為邏輯

**核心介面**：
```python
class AssetServant:
    """資產執行時模型"""
    
    def __init__(self, asset_id: str, iadl_definition: dict):
        self.asset_id = asset_id
        self.iadl_definition = iadl_definition
        self.state = {}
        self.properties = {}
        self.telemetry = {}
    
    async def read_property(self, property_name: str) -> Any:
        """讀取屬性"""
        pass
    
    async def write_property(self, property_name: str, value: Any) -> bool:
        """寫入屬性"""
        pass
    
    async def invoke_command(self, command_name: str, params: dict) -> Any:
        """調用命令"""
        pass
    
    async def get_telemetry(self, telemetry_name: str, 
                           start: datetime, end: datetime) -> List[dict]:
        """獲取遙測數據"""
        pass
```

**生命週期管理**：
```
創建 → 初始化 → 運行 → 暫停 → 恢復 → 停止 → 銷毀
```

#### 3.2.2 即時協同服務

**職責**：
- 提供多用戶在同一 3D 環境下進行即時協同作業的能力
- 模型審閱、批註、版本控制和任務分配

**技術實現**：

**與 Omniverse 整合**：
1. **USD（Universal Scene Description）整合**：
   - NDH 內部數據模型與 USD 映射
   - 所有設計數據以 USD 格式在 Omniverse 中發布和訂閱

2. **版本控制與審閱**：
   - 追蹤所有協同編輯的歷史記錄
   - 批註、標記和問題追蹤

3. **多用戶同步**：
   - 即時位置同步
   - 視角同步
   - 模型操作同步

---

## 4. 可抽換時序數據庫架構

### 4.1 設計動機

工業場域對時序數據庫的需求各異：
- **高吞吐量場景**：半導體廠、數據中心（需要 TDengine）
- **低成本場景**：中小企業（需要 QuestDB）
- **雲端部署**：AWS、Azure（需要 InfluxDB Cloud）
- **本地部署**：私有雲、邊緣計算（需要 TimescaleDB）

**供應商鎖定風險**：
- 依賴單一數據庫廠商
- 無法根據需求靈活切換
- 供應鏈中斷風險

**解決方案**：
- 設計可抽換的時序數據庫架構
- 支援多種主流時序數據庫
- 零停機在線遷移

### 4.2 架構設計

#### 4.2.1 時序數據庫抽象層

**統一介面定義**：

```python
from abc import ABC, abstractmethod
from typing import List, Dict, Any
from datetime import datetime

class ITSDBWriter(ABC):
    """時序數據庫寫入介面"""
    
    @abstractmethod
    async def write(self, measurement: str, tags: Dict[str, str], 
                   fields: Dict[str, Any], timestamp: datetime) -> bool:
        """寫入單條數據"""
        pass
    
    @abstractmethod
    async def batch_write(self, points: List[Dict]) -> bool:
        """批量寫入數據"""
        pass

class ITSDBReader(ABC):
    """時序數據庫讀取介面"""
    
    @abstractmethod
    async def query(self, query: str, params: Dict = None) -> List[Dict]:
        """執行查詢"""
        pass
    
    @abstractmethod
    async def query_range(self, measurement: str, tags: Dict, 
                         start: datetime, end: datetime, 
                         fields: List[str] = None) -> List[Dict]:
        """範圍查詢"""
        pass
    
    @abstractmethod
    async def aggregate(self, measurement: str, tags: Dict,
                       start: datetime, end: datetime,
                       aggregation: str, interval: str) -> List[Dict]:
        """聚合查詢"""
        pass

class ITSDBAdmin(ABC):
    """時序數據庫管理介面"""
    
    @abstractmethod
    async def create_measurement(self, name: str, schema: Dict) -> bool:
        """創建 measurement/table"""
        pass
    
    @abstractmethod
    async def health_check(self) -> Dict:
        """健康檢查"""
        pass

class ITSDBAdapter(ITSDBWriter, ITSDBReader, ITSDBAdmin):
    """完整的時序數據庫適配器介面"""
    
    @abstractmethod
    async def connect(self, config: Dict) -> bool:
        """連接數據庫"""
        pass
    
    @abstractmethod
    async def disconnect(self) -> bool:
        """斷開連接"""
        pass
    
    @abstractmethod
    def get_capabilities(self) -> Dict:
        """獲取數據庫能力"""
        pass
```

#### 4.2.2 適配器管理器

**職責**：
- 載入和管理適配器
- 根據配置選擇適配器
- 支援多適配器並行運行（雙寫）
- 提供適配器切換功能

**實現**：

```python
class TSDBAdapterManager:
    """時序數據庫適配器管理器"""
    
    def __init__(self, config: Dict):
        self.config = config
        self.adapters: Dict[str, ITSDBAdapter] = {}
        self.primary_adapter: Optional[ITSDBAdapter] = None
        self.secondary_adapters: List[ITSDBAdapter] = []
    
    async def initialize(self):
        """初始化適配器"""
        # 載入主適配器
        primary_config = self.config['primary']
        self.primary_adapter = await self._load_adapter(
            primary_config['type'], 
            primary_config
        )
        
        # 載入次要適配器（用於雙寫）
        if 'secondary' in self.config:
            for secondary_config in self.config['secondary']:
                adapter = await self._load_adapter(
                    secondary_config['type'],
                    secondary_config
                )
                self.secondary_adapters.append(adapter)
    
    async def write(self, measurement: str, tags: Dict, 
                   fields: Dict, timestamp: datetime) -> bool:
        """寫入數據（支援雙寫）"""
        # 寫入主數據庫
        success = await self.primary_adapter.write(
            measurement, tags, fields, timestamp
        )
        
        # 寫入次要數據庫（非阻塞）
        for adapter in self.secondary_adapters:
            try:
                await adapter.write(measurement, tags, fields, timestamp)
            except Exception as e:
                logger.warning(f"Secondary write failed: {e}")
        
        return success
    
    async def switch_primary(self, new_primary_type: str):
        """切換主數據庫"""
        if new_primary_type not in self.adapters:
            raise ValueError(f"Adapter {new_primary_type} not loaded")
        
        old_primary = self.primary_adapter
        self.primary_adapter = self.adapters[new_primary_type]
        
        # 將舊的主數據庫加入次要數據庫列表
        if old_primary not in self.secondary_adapters:
            self.secondary_adapters.append(old_primary)
        
        logger.info(f"Switched primary adapter to {new_primary_type}")
```

### 4.3 支援的時序數據庫

#### 4.3.1 InfluxDB 2.x/3.x

**特性**：
- ✅ 開源（OSS）和企業版（Enterprise）
- ✅ Flux 查詢語言（強大但學習曲線陡）
- ✅ 良好的壓縮比（7:1）
- ❌ 開源版不支援集群

**適用場景**：
- 雲端部署（InfluxDB Cloud）
- 中小型專案
- 需要 Flux 查詢能力

**效能指標**：
- 寫入吞吐量：200K points/sec
- 查詢延遲：中等
- 壓縮比：7:1

#### 4.3.2 TDengine 3.x

**特性**：
- ✅ 高效能（寫入吞吐量 1.5M points/sec）
- ✅ 超高壓縮比（16:1）
- ✅ 原生集群支援
- ✅ SQL 查詢（易於學習）
- ✅ 中國開源專案，社群活躍

**適用場景**：
- 高吞吐量場景（半導體廠、數據中心）
- 大規模部署（數千台設備）
- 需要集群和高可用性

**效能指標**：
- 寫入吞吐量：1.5M points/sec
- 查詢延遲：低
- 壓縮比：16:1

#### 4.3.3 TimescaleDB 2.x

**特性**：
- ✅ 基於 PostgreSQL（熟悉的 SQL）
- ✅ 豐富的生態系統（PostGIS、pgAdmin）
- ✅ 企業版支援集群
- ❌ 效能略遜於專用時序數據庫

**適用場景**：
- 已有 PostgreSQL 基礎設施
- 需要關聯查詢（JOIN）
- 地理空間數據（GIS）

**效能指標**：
- 寫入吞吐量：600K points/sec
- 查詢延遲：中等
- 壓縮比：10:1

#### 4.3.4 QuestDB

**特性**：
- ✅ 超高效能（寫入吞吐量 4M points/sec）
- ✅ 低延遲查詢
- ✅ SQL 查詢
- ✅ 開源且免費
- ❌ 社群相對較小

**適用場景**：
- 極高效能需求
- 低成本部署
- 邊緣計算

**效能指標**：
- 寫入吞吐量：4M points/sec
- 查詢延遲：極低
- 壓縮比：8:1

### 4.4 數據庫選型建議

| 場景 | 推薦數據庫 | 理由 |
|------|-----------|------|
| **半導體廠** | TDengine | 高吞吐量、高壓縮比、集群支援 |
| **數據中心** | TDengine / QuestDB | 高效能、低延遲 |
| **中小企業** | QuestDB / InfluxDB OSS | 低成本、易部署 |
| **雲端部署** | InfluxDB Cloud | 託管服務、無需維護 |
| **已有 PostgreSQL** | TimescaleDB | 無縫整合、熟悉的 SQL |
| **邊緣計算** | QuestDB | 低資源消耗、高效能 |
| **地理空間數據** | TimescaleDB + PostGIS | GIS 功能 |

### 4.5 零停機遷移策略

**雙寫策略**：
1. **階段 1**：啟用雙寫（主數據庫 + 新數據庫）
2. **階段 2**：歷史數據遷移（批量導出/導入）
3. **階段 3**：驗證數據一致性
4. **階段 4**：切換主數據庫
5. **階段 5**：停止雙寫，移除舊數據庫

**配置範例**：

```yaml
# ndh_config.yaml
tsdb:
  primary:
    type: influxdb
    url: http://localhost:8086
    token: my-token
    org: my-org
    bucket: ndh
  
  secondary:
    - type: tdengine
      host: localhost
      port: 6030
      user: root
      password: taosdata
      database: ndh
```

---

## 5. 企業應用整合（IT/OT 融合）

### 5.1 IT/OT 融合的挑戰

**IT 和 OT 的差異**：

| 維度 | IT 系統 | OT 系統 |
|------|---------|---------|
| **主要目標** | 數據處理和管理 | 物理過程控制 |
| **時間敏感性** | 秒級 | 毫秒級 |
| **安全重點** | 數據保密性 | 系統可用性 |
| **更新頻率** | 頻繁（每月） | 罕見（每年） |
| **標準協議** | HTTP, SQL, REST | OPC UA, Modbus, Profinet |
| **供應商** | SAP, Oracle, Microsoft | Siemens, Rockwell, Schneider |

**融合挑戰**：
1. **協議不兼容**：IT 使用 HTTP/REST，OT 使用 OPC UA/Modbus
2. **數據格式差異**：IT 使用 JSON/XML，OT 使用二進制協議
3. **安全要求不同**：IT 重視保密性，OT 重視可用性
4. **組織孤島**：IT 和 OT 團隊通常獨立運作，甚至競爭

### 5.2 NDH 的 IT/OT 融合架構

**NDH 作為中立第三方**，協調 IT 和 OT 系統：

```
┌─────────────────────────────────────────────────────────────┐
│                    IT 層（Information Technology）           │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐  │
│  │ ERP      │ MES      │ PLM      │ WMS      │ QMS      │  │
│  │ (SAP)    │(Siemens) │ (PTC)    │(Oracle)  │(Minitab) │  │
│  └────┬─────┴────┬─────┴────┬─────┴────┬─────┴────┬─────┘  │
├───────┼──────────┼──────────┼──────────┼──────────┼─────────┤
│       │          │          │          │          │         │
│       ▼          ▼          ▼          ▼          ▼         │
│  ┌────────────────────────────────────────────────────────┐ │
│  │         NDH 企業應用連接器層                            │ │
│  │  - ERP Connector (SAP, Oracle, Microsoft Dynamics)     │ │
│  │  - MES Connector (Siemens, Rockwell, AVEVA)           │ │
│  │  - PLM Connector (PTC, Siemens, Dassault)             │ │
│  │  - WMS/QMS/CMMS Connectors                            │ │
│  └────────────────────────────────────────────────────────┘ │
│                            ▲  ▼                              │
│  ┌────────────────────────────────────────────────────────┐ │
│  │         NDH 核心數據中樞                                │ │
│  │  - 統一數據模型 (IADL)                                 │ │
│  │  - 數據轉換引擎                                        │ │
│  │  - 時序數據庫                                          │ │
│  │  - Asset Servants                                     │ │
│  └────────────────────────────────────────────────────────┘ │
│                            ▲  ▼                              │
│  ┌────────────────────────────────────────────────────────┐ │
│  │         NDH OT 系統連接器層                             │ │
│  │  - OPC UA Connector                                    │ │
│  │  - Modbus Connector                                    │ │
│  │  - MQTT Connector                                      │ │
│  │  - Profinet/BACnet Connectors                         │ │
│  └────────────────────────────────────────────────────────┘ │
│       │          │          │          │          │         │
│       ▼          ▼          ▼          ▼          ▼         │
│  ┌────┴─────┬────┴─────┬────┴─────┬────┴─────┬────┴─────┐  │
│  │ SCADA    │ PLC      │ DCS      │ Sensors  │ Actuators│  │
│  │(AVEVA)   │(Siemens) │ (ABB)    │          │          │  │
│  └──────────┴──────────┴──────────┴──────────┴──────────┘  │
│                    OT 層（Operational Technology）           │
└─────────────────────────────────────────────────────────────┘
```

### 5.3 企業應用連接器詳細設計

#### 5.3.1 MES 連接器

**支援的 MES 系統**：
- Siemens Opcenter (ex-Camstar)
- Rockwell FactoryTalk
- AVEVA MES
- Dassault DELMIA
- SAP ME

**數據流向**：

**從 MES 獲取**：
- 生產訂單（Production Orders）
- 工藝路線（Routing）
- 物料清單（BOM）
- 生產進度（Production Progress）
- 品質數據（Quality Data）

**向 MES 回報**：
- 設備狀態（Equipment Status）
- 生產數量（Production Count）
- 品質檢測結果（Quality Results）
- 異常事件（Alarms/Events）

**同步機制**：
- 輪詢（Polling）：每 1-5 分鐘
- 事件驅動（Event-Driven）：即時推送
- 批次同步（Batch Sync）：每小時/每日

**連接器實現**：

```python
class MESConnector(BaseConnector):
    """MES 連接器"""
    
    async def get_production_orders(self, 
                                   start_date: datetime, 
                                   end_date: datetime) -> List[ProductionOrder]:
        """獲取生產訂單"""
        pass
    
    async def report_production_count(self, 
                                     order_id: str, 
                                     quantity: int) -> bool:
        """回報生產數量"""
        pass
    
    async def report_quality_result(self, 
                                   order_id: str, 
                                   result: QualityResult) -> bool:
        """回報品質檢測結果"""
        pass
```

#### 5.3.2 ERP 連接器

**支援的 ERP 系統**：
- SAP S/4HANA
- Oracle ERP Cloud
- Microsoft Dynamics 365
- Infor CloudSuite

**數據流向**：

**從 ERP 獲取**：
- 物料主數據（Material Master）
- 供應商資訊（Vendor Info）
- 採購訂單（Purchase Orders）
- 庫存數據（Inventory Data）

**向 ERP 回報**：
- 生產完工（Production Completion）
- 物料消耗（Material Consumption）
- 設備維護成本（Maintenance Costs）
- 能源消耗（Energy Consumption）

**同步機制**：
- API 整合（REST/SOAP）
- 中間件（Middleware）：SAP PI/PO
- 檔案交換（File Exchange）：CSV/XML

#### 5.3.3 PLM 連接器

**支援的 PLM 系統**：
- PTC Windchill
- Siemens Teamcenter
- Dassault ENOVIA
- Autodesk Fusion Lifecycle

**數據流向**：

**從 PLM 獲取**：
- 產品設計數據（CAD Models）
- 工程變更單（ECO）
- 物料清單（BOM）
- 技術文件（Technical Documents）

**向 PLM 回報**：
- 製造回饋（Manufacturing Feedback）
- 設計變更建議（Design Change Requests）
- 實際生產數據（As-Built Data）

#### 5.3.4 WMS 連接器

**支援的 WMS 系統**：
- SAP EWM
- Oracle WMS
- Manhattan Associates
- Blue Yonder (ex-JDA)

**數據流向**：

**從 WMS 獲取**：
- 庫存位置（Inventory Locations）
- 入庫/出庫單據（Inbound/Outbound Orders）
- 庫存數量（Stock Levels）

**向 WMS 回報**：
- 物料消耗（Material Consumption）
- 生產完工入庫（Finished Goods Receipt）

#### 5.3.5 QMS 連接器

**支援的 QMS 系統**：
- Minitab Quality Companion
- ETQ Reliance
- MasterControl
- Sparta Systems TrackWise

**數據流向**：

**從 QMS 獲取**：
- 品質標準（Quality Standards）
- 檢驗計劃（Inspection Plans）
- 不合格品處理（NCR）

**向 QMS 回報**：
- 檢驗結果（Inspection Results）
- SPC 數據（Statistical Process Control）
- 異常事件（Quality Events）

### 5.4 數據流範例

#### 5.4.1 生產流程數據流

```
ERP → MES → NDH → 設備 → NDH → QMS → ERP

1. ERP 下達生產訂單 → MES
2. MES 分解為工單 → NDH
3. NDH 下發參數到設備（PLC）
4. 設備回報生產數據 → NDH
5. NDH 聚合數據 → QMS（品質檢測）
6. QMS 回報合格/不合格 → NDH
7. NDH 回報生產完工 → MES → ERP
```

#### 5.4.2 維護流程數據流

```
感測器 → NDH → AI 預測 → CMMS → ERP

1. 感測器監測設備振動/溫度 → NDH
2. NDH 儲存時序數據
3. AI 模型分析異常 → 預測性維護建議
4. NDH 創建維護工單 → CMMS
5. CMMS 派工 → 技術人員
6. 維護完成後回報 → CMMS → ERP（成本核算）
```

#### 5.4.3 物料流程數據流

```
ERP → WMS → MES → NDH → ERP

1. ERP 下達採購訂單 → 供應商
2. 供應商發貨 → WMS（入庫）
3. MES 請求物料 → WMS（出庫）
4. WMS 配送到產線 → NDH（追蹤）
5. NDH 回報物料消耗 → MES → ERP（庫存扣減）
```

### 5.5 整合效益

**消除數據孤島**：
- IT 和 OT 系統無縫連接
- 數據自動流轉，無需人工轉錄
- 減少數據不一致和錯誤

**即時決策支援**：
- CEO 可即時查看整體營運數據
- 生產、品質、庫存、財務數據統一視圖
- 快速響應市場變化

**流程自動化**：
- 生產訂單自動下發
- 品質檢測結果自動回報
- 維護工單自動創建

**數據一致性**：
- 單一數據源（Single Source of Truth）
- 避免多系統數據不一致
- 審計追蹤完整

---

## 6. 核心服務模組

### 6.1 Asset Servants 詳細設計

#### 6.1.1 概念與架構

Asset Servants 是 NDH 的核心創新，將 IADL 定義的靜態資產描述轉換為動態的執行時模型。

**核心概念**：
- 每個資產在 NDH 中都有一個對應的 Asset Servant
- Asset Servant 負責管理資產的生命週期和行為
- 提供統一的 API 介面，隱藏底層複雜性

**架構圖**：

```
┌─────────────────────────────────────────────────────────────┐
│                   Asset Servant Manager                      │
│  - 創建/銷毀 Asset Servants                                  │
│  - 管理 Servant 生命週期                                     │
│  - 提供 Servant 註冊和發現                                   │
└────────────────────┬────────────────────────────────────────┘
                     │
         ┌───────────┴───────────┬───────────┬───────────┐
         ▼                       ▼           ▼           ▼
    ┌─────────┐            ┌─────────┐ ┌─────────┐ ┌─────────┐
    │ Pump    │            │ Valve   │ │ Sensor  │ │ Motor   │
    │ Servant │            │ Servant │ │ Servant │ │ Servant │
    └────┬────┘            └────┬────┘ └────┬────┘ └────┬────┘
         │                      │           │           │
         ▼                      ▼           ▼           ▼
    ┌─────────────────────────────────────────────────────────┐
    │              Underlying Systems                          │
    │  PLC, SCADA, MES, Sensors, Actuators                    │
    └─────────────────────────────────────────────────────────┘
```

#### 6.1.2 Asset Servant 類別設計

```python
from enum import Enum
from typing import Dict, List, Any, Optional
from datetime import datetime

class AssetState(Enum):
    """資產狀態"""
    CREATED = "created"
    INITIALIZING = "initializing"
    RUNNING = "running"
    PAUSED = "paused"
    STOPPED = "stopped"
    ERROR = "error"
    DESTROYED = "destroyed"

class AssetServant:
    """資產執行時模型"""
    
    def __init__(self, asset_id: str, iadl_definition: dict):
        self.asset_id = asset_id
        self.iadl_definition = iadl_definition
        self.state = AssetState.CREATED
        
        # 從 IADL 提取資訊
        self.properties = iadl_definition.get('properties', {})
        self.telemetry = iadl_definition.get('telemetry', {})
        self.commands = iadl_definition.get('commands', {})
        self.relationships = iadl_definition.get('relationships', [])
        
        # 執行時數據
        self.current_values = {}
        self.telemetry_buffer = []
        self.event_handlers = {}
    
    async def initialize(self):
        """初始化"""
        self.state = AssetState.INITIALIZING
        
        # 連接到底層系統（PLC、SCADA 等）
        await self._connect_to_underlying_systems()
        
        # 載入初始屬性值
        await self._load_initial_properties()
        
        # 啟動遙測數據收集
        await self._start_telemetry_collection()
        
        self.state = AssetState.RUNNING
        logger.info(f"Asset {self.asset_id} initialized successfully")
    
    async def read_property(self, property_name: str) -> Any:
        """讀取屬性"""
        if property_name not in self.properties:
            raise ValueError(f"Property {property_name} not found")
        
        # 從底層系統讀取最新值
        value = await self._read_from_underlying_system(property_name)
        self.current_values[property_name] = value
        
        return value
    
    async def write_property(self, property_name: str, value: Any) -> bool:
        """寫入屬性"""
        if property_name not in self.properties:
            raise ValueError(f"Property {property_name} not found")
        
        # 驗證值的類型和範圍
        property_def = self.properties[property_name]
        if not self._validate_value(value, property_def):
            raise ValueError(f"Invalid value for {property_name}")
        
        # 寫入到底層系統
        success = await self._write_to_underlying_system(property_name, value)
        
        if success:
            self.current_values[property_name] = value
            # 觸發屬性變更事件
            await self._emit_event('property_changed', {
                'property': property_name,
                'value': value,
                'timestamp': datetime.utcnow()
            })
        
        return success
    
    async def invoke_command(self, command_name: str, params: dict) -> Any:
        """調用命令"""
        if command_name not in self.commands:
            raise ValueError(f"Command {command_name} not found")
        
        command_def = self.commands[command_name]
        
        # 驗證參數
        if not self._validate_params(params, command_def.get('parameters', {})):
            raise ValueError(f"Invalid parameters for {command_name}")
        
        # 執行命令
        result = await self._execute_command(command_name, params)
        
        # 記錄命令執行
        await self._log_command_execution(command_name, params, result)
        
        return result
    
    async def get_telemetry(self, telemetry_name: str, 
                           start: datetime, end: datetime) -> List[dict]:
        """獲取遙測數據"""
        if telemetry_name not in self.telemetry:
            raise ValueError(f"Telemetry {telemetry_name} not found")
        
        # 從時序數據庫查詢
        tsdb = get_tsdb_adapter()
        data = await tsdb.query_range(
            measurement=f"asset_{self.asset_id}",
            tags={'telemetry': telemetry_name},
            start=start,
            end=end
        )
        
        return data
    
    async def get_state(self) -> Dict:
        """獲取當前狀態"""
        return {
            'asset_id': self.asset_id,
            'state': self.state.value,
            'properties': self.current_values,
            'last_update': datetime.utcnow().isoformat()
        }
    
    async def pause(self):
        """暫停"""
        self.state = AssetState.PAUSED
        await self._stop_telemetry_collection()
    
    async def resume(self):
        """恢復"""
        self.state = AssetState.RUNNING
        await self._start_telemetry_collection()
    
    async def stop(self):
        """停止"""
        self.state = AssetState.STOPPED
        await self._disconnect_from_underlying_systems()
    
    async def destroy(self):
        """銷毀"""
        await self.stop()
        self.state = AssetState.DESTROYED
        # 清理資源
        self.current_values.clear()
        self.telemetry_buffer.clear()
        self.event_handlers.clear()
```

#### 6.1.3 Asset Servant Manager

```python
class AssetServantManager:
    """Asset Servant 管理器"""
    
    def __init__(self):
        self.servants: Dict[str, AssetServant] = {}
        self.iadl_parser = IADLParser()
    
    async def create_servant(self, iadl_file_path: str) -> AssetServant:
        """從 IADL 文件創建 Asset Servant"""
        # 解析 IADL 文件
        iadl_definition = self.iadl_parser.parse_file(iadl_file_path)
        asset_id = iadl_definition['id']
        
        # 創建 Asset Servant
        servant = AssetServant(asset_id, iadl_definition)
        await servant.initialize()
        
        # 註冊到管理器
        self.servants[asset_id] = servant
        
        logger.info(f"Created Asset Servant for {asset_id}")
        return servant
    
    async def get_servant(self, asset_id: str) -> Optional[AssetServant]:
        """獲取 Asset Servant"""
        return self.servants.get(asset_id)
    
    async def destroy_servant(self, asset_id: str) -> bool:
        """銷毀 Asset Servant"""
        if asset_id not in self.servants:
            return False
        
        servant = self.servants[asset_id]
        await servant.destroy()
        del self.servants[asset_id]
        
        logger.info(f"Destroyed Asset Servant for {asset_id}")
        return True
    
    async def list_servants(self) -> List[str]:
        """列出所有 Asset Servants"""
        return list(self.servants.keys())
    
    async def health_check(self) -> Dict:
        """健康檢查"""
        total = len(self.servants)
        running = sum(1 for s in self.servants.values() if s.state == AssetState.RUNNING)
        paused = sum(1 for s in self.servants.values() if s.state == AssetState.PAUSED)
        error = sum(1 for s in self.servants.values() if s.state == AssetState.ERROR)
        
        return {
            'total_servants': total,
            'running': running,
            'paused': paused,
            'error': error,
            'health': 'healthy' if error == 0 else 'degraded'
        }
```

### 6.2 連接器架構

#### 6.2.1 基礎連接器類別

```python
from abc import ABC, abstractmethod
from enum import Enum

class ConnectorStatus(Enum):
    """連接器狀態"""
    DISCONNECTED = "disconnected"
    CONNECTING = "connecting"
    CONNECTED = "connected"
    ERROR = "error"

class BaseConnector(ABC):
    """基礎連接器類別"""
    
    def __init__(self, connector_id: str, config: Dict):
        self.connector_id = connector_id
        self.config = config
        self.status = ConnectorStatus.DISCONNECTED
        self.last_error = None
    
    @abstractmethod
    async def connect(self) -> bool:
        """連接到外部系統"""
        pass
    
    @abstractmethod
    async def disconnect(self) -> bool:
        """斷開連接"""
        pass
    
    @abstractmethod
    async def read(self, tags: List[str] = None) -> Dict:
        """讀取數據"""
        pass
    
    @abstractmethod
    async def write(self, data: Dict) -> bool:
        """寫入數據"""
        pass
    
    @abstractmethod
    def get_required_config_fields(self) -> List[str]:
        """獲取必要的配置欄位"""
        pass
    
    async def health_check(self) -> Dict:
        """健康檢查"""
        return {
            'connector_id': self.connector_id,
            'status': self.status.value,
            'last_error': self.last_error
        }
```

#### 6.2.2 OPC UA 連接器

```python
from asyncua import Client

class OPCUAConnector(BaseConnector):
    """OPC UA 連接器"""
    
    def __init__(self, connector_id: str, config: Dict):
        super().__init__(connector_id, config)
        self.client: Optional[Client] = None
    
    def get_required_config_fields(self) -> List[str]:
        return ['endpoint_url', 'username', 'password']
    
    async def connect(self) -> bool:
        """連接到 OPC UA 伺服器"""
        try:
            self.status = ConnectorStatus.CONNECTING
            
            endpoint_url = self.config['endpoint_url']
            self.client = Client(endpoint_url)
            
            # 設定認證
            if 'username' in self.config and 'password' in self.config:
                self.client.set_user(self.config['username'])
                self.client.set_password(self.config['password'])
            
            await self.client.connect()
            
            self.status = ConnectorStatus.CONNECTED
            logger.info(f"Connected to OPC UA server: {endpoint_url}")
            return True
        except Exception as e:
            self.status = ConnectorStatus.ERROR
            self.last_error = str(e)
            logger.error(f"Failed to connect to OPC UA server: {e}")
            return False
    
    async def disconnect(self) -> bool:
        """斷開連接"""
        try:
            if self.client:
                await self.client.disconnect()
            self.status = ConnectorStatus.DISCONNECTED
            return True
        except Exception as e:
            logger.error(f"Failed to disconnect from OPC UA server: {e}")
            return False
    
    async def read(self, tags: List[str] = None) -> Dict:
        """讀取 OPC UA 標籤"""
        if self.status != ConnectorStatus.CONNECTED:
            raise RuntimeError("Not connected to OPC UA server")
        
        result = {}
        for tag in tags:
            try:
                node = self.client.get_node(tag)
                value = await node.read_value()
                result[tag] = value
            except Exception as e:
                logger.error(f"Failed to read tag {tag}: {e}")
                result[tag] = None
        
        return result
    
    async def write(self, data: Dict) -> bool:
        """寫入 OPC UA 標籤"""
        if self.status != ConnectorStatus.CONNECTED:
            raise RuntimeError("Not connected to OPC UA server")
        
        try:
            for tag, value in data.items():
                node = self.client.get_node(tag)
                await node.write_value(value)
            return True
        except Exception as e:
            logger.error(f"Failed to write data: {e}")
            return False
```

---



# NDH 即時系統整合與協同管理

## 6.3 即時雙向連接與同步引擎

### 6.3.1 設計理念

NDH 作為執行時數據中樞，必須提供**即時、雙向、可靠**的系統連接能力。與傳統的單向數據採集不同，NDH 強調：

1. **雙向互動**：不僅接收數據，還能發送指令和回饋
2. **即時同步**：毫秒級延遲，支援即時控制和監控
3. **變更偵測**：自動偵測系統狀態變更並觸發事件
4. **多系統協調**：協調 MES、ERP、SCADA、Omniverse 等多個系統的數據流

### 6.3.2 即時連接架構

```
┌─────────────────────────────────────────────────────────────────┐
│                    外部系統（Real-time Systems）                 │
├──────────┬──────────┬──────────┬──────────┬──────────┬─────────┤
│   MES    │   ERP    │  SCADA   │   PLC    │Omniverse │  其他   │
│(Siemens) │  (SAP)   │ (AVEVA)  │(Rockwell)│ (NVIDIA) │         │
└────┬─────┴────┬─────┴────┬─────┴────┬─────┴────┬─────┴─────────┘
     │          │          │          │          │
     ▼          ▼          ▼          ▼          ▼
┌─────────────────────────────────────────────────────────────────┐
│              NDH 即時連接層 (Real-time Connection Layer)        │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  連接管理器 (Connection Manager)                          │  │
│  │  - 連接池管理 (Connection Pooling)                       │  │
│  │  - 心跳檢測 (Heartbeat Detection)                        │  │
│  │  - 自動重連 (Auto-reconnection)                          │  │
│  │  - 負載均衡 (Load Balancing)                             │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  雙向同步引擎 (Bidirectional Sync Engine)                │  │
│  │  - 訂閱/發布 (Subscribe/Publish)                         │  │
│  │  - 請求/響應 (Request/Response)                          │  │
│  │  - 命令/確認 (Command/Acknowledgement)                   │  │
│  │  - 批量操作 (Batch Operations)                           │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  變更偵測引擎 (Change Detection Engine)                  │  │
│  │  - CDC (Change Data Capture)                             │  │
│  │  - 事件流 (Event Streaming)                              │  │
│  │  - 差異計算 (Diff Calculation)                           │  │
│  │  - 變更通知 (Change Notification)                        │  │
│  └──────────────────────────────────────────────────────────┘  │
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │  協議適配層 (Protocol Adapter Layer)                     │  │
│  │  - REST/HTTP  - OPC UA    - Modbus TCP                   │  │
│  │  - GraphQL    - MQTT      - Profinet                     │  │
│  │  - WebSocket  - gRPC      - BACnet                       │  │
│  │  - SAP RFC    - ODBC/JDBC - USD (Omniverse)              │  │
│  └──────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────────┘
     │
     ▼
┌─────────────────────────────────────────────────────────────────┐
│              NDH 核心數據中樞 (Core Data Hub)                   │
│  - Asset Servants (執行時模型)                                 │
│  - 時序數據庫 (Time-series Database)                           │
│  - 事件總線 (Event Bus)                                        │
│  - 狀態管理 (State Management)                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 6.3.3 MES 雙向同步

**數據流向**：

**從 MES → NDH**：
- 生產訂單（Work Orders）
- 工藝路線（Routing）
- 工藝參數（Process Parameters）
- 品質標準（Quality Standards）
- 物料清單（BOM）

**從 NDH → MES**：
- 生產進度（Production Progress）
- 設備狀態（Equipment Status）
- 品質數據（Quality Data）
- 異常事件（Exceptions）
- 實際產量（Actual Output）

**實現範例**：

```python
class MESConnector:
    """MES 雙向同步連接器"""
    
    def __init__(self, config: Dict):
        self.config = config
        self.client = None
        self.subscriptions = []
        self.event_handlers = {}
    
    async def connect(self):
        """建立與 MES 的連接"""
        self.client = await self._create_client()
        await self._setup_subscriptions()
    
    async def _setup_subscriptions(self):
        """設置訂閱（從 MES 接收數據）"""
        # 訂閱生產訂單變更
        await self.subscribe(
            topic="production.orders",
            handler=self._on_order_changed
        )
        
        # 訂閱工藝參數變更
        await self.subscribe(
            topic="process.parameters",
            handler=self._on_parameter_changed
        )
    
    async def _on_order_changed(self, order: Dict):
        """處理生產訂單變更事件"""
        # 1. 驗證訂單數據
        validated_order = self._validate_order(order)
        
        # 2. 更新 NDH 內部狀態
        await self.ndh.update_production_order(validated_order)
        
        # 3. 觸發下游事件
        await self.ndh.emit_event("order.updated", validated_order)
        
        # 4. 通知相關 Asset Servants
        affected_assets = await self.ndh.get_assets_by_order(order['id'])
        for asset in affected_assets:
            await asset.on_order_changed(validated_order)
    
    async def report_progress(self, order_id: str, progress: Dict):
        """回報生產進度到 MES"""
        payload = {
            "order_id": order_id,
            "progress": progress,
            "timestamp": datetime.utcnow().isoformat(),
            "source": "NDH"
        }
        
        # 發送到 MES
        response = await self.client.post(
            f"/api/orders/{order_id}/progress",
            json=payload
        )
        
        # 記錄審計日誌
        await self.ndh.audit_log.record(
            action="report_progress",
            target=f"MES.Order.{order_id}",
            payload=payload,
            response=response
        )
        
        return response
    
    async def report_quality_data(self, order_id: str, quality_data: Dict):
        """回報品質數據到 MES"""
        payload = {
            "order_id": order_id,
            "quality_data": quality_data,
            "timestamp": datetime.utcnow().isoformat(),
            "inspector": quality_data.get("inspector"),
            "result": quality_data.get("result")  # PASS/FAIL
        }
        
        response = await self.client.post(
            f"/api/orders/{order_id}/quality",
            json=payload
        )
        
        return response
```

### 6.3.4 ERP 雙向同步

**數據流向**：

**從 ERP → NDH**：
- 物料需求計劃（MRP）
- 採購訂單（Purchase Orders）
- 成本中心（Cost Centers）
- 維護計劃（Maintenance Plans）
- 預算與資源分配

**從 NDH → ERP**：
- 實際庫存消耗
- 實際成本數據
- 設備健康狀態
- 維護記錄
- 能源消耗數據

**SAP RFC 連接範例**：

```python
class ERPConnector:
    """ERP (SAP) 雙向同步連接器"""
    
    def __init__(self, config: Dict):
        self.config = config
        self.rfc_connection = None
    
    async def connect(self):
        """建立 SAP RFC 連接"""
        from pyrfc import Connection
        
        self.rfc_connection = Connection(
            user=self.config['user'],
            passwd=self.config['password'],
            ashost=self.config['host'],
            sysnr=self.config['system_number'],
            client=self.config['client']
        )
    
    async def get_material_requirements(self, plant: str, date: str):
        """從 SAP 獲取物料需求"""
        result = self.rfc_connection.call(
            'BAPI_MATERIAL_AVAILABILITY',
            PLANT=plant,
            MATERIAL='*',
            CHECK_DATE=date
        )
        
        return result['MATERIAL_AVAILABILITY']
    
    async def post_goods_movement(self, movement: Dict):
        """回報物料移動到 SAP"""
        result = self.rfc_connection.call(
            'BAPI_GOODSMVT_CREATE',
            GOODSMVT_HEADER={
                'PSTNG_DATE': movement['posting_date'],
                'DOC_DATE': movement['document_date'],
                'REF_DOC_NO': movement['reference']
            },
            GOODSMVT_CODE={
                'GM_CODE': movement['movement_type']
            },
            GOODSMVT_ITEM=[{
                'MATERIAL': movement['material'],
                'PLANT': movement['plant'],
                'STGE_LOC': movement['storage_location'],
                'MOVE_TYPE': movement['movement_type'],
                'ENTRY_QNT': movement['quantity'],
                'ENTRY_UOM': movement['unit']
            }]
        )
        
        # 提交事務
        self.rfc_connection.call('BAPI_TRANSACTION_COMMIT')
        
        return result
```

### 6.3.5 SCADA/PLC 即時互動

**特性**：
- **毫秒級延遲**：支援即時控制迴路
- **高頻數據採集**：每秒數千個數據點
- **雙向控制**：不僅讀取，還能寫入控制指令

**OPC UA 訂閱範例**：

```python
class SCADAConnector:
    """SCADA/PLC 即時連接器（OPC UA）"""
    
    def __init__(self, config: Dict):
        self.config = config
        self.client = None
        self.subscription = None
    
    async def connect(self):
        """建立 OPC UA 連接"""
        from asyncua import Client
        
        self.client = Client(self.config['endpoint'])
        await self.client.connect()
        
        # 設置訂閱
        self.subscription = await self.client.create_subscription(
            period=100  # 100ms 更新週期
        )
    
    async def subscribe_tag(self, node_id: str, handler: Callable):
        """訂閱 OPC UA 標籤"""
        node = self.client.get_node(node_id)
        
        # 創建監控項
        handle = await self.subscription.subscribe_data_change(
            node,
            handler
        )
        
        return handle
    
    async def write_tag(self, node_id: str, value: Any):
        """寫入 OPC UA 標籤（發送控制指令）"""
        node = self.client.get_node(node_id)
        
        # 寫入前驗證權限
        if not await self._check_write_permission(node_id):
            raise PermissionError(f"No write permission for {node_id}")
        
        # 寫入值
        await node.write_value(value)
        
        # 記錄審計日誌
        await self.ndh.audit_log.record(
            action="write_tag",
            target=node_id,
            value=value,
            user=self.current_user
        )
    
    async def _on_tag_changed(self, node, value, data):
        """標籤值變更處理器"""
        # 1. 解析變更
        tag_id = str(node.nodeid)
        timestamp = data.monitored_item.Value.SourceTimestamp
        
        # 2. 寫入時序數據庫
        await self.ndh.tsdb.write(
            measurement="scada_tags",
            tags={"tag_id": tag_id},
            fields={"value": value},
            timestamp=timestamp
        )
        
        # 3. 檢查異常
        if await self._is_abnormal(tag_id, value):
            await self.ndh.emit_event("tag.abnormal", {
                "tag_id": tag_id,
                "value": value,
                "timestamp": timestamp
            })
        
        # 4. 更新 Asset Servant 狀態
        asset = await self.ndh.get_asset_by_tag(tag_id)
        if asset:
            await asset.update_property(tag_id, value)
```

### 6.3.6 Omniverse 即時協同

**USD 雙向同步**：

```python
class OmniverseConnector:
    """Omniverse 即時協同連接器"""
    
    def __init__(self, config: Dict):
        self.config = config
        self.nucleus_client = None
        self.usd_stage = None
        self.subscriptions = []
    
    async def connect(self):
        """連接到 Omniverse Nucleus"""
        from omni.client import OmniClient
        
        self.nucleus_client = OmniClient()
        await self.nucleus_client.connect(
            server=self.config['nucleus_server'],
            user=self.config['user'],
            password=self.config['password']
        )
        
        # 開啟 USD Stage
        self.usd_stage = await self._open_stage(
            self.config['stage_url']
        )
    
    async def subscribe_prim_changes(self, prim_path: str, handler: Callable):
        """訂閱 USD Prim 變更"""
        # 監聽 Prim 的屬性變更
        listener = self.usd_stage.GetObjectAtPath(prim_path).GetPrimAtPath()
        
        # 註冊變更回調
        self.subscriptions.append({
            "prim_path": prim_path,
            "handler": handler,
            "listener": listener
        })
    
    async def update_asset_transform(self, asset_id: str, transform: Dict):
        """更新資產的 Transform（從 NDH → Omniverse）"""
        # 1. 查找對應的 USD Prim
        prim_path = await self._get_prim_path_by_asset_id(asset_id)
        prim = self.usd_stage.GetPrimAtPath(prim_path)
        
        # 2. 更新 Transform
        from pxr import UsdGeom, Gf
        
        xformable = UsdGeom.Xformable(prim)
        xformable.ClearXformOpOrder()
        
        translate_op = xformable.AddTranslateOp()
        translate_op.Set(Gf.Vec3d(
            transform['position']['x'],
            transform['position']['y'],
            transform['position']['z']
        ))
        
        rotate_op = xformable.AddRotateXYZOp()
        rotate_op.Set(Gf.Vec3d(
            transform['rotation']['x'],
            transform['rotation']['y'],
            transform['rotation']['z']
        ))
        
        # 3. 保存變更
        self.usd_stage.Save()
        
        # 4. 通知其他協同用戶
        await self._broadcast_change(prim_path, transform)
    
    async def _on_prim_changed(self, prim_path: str, change_type: str):
        """Prim 變更事件處理器（從 Omniverse → NDH）"""
        # 1. 獲取變更後的數據
        prim = self.usd_stage.GetPrimAtPath(prim_path)
        
        # 2. 解析為 IADL 格式
        iadl_data = await self._usd_to_iadl(prim)
        
        # 3. 更新 NDH 內部狀態
        asset_id = await self._get_asset_id_by_prim_path(prim_path)
        await self.ndh.update_asset(asset_id, iadl_data)
        
        # 4. 觸發事件
        await self.ndh.emit_event("asset.updated.from_omniverse", {
            "asset_id": asset_id,
            "prim_path": prim_path,
            "change_type": change_type
        })
```

---

## 6.4 變更偵測與事件驅動架構

### 6.4.1 變更數據捕獲（CDC）

**目的**：自動偵測外部系統的數據變更，無需輪詢。

**實現方式**：

1. **數據庫 CDC**（針對 MES/ERP 數據庫）：
   - PostgreSQL：Logical Replication
   - SQL Server：Change Data Capture (CDC)
   - Oracle：GoldenGate

2. **消息隊列**（針對事件驅動系統）：
   - Kafka
   - RabbitMQ
   - Azure Service Bus

3. **Webhook**（針對 SaaS 系統）：
   - 註冊 Webhook 端點
   - 接收變更通知

**CDC 引擎實現**：

```python
class CDCEngine:
    """變更數據捕獲引擎"""
    
    def __init__(self, config: Dict):
        self.config = config
        self.listeners = {}
        self.event_bus = EventBus()
    
    async def start_postgresql_cdc(self, db_config: Dict, tables: List[str]):
        """啟動 PostgreSQL CDC"""
        from psycopg2.extras import LogicalReplicationConnection
        
        conn = psycopg2.connect(
            connection_factory=LogicalReplicationConnection,
            **db_config
        )
        
        cur = conn.cursor()
        
        # 創建複製槽
        try:
            cur.create_replication_slot(
                'ndh_cdc_slot',
                output_plugin='wal2json'
            )
        except psycopg2.errors.DuplicateObject:
            pass  # 槽已存在
        
        # 開始消費變更
        cur.start_replication(
            slot_name='ndh_cdc_slot',
            decode=True
        )
        
        # 處理變更流
        async for msg in cur:
            change = json.loads(msg.payload)
            await self._process_change(change)
            msg.cursor.send_feedback(flush_lsn=msg.data_start)
    
    async def _process_change(self, change: Dict):
        """處理變更事件"""
        table = change['table']
        action = change['action']  # INSERT/UPDATE/DELETE
        data = change['data']
        
        # 發布事件
        await self.event_bus.publish(
            topic=f"cdc.{table}.{action}",
            payload={
                "table": table,
                "action": action,
                "data": data,
                "timestamp": change['timestamp']
            }
        )
```

### 6.4.2 事件總線

**職責**：
- 解耦系統組件
- 支援發布/訂閱模式
- 保證事件順序和可靠性

**實現**：

```python
class EventBus:
    """事件總線"""
    
    def __init__(self):
        self.subscribers = defaultdict(list)
        self.event_store = EventStore()
    
    async def publish(self, topic: str, payload: Dict):
        """發布事件"""
        event = Event(
            id=str(uuid.uuid4()),
            topic=topic,
            payload=payload,
            timestamp=datetime.utcnow()
        )
        
        # 1. 持久化事件
        await self.event_store.save(event)
        
        # 2. 通知訂閱者
        for subscriber in self.subscribers[topic]:
            try:
                await subscriber(event)
            except Exception as e:
                logger.error(f"Subscriber error: {e}")
                # 重試邏輯
                await self._retry_delivery(subscriber, event)
    
    def subscribe(self, topic: str, handler: Callable):
        """訂閱事件"""
        self.subscribers[topic].append(handler)
    
    async def _retry_delivery(self, subscriber: Callable, event: Event, max_retries: int = 3):
        """重試事件投遞"""
        for attempt in range(max_retries):
            try:
                await asyncio.sleep(2 ** attempt)  # 指數退避
                await subscriber(event)
                return
            except Exception as e:
                logger.warning(f"Retry {attempt + 1} failed: {e}")
        
        # 所有重試失敗，記錄到死信隊列
        await self.event_store.move_to_dlq(event)
```

---

## 6.5 多用戶協同管理

### 6.5.1 即時登入管理

**Session 管理**：

```python
class SessionManager:
    """會話管理器"""
    
    def __init__(self):
        self.active_sessions = {}
        self.redis_client = redis.Redis()
    
    async def create_session(self, user: User, metadata: Dict) -> Session:
        """創建用戶會話"""
        session = Session(
            id=str(uuid.uuid4()),
            user_id=user.id,
            user_name=user.name,
            roles=user.roles,
            created_at=datetime.utcnow(),
            last_active=datetime.utcnow(),
            metadata=metadata
        )
        
        # 存儲到 Redis（支援分佈式部署）
        await self.redis_client.setex(
            f"session:{session.id}",
            3600,  # 1小時過期
            json.dumps(session.to_dict())
        )
        
        self.active_sessions[session.id] = session
        
        # 發布事件
        await event_bus.publish("user.logged_in", {
            "session_id": session.id,
            "user_id": user.id,
            "user_name": user.name
        })
        
        return session
    
    async def get_active_users(self) -> List[Dict]:
        """獲取當前活躍用戶列表"""
        return [
            {
                "user_id": session.user_id,
                "user_name": session.user_name,
                "last_active": session.last_active,
                "location": session.metadata.get("location")
            }
            for session in self.active_sessions.values()
        ]
    
    async def heartbeat(self, session_id: str):
        """心跳更新"""
        if session_id in self.active_sessions:
            self.active_sessions[session_id].last_active = datetime.utcnow()
            
            # 更新 Redis TTL
            await self.redis_client.expire(f"session:{session_id}", 3600)
```

### 6.5.2 權限控制（RBAC）

**角色定義**：

```yaml
# roles.yaml
roles:
  - name: admin
    permissions:
      - "*"  # 所有權限
  
  - name: engineer
    permissions:
      - "asset:read"
      - "asset:write"
      - "asset:command"
      - "data:read"
      - "data:write"
  
  - name: operator
    permissions:
      - "asset:read"
      - "data:read"
      - "dashboard:view"
  
  - name: viewer
    permissions:
      - "asset:read"
      - "data:read"
```

**權限檢查**：

```python
class PermissionManager:
    """權限管理器"""
    
    def __init__(self):
        self.roles = self._load_roles()
    
    async def check_permission(self, user: User, resource: str, action: str) -> bool:
        """檢查用戶權限"""
        required_permission = f"{resource}:{action}"
        
        for role_name in user.roles:
            role = self.roles.get(role_name)
            if not role:
                continue
            
            # 檢查通配符權限
            if "*" in role.permissions:
                return True
            
            # 檢查精確權限
            if required_permission in role.permissions:
                return True
            
            # 檢查資源通配符（如 "asset:*"）
            resource_wildcard = f"{resource}:*"
            if resource_wildcard in role.permissions:
                return True
        
        return False
    
    def require_permission(self, resource: str, action: str):
        """權限裝飾器"""
        def decorator(func):
            @wraps(func)
            async def wrapper(*args, **kwargs):
                user = kwargs.get('user') or args[0].current_user
                
                if not await self.check_permission(user, resource, action):
                    raise PermissionError(
                        f"User {user.name} lacks permission: {resource}:{action}"
                    )
                
                return await func(*args, **kwargs)
            return wrapper
        return decorator
```

**使用範例**：

```python
class AssetAPI:
    """資產 API"""
    
    @permission_manager.require_permission("asset", "write")
    async def update_asset(self, asset_id: str, data: Dict, user: User):
        """更新資產（需要 asset:write 權限）"""
        asset = await self.ndh.get_asset(asset_id)
        await asset.update(data)
        
        # 記錄審計日誌
        await audit_log.record(
            action="update_asset",
            user=user.name,
            target=asset_id,
            changes=data
        )
```

### 6.5.3 並發控制

**樂觀鎖（Optimistic Locking）**：

```python
class OptimisticLockManager:
    """樂觀鎖管理器"""
    
    async def update_with_version(self, asset_id: str, data: Dict, expected_version: int):
        """基於版本號的更新"""
        async with self.db.transaction():
            # 1. 讀取當前版本
            current = await self.db.fetchrow(
                "SELECT * FROM assets WHERE id = $1 FOR UPDATE",
                asset_id
            )
            
            # 2. 檢查版本號
            if current['version'] != expected_version:
                raise ConcurrentModificationError(
                    f"Asset {asset_id} was modified by another user. "
                    f"Expected version {expected_version}, got {current['version']}"
                )
            
            # 3. 更新並遞增版本號
            await self.db.execute(
                """
                UPDATE assets 
                SET data = $1, version = version + 1, updated_at = NOW()
                WHERE id = $2
                """,
                json.dumps(data),
                asset_id
            )
```

**悲觀鎖（Pessimistic Locking）**：

```python
class PessimisticLockManager:
    """悲觀鎖管理器"""
    
    async def acquire_lock(self, resource_id: str, user_id: str, timeout: int = 300):
        """獲取資源鎖"""
        lock_key = f"lock:{resource_id}"
        
        # 嘗試獲取鎖（使用 Redis SETNX）
        acquired = await self.redis.set(
            lock_key,
            user_id,
            nx=True,  # 只在不存在時設置
            ex=timeout  # 過期時間
        )
        
        if not acquired:
            # 鎖已被佔用
            current_owner = await self.redis.get(lock_key)
            raise ResourceLockedError(
                f"Resource {resource_id} is locked by user {current_owner}"
            )
        
        return Lock(resource_id, user_id, timeout)
    
    async def release_lock(self, resource_id: str, user_id: str):
        """釋放資源鎖"""
        lock_key = f"lock:{resource_id}"
        
        # 只有鎖的擁有者才能釋放
        current_owner = await self.redis.get(lock_key)
        if current_owner != user_id:
            raise PermissionError(
                f"User {user_id} does not own the lock on {resource_id}"
            )
        
        await self.redis.delete(lock_key)
```

### 6.5.4 衝突解決

**Last-Write-Wins (LWW)**：

```python
async def resolve_conflict_lww(local_version: Dict, remote_version: Dict) -> Dict:
    """最後寫入獲勝策略"""
    if local_version['timestamp'] > remote_version['timestamp']:
        return local_version
    else:
        return remote_version
```

**Three-Way Merge**：

```python
async def resolve_conflict_merge(base: Dict, local: Dict, remote: Dict) -> Dict:
    """三路合併策略"""
    result = base.copy()
    
    # 合併本地變更
    for key, value in local.items():
        if key not in base or base[key] != value:
            result[key] = value
    
    # 合併遠端變更（如果沒有衝突）
    for key, value in remote.items():
        if key not in local or local[key] == base.get(key):
            result[key] = value
        elif local[key] != value:
            # 衝突：需要人工解決
            result[key] = {
                "_conflict": True,
                "local": local[key],
                "remote": value
            }
    
    return result
```

---

## 6.6 審計日誌與追溯

### 6.6.1 審計日誌設計

**記錄內容**：
- 誰（Who）：用戶 ID、用戶名、IP 地址
- 何時（When）：時間戳（精確到毫秒）
- 做了什麼（What）：操作類型、目標資源
- 結果（Result）：成功/失敗、錯誤訊息
- 變更前後（Before/After）：數據快照

**實現**：

```python
class AuditLog:
    """審計日誌"""
    
    async def record(self, 
                    action: str,
                    user: str,
                    target: str,
                    changes: Dict = None,
                    result: str = "success",
                    error: str = None):
        """記錄審計日誌"""
        log_entry = {
            "id": str(uuid.uuid4()),
            "timestamp": datetime.utcnow().isoformat(),
            "action": action,
            "user": user,
            "user_ip": self._get_client_ip(),
            "target": target,
            "changes": changes,
            "result": result,
            "error": error
        }
        
        # 寫入時序數據庫（長期保存）
        await self.tsdb.write(
            measurement="audit_logs",
            tags={
                "action": action,
                "user": user,
                "result": result
            },
            fields=log_entry,
            timestamp=datetime.utcnow()
        )
        
        # 寫入 Elasticsearch（支援全文搜索）
        await self.es.index(
            index="audit_logs",
            document=log_entry
        )
```

### 6.6.2 數據追溯

**時間旅行查詢**：

```python
async def get_asset_at_time(asset_id: str, timestamp: datetime) -> Dict:
    """獲取資產在特定時間點的狀態"""
    # 1. 查詢審計日誌，找到該時間點之前的所有變更
    changes = await audit_log.query(
        target=asset_id,
        end_time=timestamp,
        order="asc"
    )
    
    # 2. 從初始狀態開始，逐步應用變更
    state = await get_initial_state(asset_id)
    
    for change in changes:
        state = apply_change(state, change)
    
    return state
```

---

## 總結

NDH 的即時系統整合與協同管理能力是其作為執行時數據中樞的核心價值：

1. **即時雙向連接**：與 MES、ERP、SCADA、Omniverse 等系統的毫秒級同步
2. **變更偵測**：自動捕獲外部系統的數據變更，無需輪詢
3. **事件驅動**：基於事件總線的解耦架構，支援複雜的業務邏輯
4. **多用戶協同**：完整的會話管理、權限控制和並發控制
5. **審計追溯**：完整的操作日誌和數據追溯能力

這些能力使 NDH 能夠成為工業數位分身的神經中樞，協調多個系統的即時互動，支援多用戶協同作業，並確保數據的安全性和一致性。



## 7. API 設計與介面規範

### 7.1 RESTful API

#### 7.1.1 API 版本控制

所有 API 使用版本前綴：`/api/v1/`

#### 7.1.2 資產管理 API

**創建資產**：
```http
POST /api/v1/assets
Content-Type: application/json

{
  "iadl_file_path": "/path/to/asset.iadl",
  "auto_initialize": true
}

Response 201 Created:
{
  "asset_id": "pump_001",
  "status": "running",
  "created_at": "2025-10-14T10:00:00Z"
}
```

**獲取資產狀態**：
```http
GET /api/v1/assets/{asset_id}

Response 200 OK:
{
  "asset_id": "pump_001",
  "state": "running",
  "properties": {
    "flow_rate": 150.5,
    "pressure": 5.2,
    "temperature": 65.3
  },
  "last_update": "2025-10-14T10:05:00Z"
}
```

**讀取資產屬性**：
```http
GET /api/v1/assets/{asset_id}/properties/{property_name}

Response 200 OK:
{
  "asset_id": "pump_001",
  "property": "flow_rate",
  "value": 150.5,
  "unit": "L/min",
  "timestamp": "2025-10-14T10:05:00Z"
}
```

**寫入資產屬性**：
```http
PUT /api/v1/assets/{asset_id}/properties/{property_name}
Content-Type: application/json

{
  "value": 160.0
}

Response 200 OK:
{
  "success": true,
  "asset_id": "pump_001",
  "property": "flow_rate",
  "old_value": 150.5,
  "new_value": 160.0,
  "timestamp": "2025-10-14T10:06:00Z"
}
```

**調用資產命令**：
```http
POST /api/v1/assets/{asset_id}/commands/{command_name}
Content-Type: application/json

{
  "parameters": {
    "target_speed": 1500
  }
}

Response 200 OK:
{
  "success": true,
  "command": "set_speed",
  "result": {
    "actual_speed": 1500,
    "ramp_time": 2.5
  },
  "timestamp": "2025-10-14T10:07:00Z"
}
```

**獲取遙測數據**：
```http
GET /api/v1/assets/{asset_id}/telemetry/{telemetry_name}?start=2025-10-14T00:00:00Z&end=2025-10-14T23:59:59Z

Response 200 OK:
{
  "asset_id": "pump_001",
  "telemetry": "vibration",
  "data": [
    {"timestamp": "2025-10-14T10:00:00Z", "value": 0.5},
    {"timestamp": "2025-10-14T10:01:00Z", "value": 0.52},
    ...
  ],
  "count": 1440
}
```

#### 7.1.3 時序數據 API

**寫入時序數據**：
```http
POST /api/v1/timeseries/write
Content-Type: application/json

{
  "measurement": "sensor_data",
  "tags": {
    "asset_id": "pump_001",
    "location": "factory_a"
  },
  "fields": {
    "temperature": 65.3,
    "pressure": 5.2
  },
  "timestamp": "2025-10-14T10:00:00Z"
}

Response 201 Created:
{
  "success": true,
  "points_written": 1
}
```

**批量寫入**：
```http
POST /api/v1/timeseries/batch_write
Content-Type: application/json

{
  "points": [
    {
      "measurement": "sensor_data",
      "tags": {"asset_id": "pump_001"},
      "fields": {"temperature": 65.3},
      "timestamp": "2025-10-14T10:00:00Z"
    },
    ...
  ]
}

Response 201 Created:
{
  "success": true,
  "points_written": 100
}
```

**查詢時序數據**：
```http
POST /api/v1/timeseries/query
Content-Type: application/json

{
  "measurement": "sensor_data",
  "tags": {"asset_id": "pump_001"},
  "fields": ["temperature", "pressure"],
  "start": "2025-10-14T00:00:00Z",
  "end": "2025-10-14T23:59:59Z"
}

Response 200 OK:
{
  "data": [
    {
      "timestamp": "2025-10-14T10:00:00Z",
      "temperature": 65.3,
      "pressure": 5.2
    },
    ...
  ],
  "count": 1440
}
```

**聚合查詢**：
```http
POST /api/v1/timeseries/aggregate
Content-Type: application/json

{
  "measurement": "sensor_data",
  "tags": {"asset_id": "pump_001"},
  "start": "2025-10-14T00:00:00Z",
  "end": "2025-10-14T23:59:59Z",
  "aggregation": "mean",
  "interval": "1h"
}

Response 200 OK:
{
  "data": [
    {
      "window_start": "2025-10-14T00:00:00Z",
      "temperature_mean": 65.5,
      "pressure_mean": 5.1
    },
    ...
  ],
  "count": 24
}
```

### 7.2 GraphQL API

**Schema 定義**：

```graphql
type Asset {
  id: ID!
  name: String!
  type: String!
  state: AssetState!
  properties: [Property!]!
  telemetry: [Telemetry!]!
  commands: [Command!]!
  relationships: [Relationship!]!
  lastUpdate: DateTime!
}

type Property {
  name: String!
  value: JSON!
  unit: String
  dataType: String!
  writable: Boolean!
}

type Telemetry {
  name: String!
  data: [TelemetryPoint!]!
}

type TelemetryPoint {
  timestamp: DateTime!
  value: Float!
}

type Query {
  asset(id: ID!): Asset
  assets(filter: AssetFilter): [Asset!]!
  telemetry(assetId: ID!, name: String!, start: DateTime!, end: DateTime!): Telemetry
}

type Mutation {
  createAsset(input: CreateAssetInput!): Asset!
  updateProperty(assetId: ID!, propertyName: String!, value: JSON!): Property!
  invokeCommand(assetId: ID!, commandName: String!, parameters: JSON): CommandResult!
}

type Subscription {
  assetUpdated(assetId: ID!): Asset!
  telemetryStream(assetId: ID!, telemetryName: String!): TelemetryPoint!
}
```

**查詢範例**：

```graphql
query GetAssetWithTelemetry {
  asset(id: "pump_001") {
    id
    name
    state
    properties {
      name
      value
      unit
    }
    telemetry(name: "temperature", start: "2025-10-14T00:00:00Z", end: "2025-10-14T23:59:59Z") {
      name
      data {
        timestamp
        value
      }
    }
  }
}
```

### 7.3 WebSocket API

**連接**：
```javascript
const ws = new WebSocket('ws://localhost:8000/ws/assets/pump_001');

ws.onopen = () => {
  console.log('Connected to Asset pump_001');
};

ws.onmessage = (event) => {
  const data = JSON.parse(event.data);
  console.log('Received update:', data);
};
```

**訂閱屬性變更**：
```json
{
  "action": "subscribe",
  "type": "property_changed",
  "asset_id": "pump_001",
  "property": "flow_rate"
}
```

**接收即時更新**：
```json
{
  "type": "property_changed",
  "asset_id": "pump_001",
  "property": "flow_rate",
  "old_value": 150.5,
  "new_value": 160.0,
  "timestamp": "2025-10-14T10:06:00Z"
}
```

---

## 8. 數據流與互動流程

### 8.1 IADL/FDL/NDH 完整互動流程

```
┌─────────────────────────────────────────────────────────────┐
│                階段 1：設計階段 (Design Time - Offline)      │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ 1. IADL 資產建模                                      │  │
│  │    - 使用 IADL Editor 定義資產                        │  │
│  │    - 定義屬性、遙測、命令、關係                       │  │
│  │    - 輸出: pump_001.iadl                             │  │
│  └──────────────────────────────────────────────────────┘  │
│                          ▼                                   │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ 2. FDL 工廠佈局設計                                   │  │
│  │    - 使用 Factory Designer 設計工廠佈局               │  │
│  │    - 放置資產、定義連接關係                          │  │
│  │    - 輸出: factory_layout.fdl                        │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                階段 2：部署階段 (Deployment)                 │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ 3. 部署到 NDH                                         │  │
│  │    - 上傳 FDL 檔案到 NDH                              │  │
│  │    - POST /api/v1/factory/deploy                     │  │
│  └──────────────────────────────────────────────────────┘  │
│                          ▼                                   │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ 4. FDL 解析與驗證                                     │  │
│  │    - NDH 解析 FDL 檔案                                │  │
│  │    - 驗證資產定義、連接關係                          │  │
│  │    - 檢查衝突和錯誤                                   │  │
│  └──────────────────────────────────────────────────────┘  │
│                          ▼                                   │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ 5. 載入 IADL 定義                                     │  │
│  │    - 從 FDL 中提取所有資產的 IADL 引用                │  │
│  │    - 載入每個資產的 IADL 定義                         │  │
│  └──────────────────────────────────────────────────────┘  │
│                          ▼                                   │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ 6. 批次實例化 Asset Servants                          │  │
│  │    - 為每個資產創建 Asset Servant                     │  │
│  │    - 初始化 Servant（連接底層系統）                   │  │
│  │    - 建立資產間的關係                                 │  │
│  └──────────────────────────────────────────────────────┘  │
│                          ▼                                   │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ 7. 註冊到命名服務                                     │  │
│  │    - 將所有 Asset Servants 註冊到命名服務             │  │
│  │    - 提供服務發現和路由                               │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
                          ▼
┌─────────────────────────────────────────────────────────────┐
│                階段 3：執行階段 (Runtime - Online)           │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │ 8. 即時運行與監控                                     │  │
│  │    - Asset Servants 開始運行                          │  │
│  │    - 收集遙測數據 → 時序數據庫                        │  │
│  │    - 處理命令和屬性變更                               │  │
│  │    - 觸發事件和告警                                   │  │
│  │    - 與 MES/ERP/SCADA 整合                            │  │
│  └──────────────────────────────────────────────────────┘  │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### 8.2 數據流向詳細說明

#### 8.2.1 設計階段數據流

```
IADL Editor → pump_001.iadl → Factory Designer → factory_layout.fdl
```

**IADL 範例**（pump_001.iadl）：
```yaml
id: pump_001
name: "Centrifugal Pump #1"
type: "CentrifugalPump"
manufacturer: "Grundfos"
model: "CR 64-2"

properties:
  flow_rate:
    type: float
    unit: "L/min"
    writable: true
    range: [0, 200]
  
  pressure:
    type: float
    unit: "bar"
    writable: false
    range: [0, 10]

telemetry:
  vibration:
    type: float
    unit: "mm/s"
    sampling_interval: "1s"
  
  temperature:
    type: float
    unit: "°C"
    sampling_interval: "1s"

commands:
  start:
    parameters: {}
    return_type: boolean
  
  stop:
    parameters: {}
    return_type: boolean
  
  set_speed:
    parameters:
      target_speed:
        type: integer
        unit: "RPM"
        range: [0, 3000]
    return_type: integer

relationships:
  - type: "feeds_into"
    target: "valve_001"
  - type: "powered_by"
    target: "motor_001"
```

**FDL 範例**（factory_layout.fdl）：
```yaml
factory_id: "factory_a"
name: "Factory A - Production Line 1"
version: "1.0"

assets:
  - id: pump_001
    iadl_ref: "assets/pump_001.iadl"
    position: {x: 10.5, y: 5.2, z: 0.0}
    rotation: {x: 0, y: 0, z: 90}
  
  - id: valve_001
    iadl_ref: "assets/valve_001.iadl"
    position: {x: 12.0, y: 5.2, z: 0.0}
    rotation: {x: 0, y: 0, z: 0}

connections:
  - from: pump_001
    to: valve_001
    type: "pipe"
    properties:
      diameter: 50
      material: "stainless_steel"
```

#### 8.2.2 部署階段數據流

```
factory_layout.fdl → NDH API → FDL Parser → IADL Loader → Asset Servant Manager → Asset Servants
```

**API 調用**：
```http
POST /api/v1/factory/deploy
Content-Type: multipart/form-data

fdl_file: factory_layout.fdl
auto_start: true
```

**NDH 內部處理**：
1. FDL Parser 解析 factory_layout.fdl
2. 提取所有資產的 IADL 引用
3. IADL Loader 載入每個 IADL 定義
4. Asset Servant Manager 批次創建 Asset Servants
5. 每個 Asset Servant 初始化並連接到底層系統
6. 註冊到命名服務

#### 8.2.3 執行階段數據流

```
物理設備 → OPC UA/Modbus → NDH Connector → Asset Servant → 時序數據庫
                                                    ↓
                                            MES/ERP/SCADA
```

**即時數據流**：
1. **感測器數據收集**：
   - 物理感測器（溫度、壓力、振動）
   - 通過 OPC UA/Modbus 讀取
   - NDH Connector 轉換為標準格式
   - Asset Servant 接收並處理
   - 寫入時序數據庫

2. **命令執行**：
   - 應用程式調用 Asset Servant 的命令
   - Asset Servant 驗證參數
   - 通過 Connector 下發到 PLC
   - PLC 執行命令
   - 回報執行結果

3. **事件觸發**：
   - Asset Servant 監測異常（如溫度過高）
   - 觸發告警事件
   - 通知訂閱者（Dashboard、MES）
   - 記錄到事件日誌

---

## 9. 部署架構與擴展性

### 9.1 單機部署

**適用場景**：
- 開發和測試環境
- 小型工廠（< 100 台設備）
- 邊緣計算節點

**架構圖**：

```
┌─────────────────────────────────────────┐
│         單機伺服器 (4 Core, 16GB RAM)   │
│                                          │
│  ┌────────────────────────────────────┐ │
│  │  NDH 應用程式 (FastAPI + Uvicorn)  │ │
│  └────────────────────────────────────┘ │
│                                          │
│  ┌────────────────────────────────────┐ │
│  │  時序數據庫 (QuestDB/InfluxDB)     │ │
│  └────────────────────────────────────┘ │
│                                          │
│  ┌────────────────────────────────────┐ │
│  │  關係型數據庫 (PostgreSQL)         │ │
│  └────────────────────────────────────┘ │
│                                          │
│  ┌────────────────────────────────────┐ │
│  │  Redis (訊息佇列)                  │ │
│  └────────────────────────────────────┘ │
│                                          │
└─────────────────────────────────────────┘
```

**部署步驟**：
1. 安裝 Docker 和 Docker Compose
2. 使用 docker-compose.yml 一鍵部署
3. 配置環境變數
4. 啟動服務

**docker-compose.yml**：
```yaml
version: '3.8'

services:
  ndh:
    image: macrovision/ndh:latest
    ports:
      - "8000:8000"
    environment:
      - NDH_TSDB_TYPE=questdb
      - NDH_POSTGRES_HOST=postgres
      - NDH_REDIS_HOST=redis
    depends_on:
      - postgres
      - questdb
      - redis
  
  postgres:
    image: postgres:15
    environment:
      - POSTGRES_DB=ndh
      - POSTGRES_USER=ndh
      - POSTGRES_PASSWORD=ndh_password
    volumes:
      - postgres_data:/var/lib/postgresql/data
  
  questdb:
    image: questdb/questdb:latest
    ports:
      - "9000:9000"
      - "8812:8812"
    volumes:
      - questdb_data:/root/.questdb
  
  redis:
    image: redis:7
    volumes:
      - redis_data:/data

volumes:
  postgres_data:
  questdb_data:
  redis_data:
```

### 9.2 集群部署

**適用場景**：
- 中大型工廠（100-1000 台設備）
- 多廠區部署
- 高可用性需求

**架構圖**：

```
┌─────────────────────────────────────────────────────────────┐
│                       負載均衡器 (Nginx/HAProxy)             │
└────────────┬────────────┬────────────┬────────────┬─────────┘
             │            │            │            │
       ┌─────▼────┐ ┌─────▼────┐ ┌─────▼────┐ ┌─────▼────┐
       │ NDH      │ │ NDH      │ │ NDH      │ │ NDH      │
       │ Instance │ │ Instance │ │ Instance │ │ Instance │
       │    #1    │ │    #2    │ │    #3    │ │    #4    │
       └─────┬────┘ └─────┬────┘ └─────┬────┘ └─────┬────┘
             │            │            │            │
             └────────────┴────────────┴────────────┘
                          │
       ┌──────────────────┼──────────────────┐
       │                  │                  │
  ┌────▼────┐      ┌──────▼──────┐   ┌──────▼──────┐
  │TDengine │      │ PostgreSQL  │   │   Kafka     │
  │ Cluster │      │   Cluster   │   │   Cluster   │
  │(3 nodes)│      │  (3 nodes)  │   │  (3 nodes)  │
  └─────────┘      └─────────────┘   └─────────────┘
```

**部署步驟**：
1. 部署 Kubernetes 集群
2. 使用 Helm Chart 部署 NDH
3. 配置 Ingress 和負載均衡
4. 部署時序數據庫集群
5. 配置監控和告警

**Kubernetes Deployment**：
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: ndh
spec:
  replicas: 4
  selector:
    matchLabels:
      app: ndh
  template:
    metadata:
      labels:
        app: ndh
    spec:
      containers:
      - name: ndh
        image: macrovision/ndh:latest
        ports:
        - containerPort: 8000
        env:
        - name: NDH_TSDB_TYPE
          value: "tdengine"
        - name: NDH_POSTGRES_HOST
          value: "postgres-service"
        - name: NDH_KAFKA_BROKERS
          value: "kafka-0:9092,kafka-1:9092,kafka-2:9092"
        resources:
          requests:
            memory: "2Gi"
            cpu: "1"
          limits:
            memory: "4Gi"
            cpu: "2"
```

### 9.3 多地區部署

**適用場景**：
- 跨國企業
- 多廠區分佈（美國、歐洲、亞洲）
- 數據主權要求

**架構圖**：

```
┌─────────────────────────────────────────────────────────────┐
│                    全球負載均衡 (Global LB)                  │
└────────┬────────────────────┬────────────────────┬──────────┘
         │                    │                    │
    ┌────▼────┐          ┌────▼────┐          ┌────▼────┐
    │ 美國區   │          │ 歐洲區   │          │ 亞洲區   │
    │ NDH      │          │ NDH      │          │ NDH      │
    │ Cluster  │          │ Cluster  │          │ Cluster  │
    └────┬────┘          └────┬────┘          └────┬────┘
         │                    │                    │
         └────────────────────┴────────────────────┘
                              │
                    ┌─────────▼─────────┐
                    │  數據同步層        │
                    │  (Kafka Connect)  │
                    └───────────────────┘
```

**數據同步策略**：
1. **本地優先**：數據寫入本地區域
2. **異步複製**：定期同步到其他區域
3. **衝突解決**：使用時間戳和版本號
4. **數據主權**：敏感數據僅存儲在特定區域

### 9.4 擴展性設計

#### 9.4.1 水平擴展

**無狀態服務**：
- NDH API 服務完全無狀態
- 可任意增加實例數量
- 使用 Redis/Kafka 共享狀態

**自動擴展**：
```yaml
apiVersion: autoscaling/v2
kind: HorizontalPodAutoscaler
metadata:
  name: ndh-hpa
spec:
  scaleTargetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: ndh
  minReplicas: 2
  maxReplicas: 20
  metrics:
  - type: Resource
    resource:
      name: cpu
      target:
        type: Utilization
        averageUtilization: 70
  - type: Resource
    resource:
      name: memory
      target:
        type: Utilization
        averageUtilization: 80
```

#### 9.4.2 垂直擴展

**資源配置建議**：

| 場景 | CPU | 記憶體 | 磁碟 |
|------|-----|--------|------|
| 小型（< 100 設備） | 2 Core | 4GB | 100GB SSD |
| 中型（100-500 設備） | 4 Core | 16GB | 500GB SSD |
| 大型（500-2000 設備） | 8 Core | 32GB | 2TB SSD |
| 超大型（> 2000 設備） | 16+ Core | 64GB+ | 5TB+ SSD |

#### 9.4.3 效能優化

**快取策略**：
- Redis 快取熱數據（最近 1 小時）
- 資產元數據快取（TTL: 1 小時）
- 查詢結果快取（TTL: 5 分鐘）

**數據庫優化**：
- 時序數據庫分片（按時間和資產 ID）
- PostgreSQL 讀寫分離
- 連接池配置（最小 10，最大 100）

**批次處理**：
- 批次寫入時序數據（每 1000 點或 1 秒）
- 批次創建 Asset Servants（每批 100 個）

---

## 10. 安全性與合規性

### 10.1 認證與授權

#### 10.1.1 認證機制

**支援的認證方式**：
1. **API 金鑰**：適合機器對機器通訊
2. **OAuth 2.0**：適合用戶登入
3. **JWT (JSON Web Token)**：適合微服務間通訊
4. **mTLS (Mutual TLS)**：適合高安全性場景

**API 金鑰範例**：
```http
GET /api/v1/assets/pump_001
Authorization: Bearer ndh_api_key_1234567890abcdef
```

**OAuth 2.0 流程**：
```
1. 用戶登入 → 授權伺服器
2. 授權伺服器 → 返回 access_token
3. 用戶攜帶 access_token 訪問 NDH API
4. NDH 驗證 token → 允許訪問
```

#### 10.1.2 授權模型

**基於角色的訪問控制（RBAC）**：

| 角色 | 權限 |
|------|------|
| **Admin** | 所有權限（創建/讀取/更新/刪除） |
| **Operator** | 讀取、寫入屬性、調用命令 |
| **Viewer** | 僅讀取 |
| **Auditor** | 讀取 + 審計日誌 |

**權限範例**：
```yaml
roles:
  operator:
    permissions:
      - assets:read
      - assets:write_property
      - assets:invoke_command
    resources:
      - asset_type: pump
      - asset_type: valve
```

### 10.2 數據加密

#### 10.2.1 傳輸加密

- **TLS 1.3**：所有 API 通訊使用 HTTPS
- **mTLS**：微服務間通訊使用雙向 TLS
- **VPN/IPSec**：跨廠區通訊使用 VPN

#### 10.2.2 靜態加密

- **數據庫加密**：PostgreSQL Transparent Data Encryption (TDE)
- **檔案加密**：MinIO/S3 伺服器端加密（SSE）
- **備份加密**：使用 AES-256 加密備份檔案

### 10.3 審計日誌

**記錄內容**：
- 用戶操作（誰、何時、做了什麼）
- API 調用（請求/回應）
- 資產變更（屬性變更、命令執行）
- 系統事件（啟動/停止、錯誤）

**日誌格式**：
```json
{
  "timestamp": "2025-10-14T10:00:00Z",
  "user_id": "operator_001",
  "action": "write_property",
  "resource": "asset:pump_001:flow_rate",
  "old_value": 150.5,
  "new_value": 160.0,
  "ip_address": "192.168.1.100",
  "result": "success"
}
```

**日誌保留**：
- 操作日誌：保留 1 年
- 審計日誌：保留 7 年（合規要求）
- 系統日誌：保留 90 天

### 10.4 合規性

#### 10.4.1 GDPR（歐盟一般資料保護規範）

- **數據主權**：歐盟用戶數據僅存儲在歐盟區域
- **數據刪除**：支援用戶數據刪除請求
- **數據可攜性**：支援數據匯出

#### 10.4.2 ISO 27001（資訊安全管理）

- **訪問控制**：基於 RBAC 的細粒度權限
- **加密**：傳輸和靜態數據加密
- **審計**：完整的審計日誌

#### 10.4.3 SOC 2（服務組織控制）

- **可用性**：99.9% SLA
- **安全性**：多層安全防護
- **隱私**：數據隱私保護

---

## 11. 效能指標與優化

### 11.1 效能指標

#### 11.1.1 寫入效能

| 場景 | 吞吐量 | 延遲 |
|------|--------|------|
| **單點寫入** | 10K points/sec | < 10ms (p99) |
| **批次寫入** | 100K points/sec | < 50ms (p99) |
| **極限寫入**（TDengine） | 1.5M points/sec | < 100ms (p99) |

#### 11.1.2 查詢效能

| 查詢類型 | 延遲 |
|---------|------|
| **點查詢**（單個資產、單個屬性） | < 5ms (p99) |
| **範圍查詢**（1 小時數據） | < 50ms (p99) |
| **聚合查詢**（1 天數據，1 小時間隔） | < 100ms (p99) |
| **複雜查詢**（多資產、多指標） | < 500ms (p99) |

#### 11.1.3 API 效能

| API 端點 | 延遲 | 吞吐量 |
|---------|------|--------|
| **GET /assets/{id}** | < 10ms (p99) | 10K req/sec |
| **POST /assets/{id}/commands** | < 50ms (p99) | 5K req/sec |
| **POST /timeseries/write** | < 20ms (p99) | 50K req/sec |
| **POST /timeseries/query** | < 100ms (p99) | 2K req/sec |

### 11.2 效能優化策略

#### 11.2.1 快取優化

**多層快取**：
```
L1: 應用程式記憶體快取（LRU, 100MB）
L2: Redis 快取（10GB）
L3: 時序數據庫（無限）
```

**快取策略**：
- 熱數據（最近 1 小時）：L1 + L2
- 溫數據（最近 1 天）：L2
- 冷數據（> 1 天）：L3

**快取失效**：
- 寫入時失效（Write-through）
- TTL 過期（Time-to-Live）
- 主動刷新（Refresh-ahead）

#### 11.2.2 數據庫優化

**索引優化**：
```sql
-- 時序數據庫索引
CREATE INDEX idx_asset_timestamp ON sensor_data (asset_id, timestamp DESC);

-- PostgreSQL 索引
CREATE INDEX idx_asset_type ON assets (type);
CREATE INDEX idx_asset_location ON assets (location_id);
```

**分區策略**：
- 時序數據按時間分區（每月一個分區）
- 資產數據按類型分區

**查詢優化**：
- 使用預編譯語句（Prepared Statements）
- 避免 N+1 查詢問題
- 使用批次查詢

#### 11.2.3 並行處理

**批次創建 Asset Servants**：
```python
async def batch_create_servants(iadl_files: List[str]):
    tasks = [create_servant(file) for file in iadl_files]
    servants = await asyncio.gather(*tasks)
    return servants
```

**並行數據寫入**：
```python
async def parallel_write(points: List[dict]):
    # 分批處理
    batches = [points[i:i+1000] for i in range(0, len(points), 1000)]
    
    # 並行寫入
    tasks = [tsdb.batch_write(batch) for batch in batches]
    results = await asyncio.gather(*tasks)
    
    return all(results)
```

### 11.3 監控與告警

#### 11.3.1 監控指標

**系統指標**：
- CPU 使用率
- 記憶體使用率
- 磁碟 I/O
- 網路流量

**應用指標**：
- API 請求數
- API 延遲（p50, p95, p99）
- 錯誤率
- Asset Servants 數量

**業務指標**：
- 活躍資產數
- 數據點寫入速率
- 查詢 QPS
- 告警數量

#### 11.3.2 Prometheus + Grafana

**Prometheus 配置**：
```yaml
scrape_configs:
  - job_name: 'ndh'
    static_configs:
      - targets: ['ndh:8000']
    metrics_path: '/metrics'
    scrape_interval: 15s
```

**Grafana Dashboard**：
- NDH 總覽（CPU、記憶體、請求數）
- API 效能（延遲、吞吐量、錯誤率）
- 時序數據庫（寫入速率、查詢延遲）
- Asset Servants（數量、狀態分佈）

#### 11.3.3 告警規則

**CPU 使用率過高**：
```yaml
alert: HighCPUUsage
expr: rate(process_cpu_seconds_total[5m]) > 0.8
for: 5m
labels:
  severity: warning
annotations:
  summary: "NDH CPU usage is above 80%"
```

**API 錯誤率過高**：
```yaml
alert: HighAPIErrorRate
expr: rate(http_requests_total{status=~"5.."}[5m]) / rate(http_requests_total[5m]) > 0.01
for: 5m
labels:
  severity: critical
annotations:
  summary: "NDH API error rate is above 1%"
```

---

## 12. 實施路線圖

### 12.1 第一階段（0-3個月）：核心功能開發

**目標**：完成 NDH 核心功能，支援基本的資產管理和數據整合

**任務**：
- ✅ 實現 Asset Servants 核心邏輯
- ✅ 實現時序數據庫抽象層
- ✅ 開發 InfluxDB 和 TDengine 適配器
- ✅ 實現 RESTful API（資產管理、時序數據）
- ✅ 實現 IADL 解析器
- ✅ 實現 OPC UA 和 Modbus 連接器
- ✅ 開發基礎的 Web 管理介面

**交付物**：
- NDH V3.5 Alpha 版本
- 基礎文檔和 API 文件
- 單元測試和整合測試

### 12.2 第二階段（3-6個月）：企業應用整合

**目標**：實現 IT/OT 融合，整合 MES、ERP 等企業應用

**任務**：
- ✅ 開發 MES 連接器（Siemens Opcenter、Rockwell FactoryTalk）
- ✅ 開發 ERP 連接器（SAP S/4HANA、Oracle ERP）
- ✅ 開發 PLM 連接器（PTC Windchill、Siemens Teamcenter）
- ✅ 實現數據轉換引擎
- ✅ 實現 FDL 解析器和工廠部署功能
- ✅ 開發 GraphQL API
- ✅ 實現 WebSocket 即時通訊

**交付物**：
- NDH V3.5 Beta 版本
- 企業應用整合文檔
- 整合測試案例

### 12.3 第三階段（6-9個月）：高級功能與優化

**目標**：實現 MEP 設計、衝突檢測等高級功能，優化效能

**任務**：
- ✅ 實現 MEP 設計服務
- ✅ 實現衝突檢測服務（GJK/EPA 演算法）
- ✅ 實現 ISO 圖面生成服務
- ✅ 實現流體分析服務（CFD）
- ✅ 實現即時協同服務（Omniverse 整合）
- ✅ 效能優化（快取、並行處理）
- ✅ 開發 Prometheus 監控和 Grafana Dashboard

**交付物**：
- NDH V3.5 RC 版本
- 高級功能文檔
- 效能測試報告

### 12.4 第四階段（9-12個月）：生產就緒與部署

**目標**：完成生產環境部署，支援集群和高可用性

**任務**：
- ✅ 實現高可用性架構（集群部署）
- ✅ 開發 Kubernetes Helm Chart
- ✅ 實現數據備份和恢復
- ✅ 實現安全性增強（認證、授權、加密）
- ✅ 開發完整的管理介面
- ✅ 撰寫完整的用戶文檔和運維文檔
- ✅ 進行安全審計和合規性檢查

**交付物**：
- NDH V3.5 正式版本
- 生產部署指南
- 安全和合規性報告

### 12.5 第五階段（12個月後）：持續改進與擴展

**目標**：根據用戶反饋持續改進，擴展新功能

**任務**：
- 🔄 支援更多時序數據庫（Prometheus、VictoriaMetrics）
- 🔄 支援更多企業應用（CMMS、WMS、QMS）
- 🔄 支援更多工業協議（Profinet、EtherCAT、BACnet）
- 🔄 AI/ML 整合（預測性維護、異常檢測）
- 🔄 邊緣計算支援（輕量級 NDH）
- 🔄 多租戶支援（SaaS 模式）

**交付物**：
- NDH V4.0 規劃
- 新功能原型
- 用戶反饋報告

---

## 13. 附錄

### 13.1 術語表

| 術語 | 全稱 | 說明 |
|------|------|------|
| **IDTF** | Industrial Digital Twin Framework | 工業數位分身框架 |
| **NDH** | Neutral Data Hub | 中立數據中樞 |
| **IADL** | Industrial Asset Description Language | 工業資產定義語言 |
| **FDL** | Factory Design Language | 工廠設計語言 |
| **TSDB** | Time Series Database | 時序數據庫 |
| **MEP** | Mechanical, Electrical, Plumbing | 機電管線 |
| **OPC UA** | OPC Unified Architecture | OPC 統一架構 |
| **SCADA** | Supervisory Control and Data Acquisition | 監控與數據採集系統 |
| **MES** | Manufacturing Execution System | 製造執行系統 |
| **ERP** | Enterprise Resource Planning | 企業資源規劃 |
| **PLM** | Product Lifecycle Management | 產品生命週期管理 |
| **WMS** | Warehouse Management System | 倉儲管理系統 |
| **QMS** | Quality Management System | 品質管理系統 |
| **CMMS** | Computerized Maintenance Management System | 電腦化維護管理系統 |
| **CFD** | Computational Fluid Dynamics | 計算流體力學 |
| **USD** | Universal Scene Description | 通用場景描述 |
| **GJK** | Gilbert-Johnson-Keerthi | GJK 演算法（碰撞檢測） |
| **EPA** | Expanding Polytope Algorithm | 擴展多面體演算法 |
| **BVH** | Bounding Volume Hierarchy | 包圍盒層次結構 |

### 13.2 參考文獻

1. **IDTF V3.5 核心文檔**
   - IDTF Overview
   - IADL Specification
   - FDL Specification

2. **NDH 相關文檔**
   - NDH Architecture Design
   - NDH Pluggable TSDB Architecture
   - IADL/FDL/NDH Interaction Guide

3. **時序數據庫文檔**
   - InfluxDB Documentation
   - TDengine Documentation
   - TimescaleDB Documentation
   - QuestDB Documentation

4. **工業協議標準**
   - OPC UA Specification (IEC 62541)
   - Modbus Protocol Specification
   - MQTT v5.0 Specification

5. **企業應用整合**
   - SAP S/4HANA Integration Guide
   - Siemens Opcenter API Documentation
   - PTC Windchill Integration Guide

### 13.3 聯絡資訊

**MacroVision Systems**

- **官方網站**：https://macrovision.systems
- **技術支援**：support@macrovision.systems
- **商務洽詢**：sales@macrovision.systems

**作者**：
- 林志錚 (Chih Cheng Lin, Michael Lin)
- Email: chchlin1018@gmail.com
- GitHub: https://github.com/chchlin1018

---

**文件版本**：3.5  
**最後更新**：2025年10月14日  
**文件狀態**：正式版（Final）

---

## 總結

NDH (Neutral Data Hub) 是 IDTF V3.5 框架的核心組件，提供了一個**供應商中立、高效能、可擴展的工業數據整合中樞**。通過以下核心特性，NDH 解決了工業數位化轉型中的關鍵挑戰：

### 核心價值

1. **供應商中立**：開源架構，零鎖定風險，可抽換時序數據庫
2. **IT/OT 融合**：統一整合 MES、ERP、SCADA、PLC 等系統
3. **全生命週期覆蓋**：從設計、建造到運維的端到端解決方案
4. **低成本高效能**：相較專有方案節省 70% 成本，支援百萬級數據點/秒
5. **易於擴展**：微服務架構，可根據需求靈活擴展

### 技術創新

1. **Asset Servants**：將靜態 IADL 定義轉換為動態執行時模型
2. **可抽換時序數據庫**：支援 InfluxDB、TDengine、TimescaleDB、QuestDB
3. **統一 API**：RESTful API、GraphQL、WebSocket 全面支援
4. **智能分析引擎**：MEP 設計、衝突檢測、流體分析等原生能力
5. **即時協同**：與 Omniverse 深度整合，支援多用戶協作

### 商業效益

1. **整合成本 ↓ 70%**：統一的數據模型和 API，減少客製化開發
2. **數據整合時間 ↓ 80%**：自動化部署，快速上線
3. **供應商風險 ↓ 100%**：開源架構，零鎖定
4. **ROI：6-12 個月**：快速回收投資

NDH 不僅是一個技術平台，更是工業數位化轉型的**戰略基礎設施**，為企業提供了一個開放、中立、高效的數據整合解決方案，助力企業實現真正的 IT/OT 融合和數位化轉型。

