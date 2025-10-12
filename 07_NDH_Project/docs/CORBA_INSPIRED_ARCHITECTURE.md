# NDH 分散式物件架構設計 (參考 CORBA)

## 目錄

1. [CORBA 核心概念回顧](#corba-核心概念回顧)
2. [NDH 與 CORBA 的對應關係](#ndh-與-corba-的對應關係)
3. [NDH ORB 設計](#ndh-orb-設計)
4. [IDL 定義 (使用 IADL)](#idl-定義-使用-iadl)
5. [物件參考和命名服務](#物件參考和命名服務)
6. [完整架構實作](#完整架構實作)
7. [與傳統 CORBA 的差異](#與傳統-corba-的差異)

## CORBA 核心概念回顧

### CORBA 架構圖

```
┌─────────────────────────────────────────────────────────┐
│                    Client Application                    │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│              Client Stub (Generated)                     │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│                      ORB Core                            │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   Naming     │  │   Trading    │  │   Security   │  │
│  │   Service    │  │   Service    │  │   Service    │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└────────────────────┬────────────────────────────────────┘
                     │ IIOP (Network Protocol)
                     ▼
┌─────────────────────────────────────────────────────────┐
│              Server Skeleton (Generated)                 │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│                  Object Implementation                   │
└─────────────────────────────────────────────────────────┘
```

### CORBA 核心元件

1. **IDL (Interface Definition Language)**: 定義物件介面
2. **ORB (Object Request Broker)**: 物件請求代理
3. **Stub**: 客戶端代理
4. **Skeleton**: 伺服器端骨架
5. **IOR (Interoperable Object Reference)**: 物件參考
6. **Naming Service**: 命名服務
7. **IIOP (Internet Inter-ORB Protocol)**: 通訊協定

## NDH 與 CORBA 的對應關係

### 概念對應表

| CORBA 概念 | NDH 對應 | 說明 |
|-----------|---------|------|
| IDL | IADL | 介面定義語言 |
| ORB | NDH Broker | 物件請求代理 |
| Object Reference (IOR) | Asset Reference | 資產參考 |
| Naming Service | Asset Registry | 資產註冊表 |
| Servant | Asset Instance | 資產實例 |
| POA (Portable Object Adapter) | Asset Adapter | 資產適配器 |
| IIOP | HTTP/WebSocket/MQTT | 通訊協定 |
| Stub | Client Connector | 客戶端連接器 |
| Skeleton | Server Handler | 伺服器處理器 |

### NDH 架構圖

```
┌─────────────────────────────────────────────────────────┐
│              Client Application (IADL Editor)            │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│              NDH Client Proxy (Auto-generated)           │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│                    NDH Broker (ORB)                      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   Asset      │  │   Lifecycle  │  │   Event      │  │
│  │   Registry   │  │   Service    │  │   Service    │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   Security   │  │   Transaction│  │   Replication│  │
│  │   Service    │  │   Service    │  │   Service    │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└────────────────────┬────────────────────────────────────┘
                     │ HTTP/WebSocket/MQTT
                     ▼
┌─────────────────────────────────────────────────────────┐
│              NDH Server Adapter                          │
└────────────────────┬────────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────────┐
│              Asset Implementation (Servant)              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   PUMP_001   │  │   MOTOR_001  │  │   SENSOR_001 │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└─────────────────────────────────────────────────────────┘
```

## NDH ORB 設計

### 核心 ORB 實作

```python
"""
NDH Object Request Broker (ORB)
參考 CORBA ORB 設計
"""

import asyncio
import uuid
from typing import Dict, Any, Optional, Callable
from abc import ABC, abstractmethod
from dataclasses import dataclass
from enum import Enum

class AssetLifecycleState(Enum):
    """資產生命週期狀態 (對應 CORBA POA 狀態)"""
    INACTIVE = "inactive"
    ACTIVE = "active"
    HOLDING = "holding"
    DISCARDING = "discarding"

@dataclass
class AssetReference:
    """
    資產參考 (對應 CORBA IOR)
    唯一標識一個分散式資產物件
    """
    asset_id: str
    uuid: str
    location: str
    endpoint: str  # 網路端點
    protocol: str  # http, ws, mqtt
    timestamp: float
    
    def to_string(self) -> str:
        """轉換為字串表示 (類似 IOR 字串)"""
        return f"ndhref://{self.protocol}@{self.endpoint}/{self.location}/{self.asset_id}?uuid={self.uuid}"
    
    @classmethod
    def from_string(cls, ref_string: str):
        """從字串解析 (類似 string_to_object)"""
        # 解析 ndhref://protocol@endpoint/location/asset_id?uuid=xxx
        # 簡化實作
        pass

class AssetInterface(ABC):
    """
    資產介面基類 (對應 CORBA Interface)
    所有資產物件必須實作此介面
    """
    
    @abstractmethod
    async def get_attribute(self, attr_name: str) -> Any:
        """取得屬性值"""
        pass
    
    @abstractmethod
    async def set_attribute(self, attr_name: str, value: Any) -> bool:
        """設定屬性值"""
        pass
    
    @abstractmethod
    async def invoke_operation(self, operation: str, params: Dict[str, Any]) -> Any:
        """呼叫操作"""
        pass
    
    @abstractmethod
    async def get_state(self) -> Dict[str, Any]:
        """取得完整狀態"""
        pass

class AssetServant(AssetInterface):
    """
    資產僕人 (對應 CORBA Servant)
    實際的資產物件實作
    """
    
    def __init__(self, asset_id: str, asset_type: str, location: str):
        self.asset_id = asset_id
        self.asset_type = asset_type
        self.location = location
        self.attributes: Dict[str, Any] = {}
        self.state = AssetLifecycleState.INACTIVE
        self.created_at = asyncio.get_event_loop().time()
        self.last_updated = self.created_at
    
    async def get_attribute(self, attr_name: str) -> Any:
        """取得屬性值"""
        return self.attributes.get(attr_name)
    
    async def set_attribute(self, attr_name: str, value: Any) -> bool:
        """設定屬性值"""
        self.attributes[attr_name] = value
        self.last_updated = asyncio.get_event_loop().time()
        return True
    
    async def invoke_operation(self, operation: str, params: Dict[str, Any]) -> Any:
        """呼叫操作"""
        # 動態呼叫方法
        if hasattr(self, operation):
            method = getattr(self, operation)
            return await method(**params)
        else:
            raise AttributeError(f"Operation {operation} not found")
    
    async def get_state(self) -> Dict[str, Any]:
        """取得完整狀態"""
        return {
            "asset_id": self.asset_id,
            "asset_type": self.asset_type,
            "location": self.location,
            "attributes": self.attributes,
            "lifecycle_state": self.state.value,
            "created_at": self.created_at,
            "last_updated": self.last_updated
        }
    
    async def activate(self):
        """啟動資產"""
        self.state = AssetLifecycleState.ACTIVE
    
    async def deactivate(self):
        """停用資產"""
        self.state = AssetLifecycleState.INACTIVE

class AssetAdapter:
    """
    資產適配器 (對應 CORBA POA)
    管理資產物件的生命週期
    """
    
    def __init__(self, adapter_name: str):
        self.adapter_name = adapter_name
        self.servants: Dict[str, AssetServant] = {}
        self.active_objects: Dict[str, str] = {}  # uuid -> asset_id
    
    async def activate_object(self, servant: AssetServant) -> str:
        """
        啟動物件 (對應 POA::activate_object)
        返回物件 ID
        """
        object_id = str(uuid.uuid4())
        self.servants[servant.asset_id] = servant
        self.active_objects[object_id] = servant.asset_id
        await servant.activate()
        return object_id
    
    async def deactivate_object(self, object_id: str):
        """停用物件 (對應 POA::deactivate_object)"""
        if object_id in self.active_objects:
            asset_id = self.active_objects[object_id]
            servant = self.servants.get(asset_id)
            if servant:
                await servant.deactivate()
            del self.active_objects[object_id]
    
    async def find_servant(self, asset_id: str) -> Optional[AssetServant]:
        """尋找僕人物件"""
        return self.servants.get(asset_id)
    
    def list_active_objects(self) -> list:
        """列出所有活躍物件"""
        return list(self.active_objects.keys())

class NamingService:
    """
    命名服務 (對應 CORBA Naming Service)
    提供名稱到物件參考的映射
    """
    
    def __init__(self):
        self.bindings: Dict[str, AssetReference] = {}
        self.contexts: Dict[str, Dict[str, AssetReference]] = {}
    
    async def bind(self, name: str, obj_ref: AssetReference, context: str = "default"):
        """
        綁定名稱到物件參考 (對應 NamingContext::bind)
        """
        if context not in self.contexts:
            self.contexts[context] = {}
        
        full_name = f"{context}/{name}"
        self.bindings[full_name] = obj_ref
        self.contexts[context][name] = obj_ref
    
    async def resolve(self, name: str, context: str = "default") -> Optional[AssetReference]:
        """
        解析名稱到物件參考 (對應 NamingContext::resolve)
        """
        full_name = f"{context}/{name}"
        return self.bindings.get(full_name)
    
    async def unbind(self, name: str, context: str = "default"):
        """解除綁定 (對應 NamingContext::unbind)"""
        full_name = f"{context}/{name}"
        if full_name in self.bindings:
            del self.bindings[full_name]
        if context in self.contexts and name in self.contexts[context]:
            del self.contexts[context][name]
    
    async def list(self, context: str = "default") -> list:
        """列出上下文中的所有綁定"""
        return list(self.contexts.get(context, {}).keys())

class EventService:
    """
    事件服務 (對應 CORBA Event Service)
    提供發布/訂閱機制
    """
    
    def __init__(self):
        self.subscribers: Dict[str, list] = {}  # event_type -> [callbacks]
    
    async def subscribe(self, event_type: str, callback: Callable):
        """訂閱事件"""
        if event_type not in self.subscribers:
            self.subscribers[event_type] = []
        self.subscribers[event_type].append(callback)
    
    async def unsubscribe(self, event_type: str, callback: Callable):
        """取消訂閱"""
        if event_type in self.subscribers:
            self.subscribers[event_type].remove(callback)
    
    async def publish(self, event_type: str, event_data: Any):
        """發布事件"""
        if event_type in self.subscribers:
            for callback in self.subscribers[event_type]:
                await callback(event_data)

class NDH_ORB:
    """
    NDH Object Request Broker
    核心物件請求代理
    """
    
    def __init__(self, orb_id: str, endpoint: str):
        self.orb_id = orb_id
        self.endpoint = endpoint
        
        # 核心服務
        self.naming_service = NamingService()
        self.event_service = EventService()
        
        # 適配器管理
        self.adapters: Dict[str, AssetAdapter] = {}
        
        # 物件參考快取
        self.object_cache: Dict[str, AssetReference] = {}
    
    async def init(self):
        """初始化 ORB (對應 ORB::init)"""
        # 建立預設適配器
        root_adapter = AssetAdapter("RootPOA")
        self.adapters["RootPOA"] = root_adapter
        
        print(f"NDH ORB initialized: {self.orb_id}")
        print(f"Endpoint: {self.endpoint}")
    
    def create_adapter(self, adapter_name: str) -> AssetAdapter:
        """建立新的適配器"""
        adapter = AssetAdapter(adapter_name)
        self.adapters[adapter_name] = adapter
        return adapter
    
    def get_adapter(self, adapter_name: str) -> Optional[AssetAdapter]:
        """取得適配器"""
        return self.adapters.get(adapter_name)
    
    async def create_asset_reference(
        self,
        asset_id: str,
        location: str,
        protocol: str = "http"
    ) -> AssetReference:
        """
        建立資產參考 (對應 object_to_string)
        """
        asset_uuid = str(uuid.uuid5(
            uuid.NAMESPACE_DNS,
            f"{location}.{asset_id}"
        ))
        
        ref = AssetReference(
            asset_id=asset_id,
            uuid=asset_uuid,
            location=location,
            endpoint=self.endpoint,
            protocol=protocol,
            timestamp=asyncio.get_event_loop().time()
        )
        
        # 快取參考
        self.object_cache[asset_uuid] = ref
        
        return ref
    
    async def resolve_reference(self, ref: AssetReference) -> Optional[AssetServant]:
        """
        解析參考到實際物件 (對應 string_to_object + narrow)
        """
        # 檢查是否是本地物件
        if ref.endpoint == self.endpoint:
            # 本地物件,直接從適配器取得
            for adapter in self.adapters.values():
                servant = await adapter.find_servant(ref.asset_id)
                if servant:
                    return servant
        else:
            # 遠端物件,建立代理
            return await self._create_remote_proxy(ref)
        
        return None
    
    async def _create_remote_proxy(self, ref: AssetReference) -> AssetInterface:
        """建立遠端物件代理"""
        # 實作遠端代理邏輯
        # 這裡會建立一個代理物件,將方法呼叫轉換為網路請求
        pass
    
    async def invoke_remote(
        self,
        ref: AssetReference,
        operation: str,
        params: Dict[str, Any]
    ) -> Any:
        """
        遠端方法呼叫 (對應 Dynamic Invocation Interface)
        """
        # 根據協定選擇通訊方式
        if ref.protocol == "http":
            return await self._invoke_http(ref, operation, params)
        elif ref.protocol == "ws":
            return await self._invoke_websocket(ref, operation, params)
        elif ref.protocol == "mqtt":
            return await self._invoke_mqtt(ref, operation, params)
    
    async def _invoke_http(self, ref: AssetReference, operation: str, params: Dict[str, Any]):
        """HTTP 協定呼叫"""
        import httpx
        
        url = f"{ref.endpoint}/api/v1/assets/{ref.location}/{ref.asset_id}/invoke"
        payload = {
            "operation": operation,
            "params": params
        }
        
        async with httpx.AsyncClient() as client:
            response = await client.post(url, json=payload)
            return response.json()
    
    async def _invoke_websocket(self, ref: AssetReference, operation: str, params: Dict[str, Any]):
        """WebSocket 協定呼叫"""
        # 實作 WebSocket 呼叫
        pass
    
    async def _invoke_mqtt(self, ref: AssetReference, operation: str, params: Dict[str, Any]):
        """MQTT 協定呼叫"""
        # 實作 MQTT 呼叫
        pass
    
    def get_naming_service(self) -> NamingService:
        """取得命名服務"""
        return self.naming_service
    
    def get_event_service(self) -> EventService:
        """取得事件服務"""
        return self.event_service
    
    async def shutdown(self):
        """關閉 ORB"""
        # 停用所有適配器
        for adapter in self.adapters.values():
            for object_id in adapter.list_active_objects():
                await adapter.deactivate_object(object_id)
        
        print(f"NDH ORB shutdown: {self.orb_id}")
```

## IDL 定義 (使用 IADL)

### IADL 作為 IDL

```iadl
// IADL 定義 (類似 CORBA IDL)

// 介面定義
interface Pump {
    // 屬性
    attribute float flow_rate;
    attribute float pressure;
    attribute float temperature;
    attribute string status;
    
    // 操作
    void start();
    void stop();
    void set_speed(in float speed);
    float get_efficiency();
    
    // 事件
    event alarm_triggered(in string message);
    event status_changed(in string old_status, in string new_status);
};

// 資產定義
asset PUMP_001 : Pump {
    location: "FACTORY_A/BUILDING_1";
    manufacturer: "Grundfos";
    model: "CR 64-2";
    serial_number: "GF-2024-001234";
};
```

### 從 IADL 生成程式碼

```python
class IADLCompiler:
    """
    IADL 編譯器
    從 IADL 定義生成 Python 程式碼 (類似 IDL 編譯器)
    """
    
    def compile_interface(self, iadl_interface: str) -> str:
        """編譯介面定義"""
        # 解析 IADL
        # 生成 Python 類別
        pass
    
    def generate_servant_base(self, interface_name: str) -> str:
        """生成僕人基類"""
        return f"""
class {interface_name}Servant(AssetServant):
    '''Auto-generated servant base class for {interface_name}'''
    
    def __init__(self, asset_id: str, location: str):
        super().__init__(asset_id, '{interface_name}', location)
    
    # 實作介面方法
    async def start(self):
        raise NotImplementedError()
    
    async def stop(self):
        raise NotImplementedError()
    
    async def set_speed(self, speed: float):
        raise NotImplementedError()
    
    async def get_efficiency(self) -> float:
        raise NotImplementedError()
"""
    
    def generate_proxy(self, interface_name: str) -> str:
        """生成客戶端代理"""
        return f"""
class {interface_name}Proxy(AssetInterface):
    '''Auto-generated client proxy for {interface_name}'''
    
    def __init__(self, orb: NDH_ORB, ref: AssetReference):
        self.orb = orb
        self.ref = ref
    
    async def start(self):
        return await self.orb.invoke_remote(self.ref, 'start', {{}})
    
    async def stop(self):
        return await self.orb.invoke_remote(self.ref, 'stop', {{}})
    
    async def set_speed(self, speed: float):
        return await self.orb.invoke_remote(
            self.ref, 
            'set_speed', 
            {{'speed': speed}}
        )
    
    async def get_efficiency(self) -> float:
        return await self.orb.invoke_remote(self.ref, 'get_efficiency', {{}})
"""
```

## 物件參考和命名服務

### 物件參考格式

```
NDH Object Reference (類似 IOR):

ndhref://http@ndh-server-1.example.com:8000/FACTORY_A/PUMP_001?uuid=550e8400-e29b-41d4-a716-446655440000

格式說明:
- ndhref://      : NDH 參考協定
- http           : 通訊協定 (http/ws/mqtt)
- @              : 分隔符
- ndh-server-1...: 伺服器端點
- :8000          : 埠號
- /FACTORY_A     : 位置/上下文
- /PUMP_001      : 資產 ID
- ?uuid=...      : 全域唯一識別符
```

### 命名服務使用範例

```python
async def naming_service_example():
    """命名服務使用範例"""
    
    # 初始化 ORB
    orb = NDH_ORB("ORB-1", "http://localhost:8000")
    await orb.init()
    
    # 取得命名服務
    naming = orb.get_naming_service()
    
    # 建立資產
    pump = PumpServant("PUMP_001", "FACTORY_A")
    
    # 啟動資產
    adapter = orb.get_adapter("RootPOA")
    object_id = await adapter.activate_object(pump)
    
    # 建立物件參考
    ref = await orb.create_asset_reference("PUMP_001", "FACTORY_A")
    
    # 綁定到命名服務
    await naming.bind("PUMP_001", ref, context="FACTORY_A")
    
    # 客戶端解析名稱
    resolved_ref = await naming.resolve("PUMP_001", context="FACTORY_A")
    
    if resolved_ref:
        print(f"Found asset: {resolved_ref.to_string()}")
        
        # 取得物件
        servant = await orb.resolve_reference(resolved_ref)
        
        # 呼叫方法
        await servant.set_attribute("flow_rate", 125.5)
        flow_rate = await servant.get_attribute("flow_rate")
        print(f"Flow rate: {flow_rate}")
```

## 完整架構實作

### 伺服器端實作

```python
"""
NDH Server Implementation
"""

import asyncio
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel

# 建立 FastAPI 應用
app = FastAPI(title="NDH Server")

# 全域 ORB 實例
orb: Optional[NDH_ORB] = None

class InvokeRequest(BaseModel):
    operation: str
    params: Dict[str, Any]

class AttributeRequest(BaseModel):
    attribute: str
    value: Optional[Any] = None

@app.on_event("startup")
async def startup():
    """啟動時初始化 ORB"""
    global orb
    orb = NDH_ORB("NDH-Server-1", "http://localhost:8000")
    await orb.init()
    
    # 建立一些測試資產
    await create_sample_assets()

async def create_sample_assets():
    """建立範例資產"""
    adapter = orb.get_adapter("RootPOA")
    naming = orb.get_naming_service()
    
    # 建立幫浦
    pump = PumpServant("PUMP_001", "FACTORY_A")
    await adapter.activate_object(pump)
    
    pump_ref = await orb.create_asset_reference("PUMP_001", "FACTORY_A")
    await naming.bind("PUMP_001", pump_ref, context="FACTORY_A")
    
    print("Sample assets created")

@app.post("/api/v1/assets/{location}/{asset_id}/invoke")
async def invoke_operation(
    location: str,
    asset_id: str,
    request: InvokeRequest
):
    """呼叫資產操作"""
    # 從命名服務解析
    naming = orb.get_naming_service()
    ref = await naming.resolve(asset_id, context=location)
    
    if not ref:
        raise HTTPException(status_code=404, detail="Asset not found")
    
    # 取得僕人物件
    servant = await orb.resolve_reference(ref)
    
    if not servant:
        raise HTTPException(status_code=500, detail="Cannot resolve asset")
    
    # 呼叫操作
    try:
        result = await servant.invoke_operation(
            request.operation,
            request.params
        )
        return {"status": "success", "result": result}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))

@app.get("/api/v1/assets/{location}/{asset_id}/attributes/{attr_name}")
async def get_attribute(location: str, asset_id: str, attr_name: str):
    """取得資產屬性"""
    naming = orb.get_naming_service()
    ref = await naming.resolve(asset_id, context=location)
    
    if not ref:
        raise HTTPException(status_code=404, detail="Asset not found")
    
    servant = await orb.resolve_reference(ref)
    value = await servant.get_attribute(attr_name)
    
    return {"attribute": attr_name, "value": value}

@app.post("/api/v1/assets/{location}/{asset_id}/attributes/{attr_name}")
async def set_attribute(
    location: str,
    asset_id: str,
    attr_name: str,
    request: AttributeRequest
):
    """設定資產屬性"""
    naming = orb.get_naming_service()
    ref = await naming.resolve(asset_id, context=location)
    
    if not ref:
        raise HTTPException(status_code=404, detail="Asset not found")
    
    servant = await orb.resolve_reference(ref)
    success = await servant.set_attribute(attr_name, request.value)
    
    return {"status": "success" if success else "failed"}

@app.get("/api/v1/naming/{context}")
async def list_context(context: str):
    """列出命名上下文中的所有資產"""
    naming = orb.get_naming_service()
    assets = await naming.list(context)
    return {"context": context, "assets": assets}
```

### 客戶端實作

```python
"""
NDH Client Implementation
"""

import httpx

class NDHClient:
    """
    NDH 客戶端
    提供簡單的 API 來存取遠端資產
    """
    
    def __init__(self, server_url: str):
        self.server_url = server_url
        self.client = httpx.AsyncClient()
    
    async def resolve_asset(self, asset_id: str, location: str) -> Optional[Dict]:
        """解析資產"""
        url = f"{self.server_url}/api/v1/naming/{location}"
        response = await self.client.get(url)
        
        if response.status_code == 200:
            data = response.json()
            if asset_id in data["assets"]:
                return {"asset_id": asset_id, "location": location}
        
        return None
    
    async def get_attribute(
        self,
        asset_id: str,
        location: str,
        attr_name: str
    ) -> Any:
        """取得屬性"""
        url = f"{self.server_url}/api/v1/assets/{location}/{asset_id}/attributes/{attr_name}"
        response = await self.client.get(url)
        
        if response.status_code == 200:
            return response.json()["value"]
        else:
            raise Exception(f"Failed to get attribute: {response.text}")
    
    async def set_attribute(
        self,
        asset_id: str,
        location: str,
        attr_name: str,
        value: Any
    ) -> bool:
        """設定屬性"""
        url = f"{self.server_url}/api/v1/assets/{location}/{asset_id}/attributes/{attr_name}"
        response = await self.client.post(url, json={"value": value})
        
        return response.status_code == 200
    
    async def invoke_operation(
        self,
        asset_id: str,
        location: str,
        operation: str,
        params: Dict[str, Any]
    ) -> Any:
        """呼叫操作"""
        url = f"{self.server_url}/api/v1/assets/{location}/{asset_id}/invoke"
        payload = {"operation": operation, "params": params}
        response = await self.client.post(url, json=payload)
        
        if response.status_code == 200:
            return response.json()["result"]
        else:
            raise Exception(f"Failed to invoke operation: {response.text}")
    
    async def close(self):
        """關閉客戶端"""
        await self.client.aclose()

# 使用範例
async def client_example():
    """客戶端使用範例"""
    client = NDHClient("http://localhost:8000")
    
    try:
        # 解析資產
        asset = await client.resolve_asset("PUMP_001", "FACTORY_A")
        print(f"Resolved asset: {asset}")
        
        # 取得屬性
        flow_rate = await client.get_attribute("PUMP_001", "FACTORY_A", "flow_rate")
        print(f"Flow rate: {flow_rate}")
        
        # 設定屬性
        await client.set_attribute("PUMP_001", "FACTORY_A", "flow_rate", 150.0)
        
        # 呼叫操作
        result = await client.invoke_operation(
            "PUMP_001",
            "FACTORY_A",
            "start",
            {}
        )
        print(f"Start result: {result}")
    
    finally:
        await client.close()
```

## 與傳統 CORBA 的差異

### 優勢

1. **現代化協定**: 使用 HTTP/WebSocket/MQTT 而非 IIOP
2. **雲端友善**: 易於部署到雲端環境
3. **語言中立**: 不限於特定語言綁定
4. **RESTful**: 支援 REST API 範式
5. **輕量級**: 不需要複雜的 ORB 實作

### 保留的 CORBA 概念

1. **物件參考**: 唯一標識分散式物件
2. **命名服務**: 名稱到物件的映射
3. **生命週期管理**: POA 風格的物件管理
4. **介面定義**: IDL 風格的介面描述
5. **透明性**: 本地/遠端呼叫透明

### 簡化的部分

1. **不需要 IDL 編譯器**: 使用動態語言特性
2. **簡化的協定**: HTTP 比 IIOP 簡單
3. **無需 ORB 配置**: 使用標準 Web 技術
4. **更好的互操作性**: 任何 HTTP 客戶端都能使用

## 總結

NDH 借鑑 CORBA 的核心設計理念:

1. **分散式物件模型**: 資產作為分散式物件
2. **位置透明性**: 本地和遠端資產使用相同介面
3. **命名服務**: 統一的資產查找機制
4. **生命週期管理**: 完整的物件生命週期支援
5. **事件服務**: 發布/訂閱機制

同時採用現代化技術:

1. **HTTP/WebSocket**: 取代 IIOP
2. **JSON**: 取代 CDR
3. **IADL**: 取代 IDL
4. **Python/FastAPI**: 取代傳統 CORBA 實作
5. **Redis**: 提供分散式協調

這種設計使 NDH 既具有 CORBA 的架構優勢,又具有現代分散式系統的靈活性和易用性。

---

**作者**: 林志錚 (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**最後更新**: 2025-01-09

