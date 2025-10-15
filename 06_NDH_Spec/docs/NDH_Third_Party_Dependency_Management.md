# NDH 第三方依賴管理機制

**版本**: 1.0.0  
**日期**: 2025-10-15  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin (Chih Cheng Lin)

---

## 目錄

1. [概述](#概述)
2. [第三方依賴清單](#第三方依賴清單)
3. [依賴管理策略](#依賴管理策略)
4. [版本相容性矩陣](#版本相容性矩陣)
5. [介面變動檢測機制](#介面變動檢測機制)
6. [自動化測試框架](#自動化測試框架)
7. [依賴更新流程](#依賴更新流程)
8. [回滾機制](#回滾機制)
9. [監控與告警](#監控與告警)
10. [最佳實踐](#最佳實踐)
11. [附錄](#附錄)

---

## 概述

### 背景

NDH (Neutral Data Hub) 是一個複雜的分散式系統，依賴多個第三方開源軟體和商業軟體。這些第三方軟體會定期發布新版本，可能包含：

- **功能增強**: 新功能、效能改進
- **安全性修復**: CVE 漏洞修補
- **Bug 修復**: 已知問題修復
- **介面變動**: API 變更、配置格式變更、行為變更

**風險**:
- ❌ 介面變動導致 NDH 無法正常運作
- ❌ 相容性問題導致資料丟失或損壞
- ❌ 效能退化或穩定性下降
- ❌ 安全漏洞未及時修補

### 目標

本文件旨在建立一套完整的第三方依賴管理機制，確保：

1. ✅ **及時更新**: 快速應用安全性修復和重要 Bug 修復
2. ✅ **安全更新**: 確保更新不會破壞現有功能
3. ✅ **介面相容**: 自動檢測介面變動並調整適配層
4. ✅ **快速回滾**: 更新失敗時能夠快速回滾
5. ✅ **持續監控**: 即時監控依賴軟體的健康狀態

---

## 第三方依賴清單

### 核心依賴

NDH 依賴以下核心第三方軟體：

| 類別 | 軟體 | 當前版本 | 用途 | 更新頻率 | 風險等級 |
|------|------|---------|------|---------|---------|
| **容器編排** | Kubernetes | 1.28.x | 容器編排與管理 | 每季 | 高 |
| **訊息佇列** | Apache Kafka | 3.6.x | 事件串流平台 | 每季 | 高 |
| **時序資料庫** | InfluxDB | 2.7.x | 時序資料儲存 | 每月 | 高 |
| **關聯式資料庫** | PostgreSQL | 16.x | 關聯式資料儲存 | 每年 | 中 |
| **分散式協調** | etcd | 3.5.x | 分散式鍵值儲存 | 每季 | 高 |
| **服務網格** | Istio | 1.20.x | 服務間通訊管理 | 每季 | 中 |
| **監控系統** | Prometheus | 2.48.x | 指標收集與監控 | 每月 | 低 |
| **視覺化** | Grafana | 10.2.x | 資料視覺化 | 每月 | 低 |
| **日誌系統** | Elasticsearch | 8.11.x | 日誌儲存與搜尋 | 每月 | 中 |
| **容器運行時** | containerd | 1.7.x | 容器運行時 | 每季 | 中 |

### 輔助依賴

| 類別 | 軟體 | 當前版本 | 用途 | 更新頻率 | 風險等級 |
|------|------|---------|------|---------|---------|
| **反向代理** | NGINX | 1.25.x | HTTP 反向代理 | 每月 | 低 |
| **負載均衡** | HAProxy | 2.9.x | TCP/HTTP 負載均衡 | 每季 | 低 |
| **快取** | Redis | 7.2.x | 快取與會話儲存 | 每季 | 中 |
| **物件儲存** | MinIO | RELEASE.2024-01 | S3 相容物件儲存 | 每月 | 低 |
| **憑證管理** | cert-manager | 1.13.x | TLS 憑證管理 | 每季 | 低 |

### Python 依賴

NDH 的 Python 組件依賴以下套件（部分關鍵套件）：

| 套件 | 當前版本 | 用途 | 更新頻率 | 風險等級 |
|------|---------|------|---------|---------|
| kafka-python | 2.0.x | Kafka 客戶端 | 每季 | 高 |
| influxdb-client | 1.39.x | InfluxDB 客戶端 | 每月 | 高 |
| psycopg2 | 2.9.x | PostgreSQL 客戶端 | 每季 | 中 |
| kubernetes | 28.x | Kubernetes 客戶端 | 每季 | 高 |
| fastapi | 0.105.x | Web 框架 | 每月 | 中 |
| pydantic | 2.5.x | 資料驗證 | 每月 | 中 |

---

## 依賴管理策略

### 1. 版本固定策略

**原則**: 使用精確版本號，避免自動升級

```yaml
# 錯誤示範 (會自動升級到最新版本)
kafka: latest
influxdb: 2.x

# 正確示範 (固定版本)
kafka: 3.6.1
influxdb: 2.7.4
```

**實作**:
- 使用 `requirements.txt` 固定 Python 套件版本
- 使用 Helm Chart `values.yaml` 固定容器映像版本
- 使用 `package-lock.json` 固定 Node.js 套件版本

### 2. 語義化版本管理

遵循 [Semantic Versioning 2.0.0](https://semver.org/) 規範：

**版本格式**: `MAJOR.MINOR.PATCH`

- **MAJOR**: 不相容的 API 變更
- **MINOR**: 向後相容的功能新增
- **PATCH**: 向後相容的 Bug 修復

**更新策略**:

| 版本類型 | 更新策略 | 測試要求 | 審批流程 |
|---------|---------|---------|---------|
| **PATCH** | 快速更新 (< 1 週) | 單元測試 + 整合測試 | 技術主管審批 |
| **MINOR** | 標準更新 (2-4 週) | 完整測試套件 | 架構師審批 |
| **MAJOR** | 謹慎更新 (1-3 月) | 完整測試 + 金絲雀部署 | CTO 審批 |

### 3. 安全性優先策略

**原則**: 安全性修復優先於功能更新

**流程**:
1. 訂閱所有依賴軟體的安全公告
2. CVE 漏洞發布後 24 小時內評估影響
3. 高危漏洞 (CVSS ≥ 7.0) 在 7 天內修復
4. 中危漏洞 (CVSS 4.0-6.9) 在 30 天內修復
5. 低危漏洞 (CVSS < 4.0) 在下次例行更新時修復

### 4. 長期支援版本 (LTS) 優先

**原則**: 優先選擇 LTS 版本，避免頻繁升級

**LTS 版本選擇**:

| 軟體 | LTS 版本 | 支援期限 | 下一個 LTS |
|------|---------|---------|-----------|
| Kubernetes | 1.28 | 2024-12 | 1.29 (2024-12) |
| PostgreSQL | 16 | 2028-11 | 17 (2025-11) |
| Ubuntu (Host OS) | 22.04 LTS | 2027-04 | 24.04 LTS (2029-04) |

---

## 版本相容性矩陣

### NDH V3.5 相容性矩陣

| NDH 版本 | Kubernetes | Kafka | InfluxDB | PostgreSQL | etcd | Istio |
|---------|-----------|-------|----------|-----------|------|-------|
| **3.5.0** | 1.26-1.28 | 3.4-3.6 | 2.6-2.7 | 14-16 | 3.4-3.5 | 1.18-1.20 |
| **3.5.1** | 1.27-1.29 | 3.5-3.7 | 2.7-2.8 | 15-16 | 3.5 | 1.19-1.21 |
| **3.6.0** (計畫) | 1.28-1.30 | 3.6-3.8 | 2.7-2.9 | 16-17 | 3.5-3.6 | 1.20-1.22 |

### 測試矩陣

NDH 在以下組合上進行完整測試：

**主要測試組合** (每次發布前測試):
- Kubernetes 1.28 + Kafka 3.6 + InfluxDB 2.7 + PostgreSQL 16
- Kubernetes 1.27 + Kafka 3.5 + InfluxDB 2.7 + PostgreSQL 15

**次要測試組合** (每季測試):
- 所有相容性矩陣中的其他組合

---

## 介面變動檢測機制

### 1. API 介面檢測

**目標**: 自動檢測第三方軟體的 API 介面變動

**實作**:

#### Kafka API 檢測

```python
# kafka_api_compatibility_test.py
import kafka
from kafka import KafkaProducer, KafkaConsumer
import inspect

class KafkaAPICompatibilityTest:
    """檢測 Kafka Python 客戶端的 API 相容性"""
    
    def __init__(self, expected_version="2.0.2"):
        self.expected_version = expected_version
        self.current_version = kafka.__version__
        
    def test_producer_api(self):
        """測試 Producer API"""
        required_methods = [
            'send',
            'flush',
            'close',
            'partitions_for',
            'metrics'
        ]
        
        for method in required_methods:
            if not hasattr(KafkaProducer, method):
                raise AssertionError(
                    f"KafkaProducer missing required method: {method}"
                )
            
            # 檢查方法簽名
            sig = inspect.signature(getattr(KafkaProducer, method))
            # 驗證參數...
            
    def test_consumer_api(self):
        """測試 Consumer API"""
        required_methods = [
            'subscribe',
            'poll',
            'commit',
            'close',
            'seek',
            'position'
        ]
        
        for method in required_methods:
            if not hasattr(KafkaConsumer, method):
                raise AssertionError(
                    f"KafkaConsumer missing required method: {method}"
                )
                
    def test_message_format(self):
        """測試訊息格式"""
        # 驗證 ConsumerRecord 結構
        from kafka.consumer.fetcher import ConsumerRecord
        required_fields = [
            'topic', 'partition', 'offset', 
            'timestamp', 'key', 'value'
        ]
        
        for field in required_fields:
            if field not in ConsumerRecord._fields:
                raise AssertionError(
                    f"ConsumerRecord missing required field: {field}"
                )
```

#### InfluxDB API 檢測

```python
# influxdb_api_compatibility_test.py
from influxdb_client import InfluxDBClient
from influxdb_client.client.write_api import SYNCHRONOUS
import inspect

class InfluxDBAPICompatibilityTest:
    """檢測 InfluxDB Python 客戶端的 API 相容性"""
    
    def test_write_api(self):
        """測試 Write API"""
        client = InfluxDBClient(url="http://localhost:8086", token="test")
        write_api = client.write_api(write_options=SYNCHRONOUS)
        
        required_methods = ['write', 'close']
        for method in required_methods:
            if not hasattr(write_api, method):
                raise AssertionError(
                    f"WriteApi missing required method: {method}"
                )
                
    def test_query_api(self):
        """測試 Query API"""
        client = InfluxDBClient(url="http://localhost:8086", token="test")
        query_api = client.query_api()
        
        required_methods = ['query', 'query_stream', 'query_data_frame']
        for method in required_methods:
            if not hasattr(query_api, method):
                raise AssertionError(
                    f"QueryApi missing required method: {method}"
                )
```

### 2. 配置格式檢測

**目標**: 檢測配置檔案格式變動

```python
# config_compatibility_test.py
import yaml
import json
from jsonschema import validate, ValidationError

class ConfigCompatibilityTest:
    """檢測配置格式相容性"""
    
    def __init__(self):
        # 定義預期的配置 Schema
        self.kafka_config_schema = {
            "type": "object",
            "required": ["bootstrap.servers"],
            "properties": {
                "bootstrap.servers": {"type": "string"},
                "client.id": {"type": "string"},
                "acks": {"type": "string", "enum": ["0", "1", "all"]},
                "compression.type": {
                    "type": "string", 
                    "enum": ["none", "gzip", "snappy", "lz4", "zstd"]
                }
            }
        }
        
    def test_kafka_config(self, config_file):
        """測試 Kafka 配置格式"""
        with open(config_file, 'r') as f:
            config = yaml.safe_load(f)
            
        try:
            validate(instance=config, schema=self.kafka_config_schema)
        except ValidationError as e:
            raise AssertionError(f"Kafka config validation failed: {e}")
```

### 3. 行為變動檢測

**目標**: 檢測軟體行為變動（即使 API 未變）

```python
# behavior_compatibility_test.py
import time
from kafka import KafkaProducer, KafkaConsumer

class BehaviorCompatibilityTest:
    """檢測行為相容性"""
    
    def test_kafka_message_ordering(self):
        """測試 Kafka 訊息順序保證"""
        producer = KafkaProducer(
            bootstrap_servers='localhost:9092',
            acks='all'
        )
        
        # 發送 100 條訊息到同一個 partition
        for i in range(100):
            producer.send('test-topic', 
                         value=f'message-{i}'.encode(),
                         partition=0)
        producer.flush()
        
        # 消費訊息並驗證順序
        consumer = KafkaConsumer(
            'test-topic',
            bootstrap_servers='localhost:9092',
            auto_offset_reset='earliest'
        )
        
        received = []
        for msg in consumer:
            received.append(msg.value.decode())
            if len(received) == 100:
                break
                
        expected = [f'message-{i}' for i in range(100)]
        if received != expected:
            raise AssertionError("Message ordering not preserved")
            
    def test_influxdb_timestamp_precision(self):
        """測試 InfluxDB 時間戳精度"""
        from influxdb_client import InfluxDBClient, Point
        from influxdb_client.client.write_api import SYNCHRONOUS
        
        client = InfluxDBClient(url="http://localhost:8086", token="test")
        write_api = client.write_api(write_options=SYNCHRONOUS)
        
        # 寫入納秒精度的時間戳
        point = Point("test").field("value", 1.0).time(1234567890123456789)
        write_api.write(bucket="test", record=point)
        
        # 查詢並驗證精度
        query_api = client.query_api()
        result = query_api.query('from(bucket:"test") |> range(start: -1h)')
        
        # 驗證時間戳精度...
```

---

## 自動化測試框架

### 測試層級

```
┌─────────────────────────────────────┐
│   E2E Tests (端到端測試)             │
│   - 完整工作流測試                    │
│   - 跨組件整合測試                    │
└─────────────────────────────────────┘
              ↑
┌─────────────────────────────────────┐
│   Integration Tests (整合測試)       │
│   - NDH ↔ Kafka 整合                │
│   - NDH ↔ InfluxDB 整合             │
│   - NDH ↔ PostgreSQL 整合           │
└─────────────────────────────────────┘
              ↑
┌─────────────────────────────────────┐
│   Compatibility Tests (相容性測試)   │
│   - API 介面檢測                     │
│   - 配置格式檢測                     │
│   - 行為變動檢測                     │
└─────────────────────────────────────┘
              ↑
┌─────────────────────────────────────┐
│   Unit Tests (單元測試)              │
│   - NDH 組件單元測試                 │
└─────────────────────────────────────┘
```

### 測試自動化

**CI/CD 整合**:

```yaml
# .github/workflows/dependency-compatibility-test.yml
name: Dependency Compatibility Test

on:
  schedule:
    # 每週一早上 8:00 執行
    - cron: '0 8 * * 1'
  workflow_dispatch:

jobs:
  test-kafka-compatibility:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        kafka-version: ['3.4.1', '3.5.2', '3.6.1', '3.7.0']
    steps:
      - uses: actions/checkout@v3
      
      - name: Start Kafka ${{ matrix.kafka-version }}
        run: |
          docker run -d --name kafka \
            -p 9092:9092 \
            confluentinc/cp-kafka:${{ matrix.kafka-version }}
            
      - name: Run Compatibility Tests
        run: |
          pytest tests/compatibility/test_kafka_api.py
          pytest tests/integration/test_ndh_kafka.py
          
      - name: Report Results
        if: failure()
        uses: actions/github-script@v6
        with:
          script: |
            github.rest.issues.create({
              owner: context.repo.owner,
              repo: context.repo.repo,
              title: 'Kafka ${{ matrix.kafka-version }} Compatibility Issue',
              body: 'Compatibility tests failed for Kafka ${{ matrix.kafka-version }}'
            })

  test-influxdb-compatibility:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        influxdb-version: ['2.6.1', '2.7.4', '2.8.0']
    steps:
      - uses: actions/checkout@v3
      
      - name: Start InfluxDB ${{ matrix.influxdb-version }}
        run: |
          docker run -d --name influxdb \
            -p 8086:8086 \
            influxdb:${{ matrix.influxdb-version }}
            
      - name: Run Compatibility Tests
        run: |
          pytest tests/compatibility/test_influxdb_api.py
          pytest tests/integration/test_ndh_influxdb.py
```

---

## 依賴更新流程

### 標準更新流程

```
┌─────────────────┐
│ 1. 評估階段      │
│ - 檢查變更日誌   │
│ - 評估影響範圍   │
│ - 決定更新優先級 │
└────────┬────────┘
         ↓
┌─────────────────┐
│ 2. 測試階段      │
│ - 相容性測試     │
│ - 整合測試       │
│ - 效能測試       │
└────────┬────────┘
         ↓
┌─────────────────┐
│ 3. 適配階段      │
│ - 修改適配層     │
│ - 更新配置       │
│ - 更新文檔       │
└────────┬────────┘
         ↓
┌─────────────────┐
│ 4. 部署階段      │
│ - 金絲雀部署     │
│ - 灰度發布       │
│ - 全面部署       │
└────────┬────────┘
         ↓
┌─────────────────┐
│ 5. 驗證階段      │
│ - 功能驗證       │
│ - 效能驗證       │
│ - 穩定性驗證     │
└────────┬────────┘
         ↓
┌─────────────────┐
│ 6. 監控階段      │
│ - 持續監控 7 天  │
│ - 收集反饋       │
│ - 記錄問題       │
└─────────────────┘
```

### 詳細步驟

#### 步驟 1: 評估階段

**1.1 檢查變更日誌**

```bash
# 自動化腳本：檢查依賴更新
#!/bin/bash
# check_dependency_updates.sh

DEPENDENCIES=(
  "kafka:3.6.1"
  "influxdb:2.7.4"
  "postgresql:16.1"
)

for dep in "${DEPENDENCIES[@]}"; do
  name=$(echo $dep | cut -d: -f1)
  current_version=$(echo $dep | cut -d: -f2)
  
  echo "Checking $name updates..."
  
  # 檢查 GitHub Releases
  latest_version=$(curl -s "https://api.github.com/repos/apache/$name/releases/latest" | jq -r .tag_name)
  
  if [ "$latest_version" != "$current_version" ]; then
    echo "⚠️  New version available: $name $latest_version (current: $current_version)"
    
    # 下載 CHANGELOG
    curl -s "https://raw.githubusercontent.com/apache/$name/$latest_version/CHANGELOG.md" > /tmp/${name}_changelog.md
    
    # 檢查是否有 breaking changes
    if grep -i "breaking" /tmp/${name}_changelog.md; then
      echo "🚨 BREAKING CHANGES detected!"
    fi
  fi
done
```

**1.2 評估影響範圍**

建立影響評估表：

| 依賴 | 當前版本 | 新版本 | 變更類型 | 影響範圍 | 優先級 | 預計工時 |
|------|---------|--------|---------|---------|--------|---------|
| Kafka | 3.6.1 | 3.7.0 | MINOR | Kafka Connector | 中 | 16 小時 |
| InfluxDB | 2.7.4 | 2.8.0 | MINOR | Time Series API | 中 | 8 小時 |
| PostgreSQL | 16.1 | 16.2 | PATCH | 無 | 低 | 2 小時 |

#### 步驟 2: 測試階段

**2.1 建立測試環境**

```bash
# setup_test_env.sh
#!/bin/bash

# 建立測試命名空間
kubectl create namespace ndh-test-kafka-3-7-0

# 部署新版本 Kafka
helm install kafka-test bitnami/kafka \
  --namespace ndh-test-kafka-3-7-0 \
  --set image.tag=3.7.0

# 部署 NDH (連接到新版本 Kafka)
helm install ndh-test ./ndh-chart \
  --namespace ndh-test-kafka-3-7-0 \
  --set kafka.bootstrapServers=kafka-test:9092
```

**2.2 執行相容性測試**

```bash
# 執行完整測試套件
pytest tests/compatibility/ -v
pytest tests/integration/ -v
pytest tests/e2e/ -v

# 執行效能測試
pytest tests/performance/ -v --benchmark-only
```

#### 步驟 3: 適配階段

**3.1 修改適配層**

如果檢測到介面變動，修改適配層：

```python
# ndh/adapters/kafka_adapter.py

class KafkaAdapter:
    """Kafka 適配層，隔離版本差異"""
    
    def __init__(self, version):
        self.version = version
        
    def create_producer(self, config):
        """建立 Producer，適配不同版本"""
        if self.version >= "3.7.0":
            # Kafka 3.7.0 新增了新的配置參數
            config['enable.idempotence'] = True
            config['max.in.flight.requests.per.connection'] = 5
        
        return KafkaProducer(**config)
        
    def send_message(self, producer, topic, message):
        """發送訊息，適配不同版本的回傳格式"""
        future = producer.send(topic, message)
        
        if self.version >= "3.7.0":
            # 3.7.0 的 RecordMetadata 格式有變化
            metadata = future.get(timeout=10)
            return {
                'topic': metadata.topic,
                'partition': metadata.partition,
                'offset': metadata.offset,
                'timestamp': metadata.timestamp
            }
        else:
            # 舊版本格式
            metadata = future.get(timeout=10)
            return {
                'topic': metadata.topic,
                'partition': metadata.partition,
                'offset': metadata.offset
            }
```

#### 步驟 4: 部署階段

**4.1 金絲雀部署**

```bash
# 金絲雀部署：5% 流量
kubectl patch deployment ndh-worker \
  -n production \
  -p '{"spec":{"template":{"spec":{"containers":[{"name":"ndh-worker","image":"ndh:3.5.1-kafka-3.7.0"}]}}}}'

kubectl scale deployment ndh-worker-canary --replicas=1 -n production
kubectl scale deployment ndh-worker --replicas=19 -n production

# 監控 10 分鐘
sleep 600

# 檢查錯誤率
error_rate=$(kubectl logs -l app=ndh-worker-canary -n production | grep ERROR | wc -l)
if [ $error_rate -gt 10 ]; then
  echo "Error rate too high, rolling back..."
  kubectl scale deployment ndh-worker-canary --replicas=0 -n production
  exit 1
fi
```

**4.2 灰度發布**

```bash
# 逐步增加新版本比例
# 5% → 25% → 50% → 100%

for percentage in 25 50 100; do
  new_replicas=$((20 * $percentage / 100))
  old_replicas=$((20 - $new_replicas))
  
  kubectl scale deployment ndh-worker-canary --replicas=$new_replicas -n production
  kubectl scale deployment ndh-worker --replicas=$old_replicas -n production
  
  echo "Waiting 30 minutes before next step..."
  sleep 1800
  
  # 檢查健康狀態
  # ...
done
```

#### 步驟 5: 驗證階段

**5.1 功能驗證**

```bash
# 執行冒煙測試
pytest tests/smoke/ -v

# 執行關鍵路徑測試
pytest tests/critical_path/ -v
```

**5.2 效能驗證**

```bash
# 比較更新前後的效能指標
python scripts/compare_performance.py \
  --before metrics/before_update.json \
  --after metrics/after_update.json
```

#### 步驟 6: 監控階段

**6.1 設置告警**

```yaml
# prometheus_alerts.yml
groups:
  - name: dependency_update_monitoring
    rules:
      - alert: HighErrorRateAfterUpdate
        expr: |
          rate(ndh_errors_total[5m]) > 0.01
        for: 5m
        labels:
          severity: critical
        annotations:
          summary: "High error rate detected after dependency update"
          
      - alert: PerformanceDegradation
        expr: |
          histogram_quantile(0.95, 
            rate(ndh_request_duration_seconds_bucket[5m])
          ) > 1.5 * histogram_quantile(0.95, 
            rate(ndh_request_duration_seconds_bucket[5m] offset 1d)
          )
        for: 10m
        labels:
          severity: warning
        annotations:
          summary: "Performance degradation detected after dependency update"
```

---

## 回滾機制

### 快速回滾

**觸發條件**:
- 錯誤率 > 5%
- P95 延遲增加 > 50%
- 系統可用性 < 99%
- 資料丟失或損壞

**回滾步驟**:

```bash
# 1. 立即回滾到上一個版本
kubectl rollout undo deployment/ndh-worker -n production

# 2. 驗證回滾成功
kubectl rollout status deployment/ndh-worker -n production

# 3. 檢查系統健康
kubectl get pods -n production -l app=ndh-worker

# 4. 驗證功能
pytest tests/smoke/ -v

# 5. 通知團隊
slack-notify "#ndh-ops" "Rolled back Kafka update due to high error rate"
```

### 資料回滾

如果依賴更新導致資料格式變更：

```bash
# 1. 停止寫入
kubectl scale deployment ndh-worker --replicas=0 -n production

# 2. 從快照恢復
ndh-admin snapshot restore --name "snapshot-before-kafka-update"

# 3. 驗證資料完整性
ndh-admin verify-data --start-time "2025-10-14T00:00:00Z"

# 4. 重啟服務
kubectl scale deployment ndh-worker --replicas=20 -n production
```

---

## 監控與告警

### 依賴健康監控

**監控指標**:

| 指標 | 說明 | 告警閾值 |
|------|------|---------|
| `dependency_version` | 當前依賴版本 | - |
| `dependency_api_errors` | API 呼叫錯誤數 | > 10/min |
| `dependency_latency_p95` | API 呼叫延遲 P95 | > 1s |
| `dependency_connection_failures` | 連接失敗次數 | > 5/min |
| `dependency_compatibility_test_failures` | 相容性測試失敗次數 | > 0 |

**Prometheus 監控**:

```yaml
# prometheus_rules.yml
groups:
  - name: dependency_health
    rules:
      - record: dependency_api_error_rate
        expr: |
          rate(dependency_api_errors_total[5m])
          
      - alert: DependencyAPIHighErrorRate
        expr: dependency_api_error_rate > 0.01
        for: 5m
        labels:
          severity: warning
        annotations:
          summary: "High API error rate for {{ $labels.dependency }}"
          
      - alert: DependencyConnectionFailure
        expr: |
          rate(dependency_connection_failures_total[5m]) > 0.1
        for: 2m
        labels:
          severity: critical
        annotations:
          summary: "Connection failures to {{ $labels.dependency }}"
```

### 版本追蹤

**記錄依賴版本**:

```python
# ndh/version_tracker.py
from prometheus_client import Info

dependency_version = Info(
    'dependency_version',
    'Version of third-party dependencies',
    ['dependency']
)

# 記錄版本
dependency_version.labels(dependency='kafka').info({
    'version': '3.6.1',
    'updated_at': '2025-10-14T10:00:00Z'
})
```

---

## 最佳實踐

### 1. 建立依賴清單

維護一份完整的依賴清單，包含：
- 依賴名稱和版本
- 用途說明
- 更新頻率
- 風險等級
- 負責人

### 2. 訂閱安全公告

訂閱所有依賴軟體的安全公告：
- GitHub Security Advisories
- CVE 資料庫
- 軟體官方郵件列表

### 3. 定期審查依賴

**每季度審查**:
- 檢查是否有新版本
- 評估是否需要更新
- 檢查是否有安全漏洞
- 清理不再使用的依賴

### 4. 使用依賴掃描工具

**推薦工具**:
- **Dependabot** (GitHub): 自動檢測依賴更新
- **Snyk**: 安全漏洞掃描
- **OWASP Dependency-Check**: 已知漏洞檢測
- **Trivy**: 容器映像掃描

### 5. 建立適配層

在 NDH 和第三方依賴之間建立適配層：
- 隔離版本差異
- 簡化升級過程
- 提高可測試性

### 6. 自動化測試

建立完整的自動化測試套件：
- 單元測試
- 相容性測試
- 整合測試
- 端到端測試
- 效能測試

### 7. 文檔化

記錄所有依賴更新：
- 更新原因
- 變更內容
- 測試結果
- 部署過程
- 遇到的問題和解決方案

---

## 附錄

### A. 依賴更新檢查清單

**更新前**:
- [ ] 檢查變更日誌
- [ ] 評估影響範圍
- [ ] 建立測試環境
- [ ] 執行相容性測試
- [ ] 修改適配層（如需要）
- [ ] 更新文檔
- [ ] 建立系統快照

**更新中**:
- [ ] 金絲雀部署
- [ ] 監控錯誤率
- [ ] 監控效能指標
- [ ] 灰度發布
- [ ] 功能驗證

**更新後**:
- [ ] 持續監控 7 天
- [ ] 收集反饋
- [ ] 記錄問題
- [ ] 更新依賴清單
- [ ] 更新文檔

### B. 常見問題 (FAQ)

**Q1: 如何決定是否更新依賴？**

A: 考慮以下因素：
- 是否有安全漏洞修復？（高優先級）
- 是否有重要 Bug 修復？（中優先級）
- 是否有需要的新功能？（低優先級）
- 更新成本和風險如何？

**Q2: 如何處理 Breaking Changes？**

A: 
1. 評估影響範圍
2. 修改適配層隔離變更
3. 完整測試
4. 分階段部署
5. 準備回滾方案

**Q3: 多久應該更新一次依賴？**

A:
- 安全性修復：立即更新
- PATCH 版本：每月
- MINOR 版本：每季
- MAJOR 版本：每年或按需

### C. 參考資源

- [Semantic Versioning 2.0.0](https://semver.org/)
- [OWASP Dependency-Check](https://owasp.org/www-project-dependency-check/)
- [GitHub Security Advisories](https://github.com/advisories)
- [CVE Database](https://cve.mitre.org/)
- [Kubernetes Version Skew Policy](https://kubernetes.io/releases/version-skew-policy/)

---

## 版本歷史

| 版本 | 日期 | 作者 | 變更說明 |
|------|------|------|---------|
| 1.0.0 | 2025-10-15 | 林志錚 Michael Lin | 初始版本 |

---

**© 2025 IDTF Consortium. All rights reserved.**

