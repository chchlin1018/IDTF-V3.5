# NDH 系統更新機制

**版本**: 1.0  
**日期**: 2025年10月14日  
**作者**: IDTF Consortium

---

## 1. 概述

NDH (Neutral Data Hub) 作為工業數位分身框架的核心執行時組件，必須支援安全、可靠、零停機的系統更新機制。本文件詳細說明 NDH 的系統更新策略、滾動更新流程、版本管理、回滾機制，以及更新驗證測試。

### 1.1 設計原則

1. **零停機更新 (Zero-Downtime Update)**：更新過程中系統持續提供服務
2. **可回滾性 (Rollback-able)**：任何更新都可以快速回滾到前一版本
3. **漸進式部署 (Progressive Deployment)**：分階段部署，及早發現問題
4. **自動化驗證 (Automated Validation)**：更新後自動執行健康檢查和煙霧測試
5. **版本相容性 (Version Compatibility)**：確保新舊版本在過渡期間可共存

### 1.2 更新類型

| 更新類型 | 描述 | 停機時間 | 回滾時間 | 風險等級 |
|---------|------|---------|---------|---------|
| **補丁更新 (Patch)** | 安全修復、Bug 修復 | 0 分鐘 | < 5 分鐘 | 低 |
| **次版本更新 (Minor)** | 新功能、效能改進 | 0 分鐘 | < 10 分鐘 | 中 |
| **主版本更新 (Major)** | 架構變更、破壞性變更 | < 5 分鐘 | < 30 分鐘 | 高 |
| **緊急熱修復 (Hotfix)** | 緊急安全漏洞修復 | 0 分鐘 | < 5 分鐘 | 中 |

---

## 2. 版本管理策略

### 2.1 版本號規範

NDH 採用 **語義化版本控制 (Semantic Versioning)** 規範：

```
MAJOR.MINOR.PATCH-PRERELEASE+BUILD

範例:
- v3.5.0          (正式版本)
- v3.5.1          (補丁版本)
- v3.6.0-beta.1   (測試版本)
- v4.0.0-rc.2     (候選版本)
- v3.5.0+20251014 (帶構建資訊)
```

**版本號規則**：
- **MAJOR**: 不相容的 API 變更或架構重大變更
- **MINOR**: 向後相容的新功能
- **PATCH**: 向後相容的 Bug 修復
- **PRERELEASE**: alpha, beta, rc (release candidate)
- **BUILD**: 構建日期或 Git commit hash

### 2.2 版本相容性矩陣

| NDH Master 版本 | 相容的 Worker 版本 | 相容的 IADL 版本 | 相容的 FDL 版本 |
|----------------|-------------------|-----------------|----------------|
| v3.5.x | v3.4.x - v3.5.x | v3.0+ | v3.0+ |
| v3.6.x | v3.5.x - v3.6.x | v3.0+ | v3.0+ |
| v4.0.x | v3.6.x - v4.0.x | v3.5+ | v3.5+ |

**相容性保證**：
- Master 節點可管理低一個 Minor 版本的 Worker 節點
- Worker 節點可連接高一個 Minor 版本的 Master 節點
- Major 版本更新可能破壞相容性，需要同步更新所有組件

### 2.3 版本發布週期

| 發布類型 | 頻率 | 內容 |
|---------|------|------|
| **補丁版本** | 每 2 週 | 安全修復、Bug 修復 |
| **次版本** | 每 3 個月 | 新功能、效能改進 |
| **主版本** | 每 12-18 個月 | 架構變更、重大功能 |
| **LTS (長期支援)** | 每 2 年 | 提供 3 年支援 |

---

## 3. 滾動更新機制

### 3.1 滾動更新流程

NDH 採用 **Kubernetes 滾動更新 (Rolling Update)** 策略，確保更新過程中服務不中斷。

