# NDH 與 NVIDIA Omniverse 整合架構設計

## 執行摘要

本文件描述了如何將 **NDH (Neutral Data Hub)** 與 **NVIDIA Omniverse** 平台整合,打造一個即時運行、多人協作的工業數位分身 (Industrial Digital Twins) 平台。這個整合方案結合了 NDH 的工業資料聚合能力和 Omniverse 的 3D 視覺化與協作能力,為工業 4.0 提供了一個強大的解決方案。

**核心價值**:
- **即時資料同步**: 工業現場資料即時反映到 3D 數位分身中
- **多人協作**: 支援多個利害關係人同時查看和互動同一個數位分身
- **標準化**: 基於 IADL 和 OpenUSD 兩個開放標準
- **可擴展性**: 支援從單一設備到整個工廠的數位分身

---

## 1. 整合概述

### 1.1 整合目標

將 NDH 與 Omniverse 整合,實現以下目標:

1. **工業資料視覺化**: 將來自 PLC、SCADA、MES 等系統的即時資料視覺化呈現在 3D 數位分身中
2. **即時監控**: 設備狀態、生產數據、告警資訊等即時顯示
3. **歷史回放**: 利用 NDH 的事件溯源能力,回放歷史資料和事件
4. **多人協作**: 工程師、操作員、管理層可以同時查看和討論同一個數位分身
5. **模擬與預測**: 結合工業資料和物理模擬,進行預測性維護和優化

### 1.2 整合架構圖

```
┌─────────────────────────────────────────────────────────────────┐
│                    工業現場 (OT 層)                              │
│  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  │
│  │ PLC  │  │SCADA │  │OPC UA│  │Modbus│  │ PI   │  │Sparkplug│
│  └───┬──┘  └───┬──┘  └───┬──┘  └───┬──┘  └───┬──┘  └───┬──┘  │
└──────┼─────────┼─────────┼─────────┼─────────┼─────────┼──────┘
       │         │         │         │         │         │
       └─────────┴─────────┴─────────┴─────────┴─────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                    NDH 資料中樞層                                 │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │              NDH Broker (CORBA 啟發)                      │  │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐         │  │
│  │  │ Connector  │  │ Connector  │  │ Connector  │  ...    │  │
│  │  │   Pool     │  │   Pool     │  │   Pool     │         │  │
│  │  └─────┬──────┘  └─────┬──────┘  └─────┬──────┘         │  │
│  └────────┼────────────────┼────────────────┼────────────────┘  │
│           │                │                │                    │
│           ▼                ▼                ▼                    │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              IADL 資料標準化層                              │ │
│  │  (統一的工業資產資料語言)                                   │ │
│  └────────────────────────┬───────────────────────────────────┘ │
│                           │                                      │
│                           ▼                                      │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              Kafka 事件流核心                               │ │
│  │  Topics: asset.telemetry, asset.events, asset.commands     │ │
│  └────────────────────────┬───────────────────────────────────┘ │
└───────────────────────────┼──────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│              NDH-Omniverse 整合層 (新增)                         │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │         Omniverse Connector for NDH                       │  │
│  │  ┌────────────────┐  ┌────────────────┐                  │  │
│  │  │ Kafka Consumer │  │ IADL → OpenUSD │                  │  │
│  │  │   (訂閱事件)    │  │   Transformer  │                  │  │
│  │  └────────┬───────┘  └────────┬───────┘                  │  │
│  └───────────┼──────────────────────┼────────────────────────┘  │
│              │                      │                            │
│              ▼                      ▼                            │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │         Omniverse Client Library                          │  │
│  │  (與 Nucleus 通訊)                                         │  │
│  └────────────────────────┬──────────────────────────────────┘  │
└───────────────────────────┼──────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│              NVIDIA Omniverse 平台                               │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │              Omniverse Nucleus Server                     │  │
│  │  (OpenUSD 資料庫 + 協作引擎)                               │  │
│  └────────────────────────┬──────────────────────────────────┘  │
│                           │                                      │
│                           ▼                                      │
│  ┌───────────────────────────────────────────────────────────┐  │
│  │              OpenUSD 場景                                  │  │
│  │  (3D 模型 + 即時資料 + 動畫)                                │  │
│  └────────────────────────┬──────────────────────────────────┘  │
└───────────────────────────┼──────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│              多用戶協作視覺化層                                   │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐       │
│  │ Omniverse│  │ Omniverse│  │   Web    │  │  Mobile  │       │
│  │   View   │  │   Kit    │  │  Client  │  │   App    │       │
│  │  (工程師) │  │ (操作員)  │  │ (管理層) │  │ (現場)   │       │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘       │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. 核心元件設計

### 2.1 Omniverse Connector for NDH

這是整合的核心元件,負責將 NDH 的工業資料轉換為 Omniverse 可理解的 OpenUSD 格式。

#### 2.1.1 元件架構

```python
class OmniverseConnector:
    """
    NDH 到 Omniverse 的連接器
    
    職責:
    1. 訂閱 NDH Kafka 事件流
    2. 將 IADL 資料轉換為 OpenUSD
    3. 透過 Omniverse Client Library 更新 Nucleus
    """
    
    def __init__(self, config: OmniverseConnectorConfig):
        self.kafka_consumer = KafkaConsumer(config.kafka_config)
        self.iadl_transformer = IADLToUSDTransformer()
        self.omni_client = OmniverseClient(config.nucleus_url)
        self.usd_stage = None
        
    async def connect(self):
        """連接到 Kafka 和 Nucleus"""
        await self.kafka_consumer.connect()
        await self.omni_client.connect()
        self.usd_stage = self.omni_client.open_stage(self.config.stage_url)
        
    async def start(self):
        """開始監聽和處理事件"""
        async for message in self.kafka_consumer:
            await self.process_message(message)
            
    async def process_message(self, message: KafkaMessage):
        """處理單個 Kafka 訊息"""
        # 1. 解析 IADL 資料
        iadl_data = IADLParser.parse(message.value)
        
        # 2. 轉換為 OpenUSD
        usd_updates = self.iadl_transformer.transform(iadl_data)
        
        # 3. 更新 USD Stage
        await self.apply_usd_updates(usd_updates)
        
    async def apply_usd_updates(self, updates: List[USDUpdate]):
        """應用 USD 更新到 Nucleus"""
        for update in updates:
            prim = self.usd_stage.GetPrimAtPath(update.prim_path)
            if prim:
                # 更新屬性 (如溫度、壓力、狀態等)
                prim.GetAttribute(update.attr_name).Set(update.value)
        
        # 保存變更到 Nucleus (觸發協作同步)
        self.usd_stage.Save()
