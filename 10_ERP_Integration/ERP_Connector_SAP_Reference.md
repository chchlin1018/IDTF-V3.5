# ERP Connector 參考實現 (SAP S/4HANA)

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件提供了一個 ERP Connector 的參考實現,用於將 SAP S/4HANA 系統與 NDH 進行整合。

## 2. SAP 整合方式

### 2.1 SAP API 選項

- **OData Services**: RESTful API,適合現代化整合
- **BAPI (Business API)**: SAP 的遠程函數調用,功能強大但較複雜
- **IDoc (Intermediate Document)**: SAP 的標準數據交換格式
- **SAP PI/PO**: SAP 的中間件平台

本參考實現使用 **OData Services** 和 **BAPI**。

---

## 3. 實現 (Python)

### 3.1 安裝依賴

```bash
pip install pyrfc  # SAP NetWeaver RFC SDK
pip install requests  # HTTP 客戶端
pip install pika  # RabbitMQ 客戶端
```

### 3.2 SAP Connector 實現

```python
import requests
import json
import pika
from pyrfc import Connection
from datetime import datetime

class SAPConnector:
    def __init__(self, sap_config, rabbitmq_config):
        """
        初始化 SAP Connector
        
        Args:
            sap_config: SAP 連線配置
            rabbitmq_config: RabbitMQ 連線配置
        """
        self.sap_config = sap_config
        self.rabbitmq_config = rabbitmq_config
        
        # 初始化 SAP RFC 連線
        self.sap_conn = Connection(
            ashost=sap_config['host'],
            sysnr=sap_config['system_number'],
            client=sap_config['client'],
            user=sap_config['user'],
            passwd=sap_config['password']
        )
        
        # 初始化 RabbitMQ 連線
        self.mq_conn = pika.BlockingConnection(
            pika.ConnectionParameters(rabbitmq_config['host'])
        )
        self.channel = self.mq_conn.channel()
        
        # 宣告交換機
        self.channel.exchange_declare(exchange='erp_events', exchange_type='topic')
        self.channel.exchange_declare(exchange='ndh_events', exchange_type='topic')

    def fetch_production_orders(self, plant, work_center):
        """
        從 SAP 獲取生產工單
        
        Args:
            plant: 工廠代碼
            work_center: 工作中心
        
        Returns:
            生產工單列表
        """
        # 呼叫 SAP BAPI 獲取生產工單
        result = self.sap_conn.call(
            'BAPI_PRODORD_GET_LIST',
            PLANT=plant,
            WORK_CENTER=work_center,
            SELECTION_RULE='2'  # 2 = Released orders
        )
        
        production_orders = []
        for order in result.get('PRODUCTION_ORDERS', []):
            production_orders.append({
                'productionOrderId': order['ORDER_NUMBER'],
                'productId': order['MATERIAL'],
                'quantity': order['TARGET_QUANTITY'],
                'plant': order['PLANT'],
                'workCenter': order['WORK_CENTER'],
                'startDate': order['BASIC_START_DATE'],
                'endDate': order['BASIC_END_DATE']
            })
        
        return production_orders

    def publish_production_order_to_ndh(self, production_order, equipment_id):
        """
        將生產工單發布到 NDH
        
        Args:
            production_order: 生產工單數據
            equipment_id: 設備 ID
        """
        event = {
            'eventType': 'ProductionOrderCreated',
            'timestamp': datetime.now().isoformat(),
            'payload': {
                'productionOrderId': production_order['productionOrderId'],
                'productId': production_order['productId'],
                'quantity': production_order['quantity'],
                'equipmentId': equipment_id,
                'startDate': production_order['startDate'],
                'endDate': production_order['endDate']
            }
        }
        
        self.channel.basic_publish(
            exchange='erp_events',
            routing_key='erp.productionorder.created',
            body=json.dumps(event)
        )
        
        print(f"Published production order {production_order['productionOrderId']} to NDH")

    def confirm_production_to_sap(self, confirmation):
        """
        將生產確認回傳到 SAP
        
        Args:
            confirmation: 生產確認數據
        """
        # 準備 BAPI 參數
        order_number = confirmation['productionOrderId']
        
        # 呼叫 SAP BAPI 進行生產確認
        result = self.sap_conn.call(
            'BAPI_PRODORDCONF_CREATE_TT',
            POSTG_DATE=datetime.now().strftime('%Y%m%d'),
            DOC_DATE=datetime.now().strftime('%Y%m%d'),
            CONF_DATA={
                'ORDERID': order_number,
                'CONF_ACTIVITY': '0001',  # 確認活動
                'YIELD': confirmation['confirmedQuantity'],
                'SCRAP': confirmation['scrapQuantity'],
                'CONF_TEXT': f"Confirmed by NDH at {datetime.now()}"
            },
            TIME_DATA={
                'EXEC_START_DATE': confirmation['actualStartTime'][:10].replace('-', ''),
                'EXEC_START_TIME': confirmation['actualStartTime'][11:19].replace(':', ''),
                'EXEC_END_DATE': confirmation['actualEndTime'][:10].replace('-', ''),
                'EXEC_END_TIME': confirmation['actualEndTime'][11:19].replace(':', '')
            }
        )
        
        # 提交事務
        self.sap_conn.call('BAPI_TRANSACTION_COMMIT', WAIT='X')
        
        print(f"Production confirmation {confirmation['confirmationId']} sent to SAP")
        print(f"SAP Response: {result}")

    def subscribe_to_ndh_events(self):
        """訂閱來自 NDH 的事件"""
        result = self.channel.queue_declare(queue='', exclusive=True)
        queue_name = result.method.queue
        
        # 綁定生產確認事件
        self.channel.queue_bind(
            exchange='ndh_events',
            queue=queue_name,
            routing_key='ndh.production.confirmation'
        )
        
        # 綁定設備 KPI 事件
        self.channel.queue_bind(
            exchange='ndh_events',
            queue=queue_name,
            routing_key='ndh.equipment.kpis'
        )
        
        # 設定消費者
        self.channel.basic_consume(
            queue=queue_name,
            on_message_callback=self.handle_ndh_event,
            auto_ack=True
        )
        
        print("Subscribed to NDH events. Waiting for messages...")
        self.channel.start_consuming()

    def handle_ndh_event(self, ch, method, properties, body):
        """處理來自 NDH 的事件"""
        event = json.loads(body)
        
        if method.routing_key == 'ndh.production.confirmation':
            # 處理生產確認
            self.confirm_production_to_sap(event)
        
        elif method.routing_key == 'ndh.equipment.kpis':
            # 處理設備 KPI (可選擇性地寫入 SAP)
            self.update_equipment_kpi_in_sap(event)

    def update_equipment_kpi_in_sap(self, kpi_event):
        """
        將設備 KPI 更新到 SAP (可選)
        
        Args:
            kpi_event: KPI 事件數據
        """
        # 這裡可以選擇將 KPI 數據寫入 SAP 的自定義表或 BW
        print(f"Received KPI update for {kpi_event['assetId']}: {kpi_event['kpi']} = {kpi_event['value']}")
        # 實際實現取決於 SAP 的配置

    def sync_equipment_master_data(self):
        """從 SAP 同步設備主數據到 NDH"""
        # 呼叫 SAP BAPI 或 OData 獲取設備主數據
        # 這裡使用 OData 作為範例
        odata_url = f"{self.sap_config['odata_base_url']}/API_EQUIPMENT/Equipment"
        
        response = requests.get(
            odata_url,
            auth=(self.sap_config['user'], self.sap_config['password']),
            headers={'Accept': 'application/json'}
        )
        
        if response.status_code == 200:
            equipment_list = response.json()['d']['results']
            
            for equipment in equipment_list:
                # 發布設備主數據更新事件到 NDH
                event = {
                    'eventType': 'EquipmentMasterUpdated',
                    'timestamp': datetime.now().isoformat(),
                    'payload': {
                        'equipmentId': equipment['Equipment'],
                        'description': equipment['EquipmentName'],
                        'costCenter': equipment['CostCenter'],
                        'workCenter': equipment['WorkCenter'],
                        'assetClass': equipment['TechnicalObjectType']
                    }
                }
                
                self.channel.basic_publish(
                    exchange='erp_events',
                    routing_key='erp.equipment.master.updated',
                    body=json.dumps(event)
                )
            
            print(f"Synced {len(equipment_list)} equipment master data records to NDH")
        else:
            print(f"Failed to fetch equipment master data: {response.status_code}")

    def close(self):
        """關閉連線"""
        self.sap_conn.close()
        self.mq_conn.close()
```