```
┌─────────────────────────────────────────────────────────────┐
│                    滾動更新流程                              │
└─────────────────────────────────────────────────────────────┘

階段 1: 準備階段
  ├─ 建立系統快照
  ├─ 驗證新版本映像檔
  ├─ 檢查叢集健康狀態
  └─ 通知維運團隊

階段 2: 更新 Worker 節點 (批次更新)
  ├─ Batch 1: 更新 10% Worker 節點
  │   ├─ 標記節點為 "draining"
  │   ├─ 遷移 Asset Tag Instances 到其他節點
  │   ├─ 更新節點到新版本
  │   ├─ 健康檢查 (Readiness Probe)
  │   └─ 驗證通過後繼續
  │
  ├─ Batch 2: 更新 30% Worker 節點
  │   └─ (重複上述步驟)
  │
  └─ Batch 3: 更新剩餘 60% Worker 節點
      └─ (重複上述步驟)

階段 3: 更新 Master 節點 (逐一更新)
  ├─ 更新 Standby Master 節點
  │   ├─ 更新到新版本
  │   ├─ 健康檢查
  │   └─ 驗證通過後繼續
  │
  └─ 故障轉移到新版本 Master
      ├─ 將 Active Master 切換到已更新的節點
      ├─ 更新原 Active Master
      └─ 驗證叢集狀態

階段 4: 驗證與監控
  ├─ 執行煙霧測試
  ├─ 監控關鍵指標 (30 分鐘)
  ├─ 檢查錯誤日誌
  └─ 確認更新成功

階段 5: 清理與通知
  ├─ 清理舊版本映像檔
  ├─ 更新文件
  └─ 通知維運團隊更新完成
```

### 3.2 Kubernetes 滾動更新配置

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: ndh-master
spec:
  replicas: 3
  strategy:
    type: RollingUpdate
    rollingUpdate:
      maxUnavailable: 1      # 最多 1 個 Pod 不可用
      maxSurge: 1            # 最多額外 1 個 Pod
  minReadySeconds: 30        # 新 Pod 就緒後等待 30 秒
  revisionHistoryLimit: 10   # 保留 10 個歷史版本
  
  template:
    metadata:
      labels:
        app: ndh-master
        version: v3.6.0
    spec:
      containers:
      - name: ndh-master
        image: idtf/ndh-master:v3.6.0
        
        # 健康檢查
        livenessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 60
          periodSeconds: 10
          timeoutSeconds: 5
          failureThreshold: 3
        
        readinessProbe:
          httpGet:
            path: /ready
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 5
          timeoutSeconds: 3
          failureThreshold: 2
        
        # 優雅關機
        lifecycle:
          preStop:
            exec:
              command: ["/bin/sh", "-c", "sleep 15"]
        
        terminationGracePeriodSeconds: 60
```

### 3.3 Worker 節點更新策略

Worker 節點採用 **分批次滾動更新** 策略：

```bash
#!/bin/bash
# Worker 節點滾動更新腳本

# 階段 1: 更新 10% 節點 (金絲雀部署)
ndh-admin update workers \
  --version v3.6.0 \
  --batch-size 10% \
  --wait-time 10m \
  --health-check-timeout 5m

# 如果成功，繼續更新 30% 節點
ndh-admin update workers \
  --version v3.6.0 \
  --batch-size 30% \
  --wait-time 5m \
  --health-check-timeout 5m

# 最後更新剩餘 60% 節點
ndh-admin update workers \
  --version v3.6.0 \
  --batch-size 60% \
  --wait-time 5m \
  --health-check-timeout 5m
```

**更新過程中的 Asset Tag Instance 遷移**：

```python
# Asset Tag Instance 遷移邏輯 (偽代碼)
def migrate_asset_servants(source_worker, target_workers):
    """
    將 Asset Tag Instances 從源 Worker 遷移到目標 Workers
    """
    # 1. 獲取源 Worker 上的所有 Asset Tag Instances
    servants = get_asset_servants(source_worker)
    
    # 2. 將 Worker 標記為 "draining"
    mark_worker_draining(source_worker)
    
    # 3. 停止接受新的 Asset Tag Instances
    stop_accepting_new_servants(source_worker)
    
    # 4. 逐一遷移 Asset Tag Instances
    for servant in servants:
        # 選擇負載最低的目標 Worker
        target_worker = select_least_loaded_worker(target_workers)
        
        # 在目標 Worker 上建立 Asset Tag Instance
        create_servant_on_worker(target_worker, servant.config)
        
        # 等待新 Servant 就緒
        wait_for_servant_ready(target_worker, servant.id)
        
        # 在源 Worker 上停止 Asset Tag Instance
        stop_servant_on_worker(source_worker, servant.id)
    
    # 5. 確認所有 Asset Tag Instances 已遷移
    verify_all_servants_migrated(source_worker)
    
    # 6. 標記 Worker 為 "drained"
    mark_worker_drained(source_worker)
