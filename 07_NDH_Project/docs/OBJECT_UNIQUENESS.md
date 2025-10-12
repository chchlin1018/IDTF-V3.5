# NDH 物件唯一性保證機制

## 目錄

1. [為什麼需要唯一性](#為什麼需要唯一性)
2. [唯一性的層級](#唯一性的層級)
3. [唯一識別符設計](#唯一識別符設計)
4. [實作策略](#實作策略)
5. [分散式環境的挑戰](#分散式環境的挑戰)
6. [完整實作範例](#完整實作範例)
7. [最佳實踐](#最佳實踐)

## 為什麼需要唯一性

### 問題場景

```
情境 1: 多個資料來源
- OPC UA Server 推送 PUMP_001 的資料
- Modbus Gateway 也推送 PUMP_001 的資料
- 如何確保這是同一個幫浦?

情境 2: 多個 NDH 實例
- NDH Instance A 處理 PUMP_001
- NDH Instance B 也處理 PUMP_001
- 如何避免重複處理?

情境 3: 資料衝突
- 時間 T1: Source A 說 PUMP_001.status = "running"
- 時間 T1: Source B 說 PUMP_001.status = "stopped"
- 哪個是正確的?
```

### 後果

如果沒有唯一性保證:
- **資料重複**: 同一個資產有多個記錄
- **狀態不一致**: 不同來源的資料互相衝突
- **資源浪費**: 重複處理相同的資料
- **決策錯誤**: 基於錯誤資料做出錯誤決策

## 唯一性的層級

### 層級 1: Asset ID (資產識別符)

最基本的唯一性層級。

```python
# 簡單的 Asset ID
asset_id = "PUMP_001"

# 問題: 在多個工廠中可能重複
# Factory A 有 PUMP_001
# Factory B 也有 PUMP_001
```

### 層級 2: Scoped Asset ID (範圍限定的資產識別符)

加入命名空間或範圍。

```python
# 加入工廠前綴
asset_id = "FACTORY_A.PUMP_001"
asset_id = "FACTORY_B.PUMP_001"

# 或使用階層結構
asset_id = "COMPANY/FACTORY_A/BUILDING_1/PUMP_001"
```

### 層級 3: Global Unique ID (全域唯一識別符)

使用 UUID 或類似機制。

```python
# UUID v4 (隨機)
asset_uuid = "550e8400-e29b-41d4-a716-446655440000"

# UUID v5 (基於名稱的雜湊)
import uuid
namespace = uuid.NAMESPACE_DNS
asset_uuid = uuid.uuid5(namespace, "FACTORY_A.PUMP_001")
```

### 層級 4: Composite Key (複合鍵)

結合多個屬性確保唯一性。

```python
# 複合鍵
unique_key = {
    "asset_id": "PUMP_001",
    "location": "FACTORY_A",
    "manufacturer": "Grundfos",
    "serial_number": "GF-2024-001234"
}
```

## 唯一識別符設計

### 方案 1: 階層式命名 (Hierarchical Naming)

```
格式: <組織>/<地點>/<區域>/<設備類型>/<設備編號>

範例:
- ACME_CORP/TAIPEI_PLANT/BUILDING_A/PUMP/001
- ACME_CORP/TAIPEI_PLANT/BUILDING_A/MOTOR/001
- ACME_CORP/KAOHSIUNG_PLANT/BUILDING_B/PUMP/001

優點:
✓ 人類可讀
✓ 反映實際結構
✓ 易於理解和維護

缺點:
✗ 可能很長
✗ 重新組織時需要更新
✗ 需要命名規範
```

### 方案 2: UUID (Universally Unique Identifier)

```python
import uuid

# UUID v4 - 完全隨機
asset_uuid = str(uuid.uuid4())
# "550e8400-e29b-41d4-a716-446655440000"

# UUID v5 - 基於名稱的確定性 UUID
namespace = uuid.NAMESPACE_DNS
asset_id = "FACTORY_A.PUMP_001"
asset_uuid = str(uuid.uuid5(namespace, asset_id))
# "c4a8e8e2-1234-5678-9abc-def012345678"

優點:
✓ 保證全域唯一
✓ 不需要中央協調
✓ 可以離線生成

缺點:
✗ 人類不可讀
✗ 無法從 UUID 推斷資產資訊
✗ 需要額外的映射表
```

### 方案 3: 混合式 (Hybrid)

結合人類可讀的 ID 和 UUID。

```python
{
    "asset_id": "PUMP_001",                    # 人類可讀
    "full_path": "FACTORY_A/BUILDING_1/PUMP_001",  # 階層路徑
    "uuid": "550e8400-e29b-41d4-a716-446655440000",  # 全域唯一
    "serial_number": "GF-2024-001234"          # 實體識別
}

優點:
✓ 結合兩者優點
✓ 靈活性高
✓ 適合不同使用場景

缺點:
✗ 複雜度較高
✗ 需要維護多個識別符
```

## 實作策略

### 策略 1: 資料庫唯一約束

使用資料庫的 UNIQUE 約束。

```sql
CREATE TABLE assets (
    uuid VARCHAR(36) PRIMARY KEY,
    asset_id VARCHAR(100) NOT NULL,
    location VARCHAR(100) NOT NULL,
    serial_number VARCHAR(100),
    
    -- 唯一約束
    UNIQUE(asset_id, location),
    UNIQUE(serial_number)
);

-- 嘗試插入重複資料會失敗
INSERT INTO assets VALUES 
    ('uuid-1', 'PUMP_001', 'FACTORY_A', 'SN-001');
    
-- 這會失敗
INSERT INTO assets VALUES 
    ('uuid-2', 'PUMP_001', 'FACTORY_A', 'SN-002');
```

### 策略 2: Redis 分散式鎖

使用 Redis 確保分散式環境中的唯一性。

```python
import redis
import time

class AssetRegistry:
    def __init__(self, redis_client):
        self.redis = redis_client
    
    def register_asset(self, asset_id, ttl=3600):
        """
        註冊資產,確保唯一性
        
        Args:
            asset_id: 資產 ID
            ttl: 鎖的存活時間(秒)
            
        Returns:
            True if registered successfully, False if already exists
        """
        key = f"asset:registry:{asset_id}"
        
        # 使用 SET NX (Set if Not eXists)
        result = self.redis.set(
            key, 
            "registered",
            nx=True,  # 只在鍵不存在時設定
            ex=ttl    # 過期時間
        )
        
        return result is not None
    
    def is_registered(self, asset_id):
        """檢查資產是否已註冊"""
        key = f"asset:registry:{asset_id}"
        return self.redis.exists(key) > 0
    
    def unregister_asset(self, asset_id):
        """取消註冊資產"""
        key = f"asset:registry:{asset_id}"
        self.redis.delete(key)

# 使用範例
redis_client = redis.Redis(host='localhost', port=6379)
registry = AssetRegistry(redis_client)

# NDH Instance A 嘗試註冊
if registry.register_asset("PUMP_001"):
    print("Successfully registered PUMP_001")
    # 處理 PUMP_001 的資料
else:
    print("PUMP_001 already registered by another instance")
```

### 策略 3: 版本號和時間戳

使用版本號解決衝突。

```python
class AssetData:
    def __init__(self, asset_id, values, version=1, timestamp=None):
        self.asset_id = asset_id
        self.values = values
        self.version = version
        self.timestamp = timestamp or time.time()
    
    def is_newer_than(self, other):
        """比較兩個資料哪個更新"""
        if self.version > other.version:
            return True
        elif self.version == other.version:
            return self.timestamp > other.timestamp
        return False

class AssetStore:
    def __init__(self):
        self.assets = {}  # asset_id -> AssetData
    
    def update(self, new_data):
        """更新資產資料,只接受更新的版本"""
        asset_id = new_data.asset_id
        
        if asset_id not in self.assets:
            # 新資產,直接儲存
            self.assets[asset_id] = new_data
            return True
        
        current_data = self.assets[asset_id]
        
        if new_data.is_newer_than(current_data):
            # 新資料更新,更新儲存
            self.assets[asset_id] = new_data
            return True
        else:
            # 舊資料,忽略
            return False

# 使用範例
store = AssetStore()

# 時間 T1: Source A 推送 version 1
data1 = AssetData("PUMP_001", {"status": "running"}, version=1)
store.update(data1)  # True

# 時間 T2: Source B 推送 version 1 (較晚的時間戳)
data2 = AssetData("PUMP_001", {"status": "stopped"}, version=1)
store.update(data2)  # True (時間戳較新)

# 時間 T3: Source A 推送 version 2
data3 = AssetData("PUMP_001", {"status": "running"}, version=2)
store.update(data3)  # True (版本較新)
```

### 策略 4: 來源優先級

為不同資料來源設定優先級。

```python
class SourcePriority:
    # 定義來源優先級(數字越大優先級越高)
    PRIORITIES = {
        "OPC_UA": 10,
        "MODBUS": 8,
        "MQTT": 7,
        "REST_API": 5,
        "MANUAL_INPUT": 3
    }
    
    @classmethod
    def get_priority(cls, source):
        return cls.PRIORITIES.get(source, 0)

class AssetDataWithSource:
    def __init__(self, asset_id, values, source, timestamp=None):
        self.asset_id = asset_id
        self.values = values
        self.source = source
        self.priority = SourcePriority.get_priority(source)
        self.timestamp = timestamp or time.time()
    
    def should_override(self, other):
        """判斷是否應該覆蓋另一個資料"""
        # 優先級較高的來源優先
        if self.priority > other.priority:
            return True
        elif self.priority == other.priority:
            # 相同優先級,時間較新的優先
            return self.timestamp > other.timestamp
        return False

# 使用範例
data_opc = AssetDataWithSource(
    "PUMP_001", 
    {"status": "running"}, 
    "OPC_UA"
)

data_manual = AssetDataWithSource(
    "PUMP_001", 
    {"status": "stopped"}, 
    "MANUAL_INPUT"
)

if data_opc.should_override(data_manual):
    print("Use OPC_UA data (higher priority)")
else:
    print("Use manual input data")
```

## 分散式環境的挑戰

### 挑戰 1: 多個 NDH 實例

```
Scenario:
┌─────────────┐     ┌─────────────┐
│  NDH-A      │     │  NDH-B      │
│  處理       │     │  處理       │
│  PUMP_001   │     │  PUMP_001   │
└─────────────┘     └─────────────┘
       │                   │
       └───────┬───────────┘
               │
        ┌──────▼──────┐
        │   Redis     │
        │  (協調器)    │
        └─────────────┘
```

**解決方案: 分散式鎖**

```python
import redis
from contextlib import contextmanager

class DistributedLock:
    def __init__(self, redis_client, lock_name, timeout=10):
        self.redis = redis_client
        self.lock_name = f"lock:{lock_name}"
        self.timeout = timeout
        self.identifier = str(uuid.uuid4())
    
    def acquire(self):
        """取得鎖"""
        end_time = time.time() + self.timeout
        
        while time.time() < end_time:
            # 嘗試取得鎖
            if self.redis.set(
                self.lock_name,
                self.identifier,
                nx=True,
                ex=self.timeout
            ):
                return True
            
            # 等待一小段時間後重試
            time.sleep(0.001)
        
        return False
    
    def release(self):
        """釋放鎖"""
        # 使用 Lua 腳本確保原子性
        lua_script = """
        if redis.call("get", KEYS[1]) == ARGV[1] then
            return redis.call("del", KEYS[1])
        else
            return 0
        end
        """
        self.redis.eval(lua_script, 1, self.lock_name, self.identifier)
    
    @contextmanager
    def __call__(self):
        """Context manager 用法"""
        acquired = self.acquire()
        try:
            yield acquired
        finally:
            if acquired:
                self.release()

# 使用範例
redis_client = redis.Redis(host='localhost', port=6379)

def process_asset_data(asset_id, data):
    """處理資產資料,確保只有一個實例在處理"""
    lock = DistributedLock(redis_client, f"asset:{asset_id}")
    
    with lock() as acquired:
        if acquired:
            # 成功取得鎖,處理資料
            print(f"Processing {asset_id}")
            # ... 處理邏輯 ...
        else:
            # 無法取得鎖,另一個實例正在處理
            print(f"{asset_id} is being processed by another instance")
```

### 挑戰 2: 網路分區 (Network Partition)

```
Scenario:
┌─────────────┐     ╳╳╳╳╳╳╳╳╳     ┌─────────────┐
│  NDH-A      │     ╳ 網路斷線 ╳     │  NDH-B      │
│  (認為自己  │     ╳╳╳╳╳╳╳╳╳     │  (認為自己  │
│   是主節點) │                   │   是主節點) │
└─────────────┘                   └─────────────┘
```

**解決方案: Consensus Algorithm (共識演算法)**

使用 Raft 或 Paxos 等演算法,但對於 NDH 來說,可以使用更簡單的方案:

```python
class LeaderElection:
    def __init__(self, redis_client, instance_id):
        self.redis = redis_client
        self.instance_id = instance_id
        self.leader_key = "ndh:leader"
        self.heartbeat_interval = 5  # 秒
    
    def try_become_leader(self):
        """嘗試成為 Leader"""
        result = self.redis.set(
            self.leader_key,
            self.instance_id,
            nx=True,
            ex=self.heartbeat_interval * 2
        )
        return result is not None
    
    def is_leader(self):
        """檢查自己是否是 Leader"""
        current_leader = self.redis.get(self.leader_key)
        return current_leader and current_leader.decode() == self.instance_id
    
    def send_heartbeat(self):
        """發送心跳,維持 Leader 狀態"""
        if self.is_leader():
            self.redis.expire(self.leader_key, self.heartbeat_interval * 2)
    
    def run_leader_election(self):
        """運行 Leader 選舉循環"""
        while True:
            if self.is_leader():
                # 我是 Leader,發送心跳
                self.send_heartbeat()
                print(f"{self.instance_id} is the leader")
            else:
                # 我不是 Leader,嘗試成為 Leader
                if self.try_become_leader():
                    print(f"{self.instance_id} became the leader")
            
            time.sleep(self.heartbeat_interval)

# 使用範例
redis_client = redis.Redis(host='localhost', port=6379)
election = LeaderElection(redis_client, "NDH-Instance-A")

# 在背景執行緒中運行選舉
import threading
election_thread = threading.Thread(target=election.run_leader_election)
election_thread.daemon = True
election_thread.start()

# 只有 Leader 處理資料
def process_data_if_leader(data):
    if election.is_leader():
        # 處理資料
        pass
    else:
        # 轉發給 Leader 或忽略
        pass
```

## 完整實作範例

### NDH 資產註冊表實作

```python
import redis
import uuid
import time
import json
from typing import Optional, Dict, Any
from dataclasses import dataclass, asdict
from datetime import datetime

@dataclass
class AssetMetadata:
    """資產元資料"""
    asset_id: str
    uuid: str
    full_path: str
    asset_type: str
    location: str
    serial_number: Optional[str]
    registered_at: str
    registered_by: str  # NDH instance ID
    version: int
    
    def to_dict(self):
        return asdict(self)
    
    @classmethod
    def from_dict(cls, data):
        return cls(**data)

class AssetRegistry:
    """
    NDH 資產註冊表
    確保資產的唯一性和一致性
    """
    
    def __init__(self, redis_client, instance_id):
        self.redis = redis_client
        self.instance_id = instance_id
    
    def register_asset(
        self, 
        asset_id: str,
        asset_type: str,
        location: str,
        serial_number: Optional[str] = None,
        full_path: Optional[str] = None
    ) -> tuple[bool, Optional[AssetMetadata]]:
        """
        註冊資產
        
        Returns:
            (success, metadata): 成功標誌和資產元資料
        """
        # 生成 UUID
        asset_uuid = str(uuid.uuid5(
            uuid.NAMESPACE_DNS,
            f"{location}.{asset_id}"
        ))
        
        # 檢查是否已存在
        existing = self.get_asset(asset_id, location)
        if existing:
            return False, existing
        
        # 建立元資料
        metadata = AssetMetadata(
            asset_id=asset_id,
            uuid=asset_uuid,
            full_path=full_path or f"{location}/{asset_id}",
            asset_type=asset_type,
            location=location,
            serial_number=serial_number,
            registered_at=datetime.now().isoformat(),
            registered_by=self.instance_id,
            version=1
        )
        
        # 儲存到 Redis
        # 使用多個鍵確保可以從不同角度查詢
        pipe = self.redis.pipeline()
        
        # 主鍵: asset_id + location
        main_key = f"asset:{location}:{asset_id}"
        pipe.set(main_key, json.dumps(metadata.to_dict()))
        
        # UUID 索引
        uuid_key = f"asset:uuid:{asset_uuid}"
        pipe.set(uuid_key, main_key)
        
        # 序號索引(如果有)
        if serial_number:
            sn_key = f"asset:sn:{serial_number}"
            # 檢查序號是否已被使用
            if self.redis.exists(sn_key):
                return False, None
            pipe.set(sn_key, main_key)
        
        # 位置索引
        location_set = f"assets:location:{location}"
        pipe.sadd(location_set, asset_id)
        
        # 類型索引
        type_set = f"assets:type:{asset_type}"
        pipe.sadd(type_set, f"{location}:{asset_id}")
        
        pipe.execute()
        
        return True, metadata
    
    def get_asset(
        self, 
        asset_id: str, 
        location: str
    ) -> Optional[AssetMetadata]:
        """取得資產元資料"""
        key = f"asset:{location}:{asset_id}"
        data = self.redis.get(key)
        
        if data:
            return AssetMetadata.from_dict(json.loads(data))
        return None
    
    def get_asset_by_uuid(self, asset_uuid: str) -> Optional[AssetMetadata]:
        """透過 UUID 取得資產"""
        uuid_key = f"asset:uuid:{asset_uuid}"
        main_key = self.redis.get(uuid_key)
        
        if main_key:
            data = self.redis.get(main_key)
            if data:
                return AssetMetadata.from_dict(json.loads(data))
        return None
    
    def get_asset_by_serial(self, serial_number: str) -> Optional[AssetMetadata]:
        """透過序號取得資產"""
        sn_key = f"asset:sn:{serial_number}"
        main_key = self.redis.get(sn_key)
        
        if main_key:
            data = self.redis.get(main_key)
            if data:
                return AssetMetadata.from_dict(json.loads(data))
        return None
    
    def list_assets_by_location(self, location: str) -> list[str]:
        """列出特定位置的所有資產"""
        location_set = f"assets:location:{location}"
        return [
            asset_id.decode() if isinstance(asset_id, bytes) else asset_id
            for asset_id in self.redis.smembers(location_set)
        ]
    
    def list_assets_by_type(self, asset_type: str) -> list[tuple[str, str]]:
        """列出特定類型的所有資產"""
        type_set = f"assets:type:{asset_type}"
        results = []
        for item in self.redis.smembers(type_set):
            item_str = item.decode() if isinstance(item, bytes) else item
            location, asset_id = item_str.split(':', 1)
            results.append((location, asset_id))
        return results
    
    def update_asset_version(
        self, 
        asset_id: str, 
        location: str
    ) -> bool:
        """更新資產版本號"""
        metadata = self.get_asset(asset_id, location)
        if not metadata:
            return False
        
        metadata.version += 1
        
        key = f"asset:{location}:{asset_id}"
        self.redis.set(key, json.dumps(metadata.to_dict()))
        
        return True
    
    def unregister_asset(
        self, 
        asset_id: str, 
        location: str
    ) -> bool:
        """取消註冊資產"""
        metadata = self.get_asset(asset_id, location)
        if not metadata:
            return False
        
        pipe = self.redis.pipeline()
        
        # 刪除主鍵
        main_key = f"asset:{location}:{asset_id}"
        pipe.delete(main_key)
        
        # 刪除 UUID 索引
        uuid_key = f"asset:uuid:{metadata.uuid}"
        pipe.delete(uuid_key)
        
        # 刪除序號索引
        if metadata.serial_number:
            sn_key = f"asset:sn:{metadata.serial_number}"
            pipe.delete(sn_key)
        
        # 從位置集合中移除
        location_set = f"assets:location:{location}"
        pipe.srem(location_set, asset_id)
        
        # 從類型集合中移除
        type_set = f"assets:type:{metadata.asset_type}"
        pipe.srem(type_set, f"{location}:{asset_id}")
        
        pipe.execute()
        
        return True

# 使用範例
def main():
    # 連接 Redis
    redis_client = redis.Redis(host='localhost', port=6379, decode_responses=True)
    
    # 建立註冊表
    registry = AssetRegistry(redis_client, "NDH-Instance-A")
    
    # 註冊資產
    success, metadata = registry.register_asset(
        asset_id="PUMP_001",
        asset_type="Equipment",
        location="FACTORY_A",
        serial_number="GF-2024-001234"
    )
    
    if success:
        print(f"✓ Asset registered successfully")
        print(f"  UUID: {metadata.uuid}")
        print(f"  Full Path: {metadata.full_path}")
    else:
        print(f"✗ Asset already exists")
        if metadata:
            print(f"  Registered by: {metadata.registered_by}")
            print(f"  Registered at: {metadata.registered_at}")
    
    # 查詢資產
    asset = registry.get_asset("PUMP_001", "FACTORY_A")
    if asset:
        print(f"\n✓ Asset found:")
        print(f"  Asset ID: {asset.asset_id}")
        print(f"  UUID: {asset.uuid}")
        print(f"  Type: {asset.asset_type}")
        print(f"  Version: {asset.version}")
    
    # 透過序號查詢
    asset_by_sn = registry.get_asset_by_serial("GF-2024-001234")
    if asset_by_sn:
        print(f"\n✓ Asset found by serial number:")
        print(f"  Asset ID: {asset_by_sn.asset_id}")
    
    # 列出位置的所有資產
    assets_in_location = registry.list_assets_by_location("FACTORY_A")
    print(f"\n✓ Assets in FACTORY_A: {assets_in_location}")
    
    # 列出類型的所有資產
    assets_of_type = registry.list_assets_by_type("Equipment")
    print(f"\n✓ Equipment assets: {assets_of_type}")

if __name__ == "__main__":
    main()
```

## 最佳實踐

### 1. 使用多層識別符

```python
class Asset:
    def __init__(self):
        self.asset_id = "PUMP_001"              # 人類可讀
        self.uuid = "550e8400-..."              # 全域唯一
        self.full_path = "FACTORY_A/PUMP_001"   # 階層路徑
        self.serial_number = "GF-2024-001234"   # 實體識別
```

### 2. 實作冪等性 (Idempotency)

確保重複操作不會產生副作用。

```python
def register_asset_idempotent(asset_id, location):
    """冪等的資產註冊"""
    existing = registry.get_asset(asset_id, location)
    if existing:
        # 已存在,返回現有資料
        return existing
    else:
        # 不存在,建立新資料
        success, metadata = registry.register_asset(asset_id, location)
        return metadata
```

### 3. 使用樂觀鎖 (Optimistic Locking)

```python
def update_asset_with_version_check(asset_id, location, new_data, expected_version):
    """使用版本檢查的更新"""
    metadata = registry.get_asset(asset_id, location)
    
    if metadata.version != expected_version:
        raise VersionConflictError(
            f"Version mismatch: expected {expected_version}, "
            f"got {metadata.version}"
        )
    
    # 更新資料
    # ...
    
    # 增加版本號
    registry.update_asset_version(asset_id, location)
```

### 4. 記錄審計日誌

```python
def log_asset_operation(operation, asset_id, location, instance_id):
    """記錄資產操作"""
    log_entry = {
        "timestamp": datetime.now().isoformat(),
        "operation": operation,
        "asset_id": asset_id,
        "location": location,
        "instance_id": instance_id
    }
    
    # 寫入審計日誌
    redis_client.lpush(
        f"audit:asset:{asset_id}",
        json.dumps(log_entry)
    )
    
    # 保留最近 1000 筆記錄
    redis_client.ltrim(f"audit:asset:{asset_id}", 0, 999)
```

### 5. 定期清理過期資料

```python
def cleanup_expired_registrations():
    """清理過期的資產註冊"""
    # 使用 Redis 的 TTL 機制
    # 或定期掃描並刪除過期資料
    pass
```

## 總結

確保 NDH 中物件唯一性的關鍵策略:

1. **多層識別符**: 結合人類可讀 ID、UUID 和實體序號
2. **分散式鎖**: 使用 Redis 確保並發安全
3. **版本控制**: 解決資料衝突
4. **來源優先級**: 處理多來源資料
5. **Leader 選舉**: 避免腦裂問題
6. **審計日誌**: 追蹤所有操作
7. **冪等性**: 確保操作可重複執行

這些機制共同確保了 NDH 在複雜的分散式環境中能夠可靠地維護物件唯一性。

---

**作者**: 林志錚 (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**最後更新**: 2025-01-09

