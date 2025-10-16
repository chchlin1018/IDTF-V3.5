# NDH 高可用性與災難復原機制

**版本**: 1.0  
**日期**: 2025年10月14日  
**作者**: IDTF Consortium

---

## 1. 概述

**NDH (Neutral Data Hub)** 作為工業數位分身框架的核心執行時組件，必須確保在各種故障情境下仍能維持服務連續性和數據完整性。本文件詳細說明 NDH 的高可用性架構、鏡像備援機制、系統快照與復原機制，以及災難復原策略。

### 1.1 核心設計原則

1. **無單點故障 (No Single Point of Failure, SPOF)**：所有關鍵組件均採用冗餘部署
2. **自動故障轉移 (Automatic Failover)**：故障檢測與切換在秒級完成
3. **數據一致性保證**：確保系統狀態和數據在故障前後保持一致
4. **快速復原能力**：支援系統快照和狀態復原，最小化停機時間
5. **可觀測性**：全面的監控、告警和日誌記錄

---

## 2. 高可用性架構

### 2.1 整體架構

NDH 高可用性架構採用 **Active-Active** 或 **Active-Standby** 模式，根據組件特性選擇最適合的部署方式。

```
┌─────────────────────────────────────────────────────────────┐
│                    負載均衡層 (HAProxy/NGINX)                │
│                  VIP: 10.0.0.100 (Keepalived)               │
└────────────┬────────────────────────────────┬───────────────┘
             │                                │
    ┌────────▼────────┐              ┌────────▼────────┐
    │  NDH Master 1   │              │  NDH Master 2   │
    │   (Active)      │◄────────────►│   (Standby)     │
    │  API Gateway    │  Heartbeat   │  API Gateway    │
    │  Scheduler      │              │  Scheduler      │
    └────────┬────────┘              └────────┬────────┘
             │                                │
    ┌────────▼────────────────────────────────▼────────┐
    │           服務發現與配置中心 (etcd Cluster)        │
    │              Node 1, Node 2, Node 3              │
    └────────┬────────────────────────────────┬────────┘
             │                                │
    ┌────────▼────────┐              ┌────────▼────────┐
    │  NDH Worker 1   │              │  NDH Worker 2   │
    │  Asset Servants │              │  Asset Servants │
    └────────┬────────┘              └────────┬────────┘
             │                                │
    ┌────────▼────────────────────────────────▼────────┐
    │         數據持久化層 (PostgreSQL + TSDB)          │
    │    Primary-Replica + Streaming Replication       │
    └──────────────────────────────────────────────────┘
```

### 2.2 組件高可用性配置

#### **2.2.1 NDH Master 節點**

- **部署模式**: Active-Standby (主備模式)
- **數量**: 至少 2 個節點（建議 3 個以支援多數決）
- **故障檢測**: 透過 Keepalived 或 etcd 的 leader election 機制
- **切換時間**: < 10 秒
- **狀態同步**: 透過 etcd 共享叢集狀態

**配置範例 (Kubernetes)**:

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: ndh-master
spec:
  replicas: 3
  selector:
    matchLabels:
      app: ndh-master
  template:
    metadata:
      labels:
        app: ndh-master
    spec:
      affinity:
        podAntiAffinity:
          requiredDuringSchedulingIgnoredDuringExecution:
          - labelSelector:
              matchExpressions:
              - key: app
                operator: In
                values:
                - ndh-master
            topologyKey: "kubernetes.io/hostname"
      containers:
      - name: ndh-master
        image: idtf/ndh-master:v3.5.0
        env:
        - name: NDH_MODE
          value: "master"
        - name: ETCD_ENDPOINTS
          value: "http://etcd-0:2379,http://etcd-1:2379,http://etcd-2:2379"
        livenessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 10
        readinessProbe:
          httpGet:
            path: /ready
            port: 8080
          initialDelaySeconds: 10
          periodSeconds: 5
