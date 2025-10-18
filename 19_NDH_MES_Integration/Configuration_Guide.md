

# NDH-MES 整合配置管理與參考實現

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件提供了 NDH-MES 整合的詳細配置管理與參考實現,包括配置檔案、參考程式碼和測試案例。

## 2. 配置管理

### 2.1 config.yaml

```yaml
mes:
  enabled: true
  adapters:
    - name: "aveva"
      enabled: true
      config:
        api_url: "http://aveva/api"
        event_broker_url: "amqp://guest:guest@rabbitmq:5672/"
    - name: "factorytalk"
      enabled: true
      config:
        api_url: "http://factorytalk/api"
        opc_ua_url: "opc.tcp://factorytalk:4840"
```

### 2.2 適配器管理器 (Adapter Manager)

```python
import yaml

class AdapterManager:
    def __init__(self, config_path):
        with open(config_path, "r") as f:
            self.config = yaml.safe_load(f)
        self.adapters = {}

    def load_adapters(self):
        for adapter_config in self.config["mes"]["adapters"]:
            if adapter_config["enabled"]:
                adapter_name = adapter_config["name"]
                adapter_class = self.get_adapter_class(adapter_name)
                self.adapters[adapter_name] = adapter_class(adapter_config["config"])

    def get_adapter_class(self, name):
        if name == "aveva":
            from .adapters.aveva import AvevaMESAdapter
            return AvevaMESAdapter
        elif name == "factorytalk":
            from .adapters.factorytalk import FactoryTalkMESAdapter
            return FactoryTalkMESAdapter
```

---

## 3. 參考實現

### 3.1 虛擬 MES API (FastAPI)

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

### 3.2 AVEVA MES 適配器

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

### 3.3 FactoryTalk MES 適配器

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

## 4. 測試案例

### 4.1 創建工單

1. 呼叫虛擬 MES API `POST /api/virtual-mes/work-orders`。
2. 驗證對應的 MES 適配器被呼叫。
3. 驗證工單在 MES 系統中被創建。

### 4.2 訂閱事件

1. 在 MES 系統中觸發一個事件 (如: 工單狀態變更)。
2. 驗證對應的 MES 適配器接收到事件。
3. 驗證事件被發布到 NDH 的訊息佇列。
4. 驗證 Asset Servant 接收到事件並更新資產狀態。

---

