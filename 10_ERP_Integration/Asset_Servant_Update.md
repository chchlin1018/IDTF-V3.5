# Asset Servant 更新 (ERP 整合)

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

為了支援與 ERP 的整合,Asset Servant 需要進行擴展,使其能夠處理來自 ERP 的事件、計算 KPI,並生成生產確認數據。

## 2. 架構變更

Asset Servant 將增加以下功能模組:

1. **ERP 事件訂閱者**: 監聽來自 ERP 的生產工單事件
2. **KPI 計算引擎**: 計算 OEE、MTBF、MTTR 等 KPI
3. **生產確認生成器**: 匯總產量、工時、物料消耗等數據

## 3. 實現

### 3.1 ERP 事件訂閱

```python
import pika
import json

class AssetServant:
    def __init__(self, iadl_definition):
        self.iadl = iadl_definition
        self.production_data = {}
        self.erp_data = {}
        
        # 初始化 Message Queue 連線
        self._init_message_queue()
        
        # 載入 ERP 資訊
        if 'erp' in iadl_definition:
            self.erp_data = iadl_definition['erp']

    def _init_message_queue(self):
        connection = pika.BlockingConnection(pika.ConnectionParameters('localhost'))
        self.channel = connection.channel()
        
        # 宣告交換機
        self.channel.exchange_declare(exchange='erp_events', exchange_type='topic')
        
        # 建立專屬佇列
        result = self.channel.queue_declare(queue='', exclusive=True)
        self.queue_name = result.method.queue
        
        # 綁定佇列到交換機
        self.channel.queue_bind(
            exchange='erp_events',
            queue=self.queue_name,
            routing_key='erp.productionorder.#'
        )
        
        # 設定消費者
        self.channel.basic_consume(
            queue=self.queue_name,
            on_message_callback=self.handle_erp_event,
            auto_ack=True
        )

    def handle_erp_event(self, ch, method, properties, body):
        """處理來自 ERP 的事件"""
        event = json.loads(body)
        
        if event['eventType'] == 'ProductionOrderCreated':
            payload = event['payload']
            if payload['equipmentId'] == self.iadl['id']:
                # 更新生產資訊
                self.production_data['current_production_order'] = payload['productionOrderId']
                self.production_data['target_quantity'] = payload['quantity']
                self.production_data['start_date'] = payload['startDate']
                self.production_data['end_date'] = payload['endDate']
                
                print(f"Asset {self.iadl['id']} received production order: {payload['productionOrderId']}")
```

### 3.2 KPI 計算引擎

```python
from datetime import datetime, timedelta

class KPICalculator:
    def __init__(self, asset_servant):
        self.asset_servant = asset_servant
        self.tsdb = asset_servant.tsdb  # 時序資料庫連線

    def calculate_oee(self, start_time, end_time):
        """計算 OEE (Overall Equipment Effectiveness)"""
        availability = self.calculate_availability(start_time, end_time)
        performance = self.calculate_performance(start_time, end_time)
        quality = self.calculate_quality(start_time, end_time)
        
        oee = availability * performance * quality
        
        return {
            'oee': oee,
            'availability': availability,
            'performance': performance,
            'quality': quality,
            'timestamp': datetime.now().isoformat()
        }

    def calculate_availability(self, start_time, end_time):
        """計算可用率 = 運行時間 / 計劃生產時間"""
        total_time = (end_time - start_time).total_seconds()
        
        # 從時序資料庫查詢停機時間
        downtime_query = f"""
            SELECT SUM(duration) as total_downtime
            FROM equipment_status
            WHERE asset_id = '{self.asset_servant.iadl['id']}'
            AND status = 'Down'
            AND timestamp BETWEEN '{start_time}' AND '{end_time}'
        """
        downtime = self.tsdb.query(downtime_query)[0]['total_downtime'] or 0
        
        running_time = total_time - downtime
        availability = running_time / total_time if total_time > 0 else 0
        
        return availability

    def calculate_performance(self, start_time, end_time):
        """計算效能 = (實際產量 / 理想產量)"""
        # 從時序資料庫查詢實際產量
        actual_output_query = f"""
            SELECT SUM(output) as total_output
            FROM production_data
            WHERE asset_id = '{self.asset_servant.iadl['id']}'
            AND timestamp BETWEEN '{start_time}' AND '{end_time}'
        """
        actual_output = self.tsdb.query(actual_output_query)[0]['total_output'] or 0
        
        # 計算理想產量 (基於設備的額定產能)
        rated_capacity = self.asset_servant.iadl.get('throughput', 1000)  # units/hour
        total_hours = (end_time - start_time).total_seconds() / 3600
        ideal_output = rated_capacity * total_hours
        
        performance = actual_output / ideal_output if ideal_output > 0 else 0
        
        return performance

    def calculate_quality(self, start_time, end_time):
        """計算品質率 = 良品數量 / 總產量"""
        # 從時序資料庫查詢良品和不良品數量
        quality_query = f"""
            SELECT
                SUM(good_parts) as good_parts,
                SUM(total_parts) as total_parts
            FROM production_data
            WHERE asset_id = '{self.asset_servant.iadl['id']}'
            AND timestamp BETWEEN '{start_time}' AND '{end_time}'
        """
        result = self.tsdb.query(quality_query)[0]
        good_parts = result['good_parts'] or 0
        total_parts = result['total_parts'] or 0
        
        quality = good_parts / total_parts if total_parts > 0 else 0
        
        return quality

    def calculate_mtbf(self, start_time, end_time):
        """計算 MTBF (Mean Time Between Failures)"""
        # 查詢故障次數
        failure_query = f"""
            SELECT COUNT(*) as failure_count
            FROM equipment_failures
            WHERE asset_id = '{self.asset_servant.iadl['id']}'
            AND timestamp BETWEEN '{start_time}' AND '{end_time}'
        """
        failure_count = self.tsdb.query(failure_query)[0]['failure_count'] or 0
        
        # 計算運行時間
        availability = self.calculate_availability(start_time, end_time)
        total_time = (end_time - start_time).total_seconds()
        running_time = total_time * availability
        
        mtbf = running_time / failure_count if failure_count > 0 else float('inf')
        
        return mtbf / 3600  # 轉換為小時

    def calculate_mttr(self, start_time, end_time):
        """計算 MTTR (Mean Time To Repair)"""
        # 查詢所有維修時間
        repair_query = f"""
            SELECT AVG(repair_duration) as avg_repair_time
            FROM equipment_repairs
            WHERE asset_id = '{self.asset_servant.iadl['id']}'
            AND timestamp BETWEEN '{start_time}' AND '{end_time}'
        """
        avg_repair_time = self.tsdb.query(repair_query)[0]['avg_repair_time'] or 0
        
        return avg_repair_time / 3600  # 轉換為小時
```