```

---

## 4. 藍綠部署 (Blue-Green Deployment)

對於風險較高的主版本更新，建議採用 **藍綠部署** 策略。

### 4.1 藍綠部署流程

```
┌─────────────────────────────────────────────────────────────┐
│                    藍綠部署架構                              │
└─────────────────────────────────────────────────────────────┘

                    ┌─────────────────┐
                    │  負載均衡器      │
                    │  (HAProxy)      │
                    └────────┬────────┘
                             │
                ┌────────────┴────────────┐
                │                         │
        ┌───────▼────────┐       ┌───────▼────────┐
        │   藍色環境      │       │   綠色環境      │
        │   (v3.5.0)     │       │   (v3.6.0)     │
        │   目前生產      │       │   新版本        │
        └───────┬────────┘       └───────┬────────┘
                │                         │
        ┌───────▼────────┐       ┌───────▼────────┐
        │  3x Master     │       │  3x Master     │
        │  6x Worker     │       │  6x Worker     │
        │  Database      │       │  Database      │
        └────────────────┘       └────────────────┘

部署流程:
1. 建立綠色環境 (新版本)
2. 在綠色環境中部署 v3.6.0
3. 執行完整測試
4. 將 10% 流量切換到綠色環境 (金絲雀測試)
5. 監控 30 分鐘，無問題則繼續
6. 將 100% 流量切換到綠色環境
7. 保留藍色環境 24 小時以備回滾
8. 確認無問題後，關閉藍色環境
```

### 4.2 藍綠切換腳本

```bash
#!/bin/bash
# 藍綠部署切換腳本

# 1. 建立綠色環境
ndh-admin deploy create-environment \
  --name green \
  --version v3.6.0 \
  --replicas 3 \
  --workers 6

# 2. 等待綠色環境就緒
ndh-admin deploy wait-ready \
  --environment green \
  --timeout 10m

# 3. 執行煙霧測試
ndh-admin test smoke \
  --environment green \
  --test-suite production

# 4. 金絲雀測試 (10% 流量)
ndh-admin traffic switch \
  --from blue \
  --to green \
  --percentage 10

# 5. 監控 30 分鐘
sleep 1800

# 6. 檢查錯誤率
ERROR_RATE=$(ndh-admin metrics get \
  --environment green \
  --metric error_rate \
  --duration 30m)

if [ "$ERROR_RATE" -lt "0.01" ]; then
  # 錯誤率 < 1%，繼續切換
  ndh-admin traffic switch \
    --from blue \
    --to green \
    --percentage 100
  
  echo "✅ 藍綠切換成功"
else
  # 錯誤率過高，回滾
  ndh-admin traffic switch \
    --from green \
    --to blue \
    --percentage 100
  
  echo "❌ 錯誤率過高，已回滾到藍色環境"
  exit 1
fi
```

---

## 5. 金絲雀部署 (Canary Deployment)

### 5.1 金絲雀部署策略

金絲雀部署是一種漸進式部署策略，先將新版本部署到一小部分節點，觀察穩定性後再逐步擴大範圍。

```
階段 1: 金絲雀 (5% 流量)
  ├─ 部署新版本到 5% 節點
  ├─ 監控 15 分鐘
  └─ 檢查關鍵指標

階段 2: 小規模部署 (25% 流量)
  ├─ 擴展到 25% 節點
  ├─ 監控 30 分鐘
  └─ 檢查關鍵指標

階段 3: 中規模部署 (50% 流量)
  ├─ 擴展到 50% 節點
  ├─ 監控 30 分鐘
  └─ 檢查關鍵指標