```

#### 2.1.2 配置結構

```yaml
# omniverse_connector_config.yaml
connector:
  name: "NDH-Omniverse-Connector"
  version: "1.0.0"

kafka:
  bootstrap_servers: "localhost:9092"
  topics:
    - "asset.telemetry"
    - "asset.events"
    - "asset.alarms"
  group_id: "omniverse-connector"

nucleus:
  url: "omniverse://nucleus.company.com"
  username: "ndh_connector"
  api_token: "${NUCLEUS_API_TOKEN}"

stage:
  url: "omniverse://nucleus.company.com/Projects/Factory/DigitalTwin.usd"
  update_rate: 10  # Hz

mapping:
  # IADL 到 OpenUSD 的映射規則
  asset_to_prim:
    - iadl_type: "Motor"
      usd_prim_type: "Xform"
      usd_schema: "PhysicsRigidBodyAPI"
    - iadl_type: "Sensor"
      usd_prim_type: "Xform"
      usd_schema: "SensorAPI"
      
  property_to_attribute:
    - iadl_property: "temperature"
      usd_attribute: "custom:temperature"
      unit_conversion: "celsius_to_kelvin"
    - iadl_property: "speed"
      usd_attribute: "custom:rpm"
```

### 2.2 IADL 到 OpenUSD 轉換器

負責將 IADL 格式的工業資料映射到 OpenUSD 的資料結構。

#### 2.2.1 轉換邏輯

```python
class IADLToUSDTransformer:
    """
    將 IADL 資料轉換為 OpenUSD 格式
    """
    
    def __init__(self, mapping_config: MappingConfig):
        self.mapping = mapping_config
        
    def transform(self, iadl_data: IADLAsset) -> List[USDUpdate]:
        """
        轉換單個 IADL 資產為 USD 更新
        
        Args:
            iadl_data: IADL 格式的資產資料
            
        Returns:
            USD 更新列表
        """
        updates = []
        
        # 1. 映射資產到 USD Prim
        prim_path = self._map_asset_to_prim_path(iadl_data)
        
        # 2. 映射屬性到 USD Attributes
        for prop_name, prop_value in iadl_data.properties.items():
            usd_attr = self._map_property_to_attribute(prop_name)
            converted_value = self._convert_value(prop_value, prop_name)
            
            updates.append(USDUpdate(
                prim_path=prim_path,
                attr_name=usd_attr,
                value=converted_value,
                timestamp=iadl_data.timestamp
            ))
            
        # 3. 處理狀態變化 (如顏色、可見性)
        if iadl_data.state_changed:
            visual_updates = self._generate_visual_updates(iadl_data)
            updates.extend(visual_updates)
            
        return updates
        
    def _map_asset_to_prim_path(self, asset: IADLAsset) -> str:
        """
        將 IADL 資產映射到 USD Prim 路徑
        
        範例:
        IADL: /Factory/Line1/Motor01
        USD:  /World/Factory/ProductionLine1/Motor_01
        """
        # 根據映射規則轉換路徑
        path_parts = asset.id.split('/')
        usd_path = '/World/' + '/'.join(self._sanitize_name(p) for p in path_parts)
        return usd_path
        
    def _map_property_to_attribute(self, property_name: str) -> str:
        """
        將 IADL 屬性名稱映射到 USD 屬性名稱
        
        範例:
        IADL: temperature
        USD:  custom:temperature
        """
        mapping = self.mapping.get_property_mapping(property_name)
        return mapping.usd_attribute if mapping else f"custom:{property_name}"
        
    def _convert_value(self, value: Any, property_name: str) -> Any:
        """
        轉換值的單位或格式
        
        範例:
        - 溫度: 攝氏 → 開氏
        - 時間戳: Unix timestamp → USD TimeCode
        """
        converter = self.mapping.get_value_converter(property_name)
        return converter(value) if converter else value
        
    def _generate_visual_updates(self, asset: IADLAsset) -> List[USDUpdate]:
        """
        根據資產狀態生成視覺更新
        
        範例:
        - 設備故障 → 紅色高亮
        - 設備運行 → 綠色 + 旋轉動畫
        - 設備停止 → 灰色
        """
        updates = []
        prim_path = self._map_asset_to_prim_path(asset)
        
        if asset.state == "ALARM":
            # 設置為紅色
            updates.append(USDUpdate(
                prim_path=prim_path,
                attr_name="primvars:displayColor",
                value=Gf.Vec3f(1.0, 0.0, 0.0)  # Red
            ))
        elif asset.state == "RUNNING":
            # 設置為綠色 + 啟用動畫
            updates.append(USDUpdate(
                prim_path=prim_path,
                attr_name="primvars:displayColor",
                value=Gf.Vec3f(0.0, 1.0, 0.0)  # Green
            ))
            updates.append(USDUpdate(
                prim_path=prim_path,
                attr_name="xformOp:rotateY",
                value=self._calculate_rotation(asset.properties.get("speed", 0))
            ))
            
        return updates
