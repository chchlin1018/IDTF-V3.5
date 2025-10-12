# NDH 插件架構設計

**版本**: 1.0.0  
**日期**: 2025-10-10  
**作者**: 林志錚 (Chih-Cheng Lin, Michael Lin)

---

## 目錄

1. [概述](#1-概述)
2. [插件架構](#2-插件架構)
3. [插件生命週期](#3-插件生命週期)
4. [插件介面](#4-插件介面)
5. [插件載入機制](#5-插件載入機制)
6. [插件配置](#6-插件配置)
7. [插件市場](#7-插件市場)

---

## 1. 概述

### 1.1. 什麼是 NDH 插件?

NDH 插件是一種可擴展的連接器模組,允許使用者在不修改 NDH 核心程式碼的情況下,新增對新工業系統的支援。

### 1.2. 插件系統的優勢

- **可擴展性**: 輕鬆新增新的系統整合
- **模組化**: 插件獨立開發、測試和部署
- **社群驅動**: 鼓勵社群貢獻插件
- **版本管理**: 插件獨立版本控制
- **熱插拔**: 支援動態載入和卸載

### 1.3. 插件類型

| 插件類型 | 說明 | 範例 |
|---------|------|------|
| **連接器插件** | 連接到外部系統 | Modbus 連接器、MQTT 連接器 |
| **轉換器插件** | 資料轉換和映射 | JSON 到 IADL 轉換器 |
| **處理器插件** | 資料處理和分析 | 異常偵測、資料聚合 |
| **儲存插件** | 資料儲存後端 | InfluxDB、TimescaleDB |
| **通知插件** | 告警和通知 | Email、Slack、Teams |

---

## 2. 插件架構

### 2.1. 整體架構

```
┌─────────────────────────────────────────────────────────┐
│                    NDH Core                              │
│  ┌────────────────────────────────────────────────┐    │
│  │         Plugin Manager                         │    │
│  │  ┌──────────────┐  ┌──────────────┐           │    │
│  │  │ Plugin       │  │ Plugin       │           │    │
│  │  │ Loader       │  │ Registry     │           │    │
│  │  └──────┬───────┘  └──────┬───────┘           │    │
│  └─────────┼──────────────────┼────────────────────┘    │
└────────────┼──────────────────┼──────────────────────────┘
             │                  │
             │ Load             │ Register
             │                  │
┌────────────▼──────────────────▼──────────────────────────┐
│                    Plugins                               │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │  Modbus      │  │  MQTT        │  │  Custom      │  │
│  │  Plugin      │  │  Plugin      │  │  Plugin      │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└─────────────────────────────────────────────────────────┘
```

### 2.2. 核心元件

#### 2.2.1. Plugin Manager

插件管理器負責:
- 載入和卸載插件
- 管理插件生命週期
- 維護插件註冊表
- 處理插件依賴

```python
class PluginManager:
    """插件管理器"""
    
    def __init__(self):
        self.plugins = {}           # 已載入的插件
        self.registry = {}          # 插件註冊表
        self.loader = PluginLoader()
    
    async def load_plugin(self, plugin_path: str) -> Plugin:
        """載入插件"""
        pass
    
    async def unload_plugin(self, plugin_id: str):
        """卸載插件"""
        pass
    
    def get_plugin(self, plugin_id: str) -> Plugin:
        """取得插件"""
        pass
    
    def list_plugins(self) -> List[PluginInfo]:
        """列出所有插件"""
        pass
```

#### 2.2.2. Plugin Loader

插件載入器負責:
- 發現插件
- 驗證插件
- 載入插件模組
- 解析插件元資料

```python
class PluginLoader:
    """插件載入器"""
    
    def discover_plugins(self, plugin_dir: str) -> List[str]:
        """發現插件"""
        pass
    
    def validate_plugin(self, plugin_path: str) -> bool:
        """驗證插件"""
        pass
    
    def load_module(self, plugin_path: str) -> ModuleType:
        """載入插件模組"""
        pass
    
    def parse_metadata(self, plugin_path: str) -> PluginMetadata:
        """解析插件元資料"""
        pass
```

#### 2.2.3. Plugin Registry

插件註冊表負責:
- 儲存插件資訊
- 管理插件依賴
- 追蹤插件狀態
- 提供插件查詢

```python
class PluginRegistry:
    """插件註冊表"""
    
    def register(self, plugin: Plugin):
        """註冊插件"""
        pass
    
    def unregister(self, plugin_id: str):
        """取消註冊插件"""
        pass
    
    def find_by_id(self, plugin_id: str) -> Optional[PluginInfo]:
        """根據 ID 查找插件"""
        pass
    
    def find_by_type(self, plugin_type: str) -> List[PluginInfo]:
        """根據類型查找插件"""
        pass
```

---

## 3. 插件生命週期

### 3.1. 生命週期階段

```
┌─────────┐     ┌─────────┐     ┌─────────┐     ┌─────────┐
│ Created │ ──→ │ Loaded  │ ──→ │ Started │ ──→ │ Running │
└─────────┘     └─────────┘     └─────────┘     └─────────┘
                                                       │
                                                       ↓
┌─────────┐     ┌─────────┐     ┌─────────┐     ┌─────────┐
│ Removed │ ←── │ Unloaded│ ←── │ Stopped │ ←── │ Paused  │
└─────────┘     └─────────┘     └─────────┘     └─────────┘
```

### 3.2. 生命週期方法

```python
class Plugin(ABC):
    """插件基礎類別"""
    
    @abstractmethod
    async def on_load(self):
        """載入時呼叫"""
        pass
    
    @abstractmethod
    async def on_start(self):
        """啟動時呼叫"""
        pass
    
    @abstractmethod
    async def on_stop(self):
        """停止時呼叫"""
        pass
    
    @abstractmethod
    async def on_unload(self):
        """卸載時呼叫"""
        pass
```

### 3.3. 生命週期範例

```python
class ModbusPlugin(Plugin):
    """Modbus 插件"""
    
    async def on_load(self):
        """載入時初始化"""
        self.logger = logging.getLogger(__name__)
        self.logger.info("Modbus plugin loaded")
    
    async def on_start(self):
        """啟動時建立連接"""
        self.client = ModbusTcpClient(self.config['host'])
        await self.client.connect()
        self.logger.info("Modbus plugin started")
    
    async def on_stop(self):
        """停止時斷開連接"""
        await self.client.disconnect()
        self.logger.info("Modbus plugin stopped")
    
    async def on_unload(self):
        """卸載時清理資源"""
        self.logger.info("Modbus plugin unloaded")
```

---

## 4. 插件介面

### 4.1. 連接器插件介面

```python
class ConnectorPlugin(Plugin):
    """連接器插件介面"""
    
    @abstractmethod
    async def connect(self) -> bool:
        """建立連接"""
        pass
    
    @abstractmethod
    async def disconnect(self):
        """斷開連接"""
        pass
    
    @abstractmethod
    async def read(self, address: str) -> Any:
        """讀取資料"""
        pass
    
    @abstractmethod
    async def write(self, address: str, value: Any) -> bool:
        """寫入資料"""
        pass
    
    @abstractmethod
    async def subscribe(self, addresses: List[str], callback: Callable):
        """訂閱資料變更"""
        pass
```

### 4.2. 轉換器插件介面

```python
class TransformerPlugin(Plugin):
    """轉換器插件介面"""
    
    @abstractmethod
    async def transform(self, data: Dict) -> Dict:
        """轉換資料"""
        pass
    
    @abstractmethod
    def validate(self, data: Dict) -> bool:
        """驗證資料"""
        pass
```

### 4.3. 處理器插件介面

```python
class ProcessorPlugin(Plugin):
    """處理器插件介面"""
    
    @abstractmethod
    async def process(self, data: Dict) -> Dict:
        """處理資料"""
        pass
    
    @abstractmethod
    def get_statistics(self) -> Dict:
        """取得統計資訊"""
        pass
```

---

## 5. 插件載入機制

### 5.1. 插件目錄結構

```
plugins/
├── modbus_plugin/
│   ├── __init__.py
│   ├── plugin.py
│   ├── plugin.yaml
│   ├── requirements.txt
│   └── README.md
├── mqtt_plugin/
│   ├── __init__.py
│   ├── plugin.py
│   ├── plugin.yaml
│   ├── requirements.txt
│   └── README.md
└── custom_plugin/
    ├── __init__.py
    ├── plugin.py
    ├── plugin.yaml
    ├── requirements.txt
    └── README.md
```

### 5.2. 插件元資料 (plugin.yaml)

```yaml
# 插件基本資訊
plugin:
  id: modbus_connector
  name: Modbus TCP/RTU Connector
  version: 1.0.0
  type: connector
  author: Your Name
  email: your.email@example.com
  license: MIT
  homepage: https://github.com/yourname/modbus-plugin

# 插件描述
description:
  short: Modbus TCP/RTU connector for NDH
  long: |
    This plugin provides connectivity to Modbus TCP and RTU devices.
    Supports reading/writing holding registers, coils, and input registers.

# 插件依賴
dependencies:
  python: ">=3.11"
  ndh: ">=0.4.0"
  packages:
    - pymodbus>=3.0.0
    - asyncio-mqtt>=0.16.0

# 插件配置
configuration:
  host:
    type: string
    required: true
    description: Modbus server host
  port:
    type: integer
    required: false
    default: 502
    description: Modbus server port
  timeout:
    type: integer
    required: false
    default: 10
    description: Connection timeout in seconds

# 插件能力
capabilities:
  - read
  - write
  - subscribe
  - batch_read
  - batch_write

# 插件標籤
tags:
  - modbus
  - industrial
  - plc
```

### 5.3. 插件載入流程

```python
async def load_plugin_flow(plugin_path: str):
    """插件載入流程"""
    
    # 1. 發現插件
    plugin_dirs = discover_plugins("plugins/")
    
    # 2. 驗證插件
    for plugin_dir in plugin_dirs:
        if not validate_plugin(plugin_dir):
            logger.warning(f"Invalid plugin: {plugin_dir}")
            continue
        
        # 3. 解析元資料
        metadata = parse_metadata(plugin_dir)
        
        # 4. 檢查依賴
        if not check_dependencies(metadata):
            logger.warning(f"Missing dependencies: {plugin_dir}")
            continue
        
        # 5. 載入模組
        module = load_module(plugin_dir)
        
        # 6. 建立插件實例
        plugin_class = getattr(module, metadata.plugin_class)
        plugin = plugin_class(metadata.configuration)
        
        # 7. 呼叫 on_load
        await plugin.on_load()
        
        # 8. 註冊插件
        plugin_manager.register(plugin)
        
        logger.info(f"Plugin loaded: {metadata.name} v{metadata.version}")
```

---

## 6. 插件配置

### 6.1. 配置檔案

插件配置可以透過 YAML 或 JSON 檔案提供:

```yaml
# config/plugins.yaml
plugins:
  modbus_connector:
    enabled: true
    config:
      host: "192.168.1.100"
      port: 502
      timeout: 10
      unit_id: 1
  
  mqtt_connector:
    enabled: true
    config:
      broker: "mqtt.example.com"
      port: 1883
      username: "user"
      password: "pass"
```

### 6.2. 環境變數

插件也可以透過環境變數配置:

```bash
# Modbus 插件配置
export NDH_PLUGIN_MODBUS_HOST="192.168.1.100"
export NDH_PLUGIN_MODBUS_PORT="502"

# MQTT 插件配置
export NDH_PLUGIN_MQTT_BROKER="mqtt.example.com"
export NDH_PLUGIN_MQTT_PORT="1883"
```

### 6.3. 動態配置

插件支援執行時動態配置:

```python
# 取得插件
plugin = plugin_manager.get_plugin("modbus_connector")

# 更新配置
await plugin.update_config({
    "host": "192.168.1.101",
    "port": 502
})

# 重新載入
await plugin.reload()
```

---

## 7. 插件市場

### 7.1. 插件市場架構

```
┌─────────────────────────────────────────────────────────┐
│                  Plugin Marketplace                      │
│  ┌────────────────────────────────────────────────┐    │
│  │         Plugin Repository                      │    │
│  │  ┌──────────────┐  ┌──────────────┐           │    │
│  │  │ Official     │  │ Community    │           │    │
│  │  │ Plugins      │  │ Plugins      │           │    │
│  │  └──────────────┘  └──────────────┘           │    │
│  └────────────────────────────────────────────────┘    │
│                                                          │
│  ┌────────────────────────────────────────────────┐    │
│  │         Plugin Manager CLI                     │    │
│  │  - Search plugins                              │    │
│  │  - Install plugins                             │    │
│  │  - Update plugins                              │    │
│  │  - Remove plugins                              │    │
│  └────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────┘
```

### 7.2. 插件市場 CLI

```bash
# 搜尋插件
$ ndh plugin search modbus
Found 3 plugins:
  1. modbus_tcp_connector (v1.0.0) - Modbus TCP connector
  2. modbus_rtu_connector (v1.0.0) - Modbus RTU connector
  3. modbus_gateway (v0.5.0) - Modbus gateway plugin

# 安裝插件
$ ndh plugin install modbus_tcp_connector
Installing modbus_tcp_connector v1.0.0...
✓ Downloaded plugin
✓ Installed dependencies
✓ Loaded plugin
Plugin installed successfully!

# 列出已安裝的插件
$ ndh plugin list
Installed plugins:
  - modbus_tcp_connector (v1.0.0) [enabled]
  - mqtt_connector (v1.0.0) [enabled]
  - opcua_connector (v0.4.0) [disabled]

# 更新插件
$ ndh plugin update modbus_tcp_connector
Updating modbus_tcp_connector...
✓ Downloaded v1.1.0
✓ Updated dependencies
✓ Reloaded plugin
Plugin updated successfully!

# 移除插件
$ ndh plugin remove modbus_tcp_connector
Removing modbus_tcp_connector...
✓ Stopped plugin
✓ Unloaded plugin
✓ Removed files
Plugin removed successfully!
```

### 7.3. 插件認證

官方插件市場提供插件認證服務:

| 認證等級 | 說明 | 標誌 |
|---------|------|------|
| **官方** | NDH 團隊開發和維護 | ✅ Official |
| **認證** | 通過安全和品質審核 | ✓ Verified |
| **社群** | 社群貢獻,未經審核 | ⚠ Community |

---

## 8. 插件安全

### 8.1. 沙箱機制

插件在沙箱環境中執行,限制其存取權限:

```python
class PluginSandbox:
    """插件沙箱"""
    
    def __init__(self, plugin: Plugin):
        self.plugin = plugin
        self.allowed_modules = [
            "asyncio",
            "logging",
            "json",
            "datetime"
        ]
    
    def execute(self, method: str, *args, **kwargs):
        """在沙箱中執行插件方法"""
        # 檢查模組匯入
        # 限制檔案系統存取
        # 限制網路存取
        # 限制資源使用
        pass
```

### 8.2. 權限系統

插件需要聲明所需權限:

```yaml
# plugin.yaml
permissions:
  - network:connect          # 允許網路連接
  - filesystem:read          # 允許讀取檔案
  - database:write           # 允許寫入資料庫
  - system:execute           # 允許執行系統命令
```

### 8.3. 程式碼簽名

官方和認證插件使用數位簽名:

```bash
# 簽名插件
$ ndh plugin sign modbus_plugin --key private.key

# 驗證簽名
$ ndh plugin verify modbus_plugin.zip
✓ Signature valid
✓ Signed by: NDH Team
✓ Certificate valid until: 2026-10-10
```

---

## 9. 插件效能

### 9.1. 效能監控

插件管理器提供效能監控:

```python
class PluginMonitor:
    """插件效能監控"""
    
    def get_metrics(self, plugin_id: str) -> Dict:
        """取得插件指標"""
        return {
            "cpu_usage": 5.2,          # CPU 使用率 (%)
            "memory_usage": 45.6,      # 記憶體使用 (MB)
            "requests_per_second": 150,# 每秒請求數
            "average_latency": 12.3,   # 平均延遲 (ms)
            "error_rate": 0.01         # 錯誤率 (%)
        }
```

### 9.2. 效能限制

可以為插件設定資源限制:

```yaml
# plugin.yaml
limits:
  cpu: 50%              # CPU 使用上限
  memory: 512MB         # 記憶體使用上限
  requests: 1000/s      # 每秒請求上限
  connections: 100      # 並發連接上限
```

---

## 10. 總結

NDH 插件架構提供了一個強大、靈活和安全的擴展機制,使 NDH 能夠輕鬆整合新的工業系統,並支援社群驅動的生態系統。

### 主要特色

- ✅ **模組化設計**: 插件獨立開發和部署
- ✅ **生命週期管理**: 完整的插件生命週期控制
- ✅ **動態載入**: 支援熱插拔
- ✅ **配置靈活**: 多種配置方式
- ✅ **插件市場**: 官方和社群插件
- ✅ **安全機制**: 沙箱、權限和簽名
- ✅ **效能監控**: 完整的效能指標

---

**版本**: 1.0.0  
**日期**: 2025-10-10  
**作者**: 林志錚 (Chih-Cheng Lin, Michael Lin)

