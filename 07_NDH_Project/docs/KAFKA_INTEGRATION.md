# NDH 與 Kafka 整合架構

## 目錄

1. [為什麼需要 Kafka](#為什麼需要-kafka)
2. [Kafka 在 NDH 架構中的角色](#kafka-在-ndh-架構中的角色)
3. [整合架構設計](#整合架構設計)
4. [Event Service 的 Kafka 實作](#event-service-的-kafka-實作)
5. [資產狀態同步](#資產狀態同步)
6. [完整實作範例](#完整實作範例)
7. [效能和可擴展性](#效能和可擴展性)
8. [部署架構](#部署架構)

## 為什麼需要 Kafka

### 當前架構的限制

使用記憶體佇列或 Redis 的限制:

```
記憶體佇列:
✗ 資料不持久化
✗ 無法跨程序共享
✗ 重啟後資料遺失
✗ 無法處理大量資料

Redis:
✓ 資料持久化
✓ 跨程序共享
✗ 有限的資料保留
✗ 不適合大量歷史資料
✗ 無法重播事件
```

### Kafka 的優勢

```
Kafka:
✓ 高吞吐量 (百萬訊息/秒)
✓ 持久化儲存
✓ 可重播事件
✓ 分散式架構
✓ 水平擴展
✓ 資料保留策略
✓ 消費者群組
✓ 事件溯源支援
```

### 使用場景

| 場景 | Redis | Kafka | 建議 |
|------|-------|-------|------|
| 即時通知 | ✓ | ✓ | Redis (延遲更低) |
| 資料流處理 | ✗ | ✓ | Kafka |
| 事件溯源 | ✗ | ✓ | Kafka |
| 歷史資料查詢 | ✗ | ✓ | Kafka |
| 大量資料 | ✗ | ✓ | Kafka |
| 簡單快取 | ✓ | ✗ | Redis |
| 分散式鎖 | ✓ | ✗ | Redis |

## Kafka 在 NDH 架構中的角色

### 整合架構圖

```
┌─────────────────────────────────────────────────────────────┐
│                    IADL Editor (Client)                      │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    NDH Client Proxy                          │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    NDH Broker (ORB)                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │   Naming     │  │   Lifecycle  │  │   Security   │      │
│  │   Service    │  │   Service    │  │   Service    │      │
│  │   (Redis)    │  │              │  │              │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│                                                              │
│  ┌──────────────────────────────────────────────────────┐  │
│  │            Event Service (Hybrid)                     │  │
│  │  ┌────────────────┐      ┌────────────────┐         │  │
│  │  │  Redis Queue   │      │  Kafka Topics  │         │  │
│  │  │  (即時事件)     │      │  (持久化事件)   │         │  │
│  │  └────────────────┘      └────────────────┘         │  │
│  └──────────────────────────────────────────────────────┘  │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                    Kafka Cluster                             │
│  ┌────────────┐  ┌────────────┐  ┌────────────┐           │
│  │  Broker 1  │  │  Broker 2  │  │  Broker 3  │           │
│  └────────────┘  └────────────┘  └────────────┘           │
│                                                              │
│  Topics:                                                     │
│  - ndh.assets.events         (資產事件)                      │
│  - ndh.assets.state          (資產狀態)                      │
│  - ndh.assets.commands       (資產命令)                      │
│  - ndh.assets.alarms         (資產警報)                      │
│  - ndh.assets.history        (歷史資料)                      │
└─────────────────────────────────────────────────────────────┘
```

### 混合架構策略

```python
class HybridEventService:
    """
    混合事件服務
    - Redis: 即時事件 (低延遲)
    - Kafka: 持久化事件 (可重播)
    """
    
    def __init__(self, redis_client, kafka_producer):
        self.redis = redis_client
        self.kafka = kafka_producer
        self.event_routing = {
            # 即時事件 -> Redis
            "asset.status.changed": "redis",
            "asset.alarm.triggered": "redis",
            
            # 持久化事件 -> Kafka
            "asset.created": "kafka",
            "asset.deleted": "kafka",
            "asset.state.snapshot": "kafka",
            "asset.command.executed": "kafka",
            
            # 雙寫事件 -> Redis + Kafka
            "asset.value.updated": "both"
        }
    
    async def publish(self, event_type: str, event_data: dict):
        """發布事件,根據類型選擇目標"""
        routing = self.event_routing.get(event_type, "kafka")
        
        if routing == "redis":
            await self._publish_redis(event_type, event_data)
        elif routing == "kafka":
            await self._publish_kafka(event_type, event_data)
        elif routing == "both":
            await self._publish_redis(event_type, event_data)
            await self._publish_kafka(event_type, event_data)
```

## 整合架構設計

### Topic 設計

```
Topic 命名規範:
ndh.<domain>.<entity>.<event_type>

範例:
- ndh.assets.pump.created
- ndh.assets.pump.state_changed
- ndh.assets.motor.alarm_triggered
- ndh.commands.pump.start
- ndh.commands.pump.stop
```

### 完整 Topic 結構

```python
class NDHKafkaTopics:
    """NDH Kafka Topic 定義"""
    
    # 資產事件 (Asset Events)
    ASSET_CREATED = "ndh.assets.events.created"
    ASSET_UPDATED = "ndh.assets.events.updated"
    ASSET_DELETED = "ndh.assets.events.deleted"
    ASSET_ACTIVATED = "ndh.assets.events.activated"
    ASSET_DEACTIVATED = "ndh.assets.events.deactivated"
    
    # 資產狀態 (Asset State)
    ASSET_STATE_SNAPSHOT = "ndh.assets.state.snapshot"
    ASSET_STATE_CHANGED = "ndh.assets.state.changed"
    ASSET_ATTRIBUTE_CHANGED = "ndh.assets.state.attribute_changed"
    
    # 資產命令 (Asset Commands)
    ASSET_COMMAND_REQUEST = "ndh.assets.commands.request"
    ASSET_COMMAND_RESPONSE = "ndh.assets.commands.response"
    ASSET_COMMAND_FAILED = "ndh.assets.commands.failed"
    
    # 資產警報 (Asset Alarms)
    ASSET_ALARM_TRIGGERED = "ndh.assets.alarms.triggered"
    ASSET_ALARM_CLEARED = "ndh.assets.alarms.cleared"
    ASSET_ALARM_ACKNOWLEDGED = "ndh.assets.alarms.acknowledged"
    
    # 資產歷史 (Asset History)
    ASSET_HISTORY_VALUE = "ndh.assets.history.value"
    ASSET_HISTORY_OPERATION = "ndh.assets.history.operation"
    
    # 系統事件 (System Events)
    SYSTEM_ORB_STARTED = "ndh.system.events.orb_started"
    SYSTEM_ORB_STOPPED = "ndh.system.events.orb_stopped"
    SYSTEM_ADAPTER_CREATED = "ndh.system.events.adapter_created"
```

### 訊息格式

```python
from dataclasses import dataclass, asdict
from datetime import datetime
from typing import Any, Optional
import json

@dataclass
class NDHKafkaMessage:
    """NDH Kafka 訊息格式"""
    
    # 訊息元資料
    message_id: str          # 訊息唯一 ID
    message_type: str        # 訊息類型
    timestamp: str           # ISO 8601 時間戳
    source_orb: str          # 來源 ORB ID
    
    # 資產資訊
    asset_id: str            # 資產 ID
    asset_uuid: str          # 資產 UUID
    asset_type: str          # 資產類型
    location: str            # 位置
    
    # 事件資料
    event_type: str          # 事件類型
    event_data: dict         # 事件資料
    
    # 可選欄位
    correlation_id: Optional[str] = None  # 關聯 ID
    causation_id: Optional[str] = None    # 因果 ID
    version: int = 1                       # 訊息版本
    
    def to_json(self) -> str:
        """轉換為 JSON"""
        return json.dumps(asdict(self))
    
    @classmethod
    def from_json(cls, json_str: str):
        """從 JSON 解析"""
        data = json.loads(json_str)
        return cls(**data)

# 使用範例
message = NDHKafkaMessage(
    message_id=str(uuid.uuid4()),
    message_type="asset.state.changed",
    timestamp=datetime.now().isoformat(),
    source_orb="NDH-ORB-1",
    asset_id="PUMP_001",
    asset_uuid="550e8400-e29b-41d4-a716-446655440000",
    asset_type="Pump",
    location="FACTORY_A",
    event_type="attribute_changed",
    event_data={
        "attribute": "flow_rate",
        "old_value": 100.0,
        "new_value": 125.5
    }
)
```

## Event Service 的 Kafka 實作

### Kafka Event Service

```python
"""
Kafka-based Event Service for NDH
"""

from kafka import KafkaProducer, KafkaConsumer
from kafka.admin import KafkaAdminClient, NewTopic
import asyncio
import json
from typing import Callable, Dict, List
import uuid

class KafkaEventService:
    """
    基於 Kafka 的事件服務
    取代或補充原有的記憶體事件服務
    """
    
    def __init__(
        self,
        bootstrap_servers: List[str],
        orb_id: str,
        consumer_group: str = "ndh-consumers"
    ):
        self.bootstrap_servers = bootstrap_servers
        self.orb_id = orb_id
        self.consumer_group = consumer_group
        
        # Kafka Producer (同步)
        self.producer = KafkaProducer(
            bootstrap_servers=bootstrap_servers,
            value_serializer=lambda v: json.dumps(v).encode('utf-8'),
            key_serializer=lambda k: k.encode('utf-8') if k else None,
            acks='all',  # 等待所有副本確認
            retries=3,
            max_in_flight_requests_per_connection=1  # 確保順序
        )
        
        # 訂閱者管理
        self.subscribers: Dict[str, List[Callable]] = {}
        self.consumers: Dict[str, KafkaConsumer] = {}
        
        # 消費者執行緒
        self.consumer_tasks = []
    
    async def init(self):
        """初始化 Kafka 連接"""
        # 建立管理客戶端
        admin = KafkaAdminClient(bootstrap_servers=self.bootstrap_servers)
        
        # 建立必要的 Topics
        topics = [
            NewTopic(
                name=NDHKafkaTopics.ASSET_CREATED,
                num_partitions=3,
                replication_factor=2
            ),
            NewTopic(
                name=NDHKafkaTopics.ASSET_STATE_CHANGED,
                num_partitions=6,
                replication_factor=2
            ),
            NewTopic(
                name=NDHKafkaTopics.ASSET_COMMAND_REQUEST,
                num_partitions=3,
                replication_factor=2
            ),
            # ... 其他 topics
        ]
        
        try:
            admin.create_topics(new_topics=topics, validate_only=False)
            print("Kafka topics created successfully")
        except Exception as e:
            print(f"Topics may already exist: {e}")
        
        admin.close()
    
    async def publish(
        self,
        topic: str,
        event_type: str,
        asset_id: str,
        asset_uuid: str,
        asset_type: str,
        location: str,
        event_data: dict,
        key: str = None
    ):
        """
        發布事件到 Kafka
        
        Args:
            topic: Kafka topic
            event_type: 事件類型
            asset_id: 資產 ID
            asset_uuid: 資產 UUID
            asset_type: 資產類型
            location: 位置
            event_data: 事件資料
            key: Partition key (預設使用 asset_id)
        """
        # 建立訊息
        message = NDHKafkaMessage(
            message_id=str(uuid.uuid4()),
            message_type=event_type,
            timestamp=datetime.now().isoformat(),
            source_orb=self.orb_id,
            asset_id=asset_id,
            asset_uuid=asset_uuid,
            asset_type=asset_type,
            location=location,
            event_type=event_type,
            event_data=event_data
        )
        
        # 使用 asset_id 作為 partition key,確保同一資產的事件順序
        partition_key = key or asset_id
        
        # 非同步發送
        future = self.producer.send(
            topic,
            key=partition_key,
            value=asdict(message)
        )
        
        # 等待確認
        try:
            record_metadata = future.get(timeout=10)
            print(f"Message sent to {record_metadata.topic} "
                  f"partition {record_metadata.partition} "
                  f"offset {record_metadata.offset}")
        except Exception as e:
            print(f"Failed to send message: {e}")
            raise
    
    async def subscribe(
        self,
        topic: str,
        callback: Callable,
        from_beginning: bool = False
    ):
        """
        訂閱 Kafka topic
        
        Args:
            topic: Kafka topic
            callback: 回調函數
            from_beginning: 是否從頭開始消費
        """
        if topic not in self.subscribers:
            self.subscribers[topic] = []
            
            # 建立消費者
            consumer = KafkaConsumer(
                topic,
                bootstrap_servers=self.bootstrap_servers,
                group_id=self.consumer_group,
                value_deserializer=lambda m: json.loads(m.decode('utf-8')),
                key_deserializer=lambda k: k.decode('utf-8') if k else None,
                auto_offset_reset='earliest' if from_beginning else 'latest',
                enable_auto_commit=True
            )
            
            self.consumers[topic] = consumer
            
            # 啟動消費者任務
            task = asyncio.create_task(self._consume_loop(topic))
            self.consumer_tasks.append(task)
        
        self.subscribers[topic].append(callback)
    
    async def _consume_loop(self, topic: str):
        """消費者循環"""
        consumer = self.consumers[topic]
        
        while True:
            try:
                # 批次拉取訊息
                messages = consumer.poll(timeout_ms=1000, max_records=100)
                
                for topic_partition, records in messages.items():
                    for record in records:
                        # 解析訊息
                        message_data = record.value
                        message = NDHKafkaMessage(**message_data)
                        
                        # 呼叫所有訂閱者
                        for callback in self.subscribers[topic]:
                            try:
                                await callback(message)
                            except Exception as e:
                                print(f"Error in callback: {e}")
                
                # 讓出控制權
                await asyncio.sleep(0.001)
                
            except Exception as e:
                print(f"Error in consume loop: {e}")
                await asyncio.sleep(1)
    
    async def unsubscribe(self, topic: str, callback: Callable):
        """取消訂閱"""
        if topic in self.subscribers:
            self.subscribers[topic].remove(callback)
    
    async def replay_events(
        self,
        topic: str,
        from_timestamp: datetime,
        to_timestamp: datetime,
        callback: Callable
    ):
        """
        重播事件 (Kafka 的殺手級功能)
        
        Args:
            topic: Kafka topic
            from_timestamp: 開始時間
            to_timestamp: 結束時間
            callback: 處理每個事件的回調
        """
        # 建立臨時消費者
        consumer = KafkaConsumer(
            topic,
            bootstrap_servers=self.bootstrap_servers,
            group_id=f"replay-{uuid.uuid4()}",
            value_deserializer=lambda m: json.loads(m.decode('utf-8')),
            auto_offset_reset='earliest',
            enable_auto_commit=False
        )
        
        try:
            # 取得所有分區
            partitions = consumer.partitions_for_topic(topic)
            
            for partition in partitions:
                # 根據時間戳尋找 offset
                tp = TopicPartition(topic, partition)
                consumer.assign([tp])
                
                # 尋找開始 offset
                start_offset = consumer.offsets_for_times({
                    tp: int(from_timestamp.timestamp() * 1000)
                })
                
                if start_offset[tp]:
                    consumer.seek(tp, start_offset[tp].offset)
                    
                    # 消費訊息直到結束時間
                    for record in consumer:
                        message = NDHKafkaMessage(**record.value)
                        msg_time = datetime.fromisoformat(message.timestamp)
                        
                        if msg_time > to_timestamp:
                            break
                        
                        await callback(message)
        
        finally:
            consumer.close()
    
    async def get_asset_history(
        self,
        asset_id: str,
        from_timestamp: datetime,
        to_timestamp: datetime
    ) -> List[NDHKafkaMessage]:
        """
        取得資產歷史
        利用 Kafka 的持久化特性
        """
        history = []
        
        async def collect(message: NDHKafkaMessage):
            if message.asset_id == asset_id:
                history.append(message)
        
        await self.replay_events(
            NDHKafkaTopics.ASSET_STATE_CHANGED,
            from_timestamp,
            to_timestamp,
            collect
        )
        
        return history
    
    async def shutdown(self):
        """關閉服務"""
        # 取消所有消費者任務
        for task in self.consumer_tasks:
            task.cancel()
        
        # 關閉所有消費者
        for consumer in self.consumers.values():
            consumer.close()
        
        # 關閉生產者
        self.producer.close()
```

## 資產狀態同步

### 使用 Kafka 實現資產狀態同步

```python
class AssetStateSynchronizer:
    """
    資產狀態同步器
    使用 Kafka 在多個 NDH 實例間同步資產狀態
    """
    
    def __init__(self, orb: NDH_ORB, kafka_service: KafkaEventService):
        self.orb = orb
        self.kafka = kafka_service
        self.snapshot_interval = 60  # 每 60 秒發送一次完整快照
    
    async def start(self):
        """啟動同步器"""
        # 訂閱狀態變更事件
        await self.kafka.subscribe(
            NDHKafkaTopics.ASSET_STATE_CHANGED,
            self._handle_state_changed
        )
        
        # 訂閱狀態快照事件
        await self.kafka.subscribe(
            NDHKafkaTopics.ASSET_STATE_SNAPSHOT,
            self._handle_state_snapshot
        )
        
        # 啟動快照發送任務
        asyncio.create_task(self._snapshot_loop())
    
    async def _handle_state_changed(self, message: NDHKafkaMessage):
        """處理狀態變更事件"""
        # 檢查是否是自己發送的
        if message.source_orb == self.orb.orb_id:
            return
        
        # 更新本地資產狀態
        servant = await self._find_or_create_servant(
            message.asset_id,
            message.location,
            message.asset_type
        )
        
        if servant:
            # 應用狀態變更
            event_data = message.event_data
            if event_data.get("attribute"):
                await servant.set_attribute(
                    event_data["attribute"],
                    event_data["new_value"]
                )
    
    async def _handle_state_snapshot(self, message: NDHKafkaMessage):
        """處理狀態快照事件"""
        # 檢查是否是自己發送的
        if message.source_orb == self.orb.orb_id:
            return
        
        # 重建或更新資產狀態
        servant = await self._find_or_create_servant(
            message.asset_id,
            message.location,
            message.asset_type
        )
        
        if servant:
            # 應用完整狀態
            full_state = message.event_data
            servant.attributes = full_state.get("attributes", {})
            servant.state = AssetLifecycleState(full_state.get("lifecycle_state"))
    
    async def _snapshot_loop(self):
        """定期發送狀態快照"""
        while True:
            await asyncio.sleep(self.snapshot_interval)
            
            # 遍歷所有活躍資產
            for adapter in self.orb.adapters.values():
                for asset_id, servant in adapter.servants.items():
                    # 發送快照
                    await self._send_snapshot(servant)
    
    async def _send_snapshot(self, servant: AssetServant):
        """發送資產狀態快照"""
        state = await servant.get_state()
        
        await self.kafka.publish(
            topic=NDHKafkaTopics.ASSET_STATE_SNAPSHOT,
            event_type="state_snapshot",
            asset_id=servant.asset_id,
            asset_uuid=str(uuid.uuid5(
                uuid.NAMESPACE_DNS,
                f"{servant.location}.{servant.asset_id}"
            )),
            asset_type=servant.asset_type,
            location=servant.location,
            event_data=state
        )
    
    async def _find_or_create_servant(
        self,
        asset_id: str,
        location: str,
        asset_type: str
    ) -> Optional[AssetServant]:
        """尋找或建立僕人物件"""
        # 先嘗試尋找現有的
        for adapter in self.orb.adapters.values():
            servant = await adapter.find_servant(asset_id)
            if servant:
                return servant
        
        # 不存在,建立新的
        servant = AssetServant(asset_id, asset_type, location)
        adapter = self.orb.get_adapter("RootPOA")
        await adapter.activate_object(servant)
        
        return servant
```

## 完整實作範例

### 整合 Kafka 的 NDH ORB

```python
"""
NDH ORB with Kafka Integration
"""

class NDH_ORB_Kafka(NDH_ORB):
    """
    整合 Kafka 的 NDH ORB
    """
    
    def __init__(
        self,
        orb_id: str,
        endpoint: str,
        kafka_bootstrap_servers: List[str]
    ):
        super().__init__(orb_id, endpoint)
        
        # Kafka 事件服務
        self.kafka_event_service = KafkaEventService(
            bootstrap_servers=kafka_bootstrap_servers,
            orb_id=orb_id
        )
        
        # 狀態同步器
        self.state_synchronizer = None
    
    async def init(self):
        """初始化 ORB"""
        await super().init()
        
        # 初始化 Kafka
        await self.kafka_event_service.init()
        
        # 啟動狀態同步
        self.state_synchronizer = AssetStateSynchronizer(
            self,
            self.kafka_event_service
        )
        await self.state_synchronizer.start()
        
        print(f"NDH ORB with Kafka initialized: {self.orb_id}")
    
    async def publish_asset_event(
        self,
        event_type: str,
        servant: AssetServant,
        event_data: dict
    ):
        """發布資產事件到 Kafka"""
        await self.kafka_event_service.publish(
            topic=self._get_topic_for_event(event_type),
            event_type=event_type,
            asset_id=servant.asset_id,
            asset_uuid=str(uuid.uuid5(
                uuid.NAMESPACE_DNS,
                f"{servant.location}.{servant.asset_id}"
            )),
            asset_type=servant.asset_type,
            location=servant.location,
            event_data=event_data
        )
    
    def _get_topic_for_event(self, event_type: str) -> str:
        """根據事件類型選擇 topic"""
        mapping = {
            "created": NDHKafkaTopics.ASSET_CREATED,
            "updated": NDHKafkaTopics.ASSET_UPDATED,
            "deleted": NDHKafkaTopics.ASSET_DELETED,
            "state_changed": NDHKafkaTopics.ASSET_STATE_CHANGED,
            "attribute_changed": NDHKafkaTopics.ASSET_STATE_CHANGED,
            "alarm_triggered": NDHKafkaTopics.ASSET_ALARM_TRIGGERED,
        }
        return mapping.get(event_type, NDHKafkaTopics.ASSET_CREATED)
    
    async def shutdown(self):
        """關閉 ORB"""
        await self.kafka_event_service.shutdown()
        await super().shutdown()
```

### 使用範例

```python
async def main():
    """完整使用範例"""
    
    # 建立 ORB with Kafka
    orb = NDH_ORB_Kafka(
        orb_id="NDH-ORB-1",
        endpoint="http://localhost:8000",
        kafka_bootstrap_servers=["localhost:9092"]
    )
    
    await orb.init()
    
    # 建立資產
    pump = PumpServant("PUMP_001", "FACTORY_A")
    adapter = orb.get_adapter("RootPOA")
    await adapter.activate_object(pump)
    
    # 設定屬性 (自動發布到 Kafka)
    await pump.set_attribute("flow_rate", 125.5)
    
    # 發布事件
    await orb.publish_asset_event(
        "attribute_changed",
        pump,
        {
            "attribute": "flow_rate",
            "old_value": 100.0,
            "new_value": 125.5
        }
    )
    
    # 訂閱事件
    async def handle_alarm(message: NDHKafkaMessage):
        print(f"Alarm: {message.event_data}")
    
    await orb.kafka_event_service.subscribe(
        NDHKafkaTopics.ASSET_ALARM_TRIGGERED,
        handle_alarm
    )
    
    # 查詢歷史
    history = await orb.kafka_event_service.get_asset_history(
        "PUMP_001",
        datetime.now() - timedelta(hours=24),
        datetime.now()
    )
    
    print(f"Found {len(history)} historical events")
    
    # 重播事件
    async def replay_handler(message: NDHKafkaMessage):
        print(f"Replaying: {message.timestamp} - {message.event_type}")
    
    await orb.kafka_event_service.replay_events(
        NDHKafkaTopics.ASSET_STATE_CHANGED,
        datetime.now() - timedelta(hours=1),
        datetime.now(),
        replay_handler
    )
    
    # 運行一段時間
    await asyncio.sleep(3600)
    
    # 關閉
    await orb.shutdown()

if __name__ == "__main__":
    asyncio.run(main())
```

## 效能和可擴展性

### 效能指標

```
Kafka 效能 (3 個 broker):
- 吞吐量: 1,000,000+ 訊息/秒
- 延遲: < 10ms (p99)
- 儲存: TB 級別
- 保留: 天/週/月

NDH + Kafka:
- 資產事件發布: < 5ms
- 事件消費: < 10ms
- 狀態同步: < 100ms
- 歷史查詢: < 1s (百萬筆)
```

### 水平擴展

```
單一 NDH 實例:
- 資產數量: ~10,000
- 事件速率: ~1,000/秒

多個 NDH 實例 + Kafka:
- 資產數量: 無限制
- 事件速率: ~1,000,000/秒
- 實例數量: 按需擴展
```

### Partition 策略

```python
# 使用 asset_id 作為 partition key
# 確保同一資產的事件順序
partition_key = asset_id

# 或使用 location 作為 partition key
# 將同一位置的資產分組
partition_key = location

# 或使用 asset_type 作為 partition key
# 將同類型的資產分組
partition_key = asset_type
```

## 部署架構

### 小型部署 (< 1000 資產)

```
┌─────────────┐
│  NDH ORB    │
│  + Redis    │
└─────────────┘

建議:
- 使用 Redis 即可
- 不需要 Kafka
```

### 中型部署 (1000 - 10000 資產)

```
┌─────────────┐     ┌─────────────┐
│  NDH ORB 1  │     │  NDH ORB 2  │
└──────┬──────┘     └──────┬──────┘
       │                   │
       └───────┬───────────┘
               │
       ┌───────▼────────┐
       │  Kafka (1 node)│
       │  + Redis       │
       └────────────────┘

建議:
- 單節點 Kafka
- Redis 用於快取和鎖
- Kafka 用於事件和歷史
```

### 大型部署 (> 10000 資產)

```
┌─────────┐  ┌─────────┐  ┌─────────┐
│ NDH ORB │  │ NDH ORB │  │ NDH ORB │
│    1    │  │    2    │  │    3    │
└────┬────┘  └────┬────┘  └────┬────┘
     │            │            │
     └────────────┼────────────┘
                  │
     ┌────────────▼────────────┐
     │   Kafka Cluster         │
     │  ┌──────┐  ┌──────┐    │
     │  │Broker│  │Broker│    │
     │  │  1   │  │  2   │    │
     │  └──────┘  └──────┘    │
     │  ┌──────┐               │
     │  │Broker│               │
     │  │  3   │               │
     │  └──────┘               │
     └─────────────────────────┘
              │
     ┌────────▼────────┐
     │  Redis Cluster  │
     └─────────────────┘

建議:
- 多節點 Kafka 叢集
- Redis 叢集
- 多個 NDH ORB 實例
- 負載平衡器
```

## 總結

### Kafka 為 NDH 帶來的優勢

1. **事件溯源**: 完整的事件歷史
2. **可重播性**: 可以重播任意時間段的事件
3. **高吞吐量**: 支援大規模資產管理
4. **持久化**: 資料不會遺失
5. **水平擴展**: 按需增加節點
6. **解耦**: NDH 實例間鬆耦合
7. **時間旅行**: 查詢歷史狀態

### 使用建議

| 資產規模 | 事件速率 | 建議方案 |
|---------|---------|---------|
| < 100 | < 10/秒 | 記憶體佇列 |
| 100-1000 | 10-100/秒 | Redis |
| 1000-10000 | 100-1000/秒 | Redis + Kafka (單節點) |
| > 10000 | > 1000/秒 | Kafka 叢集 |

### 遷移路徑

```
階段 1: 記憶體佇列
    ↓
階段 2: Redis (保持相容性)
    ↓
階段 3: Redis + Kafka (混合模式)
    ↓
階段 4: Kafka 為主 (企業級)
```

NDH 的 CORBA 啟發架構天然支援這種漸進式擴展,可以根據實際需求選擇合適的技術組合!

---

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**最後更新**: 2025-01-09