```

### 2.3 Omniverse Client 封裝

封裝 Omniverse Client Library,提供簡化的 API。

```python
class OmniverseClient:
    """
    Omniverse Client Library 的封裝
    """
    
    def __init__(self, nucleus_url: str):
        self.nucleus_url = nucleus_url
        self.client = None
        
    async def connect(self, username: str = None, api_token: str = None):
        """連接到 Nucleus Server"""
        import omni.client
        
        # 設置認證
        if username and api_token:
            omni.client.set_authentication_message_box_enabled(False)
            omni.client.set_credentials(username, api_token)
            
        # 連接到 Nucleus
        result = await omni.client.initialize_async()
        if result != omni.client.Result.OK:
            raise ConnectionError(f"Failed to connect to Nucleus: {result}")
            
        self.client = omni.client
        
    def open_stage(self, stage_url: str) -> Usd.Stage:
        """
        打開或創建 USD Stage
        
        Args:
            stage_url: Omniverse URL, 例如:
                      omniverse://nucleus/Projects/Factory/DigitalTwin.usd
        """
        from pxr import Usd
        
        # 檢查 Stage 是否存在
        result, entry = self.client.stat(stage_url)
        
        if result == omni.client.Result.OK:
            # Stage 存在,打開它
            stage = Usd.Stage.Open(stage_url)
        else:
            # Stage 不存在,創建新的
            stage = Usd.Stage.CreateNew(stage_url)
            
        return stage
        
    async def save_stage(self, stage: Usd.Stage):
        """
        保存 Stage 到 Nucleus
        這會觸發協作同步,所有連接的客戶端都會看到更新
        """
        stage.Save()
        
    async def create_live_layer(self, stage_url: str) -> str:
        """
        創建 Live Layer 用於即時協作
        
        Returns:
            Live Layer 的 URL
        """
        live_layer_url = stage_url.replace(".usd", "_live.usd")
        
        # 創建 Live Layer
        from pxr import Sdf
        layer = Sdf.Layer.CreateNew(live_layer_url)
        
        return live_layer_url
