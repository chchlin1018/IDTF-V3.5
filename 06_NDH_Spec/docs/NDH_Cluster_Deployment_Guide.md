# IDTF V3.5 - NDH 叢集部署指南

**版本**: 1.0
**日期**: 2025年10月14日

## 1. 簡介

本指南旨在提供在生產環境中部署 **NDH (Neutral Data Hub) 叢集** 的詳細步驟。NDH 叢集架構是為了滿足大型工業場景（超過 5,000 個資產）對於高可用性、水平擴展和負載均衡的需求而設計。

本指南涵蓋以下內容：
- 架構總覽
- 環境準備
- 部署步驟
- 監控與維護
- 故障排除

## 2. 架構總覽

NDH 叢集採用基於 Kubernetes 的微服務架構，主要包含以下組件：

- **NDH Master 節點**: 負責叢集管理、任務調度、API Gateway 等核心功能，通常以 Active-Standby 模式部署以實現高可用性。
- **NDH Worker 節點**: 負責執行 Asset Servant 實例，直接與物理資產進行數據交換。Worker 節點可根據負載進行水平擴展。
- **服務發現 (etcd/Consul)**: 用於 Worker 節點和 Asset Servant 的動態註冊與發現。
- **負載均衡器 (HAProxy/NGINX)**: 將來自客戶端的請求分發到不同的 Master 或 Worker 節點。
- **數據庫層**: 
    - **時序數據庫 (TSDB)**: 存儲資產的時間序列數據 (例如 InfluxDB, TDengine)。
    - **關聯式數據庫**: 存儲元數據、配置和事件 (例如 PostgreSQL)。
- **監控與日誌**: 使用 Prometheus 進行指標收集，Grafana 進行可視化，以及 ELK/Loki 進行日誌管理。

## 3. 環境準備

### 3.1 硬體需求

| 節點類型 | 建議配置 (每節點) | 數量 |
|---|---|---|
| Kubernetes Master | 8 vCPU, 32 GB RAM, 100 GB SSD | 3 |
| NDH Master | 4 vCPU, 16 GB RAM, 50 GB SSD | 3 |
| NDH Worker | 16 vCPU, 64 GB RAM, 200 GB SSD | 3+ (根據資產數量擴展) |
| 數據庫節點 | 16 vCPU, 64 GB RAM, 1 TB NVMe SSD | 3+ (根據數據庫類型而定) |

### 3.2 軟體需求

- **作業系統**: Ubuntu 22.04 LTS 或 CentOS 8
- **容器運行時**: Docker 或 containerd
- **Kubernetes**: v1.25+
- **Helm**: v3.8+
- **etcd**: v3.5+
- **PostgreSQL**: v14+
- **InfluxDB**: v2.7+ 或 **TDengine**: v3.0+

## 4. 部署步驟

### 4.1 部署 Kubernetes 叢集

(此處省略標準 Kubernetes 叢集的詳細部署過程，請參考官方文件或使用 kubeadm、k3s 等工具。)

### 4.2 部署相依服務 (Helm)

#### **部署 etcd**
```shell
helm repo add bitnami https://charts.bitnami.com/bitnami
helm install etcd bitnami/etcd --set auth.rbac.enabled=false
```

#### **部署 PostgreSQL**
```shell
helm install postgresql bitnami/postgresql --set primary.persistence.size=50Gi
```

#### **部署 InfluxDB**
```shell
helm install influxdb bitnami/influxdb2 --set persistence.size=100Gi
```

### 4.3 部署 NDH 叢集

(假設已有 NDH 的 Helm Chart)

1.  **配置 `values.yaml`**

    ```yaml
    replicaCount: 3

    image:
      repository: idtf/ndh-master
      tag: v3.5.0

    config:
      etcd:
        endpoints: "http://etcd:2379"
      postgresql:
        host: "postgresql-primary"
        database: "ndh_metadata"
      influxdb:
        url: "http://influxdb:8086"
        token: "YOUR_INFLUXDB_TOKEN"
        org: "idtf"
        bucket: "ndh_data"

    worker:
      image:
        repository: idtf/ndh-worker
        tag: v3.5.0
      autoscaling:
        enabled: true
        minReplicas: 3
        maxReplicas: 20
        targetCPUUtilizationPercentage: 75
    ```

2.  **使用 Helm 部署 NDH**

    ```shell
    helm install ndh ./charts/ndh -f values.yaml
    ```

## 5. 監控與維護

### 5.1 部署監控堆疊

```shell
helm repo add prometheus-community https://prometheus-community.github.io/helm-charts
helm install prometheus prometheus-community/kube-prometheus-stack
```

### 5.2 配置 Grafana 儀表板

導入預先定義的 Grafana 儀表板 (JSON 文件)，用於監控 NDH 叢集的關鍵指標，例如：
- 叢集健康狀態
- Asset Servant 數量與分佈
- API 請求延遲與錯誤率
- 數據庫連接池狀態
- 消息隊列長度

## 6. 故障排除

### 6.1 NDH Master 節點無回應

- **檢查日誌**: `kubectl logs -l app=ndh-master`
- **檢查 etcd 連線**: 確保 Master 節點可以正常連接到 etcd 服務。
- **檢查 Leader 選舉**: `etcdctl endpoint health`

### 6.2 Asset Servant 啟動失敗

- **檢查日誌**: `kubectl logs -l app=ndh-worker`
- **檢查 FDL/IADL 配置**: 確保資產定義正確無誤。
- **檢查資源限制**: `kubectl describe pod <worker-pod-name>`，確認 CPU 和內存資源是否充足。