階段 4: 全面部署 (100% 流量)
  ├─ 擴展到 100% 節點
  ├─ 監控 60 分鐘
  └─ 確認更新成功
```

### 5.2 金絲雀部署配置

```yaml
# Flagger 金絲雀部署配置
apiVersion: flagger.app/v1beta1
kind: Canary
metadata:
  name: ndh-master
  namespace: idtf
spec:
  targetRef:
    apiVersion: apps/v1
    kind: Deployment
    name: ndh-master
  
  service:
    port: 8080
  
  # 金絲雀分析
  analysis:
    interval: 1m
    threshold: 5
    maxWeight: 50
    stepWeight: 10
    
    # 關鍵指標
    metrics:
    - name: request-success-rate
      thresholdRange:
        min: 99
      interval: 1m
    
    - name: request-duration
      thresholdRange:
        max: 500
      interval: 1m
    
    - name: error-rate
      thresholdRange:
        max: 1
      interval: 1m
    
    # Webhook 測試
    webhooks:
    - name: smoke-test
      url: http://ndh-test-runner/smoke
      timeout: 5m
      metadata:
        type: pre-rollout
    
    - name: load-test
      url: http://ndh-test-runner/load
      timeout: 10m
      metadata:
        type: rollout
```

---

## 6. 自動化更新流程

### 6.1 CI/CD 整合

```yaml
# GitLab CI/CD 配置範例
stages:
  - build
  - test
  - deploy-staging
  - deploy-canary
  - deploy-production

variables:
  VERSION: "3.6.0"
  REGISTRY: "registry.idtf.io"

# 構建階段
build:
  stage: build
  script:
    - docker build -t $REGISTRY/ndh-master:$VERSION .
    - docker push $REGISTRY/ndh-master:$VERSION
  only:
    - tags

# 測試階段
test:
  stage: test
  script:
    - pytest tests/
    - go test ./...
    - npm test
  coverage: '/TOTAL.*\s+(\d+%)$/'

# 部署到 Staging 環境
deploy-staging:
  stage: deploy-staging
  script:
    - ndh-admin deploy \
        --environment staging \
        --version $VERSION \
        --wait
    - ndh-admin test smoke \
        --environment staging
  only:
    - tags

# 金絲雀部署到生產環境
deploy-canary:
  stage: deploy-canary
  script:
    - ndh-admin deploy canary \
        --environment production \
        --version $VERSION \
        --traffic-percentage 5 \
        --wait-time 15m
  only:
    - tags
  when: manual

# 全面部署到生產環境
deploy-production:
  stage: deploy-production
  script:
    - ndh-admin deploy rollout \
        --environment production \
        --version $VERSION \
        --strategy progressive
  only:
    - tags
  when: manual
```

### 6.2 自動化更新腳本

```bash
#!/bin/bash
# NDH 自動化更新腳本

set -e

VERSION=$1
ENVIRONMENT=${2:-production}

echo "🚀 開始更新 NDH 到版本 $VERSION"

# 1. 建立系統快照
echo "📸 建立系統快照..."
SNAPSHOT_NAME="pre-update-$VERSION-$(date +%Y%m%d-%H%M%S)"
ndh-admin snapshot create --name "$SNAPSHOT_NAME"

# 2. 驗證新版本映像檔
echo "🔍 驗證新版本映像檔..."
docker pull idtf/ndh-master:$VERSION
docker pull idtf/ndh-worker:$VERSION

# 3. 檢查叢集健康狀態
echo "💚 檢查叢集健康狀態..."
HEALTH_SCORE=$(ndh-admin cluster health --format json | jq -r '.score')
if [ "$HEALTH_SCORE" -lt "80" ]; then
  echo "❌ 叢集健康分數過低 ($HEALTH_SCORE)，取消更新"
  exit 1
fi

# 4. 更新 Worker 節點 (分批次)
echo "🔄 更新 Worker 節點..."
ndh-admin update workers \
  --version $VERSION \
  --batch-size 10%,30%,60% \
  --wait-time 10m,5m,5m \
  --health-check-timeout 5m