```

---

## 3. 資料流設計

### 3.1 即時資料流

```
工業設備 → NDH Connector → Kafka Topic → Omniverse Connector
                                              ↓
                                    IADL → OpenUSD 轉換
                                              ↓
                                    Omniverse Client Library
                                              ↓
                                        Nucleus Server
                                              ↓
                                    所有連接的客戶端 (即時更新)
```

**延遲分析**:
- NDH Connector → Kafka: < 10ms
- Kafka → Omniverse Connector: < 50ms
- IADL → OpenUSD 轉換: < 20ms
- 更新 Nucleus: < 100ms
- Nucleus → 客戶端: < 50ms
- **總延遲**: < 230ms (對於大多數工業應用可接受)

### 3.2 歷史資料回放

利用 NDH 的 Kafka 事件溯源能力和 OpenUSD 的時間軸功能:

```python
class HistoricalPlayback:
    """
    歷史資料回放功能
    """
    
    def __init__(self, omni_client: OmniverseClient, kafka_consumer: KafkaConsumer):
        self.omni_client = omni_client
        self.kafka_consumer = kafka_consumer
        
    async def replay(self, start_time: datetime, end_time: datetime, speed: float = 1.0):
        """
        回放指定時間範圍的歷史資料
        
        Args:
            start_time: 開始時間
            end_time: 結束時間
            speed: 回放速度 (1.0 = 實時, 2.0 = 2倍速)
        """
        # 1. 從 Kafka 讀取歷史事件
        events = await self.kafka_consumer.read_range(start_time, end_time)
        
        # 2. 在 USD 中創建時間採樣
        stage = self.omni_client.open_stage(self.config.stage_url)
        
        for event in events:
            # 計算 USD TimeCode
            time_code = self._calculate_time_code(event.timestamp, start_time)
            
            # 轉換為 USD 更新
            usd_updates = self.transformer.transform(event.data)
            
            # 應用更新到特定時間點
            for update in usd_updates:
                prim = stage.GetPrimAtPath(update.prim_path)
                attr = prim.GetAttribute(update.attr_name)
                attr.Set(update.value, time_code)
                
        # 3. 保存 Stage
        await self.omni_client.save_stage(stage)
        
        # 4. 客戶端可以使用時間軸控制回放
```

### 3.3 批次更新優化

為了提高效能,實作批次更新機制:

```python
class BatchUpdateManager:
    """
    批次更新管理器
    減少 Nucleus 的寫入次數,提高效能
    """
    
    def __init__(self, batch_size: int = 100, flush_interval: float = 0.1):
        self.batch_size = batch_size
        self.flush_interval = flush_interval
        self.pending_updates = []
        self.last_flush_time = time.time()
        
    async def add_update(self, update: USDUpdate):
        """添加更新到批次"""
        self.pending_updates.append(update)
        
        # 檢查是否需要刷新
        if (len(self.pending_updates) >= self.batch_size or
            time.time() - self.last_flush_time >= self.flush_interval):
            await self.flush()
            
    async def flush(self):
        """刷新所有待處理的更新到 Nucleus"""
        if not self.pending_updates:
            return
            
        # 應用所有更新
        for update in self.pending_updates:
            # ... 應用邏輯 ...
            pass
            
        # 一次性保存
        await self.omni_client.save_stage(self.stage)
        
        # 清空批次
        self.pending_updates.clear()
        self.last_flush_time = time.time()