```

#### **2.2.2 NDH Worker 節點**

- **部署模式**: Active-Active (多活模式)
- **數量**: 根據資產數量動態擴展（建議每個 Worker 管理 500-1000 個 Asset Servants）
- **故障處理**: 當 Worker 節點故障時，Scheduler 自動將其管理的 Asset Servants 遷移到健康節點
- **負載均衡**: 根據 CPU、記憶體和網路負載動態分配新的 Asset Servants

#### **2.2.3 服務發現與配置中心 (etcd)**

- **部署模式**: Raft 共識協議的多數決叢集
- **數量**: 3 或 5 個節點（奇數以避免腦裂）
- **數據複製**: 自動同步到所有節點
- **故障容忍**: 可容忍 (N-1)/2 個節點故障

#### **2.2.4 數據持久化層**

**PostgreSQL (元數據與配置)**:
- **部署模式**: Primary-Replica with Streaming Replication
- **數量**: 1 個 Primary + 2 個 Replica
- **自動故障轉移**: 使用 Patroni 或 Stolon 實現自動提升 Replica 為 Primary
- **備份策略**: 每日全量備份 + 連續 WAL 歸檔

**時序數據庫 (InfluxDB/TDengine)**:
- **部署模式**: 叢集模式（根據數據庫類型）
- **數據複製因子**: 3（確保數據有 3 個副本）
- **分片策略**: 根據時間範圍和資產 ID 進行分片

---

## 3. 鏡像備援機制

### 3.1 數據鏡像策略

NDH 採用 **多層次鏡像備援** 策略，確保數據在不同層級都有冗餘副本。

#### **3.1.1 記憶體狀態鏡像**

- **Asset Servant 狀態**: 每個 Asset Servant 的當前狀態（屬性值、連接狀態、最後更新時間）會定期同步到 etcd
- **同步頻率**: 每 5 秒或狀態變化時立即同步
- **用途**: 當 Worker 節點故障時，新的 Worker 可從 etcd 讀取最後已知狀態並恢復 Asset Servant

**狀態同步機制**:

```python
# Asset Servant 狀態同步 (偽代碼)
class AssetServant:
    def __init__(self, asset_id, etcd_client):
        self.asset_id = asset_id
        self.etcd_client = etcd_client
        self.state = {}
        self.last_sync_time = None
    
    def update_state(self, key, value):
        self.state[key] = value
        self.state['last_updated'] = time.time()
        # 立即同步到 etcd
        self.sync_to_etcd()
    
    def sync_to_etcd(self):
        etcd_key = f"/ndh/assets/{self.asset_id}/state"
        self.etcd_client.put(etcd_key, json.dumps(self.state))
        self.last_sync_time = time.time()
    
    def restore_from_etcd(self):
        etcd_key = f"/ndh/assets/{self.asset_id}/state"
        state_json = self.etcd_client.get(etcd_key)
        if state_json:
            self.state = json.loads(state_json)
            return True
        return False
```

#### **3.1.2 數據庫層鏡像**

- **PostgreSQL**: 使用 Streaming Replication，Primary 節點的所有寫入操作會即時複製到 Replica 節點
- **時序數據庫**: 根據複製因子（Replication Factor = 3），每筆數據會自動複製到 3 個不同的節點

#### **3.1.3 跨地域鏡像（可選）**

對於多廠區部署，可配置跨地域的數據鏡像：

- **主站點**: 台灣廠區
- **備援站點**: 中國廠區
- **複製方式**: 非同步複製（避免網路延遲影響主站點效能）
- **復原時間目標 (RTO)**: < 15 分鐘
- **復原點目標 (RPO)**: < 5 分鐘

### 3.2 配置備份

所有系統配置（IADL 定義、FDL 佈局、NDH 配置）會自動備份到：

1. **Git 版本控制**: 所有 YAML 配置文件推送到 Git 儲存庫
2. **S3 物件儲存**: 定期將配置快照上傳到 S3 或 MinIO
3. **etcd 快照**: 每小時自動建立 etcd 快照並儲存到持久化儲存

---

## 4. 系統快照與復原機制

### 4.1 系統快照機制

NDH 提供 **一致性系統快照 (Consistent System Snapshot)** 功能，可在不停機的情況下建立整個系統的狀態快照。

#### **4.1.1 快照內容**

一個完整的系統快照包含：

1. **etcd 快照**: 包含所有服務發現、配置和 Asset Servant 狀態
2. **PostgreSQL 快照**: 包含元數據、事件日誌、使用者配置
3. **時序數據庫快照**: 包含指定時間範圍內的所有時間序列數據
4. **配置文件快照**: IADL、FDL、Helm values 等配置文件

#### **4.1.2 快照建立流程**

```bash
# 手動建立系統快照
ndh-admin snapshot create --name "snapshot-before-upgrade" --description "系統升級前快照"