# 5. 更新 Master 節點
echo "🔄 更新 Master 節點..."
ndh-admin update masters \
  --version $VERSION \
  --strategy rolling \
  --wait-time 5m

# 6. 執行煙霧測試
echo "🧪 執行煙霧測試..."
ndh-admin test smoke \
  --environment $ENVIRONMENT \
  --timeout 10m

# 7. 監控關鍵指標
echo "📊 監控關鍵指標 (30 分鐘)..."
ndh-admin monitor \
  --duration 30m \
  --metrics error_rate,latency,throughput \
  --alert-threshold error_rate:0.01,latency:500

# 8. 驗證更新成功
echo "✅ 驗證更新..."
CURRENT_VERSION=$(ndh-admin version --format json | jq -r '.version')
if [ "$CURRENT_VERSION" != "$VERSION" ]; then
  echo "❌ 版本驗證失敗，當前版本: $CURRENT_VERSION"
  echo "🔙 開始回滾..."
  ndh-admin rollback --to-snapshot "$SNAPSHOT_NAME"
  exit 1
fi

echo "✅ NDH 已成功更新到版本 $VERSION"

# 9. 發送通知
ndh-admin notify \
  --channel slack \
  --message "NDH 已成功更新到版本 $VERSION"
```

---

## 7. 回滾機制

### 7.1 快速回滾策略

```bash
#!/bin/bash
# NDH 快速回滾腳本

# 方法 1: Kubernetes 原生回滾
kubectl rollout undo deployment/ndh-master -n idtf
kubectl rollout undo deployment/ndh-worker -n idtf

# 方法 2: 使用 ndh-admin 回滾
ndh-admin rollback \
  --to-version v3.5.0 \
  --strategy fast

# 方法 3: 從快照回滾
ndh-admin rollback \
  --to-snapshot "pre-update-v3.6.0-20251014-100000" \
  --verify
```

### 7.2 回滾決策樹

```
更新後出現問題
    │
    ├─ 錯誤率 > 5%? ──────────────────────► 立即回滾
    │
    ├─ 延遲增加 > 50%? ────────────────────► 立即回滾
    │
    ├─ 關鍵功能失效? ──────────────────────► 立即回滾
    │
    ├─ 數據不一致? ────────────────────────► 立即回滾 + 從快照復原
    │
    ├─ 次要功能異常? ──────────────────────► 監控 30 分鐘後決定
    │
    └─ 效能輕微下降? ──────────────────────► 繼續監控，計畫下次修復
```

### 7.3 自動回滾觸發條件

```yaml
# 自動回滾配置
rollback:
  enabled: true
  
  triggers:
    # 錯誤率過高
    - metric: error_rate
      threshold: 0.05        # 5%
      duration: 5m
      action: rollback
    
    # 延遲過高
    - metric: p95_latency
      threshold: 1000        # 1000ms
      duration: 10m
      action: rollback
    
    # 可用性下降
    - metric: availability
      threshold: 0.99        # 99%
      duration: 5m
      action: rollback
    
    # 關鍵 API 失敗
    - metric: critical_api_success_rate
      threshold: 0.95        # 95%
      duration: 3m
      action: rollback
  
  notification:
    channels:
      - slack
      - email
      - pagerduty
    message: "自動回滾已觸發: {{.Reason}}"
```

---

## 8. 數據庫遷移

### 8.1 數據庫遷移策略

對於涉及數據庫 schema 變更的更新，採用 **向後相容的遷移策略**。

```
階段 1: 擴展 Schema (Expand)
  ├─ 新增新欄位/表格
  ├─ 保留舊欄位/表格
  └─ 新舊版本可共存

階段 2: 遷移數據 (Migrate)
  ├─ 將數據從舊欄位複製到新欄位
  ├─ 驗證數據一致性
  └─ 雙寫模式 (同時寫入新舊欄位)

階段 3: 收縮 Schema (Contract)
  ├─ 停止使用舊欄位
  ├─ 刪除舊欄位/表格
  └─ 清理遷移代碼