```

---

## 4. OpenUSD 場景結構

### 4.1 場景層次結構

```
/World                              # 根節點
  /Factory                          # 工廠
    /Building                       # 建築物 (靜態模型)
      /Walls
      /Floors
      /Roof
    /ProductionLines                # 生產線
      /Line1                        # 生產線 1
        /Machines                   # 機器
          /CNC_01                   # CNC 機床 1
            /Geometry               # 3D 模型
            /Sensors                # 感測器
              /Temperature_Sensor
              /Vibration_Sensor
            /Actuators              # 執行器
              /Motor
              /Spindle
          /Robot_01                 # 機器人 1
            /Geometry
            /Joints                 # 關節
              /Joint_1
              /Joint_2
              ...
        /Conveyors                  # 輸送帶
          /Conveyor_01
      /Line2                        # 生產線 2
        ...
    /Utilities                      # 公用設施
      /HVAC                         # 暖通空調
      /Electrical                   # 電力
      /Compressed_Air               # 壓縮空氣
    /Warehouse                      # 倉庫
      /Shelves
      /AGVs                         # 自動導引車
```

### 4.2 USD Prim 範例

```python
# CNC 機床的 USD 定義
def create_cnc_machine_prim(stage: Usd.Stage, prim_path: str):
    """
    創建 CNC 機床的 USD Prim
    """
    from pxr import Usd, UsdGeom, UsdPhysics, Gf
    
    # 創建 Xform (變換節點)
    xform = UsdGeom.Xform.Define(stage, prim_path)
    
    # 設置位置
    xform.AddTranslateOp().Set(Gf.Vec3d(10.0, 0.0, 5.0))
    
    # 引用 3D 模型
    model_path = f"{prim_path}/Geometry"
    model_ref = stage.DefinePrim(model_path)
    model_ref.GetReferences().AddReference("./models/cnc_machine.usd")
    
    # 添加自定義屬性 (工業資料)
    prim = stage.GetPrimAtPath(prim_path)
    
    # 溫度
    temp_attr = prim.CreateAttribute("custom:temperature", Sdf.ValueTypeNames.Float)
    temp_attr.Set(25.0)  # 初始值
    
    # 主軸轉速
    rpm_attr = prim.CreateAttribute("custom:spindle_rpm", Sdf.ValueTypeNames.Float)
    rpm_attr.Set(0.0)
    
    # 狀態
    state_attr = prim.CreateAttribute("custom:state", Sdf.ValueTypeNames.String)
    state_attr.Set("IDLE")
    
    # 告警
    alarm_attr = prim.CreateAttribute("custom:alarm", Sdf.ValueTypeNames.Bool)
    alarm_attr.Set(False)
    
    # 添加物理屬性 (可選,用於模擬)
    UsdPhysics.RigidBodyAPI.Apply(prim)
    
    return xform
```

---

## 5. 多人協作機制

### 5.1 Live Layers

Omniverse 使用 **Live Layers** 實現多人即時協作:

```python
class CollaborationManager:
    """
    協作管理器
    """
    
    def __init__(self, omni_client: OmniverseClient):
        self.omni_client = omni_client
        self.live_layer = None
        
    async def enable_collaboration(self, stage_url: str):
        """
        為 Stage 啟用協作功能
        """
        # 創建 Live Layer
        live_layer_url = await self.omni_client.create_live_layer(stage_url)
        
        # 打開 Stage 並設置 Live Layer
        stage = self.omni_client.open_stage(stage_url)
        live_layer = Sdf.Layer.FindOrOpen(live_layer_url)
        
        # 設置為 Session Layer (不會永久保存)
        stage.SetEditTarget(live_layer)
        
        self.live_layer = live_layer
        
    async def broadcast_update(self, update: USDUpdate):
        """
        廣播更新到所有協作者
        """
        # 在 Live Layer 中應用更新
        # Nucleus 會自動同步到所有連接的客戶端
        prim = self.live_layer.GetPrimAtPath(update.prim_path)
        attr = prim.GetAttribute(update.attr_name)
        attr.Set(update.value)
        
        # 保存 Live Layer (觸發同步)
        self.live_layer.Save()
```

### 5.2 用戶角色與權限

```python
class UserRole(Enum):
    """用戶角色"""
    VIEWER = "viewer"          # 只能查看
    OPERATOR = "operator"      # 可以查看和控制
    ENGINEER = "engineer"      # 可以查看、控制和修改
    ADMIN = "admin"            # 完全控制

