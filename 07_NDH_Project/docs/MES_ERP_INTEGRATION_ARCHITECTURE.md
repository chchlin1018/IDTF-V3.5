# NDH 與 MES/ERP 系統整合架構設計

## 目錄

1. [整體架構](#整體架構)
2. [分層架構設計](#分層架構設計)
3. [連接器架構](#連接器架構)
4. [資料流設計](#資料流設計)
5. [事件驅動架構](#事件驅動架構)
6. [資料映射機制](#資料映射機制)
7. [API 設計](#api-設計)
8. [部署架構](#部署架構)

## 整體架構

### 三層整合架構

```
┌─────────────────────────────────────────────────────────────────┐
│                     Level 4: ERP 層                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │   SAP ERP    │  │ Oracle ERP   │  │ Dynamics 365 │          │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘          │
│         │                  │                  │                   │
│         └──────────────────┼──────────────────┘                  │
└────────────────────────────┼──────────────────────────────────────┘
                             │ REST API / Web Services
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                     Level 3: MES 層                              │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │ Siemens MES  │  │ Rockwell MES │  │  AVEVA MES   │          │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘          │
│         │                  │                  │                   │
│         └──────────────────┼──────────────────┘                  │
└────────────────────────────┼──────────────────────────────────────┘
                             │ REST API / OPC UA
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                  NDH Integration Layer                           │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │              Enterprise Integration Bus                   │  │
│  │  ┌────────────┐  ┌────────────┐  ┌────────────┐         │  │
│  │  │ ERP        │  │ MES        │  │ Kafka      │         │  │
│  │  │ Connector  │  │ Connector  │  │ Producer   │         │  │
│  │  └─────┬──────┘  └─────┬──────┘  └─────┬──────┘         │  │
│  └────────┼───────────────┼───────────────┼────────────────┘  │
│           │               │               │                    │
│  ┌────────▼───────────────▼───────────────▼────────────────┐  │
│  │              NDH Broker (ORB)                            │  │
│  │  ┌──────────────────────────────────────────────────┐   │  │
│  │  │         Asset Servants (IADL Objects)            │   │  │
│  │  │  ┌─────────┐  ┌─────────┐  ┌─────────┐          │   │  │
│  │  │  │ Machine │  │ Product │  │ Material│          │   │  │
│  │  │  │ Assets  │  │ Assets  │  │ Assets  │          │   │  │
│  │  │  └─────────┘  └─────────┘  └─────────┘          │   │  │
│  │  └──────────────────────────────────────────────────┘   │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────────────┬──────────────────────────────────────┘
                             │ OPC UA / Modbus / MQTT
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                  Level 2: SCADA / Control Layer                  │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │   PI System  │  │   Historian  │  │     PLC      │          │
│  └──────────────┘  └──────────────┘  └──────────────┘          │
└─────────────────────────────────────────────────────────────────┘
```

### 核心元件

| 元件 | 說明 | 職責 |
|------|------|------|
| **ERP Connector** | ERP 系統連接器 | 與 ERP 系統通訊,處理工單、BOM、庫存 |
| **MES Connector** | MES 系統連接器 | 與 MES 系統通訊,處理生產執行資料 |
| **Enterprise Integration Bus** | 企業整合匯流排 | 協調各連接器,管理資料流 |
| **NDH Broker (ORB)** | NDH 核心代理 | 管理資產物件,提供統一介面 |
| **Asset Servants** | 資產物件實例 | 表示實體資產的數位分身 |
| **Kafka Producer/Consumer** | 事件流處理 | 發布和訂閱事件,實現事件溯源 |

## 分層架構設計

### Layer 1: 資料存取層 (Data Access Layer)

```python
"""
資料存取層 - 與外部系統通訊的底層介面
"""

from abc import ABC, abstractmethod
from typing import Dict, List, Any, Optional
from datetime import datetime

class DataAccessInterface(ABC):
    """資料存取介面"""
    
    @abstractmethod
    async def connect(self) -> bool:
        """建立連接"""
        pass
    
    @abstractmethod
    async def disconnect(self):
        """斷開連接"""
        pass
    
    @abstractmethod
    async def read(self, resource: str, params: Dict = None) -> Any:
        """讀取資料"""
        pass
    
    @abstractmethod
    async def write(self, resource: str, data: Any) -> bool:
        """寫入資料"""
        pass
    
    @abstractmethod
    async def query(self, query: str, params: Dict = None) -> List[Dict]:
        """查詢資料"""
        pass

class RESTDataAccess(DataAccessInterface):
    """REST API 資料存取"""
    
    def __init__(self, base_url: str, auth: Dict):
        self.base_url = base_url
        self.auth = auth
        self.session = None
    
    async def connect(self) -> bool:
        """建立 HTTP 連接"""
        # 實作 HTTP 連接邏輯
        pass
    
    async def read(self, resource: str, params: Dict = None) -> Any:
        """HTTP GET"""
        # 實作 GET 請求
        pass
    
    async def write(self, resource: str, data: Any) -> bool:
        """HTTP POST/PUT"""
        # 實作 POST/PUT 請求
        pass

class SOAPDataAccess(DataAccessInterface):
    """SOAP Web Services 資料存取"""
    
    def __init__(self, wsdl_url: str, auth: Dict):
        self.wsdl_url = wsdl_url
        self.auth = auth
        self.client = None
    
    async def connect(self) -> bool:
        """建立 SOAP 連接"""
        # 實作 SOAP 連接邏輯
        pass

class ODataAccess(DataAccessInterface):
    """OData 資料存取 (Microsoft Dynamics)"""
    
    def __init__(self, service_url: str, auth: Dict):
        self.service_url = service_url
        self.auth = auth
    
    async def query(self, query: str, params: Dict = None) -> List[Dict]:
        """OData 查詢"""
        # 實作 OData 查詢邏輯
        pass
```

### Layer 2: 資料轉換層 (Data Transformation Layer)

```python
"""
資料轉換層 - 在外部系統格式和 NDH 內部格式之間轉換
"""

from dataclasses import dataclass
from typing import Any, Dict
from enum import Enum

class DataFormat(Enum):
    """資料格式"""
    JSON = "json"
    XML = "xml"
    CSV = "csv"
    CUSTOM = "custom"

@dataclass
class TransformationRule:
    """轉換規則"""
    source_field: str           # 來源欄位
    target_field: str           # 目標欄位
    transformation: str         # 轉換函數名稱
    params: Dict = None         # 轉換參數

class DataTransformer:
    """資料轉換器"""
    
    def __init__(self):
        self.rules: List[TransformationRule] = []
        self.functions: Dict[str, callable] = {}
        
        # 註冊內建轉換函數
        self._register_builtin_functions()
    
    def add_rule(self, rule: TransformationRule):
        """新增轉換規則"""
        self.rules.append(rule)
    
    def register_function(self, name: str, func: callable):
        """註冊轉換函數"""
        self.functions[name] = func
    
    def transform(self, source_data: Dict, direction: str = "to_ndh") -> Dict:
        """
        執行資料轉換
        
        Args:
            source_data: 來源資料
            direction: 轉換方向 ("to_ndh" 或 "from_ndh")
        
        Returns:
            轉換後的資料
        """
        result = {}
        
        for rule in self.rules:
            if direction == "to_ndh":
                source_field = rule.source_field
                target_field = rule.target_field
            else:
                source_field = rule.target_field
                target_field = rule.source_field
            
            # 取得來源值
            source_value = self._get_nested_value(source_data, source_field)
            
            # 應用轉換函數
            if rule.transformation in self.functions:
                transform_func = self.functions[rule.transformation]
                target_value = transform_func(source_value, rule.params)
            else:
                target_value = source_value
            
            # 設定目標值
            self._set_nested_value(result, target_field, target_value)
        
        return result
    
    def _register_builtin_functions(self):
        """註冊內建轉換函數"""
        self.functions["identity"] = lambda x, p: x
        self.functions["to_upper"] = lambda x, p: x.upper() if isinstance(x, str) else x
        self.functions["to_lower"] = lambda x, p: x.lower() if isinstance(x, str) else x
        self.functions["multiply"] = lambda x, p: x * p.get("factor", 1)
        self.functions["add"] = lambda x, p: x + p.get("offset", 0)
        self.functions["format_date"] = lambda x, p: self._format_date(x, p)
    
    def _get_nested_value(self, data: Dict, path: str) -> Any:
        """取得巢狀欄位值"""
        keys = path.split(".")
        value = data
        for key in keys:
            if isinstance(value, dict):
                value = value.get(key)
            else:
                return None
        return value
    
    def _set_nested_value(self, data: Dict, path: str, value: Any):
        """設定巢狀欄位值"""
        keys = path.split(".")
        current = data
        for key in keys[:-1]:
            if key not in current:
                current[key] = {}
            current = current[key]
        current[keys[-1]] = value
    
    def _format_date(self, date_value: Any, params: Dict) -> str:
        """格式化日期"""
        if isinstance(date_value, datetime):
            format_str = params.get("format", "%Y-%m-%d %H:%M:%S")
            return date_value.strftime(format_str)
        return str(date_value)

# 使用範例
transformer = DataTransformer()

# 新增轉換規則: ERP 工單 -> NDH 工單
transformer.add_rule(TransformationRule(
    source_field="ProductionOrder.OrderNumber",
    target_field="work_order.id",
    transformation="identity"
))

transformer.add_rule(TransformationRule(
    source_field="ProductionOrder.Quantity",
    target_field="work_order.quantity",
    transformation="identity"
))

transformer.add_rule(TransformationRule(
    source_field="ProductionOrder.PlannedStartDate",
    target_field="work_order.scheduled_start",
    transformation="format_date",
    params={"format": "%Y-%m-%dT%H:%M:%SZ"}
))

# 執行轉換
erp_data = {
    "ProductionOrder": {
        "OrderNumber": "PO-2025-001",
        "Quantity": 1000,
        "PlannedStartDate": datetime(2025, 1, 15, 8, 0, 0)
    }
}

ndh_data = transformer.transform(erp_data, direction="to_ndh")
```

### Layer 3: 業務邏輯層 (Business Logic Layer)

```python
"""
業務邏輯層 - 處理業務規則和工作流
"""

from enum import Enum
from typing import List, Dict, Optional, Callable
from dataclasses import dataclass
from datetime import datetime

class WorkOrderStatus(Enum):
    """工單狀態"""
    CREATED = "created"
    RELEASED = "released"
    IN_PROGRESS = "in_progress"
    PAUSED = "paused"
    COMPLETED = "completed"
    CANCELLED = "cancelled"

class EventType(Enum):
    """事件類型"""
    WORK_ORDER_CREATED = "work_order.created"
    WORK_ORDER_RELEASED = "work_order.released"
    WORK_ORDER_STARTED = "work_order.started"
    WORK_ORDER_COMPLETED = "work_order.completed"
    PRODUCTION_REPORTED = "production.reported"
    MATERIAL_CONSUMED = "material.consumed"
    QUALITY_CHECKED = "quality.checked"

@dataclass
class WorkOrder:
    """工單"""
    work_order_id: str
    product_id: str
    quantity: int
    scheduled_start: datetime
    scheduled_end: datetime
    status: WorkOrderStatus
    actual_start: Optional[datetime] = None
    actual_end: Optional[datetime] = None
    completed_quantity: int = 0
    
    # ERP 參考
    erp_order_id: str = None
    
    # MES 參考
    mes_job_id: str = None

@dataclass
class ProductionEvent:
    """生產事件"""
    event_id: str
    event_type: EventType
    work_order_id: str
    timestamp: datetime
    data: Dict
    source: str  # "ERP", "MES", "NDH"

class WorkOrderManager:
    """工單管理器"""
    
    def __init__(self):
        self.work_orders: Dict[str, WorkOrder] = {}
        self.event_handlers: Dict[EventType, List[Callable]] = {}
    
    def create_work_order(self, work_order: WorkOrder) -> bool:
        """建立工單"""
        if work_order.work_order_id in self.work_orders:
            return False
        
        self.work_orders[work_order.work_order_id] = work_order
        
        # 發布事件
        self._publish_event(ProductionEvent(
            event_id=f"evt_{work_order.work_order_id}_{datetime.now().timestamp()}",
            event_type=EventType.WORK_ORDER_CREATED,
            work_order_id=work_order.work_order_id,
            timestamp=datetime.now(),
            data={"work_order": work_order.__dict__},
            source="NDH"
        ))
        
        return True
    
    def release_work_order(self, work_order_id: str) -> bool:
        """釋放工單 (從 ERP 到 MES)"""
        work_order = self.work_orders.get(work_order_id)
        if not work_order:
            return False
        
        if work_order.status != WorkOrderStatus.CREATED:
            return False
        
        work_order.status = WorkOrderStatus.RELEASED
        
        # 發布事件
        self._publish_event(ProductionEvent(
            event_id=f"evt_{work_order_id}_released",
            event_type=EventType.WORK_ORDER_RELEASED,
            work_order_id=work_order_id,
            timestamp=datetime.now(),
            data={"work_order": work_order.__dict__},
            source="NDH"
        ))
        
        return True
    
    def start_work_order(self, work_order_id: str) -> bool:
        """開始工單"""
        work_order = self.work_orders.get(work_order_id)
        if not work_order:
            return False
        
        if work_order.status != WorkOrderStatus.RELEASED:
            return False
        
        work_order.status = WorkOrderStatus.IN_PROGRESS
        work_order.actual_start = datetime.now()
        
        # 發布事件
        self._publish_event(ProductionEvent(
            event_id=f"evt_{work_order_id}_started",
            event_type=EventType.WORK_ORDER_STARTED,
            work_order_id=work_order_id,
            timestamp=datetime.now(),
            data={
                "work_order_id": work_order_id,
                "actual_start": work_order.actual_start.isoformat()
            },
            source="NDH"
        ))
        
        return True
    
    def report_production(
        self,
        work_order_id: str,
        quantity: int,
        timestamp: datetime = None
    ) -> bool:
        """報告生產數量"""
        work_order = self.work_orders.get(work_order_id)
        if not work_order:
            return False
        
        if work_order.status != WorkOrderStatus.IN_PROGRESS:
            return False
        
        work_order.completed_quantity += quantity
        
        # 發布事件
        self._publish_event(ProductionEvent(
            event_id=f"evt_{work_order_id}_production_{datetime.now().timestamp()}",
            event_type=EventType.PRODUCTION_REPORTED,
            work_order_id=work_order_id,
            timestamp=timestamp or datetime.now(),
            data={
                "work_order_id": work_order_id,
                "quantity": quantity,
                "total_completed": work_order.completed_quantity
            },
            source="NDH"
        ))
        
        # 檢查是否完成
        if work_order.completed_quantity >= work_order.quantity:
            self.complete_work_order(work_order_id)
        
        return True
    
    def complete_work_order(self, work_order_id: str) -> bool:
        """完成工單"""
        work_order = self.work_orders.get(work_order_id)
        if not work_order:
            return False
        
        work_order.status = WorkOrderStatus.COMPLETED
        work_order.actual_end = datetime.now()
        
        # 發布事件
        self._publish_event(ProductionEvent(
            event_id=f"evt_{work_order_id}_completed",
            event_type=EventType.WORK_ORDER_COMPLETED,
            work_order_id=work_order_id,
            timestamp=datetime.now(),
            data={
                "work_order_id": work_order_id,
                "completed_quantity": work_order.completed_quantity,
                "actual_start": work_order.actual_start.isoformat(),
                "actual_end": work_order.actual_end.isoformat()
            },
            source="NDH"
        ))
        
        return True
    
    def register_event_handler(
        self,
        event_type: EventType,
        handler: Callable[[ProductionEvent], None]
    ):
        """註冊事件處理器"""
        if event_type not in self.event_handlers:
            self.event_handlers[event_type] = []
        self.event_handlers[event_type].append(handler)
    
    def _publish_event(self, event: ProductionEvent):
        """發布事件"""
        if event.event_type in self.event_handlers:
            for handler in self.event_handlers[event.event_type]:
                try:
                    handler(event)
                except Exception as e:
                    print(f"Error in event handler: {e}")
```

### Layer 4: 整合協調層 (Integration Orchestration Layer)

```python
"""
整合協調層 - 協調 ERP、MES 和 NDH 之間的資料流
"""

class IntegrationOrchestrator:
    """整合協調器"""
    
    def __init__(
        self,
        erp_connector,
        mes_connector,
        ndh_broker,
        work_order_manager: WorkOrderManager
    ):
        self.erp_connector = erp_connector
        self.mes_connector = mes_connector
        self.ndh_broker = ndh_broker
        self.work_order_manager = work_order_manager
        
        # 註冊事件處理器
        self._register_event_handlers()
    
    def _register_event_handlers(self):
        """註冊事件處理器"""
        
        # 工單建立 -> 發送到 MES
        self.work_order_manager.register_event_handler(
            EventType.WORK_ORDER_RELEASED,
            self._handle_work_order_released
        )
        
        # 工單完成 -> 回報到 ERP
        self.work_order_manager.register_event_handler(
            EventType.WORK_ORDER_COMPLETED,
            self._handle_work_order_completed
        )
        
        # 生產報告 -> 發送到 MES 和 ERP
        self.work_order_manager.register_event_handler(
            EventType.PRODUCTION_REPORTED,
            self._handle_production_reported
        )
    
    async def _handle_work_order_released(self, event: ProductionEvent):
        """處理工單釋放事件"""
        work_order_data = event.data["work_order"]
        
        # 發送到 MES
        mes_job = await self.mes_connector.create_job(work_order_data)
        
        # 更新工單的 MES 參考
        work_order = self.work_order_manager.work_orders[event.work_order_id]
        work_order.mes_job_id = mes_job["job_id"]
        
        print(f"Work order {event.work_order_id} released to MES as job {mes_job['job_id']}")
    
    async def _handle_work_order_completed(self, event: ProductionEvent):
        """處理工單完成事件"""
        work_order = self.work_order_manager.work_orders[event.work_order_id]
        
        # 回報到 ERP
        await self.erp_connector.report_production_completion({
            "order_id": work_order.erp_order_id,
            "completed_quantity": work_order.completed_quantity,
            "actual_start": work_order.actual_start.isoformat(),
            "actual_end": work_order.actual_end.isoformat()
        })
        
        print(f"Work order {event.work_order_id} completion reported to ERP")
    
    async def _handle_production_reported(self, event: ProductionEvent):
        """處理生產報告事件"""
        # 發送到 MES
        await self.mes_connector.report_production(event.data)
        
        # 發送到 ERP (如果需要即時更新)
        # await self.erp_connector.update_production_progress(event.data)
        
        print(f"Production reported: {event.data['quantity']} units")
    
    async def sync_from_erp(self):
        """從 ERP 同步工單"""
        # 取得新的生產訂單
        orders = await self.erp_connector.get_production_orders(status="new")
        
        for order in orders:
            # 轉換為 NDH 工單
            work_order = WorkOrder(
                work_order_id=f"WO-{order['order_id']}",
                product_id=order['product_id'],
                quantity=order['quantity'],
                scheduled_start=order['planned_start'],
                scheduled_end=order['planned_end'],
                status=WorkOrderStatus.CREATED,
                erp_order_id=order['order_id']
            )
            
            # 建立工單
            self.work_order_manager.create_work_order(work_order)
            
            # 自動釋放工單
            self.work_order_manager.release_work_order(work_order.work_order_id)
    
    async def sync_from_mes(self):
        """從 MES 同步生產資料"""
        # 取得進行中的工單
        in_progress_orders = [
            wo for wo in self.work_order_manager.work_orders.values()
            if wo.status == WorkOrderStatus.IN_PROGRESS
        ]
        
        for work_order in in_progress_orders:
            if work_order.mes_job_id:
                # 從 MES 取得生產進度
                progress = await self.mes_connector.get_job_progress(
                    work_order.mes_job_id
                )
                
                # 更新生產數量
                new_quantity = progress["completed_quantity"] - work_order.completed_quantity
                if new_quantity > 0:
                    self.work_order_manager.report_production(
                        work_order.work_order_id,
                        new_quantity
                    )
```

## 連接器架構

### 統一連接器介面

```python
"""
統一的企業系統連接器介面
"""

from abc import ABC, abstractmethod
from typing import Dict, List, Optional, Any
from enum import Enum

class SystemType(Enum):
    """系統類型"""
    ERP = "erp"
    MES = "mes"
    SCADA = "scada"
    WMS = "wms"  # Warehouse Management System
    QMS = "qms"  # Quality Management System

class EnterpriseConnector(ABC):
    """企業系統連接器基礎類別"""
    
    def __init__(
        self,
        connector_id: str,
        system_type: SystemType,
        config: Dict
    ):
        self.connector_id = connector_id
        self.system_type = system_type
        self.config = config
        self.connected = False
    
    @abstractmethod
    async def connect(self) -> bool:
        """建立連接"""
        pass
    
    @abstractmethod
    async def disconnect(self):
        """斷開連接"""
        pass
    
    @abstractmethod
    async def test_connection(self) -> bool:
        """測試連接"""
        pass
    
    # 工單相關
    @abstractmethod
    async def get_production_orders(
        self,
        status: Optional[str] = None,
        start_date: Optional[datetime] = None
    ) -> List[Dict]:
        """取得生產訂單"""
        pass
    
    @abstractmethod
    async def create_production_order(self, order_data: Dict) -> Dict:
        """建立生產訂單"""
        pass
    
    @abstractmethod
    async def update_production_order(
        self,
        order_id: str,
        updates: Dict
    ) -> bool:
        """更新生產訂單"""
        pass
    
    # 物料相關
    @abstractmethod
    async def get_material_stock(self, material_id: str) -> Dict:
        """取得物料庫存"""
        pass
    
    @abstractmethod
    async def report_material_consumption(
        self,
        consumption_data: Dict
    ) -> bool:
        """報告物料消耗"""
        pass
    
    # 生產相關
    @abstractmethod
    async def report_production_completion(
        self,
        completion_data: Dict
    ) -> bool:
        """報告生產完成"""
        pass
    
    @abstractmethod
    async def report_quality_data(self, quality_data: Dict) -> bool:
        """報告品質資料"""
        pass
```

(續下一部分...)

---

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**最後更新**: 2025-01-10