```

### 8.2 數據庫遷移工具

使用 **Flyway** 或 **Liquibase** 進行數據庫版本控制和遷移。

```sql
-- V3.6.0__add_asset_servant_metadata.sql
-- 擴展階段: 新增 metadata 欄位

-- 新增新欄位 (允許 NULL，向後相容)
ALTER TABLE asset_servants 
ADD COLUMN metadata JSONB DEFAULT '{}';

-- 建立索引
CREATE INDEX idx_asset_servants_metadata 
ON asset_servants USING GIN (metadata);

-- 遷移現有數據
UPDATE asset_servants 
SET metadata = jsonb_build_object(
  'legacy_config', config,
  'migrated_at', NOW()
)
WHERE metadata = '{}';
```

```sql
-- V3.7.0__remove_legacy_config.sql
-- 收縮階段: 移除舊欄位 (在確認所有節點都已更新後執行)

-- 檢查是否所有節點都已更新
DO $$
BEGIN
  IF EXISTS (
    SELECT 1 FROM ndh_nodes 
    WHERE version < '3.7.0'
  ) THEN
    RAISE EXCEPTION '仍有節點未更新到 v3.7.0，無法移除舊欄位';
  END IF;
END $$;

-- 移除舊欄位
ALTER TABLE asset_servants 
DROP COLUMN IF EXISTS config;
```

### 8.3 數據庫遷移腳本

```bash
#!/bin/bash
# 數據庫遷移腳本

VERSION=$1

echo "🗄️ 開始數據庫遷移到版本 $VERSION"

# 1. 備份數據庫
echo "💾 備份數據庫..."
pg_dump -h $DB_HOST -U $DB_USER -d ndh_metadata > backup_$VERSION.sql

# 2. 執行遷移
echo "🔄 執行數據庫遷移..."
flyway -url=jdbc:postgresql://$DB_HOST:5432/ndh_metadata \
       -user=$DB_USER \
       -password=$DB_PASSWORD \
       -locations=filesystem:./migrations \
       -target=$VERSION \
       migrate

# 3. 驗證遷移
echo "✅ 驗證遷移..."
flyway -url=jdbc:postgresql://$DB_HOST:5432/ndh_metadata \
       -user=$DB_USER \
       -password=$DB_PASSWORD \
       validate

# 4. 檢查數據完整性
echo "🔍 檢查數據完整性..."
psql -h $DB_HOST -U $DB_USER -d ndh_metadata -f verify_$VERSION.sql

echo "✅ 數據庫遷移完成"
```

---

## 9. 更新驗證測試

### 9.1 煙霧測試 (Smoke Test)

```python
# smoke_test.py
import requests
import time

def smoke_test(base_url):
    """
    執行煙霧測試，驗證基本功能
    """
    tests = [
        {
            "name": "健康檢查",
            "method": "GET",
            "url": f"{base_url}/health",
            "expected_status": 200
        },
        {
            "name": "版本資訊",
            "method": "GET",
            "url": f"{base_url}/version",
            "expected_status": 200
        },
        {
            "name": "列出資產",
            "method": "GET",
            "url": f"{base_url}/api/v1/assets",
            "expected_status": 200
        },
        {
            "name": "獲取單一資產",
            "method": "GET",
            "url": f"{base_url}/api/v1/assets/test-asset-001",
            "expected_status": 200
        },
        {
            "name": "獲取數據點",
            "method": "GET",
            "url": f"{base_url}/api/v1/assets/test-asset-001/data_points/temperature",
            "expected_status": 200
        }
    ]
    
    passed = 0
    failed = 0
    
    for test in tests:
        try:
            response = requests.request(
                method=test["method"],
                url=test["url"],
                timeout=10
            )
            
            if response.status_code == test["expected_status"]:
                print(f"✅ {test['name']}: PASS")
                passed += 1
            else:
                print(f"❌ {test['name']}: FAIL (狀態碼: {response.status_code})")
                failed += 1
        except Exception as e:
            print(f"❌ {test['name']}: ERROR ({str(e)})")
            failed += 1
    
    print(f"\n總計: {passed} 通過, {failed} 失敗")
    return failed == 0

