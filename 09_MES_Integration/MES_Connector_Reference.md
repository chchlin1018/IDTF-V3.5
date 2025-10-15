# MES Connector 參考實現

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件提供了一個 MES Connector 的參考實現,用於將 MES 系統的事件發布到 Message Queue,並從 Message Queue 訂閱來自 IDTF 的事件。

## 2. 實現 (Python + RabbitMQ)

```python
import pika
import json
import time

class MESConnector:
    def __init__(self, mes_api):
        self.mes_api = mes_api
        
        # 初始化 Message Queue 連線
        self.connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
        self.channel = self.connection.channel()
        
        # 宣告交換機
        self.channel.exchange_declare(exchange='mes_events', exchange_type='topic')
        self.channel.exchange_declare(exchange='idtf_events', exchange_type='topic')

    def publish_work_order_event(self, event_type, work_order_data):
        event = {
            "eventType": event_type,
            "timestamp": time.time(),
            "payload": work_order_data
        }
        
        self.channel.basic_publish(
            exchange='mes_events',
            routing_key='mes.workorder.events',
            body=json.dumps(event)
        )

    def subscribe_to_idtf_events(self):
        result = self.channel.queue_declare(queue='', exclusive=True)
        queue_name = result.method.queue
        
        self.channel.queue_bind(exchange='idtf_events', queue=queue_name, routing_key='idtf.equipment.status')
        
        self.channel.basic_consume(queue=queue_name, on_message_callback=self.handle_idtf_event, auto_ack=True)
        
        # 在背景執行緒中開始消費
        # ...

    def handle_idtf_event(self, ch, method, properties, body):
        event = json.loads(body)
        
        if event['eventType'] == 'EquipmentStatusChanged':
            # 更新 MES 中的設備狀態
            self.mes_api.update_equipment_status(event['payload']['equipmentId'], event['payload']['status'])

# 範例: 模擬從 MES 獲取工單事件並發布
if __name__ == '__main__':
    # 假設 mes_api 是一個與 MES 系統互動的物件
    mes_api = ... 
    connector = MESConnector(mes_api)
    
    # 模擬一個工單開始事件
    work_order_data = {
        "workOrderId": "WO-12345",
        "productId": "PROD-XYZ",
        "quantity": 1000,
        "equipmentId": "EQ-001"
    }
    connector.publish_work_order_event('WorkOrderStarted', work_order_data)
```

---

