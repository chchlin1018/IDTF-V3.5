

# FactoryTalk MES 與 NDH 雙向連接器實施指南

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件提供了 FactoryTalk MES 與 NDH 雙向連接器的詳細實施指南,包括數據流、API 規格和參考程式碼。

## 2. 數據流

### 2.1 從 MES 到 NDH

1. **工單狀態變更 (MES)**
   - FactoryTalk MES 中的工單狀態發生變更。
   - FactoryTalk OPC UA Server 的對應 Tag 值發生變更。
   - **MES to NDH Service** 透過 OPC UA 訂閱到該 Tag 的變更。
   - 服務將事件轉換為 NDH 標準格式,並發布到 NDH 的訊息佇列。
   - NDH 中的 Asset Servant 訂閱到該事件,並更新相關資產的 `production` 屬性。

2. **設備 OEE 數據 (MES)**
   - FactoryTalk MES 計算出設備的 OEE 數據。
   - OPC UA Server 的 OEE 相關 Tag 值更新。
   - **MES to NDH Service** 訂閱到這些 Tag 的變更。
   - 服務將數據轉換並發布到 NDH 的訊息佇列。
   - Asset Servant 更新資產的 `kpi` 屬性。

### 2.2 從 NDH 到 MES

1. **手動輸入生產數據 (NDH)**
   - 操作員在 NDH 的介面中手動輸入生產數據 (如:廢品數量)。
   - Asset Servant 發布 `ManualDataEntered` 事件到 NDH 的訊息佇列。
   - **NDH to MES Service** 訂閱到該事件。
   - 服務呼叫 FactoryTalk Web API 的 `updateProductionData` 方法,將數據寫入 MES。

2. **觸發 MES 工作流 (NDH)**
   - NDH 中的規則引擎偵測到需要觸發 MES 的某個工作流 (如:請求物料)。
   - Asset Servant 發布 `TriggerWorkflow` 事件。
   - **NDH to MES Service** 訂閱到該事件。
   - 服務呼叫 FactoryTalk Web API 的 `triggerWorkflow` 方法,觸發 MES 的工作流。

---

## 3. API 規格

### 3.1 FactoryTalk Web API

- `POST /api/factorytalk/production-data`
- `POST /api/factorytalk/workflows/trigger`
- `GET /api/factorytalk/work-orders`
- `GET /api/factorytalk/equipments/{id}/oee`

### 3.2 NDH API

- `POST /api/v1/mes/events`
- `GET /api/v1/assets/{id}/production`

---

## 4. 參考程式碼

### 4.1 MES to NDH Service (.NET Core)

```csharp
// Program.cs
public static IHostBuilder CreateHostBuilder(string[] args) =>
    Host.CreateDefaultBuilder(args)
        .ConfigureServices((hostContext, services) =>
        {
            services.AddHostedService<MESToNDHWorker>();
        });

// MESToNDHWorker.cs
public class MESToNDHWorker : BackgroundService
{
    private readonly ILogger<MESToNDHWorker> _logger;
    private readonly IOPCUAClient _opcuaClient;
    private readonly INDHMessagePublisher _publisher;

    public MESToNDHWorker(ILogger<MESToNDHWorker> logger, IOPCUAClient opcuaClient, INDHMessagePublisher publisher)
    {
        _logger = logger;
        _opcuaClient = opcuaClient;
        _publisher = publisher;
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        await _opcuaClient.SubscribeAsync("ns=2;s=FactoryTalk.WorkOrder.Status", async (value) =>
        {
            var ndhEvent = new { eventType = "WorkOrderStatusChanged", payload = value };
            await _publisher.PublishAsync("ndh.mes.events", ndhEvent);
        });
    }
}
```

### 4.2 NDH to MES Service (Python)

```python
# main.py
import pika
import requests

def on_message(channel, method, properties, body):
    event = json.loads(body)
    if event["eventType"] == "ManualDataEntered":
        data = transform_to_mes_format(event)
        response = requests.post("http://factorytalk/api/factorytalk/production-data", json=data)
        if response.status_code == 200:
            channel.basic_ack(delivery_tag=method.delivery_tag)

connection = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
channel = connection.channel()
channel.queue_declare(queue="ndh_to_mes")
channel.basic_consume(queue="ndh_to_mes", on_message_callback=on_message)
channel.start_consuming()
```

---

## 5. 預期結果

- **無縫整合**: 實現 FactoryTalk MES 與 NDH 之間的無縫數據交換。
- **即時同步**: 確保 MES 與 NDH 之間的數據即時同步。
- **高可靠性**: 透過訊息佇列確保數據交換的可靠性。
- **可擴展性**: 模組化設計,易於擴展和維護。

---