# 自動定期快照（透過 CronJob）
apiVersion: batch/v1
kind: CronJob
metadata:
  name: ndh-snapshot
spec:
  schedule: "0 */6 * * *"  # 每 6 小時執行一次
  jobTemplate:
    spec:
      template:
        spec:
          containers:
          - name: snapshot
            image: idtf/ndh-admin:v3.5.0
            command:
            - /bin/sh
            - -c
            - |
              ndh-admin snapshot create --name "auto-snapshot-$(date +%Y%m%d-%H%M%S)" --retention-days 7
          restartPolicy: OnFailure
```

#### **4.1.3 快照一致性保證**

為確保快照的一致性，NDH 採用 **兩階段快照 (Two-Phase Snapshot)** 機制：

1. **Phase 1: 暫停寫入**
   - 向所有 Worker 節點發送 "準備快照" 訊號
   - Worker 節點完成當前事務後進入 "只讀模式"
   - 所有待寫入的數據先緩存在記憶體中

2. **Phase 2: 建立快照**
   - 同時建立 etcd、PostgreSQL、TSDB 的快照
   - 快照建立完成後，恢復寫入操作
   - 緩存的數據寫入數據庫

**暫停時間**: < 5 秒（對於大多數工業應用可接受）

### 4.2 系統復原機制

#### **4.2.1 復原場景**

系統快照復原適用於以下場景：

1. **突然停電**: 系統異常關機後，可復原到最近的一致性快照
2. **軟體升級失敗**: 升級後系統不穩定，回滾到升級前的快照
3. **配置錯誤**: 錯誤的配置變更導致系統異常，復原到正確配置的快照
4. **數據損壞**: 數據庫損壞或數據不一致，復原到已知良好狀態

#### **4.2.2 復原流程**

```bash
# 列出所有可用快照
ndh-admin snapshot list

# 輸出範例:
# SNAPSHOT NAME                    CREATED AT           SIZE      STATUS
# snapshot-before-upgrade          2025-10-14 10:00:00  2.3 GB    READY
# auto-snapshot-20251014-040000    2025-10-14 04:00:00  2.1 GB    READY
# auto-snapshot-20251013-220000    2025-10-13 22:00:00  2.0 GB    READY

# 復原到指定快照
ndh-admin snapshot restore --name "snapshot-before-upgrade" --confirm

# 復原流程:
# 1. 停止所有 NDH 服務
# 2. 復原 etcd 快照
# 3. 復原 PostgreSQL 快照
# 4. 復原時序數據庫快照
# 5. 重新啟動 NDH 服務
# 6. 驗證系統狀態
```

#### **4.2.3 增量復原（快速復原）**

對於短時間內的故障（如停電 < 10 分鐘），NDH 支援 **增量復原** 模式：

1. **WAL 重放**: 從最近的快照開始，重放 PostgreSQL 的 WAL (Write-Ahead Log)
2. **時序數據補齊**: 從備援節點或邊緣節點補齊缺失的時間序列數據
3. **狀態重建**: 從 etcd 和數據庫重建 Asset Servant 的記憶體狀態

**復原時間**: 通常 < 2 分鐘

### 4.3 停電保護機制

#### **4.3.1 UPS 整合**

NDH 可整合 UPS (不斷電系統) 以應對突然停電：

```yaml
# NDH 配置: UPS 整合
ups:
  enabled: true
  protocol: "SNMP"  # 或 "USB", "Network UPS Tools (NUT)"
  host: "192.168.1.100"
  community: "public"
  
  # 當 UPS 電池剩餘時間 < 5 分鐘時觸發
  low_battery_threshold: 300  # 秒
  
  actions:
    on_low_battery:
      - create_snapshot  # 立即建立快照
      - graceful_shutdown  # 優雅關機