if __name__ == "__main__":
    success = smoke_test("http://ndh-api.idtf.local")
    exit(0 if success else 1)
```

### 9.2 負載測試 (Load Test)

```python
# load_test.py
import locust
from locust import HttpUser, task, between

class NDHUser(HttpUser):
    wait_time = between(1, 3)
    
    @task(3)
    def get_asset_list(self):
        """獲取資產列表"""
        self.client.get("/api/v1/assets")
    
    @task(5)
    def get_asset_detail(self):
        """獲取資產詳情"""
        self.client.get("/api/v1/assets/test-asset-001")
    
    @task(10)
    def get_data_point(self):
        """獲取數據點"""
        self.client.get("/api/v1/assets/test-asset-001/data_points/temperature")
    
    @task(2)
    def set_data_point(self):
        """設置數據點"""
        self.client.post(
            "/api/v1/assets/test-asset-001/data_points/setpoint",
            json={"value": 25.0}
        )

# 執行負載測試
# locust -f load_test.py --host=http://ndh-api.idtf.local --users 100 --spawn-rate 10 --run-time 10m
```

### 9.3 整合測試 (Integration Test)

```bash
#!/bin/bash
# integration_test.sh

echo "🧪 執行整合測試..."

# 1. 測試 OPC UA 連接
echo "測試 OPC UA 連接..."
ndh-admin test opcua \
  --endpoint "opc.tcp://plc.factory.local:4840" \
  --timeout 30s

# 2. 測試 MQTT 連接
echo "測試 MQTT 連接..."
ndh-admin test mqtt \
  --broker "mqtt://broker.factory.local:1883" \
  --topic "factory/sensors/#" \
  --timeout 30s

# 3. 測試時序數據庫寫入
echo "測試時序數據庫寫入..."
ndh-admin test tsdb-write \
  --points 1000 \
  --duration 1m

# 4. 測試時序數據庫查詢
echo "測試時序數據庫查詢..."
ndh-admin test tsdb-query \
  --query "SELECT * FROM asset_data WHERE time > now() - 1h" \
  --timeout 10s

# 5. 測試 Omniverse 同步
echo "測試 Omniverse 同步..."
ndh-admin test omniverse-sync \
  --asset "test-asset-001" \
  --timeout 30s

echo "✅ 整合測試完成"
```

---

## 10. 監控與告警

### 10.1 更新過程監控指標

```yaml
# Prometheus 監控指標
metrics:
  # 更新狀態
  - ndh_update_in_progress              # 是否正在更新
  - ndh_update_phase                    # 當前更新階段
  - ndh_update_duration_seconds         # 更新持續時間
  
  # 版本資訊
  - ndh_version_info                    # 當前版本資訊
  - ndh_node_version                    # 各節點版本
  
  # 健康狀態
  - ndh_update_health_check_passed      # 健康檢查通過次數
  - ndh_update_health_check_failed      # 健康檢查失敗次數
  
  # 錯誤指標
  - ndh_update_errors_total             # 更新錯誤總數
  - ndh_update_rollback_count           # 回滾次數
```

### 10.2 更新告警規則

```yaml
# Prometheus 告警規則
groups:
- name: ndh_update_alerts
  interval: 30s
  rules:
  
  # 更新時間過長
  - alert: UpdateTakingTooLong
    expr: ndh_update_duration_seconds > 3600
    for: 5m
    labels:
      severity: warning
    annotations:
      summary: "NDH 更新時間過長"
      description: "NDH 更新已進行 {{ $value }} 秒，超過預期時間"
  
  # 健康檢查失敗
  - alert: UpdateHealthCheckFailed
    expr: rate(ndh_update_health_check_failed[5m]) > 0.1
    for: 2m
    labels:
      severity: critical
    annotations:
      summary: "NDH 更新健康檢查失敗"
      description: "健康檢查失敗率: {{ $value }}"
  
  # 版本不一致
  - alert: VersionMismatch
    expr: count(count by (version) (ndh_version_info)) > 2
    for: 30m
    labels:
      severity: warning
    annotations:
      summary: "NDH 節點版本不一致"
      description: "叢集中存在 {{ $value }} 個不同版本"