### 3.3 生產確認生成器

```python
class ProductionConfirmationGenerator:
    def __init__(self, asset_servant):
        self.asset_servant = asset_servant
        self.tsdb = asset_servant.tsdb

    def generate_confirmation(self, production_order_id, start_time, end_time):
        """生成生產確認數據"""
        # 查詢實際產量
        output_query = f"""
            SELECT
                SUM(good_parts) as confirmed_quantity,
                SUM(scrap_parts) as scrap_quantity
            FROM production_data
            WHERE asset_id = '{self.asset_servant.iadl['id']}'
            AND production_order_id = '{production_order_id}'
            AND timestamp BETWEEN '{start_time}' AND '{end_time}'
        """
        output_result = self.tsdb.query(output_query)[0]
        
        # 查詢工時
        labor_hours = (end_time - start_time).total_seconds() / 3600
        
        # 查詢物料消耗 (假設從 MES 或其他系統獲取)
        material_consumption = self._get_material_consumption(production_order_id)
        
        # 查詢能源消耗
        energy_query = f"""
            SELECT SUM(energy_consumption) as total_energy
            FROM energy_data
            WHERE asset_id = '{self.asset_servant.iadl['id']}'
            AND timestamp BETWEEN '{start_time}' AND '{end_time}'
        """
        energy_result = self.tsdb.query(energy_query)[0]
        
        confirmation = {
            'confirmationId': f"CONF-{datetime.now().strftime('%Y%m%d%H%M%S')}",
            'productionOrderId': production_order_id,
            'equipmentId': self.asset_servant.iadl['id'],
            'confirmedQuantity': output_result['confirmed_quantity'] or 0,
            'scrapQuantity': output_result['scrap_quantity'] or 0,
            'actualStartTime': start_time.isoformat(),
            'actualEndTime': end_time.isoformat(),
            'laborHours': labor_hours,
            'materialConsumption': material_consumption,
            'energyConsumption': {
                'electricity': energy_result['total_energy'] or 0,
                'unit': 'kWh'
            }
        }
        
        # 發送到 Message Queue
        self._publish_to_erp(confirmation)
        
        return confirmation

    def _get_material_consumption(self, production_order_id):
        """獲取物料消耗數據 (從 MES 或其他系統)"""
        # 這裡簡化處理,實際應該從 MES 系統獲取
        return [
            {
                'materialId': 'MAT-001',
                'quantity': 5000,
                'unit': 'pcs'
            }
        ]

    def _publish_to_erp(self, confirmation):
        """將生產確認發送到 ERP"""
        self.asset_servant.channel.basic_publish(
            exchange='ndh_events',
            routing_key='ndh.production.confirmation',
            body=json.dumps(confirmation)
        )
        print(f"Production confirmation sent to ERP: {confirmation['confirmationId']}")
```

---

## 4. 使用範例

```python
# 初始化 Asset Servant
iadl_def = {
    'id': 'EQ-002',
    'type': 'Die Bonder',
    'throughput': 1000,  # units/hour
    'erp': {
        'equipment_id': '10001234',
        'cost_center': 'CC-FAB1-ASSEMBLY',
        'work_center': 'WC-DB-01'
    }
}

asset_servant = AssetServant(iadl_def)

# 計算 OEE
kpi_calculator = KPICalculator(asset_servant)
start_time = datetime(2025, 10, 15, 8, 0, 0)
end_time = datetime(2025, 10, 16, 17, 0, 0)
oee_result = kpi_calculator.calculate_oee(start_time, end_time)
print(f"OEE: {oee_result['oee']:.2%}")

# 生成生產確認
confirmation_generator = ProductionConfirmationGenerator(asset_servant)
confirmation = confirmation_generator.generate_confirmation(
    production_order_id='PO-67890',
    start_time=start_time,
    end_time=end_time
)
print(f"Confirmation generated: {confirmation['confirmationId']}")
```

---