```

#### **4.3.2 優雅關機流程**

當檢測到 UPS 電池即將耗盡時，NDH 會執行以下步驟：

1. **停止接受新請求**: API Gateway 返回 503 Service Unavailable
2. **完成進行中的事務**: 等待所有進行中的寫入操作完成（最多等待 30 秒）
3. **建立緊急快照**: 快速建立系統狀態快照
4. **同步數據到磁碟**: 強制將所有緩存數據寫入磁碟
5. **關閉服務**: 依序關閉 Worker → Master → 數據庫

#### **4.3.3 快速啟動與狀態恢復**

當電力恢復後，NDH 會自動執行以下步驟：

1. **檢測最後快照**: 識別最近的一致性快照
2. **驗證數據完整性**: 檢查數據庫和 etcd 的數據完整性
3. **自動復原**: 如果檢測到數據不一致，自動從最後快照復原
4. **重新連接設備**: 重新建立與物理設備的連接
5. **同步時間差**: 從設備或時序數據庫補齊停機期間的數據

**啟動時間**: 從電力恢復到系統完全可用 < 5 分鐘

---

## 5. 故障檢測與自動恢復

### 5.1 健康檢查機制

NDH 實施多層次的健康檢查：

#### **5.1.1 節點級健康檢查**

```yaml
# Kubernetes Liveness Probe
livenessProbe:
  httpGet:
    path: /health
    port: 8080
  initialDelaySeconds: 30
  periodSeconds: 10
  timeoutSeconds: 5
  failureThreshold: 3

# Kubernetes Readiness Probe
readinessProbe:
  httpGet:
    path: /ready
    port: 8080
  initialDelaySeconds: 10
  periodSeconds: 5
  timeoutSeconds: 3
  failureThreshold: 2
```

#### **5.1.2 服務級健康檢查**

- **API Gateway**: 每秒檢查 API 回應時間和錯誤率
- **Asset Servant**: 每 30 秒檢查與物理設備的連接狀態
- **數據庫**: 每分鐘檢查連接池狀態和查詢延遲

#### **5.1.3 業務級健康檢查**

- **數據新鮮度**: 檢查關鍵資產的數據更新時間，超過閾值則告警
- **告警觸發率**: 監控告警觸發頻率，異常增加可能表示系統問題
- **端到端延遲**: 定期執行端到端測試，測量從設備到 API 的完整延遲

### 5.2 自動故障轉移

#### **5.2.1 Master 節點故障轉移**

```
時間軸:
T0: Master 1 (Active) 停止回應心跳
T0+3s: Keepalived 檢測到故障
T0+5s: Master 2 (Standby) 被提升為 Active
T0+8s: 負載均衡器更新路由，流量切換到 Master 2
T0+10s: 系統完全恢復正常
```

#### **5.2.2 Worker 節點故障轉移**

```
時間軸:
T0: Worker 1 停止回應心跳
T0+10s: Master 檢測到 Worker 1 故障
T0+15s: Master 從 etcd 讀取 Worker 1 管理的 Asset Servants 列表
T0+20s: Master 將這些 Asset Servants 重新分配到 Worker 2 和 Worker 3
T0+30s: 新的 Worker 從 etcd 恢復 Asset Servant 狀態
T0+45s: Asset Servants 重新連接到物理設備
T0+60s: 系統完全恢復正常
```

#### **5.2.3 數據庫故障轉移**

**PostgreSQL (使用 Patroni)**:

```
時間軸:
T0: Primary 節點故障
T0+5s: Patroni 檢測到 Primary 不可用
T0+10s: Patroni 在 Replica 之間進行 leader election
T0+15s: 選出的 Replica 被提升為新的 Primary
T0+20s: 其他 Replica 重新配置為從新 Primary 複製
T0+25s: 應用程式連接池更新 Primary 地址
T0+30s: 數據庫服務完全恢復
```

### 5.3 告警與通知

NDH 整合多種告警通道：

```yaml
# 告警配置
alerting:
  channels:
    - type: email
      recipients: ["ops-team@company.com"]
      severity: ["critical", "high"]
    
    - type: slack
      webhook_url: "https://hooks.slack.com/services/XXX"
      channel: "#ndh-alerts"
      severity: ["critical", "high", "medium"]
    
    - type: pagerduty
      api_key: "YOUR_PAGERDUTY_KEY"
      severity: ["critical"]
  
  rules:
    - name: "Master Node Down"
      condition: "ndh_master_up == 0"
      severity: "critical"
      message: "NDH Master 節點 {{$labels.instance}} 已離線"
    
    - name: "Worker Node Down"
      condition: "ndh_worker_up == 0"
      severity: "high"
      message: "NDH Worker 節點 {{$labels.instance}} 已離線"
    
    - name: "Database Connection Failed"
      condition: "ndh_db_connection_errors > 10"
      severity: "critical"
      message: "數據庫連接失敗次數過多"
    
    - name: "Asset Servant Offline"
      condition: "ndh_asset_servant_offline_count > 50"
      severity: "high"
      message: "超過 50 個 Asset Servants 離線"
