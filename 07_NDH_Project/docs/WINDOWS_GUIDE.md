# NDH Test Framework - Windows 安裝和使用指南

## 目錄

1. [系統需求](#系統需求)
2. [安裝步驟](#安裝步驟)
3. [啟動 NDH 服務](#啟動-ndh-服務)
4. [執行範例](#執行範例)
5. [Redis 設定(可選)](#redis-設定可選)
6. [常見問題](#常見問題)
7. [進階配置](#進階配置)

## 系統需求

### 必要條件

- **作業系統**: Windows 10 或 Windows 11 (64-bit)
- **Python**: 3.11 或更高版本
- **記憶體**: 至少 4GB RAM
- **硬碟空間**: 至少 1GB 可用空間

### 可選條件

- **Redis**: 用於訊息佇列功能(可選)
- **WSL2**: 用於執行 Redis(推薦)

## 安裝步驟

### 步驟 1: 安裝 Python

1. 前往 [Python 官方網站](https://www.python.org/downloads/)
2. 下載 Python 3.11 或更高版本的 Windows 安裝程式
3. 執行安裝程式
4. **重要**: 勾選 "Add Python to PATH" 選項
5. 點擊 "Install Now"

驗證安裝:
```powershell
python --version
```

應該顯示類似 `Python 3.11.x` 的版本資訊。

### 步驟 2: 下載 NDH Test Framework

將 NDH Test Framework 專案解壓縮到您想要的位置,例如:
```
C:\Projects\ndh-test-framework\
```

### 步驟 3: 執行自動設定腳本

開啟 PowerShell (以系統管理員身分執行),導航到專案目錄:

```powershell
cd C:\Projects\ndh-test-framework
```

執行設定腳本:
```powershell
.\scripts\setup_windows.ps1
```

如果遇到執行政策錯誤,請先執行:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### 步驟 4: 手動安裝(替代方案)

如果自動腳本無法執行,可以手動安裝:

```powershell
# 建立虛擬環境
python -m venv venv

# 啟動虛擬環境
.\venv\Scripts\Activate.ps1

# 升級 pip
python -m pip install --upgrade pip

# 安裝依賴
pip install -r requirements.txt
```

## 啟動 NDH 服務

### 方法 1: 使用 PowerShell

```powershell
# 確保虛擬環境已啟動
.\venv\Scripts\Activate.ps1

# 啟動 NDH 服務
python -m ndh.main
```

### 方法 2: 使用命令提示字元 (CMD)

```cmd
REM 啟動虛擬環境
venv\Scripts\activate.bat

REM 啟動 NDH 服務
python -m ndh.main
```

### 驗證服務運行

服務啟動後,您應該看到類似以下的輸出:

```
============================================================
Starting NDH (Neutral Data Hub)
============================================================
Host: 0.0.0.0
Port: 8000
Environment: development
Redis: Disabled
============================================================
INFO:     Started server process [12345]
INFO:     Waiting for application startup.
INFO:     Application startup complete.
INFO:     Uvicorn running on http://0.0.0.0:8000
```

開啟瀏覽器,訪問以下網址驗證:

- **API 文件**: http://localhost:8000/docs
- **健康檢查**: http://localhost:8000/health

## 執行範例

### 基本範例: 資料推送和拉取

開啟**新的** PowerShell 視窗(保持 NDH 服務運行):

```powershell
# 啟動虛擬環境
cd C:\Projects\ndh-test-framework
.\venv\Scripts\Activate.ps1

# 執行範例
python examples\basic\simple_push_pull.py
```

您應該看到:

```
============================================================
NDH Simple Push/Pull Example
============================================================

============================================================
Checking NDH service health...
============================================================
✓ NDH service is healthy!
  Status: healthy
  Version: 1.0.0
  Environment: development

============================================================
Pushing data to NDH...
============================================================
✓ Data pushed successfully!
  Data ID: PUMP_001_2025-01-09T10:00:00
  Status: success
```

### 使用 API 文件進行測試

1. 開啟瀏覽器,訪問 http://localhost:8000/docs
2. 展開 `/api/v1/data/push` 端點
3. 點擊 "Try it out"
4. 修改請求主體:
   ```json
   {
     "asset_id": "MOTOR_001",
     "timestamp": "2025-01-09T10:00:00Z",
     "values": {
       "rpm": 1500,
       "temperature": 65.5
     },
     "quality": "good"
   }
   ```
5. 點擊 "Execute"
6. 查看回應

## Redis 設定(可選)

Redis 提供更強大的訊息佇列功能,但不是必需的。

### 方法 1: 使用 WSL2 (推薦)

1. 安裝 WSL2:
   ```powershell
   wsl --install
   ```

2. 重新啟動電腦

3. 開啟 WSL2 終端機:
   ```powershell
   wsl
   ```

4. 在 WSL2 中安裝 Redis:
   ```bash
   sudo apt-get update
   sudo apt-get install redis-server
   ```

5. 啟動 Redis:
   ```bash
   redis-server
   ```

6. 在另一個 WSL2 終端機中測試:
   ```bash
   redis-cli ping
   ```
   應該返回 `PONG`

### 方法 2: 使用 Memurai (Redis for Windows)

1. 前往 [Memurai 官網](https://www.memurai.com/)
2. 下載 Memurai 安裝程式
3. 執行安裝程式
4. Memurai 會作為 Windows 服務自動啟動

### 啟用 Redis 支援

建立環境變數檔案 `.env`:

```powershell
# 在專案根目錄建立 .env 檔案
notepad .env
```

新增以下內容:

```
NDH_REDIS_ENABLED=true
NDH_REDIS_HOST=localhost
NDH_REDIS_PORT=6379
```

儲存並關閉。重新啟動 NDH 服務。

## 常見問題

### Q1: 執行腳本時出現 "無法載入檔案" 錯誤

**A**: 這是 PowerShell 執行政策問題。執行:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

### Q2: 找不到 Python 命令

**A**: Python 未加入 PATH。重新安裝 Python 並確保勾選 "Add Python to PATH"。

或手動新增 PATH:
1. 開啟 "系統內容" > "進階" > "環境變數"
2. 在 "使用者變數" 中找到 `Path`
3. 新增 Python 安裝路徑,例如 `C:\Users\YourName\AppData\Local\Programs\Python\Python311`

### Q3: 無法連接到 NDH 服務

**A**: 確保:
1. NDH 服務正在運行
2. 沒有防火牆阻擋 port 8000
3. 使用正確的 URL: `http://localhost:8000`

### Q4: Redis 連接失敗

**A**: 
1. 確認 Redis 服務正在運行
2. 檢查 `.env` 檔案中的 Redis 設定
3. 如果不需要 Redis,將 `NDH_REDIS_ENABLED` 設為 `false`

### Q5: 如何停止 NDH 服務?

**A**: 在運行 NDH 的 PowerShell 視窗中按 `Ctrl+C`

## 進階配置

### 修改服務埠

建立或編輯 `.env` 檔案:

```
NDH_PORT=9000
```

### 啟用生產模式

```
NDH_ENVIRONMENT=production
```

### 調整佇列大小

```
NDH_QUEUE_MAX_SIZE=50000
```

### 完整配置範例

`.env` 檔案範例:

```
# Server Configuration
NDH_HOST=0.0.0.0
NDH_PORT=8000
NDH_ENVIRONMENT=development

# Redis Configuration
NDH_REDIS_ENABLED=true
NDH_REDIS_HOST=localhost
NDH_REDIS_PORT=6379
NDH_REDIS_DB=0

# Queue Configuration
NDH_QUEUE_MAX_SIZE=10000
NDH_QUEUE_TIMEOUT=30

# API Configuration
NDH_API_RATE_LIMIT=100
NDH_API_TIMEOUT=30

# Logging
NDH_LOG_LEVEL=INFO
```

## 以 Windows 服務方式運行

使用 NSSM (Non-Sucking Service Manager):

1. 下載 NSSM: https://nssm.cc/download
2. 解壓縮到 `C:\nssm`
3. 以系統管理員身分開啟 PowerShell:

```powershell
cd C:\nssm\win64

# 安裝服務
.\nssm.exe install NDH "C:\Projects\ndh-test-framework\venv\Scripts\python.exe" "-m ndh.main"

# 設定工作目錄
.\nssm.exe set NDH AppDirectory "C:\Projects\ndh-test-framework"

# 啟動服務
.\nssm.exe start NDH

# 查看服務狀態
.\nssm.exe status NDH
```

管理服務:

```powershell
# 停止服務
.\nssm.exe stop NDH

# 重新啟動服務
.\nssm.exe restart NDH

# 移除服務
.\nssm.exe remove NDH confirm
```

## 下一步

- 閱讀 [API 文件](http://localhost:8000/docs)
- 查看更多範例: `examples/` 目錄
- 學習如何開發自訂連接器: `docs/CONNECTOR_DEVELOPMENT.md`
- 整合 IADL Editor: `examples/iadl_integration/`

## 技術支援

如有問題,請聯絡:
- Email: chchlin1018@gmail.com
- GitHub: https://github.com/chchlin1018/ndh-test-framework

---

**作者**: 林志錚 (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**版本**: 1.0.0  
**最後更新**: 2025-01-09

