# NDH 連接器插件開發指南

**版本**: 1.0.0  
**日期**: 2025-10-10  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)

---

## 目錄

1. [**簡介**](#1-簡介)
2. [**開發環境設置**](#2-開發環境設置)
3. [**建立您的第一個插件**](#3-建立您的第一個插件)
4. [**實作連接器邏輯**](#4-實作連接器邏輯)
5. [**處理配置與日誌**](#5-處理配置與日誌)
6. [**測試您的插件**](#6-測試您的插件)
7. [**打包與發布**](#7-打包與發布)
8. [**完整範例: Modbus TCP 連接器**](#8-完整範例-modbus-tcp-連接器)
9. [**最佳實踐**](#9-最佳實踐)

---

## 1. 簡介

### 1.1. 指南目的

本指南旨在引導開發者完成 NDH 連接器插件的完整開發流程,從環境設置、建立專案、實作邏輯、測試到最終打包發布。透過本指南,您將學會如何為 NDH 擴充新的工業系統整合能力。

### 1.2. 先備知識

在開始之前,您應該具備以下知識:

- **Python 3.11+**: 熟悉 `async/await` 語法
- **物件導向程式設計 (OOP)**: 熟悉類別和繼承
- **工業通訊協定**: 了解您要整合的協定 (例如 Modbus, OPC UA, MQTT)
- **REST API**: 基本的 REST API 概念
- **Git**: 版本控制基礎

---

## 2. 開發環境設置

### 2.1. 安裝 NDH

首先,從 GitHub 克隆 NDH 專案並安裝依賴:

```bash
# 克隆專案
$ git clone https://github.com/chchlin1018/NDH-Win-Test1.git
$ cd NDH-Win-Test1

# 建立虛擬環境
$ python3 -m venv venv
$ source venv/bin/activate

# 安裝依賴
$ pip install -r requirements.txt
```

### 2.2. 安裝開發工具

建議安裝以下開發工具:

- **Visual Studio Code**: 推薦的程式碼編輯器
- **Python Extension for VS Code**: 提供豐富的 Python 開發功能
- **Black**: 自動化程式碼格式化
- **pylint**: 程式碼靜態分析

```bash
# 安裝開發工具
$ pip install black pylint
```

### 2.3. 驗證安裝

執行 NDH 測試套件,確保您的環境已正確設置:

```bash
$ pytest
============================= test session starts ==============================
...
============================= 100 passed in 5.12s ==============================
```

---

## 3. 建立您的第一個插件

### 3.1. 建立插件目錄

在 `plugins/` 目錄下建立您的插件專案:

```bash
$ mkdir -p plugins/my_first_plugin
$ cd plugins/my_first_plugin
```

您的插件目錄結構應如下:

```
my_first_plugin/
├── __init__.py         # 空白檔案,標識為 Python 套件
├── plugin.py           # 插件主要邏輯
├── plugin.yaml         # 插件元資料
├── requirements.txt    # 插件依賴
└── README.md           # 插件說明文件
```

### 3.2. 撰寫插件元資料 (`plugin.yaml`)

`plugin.yaml` 檔案定義了插件的所有元資料,包括 ID、名稱、版本、依賴和配置。這是 NDH 認識您插件的唯一方式。

```yaml
# plugins/my_first_plugin/plugin.yaml

# 1. 基本資訊
plugin:
  id: my_first_plugin
  name: My First Plugin
  version: 0.1.0
  type: connector
  author: Your Name
  email: your.email@example.com
  license: MIT

# 2. 描述
description:
  short: A simple example plugin for NDH
  long: This plugin demonstrates how to create a basic connector plugin.

# 3. 依賴
dependencies:
  python: ">=3.11"
  ndh: ">=0.4.0"
  packages:
    - requests>=2.28.0

# 4. 配置
configuration:
  api_endpoint:
    type: string
    required: true
    description: The API endpoint to connect to.
  api_key:
    type: string
    required: false
    description: The API key for authentication.
  timeout:
    type: integer
    required: false
    default: 10
    description: Request timeout in seconds.

# 5. 能力
capabilities:
  - read
  - write
```

### 3.3. 建立插件進入點 (`plugin.py`)

`plugin.py` 是插件的進入點,您需要建立一個繼承自 `ndh.connectors.base.ConnectorPlugin` 的類別。

```python
# plugins/my_first_plugin/plugin.py

import asyncio
import logging
from typing import Any, Dict, List

from ndh.connectors.base import ConnectorPlugin

# 建立日誌記錄器
logger = logging.getLogger(__name__)

class MyFirstPlugin(ConnectorPlugin):
    """我的第一個插件"""

    def __init__(self, config: Dict):
        super().__init__(config)
        self.client = None

    async def on_load(self):
        """插件載入時呼叫"""
        logger.info("My First Plugin loaded")

    async def on_start(self):
        """插件啟動時呼叫"""
        logger.info("My First Plugin started")
        await self.connect()

    async def on_stop(self):
        """插件停止時呼叫"""
        logger.info("My First Plugin stopped")
        await self.disconnect()

    async def on_unload(self):
        """插件卸載時呼叫"""
        logger.info("My First Plugin unloaded")

    async def connect(self) -> bool:
        """實作連接邏輯"""
        logger.info("Connecting to %s", self.config.get("api_endpoint"))
        # 在此處加入您的連接邏輯
        self.client = "connected"
        return True

    async def disconnect(self):
        """實作斷開連接邏輯"""
        logger.info("Disconnecting...")
        self.client = None

    async def read(self, address: str) -> Any:
        """實作讀取邏輯"""
        logger.info("Reading from %s", address)
        # 在此處加入您的讀取邏輯
        return {"address": address, "value": 123}

    async def write(self, address: str, value: Any) -> bool:
        """實作寫入邏輯"""
        logger.info("Writing %s to %s", value, address)
        # 在此處加入您的寫入邏輯
        return True

    async def subscribe(self, addresses: List[str], callback):
        """實作訂閱邏輯"""
        logger.info("Subscribing to %s", addresses)
        # 在此處加入您的訂閱邏輯
        # 範例: 每秒推送一次資料
        while True:
            await asyncio.sleep(1)
            for address in addresses:
                await callback({"address": address, "value": 456})
```

### 3.4. 啟用您的插件

在 NDH 的主配置檔案 `config/config.yaml` 中啟用您的插件:

```yaml
# config/config.yaml

plugins:
  my_first_plugin:
    enabled: true
    config:
      api_endpoint: "https://api.example.com/v1"
      api_key: "your-api-key"
```

現在,當您啟動 NDH 時,您的插件將會被載入和執行。

---

## 4. 實作連接器邏輯

### 4.1. `connect()` 和 `disconnect()`

- `connect()`: 建立與目標系統的連接,例如 TCP 連接、API 客戶端初始化。
- `disconnect()`: 安全地關閉連接,釋放資源。

### 4.2. `read()` 和 `write()`

- `read()`: 從目標系統讀取資料。`address` 參數是目標系統中的資料點位址。
- `write()`: 將資料寫入到目標系統。

### 4.3. `subscribe()`

`subscribe()` 是實現即時資料更新的關鍵。

- **回調函數**: `subscribe()` 接收一個 `callback` 函數。當有新資料時,您需要呼叫此函數,並將資料作為參數傳遞。
- **非同步任務**: 您通常需要建立一個非同步任務來處理訂閱,例如監聽 WebSocket 或輪詢 API。

```python
async def subscribe(self, addresses: List[str], callback):
    """訂閱範例"""
    async def _subscription_task():
        while True:
            try:
                # 輪詢 API
                data = await self.poll_api(addresses)
                
                # 推送資料
                for item in data:
                    await callback(item)
                
                await asyncio.sleep(1)
            except Exception as e:
                logger.error("Subscription error: %s", e)
                await asyncio.sleep(5) # 發生錯誤時等待
    
    # 建立並執行非同步任務
    asyncio.create_task(_subscription_task())
```

---

## 5. 處理配置與日誌

### 5.1. 存取配置

您的插件類別在初始化時會接收一個 `config` 字典。您可以透過 `self.config` 存取配置:

```python
host = self.config.get("host", "localhost")
port = self.config.get("port", 502)
timeout = self.config.get("timeout", 10)
```

### 5.2. 使用日誌

NDH 使用 Python 的 `logging` 模組。建議為您的插件建立一個專用的日誌記錄器:

```python
import logging

logger = logging.getLogger(__name__)

logger.info("This is an info message")
logger.warning("This is a warning message")
logger.error("This is an error message")
```

---

## 6. 測試您的插件

### 6.1. 單元測試

使用 `unittest.mock` 來模擬外部依賴,測試您的插件邏輯。

```python
# tests/test_my_first_plugin.py

import unittest
from unittest.mock import AsyncMock, patch

from plugins.my_first_plugin.plugin import MyFirstPlugin

class TestMyFirstPlugin(unittest.TestCase):

    @patch("plugins.my_first_plugin.plugin.requests.get")
    def test_read(self, mock_get):
        mock_get.return_value.json.return_value = {"value": 123}
        
        plugin = MyFirstPlugin({"api_endpoint": "http://test.com"})
        result = asyncio.run(plugin.read("test_address"))
        
        self.assertEqual(result, {"value": 123})
```

### 6.2. 整合測試

建立一個完整的整合測試,驗證您的插件與 NDH 的整合。

```python
# tests/integration/test_my_first_plugin_integration.py

import asyncio
import pytest

from ndh.core.broker import NDHBroker

@pytest.mark.asyncio
async def test_plugin_integration():
    broker = NDHBroker()
    await broker.load_plugins()
    
    # 取得插件
    plugin = broker.plugin_manager.get_plugin("my_first_plugin")
    self.assertIsNotNone(plugin)
    
    # 測試讀取
    result = await plugin.read("test_address")
    self.assertEqual(result["value"], 123)
```

---

## 7. 打包與發布

### 7.1. 打包插件

將您的插件目錄打包成一個 `.zip` 檔案:

```bash
$ zip -r my_first_plugin_v0.1.0.zip my_first_plugin/
```

### 7.2. 發布到插件市場

(此功能仍在開發中)

未來,您可以將您的插件提交到 NDH 官方插件市場:

```bash
$ ndh plugin publish my_first_plugin_v0.1.0.zip
```

---

## 8. 完整範例: Modbus TCP 連接器

(請參考下一份文件: `PLUGIN_EXAMPLE_MODBUS.md`)

---

## 9. 最佳實踐

1. **保持非同步**: 所有 I/O 操作都應該使用 `async/await`。
2. **優雅地處理錯誤**: 使用 `try...except` 捕捉錯誤,並記錄詳細的日誌。
3. **提供預設配置**: 在 `plugin.yaml` 中為非必要配置提供預設值。
4. **撰寫清晰的文件**: `README.md` 應該包含安裝、配置和使用說明。
5. **撰寫單元測試**: 確保您的插件穩定可靠。
6. **遵循 PEP 8**: 保持您的程式碼風格一致。
7. **版本管理**: 使用語意化版本 (Semantic Versioning)。

---

**版本**: 1.0.0  
**日期**: 2025-10-10  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)