```

---

## 6. 災難復原計畫 (Disaster Recovery Plan)

### 6.1 災難場景分類

| 災難等級 | 場景描述 | RTO | RPO | 復原策略 |
|---------|---------|-----|-----|---------|
| **Level 1** | 單一節點故障 | < 1 分鐘 | 0 | 自動故障轉移 |
| **Level 2** | 多節點故障 | < 5 分鐘 | < 1 分鐘 | 從快照復原 + 重新部署 |
| **Level 3** | 整個叢集故障 | < 15 分鐘 | < 5 分鐘 | 從備援站點啟動 |
| **Level 4** | 資料中心災難 | < 1 小時 | < 15 分鐘 | 切換到異地備援中心 |

### 6.2 備份策略

#### **6.2.1 備份類型與頻率**

| 備份類型 | 頻率 | 保留期限 | 儲存位置 |
|---------|------|---------|---------|
| **完整快照** | 每日 00:00 | 30 天 | 本地 + S3 |
| **增量快照** | 每 6 小時 | 7 天 | 本地 |
| **配置備份** | 每次變更時 | 永久 | Git + S3 |
| **WAL 歸檔** | 連續 | 7 天 | 本地 + S3 |
| **異地備份** | 每日 02:00 | 90 天 | 異地 S3 |

#### **6.2.2 備份驗證**

每週執行一次備份驗證測試：

```bash
# 自動化備份驗證腳本
#!/bin/bash

# 1. 選擇最近的備份
LATEST_SNAPSHOT=$(ndh-admin snapshot list --format json | jq -r '.[0].name')

# 2. 在測試環境中復原
ndh-admin snapshot restore --name "$LATEST_SNAPSHOT" --target test-cluster

# 3. 驗證數據完整性
ndh-admin verify --cluster test-cluster --check-all

# 4. 執行煙霧測試
ndh-admin smoke-test --cluster test-cluster

# 5. 清理測試環境
ndh-admin cluster delete --name test-cluster
```

### 6.3 災難復原演練

建議每季度執行一次完整的災難復原演練：

**演練步驟**:

1. **模擬災難**: 關閉主站點的所有 NDH 服務
2. **啟動備援站點**: 在備援站點啟動 NDH 叢集
3. **復原數據**: 從最近的異地備份復原數據
4. **驗證功能**: 執行完整的功能測試
5. **切換流量**: 將生產流量切換到備援站點
6. **記錄與改進**: 記錄復原時間和遇到的問題，更新復原計畫

---

## 7. 監控與可觀測性

### 7.1 關鍵指標監控

```yaml
# Prometheus 監控指標
metrics:
  # 系統健康度
  - ndh_cluster_health_score          # 叢集整體健康分數 (0-100)
  - ndh_master_up                     # Master 節點存活狀態
  - ndh_worker_up                     # Worker 節點存活狀態
  
  # 效能指標
  - ndh_api_request_duration_seconds  # API 請求延遲
  - ndh_api_request_total             # API 請求總數
  - ndh_asset_servant_count           # Asset Servant 總數
  - ndh_asset_servant_active_count    # 活躍的 Asset Servant 數量
  
  # 資源使用
  - ndh_cpu_usage_percent             # CPU 使用率
  - ndh_memory_usage_bytes            # 記憶體使用量
  - ndh_disk_usage_percent            # 磁碟使用率
  
  # 數據庫指標
  - ndh_db_connection_pool_active     # 活躍數據庫連接數
  - ndh_db_query_duration_seconds     # 數據庫查詢延遲
  - ndh_tsdb_write_throughput         # 時序數據庫寫入吞吐量
  
  # 高可用性指標
  - ndh_failover_count                # 故障轉移次數
  - ndh_snapshot_creation_duration    # 快照建立時間
  - ndh_snapshot_size_bytes           # 快照大小
