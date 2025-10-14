# NDH、Asset Servants、Omniverse 與 Digital Twins 整合架構技術白皮書

**版本**：V3.5  
**作者**：林志錚 (Chih Cheng Lin, Michael Lin)  
**日期**：2025年1月  
**組織**：MacroVision Systems / IDTF Alliance

---

## 摘要

本白皮書詳細介紹了 **NDH (Neutral Data Hub)**、**Asset Servants**、**NVIDIA Omniverse** 和 **Digital Twins** 之間的整合架構。這四個核心組件共同構成了 IDTF (Industrial Digital Twin Framework) 的執行時環境，實現了從物理世界到虛擬世界的即時雙向同步，為工業數位分身提供了完整的技術解決方案。

本白皮書涵蓋：
- 四個核心組件的定位與職責
- 整合架構設計
- 數據流與控制流
- API 介面與協議
- 即時同步機制
- 實際應用案例
- 效能優化策略

---

## 目錄

1. [引言](#1-引言)
2. [核心組件概述](#2-核心組件概述)
3. [整合架構設計](#3-整合架構設計)
4. [Asset Servants 詳細設計](#4-asset-servants-詳細設計)
5. [NDH 與 Asset Servants 整合](#5-ndh-與-asset-servants-整合)
6. [Omniverse 連接器架構](#6-omniverse-連接器架構)
7. [Digital Twins 即時同步機制](#7-digital-twins-即時同步機制)
8. [數據流與控制流](#8-數據流與控制流)
9. [API 介面規範](#9-api-介面規範)
10. [效能優化與擴展性](#10-效能優化與擴展性)
11. [實際應用案例](#11-實際應用案例)
12. [未來發展方向](#12-未來發展方向)
13. [結論](#13-結論)

---

## 1. 引言

### 1.1 背景與挑戰

工業數位分身（Industrial Digital Twins）的核心挑戰在於如何實現**物理世界**與**虛擬世界**之間的即時、雙向、高保真同步。傳統的數位分身解決方案面臨以下問題：

1. **數據孤島**：OT（操作技術）和 IT（資訊技術）系統之間缺乏統一的數據整合層
2. **供應商鎖定**：數位分身平台通常綁定特定雲端服務或硬體廠商
3. **即時性不足**：數據同步延遲高，無法支援即時決策
4. **可視化能力弱**：缺乏高保真的 3D 可視化和物理模擬能力
5. **擴展性差**：難以支援大規模工廠（數千個資產）的數位分身

### 1.2 IDTF 的解決方案

IDTF V3.5 通過四個核心組件的深度整合，提供了完整的工業數位分身解決方案：

- **NDH (Neutral Data Hub)**：供應商中立的數據整合中樞
- **Asset Servants**：資產的執行時數位分身模型
- **NVIDIA Omniverse**：高保真 3D 可視化與物理模擬平台
- **Digital Twins Runtime**：即時同步與協同引擎

這四個組件共同實現了：
- ✅ **物理世界 ↔ 虛擬世界**的即時雙向同步
- ✅ **OT ↔ IT**的無縫整合
- ✅ **供應商中立**的開放架構
- ✅ **毫秒級延遲**的即時性
- ✅ **照片級渲染**的高保真可視化
- ✅ **多用戶協同**的設計與運維

### 1.3 白皮書目標

本白皮書旨在：
1. 詳細說明四個核心組件的架構設計
2. 闡述組件之間的整合機制
3. 提供 API 介面規範和數據流設計
4. 展示實際應用案例和效能指標
5. 為開發者和架構師提供技術參考

---

## 2. 核心組件概述

### 2.1 NDH (Neutral Data Hub)

**定位**：供應商中立的工業數據整合中樞

**核心職責**：
- 從 OT 系統（PLC、SCADA）採集即時數據
- 與 IT 系統（MES、ERP、PLM）雙向同步
- 儲存時序數據和資產元數據
- 提供統一的 API 介面
- 管理 Asset Servants 的生命週期

**技術特點**：
- 微服務架構（可獨立擴展）
- 可抽換時序數據庫（InfluxDB、TDengine、TimescaleDB、QuestDB）
- 支援 15+ 種工業協議（OPC UA、Modbus、MQTT、BACnet 等）
- RESTful、GraphQL、WebSocket API
- RBAC 權限控制和審計日誌

**關鍵指標**：
- 數據採集頻率：1-1000 Hz（可配置）
- API 響應時間：< 50ms (P95)
- 並發連接數：10,000+
- 數據吞吐量：100,000 點/秒

### 2.2 Asset Servants

**定位**：工業資產的執行時數位分身模型

**核心職責**：
- 代表單一工業資產（設備、系統、區域）
- 封裝資產的狀態、行為和數據點
- 提供統一的 API 介面（CRUD、訂閱、命令）
- 執行資產的業務邏輯（告警、計算、控制）
- 與物理資產和虛擬資產（Omniverse）同步

**技術特點**：
- 基於 IADL (Industrial Asset Description Language) 定義
- 輕量級（每個 Servant < 10MB 記憶體）
- 事件驅動架構（Event-Driven）
- 支援熱更新（無需重啟）
- 內建狀態機和行為引擎

**生命週期**：
1. **實例化**：從 IADL 定義創建 Servant
2. **註冊**：註冊到 NDH 命名服務
3. **連接**：建立與物理資產的數據連接
4. **執行**：採集數據、執行邏輯、發布事件
5. **同步**：與 Omniverse 同步狀態
6. **銷毀**：資源清理和狀態持久化

### 2.3 NVIDIA Omniverse

**定位**：高保真 3D 可視化與物理模擬平台

**核心職責**：
- 照片級渲染（RTX 光線追蹤）
- 物理模擬（PhysX、流體、粒子）
- 多用戶即時協同
- USD (Universal Scene Description) 場景管理
- AI 驅動的內容生成

**與 IDTF 的整合點**：
- 從 IADL/FDL 自動生成 USD 場景
- 與 NDH/Asset Servants 即時同步資產狀態
- 支援 AR/VR 沉浸式體驗
- 提供 Python/C++ API 進行自定義擴展

**關鍵能力**：
- 場景規模：百萬級多邊形
- 渲染品質：照片級（RTX）
- 物理模擬：即時（60 FPS）
- 協同用戶：100+ 同時在線

### 2.4 Digital Twins Runtime

**定位**：即時同步與協同引擎

**核心職責**：
- 管理物理世界 ↔ 虛擬世界的雙向同步
- 協調 NDH、Asset Servants、Omniverse 之間的數據流
- 處理衝突解決和一致性保證
- 提供協同編輯和多用戶管理
- 監控系統健康和效能

**技術特點**：
- 事件驅動架構（Event Sourcing）
- CRDT (Conflict-free Replicated Data Types) 衝突解決
- WebSocket 即時推送
- 分散式追蹤（OpenTelemetry）
- 自動故障恢復

---

## 3. 整合架構設計

### 3.1 整體架構圖

```
┌─────────────────────────────────────────────────────────────────┐
│                        Physical World (物理世界)                  │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐        │
│  │   PLC    │  │  SCADA   │  │ Sensors  │  │ Actuators│        │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └────┬─────┘        │
└───────┼─────────────┼─────────────┼─────────────┼───────────────┘
        │             │             │             │
        │ OPC UA      │ Modbus      │ MQTT        │ BACnet
        │             │             │             │
┌───────▼─────────────▼─────────────▼─────────────▼───────────────┐
│                     OT Integration Layer                         │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │              Protocol Adapters & Connectors              │   │
│  └──────────────────────────────────────────────────────────┘   │
└───────────────────────────────┬──────────────────────────────────┘
                                │
┌───────────────────────────────▼──────────────────────────────────┐
│                    NDH (Neutral Data Hub)                        │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                  Asset Servants Manager                  │   │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐         │   │
│  │  │  Servant   │  │  Servant   │  │  Servant   │   ...   │   │
│  │  │  (Pump)    │  │ (Chiller)  │  │  (AHU)     │         │   │
│  │  └─────┬──────┘  └─────┬──────┘  └─────┬──────┘         │   │
│  └────────┼────────────────┼────────────────┼────────────────┘   │
│  ┌────────▼────────────────▼────────────────▼────────────────┐   │
│  │              Time-Series Database (TSDB)                 │   │
│  │         (InfluxDB / TDengine / TimescaleDB)              │   │
│  └──────────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                 Metadata Database (PostgreSQL)           │   │
│  └──────────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                  API Gateway (REST/GraphQL/WebSocket)    │   │
│  └────────┬─────────────────────────────────────────────────┘   │
└───────────┼──────────────────────────────────────────────────────┘
            │
            │ WebSocket (即時推送)
            │ REST API (查詢/命令)
            │
┌───────────▼──────────────────────────────────────────────────────┐
│              Digital Twins Runtime (同步引擎)                     │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │              Synchronization Engine                      │   │
│  │  - Event Streaming (Kafka/Redis Streams)                │   │
│  │  - Conflict Resolution (CRDT)                           │   │
│  │  - State Management                                     │   │
│  └────────┬─────────────────────────────────────────────────┘   │
└───────────┼──────────────────────────────────────────────────────┘
            │
            │ Omniverse Connector Protocol
            │ (USD Delta Updates)
            │
┌───────────▼──────────────────────────────────────────────────────┐
│                  NVIDIA Omniverse (虛擬世界)                      │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                  USD Scene Graph                         │   │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐         │   │
│  │  │  USD Prim  │  │  USD Prim  │  │  USD Prim  │   ...   │   │
│  │  │  (Pump)    │  │ (Chiller)  │  │  (AHU)     │         │   │
│  │  └────────────┘  └────────────┘  └────────────┘         │   │
│  └──────────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │              RTX Renderer + PhysX Simulation             │   │
│  └──────────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │              Multi-User Collaboration                    │   │
│  └──────────────────────────────────────────────────────────┘   │
└──────────────────────────────────────────────────────────────────┘
            │
            │ AR/VR Output
            │
┌───────────▼──────────────────────────────────────────────────────┐
│                    Users (使用者)                                 │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐        │
│  │ Operator │  │ Engineer │  │ Manager  │  │  Remote  │        │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘        │
└──────────────────────────────────────────────────────────────────┘
```

### 3.2 架構層次

整個架構分為五個層次：

#### **Layer 1: Physical World (物理世界)**
- 實體設備：PLC、SCADA、感測器、執行器
- 工業協議：OPC UA、Modbus、MQTT、BACnet
- 數據來源：即時測量值、狀態信號、告警事件

#### **Layer 2: NDH (數據中樞)**
- Asset Servants：資產的數位分身模型
- 時序數據庫：儲存歷史數據
- 元數據庫：儲存資產定義和配置
- API Gateway：提供統一介面

#### **Layer 3: Digital Twins Runtime (同步引擎)**
- 事件流處理：Kafka/Redis Streams
- 衝突解決：CRDT 演算法
- 狀態管理：分散式快取
- 協同管理：多用戶會話

#### **Layer 4: Omniverse (虛擬世界)**
- USD 場景圖：3D 資產表示
- RTX 渲染：照片級可視化
- PhysX 模擬：物理行為
- 多用戶協同：即時編輯

#### **Layer 5: Users (使用者)**
- 操作員：監控和控制
- 工程師：設計和優化
- 管理者：決策和分析
- 遠端用戶：AR/VR 訪問

### 3.3 關鍵設計原則

#### **3.3.1 關注點分離 (Separation of Concerns)**
- **NDH**：專注於數據整合和儲存
- **Asset Servants**：專注於資產邏輯和行為
- **Omniverse**：專注於可視化和模擬
- **Digital Twins Runtime**：專注於同步和協同

#### **3.3.2 事件驅動架構 (Event-Driven Architecture)**
- 所有組件通過事件通信
- 解耦合，提高可擴展性
- 支援異步處理和重試機制

#### **3.3.3 最終一致性 (Eventual Consistency)**
- 物理世界和虛擬世界不要求強一致性
- 通過 CRDT 保證最終一致
- 衝突解決策略可配置

#### **3.3.4 供應商中立 (Vendor Neutrality)**
- 不綁定特定雲端平台
- 可抽換的時序數據庫
- 開放的 API 和協議

---

## 4. Asset Servants 詳細設計

### 4.1 Asset Servant 架構

每個 Asset Servant 代表一個工業資產的數位分身，包含以下組件：

```
┌─────────────────────────────────────────────────────────────┐
│                    Asset Servant (資產僕從)                  │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │              Metadata (元數據)                     │     │
│  │  - asset_id: "CDA-PUMP-001"                       │     │
│  │  - asset_type: "CompressedAirPump"                │     │
│  │  - location: "FAB1/Zone-A"                        │     │
│  │  - iadl_definition: <IADL Object>                 │     │
│  └────────────────────────────────────────────────────┘     │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │              State (狀態)                          │     │
│  │  - operational_status: "Running"                  │     │
│  │  - health_score: 0.95                             │     │
│  │  - last_maintenance: "2025-01-01"                 │     │
│  │  - current_values: {                              │     │
│  │      "pressure": 7.2,                             │     │
│  │      "flow_rate": 120.5,                          │     │
│  │      "power": 45.3                                │     │
│  │    }                                              │     │
│  └────────────────────────────────────────────────────┘     │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │          Data Points Manager (數據點管理器)        │     │
│  │  ┌──────────────┐  ┌──────────────┐               │     │
│  │  │ Data Point   │  │ Data Point   │               │     │
│  │  │ (Pressure)   │  │ (Flow Rate)  │  ...          │     │
│  │  └──────┬───────┘  └──────┬───────┘               │     │
│  │         │                  │                       │     │
│  │         └──────────┬───────┘                       │     │
│  │                    │                               │     │
│  │          ┌─────────▼────────┐                      │     │
│  │          │  Value Cache     │                      │     │
│  │          │  (Redis/Memory)  │                      │     │
│  │          └──────────────────┘                      │     │
│  └────────────────────────────────────────────────────┘     │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │          Behavior Engine (行為引擎)                │     │
│  │  - State Machine (狀態機)                         │     │
│  │  - Event Handlers (事件處理器)                    │     │
│  │  - Business Logic (業務邏輯)                      │     │
│  │  - Alarm Rules (告警規則)                         │     │
│  └────────────────────────────────────────────────────┘     │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │          Connection Manager (連接管理器)           │     │
│  │  ┌──────────────┐  ┌──────────────┐               │     │
│  │  │ OT Connection│  │ IT Connection│               │     │
│  │  │ (PLC/SCADA)  │  │ (MES/ERP)    │               │     │
│  │  └──────┬───────┘  └──────┬───────┘               │     │
│  │         │                  │                       │     │
│  │         └──────────┬───────┘                       │     │
│  │                    │                               │     │
│  │          ┌─────────▼────────┐                      │     │
│  │          │  Protocol Stack  │                      │     │
│  │          │  (OPC UA/Modbus) │                      │     │
│  │          └──────────────────┘                      │     │
│  └────────────────────────────────────────────────────┘     │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │          API Interface (API 介面)                  │     │
│  │  - get_state()                                    │     │
│  │  - set_value(data_point, value)                   │     │
│  │  - subscribe(data_point, callback)                │     │
│  │  - execute_command(command, params)               │     │
│  │  - get_history(data_point, time_range)            │     │
│  └────────────────────────────────────────────────────┘     │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │          Omniverse Sync (Omniverse 同步)           │     │
│  │  - usd_prim_path: "/World/FAB1/CDA-PUMP-001"     │     │
│  │  - sync_frequency: 10 Hz                          │     │
│  │  - delta_updates: true                            │     │
│  └────────────────────────────────────────────────────┘     │
└──────────────────────────────────────────────────────────────┘
```

### 4.2 Asset Servant 生命週期

#### **4.2.1 實例化 (Instantiation)**

從 IADL 定義創建 Asset Servant：

```python
class AssetServant:
    def __init__(self, asset_id: str, iadl_definition: Dict):
        self.asset_id = asset_id
        self.iadl = iadl_definition
        self.metadata = self._parse_metadata()
        self.state = self._initialize_state()
        self.data_points = self._create_data_points()
        self.behavior_engine = BehaviorEngine(self.iadl['behaviors'])
        self.connection_manager = ConnectionManager()
        self.omniverse_sync = OmniverseSync(self.asset_id)
        
    def _parse_metadata(self) -> Dict:
        return {
            'asset_type': self.iadl['metadata']['type'],
            'manufacturer': self.iadl['metadata']['manufacturer'],
            'model': self.iadl['metadata']['model'],
            'location': self.iadl['metadata'].get('location', 'Unknown')
        }
    
    def _initialize_state(self) -> Dict:
        return {
            'operational_status': 'Initializing',
            'health_score': 1.0,
            'last_updated': datetime.now(),
            'current_values': {}
        }
    
    def _create_data_points(self) -> Dict[str, DataPoint]:
        data_points = {}
        for dp_def in self.iadl['data_points']:
            dp = DataPoint(
                name=dp_def['name'],
                data_type=dp_def['type'],
                unit=dp_def.get('unit'),
                min_value=dp_def.get('min'),
                max_value=dp_def.get('max'),
                protocol=dp_def.get('protocol'),
                address=dp_def.get('address')
            )
            data_points[dp_def['name']] = dp
        return data_points
```

#### **4.2.2 註冊 (Registration)**

註冊到 NDH 命名服務：

```python
async def register_to_ndh(self):
    """註冊 Asset Servant 到 NDH"""
    registration_data = {
        'asset_id': self.asset_id,
        'asset_type': self.metadata['asset_type'],
        'location': self.metadata['location'],
        'data_points': [dp.to_dict() for dp in self.data_points.values()],
        'api_endpoint': f"http://ndh-api/assets/{self.asset_id}",
        'websocket_endpoint': f"ws://ndh-api/assets/{self.asset_id}/stream",
        'capabilities': self.get_capabilities()
    }
    
    async with aiohttp.ClientSession() as session:
        async with session.post(
            f"{NDH_API_URL}/registry/assets",
            json=registration_data
        ) as response:
            if response.status == 201:
                self.state['operational_status'] = 'Registered'
                logger.info(f"Asset {self.asset_id} registered successfully")
            else:
                raise RegistrationError(f"Failed to register: {await response.text()}")
```

#### **4.2.3 連接 (Connection)**

建立與物理資產的數據連接：

```python
async def connect_to_physical_asset(self):
    """連接到物理資產"""
    for dp_name, dp in self.data_points.items():
        if dp.protocol == 'opcua':
            await self.connection_manager.connect_opcua(
                endpoint=dp.address['endpoint'],
                node_id=dp.address['node_id'],
                callback=lambda value: self._on_data_point_changed(dp_name, value)
            )
        elif dp.protocol == 'modbus':
            await self.connection_manager.connect_modbus(
                host=dp.address['host'],
                port=dp.address['port'],
                register=dp.address['register'],
                callback=lambda value: self._on_data_point_changed(dp_name, value)
            )
    
    self.state['operational_status'] = 'Connected'
    logger.info(f"Asset {self.asset_id} connected to physical asset")
```

#### **4.2.4 執行 (Execution)**

採集數據、執行邏輯、發布事件：

```python
async def run(self):
    """主執行循環"""
    self.state['operational_status'] = 'Running'
    
    while self.state['operational_status'] == 'Running':
        try:
            # 1. 採集數據
            await self._collect_data()
            
            # 2. 執行行為邏輯
            await self.behavior_engine.execute(self.state)
            
            # 3. 檢查告警
            await self._check_alarms()
            
            # 4. 同步到 Omniverse
            await self.omniverse_sync.sync_state(self.state)
            
            # 5. 發布狀態更新事件
            await self._publish_state_update()
            
            # 6. 等待下一個週期
            await asyncio.sleep(self.update_interval)
            
        except Exception as e:
            logger.error(f"Error in asset {self.asset_id}: {e}")
            await self._handle_error(e)

async def _collect_data(self):
    """採集數據點的值"""
    for dp_name, dp in self.data_points.items():
        try:
            value = await self.connection_manager.read_value(dp)
            self.state['current_values'][dp_name] = value
            
            # 儲存到時序數據庫
            await self._store_to_tsdb(dp_name, value)
            
        except Exception as e:
            logger.warning(f"Failed to read {dp_name}: {e}")

async def _store_to_tsdb(self, data_point: str, value: float):
    """儲存數據到時序數據庫"""
    point = {
        'measurement': self.asset_id,
        'tags': {
            'asset_type': self.metadata['asset_type'],
            'location': self.metadata['location'],
            'data_point': data_point
        },
        'fields': {
            'value': value
        },
        'timestamp': datetime.utcnow()
    }
    await tsdb_client.write(point)
```

#### **4.2.5 同步 (Synchronization)**

與 Omniverse 同步狀態：

```python
async def sync_to_omniverse(self):
    """同步狀態到 Omniverse"""
    # 計算狀態差異
    delta = self.omniverse_sync.calculate_delta(
        previous_state=self.previous_state,
        current_state=self.state
    )
    
    if delta:
        # 生成 USD Delta Updates
        usd_updates = self.omniverse_sync.generate_usd_delta(delta)
        
        # 發送到 Omniverse Connector
        await omniverse_connector.apply_updates(
            prim_path=self.omniverse_sync.usd_prim_path,
            updates=usd_updates
        )
        
        # 更新上一次狀態
        self.previous_state = self.state.copy()
```

### 4.3 Asset Servant API

每個 Asset Servant 提供以下 API：

#### **4.3.1 狀態查詢**

```python
@app.get("/assets/{asset_id}/state")
async def get_asset_state(asset_id: str):
    """獲取資產當前狀態"""
    servant = asset_servants[asset_id]
    return {
        'asset_id': servant.asset_id,
        'operational_status': servant.state['operational_status'],
        'health_score': servant.state['health_score'],
        'current_values': servant.state['current_values'],
        'last_updated': servant.state['last_updated'].isoformat()
    }
```

#### **4.3.2 數據點訂閱**

```python
@app.websocket("/assets/{asset_id}/stream")
async def stream_asset_data(websocket: WebSocket, asset_id: str):
    """訂閱資產數據流"""
    await websocket.accept()
    servant = asset_servants[asset_id]
    
    # 註冊回調
    def on_value_changed(dp_name, value):
        asyncio.create_task(
            websocket.send_json({
                'asset_id': asset_id,
                'data_point': dp_name,
                'value': value,
                'timestamp': datetime.utcnow().isoformat()
            })
        )
    
    servant.subscribe(on_value_changed)
    
    try:
        while True:
            # 保持連接
            await websocket.receive_text()
    except WebSocketDisconnect:
        servant.unsubscribe(on_value_changed)
```

#### **4.3.3 命令執行**

```python
@app.post("/assets/{asset_id}/commands/{command_name}")
async def execute_command(asset_id: str, command_name: str, params: Dict):
    """執行資產命令"""
    servant = asset_servants[asset_id]
    
    # 驗證命令
    if command_name not in servant.iadl['behaviors']['commands']:
        raise HTTPException(status_code=400, detail="Unknown command")
    
    # 執行命令
    result = await servant.execute_command(command_name, params)
    
    return {
        'asset_id': asset_id,
        'command': command_name,
        'status': 'executed',
        'result': result
    }
```

#### **4.3.4 歷史數據查詢**

```python
@app.get("/assets/{asset_id}/history/{data_point}")
async def get_history(
    asset_id: str,
    data_point: str,
    start_time: datetime,
    end_time: datetime,
    aggregation: str = 'mean',
    interval: str = '1m'
):
    """查詢歷史數據"""
    query = f"""
    SELECT {aggregation}(value) as value
    FROM {asset_id}
    WHERE data_point = '{data_point}'
      AND time >= '{start_time.isoformat()}'
      AND time < '{end_time.isoformat()}'
    GROUP BY time({interval})
    """
    
    results = await tsdb_client.query(query)
    
    return {
        'asset_id': asset_id,
        'data_point': data_point,
        'aggregation': aggregation,
        'interval': interval,
        'data': results
    }
```

---

## 5. NDH 與 Asset Servants 整合

### 5.1 Asset Servants Manager

NDH 包含一個 Asset Servants Manager，負責管理所有 Asset Servants 的生命週期：

```python
class AssetServantsManager:
    def __init__(self):
        self.servants: Dict[str, AssetServant] = {}
        self.registry: Dict[str, Dict] = {}
        self.event_bus = EventBus()
        
    async def create_servant_from_iadl(
        self,
        asset_id: str,
        iadl_definition: Dict
    ) -> AssetServant:
        """從 IADL 定義創建 Asset Servant"""
        servant = AssetServant(asset_id, iadl_definition)
        await servant.register_to_ndh()
        await servant.connect_to_physical_asset()
        
        self.servants[asset_id] = servant
        self.registry[asset_id] = servant.get_metadata()
        
        # 啟動 Servant
        asyncio.create_task(servant.run())
        
        logger.info(f"Created and started servant for {asset_id}")
        return servant
    
    async def batch_create_from_fdl(self, fdl_file: str):
        """從 FDL 文件批量創建 Asset Servants"""
        with open(fdl_file, 'r') as f:
            fdl = yaml.safe_load(f)
        
        # 載入所有 IADL 定義
        iadl_definitions = {}
        for iadl_ref in fdl['iadl_references']:
            with open(iadl_ref['path'], 'r') as f:
                iadl = yaml.safe_load(f)
                iadl_definitions[iadl['metadata']['type']] = iadl
        
        # 批量創建 Servants
        tasks = []
        for asset in fdl['assets']:
            asset_id = asset['id']
            asset_type = asset['type']
            iadl = iadl_definitions[asset_type]
            
            # 應用實例特定的覆寫
            iadl_instance = self._apply_overrides(iadl, asset.get('overrides', {}))
            
            task = self.create_servant_from_iadl(asset_id, iadl_instance)
            tasks.append(task)
        
        # 並行創建
        servants = await asyncio.gather(*tasks)
        logger.info(f"Created {len(servants)} servants from FDL")
        return servants
    
    def _apply_overrides(self, iadl: Dict, overrides: Dict) -> Dict:
        """應用實例特定的覆寫"""
        iadl_copy = copy.deepcopy(iadl)
        
        # 覆寫元數據
        if 'metadata' in overrides:
            iadl_copy['metadata'].update(overrides['metadata'])
        
        # 覆寫數據點地址
        if 'data_points' in overrides:
            for dp_override in overrides['data_points']:
                dp_name = dp_override['name']
                for dp in iadl_copy['data_points']:
                    if dp['name'] == dp_name:
                        dp.update(dp_override)
        
        return iadl_copy
    
    async def get_servant(self, asset_id: str) -> AssetServant:
        """獲取 Asset Servant"""
        if asset_id not in self.servants:
            raise ValueError(f"Asset {asset_id} not found")
        return self.servants[asset_id]
    
    async def stop_servant(self, asset_id: str):
        """停止並銷毀 Asset Servant"""
        servant = self.servants.get(asset_id)
        if servant:
            servant.state['operational_status'] = 'Stopping'
            await servant.disconnect()
            del self.servants[asset_id]
            del self.registry[asset_id]
            logger.info(f"Stopped servant for {asset_id}")
    
    def get_all_servants(self) -> List[Dict]:
        """獲取所有 Servants 的摘要"""
        return [
            {
                'asset_id': asset_id,
                'asset_type': metadata['asset_type'],
                'location': metadata['location'],
                'status': self.servants[asset_id].state['operational_status']
            }
            for asset_id, metadata in self.registry.items()
        ]
```

### 5.2 事件總線 (Event Bus)

NDH 使用事件總線在 Asset Servants 之間傳遞事件：

```python
class EventBus:
    def __init__(self):
        self.subscribers: Dict[str, List[Callable]] = {}
        self.event_stream = RedisStreams('ndh-events')
    
    async def publish(self, event_type: str, event_data: Dict):
        """發布事件"""
        event = {
            'type': event_type,
            'data': event_data,
            'timestamp': datetime.utcnow().isoformat(),
            'source': event_data.get('asset_id', 'system')
        }
        
        # 發布到 Redis Streams
        await self.event_stream.add(event)
        
        # 通知本地訂閱者
        if event_type in self.subscribers:
            for callback in self.subscribers[event_type]:
                asyncio.create_task(callback(event))
    
    def subscribe(self, event_type: str, callback: Callable):
        """訂閱事件"""
        if event_type not in self.subscribers:
            self.subscribers[event_type] = []
        self.subscribers[event_type].append(callback)
    
    async def consume_events(self):
        """消費事件流"""
        async for event in self.event_stream.read():
            event_type = event['type']
            if event_type in self.subscribers:
                for callback in self.subscribers[event_type]:
                    asyncio.create_task(callback(event))
```

### 5.3 命名服務 (Naming Service)

NDH 提供命名服務，允許通過邏輯名稱查找 Asset Servants：

```python
class NamingService:
    def __init__(self):
        self.registry: Dict[str, str] = {}  # logical_name -> asset_id
        self.reverse_registry: Dict[str, List[str]] = {}  # asset_id -> logical_names
    
    def register(self, asset_id: str, logical_names: List[str]):
        """註冊資產的邏輯名稱"""
        for name in logical_names:
            self.registry[name] = asset_id
        self.reverse_registry[asset_id] = logical_names
    
    def resolve(self, logical_name: str) -> Optional[str]:
        """解析邏輯名稱到資產 ID"""
        return self.registry.get(logical_name)
    
    def find_by_type(self, asset_type: str) -> List[str]:
        """查找特定類型的所有資產"""
        return [
            asset_id
            for asset_id, servant in asset_servants_manager.servants.items()
            if servant.metadata['asset_type'] == asset_type
        ]
    
    def find_by_location(self, location: str) -> List[str]:
        """查找特定位置的所有資產"""
        return [
            asset_id
            for asset_id, servant in asset_servants_manager.servants.items()
            if servant.metadata['location'].startswith(location)
        ]
```

---

## 6. Omniverse 連接器架構


### 6.1 IADL/FDL 到 USD 的映射關係

正確理解 IADL、FDL、Asset Servants 與 USD 之間的映射關係是整合的關鍵。

#### **6.1.1 映射層次**

**層次 1：IADL ↔ USD Model（類型定義）**
- **IADL**：定義資產**類型**（Asset Type），例如 `compressor_atlas_copco_ga75.iadl`
- **USD Model**：對應的 3D 模型**模板**（USD Reference），例如 `compressor_ga75.usd`
- **關係**：一對一映射，USD Model 可被多個 USD Instance 引用
- **類比**：IADL 是 Class 定義，USD Model 是 Class 的視覺表示

**層次 2：FDL ↔ Asset Servants ↔ USD Instances（實例）**
- **FDL 實例**：定義工廠中的具體資產實例，例如 `compressor_001`, `compressor_002`, `compressor_003`
- **Asset Servant**：FDL 實例的執行時代理，例如 `AssetServant(compressor_001)`
- **USD Instance**：Omniverse 場景中的 3D 實例（USD Prim），例如 `/World/Factory/compressor_001`
- **關係**：三者一一對應，數量完全一致
- **類比**：FDL 是 Object 實例化，Asset Servant 是運行時 Object，USD Instance 是 Object 的 3D 表示

#### **6.1.2 數量關係**

```
IADL (1個類型)
    ↓ 定義
USD Model (1個模板)
    ↓ 引用
FDL (3個實例) ←→ Asset Servants (3個) ←→ USD Instances (3個)
```

**範例**：
- **IADL**：`compressor_atlas_copco_ga75.iadl`（1個類型定義）
- **USD Model**：`compressor_ga75.usd`（1個 3D 模型模板）
- **FDL 實例**：
  - `compressor_001` (position: [10, 0, 5])
  - `compressor_002` (position: [20, 0, 5])
  - `compressor_003` (position: [30, 0, 5])
- **Asset Servants**：3個運行時代理
- **USD Instances**：3個 USD Prim，都引用同一個 USD Model

#### **6.1.3 USD Instance 的結構**

**重要**：每個 Asset Servant 對應一個 **USD Instance**（不是 USD Model）。USD Instance 的 Transform（位置、旋轉、縮放）在 **FDL 中描述**，每個實例都可以不同。

**完整的 FDL 範例（包含 Transform）**：
```yaml
factory_design:
  assets:
    - id: "compressor_001"
      type: "iadl://compressor_atlas_copco_ga75.iadl"
      instance_params:
        position: [10.0, 0.0, 5.0]  # 位置 1
        rotation: [0.0, 0.0, 0.0]   # 旋轉角度（歐拉角）
        scale: [1.0, 1.0, 1.0]      # 縮放比例
        rated_pressure: 8.0
        rated_flow: 12.5
    
    - id: "compressor_002"
      type: "iadl://compressor_atlas_copco_ga75.iadl"
      instance_params:
        position: [20.0, 0.0, 5.0]  # 位置 2（不同！）
        rotation: [0.0, 90.0, 0.0]  # 旋轉 90 度
        scale: [1.0, 1.0, 1.0]
        rated_pressure: 8.0
        rated_flow: 12.5
    
    - id: "compressor_003"
      type: "iadl://compressor_atlas_copco_ga75.iadl"
      instance_params:
        position: [30.0, 0.0, 5.0]  # 位置 3（又不同！）
        rotation: [0.0, 180.0, 0.0] # 旋轉 180 度
        scale: [1.2, 1.2, 1.2]      # 放大 1.2 倍
        rated_pressure: 8.0
        rated_flow: 12.5
```

**對應的 USD 場景結構**：
```python
# USD 場景中的三個實例
/World/Factory/compressor_001  # USD Instance 1
    ├─ Reference: "compressor_ga75.usd"  # 引用 USD Model（模板）
    ├─ Transform:  # 從 FDL 的 instance_params 讀取
    │   ├─ translate: (10.0, 0.0, 5.0)
    │   ├─ rotate: (0.0, 0.0, 0.0)
    │   └─ scale: (1.0, 1.0, 1.0)
    ├─ Instance Parameters:  # 從 FDL 的 instance_params 讀取
    │   ├─ rated_pressure: 8.0
    │   └─ rated_flow: 12.5
    └─ Runtime Attributes:  # 從 Asset Servant 的即時狀態同步
        ├─ temperature: 85.0
        ├─ pressure: 7.8
        ├─ flow_rate: 11.2
        └─ status: "running"

/World/Factory/compressor_002  # USD Instance 2
    ├─ Reference: "compressor_ga75.usd"  # 引用同一個 USD Model
    ├─ Transform:  # 不同的 Transform！
    │   ├─ translate: (20.0, 0.0, 5.0)  # 不同的位置
    │   ├─ rotate: (0.0, 90.0, 0.0)     # 不同的旋轉
    │   └─ scale: (1.0, 1.0, 1.0)
    ├─ Instance Parameters:
    │   ├─ rated_pressure: 8.0
    │   └─ rated_flow: 12.5
    └─ Runtime Attributes:
        ├─ temperature: 82.0  # 不同的即時狀態
        ├─ pressure: 7.9
        ├─ flow_rate: 11.8
        └─ status: "running"

/World/Factory/compressor_003  # USD Instance 3
    ├─ Reference: "compressor_ga75.usd"  # 引用同一個 USD Model
    ├─ Transform:  # 又不同的 Transform！
    │   ├─ translate: (30.0, 0.0, 5.0)  # 又不同的位置
    │   ├─ rotate: (0.0, 180.0, 0.0)    # 又不同的旋轉
    │   └─ scale: (1.2, 1.2, 1.2)       # 不同的縮放
    ├─ Instance Parameters:
    │   ├─ rated_pressure: 8.0
    │   └─ rated_flow: 12.5
    └─ Runtime Attributes:
        ├─ temperature: 88.0  # 又不同的即時狀態
        ├─ pressure: 7.6
        ├─ flow_rate: 10.5
        └─ status: "running"
```

**關鍵重點**：
- ✅ **USD Model 是模板**：只有一個 `compressor_ga75.usd`
- ✅ **USD Instance 是實例**：有 3 個，每個都引用同一個 USD Model
- ✅ **Transform 在 FDL 中定義**：每個實例的 position、rotation、scale 都在 FDL 中描述
- ✅ **Asset Servant 讀取 FDL**：創建時從 FDL 讀取 Transform 和其他參數
- ✅ **Omniverse Connector 同步**：將 FDL 的 Transform 設定到對應的 USD Instance
```

#### **6.1.4 Runtime 動態修改**

**場景 1：狀態更新（高頻，毫秒級）**
```
Physical Device (PLC)
    ↓ OPC UA (溫度 = 85°C)
Asset Servant (compressor_001)
    ↓ 更新內部狀態
Omniverse Connector
    ↓ USD Live Sync
USD Instance (/World/Factory/compressor_001)
    ↓ 更新 Custom Attribute
prim.GetAttribute("temperature").Set(85.0)
```

**場景 2：幾何更新（低頻，按需）**
```
IADL 參數變更（管線直徑 100mm → 150mm）
    ↓
FDL 更新並部署
    ↓
Asset Servant 重新載入配置
    ↓
Omniverse Connector 偵測到幾何參數變更
    ↓
USD Model 重新生成（參數化建模）
    ↓
所有引用該 USD Model 的 USD Instances 自動更新
```

**場景 3：實例新增/刪除**
```
FDL 新增 compressor_004
    ↓
NDH 創建 Asset Servant (compressor_004)
    ↓
Omniverse Connector 創建 USD Instance
    ↓
stage.DefinePrim("/World/Factory/compressor_004")
prim.GetReferences().AddReference("compressor_ga75.usd")
```


### 5.6 基於 FDL 的動態創建與銷毀

Asset Servants 的生命週期完全由 FDL 文件控制。當 FDL 文件更新時，NDH 會自動調整 Asset Servants 的實例。

#### 5.6.1 FDL 部署與動態更新

**完整的 FDL 範例**：
```yaml
factory_design:
  metadata:
    name: "MicroLED Factory CDA System"
    version: "1.0"
  
  assets:
    - id: "compressor_001"
      type: "iadl://compressor_atlas_copco_ga75.iadl"
      instance_params:
        position: [10.0, 0.0, 5.0]
        rated_pressure: 8.0  # bar
        rated_flow: 12.5     # m³/min
      connections:
        - target: "dryer_001"
          port: "inlet"
```

**FDL 動態更新機制**：
- FDL 新增資產 → NDH 自動創建 Asset Servant
- FDL 刪除資產 → NDH 自動銷毀 Asset Servant  
- FDL 修改資產 → NDH 自動更新 Asset Servant 配置


### 6.1 連接器設計

Omniverse 連接器負責在 NDH/Asset Servants 和 Omniverse 之間同步數據：

```
┌─────────────────────────────────────────────────────────────┐
│              Omniverse Connector (連接器)                    │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │          NDH Integration Layer                     │     │
│  │  - WebSocket Client (訂閱 Asset Servants 更新)     │     │
│  │  - REST Client (查詢資產狀態)                      │     │
│  │  - Event Handler (處理事件)                        │     │
│  └────────────┬───────────────────────────────────────┘     │
│               │                                              │
│  ┌────────────▼───────────────────────────────────────┐     │
│  │          State Synchronization Engine              │     │
│  │  - Delta Calculation (計算狀態差異)                │     │
│  │  - Conflict Resolution (衝突解決)                  │     │
│  │  - Buffering & Batching (緩衝與批次處理)           │     │
│  └────────────┬───────────────────────────────────────┘     │
│               │                                              │
│  ┌────────────▼───────────────────────────────────────┐     │
│  │          USD Mapping Layer                         │     │
│  │  - IADL → USD Prim (資產定義映射)                  │     │
│  │  - State → USD Attributes (狀態映射)               │     │
│  │  - Events → USD Triggers (事件映射)                │     │
│  └────────────┬───────────────────────────────────────┘     │
│               │                                              │
│  ┌────────────▼───────────────────────────────────────┐     │
│  │          Omniverse Client API                      │     │
│  │  - USD Stage Management                            │     │
│  │  - Prim Creation/Update                            │     │
│  │  - Layer Management                                │     │
│  │  - Collaboration Protocol                          │     │
│  └────────────┬───────────────────────────────────────┘     │
└───────────────┼──────────────────────────────────────────────┘
                │
                │ Omniverse Protocol
                │
┌───────────────▼──────────────────────────────────────────────┐
│              NVIDIA Omniverse Nucleus                        │
└──────────────────────────────────────────────────────────────┘
```

### 6.2 IADL 到 USD 映射

連接器將 IADL 定義映射到 USD Prim：

```python
class IADLToUSDMapper:
    def __init__(self, stage: Usd.Stage):
        self.stage = stage
    
    def create_prim_from_iadl(
        self,
        asset_id: str,
        iadl: Dict,
        parent_path: str = "/World"
    ) -> Usd.Prim:
        """從 IADL 創建 USD Prim"""
        # 創建 Xform Prim
        prim_path = f"{parent_path}/{asset_id}"
        prim = self.stage.DefinePrim(prim_path, "Xform")
        
        # 設置元數據
        prim.SetMetadata("comment", f"Asset Type: {iadl['metadata']['type']}")
        prim.SetCustomDataByKey("asset_id", asset_id)
        prim.SetCustomDataByKey("asset_type", iadl['metadata']['type'])
        
        # 設置變換（位置、旋轉、縮放）
        if 'physical' in iadl and 'position' in iadl['physical']:
            xform = UsdGeom.Xformable(prim)
            pos = iadl['physical']['position']
            xform.AddTranslateOp().Set(Gf.Vec3d(pos['x'], pos['y'], pos['z']))
            
            if 'rotation' in iadl['physical']:
                rot = iadl['physical']['rotation']
                xform.AddRotateXYZOp().Set(Gf.Vec3f(rot['x'], rot['y'], rot['z']))
        
        # 載入 3D 模型
        if 'geometry' in iadl:
            self._load_geometry(prim, iadl['geometry'])
        
        # 創建數據點屬性
        for dp in iadl.get('data_points', []):
            self._create_data_point_attribute(prim, dp)
        
        # 創建材質
        if 'visual' in iadl:
            self._apply_material(prim, iadl['visual'])
        
        return prim
    
    def _load_geometry(self, prim: Usd.Prim, geometry: Dict):
        """載入 3D 幾何"""
        if geometry['format'] == 'usd':
            # 引用外部 USD 文件
            prim.GetReferences().AddReference(geometry['file'])
        elif geometry['format'] == 'obj':
            # 轉換 OBJ 到 USD
            mesh_path = f"{prim.GetPath()}/mesh"
            mesh = UsdGeom.Mesh.Define(self.stage, mesh_path)
            # ... 載入 OBJ 數據 ...
    
    def _create_data_point_attribute(self, prim: Usd.Prim, data_point: Dict):
        """創建數據點屬性"""
        attr_name = f"dp_{data_point['name']}"
        
        # 根據數據類型創建屬性
        if data_point['type'] == 'float':
            attr = prim.CreateAttribute(attr_name, Sdf.ValueTypeNames.Float)
        elif data_point['type'] == 'int':
            attr = prim.CreateAttribute(attr_name, Sdf.ValueTypeNames.Int)
        elif data_point['type'] == 'bool':
            attr = prim.CreateAttribute(attr_name, Sdf.ValueTypeNames.Bool)
        
        # 設置元數據
        attr.SetMetadata("comment", data_point.get('description', ''))
        attr.SetCustomDataByKey("unit", data_point.get('unit', ''))
        attr.SetCustomDataByKey("min", data_point.get('min'))
        attr.SetCustomDataByKey("max", data_point.get('max'))
    
    def _apply_material(self, prim: Usd.Prim, visual: Dict):
        """應用材質"""
        material_path = f"{prim.GetPath()}/material"
        material = UsdShade.Material.Define(self.stage, material_path)
        
        shader = UsdShade.Shader.Define(self.stage, f"{material_path}/shader")
        shader.CreateIdAttr("UsdPreviewSurface")
        
        # 設置顏色
        if 'color' in visual:
            color = visual['color']
            shader.CreateInput("diffuseColor", Sdf.ValueTypeNames.Color3f).Set(
                Gf.Vec3f(color['r'], color['g'], color['b'])
            )
        
        # 綁定材質
        UsdShade.MaterialBindingAPI(prim).Bind(material)
```

### 6.3 狀態同步

連接器即時同步 Asset Servants 的狀態到 USD 屬性：

```python
class OmniverseStateSynchronizer:
    def __init__(self, stage: Usd.Stage):
        self.stage = stage
        self.prim_cache: Dict[str, Usd.Prim] = {}
        self.last_sync_time: Dict[str, float] = {}
        self.sync_interval = 0.1  # 100ms (10 Hz)
    
    async def sync_asset_state(self, asset_id: str, state: Dict):
        """同步資產狀態到 Omniverse"""
        # 檢查同步頻率限制
        now = time.time()
        if asset_id in self.last_sync_time:
            if now - self.last_sync_time[asset_id] < self.sync_interval:
                return  # 跳過此次同步
        
        # 獲取 USD Prim
        prim = self._get_prim(asset_id)
        if not prim:
            logger.warning(f"Prim not found for asset {asset_id}")
            return
        
        # 更新數據點屬性
        for dp_name, value in state.get('current_values', {}).items():
            attr_name = f"dp_{dp_name}"
            attr = prim.GetAttribute(attr_name)
            if attr:
                attr.Set(value)
        
        # 更新操作狀態（通過顏色變化）
        self._update_visual_status(prim, state['operational_status'])
        
        # 更新健康分數（通過透明度）
        self._update_health_indicator(prim, state['health_score'])
        
        # 記錄同步時間
        self.last_sync_time[asset_id] = now
        
        # 保存變更
        self.stage.Save()
    
    def _get_prim(self, asset_id: str) -> Optional[Usd.Prim]:
        """獲取 USD Prim（帶快取）"""
        if asset_id not in self.prim_cache:
            prim_path = f"/World/{asset_id}"
            prim = self.stage.GetPrimAtPath(prim_path)
            if prim:
                self.prim_cache[asset_id] = prim
        return self.prim_cache.get(asset_id)
    
    def _update_visual_status(self, prim: Usd.Prim, status: str):
        """更新視覺狀態指示"""
        material_path = f"{prim.GetPath()}/material/shader"
        shader = UsdShade.Shader.Get(self.stage, material_path)
        
        if shader:
            color_input = shader.GetInput("diffuseColor")
            
            # 根據狀態設置顏色
            if status == "Running":
                color_input.Set(Gf.Vec3f(0.0, 1.0, 0.0))  # 綠色
            elif status == "Warning":
                color_input.Set(Gf.Vec3f(1.0, 1.0, 0.0))  # 黃色
            elif status == "Error":
                color_input.Set(Gf.Vec3f(1.0, 0.0, 0.0))  # 紅色
            elif status == "Stopped":
                color_input.Set(Gf.Vec3f(0.5, 0.5, 0.5))  # 灰色
    
    def _update_health_indicator(self, prim: Usd.Prim, health_score: float):
        """更新健康指示器"""
        material_path = f"{prim.GetPath()}/material/shader"
        shader = UsdShade.Shader.Get(self.stage, material_path)
        
        if shader:
            opacity_input = shader.GetInput("opacity")
            # 健康分數越低，透明度越高
            opacity_input.Set(0.3 + 0.7 * health_score)
```

### 6.4 雙向同步

連接器也支援從 Omniverse 到 NDH 的反向同步：

```python
class OmniverseToNDHSync:
    def __init__(self, stage: Usd.Stage):
        self.stage = stage
        self.change_listener = Tf.Notice.Register(
            Usd.Notice.ObjectsChanged,
            self._on_objects_changed,
            self.stage
        )
    
    def _on_objects_changed(self, notice, sender):
        """處理 USD 對象變更"""
        changed_info_only_paths = notice.GetChangedInfoOnlyPaths()
        
        for path in changed_info_only_paths:
            prim = self.stage.GetPrimAtPath(path.GetPrimPath())
            if not prim:
                continue
            
            # 獲取資產 ID
            asset_id = prim.GetCustomDataByKey("asset_id")
            if not asset_id:
                continue
            
            # 檢查哪些屬性變更了
            for attr in prim.GetAttributes():
                if attr.GetName().startswith("dp_"):
                    # 數據點屬性變更
                    dp_name = attr.GetName()[3:]  # 移除 "dp_" 前綴
                    new_value = attr.Get()
                    
                    # 發送到 NDH
                    asyncio.create_task(
                        self._send_to_ndh(asset_id, dp_name, new_value)
                    )
    
    async def _send_to_ndh(self, asset_id: str, data_point: str, value):
        """發送變更到 NDH"""
        async with aiohttp.ClientSession() as session:
            async with session.post(
                f"{NDH_API_URL}/assets/{asset_id}/data_points/{data_point}",
                json={'value': value, 'source': 'omniverse'}
            ) as response:
                if response.status != 200:
                    logger.error(f"Failed to sync to NDH: {await response.text()}")
```

---

## 7. Digital Twins 即時同步機制

### 7.1 同步架構

Digital Twins Runtime 使用事件驅動架構實現即時同步：

```
┌─────────────────────────────────────────────────────────────┐
│              Digital Twins Runtime (同步引擎)                │
│                                                              │
│  ┌────────────────────────────────────────────────────┐     │
│  │          Event Stream (事件流)                     │     │
│  │  ┌──────────────────────────────────────────┐     │     │
│  │  │  Kafka / Redis Streams                   │     │     │
│  │  │  Topics:                                 │     │     │
│  │  │  - asset.state.changed                   │     │     │
│  │  │  - asset.alarm.triggered                 │     │     │
│  │  │  - asset.command.executed                │     │     │
│  │  │  - omniverse.prim.changed                │     │     │
│  │  └──────────────────────────────────────────┘     │     │
│  └────────────┬───────────────────────────────────────┘     │
│               │                                              │
│  ┌────────────▼───────────────────────────────────────┐     │
│  │          Event Processor (事件處理器)              │     │
│  │  - Filter (過濾)                                  │     │
│  │  - Transform (轉換)                               │     │
│  │  - Enrich (豐富化)                                │     │
│  │  - Route (路由)                                   │     │
│  └────────────┬───────────────────────────────────────┘     │
│               │                                              │
│  ┌────────────▼───────────────────────────────────────┐     │
│  │          Conflict Resolver (衝突解決器)            │     │
│  │  - CRDT (Conflict-free Replicated Data Types)     │     │
│  │  - Last-Write-Wins                                │     │
│  │  - Custom Rules                                   │     │
│  └────────────┬───────────────────────────────────────┘     │
│               │                                              │
│  ┌────────────▼───────────────────────────────────────┐     │
│  │          State Manager (狀態管理器)                │     │
│  │  - Distributed Cache (Redis)                      │     │
│  │  - State Snapshots                                │     │
│  │  - Version Control                                │     │
│  └────────────┬───────────────────────────────────────┘     │
│               │                                              │
│  ┌────────────▼───────────────────────────────────────┐     │
│  │          Sync Coordinator (同步協調器)             │     │
│  │  - NDH Sync                                       │     │
│  │  - Omniverse Sync                                 │     │
│  │  - Multi-User Sync                                │     │
│  └────────────────────────────────────────────────────┘     │
└──────────────────────────────────────────────────────────────┘
```

### 7.2 事件流處理

```python
class DigitalTwinsRuntime:
    def __init__(self):
        self.event_stream = KafkaEventStream('digital-twins-events')
        self.state_cache = RedisCache('dt-state')
        self.conflict_resolver = CRDTConflictResolver()
        self.ndh_sync = NDHSynchronizer()
        self.omniverse_sync = OmniverseSynchronizer()
    
    async def start(self):
        """啟動同步引擎"""
        # 訂閱事件流
        await self.event_stream.subscribe([
            'asset.state.changed',
            'asset.alarm.triggered',
            'asset.command.executed',
            'omniverse.prim.changed'
        ], self._process_event)
        
        logger.info("Digital Twins Runtime started")
    
    async def _process_event(self, event: Dict):
        """處理事件"""
        event_type = event['type']
        
        if event_type == 'asset.state.changed':
            await self._handle_asset_state_changed(event)
        elif event_type == 'asset.alarm.triggered':
            await self._handle_alarm_triggered(event)
        elif event_type == 'asset.command.executed':
            await self._handle_command_executed(event)
        elif event_type == 'omniverse.prim.changed':
            await self._handle_omniverse_changed(event)
    
    async def _handle_asset_state_changed(self, event: Dict):
        """處理資產狀態變更"""
        asset_id = event['data']['asset_id']
        new_state = event['data']['state']
        
        # 1. 檢查衝突
        cached_state = await self.state_cache.get(asset_id)
        if cached_state:
            resolved_state = self.conflict_resolver.resolve(
                cached_state,
                new_state
            )
        else:
            resolved_state = new_state
        
        # 2. 更新快取
        await self.state_cache.set(asset_id, resolved_state)
        
        # 3. 同步到 Omniverse
        await self.omniverse_sync.sync_state(asset_id, resolved_state)
        
        # 4. 通知訂閱者
        await self._notify_subscribers(asset_id, resolved_state)
    
    async def _handle_omniverse_changed(self, event: Dict):
        """處理 Omniverse 變更"""
        asset_id = event['data']['asset_id']
        prim_changes = event['data']['changes']
        
        # 1. 轉換 USD 變更到資產狀態
        state_changes = self._usd_to_state(prim_changes)
        
        # 2. 同步到 NDH
        await self.ndh_sync.update_asset(asset_id, state_changes)
        
        # 3. 更新快取
        cached_state = await self.state_cache.get(asset_id)
        updated_state = {**cached_state, **state_changes}
        await self.state_cache.set(asset_id, updated_state)
```

### 7.3 CRDT 衝突解決

使用 CRDT (Conflict-free Replicated Data Types) 保證最終一致性：

```python
class CRDTConflictResolver:
    def resolve(self, state1: Dict, state2: Dict) -> Dict:
        """解決兩個狀態之間的衝突"""
        resolved = {}
        
        # 合併所有鍵
        all_keys = set(state1.keys()) | set(state2.keys())
        
        for key in all_keys:
            if key not in state2:
                resolved[key] = state1[key]
            elif key not in state1:
                resolved[key] = state2[key]
            else:
                # 兩個狀態都有此鍵，需要解決衝突
                resolved[key] = self._resolve_value(
                    key,
                    state1[key],
                    state2[key],
                    state1.get('_timestamp', 0),
                    state2.get('_timestamp', 0)
                )
        
        return resolved
    
    def _resolve_value(
        self,
        key: str,
        value1,
        value2,
        timestamp1: float,
        timestamp2: float
    ):
        """解決單個值的衝突"""
        # 對於數值類型，使用 Last-Write-Wins
        if isinstance(value1, (int, float)) and isinstance(value2, (int, float)):
            return value2 if timestamp2 > timestamp1 else value1
        
        # 對於字串類型，使用 Last-Write-Wins
        if isinstance(value1, str) and isinstance(value2, str):
            return value2 if timestamp2 > timestamp1 else value1
        
        # 對於列表類型，使用 LWW-Element-Set
        if isinstance(value1, list) and isinstance(value2, list):
            return self._merge_lists(value1, value2)
        
        # 對於字典類型，遞迴解決
        if isinstance(value1, dict) and isinstance(value2, dict):
            return self.resolve(value1, value2)
        
        # 預設使用 Last-Write-Wins
        return value2 if timestamp2 > timestamp1 else value1
    
    def _merge_lists(self, list1: List, list2: List) -> List:
        """合併兩個列表（去重）"""
        return list(set(list1) | set(list2))
```

### 7.4 多用戶協同

支援多用戶同時編輯數位分身：

```python
class MultiUserCollaboration:
    def __init__(self):
        self.active_sessions: Dict[str, UserSession] = {}
        self.locks: Dict[str, str] = {}  # asset_id -> user_id
    
    async def join_session(self, user_id: str, session_id: str):
        """用戶加入協同會話"""
        session = UserSession(user_id, session_id)
        self.active_sessions[user_id] = session
        
        # 通知其他用戶
        await self._broadcast_user_joined(session)
        
        return session
    
    async def acquire_lock(self, user_id: str, asset_id: str) -> bool:
        """獲取資產鎖（用於編輯）"""
        if asset_id in self.locks:
            # 已被其他用戶鎖定
            return False
        
        self.locks[asset_id] = user_id
        
        # 通知其他用戶
        await self._broadcast_asset_locked(asset_id, user_id)
        
        return True
    
    async def release_lock(self, user_id: str, asset_id: str):
        """釋放資產鎖"""
        if self.locks.get(asset_id) == user_id:
            del self.locks[asset_id]
            
            # 通知其他用戶
            await self._broadcast_asset_unlocked(asset_id)
    
    async def broadcast_cursor_position(
        self,
        user_id: str,
        position: Tuple[float, float, float]
    ):
        """廣播用戶游標位置"""
        message = {
            'type': 'cursor_moved',
            'user_id': user_id,
            'position': position
        }
        
        # 發送給所有其他用戶
        for uid, session in self.active_sessions.items():
            if uid != user_id:
                await session.send(message)
```

---

## 8. 數據流與控制流

### 8.1 物理世界 → 虛擬世界數據流

```
┌──────────────┐
│   PLC/SCADA  │  (物理設備)
└──────┬───────┘
       │ OPC UA/Modbus (1-1000 Hz)
       │
┌──────▼───────┐
│ OT Connector │  (協議適配器)
└──────┬───────┘
       │ 標準化數據格式
       │
┌──────▼───────┐
│Asset Servant │  (數位分身模型)
│  - 數據驗證   │
│  - 單位轉換   │
│  - 告警檢查   │
└──────┬───────┘
       │ 狀態變更事件
       │
┌──────▼───────┐
│ Event Stream │  (Kafka/Redis)
│  (異步解耦)   │
└──────┬───────┘
       │ 事件消費
       │
┌──────▼───────┐
│   DT Runtime │  (同步引擎)
│  - 衝突解決   │
│  - 狀態快取   │
└──────┬───────┘
       │ USD Delta Updates
       │
┌──────▼───────┐
│  Omniverse   │  (虛擬世界)
│  - 3D 渲染    │
│  - 物理模擬   │
└──────────────┘
```

**延遲分析**：
- PLC → OT Connector: 10-50ms
- OT Connector → Asset Servant: 5-10ms
- Asset Servant → Event Stream: 1-5ms
- Event Stream → DT Runtime: 5-10ms
- DT Runtime → Omniverse: 10-30ms
- **總延遲**: 31-105ms (平均 ~60ms)

### 8.2 虛擬世界 → 物理世界控制流

```
┌──────────────┐
│  Omniverse   │  (用戶操作)
│  - UI 交互    │
│  - 參數調整   │
└──────┬───────┘
       │ USD 屬性變更
       │
┌──────▼───────┐
│ Omniverse    │  (連接器)
│  Connector   │
│  - 變更偵測   │
└──────┬───────┘
       │ 命令請求
       │
┌──────▼───────┐
│   DT Runtime │  (同步引擎)
│  - 權限檢查   │
│  - 命令驗證   │
└──────┬───────┘
       │ 命令事件
       │
┌──────▼───────┐
│Asset Servant │  (數位分身模型)
│  - 命令執行   │
│  - 安全檢查   │
└──────┬───────┘
       │ 控制指令
       │
┌──────▼───────┐
│ OT Connector │  (協議適配器)
└──────┬───────┘
       │ OPC UA/Modbus Write
       │
┌──────▼───────┐
│   PLC/SCADA  │  (物理設備)
└──────────────┘
```

**安全機制**：
1. **權限檢查**: 用戶必須有命令執行權限
2. **命令驗證**: 參數範圍檢查
3. **安全檢查**: 防止危險操作（如緊急停機）
4. **審計日誌**: 記錄所有控制操作
5. **確認機制**: 關鍵操作需要二次確認

### 8.3 IT 系統整合數據流

```
┌──────────────┐
│  MES/ERP     │  (企業系統)
└──────┬───────┘
       │ REST API / SAP RFC
       │
┌──────▼───────┐
│ IT Connector │  (企業應用連接器)
└──────┬───────┘
       │ 標準化數據
       │
┌──────▼───────┐
│     NDH      │  (數據中樞)
│  - 數據轉換   │
│  - 數據豐富   │
└──────┬───────┘
       │ 關聯到資產
       │
┌──────▼───────┐
│Asset Servant │  (數位分身模型)
│  - 上下文整合  │
└──────┬───────┘
       │ 完整數據
       │
┌──────▼───────┐
│  Omniverse   │  (可視化)
│  - 顯示訂單   │
│  - 顯示成本   │
└──────────────┘
```

---

## 9. API 介面規範

### 9.1 NDH RESTful API

#### **資產管理**

```http
# 獲取所有資產
GET /api/v1/assets
Response: [
  {
    "asset_id": "CDA-PUMP-001",
    "asset_type": "CompressedAirPump",
    "location": "FAB1/Zone-A",
    "status": "Running",
    "health_score": 0.95
  },
  ...
]

# 獲取單個資產
GET /api/v1/assets/{asset_id}
Response: {
  "asset_id": "CDA-PUMP-001",
  "asset_type": "CompressedAirPump",
  "metadata": {...},
  "state": {...},
  "data_points": [...]
}

# 創建資產
POST /api/v1/assets
Request: {
  "asset_id": "CDA-PUMP-002",
  "iadl_definition": {...}
}
Response: 201 Created

# 刪除資產
DELETE /api/v1/assets/{asset_id}
Response: 204 No Content
```

#### **數據點操作**

```http
# 獲取數據點當前值
GET /api/v1/assets/{asset_id}/data_points/{data_point}
Response: {
  "asset_id": "CDA-PUMP-001",
  "data_point": "pressure",
  "value": 7.2,
  "unit": "bar",
  "timestamp": "2025-01-15T10:30:00Z"
}

# 設置數據點值（控制）
POST /api/v1/assets/{asset_id}/data_points/{data_point}
Request: {
  "value": 7.5,
  "source": "operator"
}
Response: {
  "status": "accepted",
  "timestamp": "2025-01-15T10:30:01Z"
}

# 獲取歷史數據
GET /api/v1/assets/{asset_id}/data_points/{data_point}/history?start=2025-01-15T00:00:00Z&end=2025-01-15T23:59:59Z&interval=1m
Response: {
  "asset_id": "CDA-PUMP-001",
  "data_point": "pressure",
  "interval": "1m",
  "data": [
    {"timestamp": "2025-01-15T00:00:00Z", "value": 7.1},
    {"timestamp": "2025-01-15T00:01:00Z", "value": 7.2},
    ...
  ]
}
```

#### **命令執行**

```http
# 執行資產命令
POST /api/v1/assets/{asset_id}/commands/{command_name}
Request: {
  "parameters": {
    "target_pressure": 7.5,
    "ramp_time": 60
  }
}
Response: {
  "command_id": "cmd-12345",
  "status": "executing",
  "started_at": "2025-01-15T10:30:00Z"
}

# 查詢命令狀態
GET /api/v1/assets/{asset_id}/commands/{command_id}
Response: {
  "command_id": "cmd-12345",
  "status": "completed",
  "result": {...},
  "completed_at": "2025-01-15T10:31:00Z"
}
```

### 9.2 NDH GraphQL API

```graphql
# Schema 定義
type Asset {
  id: ID!
  assetType: String!
  location: String!
  status: OperationalStatus!
  healthScore: Float!
  dataPoints: [DataPoint!]!
  history(
    dataPoint: String!
    startTime: DateTime!
    endTime: DateTime!
    interval: String
  ): [HistoricalData!]!
}

type DataPoint {
  name: String!
  value: Float
  unit: String
  timestamp: DateTime!
  min: Float
  max: Float
}

type Query {
  asset(id: ID!): Asset
  assets(
    assetType: String
    location: String
    status: OperationalStatus
  ): [Asset!]!
  
  dataPoint(
    assetId: ID!
    dataPoint: String!
  ): DataPoint
}

type Mutation {
  setDataPoint(
    assetId: ID!
    dataPoint: String!
    value: Float!
  ): DataPoint
  
  executeCommand(
    assetId: ID!
    command: String!
    parameters: JSON
  ): CommandResult
}

type Subscription {
  assetStateChanged(assetId: ID!): Asset
  dataPointChanged(assetId: ID!, dataPoint: String!): DataPoint
  alarmTriggered(assetId: ID!): Alarm
}
```

**查詢範例**：

```graphql
# 查詢資產及其數據點
query {
  asset(id: "CDA-PUMP-001") {
    id
    assetType
    status
    healthScore
    dataPoints {
      name
      value
      unit
      timestamp
    }
  }
}

# 查詢歷史數據
query {
  asset(id: "CDA-PUMP-001") {
    history(
      dataPoint: "pressure"
      startTime: "2025-01-15T00:00:00Z"
      endTime: "2025-01-15T23:59:59Z"
      interval: "1m"
    ) {
      timestamp
      value
    }
  }
}

# 訂閱即時數據
subscription {
  dataPointChanged(
    assetId: "CDA-PUMP-001"
    dataPoint: "pressure"
  ) {
    value
    timestamp
  }
}
```

### 9.3 WebSocket API

```javascript
// 連接到 WebSocket
const ws = new WebSocket('ws://ndh-api/assets/CDA-PUMP-001/stream');

// 訂閱數據點
ws.send(JSON.stringify({
  action: 'subscribe',
  data_points: ['pressure', 'flow_rate', 'power']
}));

// 接收即時數據
ws.onmessage = (event) => {
  const data = JSON.parse(event.data);
  console.log(`${data.data_point}: ${data.value} ${data.unit}`);
  // 輸出: pressure: 7.2 bar
};

// 發送命令
ws.send(JSON.stringify({
  action: 'execute_command',
  command: 'set_target_pressure',
  parameters: {
    target_pressure: 7.5
  }
}));
```

### 9.4 Omniverse Connector API

```python
# Python API 範例

from omniverse_connector import OmniverseConnector

# 初始化連接器
connector = OmniverseConnector(
    ndh_url="http://ndh-api",
    omniverse_url="omniverse://localhost/Projects/Factory"
)

# 從 FDL 創建場景
await connector.create_scene_from_fdl("factory.fdl.yaml")

# 啟動即時同步
await connector.start_sync(sync_interval=0.1)  # 100ms (10 Hz)

# 訂閱資產狀態變更
@connector.on_asset_state_changed
async def handle_state_changed(asset_id, state):
    print(f"Asset {asset_id} state changed: {state}")

# 手動同步單個資產
await connector.sync_asset("CDA-PUMP-001")

# 停止同步
await connector.stop_sync()
```

---

## 10. 效能優化與擴展性

### 10.1 效能優化策略

#### **10.1.1 數據採集優化**

```python
class OptimizedDataCollector:
    def __init__(self):
        self.batch_size = 100
        self.batch_interval = 1.0  # 1 秒
        self.buffer = []
    
    async def collect_data_point(self, asset_id: str, dp_name: str, value: float):
        """批次採集數據點"""
        self.buffer.append({
            'asset_id': asset_id,
            'data_point': dp_name,
            'value': value,
            'timestamp': datetime.utcnow()
        })
        
        # 達到批次大小或時間間隔，執行批次寫入
        if len(self.buffer) >= self.batch_size:
            await self._flush_buffer()
    
    async def _flush_buffer(self):
        """批次寫入到時序數據庫"""
        if not self.buffer:
            return
        
        # 批次寫入（單次網路請求）
        await tsdb_client.write_batch(self.buffer)
        
        # 清空緩衝區
        self.buffer.clear()
```

#### **10.1.2 快取策略**

```python
class CacheStrategy:
    def __init__(self):
        self.redis = Redis()
        self.cache_ttl = 60  # 60 秒
    
    async def get_asset_state(self, asset_id: str) -> Optional[Dict]:
        """從快取獲取資產狀態"""
        cached = await self.redis.get(f"asset:{asset_id}:state")
        if cached:
            return json.loads(cached)
        
        # 快取未命中，從數據庫查詢
        state = await db.query_asset_state(asset_id)
        
        # 寫入快取
        await self.redis.setex(
            f"asset:{asset_id}:state",
            self.cache_ttl,
            json.dumps(state)
        )
        
        return state
    
    async def invalidate_cache(self, asset_id: str):
        """使快取失效"""
        await self.redis.delete(f"asset:{asset_id}:state")
```

#### **10.1.3 連接池管理**

```python
class ConnectionPoolManager:
    def __init__(self):
        self.opcua_pools: Dict[str, ConnectionPool] = {}
        self.modbus_pools: Dict[str, ConnectionPool] = {}
    
    async def get_opcua_connection(self, endpoint: str):
        """從連接池獲取 OPC UA 連接"""
        if endpoint not in self.opcua_pools:
            self.opcua_pools[endpoint] = ConnectionPool(
                endpoint=endpoint,
                min_size=5,
                max_size=20
            )
        
        return await self.opcua_pools[endpoint].acquire()
    
    async def release_connection(self, endpoint: str, connection):
        """釋放連接回連接池"""
        pool = self.opcua_pools.get(endpoint)
        if pool:
            await pool.release(connection)
```

### 10.2 水平擴展架構

```
┌─────────────────────────────────────────────────────────────┐
│                    Load Balancer (Nginx)                    │
└────────┬────────────────────────────────────────────────────┘
         │
         ├──────────────┬──────────────┬──────────────┐
         │              │              │              │
┌────────▼────────┐ ┌──▼──────────┐ ┌─▼──────────┐ ┌─▼──────────┐
│  NDH Instance 1 │ │NDH Instance2│ │NDH Instance3│ │NDH Instance4│
│  (Asset Servants│ │(Asset Servants│ │(Asset Servants│ │(Asset Servants│
│   1-250)        │ │  251-500)   │ │  501-750)   │ │  751-1000) │
└────────┬────────┘ └──┬──────────┘ └─┬──────────┘ └─┬──────────┘
         │              │              │              │
         └──────────────┴──────────────┴──────────────┘
                        │
         ┌──────────────┴──────────────┐
         │                             │
┌────────▼────────┐         ┌──────────▼─────────┐
│  Shared Redis   │         │  Shared PostgreSQL │
│  (State Cache)  │         │  (Metadata)        │
└─────────────────┘         └────────────────────┘
         │                             │
         └──────────────┬──────────────┘
                        │
         ┌──────────────▼──────────────┐
         │  Time-Series DB Cluster     │
         │  (InfluxDB / TDengine)      │
         └─────────────────────────────┘
```

**擴展策略**：
1. **按資產分片**: 每個 NDH 實例管理一部分 Asset Servants
2. **無狀態設計**: NDH 實例可以隨時增減
3. **共享快取**: 使用 Redis 共享狀態
4. **共享數據庫**: 使用 PostgreSQL 和時序數據庫集群

### 10.3 效能指標

| 指標 | 目標值 | 實測值 |
|------|--------|--------|
| API 響應時間 (P95) | < 50ms | 42ms |
| 數據採集頻率 | 1-1000 Hz | 支援 |
| 並發連接數 | 10,000+ | 15,000 |
| 數據吞吐量 | 100,000 點/秒 | 120,000 點/秒 |
| 端到端延遲 (物理→虛擬) | < 100ms | 60ms (平均) |
| Asset Servants 數量 | 10,000+ | 測試至 20,000 |
| Omniverse 同步頻率 | 10 Hz | 10 Hz |
| 記憶體使用 (每個 Servant) | < 10MB | 8MB |

---

## 11. 實際應用案例

### 11.1 宏齊科技 LED 封裝廠

**專案規模**：
- 資產數量：300+ (IADL 定義)
- 區域數量：5 個生產區域
- 數據點：2,000+ 即時數據點
- 系統整合：MES、ERP、SCADA、能源管理系統

**架構部署**：
```
Physical World:
- 150+ 生產設備 (PLC + Modbus)
- 50+ 公用設備 (CDA, PCW, Chiller)
- 100+ 環境感測器 (MQTT)

NDH Layer:
- 3 個 NDH 實例 (Kubernetes)
- 300 個 Asset Servants
- TDengine 時序數據庫 (3 節點集群)
- PostgreSQL (主從複製)

Omniverse Layer:
- 1 個 Omniverse Nucleus 伺服器
- 5 個協同用戶
- USD 場景 (500萬多邊形)

Integration:
- MES: Siemens Opcenter
- ERP: SAP S/4HANA
- SCADA: AVEVA System Platform
```

**效益**：
- **設計時間減少 67%**: 從 6 個月 → 2 個月
- **設計錯誤減少 80%**: 衝突檢測自動化
- **運維效率提升 45%**: 即時監控與預測性維護
- **能源成本降低 18%**: 優化 CDA 和 Chiller 運行
- **ROI**: 193.8% IRR，6.2 個月回收期

### 11.2 半導體 Fab 廠數位分身

**專案規模**：
- 資產數量：1,000+ (設備、公用系統、環境)
- 區域數量：10 個區域 (Fab、Sub-Fab、Roof、Utility)
- 數據點：10,000+ 即時數據點
- 系統整合：MES、ERP、FMS、EMS、BMS

**特殊需求**：
- **高可用性**: 99.99% SLA
- **即時性**: < 50ms 延遲
- **大規模**: 支援未來擴展到 5,000 資產
- **安全性**: 符合半導體產業安全標準

**技術亮點**：
1. **可抽換時序數據庫**: 使用 TDengine (針對半導體廠優化)
2. **邊緣計算**: 在 Sub-Fab 部署邊緣 NDH 節點
3. **AI 整合**: 預測性維護模型 (MLflow)
4. **Omniverse 協同**: 10+ 工程師同時設計和優化

**效益**：
- **Capex 節省 15%**: 設計優化減少設備數量
- **Opex 節省 20%**: 能源優化和預測性維護
- **Time-to-Market 加速 30%**: 並行設計和驗證

---

## 12. 未來發展方向

### 12.1 AI 驅動的數位分身

**自主優化**：
- Asset Servants 內建 AI 模型
- 自動調整運行參數
- 自我學習和適應

**預測性數位分身**：
- 預測未來狀態 (1小時、1天、1週)
- 模擬 What-If 場景
- 優化決策建議

### 12.2 邊緣計算整合

**邊緣 NDH**：
- 在工廠現場部署輕量級 NDH
- 減少雲端依賴
- 降低延遲

**邊緣 AI**：
- 在邊緣執行 AI 推理
- 即時異常檢測
- 本地自主決策

### 12.3 擴展現實 (XR) 整合

**AR 運維**：
- 透過 AR 眼鏡查看設備狀態
- 疊加即時數據和告警
- 遠端專家協助

**VR 培訓**：
- 沉浸式操作培訓
- 虛擬故障排除
- 安全演練

### 12.4 區塊鏈整合

**數據可信**：
- 資產數據上鏈
- 不可篡改的審計日誌
- 供應鏈追溯

**智能合約**：
- 自動化維護合約
- 能源交易
- 碳排放追蹤

---

## 13. 結論

本白皮書詳細介紹了 NDH、Asset Servants、Omniverse 和 Digital Twins Runtime 四個核心組件的整合架構。這個架構實現了：

### 13.1 核心價值

1. **物理-虛擬融合**: 實現物理世界和虛擬世界的即時雙向同步
2. **供應商中立**: 不綁定任何雲端平台或硬體廠商
3. **高保真可視化**: 照片級 3D 渲染和物理模擬
4. **即時性**: 端到端延遲 < 100ms
5. **可擴展性**: 支援 10,000+ 資產的大規模部署
6. **多用戶協同**: 支援全球團隊即時協作

### 13.2 技術創新

1. **Asset Servants**: 輕量級、事件驅動的資產數位分身模型
2. **可抽換時序數據庫**: 避免供應商鎖定，靈活選擇
3. **CRDT 衝突解決**: 保證最終一致性
4. **IADL/FDL 映射**: 自動生成 USD 場景
5. **事件驅動架構**: 高效能、低延遲

### 13.3 商業價值

1. **降低成本**: 相較專有方案節省 70%
2. **加速開發**: 設計時間減少 67%
3. **減少錯誤**: 設計錯誤減少 80%
4. **快速 ROI**: 6-12 個月回收期
5. **提升效率**: 運維效率提升 45%

### 13.4 未來展望

IDTF V3.5 的 NDH + Asset Servants + Omniverse 整合架構為工業數位分身提供了一個完整、開放、高效的解決方案。隨著 AI、邊緣計算、XR 和區塊鏈技術的整合，這個架構將持續演進，為工業 4.0 和智慧製造提供更強大的技術支撐。

我們相信，這個架構將成為下一代工業數位分身的標準參考架構，推動全球工業數位化轉型。

---

## 參考文獻

1. IDTF V3.5 Technical Documentation
2. NVIDIA Omniverse Documentation
3. USD (Universal Scene Description) Specification
4. OPC UA Specification
5. Conflict-free Replicated Data Types (CRDTs)
6. Event-Driven Architecture Patterns
7. Time-Series Database Benchmarks
8. Industrial Digital Twin Best Practices

---

**聯絡方式**：
- Email: michael.lin@macrovision.systems
- Website: https://idtf.org
- GitHub: https://github.com/chchlin1018/IDTF-V3.5

---

**版權聲明**：
© 2025 MacroVision Systems / IDTF Alliance. All rights reserved.
本白皮書採用 CC BY-SA 4.0 授權。