class PermissionManager:
    """
    權限管理器
    """
    
    def __init__(self, nucleus_url: str):
        self.nucleus_url = nucleus_url
        
    async def set_permissions(self, stage_url: str, user: str, role: UserRole):
        """
        設置用戶對 Stage 的權限
        """
        # 使用 Nucleus API 設置 ACL
        permissions = {
            UserRole.VIEWER: ["read"],
            UserRole.OPERATOR: ["read", "write_live"],
            UserRole.ENGINEER: ["read", "write_live", "write"],
            UserRole.ADMIN: ["read", "write_live", "write", "admin"]
        }
        
        await self.omni_client.set_acl(
            stage_url,
            user,
            permissions[role]
        )
```

---

## 6. 效能優化策略

### 6.1 資料過濾與降採樣

```python
class DataFilter:
    """
    資料過濾器
    減少不必要的更新,提高效能
    """
    
    def __init__(self, config: FilterConfig):
        self.config = config
        self.last_values = {}
        
    def should_update(self, asset_id: str, property_name: str, new_value: Any) -> bool:
        """
        判斷是否需要更新
        
        策略:
        1. 值變化閾值: 只有變化超過閾值才更新
        2. 時間間隔: 限制更新頻率
        3. 重要性: 根據屬性重要性決定
        """
        key = f"{asset_id}.{property_name}"
        
        # 檢查時間間隔
        if key in self.last_update_time:
            elapsed = time.time() - self.last_update_time[key]
            if elapsed < self.config.min_update_interval:
                return False
                
        # 檢查值變化
        if key in self.last_values:
            old_value = self.last_values[key]
            threshold = self.config.get_threshold(property_name)
            
            if abs(new_value - old_value) < threshold:
                return False
                
        # 更新記錄
        self.last_values[key] = new_value
        self.last_update_time[key] = time.time()
        
        return True
```

### 6.2 LOD (Level of Detail) 管理

```python
class LODManager:
    """
    LOD 管理器
    根據視角距離動態調整模型細節
    """
    
    def __init__(self, stage: Usd.Stage):
        self.stage = stage
        
    def setup_lod(self, prim_path: str, lod_models: List[str]):
        """
        為 Prim 設置 LOD
        
        Args:
            prim_path: Prim 路徑
            lod_models: LOD 模型列表 [high, medium, low]
        """
        from pxr import UsdGeom
        
        prim = self.stage.GetPrimAtPath(prim_path)
        
        # 創建 LOD 變體集
        variant_set = prim.GetVariantSets().AddVariantSet("lod")
        
        for i, model_path in enumerate(lod_models):
            variant_name = f"lod{i}"
            variant_set.AddVariant(variant_name)
            variant_set.SetVariantSelection(variant_name)
            
            with variant_set.GetVariantEditContext():
                # 引用對應的 LOD 模型
                model_prim = self.stage.DefinePrim(f"{prim_path}/Model")
                model_prim.GetReferences().AddReference(model_path)
                
        # 設置默認 LOD
        variant_set.SetVariantSelection("lod0")
```

### 6.3 空間分區

```python
class SpatialPartitioning:
    """
    空間分區
    將大型場景分割為多個子場景,按需載入
    """
    
    def __init__(self, stage: Usd.Stage):
        self.stage = stage
        
    def create_partitions(self, bounds: Tuple[float, float, float, float],
                         partition_size: float = 100.0):
        """
        創建空間分區
        
        Args:
            bounds: 場景邊界 (min_x, min_y, max_x, max_y)
            partition_size: 分區大小 (米)
        """
        min_x, min_y, max_x, max_y = bounds
        
        for x in range(int(min_x), int(max_x), int(partition_size)):
            for y in range(int(min_y), int(max_y), int(partition_size)):
                # 創建分區
                partition_name = f"Partition_{x}_{y}"
                partition_path = f"/World/Partitions/{partition_name}"
                
                # 創建為 Payload (延遲載入)
                partition_prim = self.stage.DefinePrim(partition_path)
                partition_prim.GetPayloads().AddPayload(
                    f"./partitions/{partition_name}.usd"
                )
