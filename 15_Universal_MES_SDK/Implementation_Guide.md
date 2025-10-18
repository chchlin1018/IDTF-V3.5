

# 通用 MES SDK 實施指南

**作者**: Manus AI  
**日期**: 2025年10月14日  
**版本**: 1.0

---

## 1. 簡介

本文件提供了通用 MES SDK 的詳細實施指南,包括如何開發一個新的 MES 適配器,以及如何將 SDK 整合到 NDH 中。

## 2. 開發一個新的 MES 適配器

以 Siemens Opcenter 為例,開發一個新的 MES 適配器需要以下步驟:

### 步驟 1: 創建適配器類

```python
# adapters/siemens.py

import requests
from typing import List, Dict
from .base import IMESAdapter

class SiemensOpcenterAdapter(IMESAdapter):
    def __init__(self, params):
        self.api_url = params["api_url"]
        self.api_key = params["api_key"]

    def get_work_orders(self, plant: str, work_center: str) -> List[Dict]:
        response = requests.get(
            f"{self.api_url}/work-orders",
            params={"plant": plant, "work_center": work_center},
            headers={"Authorization": f"Bearer {self.api_key}"}
        )
        response.raise_for_status()
        return self._transform_work_orders(response.json())

    def confirm_production(self, confirmation: Dict) -> bool:
        response = requests.post(
            f"{self.api_url}/confirmations",
            json=self._transform_confirmation(confirmation),
            headers={"Authorization": f"Bearer {self.api_key}"}
        )
        return response.status_code == 201

    def _transform_work_orders(self, data: List[Dict]) -> List[Dict]:
        # 將 Siemens 的數據格式轉換為標準格式
        transformed = []
        for item in data:
            transformed.append({
                "workOrderId": item["orderId"],
                "productId": item["materialId"],
                "quantity": item["orderQuantity"],
                "status": item["orderStatus"]
            })
        return transformed

    def _transform_confirmation(self, data: Dict) -> Dict:
        # 將標準格式轉換為 Siemens 的數據格式
        return {
            "orderId": data["workOrderId"],
            "confirmedQuantity": data["confirmedQuantity"],
            "scrapQuantity": data["scrapQuantity"]
        }
```

### 步驟 2: 更新配置文件

在 `config.yaml` 中新增 Siemens 適配器的配置:

```yaml
mes:
  adapters:
    - name: siemens
      class: adapters.siemens.SiemensOpcenterAdapter
      params:
        api_url: "https://siemens-mes.company.com/api"
        api_key: "secret_key"
```

### 步驟 3: 使用適配器

```python
from universal_mes_sdk import AdapterManager

config = load_config("config.yaml")
manager = AdapterManager(config["mes"])

siemens_adapter = manager.get_adapter("siemens")
work_orders = siemens_adapter.get_work_orders(plant="P1", work_center="WC1")
```

---

## 3. 整合到 NDH

### 步驟 1: 在 NDH 中引入 SDK

在 NDH 的 `requirements.txt` 中新增 `universal-mes-sdk`。

### 步驟 2: 在 NDH API 中使用 SDK

```python
# ndh/api/mes_routes.py

from fastapi import APIRouter, Depends
from universal_mes_sdk import AdapterManager

router = APIRouter()

# 假設 manager 已經透過依賴注入初始化
@router.get("/work-orders")
def get_work_orders(plant: str, work_center: str, manager: AdapterManager = Depends()):
    adapter = manager.get_adapter() # 獲取預設適配器
    return adapter.get_work_orders(plant, work_center)
```

---

## 4. 預期結果

- **快速整合**: 開發一個新的 MES 適配器只需要幾天時間,而不是幾周。
- **代碼重用**: 所有與 MES 相關的業務邏輯都可以在 NDH 中重用。
- **統一監控**: 可以透過通用 MES SDK 監控所有 MES 系統的健康狀況。
- **靈活切換**: 可以輕鬆地在不同的 MES 系統之間切換,而無需修改 NDH 的核心代碼。

---

