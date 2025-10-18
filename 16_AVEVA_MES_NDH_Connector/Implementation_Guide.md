

# AVEVA MES 與 NDH 雙向連接器實施指南

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件提供了 AVEVA MES 與 NDH 雙向連接器的詳細實施指南,包括數據流、API 規格和參考程式碼。

## 2. 數據流

### 2.1 從 MES 到 NDH

1. **工單創建 (MES)**
   - 使用者在 AVEVA MES 中創建一個新的生產工單。
   - MES Event Broker 發布 `WorkOrderCreated` 事件。
   - **MES to NDH Service** 訂閱到該事件。
   - 服務將事件轉換為 NDH 標準格式,並發布到 NDH 的訊息佇列。
   - NDH 中的 Asset Servant 訂閱到該事件,並更新相關資產的 `production` 屬性。

2. **設備狀態變更 (MES)**
   - 設備狀態在 AVEVA MES 中發生變更 (如:從 `Running` 變為 `Down`)。
   - MES Event Broker 發布 `EquipmentStatusChanged` 事件。
   - **MES to NDH Service** 訂閱到該事件。
   - 服務將事件轉換並發布到 NDH 的訊息佇列。
   - Asset Servant 更新資產的 `status` 屬性。

### 2.2 從 NDH 到 MES

1. **產量回報 (NDH)**
   - 設備完成一個產品的加工。
   - Asset Servant 根據 PLC 數據計算產量,並發布 `ProductionConfirmed` 事件到 NDH 的訊息佇列。
   - **NDH to MES Service** 訂閱到該事件。
   - 服務呼叫 AVEVA MES Web API 的 `confirmProduction` 方法,回傳生產確認。

2. **品質檢驗 (NDH)**
   - 品質檢驗系統 (如:AOI) 完成檢驗,並將結果發送到 NDH。
   - Asset Servant 發布 `QualityInspectionCompleted` 事件。
   - **NDH to MES Service** 訂閱到該事件。
   - 服務呼叫 AVEVA MES Web API 的 `recordQualitySample` 方法,記錄品質樣本。

---

## 3. API 規格

### 3.1 AVEVA MES Web API

- `POST /api/mes/confirmations`
- `POST /api/mes/quality-samples`
- `GET /api/mes/work-orders`
- `GET /api/mes/equipments/{id}/status`

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
    private readonly IMESEventSubscriber _subscriber;
    private readonly INDHMessagePublisher _publisher;

    public MESToNDHWorker(ILogger<MESToNDHWorker> logger, IMESEventSubscriber subscriber, INDHMessagePublisher publisher)
    {
        _logger = logger;
        _subscriber = subscriber;
        _publisher = publisher;
    }

    protected override async Task ExecuteAsync(CancellationToken stoppingToken)
    {
        await _subscriber.SubscribeAsync("WorkOrderCreated", async (e) =>
        {
            var ndhEvent = TransformEvent(e);
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
    if event["eventType"] == "ProductionConfirmed":
        confirmation = transform_to_mes_confirmation(event)
        response = requests.post("http://aveva-mes/api/mes/confirmations", json=confirmation)
        if response.status_code == 201:
            channel.basic_ack(delivery_tag=method.delivery_tag)

connection = pika.BlockingConnection(pika.ConnectionParameters("localhost"))
channel = connection.channel()
channel.queue_declare(queue="ndh_to_mes")
channel.basic_consume(queue="ndh_to_mes", on_message_callback=on_message)
channel.start_consuming()
```

---

## 5. 預期結果

- **無縫整合**: 實現 AVEVA MES 與 NDH 之間的無縫數據交換。
- **即時同步**: 確保 MES 與 NDH 之間的數據即時同步。
- **高可靠性**: 透過訊息佇列確保數據交換的可靠性。
- **可擴展性**: 模組化設計,易於擴展和維護。

---