```

---

## 7. 部署架構

### 7.1 本地部署

```
┌─────────────────────────────────────────────────────────────┐
│                    本地網路                                  │
│                                                              │
│  ┌──────────────┐      ┌──────────────┐                    │
│  │  NDH Server  │──────│    Kafka     │                    │
│  │  (Docker)    │      │   (Docker)   │                    │
│  └──────┬───────┘      └──────┬───────┘                    │
│         │                     │                             │
│         └─────────┬───────────┘                             │
│                   │                                         │
│         ┌─────────▼──────────┐                             │
│         │  Omniverse         │                             │
│         │  Connector         │                             │
│         │  (Docker)          │                             │
│         └─────────┬──────────┘                             │
│                   │                                         │
│         ┌─────────▼──────────┐                             │
│         │  Nucleus Server    │                             │
│         │  (本地安裝)         │                             │
│         └─────────┬──────────┘                             │
│                   │                                         │
│         ┌─────────▼──────────┐                             │
│         │  Omniverse View    │                             │
│         │  (客戶端)           │                             │
│         └────────────────────┘                             │
└─────────────────────────────────────────────────────────────┘
```

### 7.2 雲端部署

```
┌─────────────────────────────────────────────────────────────┐
│                    AWS / Azure / GCP                         │
│                                                              │
│  ┌──────────────┐      ┌──────────────┐                    │
│  │  NDH Server  │──────│  Kafka MSK   │                    │
│  │  (EKS/AKS)   │      │  (Managed)   │                    │
│  └──────┬───────┘      └──────┬───────┘                    │
│         │                     │                             │
│         └─────────┬───────────┘                             │
│                   │                                         │
│         ┌─────────▼──────────┐                             │
│         │  Omniverse         │                             │
│         │  Connector         │                             │
│         │  (Container)       │                             │
│         └─────────┬──────────┘                             │
│                   │                                         │
│         ┌─────────▼──────────┐                             │
│         │  Omniverse on      │                             │
│         │  DGX Cloud         │                             │
│         └─────────┬──────────┘                             │
└───────────────────┼──────────────────────────────────────────┘
                    │
                    │ (HTTPS / WebRTC)
                    │
┌───────────────────▼──────────────────────────────────────────┐
│                    用戶端                                     │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  Web Browser │  │  Mobile App  │  │ Omniverse    │      │
│  │  (Streaming) │  │  (Streaming) │  │ View (Native)│      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
```

### 7.3 混合部署

```
┌─────────────────────────────────────────────────────────────┐
│                    本地資料中心                               │
│  ┌──────────────┐      ┌──────────────┐                    │
│  │  NDH Server  │──────│    Kafka     │                    │
│  │  (本地)      │      │   (本地)     │                    │
│  └──────┬───────┘      └──────┬───────┘                    │
│         │                     │                             │
│         └─────────┬───────────┘                             │
│                   │                                         │
│         ┌─────────▼──────────┐                             │
│         │  Omniverse         │                             │
│         │  Connector         │                             │
│         └─────────┬──────────┘                             │
└───────────────────┼──────────────────────────────────────────┘
                    │ (VPN / Direct Connect)
                    │
┌───────────────────▼──────────────────────────────────────────┐
│                    雲端 (AWS / Azure)                         │
│         ┌────────────────────┐                              │
│         │  Omniverse on      │                              │
│         │  DGX Cloud         │                              │
│         │  (渲染 + 協作)      │                              │
│         └─────────┬──────────┘                              │
└───────────────────┼──────────────────────────────────────────┘
                    │
                    ▼
              全球用戶存取
```

---

## 8. 安全性考量

### 8.1 認證與授權

```python
class SecurityManager:
    """
    安全管理器
    """
    
    def __init__(self, config: SecurityConfig):
        self.config = config
        
    async def authenticate_connector(self) -> str:
        """
        連接器認證
        返回 API Token
        """
        # 使用服務帳戶認證
        response = await self.nucleus_api.authenticate(
            username=self.config.service_account,
            password=self.config.service_password
        )
        
        return response.api_token
        
    async def authorize_operation(self, user: str, operation: str, resource: str) -> bool:
        """
        授權檢查
        """
        # 檢查用戶是否有權限執行操作
        permissions = await self.get_user_permissions(user)
        
        return operation in permissions.get(resource, [])
