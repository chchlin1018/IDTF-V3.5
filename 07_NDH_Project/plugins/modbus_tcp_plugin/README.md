# Modbus TCP Connector Plugin for NDH

這是一個用於 NDH (Neutral Data Hub) 的 Modbus TCP 連接器插件,提供與 Modbus TCP 設備的連接能力。

## 功能特色

- ✅ 讀取 Holding Registers
- ✅ 讀取 Coils
- ✅ 讀取 Input Registers
- ✅ 讀取 Discrete Inputs
- ✅ 寫入 Holding Registers
- ✅ 寫入 Coils
- ✅ 批次讀取
- ✅ 批次寫入
- ✅ 資料訂閱 (透過輪詢)
- ✅ 自動重連
- ✅ 統計資訊

## 安裝

### 1. 安裝依賴

```bash
pip install pymodbus>=3.5.0
```

### 2. 複製插件到 NDH 插件目錄

```bash
cp -r modbus_tcp_plugin /path/to/ndh/plugins/
```

### 3. 配置插件

在 NDH 的配置檔案 `config/config.yaml` 中啟用插件:

```yaml
plugins:
  modbus_tcp_connector:
    enabled: true
    config:
      host: "192.168.1.100"
      port: 502
      unit_id: 1
      timeout: 10
```

## 配置參數

| 參數 | 類型 | 必要 | 預設值 | 說明 |
|------|------|------|--------|------|
| `host` | string | ✅ | - | Modbus TCP 伺服器主機 (IP 或主機名) |
| `port` | integer | ❌ | 502 | Modbus TCP 伺服器埠號 |
| `unit_id` | integer | ❌ | 1 | Modbus 單元 ID (從站 ID) |
| `timeout` | integer | ❌ | 10 | 連接逾時時間 (秒) |
| `retry_on_empty` | boolean | ❌ | false | 空回應時重試 |
| `retries` | integer | ❌ | 3 | 連接失敗時的重試次數 |

## 使用範例

### 讀取 Holding Register

```python
# 讀取位址 100 的 Holding Register
value = await plugin.read("holding:100")
print(f"Value: {value}")
```

### 寫入 Holding Register

```python
# 寫入值 123 到位址 100 的 Holding Register
success = await plugin.write("holding:100", 123)
print(f"Write success: {success}")
```

### 讀取 Coil

```python
# 讀取位址 0 的 Coil
value = await plugin.read("coil:0")
print(f"Coil value: {value}")
```

### 寫入 Coil

```python
# 寫入 True 到位址 0 的 Coil
success = await plugin.write("coil:0", True)
print(f"Write success: {success}")
```

### 批次讀取

```python
# 批次讀取多個位址
addresses = ["holding:100", "holding:101", "coil:0"]
results = await plugin.batch_read(addresses)
print(f"Results: {results}")
```

### 訂閱資料變更

```python
# 訂閱資料變更
async def on_data_change(data):
    print(f"Data changed: {data}")

await plugin.subscribe(["holding:100", "holding:101"], on_data_change)
```

### 取得統計資訊

```python
# 取得統計資訊
stats = plugin.get_statistics()
print(f"Statistics: {stats}")
```

## 位址格式

位址格式為 `type:address`,其中:

- `type`: 暫存器類型
  - `holding`: Holding Register (可讀寫)
  - `coil`: Coil (可讀寫)
  - `input`: Input Register (唯讀)
  - `discrete`: Discrete Input (唯讀)
- `address`: 暫存器位址 (整數)

範例:
- `holding:100` - Holding Register 位址 100
- `coil:0` - Coil 位址 0
- `input:200` - Input Register 位址 200
- `discrete:50` - Discrete Input 位址 50

如果省略類型,預設為 Holding Register:
- `100` 等同於 `holding:100`

## 測試

### 使用 Modbus 模擬器

您可以使用 Modbus 模擬器來測試插件:

1. **pymodbus.server** (Python)
   ```bash
   pip install pymodbus
   pymodbus.server --host 127.0.0.1 --port 502
   ```

2. **ModRSsim2** (Windows)
   - 下載: http://modrssim.sourceforge.net/

3. **diagslave** (Linux/Windows/Mac)
   - 下載: https://www.modbusdriver.com/diagslave.html

### 測試腳本

```python
import asyncio
from plugins.modbus_tcp_plugin.plugin import ModbusTCPPlugin

async def test_plugin():
    # 建立插件實例
    plugin = ModbusTCPPlugin({
        "host": "127.0.0.1",
        "port": 502,
        "unit_id": 1
    })
    
    # 載入和啟動插件
    await plugin.on_load()
    await plugin.on_start()
    
    # 寫入測試
    await plugin.write("holding:100", 123)
    
    # 讀取測試
    value = await plugin.read("holding:100")
    print(f"Read value: {value}")
    
    # 停止和卸載插件
    await plugin.on_stop()
    await plugin.on_unload()

# 執行測試
asyncio.run(test_plugin())
```

## 故障排除

### 連接失敗

如果無法連接到 Modbus 伺服器:

1. 檢查主機和埠號是否正確
2. 確認 Modbus 伺服器正在執行
3. 檢查防火牆設定
4. 增加逾時時間

### 讀取/寫入錯誤

如果讀取或寫入失敗:

1. 檢查位址格式是否正確
2. 確認位址範圍在設備支援的範圍內
3. 檢查單元 ID 是否正確
4. 查看日誌以獲取詳細錯誤資訊

### 效能問題

如果遇到效能問題:

1. 使用批次讀取/寫入減少請求次數
2. 調整輪詢間隔 (訂閱功能)
3. 考慮使用多個連接 (不同的插件實例)

## 授權

MIT License

## 作者

Your Name (your.email@example.com)

## 版本歷史

- **1.0.0** (2025-10-10)
  - 初始版本
  - 支援 Holding Registers, Coils, Input Registers, Discrete Inputs
  - 支援批次讀取/寫入
  - 支援資料訂閱

## 參考資料

- [Modbus 協定規範](https://modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf)
- [pymodbus 文件](https://pymodbus.readthedocs.io/)
- [NDH 文件](https://github.com/chchlin1018/NDH-Win-Test1)