```

---

## 11. 最佳實踐

### 11.1 更新前檢查清單

- [ ] 建立完整的系統快照
- [ ] 驗證新版本的映像檔可用
- [ ] 檢查叢集健康狀態 (健康分數 > 80)
- [ ] 確認沒有正在進行的維護工作
- [ ] 通知相關團隊和利益相關者
- [ ] 準備回滾計畫
- [ ] 檢查數據庫遷移腳本
- [ ] 驗證測試環境的更新成功
- [ ] 確認監控和告警系統正常運作
- [ ] 準備緊急聯絡人清單

### 11.2 更新後驗證清單

- [ ] 執行煙霧測試
- [ ] 執行負載測試
- [ ] 執行整合測試
- [ ] 檢查所有節點版本一致
- [ ] 驗證關鍵 API 功能正常
- [ ] 檢查錯誤日誌
- [ ] 監控關鍵指標 (至少 30 分鐘)
- [ ] 驗證數據庫遷移成功
- [ ] 檢查 Asset Tag Instances 狀態
- [ ] 確認 Omniverse 同步正常

### 11.3 更新時間建議

| 更新類型 | 建議時間 | 原因 |
|---------|---------|------|
| **補丁更新** | 工作日下午 | 影響小，可快速回滾 |
| **次版本更新** | 週末或非高峰時段 | 需要更多監控時間 |
| **主版本更新** | 計畫性停機窗口 | 風險較高，需要充分準備 |
| **緊急熱修復** | 立即執行 | 安全漏洞修復優先 |

---

## 12. 故障排除

### 12.1 常見問題

#### **問題 1: 更新卡在某個階段**

**症狀**: 更新進度長時間沒有變化

**可能原因**:
- 健康檢查一直失敗
- 節點資源不足
- 網路連接問題

**解決方案**:
```bash
# 檢查更新狀態
ndh-admin update status --verbose

# 檢查節點健康狀態
kubectl get pods -n idtf -o wide

# 查看日誌
kubectl logs -n idtf deployment/ndh-master --tail=100

# 手動推進更新
ndh-admin update force-continue --confirm
```

#### **問題 2: 更新後錯誤率增加**

**症狀**: 更新完成後，API 錯誤率明顯增加

**可能原因**:
- 新版本引入 Bug
- 配置不相容
- 數據庫遷移問題

**解決方案**:
```bash
# 立即回滾
ndh-admin rollback --to-previous-version --fast

# 檢查錯誤日誌
ndh-admin logs --level error --since 30m

# 分析錯誤模式
ndh-admin analyze errors --group-by endpoint
```

#### **問題 3: 版本不一致**

**症狀**: 部分節點版本與預期不符

**可能原因**:
- 更新過程中斷
- 節點故障
- 映像檔拉取失敗

**解決方案**:
```bash
# 檢查所有節點版本
ndh-admin nodes list --show-version

# 強制更新特定節點
ndh-admin update node \
  --node-id worker-3 \
  --version v3.6.0 \
  --force

# 重新同步所有節點
ndh-admin update sync-all --target-version v3.6.0
```

---

## 13. 總結

NDH 的系統更新機制提供了完整的更新策略和工具，確保在不影響生產環境的情況下安全地進行系統升級。透過滾動更新、藍綠部署、金絲雀部署等多種策略，以及完善的回滾機制和自動化測試，NDH 能夠實現：

- ✅ **零停機更新**：更新過程中系統持續提供服務
- ✅ **快速回滾**：任何問題都可在 5 分鐘內回滾
- ✅ **漸進式部署**：分階段部署，及早發現問題
- ✅ **自動化驗證**：更新後自動執行健康檢查和測試
- ✅ **版本相容性**：確保新舊版本可共存

這些機制使 NDH 的系統更新過程**安全、可靠、可控**，滿足工業級應用的嚴格要求。

---

**附錄**:
- [附錄 A: 更新命令參考](./appendix/update_commands.md)
- [附錄 B: 更新檢查清單](./appendix/update_checklist.md)
- [附錄 C: 故障排除指南](./appendix/troubleshooting_guide.md)

