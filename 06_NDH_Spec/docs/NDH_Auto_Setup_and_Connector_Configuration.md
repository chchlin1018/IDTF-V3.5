# NDH 自動化初始設定與連接器配置系統

**文件版本**: 1.0  
**最後更新**: 2025-10-15  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin (Chih Cheng Lin)

---

## 目錄

1. [概述](#概述)
2. [設計理念](#設計理念)
3. [自動發現機制](#自動發現機制)
4. [連接器類型](#連接器類型)
5. [初始設定流程](#初始設定流程)
6. [Web UI 設定介面](#web-ui-設定介面)
7. [CLI 設定工具](#cli-設定工具)
8. [配置檔案格式](#配置檔案格式)
9. [連接器測試與驗證](#連接器測試與驗證)
10. [故障排除](#故障排除)
11. [最佳實踐](#最佳實踐)

---

## 概述

NDH 自動化初始設定與連接器配置系統提供了一個智能化的部署體驗，能夠自動發現網路上的各種工業系統（時序資料庫、Omniverse、MES、ERP 等），並自動配置連接器。

### 核心目標

1. **降低部署門檻**: 從數天到數小時
2. **減少配置錯誤**: 自動驗證和測試
3. **提升使用者體驗**: 視覺化的設定介面
4. **支援多種部署場景**: 單機、叢集、雲端

### 核心功能

- ✅ **自動發現**: 掃描網路，發現可用的系統
- ✅ **智能配置**: 根據系統類型自動生成配置
- ✅ **連接測試**: 自動測試連接並驗證功能
- ✅ **配置管理**: 匯入/匯出配置，版本控制
- ✅ **故障診斷**: 自動診斷連接問題並提供解決方案

---

## 設計理念

### 1. 零配置優先 (Zero-Configuration First)

系統應該能夠在沒有任何手動配置的情況下自動發現和連接到常見的系統。

**範例**:
```bash
# 啟動 NDH，自動發現並連接所有系統
$ ndh-admin init --auto-discover

🔍 正在掃描網路...
✅ 發現 InfluxDB: 192.168.1.100:8086
✅ 發現 Omniverse Nucleus: 192.168.1.101:3009
✅ 發現 MES (SAP ME): 192.168.1.102:8080
✅ 發現 ERP (SAP S/4HANA): 192.168.1.103:443

🔧 正在自動配置連接器...
✅ InfluxDB 連接器已配置
✅ Omniverse 連接器已配置
✅ MES 連接器已配置
✅ ERP 連接器已配置

✅ NDH 初始化完成！
```

### 2. 漸進式配置 (Progressive Configuration)

支援從簡單到複雜的漸進式配置：

1. **Level 1**: 自動發現 + 預設配置
2. **Level 2**: 手動調整部分參數
3. **Level 3**: 完全自訂配置

### 3. 配置即代碼 (Configuration as Code)

所有配置都可以匯出為 YAML/JSON 檔案，支援版本控制和自動化部署。

### 4. 可觀測性 (Observability)

配置過程的每一步都有詳細的日誌和狀態反饋。

---

## 自動發現機制

### 1. 網路掃描策略

#### 1.1 主動掃描 (Active Scanning)

**方法**: 掃描指定的 IP 範圍和端口

```yaml
scan_config:
  ip_ranges:
    - "192.168.1.0/24"
    - "10.0.0.0/16"
  
  port_ranges:
    # 時序資料庫
    - name: "InfluxDB"
      ports: [8086]
    - name: "TimescaleDB"
      ports: [5432]
    
    # Omniverse
    - name: "Omniverse Nucleus"
      ports: [3009, 3019, 3100]
    
    # MES
    - name: "SAP ME"
      ports: [8080, 50000]
    - name: "Siemens Opcenter"
      ports: [80, 443]
    
    # ERP
    - name: "SAP S/4HANA"
      ports: [443, 3200, 3300]
    - name: "Oracle ERP Cloud"
      ports: [443]
  
  timeout_seconds: 5
  max_concurrent_scans: 100
```

**實作**:

```python
import socket
import concurrent.futures
from typing import List, Tuple

class NetworkScanner:
    def __init__(self, config: dict):
        self.config = config
    
    def scan_port(self, ip: str, port: int, timeout: int = 5) -> bool:
        """掃描單個端口"""
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
                sock.settimeout(timeout)
                result = sock.connect_ex((ip, port))
                return result == 0
        except:
            return False
    
    def scan_ip_range(self, ip_range: str, ports: List[int]) -> List[Tuple[str, int]]:
        """掃描 IP 範圍"""
        import ipaddress
        
        network = ipaddress.ip_network(ip_range)
        discovered = []
        
        with concurrent.futures.ThreadPoolExecutor(max_workers=100) as executor:
            futures = []
            for ip in network.hosts():
                for port in ports:
                    future = executor.submit(self.scan_port, str(ip), port)
                    futures.append((future, str(ip), port))
            
            for future, ip, port in futures:
                if future.result():
                    discovered.append((ip, port))
        
        return discovered
    
    def discover_systems(self) -> dict:
        """發現所有系統"""
        discovered_systems = {}
        
        for port_config in self.config['port_ranges']:
            system_name = port_config['name']
            ports = port_config['ports']
            
            print(f"🔍 正在掃描 {system_name}...")
            
            for ip_range in self.config['ip_ranges']:
                found = self.scan_ip_range(ip_range, ports)
                
                if found:
                    if system_name not in discovered_systems:
                        discovered_systems[system_name] = []
                    discovered_systems[system_name].extend(found)
        
        return discovered_systems
```

#### 1.2 被動發現 (Passive Discovery)

**方法**: 監聽網路流量，識別系統

- mDNS/Bonjour 服務發現
- SSDP (Simple Service Discovery Protocol)
- 監聽特定的廣播/多播訊息

**實作**:

```python
from zeroconf import ServiceBrowser, Zeroconf

class PassiveDiscovery:
    def __init__(self):
        self.zeroconf = Zeroconf()
        self.discovered_services = []
    
    def on_service_state_change(self, zeroconf, service_type, name, state_change):
        """服務狀態變更回調"""
        if state_change is ServiceStateChange.Added:
            info = zeroconf.get_service_info(service_type, name)
            if info:
                self.discovered_services.append({
                    'name': name,
                    'type': service_type,
                    'address': socket.inet_ntoa(info.addresses[0]),
                    'port': info.port,
                    'properties': info.properties
                })
    
    def discover(self, service_types: List[str], timeout: int = 10):
        """發現服務"""
        browsers = []
        for service_type in service_types:
            browser = ServiceBrowser(self.zeroconf, service_type, 
                                   handlers=[self.on_service_state_change])
            browsers.append(browser)
        
        time.sleep(timeout)
        
        self.zeroconf.close()
        return self.discovered_services
```

#### 1.3 配置檔案發現 (Configuration File Discovery)

**方法**: 讀取預先配置的系統清單

```yaml
# /etc/ndh/systems.yaml
systems:
  - name: "Production InfluxDB"
    type: "influxdb"
    host: "influxdb.production.local"
    port: 8086
  
  - name: "Omniverse Nucleus"
    type: "omniverse"
    host: "omniverse.production.local"
    port: 3009
  
  - name: "SAP ME"
    type: "mes"
    vendor: "sap"
    host: "mes.production.local"
    port: 8080
```

---

### 2. 系統識別與指紋識別

發現端口後，需要識別系統類型和版本。

#### 2.1 HTTP Banner 識別

```python
import requests

def identify_http_service(host: str, port: int) -> dict:
    """透過 HTTP 識別服務"""
    try:
        response = requests.get(f"http://{host}:{port}", timeout=5)
        
        # 檢查 Server header
        server = response.headers.get('Server', '')
        
        # InfluxDB
        if 'InfluxDB' in server:
            return {
                'type': 'influxdb',
                'version': extract_version(server)
            }
        
        # 檢查特定的 API 端點
        # Omniverse Nucleus
        if port == 3009:
            try:
                nucleus_response = requests.get(
                    f"http://{host}:{port}/api/version", 
                    timeout=5
                )
                if nucleus_response.status_code == 200:
                    return {
                        'type': 'omniverse_nucleus',
                        'version': nucleus_response.json().get('version')
                    }
            except:
                pass
        
        return {'type': 'unknown'}
    
    except:
        return {'type': 'unknown'}
```

#### 2.2 資料庫指紋識別

```python
def identify_database(host: str, port: int) -> dict:
    """識別資料庫類型"""
    
    # PostgreSQL/TimescaleDB (port 5432)
    if port == 5432:
        try:
            import psycopg2
            conn = psycopg2.connect(
                host=host,
                port=port,
                user='postgres',
                password='',
                connect_timeout=5
            )
            cursor = conn.cursor()
            cursor.execute("SELECT version()")
            version = cursor.fetchone()[0]
            
            # 檢查是否為 TimescaleDB
            cursor.execute(
                "SELECT * FROM pg_extension WHERE extname = 'timescaledb'"
            )
            is_timescaledb = cursor.fetchone() is not None
            
            conn.close()
            
            return {
                'type': 'timescaledb' if is_timescaledb else 'postgresql',
                'version': version
            }
        except:
            return {'type': 'postgresql', 'version': 'unknown'}
    
    return {'type': 'unknown'}
```

#### 2.3 MES/ERP 系統識別

```python
def identify_mes_erp(host: str, port: int) -> dict:
    """識別 MES/ERP 系統"""
    
    # SAP 系統通常有特定的 HTTP 端點
    try:
        # SAP ME
        response = requests.get(
            f"http://{host}:{port}/sap/public/info",
            timeout=5
        )
        if response.status_code == 200:
            return {
                'type': 'mes',
                'vendor': 'sap',
                'product': 'SAP ME'
            }
    except:
        pass
    
    # Siemens Opcenter
    try:
        response = requests.get(
            f"http://{host}:{port}/Opcenter/api/info",
            timeout=5
        )
        if response.status_code == 200:
            return {
                'type': 'mes',
                'vendor': 'siemens',
                'product': 'Opcenter'
            }
    except:
        pass
    
    return {'type': 'unknown'}
```

---

## 連接器類型

### 1. 時序資料庫連接器

#### 1.1 InfluxDB 連接器

**自動配置**:

```python
class InfluxDBConnectorAutoConfig:
    def discover_and_configure(self, host: str, port: int) -> dict:
        """自動發現並配置 InfluxDB"""
        
        # 1. 檢測版本
        version = self.detect_version(host, port)
        
        # 2. 測試連接
        if not self.test_connection(host, port):
            raise Exception("無法連接到 InfluxDB")
        
        # 3. 列出可用的資料庫/bucket
        databases = self.list_databases(host, port)
        
        # 4. 生成配置
        config = {
            'type': 'influxdb',
            'version': version,
            'host': host,
            'port': port,
            'protocol': 'http',
            'databases': databases,
            'default_database': databases[0] if databases else 'ndh',
            'retention_policy': 'autogen',
            'timeout': 10,
            'max_retries': 3
        }
        
        return config
    
    def detect_version(self, host: str, port: int) -> str:
        """檢測 InfluxDB 版本"""
        import requests
        
        response = requests.get(f"http://{host}:{port}/ping")
        version_header = response.headers.get('X-Influxdb-Version', 'unknown')
        
        if version_header.startswith('1.'):
            return '1.x'
        elif version_header.startswith('2.'):
            return '2.x'
        else:
            return 'unknown'
    
    def list_databases(self, host: str, port: int) -> List[str]:
        """列出資料庫"""
        from influxdb import InfluxDBClient
        
        client = InfluxDBClient(host=host, port=port)
        databases = client.get_list_database()
        
        return [db['name'] for db in databases]
```

**配置範例**:

```yaml
connectors:
  - name: "influxdb_production"
    type: "influxdb"
    version: "2.x"
    connection:
      host: "192.168.1.100"
      port: 8086
      protocol: "http"
      org: "idtf"
      bucket: "factory_data"
      token: "${INFLUXDB_TOKEN}"  # 從環境變數讀取
    
    settings:
      timeout: 10
      max_retries: 3
      batch_size: 5000
      flush_interval: 1000  # ms
    
    health_check:
      enabled: true
      interval: 30  # seconds
      endpoint: "/health"
```

#### 1.2 TimescaleDB 連接器

**自動配置**:

```python
class TimescaleDBConnectorAutoConfig:
    def discover_and_configure(self, host: str, port: int) -> dict:
        """自動發現並配置 TimescaleDB"""
        
        import psycopg2
        
        # 1. 測試連接
        conn = psycopg2.connect(
            host=host,
            port=port,
            user='postgres',
            password='',
            dbname='postgres'
        )
        
        cursor = conn.cursor()
        
        # 2. 檢測 TimescaleDB 版本
        cursor.execute(
            "SELECT extversion FROM pg_extension WHERE extname = 'timescaledb'"
        )
        timescaledb_version = cursor.fetchone()[0]
        
        # 3. 列出資料庫
        cursor.execute("SELECT datname FROM pg_database WHERE datistemplate = false")
        databases = [row[0] for row in cursor.fetchall()]
        
        # 4. 列出 hypertables
        cursor.execute("""
            SELECT hypertable_schema, hypertable_name
            FROM timescaledb_information.hypertables
        """)
        hypertables = cursor.fetchall()
        
        conn.close()
        
        # 5. 生成配置
        config = {
            'type': 'timescaledb',
            'version': timescaledb_version,
            'host': host,
            'port': port,
            'databases': databases,
            'default_database': 'ndh',
            'hypertables': [
                {'schema': schema, 'table': table}
                for schema, table in hypertables
            ],
            'connection_pool': {
                'min_size': 5,
                'max_size': 20
            }
        }
        
        return config
```

---

### 2. Omniverse 連接器

#### 2.1 自動發現 Omniverse Nucleus

**實作**:

```python
class OmniverseConnectorAutoConfig:
    def discover_and_configure(self, host: str, port: int = 3009) -> dict:
        """自動發現並配置 Omniverse Nucleus"""
        
        import requests
        
        # 1. 檢測 Nucleus 版本
        response = requests.get(f"http://{host}:{port}/api/version")
        version_info = response.json()
        
        # 2. 列出可用的 USD 倉庫
        response = requests.get(f"http://{host}:{port}/api/repos")
        repos = response.json()
        
        # 3. 測試 USD 訪問
        test_repo = repos[0] if repos else None
        if test_repo:
            usd_url = f"omniverse://{host}/{test_repo['name']}"
            if not self.test_usd_access(usd_url):
                raise Exception("無法訪問 USD 倉庫")
        
        # 4. 生成配置
        config = {
            'type': 'omniverse',
            'nucleus_version': version_info.get('version'),
            'host': host,
            'port': port,
            'repos': repos,
            'default_repo': test_repo['name'] if test_repo else 'Projects',
            'usd_stage_path': f"omniverse://{host}/{test_repo['name']}/Factory/factory.usd",
            'connector_settings': {
                'sync_interval': 1000,  # ms
                'batch_updates': True,
                'enable_live_sync': True
            }
        }
        
        return config
    
    def test_usd_access(self, usd_url: str) -> bool:
        """測試 USD 訪問"""
        try:
            from pxr import Usd
            stage = Usd.Stage.Open(usd_url)
            return stage is not None
        except:
            return False
```

**配置範例**:

```yaml
connectors:
  - name: "omniverse_production"
    type: "omniverse"
    nucleus_version: "2023.2.0"
    connection:
      host: "192.168.1.101"
      port: 3009
      repo: "Projects"
      usd_stage_path: "omniverse://192.168.1.101/Projects/Factory/factory.usd"
      auth:
        username: "${OMNIVERSE_USER}"
        password: "${OMNIVERSE_PASSWORD}"
    
    settings:
      sync_interval: 1000  # ms
      batch_updates: true
      enable_live_sync: true
      max_prims_per_batch: 1000
    
    health_check:
      enabled: true
      interval: 60  # seconds
```

---

### 3. MES 連接器

#### 3.1 SAP ME 連接器

**自動發現**:

```python
class SAPMEConnectorAutoConfig:
    def discover_and_configure(self, host: str, port: int = 8080) -> dict:
        """自動發現並配置 SAP ME"""
        
        import requests
        
        # 1. 檢測 SAP ME 版本
        response = requests.get(
            f"http://{host}:{port}/sap/public/info",
            timeout=10
        )
        info = response.json()
        
        # 2. 測試 OData API
        odata_url = f"http://{host}:{port}/odata/v4"
        response = requests.get(f"{odata_url}/$metadata", timeout=10)
        
        if response.status_code != 200:
            raise Exception("無法訪問 SAP ME OData API")
        
        # 3. 列出可用的實體集
        metadata = response.text
        entity_sets = self.parse_entity_sets(metadata)
        
        # 4. 生成配置
        config = {
            'type': 'mes',
            'vendor': 'sap',
            'product': 'SAP ME',
            'version': info.get('version'),
            'host': host,
            'port': port,
            'api_type': 'odata_v4',
            'base_url': odata_url,
            'entity_sets': entity_sets,
            'polling_interval': 5000,  # ms
            'supported_operations': [
                'get_work_orders',
                'get_operations',
                'report_production',
                'get_material_status'
            ]
        }
        
        return config
    
    def parse_entity_sets(self, metadata: str) -> List[str]:
        """解析 OData metadata 中的實體集"""
        import xml.etree.ElementTree as ET
        
        root = ET.fromstring(metadata)
        entity_sets = []
        
        for entity_set in root.findall('.//{*}EntitySet'):
            entity_sets.append(entity_set.get('Name'))
        
        return entity_sets
```

**配置範例**:

```yaml
connectors:
  - name: "sap_me_production"
    type: "mes"
    vendor: "sap"
    product: "SAP ME"
    version: "15.4"
    connection:
      host: "192.168.1.102"
      port: 8080
      protocol: "http"
      base_url: "http://192.168.1.102:8080/odata/v4"
      auth:
        type: "basic"
        username: "${SAP_ME_USER}"
        password: "${SAP_ME_PASSWORD}"
    
    settings:
      polling_interval: 5000  # ms
      batch_size: 100
      timeout: 30
    
    mappings:
      work_order: "WorkOrders"
      operation: "Operations"
      material: "Materials"
      production_data: "ProductionData"
    
    health_check:
      enabled: true
      interval: 30  # seconds
      endpoint: "/sap/public/ping"
```

#### 3.2 Siemens Opcenter 連接器

**自動發現**:

```python
class OpcenterConnectorAutoConfig:
    def discover_and_configure(self, host: str, port: int = 443) -> dict:
        """自動發現並配置 Siemens Opcenter"""
        
        import requests
        
        # 1. 檢測 Opcenter 版本
        response = requests.get(
            f"https://{host}:{port}/Opcenter/api/info",
            verify=False,
            timeout=10
        )
        info = response.json()
        
        # 2. 測試 REST API
        api_url = f"https://{host}:{port}/Opcenter/api/v1"
        response = requests.get(
            f"{api_url}/workorders",
            verify=False,
            timeout=10
        )
        
        if response.status_code not in [200, 401]:  # 401 表示需要認證，但 API 可用
            raise Exception("無法訪問 Opcenter REST API")
        
        # 3. 生成配置
        config = {
            'type': 'mes',
            'vendor': 'siemens',
            'product': 'Opcenter',
            'version': info.get('version'),
            'host': host,
            'port': port,
            'api_type': 'rest',
            'base_url': api_url,
            'polling_interval': 5000,  # ms
            'supported_operations': [
                'get_work_orders',
                'get_operations',
                'report_production',
                'get_material_status',
                'get_equipment_status'
            ]
        }
        
        return config
```

---

### 4. ERP 連接器

#### 4.1 SAP S/4HANA 連接器

**自動發現**:

```python
class SAPS4HANAConnectorAutoConfig:
    def discover_and_configure(self, host: str, port: int = 443) -> dict:
        """自動發現並配置 SAP S/4HANA"""
        
        import requests
        
        # 1. 檢測 SAP 系統
        response = requests.get(
            f"https://{host}:{port}/sap/public/info",
            verify=False,
            timeout=10
        )
        info = response.json()
        
        # 2. 測試 OData API
        odata_url = f"https://{host}:{port}/sap/opu/odata/sap"
        response = requests.get(
            f"{odata_url}/API_PRODUCT_SRV/$metadata",
            verify=False,
            timeout=10
        )
        
        if response.status_code not in [200, 401]:
            raise Exception("無法訪問 SAP S/4HANA OData API")
        
        # 3. 生成配置
        config = {
            'type': 'erp',
            'vendor': 'sap',
            'product': 'S/4HANA',
            'version': info.get('version'),
            'host': host,
            'port': port,
            'api_type': 'odata_v2',
            'base_url': odata_url,
            'polling_interval': 60000,  # ms (ERP 通常不需要頻繁輪詢)
            'supported_operations': [
                'get_materials',
                'get_purchase_orders',
                'get_sales_orders',
                'get_production_orders',
                'get_inventory'
            ]
        }
        
        return config
```

**配置範例**:

```yaml
connectors:
  - name: "sap_s4hana_production"
    type: "erp"
    vendor: "sap"
    product: "S/4HANA"
    version: "2022"
    connection:
      host: "192.168.1.103"
      port: 443
      protocol: "https"
      base_url: "https://192.168.1.103/sap/opu/odata/sap"
      auth:
        type: "oauth2"
        client_id: "${SAP_CLIENT_ID}"
        client_secret: "${SAP_CLIENT_SECRET}"
        token_url: "https://192.168.1.103/sap/bc/sec/oauth2/token"
    
    settings:
      polling_interval: 60000  # ms
      batch_size: 50
      timeout: 60
    
    mappings:
      material: "API_PRODUCT_SRV/A_Product"
      purchase_order: "API_PURCHASEORDER_PROCESS_SRV/A_PurchaseOrder"
      sales_order: "API_SALES_ORDER_SRV/A_SalesOrder"
      production_order: "API_PRODUCTION_ORDER_2_SRV/A_ProductionOrder_2"
    
    health_check:
      enabled: true
      interval: 300  # seconds
      endpoint: "/sap/public/ping"
```

---

## 初始設定流程

### 1. 互動式 CLI 設定

```bash
$ ndh-admin init

╔══════════════════════════════════════════════════════════════╗
║         歡迎使用 NDH 初始化設定精靈                          ║
╚══════════════════════════════════════════════════════════════╝

步驟 1/5: 選擇部署模式

請選擇部署模式:
  1) 單機模式 (Single Node)
  2) 叢集模式 (Cluster)
  3) 雲端模式 (Cloud)

您的選擇 [1]: 2

步驟 2/5: 網路掃描

是否要自動掃描網路以發現系統? [Y/n]: Y

請輸入要掃描的 IP 範圍 (多個範圍用逗號分隔):
[192.168.1.0/24]: 192.168.1.0/24,10.0.0.0/16

🔍 正在掃描網路...
  掃描 192.168.1.0/24... [████████████████████] 100% (254/254)
  掃描 10.0.0.0/16...    [████████████████████] 100% (65536/65536)

✅ 掃描完成！發現 4 個系統:

  1. InfluxDB 2.x
     地址: 192.168.1.100:8086
     狀態: ✅ 可連接

  2. Omniverse Nucleus 2023.2.0
     地址: 192.168.1.101:3009
     狀態: ✅ 可連接

  3. SAP ME 15.4
     地址: 192.168.1.102:8080
     狀態: ⚠️  需要認證

  4. SAP S/4HANA 2022
     地址: 192.168.1.103:443
     狀態: ⚠️  需要認證

步驟 3/5: 配置連接器

正在自動配置連接器...

[1/4] InfluxDB 連接器
  ✅ 自動配置完成
  ✅ 連接測試通過
  ✅ 發現 3 個資料庫: factory_data, quality_data, energy_data

[2/4] Omniverse 連接器
  ✅ 自動配置完成
  ✅ 連接測試通過
  ✅ 發現 2 個倉庫: Projects, Assets

[3/4] SAP ME 連接器
  ⚠️  需要認證資訊
  
  請輸入 SAP ME 使用者名稱: admin
  請輸入 SAP ME 密碼: ********
  
  ✅ 認證成功
  ✅ 連接測試通過
  ✅ 發現 15 個實體集

[4/4] SAP S/4HANA 連接器
  ⚠️  需要 OAuth2 認證資訊
  
  請輸入 Client ID: ndh-client
  請輸入 Client Secret: ********
  
  ✅ 認證成功
  ✅ 連接測試通過
  ✅ API 可用

步驟 4/5: 配置儲存

請選擇配置儲存位置:
  1) 本地檔案 (/etc/ndh/config.yaml)
  2) etcd
  3) Kubernetes ConfigMap

您的選擇 [1]: 1

✅ 配置已儲存到 /etc/ndh/config.yaml

步驟 5/5: 啟動 NDH

是否立即啟動 NDH? [Y/n]: Y

🚀 正在啟動 NDH...
  ✅ Master 節點啟動成功
  ✅ Worker 節點啟動成功
  ✅ 所有連接器已啟動
  ✅ 健康檢查通過

╔══════════════════════════════════════════════════════════════╗
║                    NDH 初始化完成！                          ║
╚══════════════════════════════════════════════════════════════╝

NDH 已成功啟動並連接到以下系統:
  • InfluxDB (192.168.1.100:8086)
  • Omniverse (192.168.1.101:3009)
  • SAP ME (192.168.1.102:8080)
  • SAP S/4HANA (192.168.1.103:443)

Web UI: http://localhost:8080
API: http://localhost:8080/api/v1

下一步:
  1. 訪問 Web UI 查看系統狀態
  2. 匯入 IADL/FDL 定義
  3. 開始監控和管理您的工廠

需要幫助? 執行: ndh-admin help
```

---

### 2. 非互動式 CLI 設定

```bash
# 使用配置檔案
$ ndh-admin init --config /path/to/config.yaml

# 使用命令列參數
$ ndh-admin init \
  --mode cluster \
  --scan-range "192.168.1.0/24" \
  --influxdb "192.168.1.100:8086" \
  --omniverse "192.168.1.101:3009" \
  --mes "192.168.1.102:8080" \
  --mes-type "sap-me" \
  --mes-user "admin" \
  --mes-password "password" \
  --erp "192.168.1.103:443" \
  --erp-type "sap-s4hana" \
  --erp-client-id "ndh-client" \
  --erp-client-secret "secret" \
  --auto-start
```

---

## Web UI 設定介面

### 1. 設定精靈 (Setup Wizard)

**頁面 1: 歡迎**

```
┌─────────────────────────────────────────────────────────┐
│  歡迎使用 NDH                                           │
│                                                         │
│  這個設定精靈將協助您:                                  │
│  • 自動發現網路上的系統                                 │
│  • 配置連接器                                           │
│  • 測試連接                                             │
│  • 啟動 NDH                                             │
│                                                         │
│  預計時間: 5-10 分鐘                                    │
│                                                         │
│                                   [開始設定] [稍後設定] │
└─────────────────────────────────────────────────────────┘
```

**頁面 2: 網路掃描**

```
┌─────────────────────────────────────────────────────────┐
│  步驟 1/4: 網路掃描                                     │
│                                                         │
│  IP 範圍:  [192.168.1.0/24        ] [+ 新增範圍]       │
│                                                         │
│  掃描端口:                                              │
│  ☑ 時序資料庫 (InfluxDB, TimescaleDB)                  │
│  ☑ Omniverse Nucleus                                   │
│  ☑ MES 系統 (SAP ME, Opcenter)                         │
│  ☑ ERP 系統 (SAP S/4HANA, Oracle)                      │
│                                                         │
│  [開始掃描]                                             │
│                                                         │
│  掃描進度: [████████████████████] 100% (254/254)       │
│                                                         │
│  發現的系統:                                            │
│  ┌───────────────────────────────────────────────────┐ │
│  │ ✅ InfluxDB 2.x          192.168.1.100:8086      │ │
│  │ ✅ Omniverse Nucleus     192.168.1.101:3009      │ │
│  │ ⚠️  SAP ME 15.4          192.168.1.102:8080      │ │
│  │ ⚠️  SAP S/4HANA 2022     192.168.1.103:443       │ │
│  └───────────────────────────────────────────────────┘ │
│                                                         │
│                                       [上一步] [下一步] │
└─────────────────────────────────────────────────────────┘
```

**頁面 3: 配置連接器**

```
┌─────────────────────────────────────────────────────────┐
│  步驟 2/4: 配置連接器                                   │
│                                                         │
│  ┌─ InfluxDB ─────────────────────────────────────────┐ │
│  │ 地址: 192.168.1.100:8086                          │ │
│  │ 版本: 2.x                                         │ │
│  │ 組織: [idtf            ]                          │ │
│  │ Bucket: [factory_data    ]                        │ │
│  │ Token: [••••••••••••••••]                         │ │
│  │                                                   │ │
│  │ [測試連接] ✅ 連接成功                            │ │
│  └───────────────────────────────────────────────────┘ │
│                                                         │
│  ┌─ Omniverse ────────────────────────────────────────┐ │
│  │ 地址: 192.168.1.101:3009                          │ │
│  │ 版本: 2023.2.0                                    │ │
│  │ 倉庫: [Projects        ]                          │ │
│  │ USD 路徑: [omniverse://192.168.1.101/Projects/... │ │
│  │ 使用者: [admin          ]                         │ │
│  │ 密碼: [••••••••••••••••]                         │ │
│  │                                                   │ │
│  │ [測試連接] ✅ 連接成功                            │ │
│  └───────────────────────────────────────────────────┘ │
│                                                         │
│  ┌─ SAP ME ───────────────────────────────────────────┐ │
│  │ 地址: 192.168.1.102:8080                          │ │
│  │ 版本: 15.4                                        │ │
│  │ 使用者: [admin          ]                         │ │
│  │ 密碼: [••••••••••••••••]                         │ │
│  │                                                   │ │
│  │ [測試連接] ✅ 連接成功                            │ │
│  └───────────────────────────────────────────────────┘ │
│                                                         │
│                                       [上一步] [下一步] │
└─────────────────────────────────────────────────────────┘
```

**頁面 4: 測試與驗證**

```
┌─────────────────────────────────────────────────────────┐
│  步驟 3/4: 測試與驗證                                   │
│                                                         │
│  正在測試所有連接器...                                  │
│                                                         │
│  ✅ InfluxDB 連接器                                     │
│     • 連接測試: ✅ 通過                                 │
│     • 寫入測試: ✅ 通過                                 │
│     • 查詢測試: ✅ 通過                                 │
│                                                         │
│  ✅ Omniverse 連接器                                    │
│     • 連接測試: ✅ 通過                                 │
│     • USD 訪問: ✅ 通過                                 │
│     • 寫入測試: ✅ 通過                                 │
│                                                         │
│  ✅ SAP ME 連接器                                       │
│     • 連接測試: ✅ 通過                                 │
│     • API 訪問: ✅ 通過                                 │
│     • 數據查詢: ✅ 通過                                 │
│                                                         │
│  ✅ SAP S/4HANA 連接器                                  │
│     • 連接測試: ✅ 通過                                 │
│     • OAuth2 認證: ✅ 通過                              │
│     • API 訪問: ✅ 通過                                 │
│                                                         │
│  所有測試通過！                                         │
│                                                         │
│                                       [上一步] [下一步] │
└─────────────────────────────────────────────────────────┘
```

**頁面 5: 完成**

```
┌─────────────────────────────────────────────────────────┐
│  步驟 4/4: 完成                                         │
│                                                         │
│  ✅ NDH 初始化完成！                                    │
│                                                         │
│  已配置的連接器:                                        │
│  • InfluxDB (192.168.1.100:8086)                       │
│  • Omniverse (192.168.1.101:3009)                      │
│  • SAP ME (192.168.1.102:8080)                         │
│  • SAP S/4HANA (192.168.1.103:443)                     │
│                                                         │
│  配置已儲存到: /etc/ndh/config.yaml                     │
│                                                         │
│  下一步:                                                │
│  1. 匯入 IADL/FDL 定義                                  │
│  2. 開始監控和管理您的工廠                              │
│                                                         │
│                          [啟動 NDH] [匯出配置] [完成]  │
└─────────────────────────────────────────────────────────┘
```

---

### 2. 連接器管理介面

**連接器列表頁面**:

```
┌─────────────────────────────────────────────────────────────────────────┐
│  連接器管理                                      [+ 新增連接器] [重新掃描] │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─ InfluxDB ────────────────────────────────────────────────────────┐ │
│  │ 名稱: influxdb_production                     狀態: ✅ 運行中     │ │
│  │ 地址: 192.168.1.100:8086                                         │ │
│  │ 版本: 2.x                                                        │ │
│  │ 健康狀態: ✅ 健康                                                │ │
│  │ 最後檢查: 2025-10-15 14:30:00                                   │ │
│  │                                                                  │ │
│  │ [編輯] [測試] [停止] [刪除]                                      │ │
│  └──────────────────────────────────────────────────────────────────┘ │
│                                                                         │
│  ┌─ Omniverse ───────────────────────────────────────────────────────┐ │
│  │ 名稱: omniverse_production                    狀態: ✅ 運行中     │ │
│  │ 地址: 192.168.1.101:3009                                         │ │
│  │ 版本: 2023.2.0                                                   │ │
│  │ 健康狀態: ✅ 健康                                                │ │
│  │ 最後檢查: 2025-10-15 14:30:00                                   │ │
│  │                                                                  │ │
│  │ [編輯] [測試] [停止] [刪除]                                      │ │
│  └──────────────────────────────────────────────────────────────────┘ │
│                                                                         │
│  ┌─ SAP ME ──────────────────────────────────────────────────────────┐ │
│  │ 名稱: sap_me_production                       狀態: ⚠️  警告      │ │
│  │ 地址: 192.168.1.102:8080                                         │ │
│  │ 版本: 15.4                                                       │ │
│  │ 健康狀態: ⚠️  響應慢 (1200ms)                                    │ │
│  │ 最後檢查: 2025-10-15 14:30:00                                   │ │
│  │                                                                  │ │
│  │ [編輯] [測試] [重啟] [刪除]                                      │ │
│  └──────────────────────────────────────────────────────────────────┘ │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## CLI 設定工具

### 1. ndh-admin 命令

```bash
# 初始化 NDH
ndh-admin init [OPTIONS]

# 選項:
  --mode <mode>                部署模式 (single|cluster|cloud)
  --config <file>              配置檔案路徑
  --scan-range <range>         IP 掃描範圍
  --auto-discover              自動發現系統
  --auto-configure             自動配置連接器
  --auto-start                 自動啟動 NDH
  --interactive                互動式設定
  --non-interactive            非互動式設定
  
  # 連接器配置
  --influxdb <host:port>       InfluxDB 地址
  --influxdb-token <token>     InfluxDB Token
  --omniverse <host:port>      Omniverse Nucleus 地址
  --mes <host:port>            MES 系統地址
  --mes-type <type>            MES 類型 (sap-me|opcenter)
  --erp <host:port>            ERP 系統地址
  --erp-type <type>            ERP 類型 (sap-s4hana|oracle)

# 範例:
$ ndh-admin init --auto-discover --auto-configure --auto-start

$ ndh-admin init \
  --config /etc/ndh/config.yaml \
  --non-interactive

$ ndh-admin init \
  --mode cluster \
  --influxdb "192.168.1.100:8086" \
  --influxdb-token "my-token" \
  --omniverse "192.168.1.101:3009" \
  --auto-start
```

### 2. 連接器管理命令

```bash
# 列出所有連接器
$ ndh-admin connector list

NAME                    TYPE        STATUS    HEALTH
influxdb_production     influxdb    running   ✅ healthy
omniverse_production    omniverse   running   ✅ healthy
sap_me_production       mes         running   ⚠️  slow
sap_s4hana_production   erp         running   ✅ healthy

# 測試連接器
$ ndh-admin connector test influxdb_production

Testing influxdb_production...
  ✅ Connection test: PASSED
  ✅ Write test: PASSED
  ✅ Query test: PASSED

All tests passed!

# 啟動/停止連接器
$ ndh-admin connector start influxdb_production
$ ndh-admin connector stop influxdb_production
$ ndh-admin connector restart influxdb_production

# 查看連接器詳情
$ ndh-admin connector show influxdb_production

Name: influxdb_production
Type: influxdb
Status: running
Health: ✅ healthy

Connection:
  Host: 192.168.1.100
  Port: 8086
  Protocol: http
  Org: idtf
  Bucket: factory_data

Settings:
  Timeout: 10s
  Max Retries: 3
  Batch Size: 5000

Health Check:
  Enabled: true
  Interval: 30s
  Last Check: 2025-10-15 14:30:00
  Status: ✅ healthy

# 新增連接器
$ ndh-admin connector add \
  --name my_influxdb \
  --type influxdb \
  --host 192.168.1.100 \
  --port 8086 \
  --org idtf \
  --bucket factory_data \
  --token my-token

# 刪除連接器
$ ndh-admin connector delete influxdb_production

# 匯出連接器配置
$ ndh-admin connector export influxdb_production > influxdb.yaml

# 匯入連接器配置
$ ndh-admin connector import influxdb.yaml
```

### 3. 網路掃描命令

```bash
# 掃描網路
$ ndh-admin scan --range "192.168.1.0/24"

Scanning 192.168.1.0/24...
[████████████████████] 100% (254/254)

Discovered systems:
  ✅ InfluxDB 2.x          192.168.1.100:8086
  ✅ Omniverse Nucleus     192.168.1.101:3009
  ⚠️  SAP ME 15.4          192.168.1.102:8080
  ⚠️  SAP S/4HANA 2022     192.168.1.103:443

# 掃描特定類型的系統
$ ndh-admin scan --range "192.168.1.0/24" --type influxdb

# 掃描並自動配置
$ ndh-admin scan --range "192.168.1.0/24" --auto-configure

# 匯出掃描結果
$ ndh-admin scan --range "192.168.1.0/24" --output scan_results.json
```

---

## 配置檔案格式

### 完整配置範例

```yaml
# /etc/ndh/config.yaml

# NDH 全域配置
ndh:
  mode: "cluster"  # single, cluster, cloud
  version: "3.5.1"
  
  # 叢集配置
  cluster:
    master_nodes:
      - "192.168.1.10:6443"
      - "192.168.1.11:6443"
      - "192.168.1.12:6443"
    
    worker_nodes:
      - "192.168.1.20:6443"
      - "192.168.1.21:6443"
      - "192.168.1.22:6443"
  
  # 網路配置
  network:
    listen_address: "0.0.0.0"
    api_port: 8080
    grpc_port: 9090

# 連接器配置
connectors:
  # 時序資料庫
  - name: "influxdb_production"
    type: "influxdb"
    enabled: true
    version: "2.x"
    connection:
      host: "192.168.1.100"
      port: 8086
      protocol: "http"
      org: "idtf"
      bucket: "factory_data"
      token: "${INFLUXDB_TOKEN}"
    settings:
      timeout: 10
      max_retries: 3
      batch_size: 5000
      flush_interval: 1000
    health_check:
      enabled: true
      interval: 30
      endpoint: "/health"
  
  # Omniverse
  - name: "omniverse_production"
    type: "omniverse"
    enabled: true
    nucleus_version: "2023.2.0"
    connection:
      host: "192.168.1.101"
      port: 3009
      repo: "Projects"
      usd_stage_path: "omniverse://192.168.1.101/Projects/Factory/factory.usd"
      auth:
        username: "${OMNIVERSE_USER}"
        password: "${OMNIVERSE_PASSWORD}"
    settings:
      sync_interval: 1000
      batch_updates: true
      enable_live_sync: true
      max_prims_per_batch: 1000
    health_check:
      enabled: true
      interval: 60
  
  # MES
  - name: "sap_me_production"
    type: "mes"
    enabled: true
    vendor: "sap"
    product: "SAP ME"
    version: "15.4"
    connection:
      host: "192.168.1.102"
      port: 8080
      protocol: "http"
      base_url: "http://192.168.1.102:8080/odata/v4"
      auth:
        type: "basic"
        username: "${SAP_ME_USER}"
        password: "${SAP_ME_PASSWORD}"
    settings:
      polling_interval: 5000
      batch_size: 100
      timeout: 30
    mappings:
      work_order: "WorkOrders"
      operation: "Operations"
      material: "Materials"
      production_data: "ProductionData"
    health_check:
      enabled: true
      interval: 30
      endpoint: "/sap/public/ping"
  
  # ERP
  - name: "sap_s4hana_production"
    type: "erp"
    enabled: true
    vendor: "sap"
    product: "S/4HANA"
    version: "2022"
    connection:
      host: "192.168.1.103"
      port: 443
      protocol: "https"
      base_url: "https://192.168.1.103/sap/opu/odata/sap"
      auth:
        type: "oauth2"
        client_id: "${SAP_CLIENT_ID}"
        client_secret: "${SAP_CLIENT_SECRET}"
        token_url: "https://192.168.1.103/sap/bc/sec/oauth2/token"
    settings:
      polling_interval: 60000
      batch_size: 50
      timeout: 60
    mappings:
      material: "API_PRODUCT_SRV/A_Product"
      purchase_order: "API_PURCHASEORDER_PROCESS_SRV/A_PurchaseOrder"
      sales_order: "API_SALES_ORDER_SRV/A_SalesOrder"
      production_order: "API_PRODUCTION_ORDER_2_SRV/A_ProductionOrder_2"
    health_check:
      enabled: true
      interval: 300
      endpoint: "/sap/public/ping"

# 日誌配置
logging:
  level: "info"  # debug, info, warning, error
  output: "stdout"  # stdout, file, syslog
  file: "/var/log/ndh/ndh.log"
  max_size: "100MB"
  max_backups: 10

# 監控配置
monitoring:
  enabled: true
  prometheus:
    enabled: true
    port: 9091
  grafana:
    enabled: true
    port: 3000
```

---

## 連接器測試與驗證

### 1. 自動化測試套件

```python
class ConnectorTestSuite:
    def __init__(self, connector_config: dict):
        self.config = connector_config
        self.connector = self.create_connector()
    
    def run_all_tests(self) -> dict:
        """執行所有測試"""
        results = {
            'connector_name': self.config['name'],
            'connector_type': self.config['type'],
            'tests': []
        }
        
        # 1. 連接測試
        results['tests'].append(self.test_connection())
        
        # 2. 認證測試
        results['tests'].append(self.test_authentication())
        
        # 3. 讀取測試
        results['tests'].append(self.test_read())
        
        # 4. 寫入測試
        results['tests'].append(self.test_write())
        
        # 5. 效能測試
        results['tests'].append(self.test_performance())
        
        # 6. 故障恢復測試
        results['tests'].append(self.test_failover())
        
        return results
    
    def test_connection(self) -> dict:
        """測試連接"""
        try:
            start_time = time.time()
            self.connector.connect()
            latency = (time.time() - start_time) * 1000
            
            return {
                'name': 'Connection Test',
                'status': 'passed',
                'latency_ms': latency,
                'message': f'Connected successfully in {latency:.2f}ms'
            }
        except Exception as e:
            return {
                'name': 'Connection Test',
                'status': 'failed',
                'error': str(e)
            }
    
    def test_authentication(self) -> dict:
        """測試認證"""
        try:
            self.connector.authenticate()
            return {
                'name': 'Authentication Test',
                'status': 'passed',
                'message': 'Authentication successful'
            }
        except Exception as e:
            return {
                'name': 'Authentication Test',
                'status': 'failed',
                'error': str(e)
            }
    
    def test_read(self) -> dict:
        """測試讀取"""
        try:
            data = self.connector.read_test_data()
            return {
                'name': 'Read Test',
                'status': 'passed',
                'records_read': len(data),
                'message': f'Successfully read {len(data)} records'
            }
        except Exception as e:
            return {
                'name': 'Read Test',
                'status': 'failed',
                'error': str(e)
            }
    
    def test_write(self) -> dict:
        """測試寫入"""
        try:
            test_data = self.generate_test_data()
            self.connector.write(test_data)
            return {
                'name': 'Write Test',
                'status': 'passed',
                'records_written': len(test_data),
                'message': f'Successfully wrote {len(test_data)} records'
            }
        except Exception as e:
            return {
                'name': 'Write Test',
                'status': 'failed',
                'error': str(e)
            }
    
    def test_performance(self) -> dict:
        """測試效能"""
        try:
            # 寫入效能
            write_start = time.time()
            test_data = self.generate_test_data(count=1000)
            self.connector.write(test_data)
            write_time = time.time() - write_start
            write_throughput = len(test_data) / write_time
            
            # 讀取效能
            read_start = time.time()
            data = self.connector.read_test_data(limit=1000)
            read_time = time.time() - read_start
            read_throughput = len(data) / read_time
            
            return {
                'name': 'Performance Test',
                'status': 'passed',
                'write_throughput': f'{write_throughput:.2f} records/s',
                'read_throughput': f'{read_throughput:.2f} records/s',
                'message': 'Performance test completed'
            }
        except Exception as e:
            return {
                'name': 'Performance Test',
                'status': 'failed',
                'error': str(e)
            }
```

### 2. 健康檢查

```python
class ConnectorHealthCheck:
    def __init__(self, connector_config: dict):
        self.config = connector_config
        self.connector = self.create_connector()
    
    def check_health(self) -> dict:
        """檢查連接器健康狀態"""
        health = {
            'connector_name': self.config['name'],
            'connector_type': self.config['type'],
            'timestamp': datetime.now().isoformat(),
            'status': 'unknown',
            'checks': []
        }
        
        # 1. 連接檢查
        connection_check = self.check_connection()
        health['checks'].append(connection_check)
        
        # 2. 響應時間檢查
        latency_check = self.check_latency()
        health['checks'].append(latency_check)
        
        # 3. 錯誤率檢查
        error_rate_check = self.check_error_rate()
        health['checks'].append(error_rate_check)
        
        # 4. 資源使用檢查
        resource_check = self.check_resources()
        health['checks'].append(resource_check)
        
        # 確定整體狀態
        if all(check['status'] == 'healthy' for check in health['checks']):
            health['status'] = 'healthy'
        elif any(check['status'] == 'critical' for check in health['checks']):
            health['status'] = 'critical'
        else:
            health['status'] = 'degraded'
        
        return health
    
    def check_connection(self) -> dict:
        """檢查連接"""
        try:
            self.connector.ping()
            return {
                'name': 'Connection',
                'status': 'healthy',
                'message': 'Connection is active'
            }
        except Exception as e:
            return {
                'name': 'Connection',
                'status': 'critical',
                'message': f'Connection failed: {str(e)}'
            }
    
    def check_latency(self) -> dict:
        """檢查響應時間"""
        try:
            start_time = time.time()
            self.connector.ping()
            latency = (time.time() - start_time) * 1000
            
            if latency < 100:
                status = 'healthy'
            elif latency < 500:
                status = 'degraded'
            else:
                status = 'critical'
            
            return {
                'name': 'Latency',
                'status': status,
                'value': f'{latency:.2f}ms',
                'threshold': '< 100ms (healthy), < 500ms (degraded)'
            }
        except Exception as e:
            return {
                'name': 'Latency',
                'status': 'critical',
                'message': f'Latency check failed: {str(e)}'
            }
    
    def check_error_rate(self) -> dict:
        """檢查錯誤率"""
        try:
            metrics = self.connector.get_metrics()
            error_rate = metrics['error_count'] / metrics['total_requests'] * 100
            
            if error_rate < 1:
                status = 'healthy'
            elif error_rate < 5:
                status = 'degraded'
            else:
                status = 'critical'
            
            return {
                'name': 'Error Rate',
                'status': status,
                'value': f'{error_rate:.2f}%',
                'threshold': '< 1% (healthy), < 5% (degraded)'
            }
        except Exception as e:
            return {
                'name': 'Error Rate',
                'status': 'unknown',
                'message': f'Error rate check failed: {str(e)}'
            }
```

---

## 故障排除

### 常見問題與解決方案

#### 1. 無法發現系統

**問題**: 網路掃描無法發現任何系統

**可能原因**:
- 網路不通
- 防火牆阻擋
- IP 範圍錯誤
- 端口未開放

**解決方案**:
```bash
# 1. 檢查網路連通性
$ ping 192.168.1.100

# 2. 檢查端口是否開放
$ nc -zv 192.168.1.100 8086

# 3. 檢查防火牆規則
$ sudo iptables -L

# 4. 使用 nmap 掃描
$ nmap -p 8086 192.168.1.100

# 5. 手動指定系統
$ ndh-admin connector add \
  --name my_influxdb \
  --type influxdb \
  --host 192.168.1.100 \
  --port 8086
```

#### 2. 連接測試失敗

**問題**: 連接器配置後測試失敗

**可能原因**:
- 認證資訊錯誤
- 網路問題
- 系統配置錯誤
- 版本不相容

**解決方案**:
```bash
# 1. 檢查連接器日誌
$ ndh-admin connector logs influxdb_production

# 2. 測試連接
$ ndh-admin connector test influxdb_production --verbose

# 3. 檢查認證資訊
$ ndh-admin connector show influxdb_production

# 4. 重新配置
$ ndh-admin connector edit influxdb_production
```

#### 3. 效能問題

**問題**: 連接器響應慢或超時

**可能原因**:
- 網路延遲
- 系統負載高
- 批次大小不當
- 連接池配置不當

**解決方案**:
```bash
# 1. 檢查延遲
$ ndh-admin connector ping influxdb_production

# 2. 調整批次大小
$ ndh-admin connector config influxdb_production \
  --set batch_size=1000

# 3. 調整超時時間
$ ndh-admin connector config influxdb_production \
  --set timeout=30

# 4. 調整連接池
$ ndh-admin connector config influxdb_production \
  --set connection_pool.max_size=50
```

---

## 最佳實踐

### 1. 安全性

- ✅ 使用環境變數儲存敏感資訊（Token、密碼）
- ✅ 啟用 TLS/SSL 加密
- ✅ 定期輪換認證資訊
- ✅ 限制網路訪問（防火牆規則）
- ✅ 啟用審計日誌

### 2. 可靠性

- ✅ 啟用健康檢查
- ✅ 配置重試機制
- ✅ 設置合理的超時時間
- ✅ 監控連接器狀態
- ✅ 定期備份配置

### 3. 效能

- ✅ 調整批次大小
- ✅ 使用連接池
- ✅ 啟用快取
- ✅ 優化輪詢間隔
- ✅ 監控效能指標

### 4. 可維護性

- ✅ 使用配置即代碼
- ✅ 版本控制配置檔案
- ✅ 文檔化自訂配置
- ✅ 定期審查和更新
- ✅ 自動化測試

---

## 總結

NDH 自動化初始設定與連接器配置系統提供了：

1. ✅ **零配置優先**: 自動發現和配置
2. ✅ **多種配置方式**: CLI、Web UI、配置檔案
3. ✅ **完整的測試與驗證**: 自動化測試套件
4. ✅ **健康監控**: 持續監控連接器狀態
5. ✅ **故障排除**: 詳細的診斷和解決方案

這將大幅降低 NDH 的部署門檻，從數天縮短到數小時，並減少配置錯誤，提升使用者體驗。

---

**© 2025 IDTF Consortium. All rights reserved.**

