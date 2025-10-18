

# 虛擬化 MES 實施指南

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件提供了 V-MES (Virtual MES) 的詳細實施指南,包括技術選型、開發步驟和部署建議。

## 2. 技術選型

- **後端框架**: FastAPI (Python)
- **模擬引擎**: SimPy (Python)
- **前端框架**: React
- **數據庫**: SQLite
- **訊息佇列**: RabbitMQ
- **容器化**: Docker

## 3. 開發步驟

### 步驟 1: 數據模型定義 (Pydantic)

```python
from pydantic import BaseModel
from typing import List

class EquipmentConfig(BaseModel):
    equipmentId: str
    oee: float
    mtbf: int
    mttr: int

class SimulationConfig(BaseModel):
    scenarioName: str
    duration: int
    timeScale: int
    equipments: List[EquipmentConfig]
```

### 步驟 2: 模擬引擎實現 (SimPy)

```python
import simpy

class Equipment:
    def __init__(self, env, config):
        self.env = env
        self.config = config
        self.action = env.process(self.run())

    def run(self):
        while True:
            # 正常運行
            yield self.env.timeout(self.config.mtbf * self.config.timeScale)
            
            # 發生故障
            print(f"[{self.env.now}] {self.config.equipmentId} broke down.")
            yield self.env.timeout(self.config.mttr * self.config.timeScale)
            
            # 恢復運行
            print(f"[{self.env.now}] {self.config.equipmentId} recovered.")

def run_simulation(config: SimulationConfig):
    env = simpy.Environment()
    for eq_config in config.equipments:
        Equipment(env, eq_config)
    
    env.run(until=config.duration)
```

### 步驟 3: API 接口實現 (FastAPI)

```python
from fastapi import FastAPI

app = FastAPI()

@app.post("/api/v1/simulations")
def create_simulation(config: SimulationConfig):
    # 啟動一個新的模擬進程
    process = multiprocessing.Process(target=run_simulation, args=(config,))
    process.start()
    return {"message": "Simulation started.", "simulationId": process.pid}
```

### 步驟 4: 前端介面開發 (React)

- 使用 Ant Design 或 Material-UI 快速搭建 UI。
- 使用 Axios 與後端 API 進行交互。
- 使用 WebSocket 顯示即時的模擬日誌。

### 步驟 5: 容器化部署 (Docker)

```dockerfile
FROM python:3.9

WORKDIR /app

COPY requirements.txt .
RUN pip install -r requirements.txt

COPY . .

CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]
```

---

## 4. 預期結果

- 一個功能完整的 V-MES 系統,能夠模擬真實 MES 的核心功能。
- 一套標準化的 API 介面,可以與 NDH 無縫整合。
- 一個可配置的模擬環境,可以滿足不同的測試和演示需求。
- 一個輕量化的容器化部署方案,可以快速部署和使用。

---