```

### 8.2 資料加密

```python
class EncryptionManager:
    """
    加密管理器
    """
    
    def __init__(self, encryption_key: bytes):
        self.cipher = Fernet(encryption_key)
        
    def encrypt_sensitive_data(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """
        加密敏感資料
        """
        encrypted_data = data.copy()
        
        for key in self.config.sensitive_fields:
            if key in encrypted_data:
                value = str(encrypted_data[key])
                encrypted_value = self.cipher.encrypt(value.encode())
                encrypted_data[key] = encrypted_value.decode()
                
        return encrypted_data
        
    def decrypt_sensitive_data(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """
        解密敏感資料
        """
        decrypted_data = data.copy()
        
        for key in self.config.sensitive_fields:
            if key in decrypted_data:
                encrypted_value = decrypted_data[key].encode()
                value = self.cipher.decrypt(encrypted_value)
                decrypted_data[key] = value.decode()
                
        return decrypted_data
```

### 8.3 審計日誌

```python
class AuditLogger:
    """
    審計日誌記錄器
    """
    
    def __init__(self, log_path: str):
        self.log_path = log_path
        
    async def log_operation(self, user: str, operation: str, resource: str, result: str):
        """
        記錄操作
        """
        log_entry = {
            "timestamp": datetime.now().isoformat(),
            "user": user,
            "operation": operation,
            "resource": resource,
            "result": result,
            "ip_address": self.get_client_ip()
        }
        
        # 寫入日誌
        async with aiofiles.open(self.log_path, 'a') as f:
            await f.write(json.dumps(log_entry) + '\n')
```

---

## 9. 監控與告警

### 9.1 效能監控

```python
class PerformanceMonitor:
    """
    效能監控器
    """
    
    def __init__(self):
        self.metrics = {
            "kafka_lag": 0,
            "transformation_time": 0,
            "nucleus_write_time": 0,
            "total_latency": 0,
            "updates_per_second": 0
        }
        
    async def collect_metrics(self):
        """
        收集效能指標
        """
        while True:
            # Kafka Lag
            self.metrics["kafka_lag"] = await self.get_kafka_lag()
            
            # 轉換時間
            self.metrics["transformation_time"] = self.transformer.get_avg_time()
            
            # Nucleus 寫入時間
            self.metrics["nucleus_write_time"] = self.omni_client.get_avg_write_time()
            
            # 總延遲
            self.metrics["total_latency"] = sum([
                self.metrics["kafka_lag"],
                self.metrics["transformation_time"],
                self.metrics["nucleus_write_time"]
            ])
            
            # 更新頻率
            self.metrics["updates_per_second"] = self.update_counter.get_rate()
            
            # 發送到監控系統 (Prometheus, Grafana)
            await self.send_to_prometheus(self.metrics)
            
            await asyncio.sleep(10)
```

### 9.2 告警系統

```python
class AlertManager:
    """
    告警管理器
    """
    
    def __init__(self, config: AlertConfig):
        self.config = config
        self.alert_rules = self.load_alert_rules()
        
    async def check_alerts(self, metrics: Dict[str, float]):
        """
        檢查告警條件
        """
        for rule in self.alert_rules:
            if self.evaluate_rule(rule, metrics):
                await self.send_alert(rule, metrics)
                
    def evaluate_rule(self, rule: AlertRule, metrics: Dict[str, float]) -> bool:
        """
        評估告警規則
        """
        metric_value = metrics.get(rule.metric_name)
        
        if rule.condition == "greater_than":
            return metric_value > rule.threshold
        elif rule.condition == "less_than":
            return metric_value < rule.threshold
            
        return False
        
    async def send_alert(self, rule: AlertRule, metrics: Dict[str, float]):
        """
        發送告警
        """
        alert_message = f"""
        Alert: {rule.name}
        Metric: {rule.metric_name}
        Current Value: {metrics[rule.metric_name]}
        Threshold: {rule.threshold}
        Severity: {rule.severity}
        """
        
        # 發送到多個通道
        await self.send_email(alert_message)
        await self.send_slack(alert_message)
        await self.send_webhook(alert_message)
```

---

## 10. 總結

本文件描述了 NDH 與 NVIDIA Omniverse 整合的完整架構設計。這個整合方案實現了:

1. **即時資料同步**: 工業現場資料即時反映到 3D 數位分身
2. **多人協作**: 支援多個利害關係人同時查看和互動
3. **標準化**: 基於 IADL 和 OpenUSD 兩個開放標準
4. **可擴展性**: 從單一設備到整個工廠
5. **高效能**: 通過批次更新、LOD、空間分區等優化策略
6. **安全性**: 完整的認證、授權、加密和審計機制
7. **可觀測性**: 全面的監控和告警系統

下一步將實作核心元件並進行測試驗證。

