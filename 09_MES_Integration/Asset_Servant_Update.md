# Asset Servant 更新 (MES 整合)

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 2.0

---

## 1. 簡介

為了支援與 MES 的整合,Asset Servant 需要進行擴展,使其能夠處理來自 MES 的事件並更新自身的狀態。

## 2. 架構變更

Asset Servant 將增加一個 Message Queue 的訂閱者 (Subscriber),用於監聽 MES 相關的事件。

```python
import pika

class AssetServant:
    def __init__(self, iadl_definition):
        self.iadl = iadl_definition
        self.production_data = {}
        
        # 初始化 Message Queue 連線
        self._init_message_queue()

    def _init_message_queue(self):
        connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
        self.channel = connection.channel()
        
        # 宣告交換機
        self.channel.exchange_declare(exchange='mes_events', exchange_type='topic')
        
        # 建立專屬佇列
        result = self.channel.queue_declare(queue='', exclusive=True)
        self.queue_name = result.method.queue
        
        # 綁定佇列到交換機
        self.channel.queue_bind(exchange='mes_events', queue=self.queue_name, routing_key='mes.workorder.events')
        
        # 設定消費者
        self.channel.basic_consume(queue=self.queue_name, on_message_callback=self.handle_mes_event, auto_ack=True)
        
        # 在背景執行緒中開始消費
        # ...

    def handle_mes_event(self, ch, method, properties, body):
        event = json.loads(body)
        
        if event['eventType'] == 'WorkOrderStarted':
            if event['payload']['equipmentId'] == self.iadl['id']:
                self.production_data['current_work_order'] = event['payload']['workOrderId']
                self.production_data['current_product_id'] = event['payload']['productId']
                self.production_data['wip_status'] = 'Processing'
                
                # 透過 NDH API 更新 IADL
                self._update_iadl_production_data()

    def _update_iadl_production_data(self):
        # ... 呼叫 PUT /api/v1/assets/{assetId}/production ...
        pass
```

## 3. IADL `production` 區塊的處理

Asset Servant 在初始化時,會讀取 IADL 中的 `production` 區塊,並將其作為初始狀態。

當收到來自 MES 的事件時,Asset Servant 會更新內部的 `production_data` 字典,並可選擇性地將其寫回 IADL (透過 NDH API),以實現狀態的持久化。

---

