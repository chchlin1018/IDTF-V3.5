# NDH 連接器插件安裝與使用指南

**版本**: 1.0.0  
**日期**: 2025-10-10  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)

---

## 目錄

1. [**簡介**](#1-簡介)
2. [**尋找插件**](#2-尋找插件)
3. [**安裝插件**](#3-安裝插件)
4. [**配置插件**](#4-配置插件)
5. [**使用插件**](#5-使用插件)
6. [**管理插件**](#6-管理插件)
7. [**完整範例: 安裝與使用 Modbus TCP 插件**](#7-完整範例-安裝與使用-modbus-tcp-插件)
8. [**故障排除**](#8-故障排除)

---

## 1. 簡介

### 1.1. 指南目的

本指南旨在引導使用者完成 NDH 連接器插件的安裝、配置和使用。透過本指南,您將學會如何為 NDH 擴充新的系統整合能力,並與這些系統進行互動。

### 1.2. 什麼是 NDH 插件?

NDH 插件是一種可擴展的模組,允許使用者在不修改 NDH 核心程式碼的情況下,新增對新工業系統的支援。這使得 NDH 能夠輕鬆地與各種不同的設備和軟體進行整合。

---

## 2. 尋找插件

### 2.1. 官方插件

NDH 官方提供了一系列經過測試和驗證的插件,涵蓋了主流的工業通訊協定和系統。

您可以在 NDH 官方 GitHub 倉庫的 `plugins/` 目錄下找到這些插件:
- **GitHub**: [https://github.com/chchlin1018/NDH-Win-Test1/tree/main/plugins](https://github.com/chchlin1018/NDH-Win-Test1/tree/main/plugins)

### 2.2. 社群插件

(此功能仍在開發中)

未來,NDH 將建立一個插件市場,讓社群成員可以分享和下載插件。

### 2.3. 插件資訊

每個插件都包含一個 `plugin.yaml` 檔案,其中定義了插件的元資料,包括:

- **名稱和描述**: 插件的功能
- **版本**: 插件的版本號
- **作者**: 插件的開發者
- **依賴**: 插件所需的 Python 套件
- **配置**: 插件的配置參數

在安裝插件之前,請務必閱讀 `plugin.yaml` 和 `README.md` 檔案,了解插件的功能和使用方法。

---

## 3. 安裝插件

### 3.1. 步驟 1: 下載插件

從 GitHub 或其他來源下載您需要的插件。您可以直接克隆整個倉庫,或只下載特定的插件目錄。

```bash
# 克隆整個倉庫
$ git clone https://github.com/chchlin1018/NDH-Win-Test1.git

# 或者只下載 Modbus TCP 插件
$ git clone --depth 1 --filter=blob:none --sparse https://github.com/chchlin1018/NDH-Win-Test1.git
$ cd NDH-Win-Test1
$ git sparse-checkout set plugins/modbus_tcp_plugin
```

### 3.2. 步驟 2: 複製到插件目錄

將下載的插件目錄複製到 NDH 專案的 `plugins/` 目錄下。

```bash
$ cp -r /path/to/downloaded/modbus_tcp_plugin /path/to/ndh/plugins/
```

### 3.3. 步驟 3: 安裝依賴

每個插件都有一個 `requirements.txt` 檔案,列出了該插件所需的 Python 套件。您需要安裝這些依賴。

```bash
# 進入 NDH 專案目錄
$ cd /path/to/ndh

# 啟動虛擬環境
$ source venv/bin/activate

# 安裝插件依賴
$ pip install -r plugins/modbus_tcp_plugin/requirements.txt
```

---

## 4. 配置插件

### 4.1. 找到配置檔案

NDH 的主配置檔案位於 `config/config.yaml`。

### 4.2. 啟用並配置插件

在 `config.yaml` 中,找到 `plugins` 區塊,並為您要使用的插件新增一個條目。

```yaml
# config/config.yaml

plugins:
  # 插件 ID (來自 plugin.yaml)
  modbus_tcp_connector:
    # 啟用插件
    enabled: true
    
    # 插件配置 (來自 plugin.yaml 的 configuration 區塊)
    config:
      host: "192.168.1.100"
      port: 502
      unit_id: 1
      timeout: 10
  
  # 您可以同時配置多個插件
  mqtt_connector:
    enabled: true
    config:
      broker: "mqtt.example.com"
      port: 1883
```

**重要提示**:
- **插件 ID**: 必須與插件目錄中 `plugin.yaml` 檔案裡定義的 `plugin.id` 完全相符。
- **配置參數**: 必須與 `plugin.yaml` 中 `configuration` 區塊定義的參數相符。

### 4.3. 重新啟動 NDH

儲存配置檔案後,重新啟動 NDH 以載入新的插件和配置。

```bash
$ uvicorn ndh.main:app --reload
```

您應該會在日誌中看到插件已成功載入的訊息:

```
INFO:     Modbus TCP Plugin loaded for 192.168.1.100:502
INFO:     Modbus TCP Plugin starting...
INFO:     Connected to Modbus TCP server successfully
```

---

## 5. 使用插件

一旦插件被載入,您就可以透過 NDH Broker 與其互動。

### 5.1. 取得插件實例

```python
from ndh.core.broker import NDHBroker

# 建立 Broker 實例
broker = NDHBroker()

# 載入插件
await broker.load_plugins()

# 取得插件實例
modbus_plugin = broker.plugin_manager.get_plugin("modbus_tcp_connector")
```

### 5.2. 呼叫插件方法

您可以直接呼叫插件類別中定義的方法。

```python
# 讀取資料
value = await modbus_plugin.read("holding:100")
print(f"Value: {value}")

# 寫入資料
success = await modbus_plugin.write("coil:0", True)
print(f"Write success: {success}")
```

### 5.3. 透過 NDH API 使用插件

您也可以透過 NDH 的 REST API 與插件互動。

#### 讀取資料

```bash
$ curl -X GET "http://127.0.0.1:8000/plugins/modbus_tcp_connector/read?address=holding:100"

{
  "address": "holding:100",
  "value": 123
}
```

#### 寫入資料

```bash
$ curl -X POST "http://127.0.0.1:8000/plugins/modbus_tcp_connector/write" \
  -H "Content-Type: application/json" \
  -d '{"address": "coil:0", "value": true}'

{
  "success": true
}
```

---

## 6. 管理插件

### 6.1. 停用插件

如果您想暫時停用一個插件,只需在 `config.yaml` 中將 `enabled` 設為 `false`:

```yaml
plugins:
  modbus_tcp_connector:
    enabled: false
    # ...
```

### 6.2. 更新插件

1. 下載新版本的插件
2. 替換 `plugins/` 目錄下的舊版本
3. 安裝新的依賴 (如果 `requirements.txt` 有變更)
4. 重新啟動 NDH

### 6.3. 移除插件

1. 在 `config.yaml` 中移除插件的配置
2. 刪除 `plugins/` 目錄下的插件目錄
3. (可選) 卸載插件的依賴
4. 重新啟動 NDH

---

## 7. 完整範例: 安裝與使用 Modbus TCP 插件

### 7.1. 步驟 1: 準備環境

- 確保 NDH 已安裝並可執行
- 啟動一個 Modbus TCP 模擬器 (例如 `pymodbus.server`)

### 7.2. 步驟 2: 安裝插件

```bash
# 假設您已下載 Modbus TCP 插件到 /tmp
$ cp -r /tmp/modbus_tcp_plugin /path/to/ndh/plugins/

# 安裝依賴
$ pip install -r /path/to/ndh/plugins/modbus_tcp_plugin/requirements.txt
```

### 7.3. 步驟 3: 配置插件

編輯 `config/config.yaml`:

```yaml
plugins:
  modbus_tcp_connector:
    enabled: true
    config:
      host: "127.0.0.1"  # 模擬器的主機
      port: 502
```

### 7.4. 步驟 4: 啟動 NDH

```bash
$ uvicorn ndh.main:app --reload
```

### 7.5. 步驟 5: 測試插件

建立一個 Python 腳本 `test_modbus.py`:

```python
import asyncio
import requests

async def main():
    # 寫入資料
    response = requests.post(
        "http://127.0.0.1:8000/plugins/modbus_tcp_connector/write",
        json={"address": "holding:100", "value": 456}
    )
    print(f"Write response: {response.json()}")
    
    # 讀取資料
    response = requests.get(
        "http://127.0.0.1:8000/plugins/modbus_tcp_connector/read?address=holding:100"
    )
    print(f"Read response: {response.json()}")

if __name__ == "__main__":
    asyncio.run(main())
```

執行腳本:

```bash
$ python test_modbus.py

Write response: {'success': True}
Read response: {'address': 'holding:100', 'value': 456}
```

---

## 8. 故障排除

### 插件未載入

- **檢查 `config.yaml`**: 確保插件 ID 正確,且 `enabled` 為 `true`。
- **檢查日誌**: 查看 NDH 啟動時的日誌,是否有任何錯誤訊息。
- **檢查目錄結構**: 確保插件目錄結構正確,包含 `plugin.yaml` 和 `plugin.py`。

### 連接錯誤

- **檢查配置**: 確保主機、埠號、認證等資訊正確。
- **網路問題**: 檢查 NDH 是否可以存取目標系統。
- **防火牆**: 檢查防火牆是否阻擋了連接。

### 依賴問題

- **虛擬環境**: 確保您已啟動正確的虛擬環境。
- **安裝依賴**: 確保您已安裝插件的 `requirements.txt` 中的所有依賴。

---

**版本**: 1.0.0  
**日期**: 2025-10-10  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)