```

### 7.2 Grafana 儀表板

建議建立以下 Grafana 儀表板：

1. **叢集概覽**: 顯示整體健康狀態、節點數量、資產數量
2. **效能監控**: API 延遲、吞吐量、資源使用率
3. **高可用性**: 故障轉移事件、快照狀態、備份狀態
4. **數據庫監控**: 連接池、查詢效能、複製延遲
5. **告警歷史**: 所有告警事件的時間軸和統計

---

## 8. 最佳實踐與建議

### 8.1 部署建議

1. **使用奇數個 etcd 節點**: 3 或 5 個節點以避免腦裂
2. **跨可用區部署**: 將節點分散到不同的可用區或機架
3. **定期演練**: 每季度執行災難復原演練
4. **監控告警**: 確保告警通道暢通，並有 24/7 值班人員
5. **文件更新**: 保持災難復原計畫和操作手冊的更新

### 8.2 效能優化

1. **使用 SSD**: 所有數據庫和 etcd 節點使用 NVMe SSD
2. **網路優化**: 使用 10Gbps 或更高速度的網路連接
3. **資源預留**: 為故障轉移預留 20-30% 的資源容量
4. **快照壓縮**: 啟用快照壓縮以減少儲存空間和傳輸時間

### 8.3 安全性考量

1. **加密傳輸**: 所有節點間通訊使用 TLS 加密
2. **加密儲存**: 備份數據使用 AES-256 加密
3. **存取控制**: 使用 RBAC 限制對快照和復原功能的存取
4. **稽核日誌**: 記錄所有快照建立、復原和配置變更操作

---

## 9. 故障排除指南

### 9.1 常見問題

#### **問題 1: Master 節點無法完成故障轉移**

**症狀**: Standby Master 無法被提升為 Active

**可能原因**:
- etcd 叢集不健康
- 網路分區導致腦裂
- Keepalived 配置錯誤

**解決方案**:
```bash
# 檢查 etcd 健康狀態
etcdctl endpoint health --cluster

# 檢查 Keepalived 狀態
systemctl status keepalived

# 手動觸發故障轉移
ndh-admin failover --force --target master-2
```

#### **問題 2: 快照復原後數據不一致**

**症狀**: 復原後部分資產狀態不正確

**可能原因**:
- 快照建立時未正確暫停寫入
- WAL 日誌損壞
- 時序數據庫複製延遲

**解決方案**:
```bash
# 驗證快照完整性
ndh-admin snapshot verify --name <snapshot-name>

# 從 WAL 重新同步
ndh-admin recovery --mode wal-replay --from <snapshot-name>

# 從備援節點補齊數據
ndh-admin sync --source replica-node --target primary-node
```

#### **問題 3: Worker 節點故障後 Asset Servants 無法遷移**

**症狀**: Worker 故障後，Asset Servants 未在其他節點上啟動

**可能原因**:
- etcd 中缺少 Asset Servant 狀態
- 其他 Worker 節點資源不足
- 網路連接問題

**解決方案**:
```bash
# 檢查 etcd 中的 Asset Servant 狀態
etcdctl get --prefix /ndh/assets/

# 檢查 Worker 節點資源
ndh-admin worker list --show-resources

# 手動觸發遷移
ndh-admin migrate --from worker-1 --to worker-2,worker-3
```

---

## 10. 總結

NDH 的高可用性與災難復原機制確保了工業數位分身系統在各種故障情境下的穩定運行。透過多層次的冗餘設計、自動故障轉移、系統快照與復原機制，NDH 能夠達到：

- **99.9% 以上的可用性** (每年停機時間 < 8.76 小時)
- **秒級故障檢測與分鐘級故障恢復**
- **零數據丟失或最小數據丟失** (RPO < 5 分鐘)
- **快速災難復原能力** (RTO < 15 分鐘)

這些機制使 NDH 成為一個 **安全、穩健、可靠** 的工業級數位分身平台，能夠滿足關鍵任務應用的嚴格要求。

---

**附錄**:
- [附錄 A: 快照格式規範](./appendix/snapshot_format_spec.md)
- [附錄 B: 災難復原演練檢查清單](./appendix/dr_drill_checklist.md)
- [附錄 C: 監控告警規則完整列表](./appendix/alerting_rules.md)