---

## 4. 使用範例

```python
# SAP 配置
sap_config = {
    'host': 'sap-server.company.com',
    'system_number': '00',
    'client': '100',
    'user': 'INTEGRATION_USER',
    'password': 'password',
    'odata_base_url': 'https://sap-server.company.com:44300/sap/opu/odata/sap'
}

# RabbitMQ 配置
rabbitmq_config = {
    'host': 'localhost'
}

# 初始化 SAP Connector
connector = SAPConnector(sap_config, rabbitmq_config)

# 1. 同步設備主數據
connector.sync_equipment_master_data()

# 2. 獲取生產工單並發布到 NDH
production_orders = connector.fetch_production_orders(plant='1000', work_center='WC-DB-01')
for order in production_orders:
    connector.publish_production_order_to_ndh(order, equipment_id='EQ-002')

# 3. 訂閱 NDH 事件 (阻塞式,持續運行)
connector.subscribe_to_ndh_events()

# 關閉連線 (在程式結束時)
# connector.close()
```

---

## 5. 部署建議

### 5.1 容器化部署

```dockerfile
FROM python:3.9

# 安裝 SAP NetWeaver RFC SDK
RUN apt-get update && apt-get install -y wget unzip
RUN wget https://support.sap.com/swdc/nwrfcsdk/linux/nwrfc750P_8-70002752.zip
RUN unzip nwrfc750P_8-70002752.zip -d /usr/local/sap
ENV LD_LIBRARY_PATH=/usr/local/sap/nwrfcsdk/lib:$LD_LIBRARY_PATH

# 安裝 Python 依賴
COPY requirements.txt .
RUN pip install -r requirements.txt

# 複製應用程式
COPY sap_connector.py .

CMD ["python", "sap_connector.py"]
```

### 5.2 監控與日誌

- 使用 Prometheus 監控 Connector 的運行狀態
- 使用 ELK Stack 收集和分析日誌
- 設定告警機制,當 SAP 連線失敗或 Message Queue 堆積時發送通知

---

## 6. 安全性考量

- **加密通訊**: 使用 TLS/SSL 加密與 SAP 的通訊
- **憑證管理**: 使用環境變數或密鑰管理服務 (如 HashiCorp Vault) 儲存 SAP 憑證
- **最小權限原則**: SAP 整合用戶只授予必要的權限

---

