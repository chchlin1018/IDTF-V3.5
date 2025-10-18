

# NDH-MES 雙向連接器虛擬介面實施指南

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件提供了 NDH-MES 雙向連接器虛擬介面的詳細實施指南,包括 API 規格、數據模型和參考程式碼。

## 2. API 規格

### 2.1 虛擬 MES API

| 端點 | 方法 | 用途 |
|---|---|---|
| `/api/virtual-mes/work-orders` | GET | 查詢工單 |
| `/api/virtual-mes/work-orders` | POST | 創建工單 |
| `/api/virtual-mes/work-orders/{id}` | GET | 查詢單一工單 |
| `/api/virtual-mes/work-orders/{id}` | PUT | 更新工單 |
| `/api/virtual-mes/equipments/{id}/status` | GET | 查詢設備狀態 |
| `/api/virtual-mes/equipments/{id}/oee` | GET | 查詢設備 OEE |

### 2.2 事件

| 事件名稱 | 描述 | 來源 |
|---|---|---|
| `WorkOrderCreated` | 工單已創建 | MES -> NDH |
| `WorkOrderStatusChanged` | 工單狀態已變更 | MES -> NDH |
| `EquipmentStatusChanged` | 設備狀態已變更 | MES -> NDH |
| `ProductionDataReported` | 生產數據已回報 | NDH -> MES |

---

## 3. 數據模型 (通用)

### 3.1 WorkOrder

```json
{
  "id": "string",
  "productId": "string",
  "quantity": "integer",
  "status": "string (Created, InProgress, Completed, Canceled)",
  "startTime": "datetime",
  "endTime": "datetime"
}
```

### 3.2 Equipment

```json
{
  "id": "string",
  "name": "string",
  "status": "string (Running, Idle, Down, Maintenance)",
  "oee": "float"
}
```

---

## 4. 參考程式碼

### 4.1 虛擬 MES API (FastAPI)

```python
from fastapi import FastAPI
from pydantic import BaseModel

app = FastAPI()

class WorkOrder(BaseModel):
    id: str
    productId: str
    quantity: int

@app.post("/api/virtual-mes/work-orders")
def create_work_order(work_order: WorkOrder):
    # 透過 Adapter Manager 呼叫對應的 MES 適配器
    adapter = adapter_manager.get_adapter("aveva")
    adapter.create_work_order(work_order)
    return {"status": "success"}
```

### 4.2 AVEVA MES 適配器

```python
class AvevaMESAdapter:
    def create_work_order(self, work_order):
        # 將通用數據模型轉換為 AVEVA MES 的數據模型
        aveva_work_order = transform_to_aveva_format(work_order)
        # 呼叫 AVEVA MES API
        requests.post("http://aveva/api/work-orders", json=aveva_work_order)

    def subscribe_events(self, event_dispatcher):
        # 訂閱 AVEVA MES 的事件
        # ...
        # 將事件發布到事件分派器
        event_dispatcher.dispatch(event)
```

### 4.3 FactoryTalk MES 適配器

```python
class FactoryTalkMESAdapter:
    def create_work_order(self, work_order):
        # 將通用數據模型轉換為 FactoryTalk MES 的數據模型
        factorytalk_work_order = transform_to_factorytalk_format(work_order)
        # 呼叫 FactoryTalk MES API
        requests.post("http://factorytalk/api/work-orders", json=factorytalk_work_order)

    def subscribe_events(self, event_dispatcher):
        # 訂閱 FactoryTalk MES 的事件 (OPC UA)
        # ...
        # 將事件發布到事件分派器
        event_dispatcher.dispatch(event)
```

---

## 5. 預期結果

- **統一介面**: 提供一個統一的 API 和事件模型,簡化整合。
- **快速擴展**: 透過適配器模式,可以快速支援新的 MES 品牌。
- **降低成本**: 大大降低整合不同 MES 系統的成本和時間。

---

