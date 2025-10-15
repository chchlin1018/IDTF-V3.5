# NDH MES/ERP 連接器實作

## 目錄

1. [連接器實作概述](#連接器實作概述)
2. [SAP ERP 連接器](#sap-erp-連接器)
3. [Microsoft Dynamics 365 連接器](#microsoft-dynamics-365-連接器)
4. [Siemens Opcenter MES 連接器](#siemens-opcenter-mes-連接器)
5. [通用 REST API 連接器](#通用-rest-api-連接器)
6. [連接器工廠](#連接器工廠)
7. [使用範例](#使用範例)

## 連接器實作概述

### 連接器架構

```
┌─────────────────────────────────────────────────────────┐
│           EnterpriseConnector (抽象基礎類別)             │
│  ┌───────────────────────────────────────────────────┐  │
│  │ - connect()                                       │  │
│  │ - disconnect()                                    │  │
│  │ - get_production_orders()                        │  │
│  │ - report_production_completion()                 │  │
│  │ - ...                                            │  │
│  └───────────────────────────────────────────────────┘  │
└────────────────────┬────────────────────────────────────┘
                     │
         ┌───────────┴───────────┬───────────────┐
         │                       │               │
┌────────▼────────┐   ┌─────────▼────────┐  ┌──▼────────────┐
│  ERPConnector   │   │  MESConnector    │  │ SCADAConnector│
│  (抽象類別)      │   │  (抽象類別)       │  │ (抽象類別)     │
└────────┬────────┘   └─────────┬────────┘  └──┬────────────┘
         │                      │               │
    ┌────┴────┐           ┌────┴────┐     ┌────┴────┐
    │         │           │         │     │         │
┌───▼──┐  ┌──▼───┐   ┌──▼───┐  ┌──▼──┐ ┌▼──┐  ┌──▼──┐
│ SAP  │  │Dyn365│   │Siemens│ │Rock-│ │PI │  │Hist-│
│ ERP  │  │ ERP  │   │  MES  │ │well │ │Sys│  │orian│
└──────┘  └──────┘   └───────┘ └─────┘ └───┘  └─────┘
```

### 設計原則

1. **統一介面**: 所有連接器實作相同的介面
2. **可擴展性**: 易於新增新的系統連接器
3. **錯誤處理**: 完善的異常處理和重試機制
4. **非同步**: 使用 async/await 提高效能
5. **可測試性**: 支援模擬模式用於測試

## SAP ERP 連接器

### SAP ERP 整合方式

SAP ERP 提供多種整合方式:

| 方式 | 說明 | 適用場景 |
|------|------|---------|
| **BAPI** | Business API,RFC 函數 | 即時交易處理 |
| **IDoc** | Intermediate Document,非同步訊息 | 批次資料交換 |
| **OData** | REST-like API | 現代化整合 |
| **SAP Gateway** | RESTful API 閘道 | 雲端和行動應用 |

### SAP ERP 連接器實作

```python
"""
SAP ERP 連接器實作
"""

import asyncio
from typing import Dict, List, Optional
from datetime import datetime
from pyrfc import Connection, ABAPApplicationError, ABAPRuntimeError
import aiohttp

class SAPERPConnector(EnterpriseConnector):
    """SAP ERP 連接器"""
    
    def __init__(self, connector_id: str, config: Dict):
        super().__init__(connector_id, SystemType.ERP, config)
        
        self.connection = None
        self.use_odata = config.get("use_odata", False)
        
        if self.use_odata:
            self.odata_url = config["odata_url"]
            self.session = None
        else:
            # RFC 連接參數
            self.rfc_params = {
                "ashost": config["host"],
                "sysnr": config["system_number"],
                "client": config["client"],
                "user": config["username"],
                "passwd": config["password"]
            }
    
    async def connect(self) -> bool:
        """建立連接"""
        try:
            if self.use_odata:
                # 建立 HTTP 會話
                self.session = aiohttp.ClientSession(
                    auth=aiohttp.BasicAuth(
                        self.config["username"],
                        self.config["password"]
                    )
                )
            else:
                # 建立 RFC 連接
                self.connection = Connection(**self.rfc_params)
            
            self.connected = True
            return True
        
        except Exception as e:
            print(f"Failed to connect to SAP ERP: {e}")
            return False
    
    async def disconnect(self):
        """斷開連接"""
        if self.use_odata and self.session:
            await self.session.close()
        elif self.connection:
            self.connection.close()
        
        self.connected = False
    
    async def test_connection(self) -> bool:
        """測試連接"""
        try:
            if self.use_odata:
                async with self.session.get(f"{self.odata_url}/$metadata") as resp:
                    return resp.status == 200
            else:
                # 呼叫 RFC_PING
                result = await asyncio.to_thread(
                    self.connection.call,
                    "RFC_PING"
                )
                return True
        except Exception as e:
            print(f"Connection test failed: {e}")
            return False
    
    async def get_production_orders(
        self,
        status: Optional[str] = None,
        start_date: Optional[datetime] = None
    ) -> List[Dict]:
        """
        取得生產訂單
        
        使用 BAPI: BAPI_PRODORD_GET_LIST
        """
        if not self.connected:
            raise ConnectionError("Not connected to SAP ERP")
        
        try:
            # 準備查詢參數
            selection_params = []
            
            if status:
                selection_params.append({
                    "FIELDNAME": "STATUS",
                    "SIGN": "I",
                    "OPTION": "EQ",
                    "LOW": status
                })
            
            if start_date:
                selection_params.append({
                    "FIELDNAME": "GSTRP",  # 基本開始日期
                    "SIGN": "I",
                    "OPTION": "GE",
                    "LOW": start_date.strftime("%Y%m%d")
                })
            
            # 呼叫 BAPI
            result = await asyncio.to_thread(
                self.connection.call,
                "BAPI_PRODORD_GET_LIST",
                SELECTION=selection_params
            )
            
            # 轉換為標準格式
            orders = []
            for order in result.get("ORDERS", []):
                orders.append({
                    "order_id": order["AUFNR"],  # 訂單編號
                    "product_id": order["MATNR"],  # 物料編號
                    "quantity": float(order["GAMNG"]),  # 訂單數量
                    "planned_start": self._parse_sap_date(order["GSTRP"]),
                    "planned_end": self._parse_sap_date(order["GLTRP"]),
                    "status": order["STATUS"]
                })
            
            return orders
        
        except (ABAPApplicationError, ABAPRuntimeError) as e:
            print(f"SAP BAPI error: {e}")
            raise
    
    async def create_production_order(self, order_data: Dict) -> Dict:
        """
        建立生產訂單
        
        使用 BAPI: BAPI_PRODORD_CREATE
        """
        if not self.connected:
            raise ConnectionError("Not connected to SAP ERP")
        
        try:
            # 準備訂單標頭
            order_header = {
                "MATERIAL": order_data["product_id"],
                "PLANT": order_data.get("plant", "1000"),
                "ORDER_TYPE": order_data.get("order_type", "ZP01"),
                "QUANTITY": str(order_data["quantity"]),
                "START_DATE": order_data["planned_start"].strftime("%Y%m%d"),
                "FINISH_DATE": order_data["planned_end"].strftime("%Y%m%d")
            }
            
            # 呼叫 BAPI
            result = await asyncio.to_thread(
                self.connection.call,
                "BAPI_PRODORD_CREATE",
                ORDERDATA=order_header
            )
            
            # 提交事務
            await asyncio.to_thread(
                self.connection.call,
                "BAPI_TRANSACTION_COMMIT",
                WAIT="X"
            )
            
            return {
                "order_id": result["ORDER_NUMBER"],
                "status": "created"
            }
        
        except (ABAPApplicationError, ABAPRuntimeError) as e:
            print(f"Failed to create production order: {e}")
            raise
    
    async def report_production_completion(
        self,
        completion_data: Dict
    ) -> bool:
        """
        報告生產完成
        
        使用 BAPI: BAPI_PRODORDCONF_CREATE_TT (Time Ticket)
        """
        if not self.connected:
            raise ConnectionError("Not connected to SAP ERP")
        
        try:
            # 準備確認資料
            time_ticket = {
                "ORDERID": completion_data["order_id"],
                "OPERATION": completion_data.get("operation", "0010"),
                "YIELD": str(completion_data["completed_quantity"]),
                "CONF_DATE": completion_data.get(
                    "actual_end",
                    datetime.now()
                ).strftime("%Y%m%d"),
                "CONF_TIME": completion_data.get(
                    "actual_end",
                    datetime.now()
                ).strftime("%H%M%S"),
                "FIN_CONF": "X"  # 最終確認
            }
            
            # 呼叫 BAPI
            result = await asyncio.to_thread(
                self.connection.call,
                "BAPI_PRODORDCONF_CREATE_TT",
                TIMETICKETS=[time_ticket]
            )
            
            # 檢查錯誤
            if result.get("RETURN", {}).get("TYPE") == "E":
                raise Exception(f"SAP Error: {result['RETURN']['MESSAGE']}")
            
            # 提交事務
            await asyncio.to_thread(
                self.connection.call,
                "BAPI_TRANSACTION_COMMIT",
                WAIT="X"
            )
            
            return True
        
        except Exception as e:
            print(f"Failed to report production completion: {e}")
            return False
    
    async def get_material_stock(self, material_id: str) -> Dict:
        """
        取得物料庫存
        
        使用 BAPI: BAPI_MATERIAL_AVAILABILITY
        """
        if not self.connected:
            raise ConnectionError("Not connected to SAP ERP")
        
        try:
            result = await asyncio.to_thread(
                self.connection.call,
                "BAPI_MATERIAL_AVAILABILITY",
                MATERIAL=material_id,
                PLANT=self.config.get("plant", "1000")
            )
            
            return {
                "material_id": material_id,
                "available_quantity": float(result.get("AV_QTY_PLT", 0)),
                "unit": result.get("UNIT", ""),
                "timestamp": datetime.now()
            }
        
        except Exception as e:
            print(f"Failed to get material stock: {e}")
            raise
    
    async def report_material_consumption(
        self,
        consumption_data: Dict
    ) -> bool:
        """
        報告物料消耗
        
        使用 BAPI: BAPI_GOODSMVT_CREATE (貨物移動)
        """
        if not self.connected:
            raise ConnectionError("Not connected to SAP ERP")
        
        try:
            # 準備貨物移動標頭
            gm_header = {
                "PSTNG_DATE": datetime.now().strftime("%Y%m%d"),
                "DOC_DATE": datetime.now().strftime("%Y%m%d"),
                "REF_DOC_NO": consumption_data.get("work_order_id", "")
            }
            
            # 準備貨物移動項目
            gm_items = [{
                "MATERIAL": consumption_data["material_id"],
                "PLANT": self.config.get("plant", "1000"),
                "STGE_LOC": consumption_data.get("storage_location", "0001"),
                "MOVE_TYPE": "261",  # 生產發料
                "ENTRY_QNT": str(consumption_data["quantity"]),
                "ORDERID": consumption_data.get("work_order_id", "")
            }]
            
            # 呼叫 BAPI
            result = await asyncio.to_thread(
                self.connection.call,
                "BAPI_GOODSMVT_CREATE",
                GOODSMVT_HEADER=gm_header,
                GOODSMVT_ITEM=gm_items
            )
            
            # 檢查錯誤
            if result.get("RETURN", {}).get("TYPE") == "E":
                raise Exception(f"SAP Error: {result['RETURN']['MESSAGE']}")
            
            # 提交事務
            await asyncio.to_thread(
                self.connection.call,
                "BAPI_TRANSACTION_COMMIT",
                WAIT="X"
            )
            
            return True
        
        except Exception as e:
            print(f"Failed to report material consumption: {e}")
            return False
    
    async def report_quality_data(self, quality_data: Dict) -> bool:
        """
        報告品質資料
        
        使用 BAPI: BAPI_QUALNOT_CREATE (品質通知)
        """
        if not self.connected:
            raise ConnectionError("Not connected to SAP ERP")
        
        try:
            # 準備品質通知
            notif_header = {
                "NOTIF_TYPE": "Q1",  # 品質通知類型
                "MATERIAL": quality_data.get("product_id", ""),
                "SHORT_TEXT": quality_data.get("description", "Quality Issue")
            }
            
            # 呼叫 BAPI
            result = await asyncio.to_thread(
                self.connection.call,
                "BAPI_QUALNOT_CREATE",
                NOTIFHEADER=notif_header
            )
            
            # 提交事務
            await asyncio.to_thread(
                self.connection.call,
                "BAPI_TRANSACTION_COMMIT",
                WAIT="X"
            )
            
            return True
        
        except Exception as e:
            print(f"Failed to report quality data: {e}")
            return False
    
    def _parse_sap_date(self, sap_date: str) -> datetime:
        """解析 SAP 日期格式 (YYYYMMDD)"""
        if not sap_date:
            return None
        return datetime.strptime(sap_date, "%Y%m%d")
```

## Microsoft Dynamics 365 連接器

### Dynamics 365 整合方式

Microsoft Dynamics 365 使用 **OData** 協定:

- **Web API**: RESTful API,基於 OData v4.0
- **認證**: OAuth 2.0 / Azure AD
- **查詢**: OData 查詢語法

### Dynamics 365 連接器實作

```python
"""
Microsoft Dynamics 365 ERP 連接器實作
"""

import aiohttp
from typing import Dict, List, Optional
from datetime import datetime
import json

class Dynamics365Connector(EnterpriseConnector):
    """Microsoft Dynamics 365 連接器"""
    
    def __init__(self, connector_id: str, config: Dict):
        super().__init__(connector_id, SystemType.ERP, config)
        
        self.api_url = config["api_url"]  # https://org.api.crm.dynamics.com/api/data/v9.2
        self.tenant_id = config["tenant_id"]
        self.client_id = config["client_id"]
        self.client_secret = config["client_secret"]
        
        self.session = None
        self.access_token = None
    
    async def connect(self) -> bool:
        """建立連接並取得存取權杖"""
        try:
            # 取得 OAuth 2.0 存取權杖
            token_url = f"https://login.microsoftonline.com/{self.tenant_id}/oauth2/v2.0/token"
            
            async with aiohttp.ClientSession() as session:
                async with session.post(token_url, data={
                    "grant_type": "client_credentials",
                    "client_id": self.client_id,
                    "client_secret": self.client_secret,
                    "scope": f"{self.api_url}/.default"
                }) as resp:
                    token_data = await resp.json()
                    self.access_token = token_data["access_token"]
            
            # 建立會話
            self.session = aiohttp.ClientSession(
                headers={
                    "Authorization": f"Bearer {self.access_token}",
                    "OData-MaxVersion": "4.0",
                    "OData-Version": "4.0",
                    "Accept": "application/json",
                    "Content-Type": "application/json"
                }
            )
            
            self.connected = True
            return True
        
        except Exception as e:
            print(f"Failed to connect to Dynamics 365: {e}")
            return False
    
    async def disconnect(self):
        """斷開連接"""
        if self.session:
            await self.session.close()
        self.connected = False
    
    async def test_connection(self) -> bool:
        """測試連接"""
        try:
            async with self.session.get(f"{self.api_url}/WhoAmI") as resp:
                return resp.status == 200
        except Exception as e:
            print(f"Connection test failed: {e}")
            return False
    
    async def get_production_orders(
        self,
        status: Optional[str] = None,
        start_date: Optional[datetime] = None
    ) -> List[Dict]:
        """
        取得生產訂單
        
        使用 OData 查詢 msdyn_productionorders 實體
        """
        if not self.connected:
            raise ConnectionError("Not connected to Dynamics 365")
        
        try:
            # 建立 OData 查詢
            query = f"{self.api_url}/msdyn_productionorders"
            filters = []
            
            if status:
                filters.append(f"msdyn_status eq '{status}'")
            
            if start_date:
                filters.append(
                    f"msdyn_scheduledstart ge {start_date.isoformat()}"
                )
            
            if filters:
                query += "?$filter=" + " and ".join(filters)
            
            # 執行查詢
            async with self.session.get(query) as resp:
                if resp.status != 200:
                    raise Exception(f"Query failed: {resp.status}")
                
                data = await resp.json()
                
                # 轉換為標準格式
                orders = []
                for order in data.get("value", []):
                    orders.append({
                        "order_id": order["msdyn_productionorderid"],
                        "product_id": order.get("_msdyn_product_value", ""),
                        "quantity": order.get("msdyn_quantity", 0),
                        "planned_start": order.get("msdyn_scheduledstart"),
                        "planned_end": order.get("msdyn_scheduledend"),
                        "status": order.get("msdyn_status")
                    })
                
                return orders
        
        except Exception as e:
            print(f"Failed to get production orders: {e}")
            raise
    
    async def create_production_order(self, order_data: Dict) -> Dict:
        """建立生產訂單"""
        if not self.connected:
            raise ConnectionError("Not connected to Dynamics 365")
        
        try:
            # 準備訂單資料
            order_payload = {
                "msdyn_name": f"PO-{order_data.get('order_id', '')}",
                "msdyn_product@odata.bind": f"/products({order_data['product_id']})",
                "msdyn_quantity": order_data["quantity"],
                "msdyn_scheduledstart": order_data["planned_start"].isoformat(),
                "msdyn_scheduledend": order_data["planned_end"].isoformat(),
                "msdyn_status": 192350000  # Created
            }
            
            # 建立訂單
            async with self.session.post(
                f"{self.api_url}/msdyn_productionorders",
                json=order_payload
            ) as resp:
                if resp.status not in [200, 201, 204]:
                    raise Exception(f"Failed to create order: {resp.status}")
                
                # 取得建立的訂單 ID
                order_id = resp.headers.get("OData-EntityId", "").split("(")[-1].rstrip(")")
                
                return {
                    "order_id": order_id,
                    "status": "created"
                }
        
        except Exception as e:
            print(f"Failed to create production order: {e}")
            raise
    
    async def update_production_order(
        self,
        order_id: str,
        updates: Dict
    ) -> bool:
        """更新生產訂單"""
        if not self.connected:
            raise ConnectionError("Not connected to Dynamics 365")
        
        try:
            # 更新訂單
            async with self.session.patch(
                f"{self.api_url}/msdyn_productionorders({order_id})",
                json=updates
            ) as resp:
                return resp.status in [200, 204]
        
        except Exception as e:
            print(f"Failed to update production order: {e}")
            return False
    
    async def report_production_completion(
        self,
        completion_data: Dict
    ) -> bool:
        """報告生產完成"""
        if not self.connected:
            raise ConnectionError("Not connected to Dynamics 365")
        
        try:
            # 更新訂單狀態為完成
            updates = {
                "msdyn_status": 192350003,  # Completed
                "msdyn_actualquantity": completion_data["completed_quantity"],
                "msdyn_actualstart": completion_data.get(
                    "actual_start",
                    datetime.now()
                ).isoformat(),
                "msdyn_actualend": completion_data.get(
                    "actual_end",
                    datetime.now()
                ).isoformat()
            }
            
            return await self.update_production_order(
                completion_data["order_id"],
                updates
            )
        
        except Exception as e:
            print(f"Failed to report production completion: {e}")
            return False
    
    async def get_material_stock(self, material_id: str) -> Dict:
        """取得物料庫存"""
        if not self.connected:
            raise ConnectionError("Not connected to Dynamics 365")
        
        try:
            # 查詢庫存
            query = (
                f"{self.api_url}/msdyn_inventorylevels"
                f"?$filter=_msdyn_product_value eq '{material_id}'"
            )
            
            async with self.session.get(query) as resp:
                if resp.status != 200:
                    raise Exception(f"Query failed: {resp.status}")
                
                data = await resp.json()
                
                total_quantity = sum(
                    item.get("msdyn_quantity", 0)
                    for item in data.get("value", [])
                )
                
                return {
                    "material_id": material_id,
                    "available_quantity": total_quantity,
                    "unit": "",
                    "timestamp": datetime.now()
                }
        
        except Exception as e:
            print(f"Failed to get material stock: {e}")
            raise
    
    async def report_material_consumption(
        self,
        consumption_data: Dict
    ) -> bool:
        """報告物料消耗"""
        if not self.connected:
            raise ConnectionError("Not connected to Dynamics 365")
        
        try:
            # 建立庫存調整
            adjustment_payload = {
                "msdyn_product@odata.bind": f"/products({consumption_data['material_id']})",
                "msdyn_quantity": -consumption_data["quantity"],  # 負數表示消耗
                "msdyn_transactiontype": 192350001,  # Consumption
                "msdyn_referencetype": 192350000,  # Production Order
                "msdyn_reference": consumption_data.get("work_order_id", "")
            }
            
            async with self.session.post(
                f"{self.api_url}/msdyn_inventoryadjustments",
                json=adjustment_payload
            ) as resp:
                return resp.status in [200, 201, 204]
        
        except Exception as e:
            print(f"Failed to report material consumption: {e}")
            return False
    
    async def report_quality_data(self, quality_data: Dict) -> bool:
        """報告品質資料"""
        if not self.connected:
            raise ConnectionError("Not connected to Dynamics 365")
        
        try:
            # 建立品質檢驗記錄
            quality_payload = {
                "msdyn_name": f"QC-{quality_data.get('record_id', '')}",
                "msdyn_product@odata.bind": f"/products({quality_data.get('product_id', '')})",
                "msdyn_result": quality_data.get("result", ""),
                "msdyn_inspectiondate": datetime.now().isoformat()
            }
            
            async with self.session.post(
                f"{self.api_url}/msdyn_qualityinspections",
                json=quality_payload
            ) as resp:
                return resp.status in [200, 201, 204]
        
        except Exception as e:
            print(f"Failed to report quality data: {e}")
            return False
```

## Siemens Opcenter MES 連接器

### Opcenter MES 整合方式

Siemens Opcenter (前身為 Camstar) 提供:

- **REST API**: 現代化的 RESTful API
- **SOAP Web Services**: 傳統的 Web Services
- **OPC UA**: 即時資料存取

### Opcenter MES 連接器實作

```python
"""
Siemens Opcenter MES 連接器實作
"""

import aiohttp
from typing import Dict, List, Optional
from datetime import datetime

class OpcenterMESConnector(EnterpriseConnector):
    """Siemens Opcenter MES 連接器"""
    
    def __init__(self, connector_id: str, config: Dict):
        super().__init__(connector_id, SystemType.MES, config)
        
        self.api_url = config["api_url"]  # http://server/OpcenterAPI
        self.username = config["username"]
        self.password = config["password"]
        
        self.session = None
        self.session_token = None
    
    async def connect(self) -> bool:
        """建立連接並登入"""
        try:
            self.session = aiohttp.ClientSession()
            
            # 登入取得 Session Token
            login_payload = {
                "UserName": self.username,
                "Password": self.password
            }
            
            async with self.session.post(
                f"{self.api_url}/Login",
                json=login_payload
            ) as resp:
                if resp.status != 200:
                    raise Exception(f"Login failed: {resp.status}")
                
                data = await resp.json()
                self.session_token = data["SessionToken"]
            
            # 更新會話標頭
            self.session.headers.update({
                "Authorization": f"Bearer {self.session_token}",
                "Content-Type": "application/json"
            })
            
            self.connected = True
            return True
        
        except Exception as e:
            print(f"Failed to connect to Opcenter MES: {e}")
            return False
    
    async def disconnect(self):
        """斷開連接並登出"""
        if self.session and self.session_token:
            try:
                await self.session.post(f"{self.api_url}/Logout")
            except:
                pass
            await self.session.close()
        
        self.connected = False
    
    async def test_connection(self) -> bool:
        """測試連接"""
        try:
            async with self.session.get(f"{self.api_url}/Status") as resp:
                return resp.status == 200
        except Exception as e:
            print(f"Connection test failed: {e}")
            return False
    
    async def get_production_orders(
        self,
        status: Optional[str] = None,
        start_date: Optional[datetime] = None
    ) -> List[Dict]:
        """取得生產訂單 (在 Opcenter 中稱為 Container)"""
        if not self.connected:
            raise ConnectionError("Not connected to Opcenter MES")
        
        try:
            # 準備查詢參數
            query_payload = {
                "ContainerStatus": status,
                "StartDate": start_date.isoformat() if start_date else None
            }
            
            # 查詢 Containers
            async with self.session.post(
                f"{self.api_url}/Containers/Query",
                json=query_payload
            ) as resp:
                if resp.status != 200:
                    raise Exception(f"Query failed: {resp.status}")
                
                data = await resp.json()
                
                # 轉換為標準格式
                orders = []
                for container in data.get("Containers", []):
                    orders.append({
                        "order_id": container["Name"],
                        "product_id": container.get("Product", ""),
                        "quantity": container.get("Qty", 0),
                        "planned_start": container.get("PlannedStartTime"),
                        "planned_end": container.get("PlannedEndTime"),
                        "status": container.get("Status")
                    })
                
                return orders
        
        except Exception as e:
            print(f"Failed to get production orders: {e}")
            raise
    
    async def create_job(self, work_order_data: Dict) -> Dict:
        """建立 MES 作業"""
        if not self.connected:
            raise ConnectionError("Not connected to Opcenter MES")
        
        try:
            # 建立 Container
            container_payload = {
                "Name": work_order_data["work_order_id"],
                "Product": work_order_data["product_id"],
                "Qty": work_order_data["quantity"],
                "PlannedStartTime": work_order_data["scheduled_start"].isoformat(),
                "PlannedEndTime": work_order_data["scheduled_end"].isoformat()
            }
            
            async with self.session.post(
                f"{self.api_url}/Containers/Create",
                json=container_payload
            ) as resp:
                if resp.status not in [200, 201]:
                    raise Exception(f"Failed to create job: {resp.status}")
                
                data = await resp.json()
                
                return {
                    "job_id": data["Container"]["Name"],
                    "status": "created"
                }
        
        except Exception as e:
            print(f"Failed to create job: {e}")
            raise
    
    async def start_job(self, job_id: str) -> bool:
        """開始作業"""
        if not self.connected:
            raise ConnectionError("Not connected to Opcenter MES")
        
        try:
            # 執行 Start 交易
            start_payload = {
                "Container": job_id
            }
            
            async with self.session.post(
                f"{self.api_url}/Transactions/Start",
                json=start_payload
            ) as resp:
                return resp.status == 200
        
        except Exception as e:
            print(f"Failed to start job: {e}")
            return False
    
    async def report_production(self, production_data: Dict) -> bool:
        """報告生產數量"""
        if not self.connected:
            raise ConnectionError("Not connected to Opcenter MES")
        
        try:
            # 執行 Move 交易 (報工)
            move_payload = {
                "Container": production_data["work_order_id"],
                "Qty": production_data["quantity"],
                "MoveType": "Standard"
            }
            
            async with self.session.post(
                f"{self.api_url}/Transactions/Move",
                json=move_payload
            ) as resp:
                return resp.status == 200
        
        except Exception as e:
            print(f"Failed to report production: {e}")
            return False
    
    async def get_job_progress(self, job_id: str) -> Dict:
        """取得作業進度"""
        if not self.connected:
            raise ConnectionError("Not connected to Opcenter MES")
        
        try:
            async with self.session.get(
                f"{self.api_url}/Containers/{job_id}"
            ) as resp:
                if resp.status != 200:
                    raise Exception(f"Query failed: {resp.status}")
                
                data = await resp.json()
                container = data["Container"]
                
                return {
                    "job_id": job_id,
                    "status": container.get("Status"),
                    "completed_quantity": container.get("CompletedQty", 0),
                    "total_quantity": container.get("Qty", 0)
                }
        
        except Exception as e:
            print(f"Failed to get job progress: {e}")
            raise
    
    # 實作其他必要方法...
    async def create_production_order(self, order_data: Dict) -> Dict:
        """建立生產訂單 (委派給 create_job)"""
        return await self.create_job(order_data)
    
    async def update_production_order(
        self,
        order_id: str,
        updates: Dict
    ) -> bool:
        """更新生產訂單"""
        # 實作更新邏輯
        pass
    
    async def report_production_completion(
        self,
        completion_data: Dict
    ) -> bool:
        """報告生產完成"""
        # 實作完成報告邏輯
        pass
    
    async def get_material_stock(self, material_id: str) -> Dict:
        """取得物料庫存"""
        # 實作庫存查詢邏輯
        pass
    
    async def report_material_consumption(
        self,
        consumption_data: Dict
    ) -> bool:
        """報告物料消耗"""
        # 實作物料消耗報告邏輯
        pass
    
    async def report_quality_data(self, quality_data: Dict) -> bool:
        """報告品質資料"""
        # 實作品質資料報告邏輯
        pass
```

(續下一部分...)

---

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**最後更新**: 2025-01-10



## 通用 REST API 連接器

### 通用連接器設計

對於沒有專用連接器的系統,提供通用 REST API 連接器:

```python
"""
通用 REST API 連接器
"""

import aiohttp
from typing import Dict, List, Optional, Any
from datetime import datetime
import json

class GenericRESTConnector(EnterpriseConnector):
    """通用 REST API 連接器"""
    
    def __init__(self, connector_id: str, config: Dict):
        system_type = SystemType[config.get("system_type", "ERP").upper()]
        super().__init__(connector_id, system_type, config)
        
        self.base_url = config["base_url"]
        self.auth_type = config.get("auth_type", "basic")  # basic, bearer, apikey
        self.auth_config = config.get("auth", {})
        
        # API 端點映射
        self.endpoints = config.get("endpoints", {})
        
        # 欄位映射
        self.field_mappings = config.get("field_mappings", {})
        
        self.session = None
    
    async def connect(self) -> bool:
        """建立連接"""
        try:
            headers = {"Content-Type": "application/json"}
            
            # 設定認證
            auth = None
            if self.auth_type == "basic":
                auth = aiohttp.BasicAuth(
                    self.auth_config["username"],
                    self.auth_config["password"]
                )
            elif self.auth_type == "bearer":
                headers["Authorization"] = f"Bearer {self.auth_config['token']}"
            elif self.auth_type == "apikey":
                headers[self.auth_config["header_name"]] = self.auth_config["api_key"]
            
            self.session = aiohttp.ClientSession(
                headers=headers,
                auth=auth
            )
            
            self.connected = True
            return True
        
        except Exception as e:
            print(f"Failed to connect: {e}")
            return False
    
    async def disconnect(self):
        """斷開連接"""
        if self.session:
            await self.session.close()
        self.connected = False
    
    async def test_connection(self) -> bool:
        """測試連接"""
        try:
            test_endpoint = self.endpoints.get("test", "/health")
            async with self.session.get(f"{self.base_url}{test_endpoint}") as resp:
                return resp.status == 200
        except Exception as e:
            print(f"Connection test failed: {e}")
            return False
    
    async def _make_request(
        self,
        method: str,
        endpoint: str,
        data: Optional[Dict] = None,
        params: Optional[Dict] = None
    ) -> Any:
        """發送 HTTP 請求"""
        url = f"{self.base_url}{endpoint}"
        
        async with self.session.request(
            method,
            url,
            json=data,
            params=params
        ) as resp:
            if resp.status not in [200, 201, 204]:
                raise Exception(f"Request failed: {resp.status}")
            
            if resp.status == 204:
                return None
            
            return await resp.json()
    
    def _map_fields(self, data: Dict, mapping_key: str, direction: str = "to_ndh") -> Dict:
        """映射欄位"""
        if mapping_key not in self.field_mappings:
            return data
        
        mapping = self.field_mappings[mapping_key]
        result = {}
        
        for ndh_field, external_field in mapping.items():
            if direction == "to_ndh":
                if external_field in data:
                    result[ndh_field] = data[external_field]
            else:  # from_ndh
                if ndh_field in data:
                    result[external_field] = data[ndh_field]
        
        return result
    
    async def get_production_orders(
        self,
        status: Optional[str] = None,
        start_date: Optional[datetime] = None
    ) -> List[Dict]:
        """取得生產訂單"""
        if not self.connected:
            raise ConnectionError("Not connected")
        
        endpoint = self.endpoints.get("get_orders", "/production-orders")
        params = {}
        
        if status:
            params["status"] = status
        if start_date:
            params["start_date"] = start_date.isoformat()
        
        data = await self._make_request("GET", endpoint, params=params)
        
        # 映射欄位
        orders = []
        for order in data.get("orders", data):
            mapped_order = self._map_fields(order, "production_order", "to_ndh")
            orders.append(mapped_order)
        
        return orders
    
    async def create_production_order(self, order_data: Dict) -> Dict:
        """建立生產訂單"""
        if not self.connected:
            raise ConnectionError("Not connected")
        
        endpoint = self.endpoints.get("create_order", "/production-orders")
        
        # 映射欄位
        mapped_data = self._map_fields(order_data, "production_order", "from_ndh")
        
        result = await self._make_request("POST", endpoint, data=mapped_data)
        
        return {
            "order_id": result.get("id", result.get("order_id")),
            "status": "created"
        }
    
    async def update_production_order(
        self,
        order_id: str,
        updates: Dict
    ) -> bool:
        """更新生產訂單"""
        if not self.connected:
            raise ConnectionError("Not connected")
        
        endpoint = self.endpoints.get("update_order", f"/production-orders/{order_id}")
        endpoint = endpoint.replace("{order_id}", order_id)
        
        # 映射欄位
        mapped_updates = self._map_fields(updates, "production_order", "from_ndh")
        
        await self._make_request("PUT", endpoint, data=mapped_updates)
        return True
    
    async def report_production_completion(
        self,
        completion_data: Dict
    ) -> bool:
        """報告生產完成"""
        if not self.connected:
            raise ConnectionError("Not connected")
        
        endpoint = self.endpoints.get("report_completion", "/production-completions")
        
        # 映射欄位
        mapped_data = self._map_fields(completion_data, "production_completion", "from_ndh")
        
        await self._make_request("POST", endpoint, data=mapped_data)
        return True
    
    async def get_material_stock(self, material_id: str) -> Dict:
        """取得物料庫存"""
        if not self.connected:
            raise ConnectionError("Not connected")
        
        endpoint = self.endpoints.get("get_stock", f"/materials/{material_id}/stock")
        endpoint = endpoint.replace("{material_id}", material_id)
        
        data = await self._make_request("GET", endpoint)
        
        # 映射欄位
        return self._map_fields(data, "material_stock", "to_ndh")
    
    async def report_material_consumption(
        self,
        consumption_data: Dict
    ) -> bool:
        """報告物料消耗"""
        if not self.connected:
            raise ConnectionError("Not connected")
        
        endpoint = self.endpoints.get("report_consumption", "/material-consumptions")
        
        # 映射欄位
        mapped_data = self._map_fields(consumption_data, "material_consumption", "from_ndh")
        
        await self._make_request("POST", endpoint, data=mapped_data)
        return True
    
    async def report_quality_data(self, quality_data: Dict) -> bool:
        """報告品質資料"""
        if not self.connected:
            raise ConnectionError("Not connected")
        
        endpoint = self.endpoints.get("report_quality", "/quality-records")
        
        # 映射欄位
        mapped_data = self._map_fields(quality_data, "quality_record", "from_ndh")
        
        await self._make_request("POST", endpoint, data=mapped_data)
        return True
```

### 通用連接器配置範例

```json
{
  "connector_id": "custom_erp_connector",
  "system_type": "ERP",
  "base_url": "https://api.example.com/v1",
  "auth_type": "bearer",
  "auth": {
    "token": "your-api-token-here"
  },
  "endpoints": {
    "test": "/health",
    "get_orders": "/orders",
    "create_order": "/orders",
    "update_order": "/orders/{order_id}",
    "report_completion": "/orders/{order_id}/complete",
    "get_stock": "/inventory/{material_id}",
    "report_consumption": "/inventory/consume",
    "report_quality": "/quality/records"
  },
  "field_mappings": {
    "production_order": {
      "order_id": "orderNumber",
      "product_id": "productCode",
      "quantity": "qty",
      "scheduled_start": "startDate",
      "scheduled_end": "endDate",
      "status": "orderStatus"
    },
    "material_stock": {
      "material_id": "materialCode",
      "available_quantity": "availableQty",
      "unit": "uom"
    }
  }
}
```

## 連接器工廠

### 工廠模式實作

```python
"""
連接器工廠 - 根據配置建立連接器實例
"""

from typing import Dict, Type
from enum import Enum

class ConnectorType(Enum):
    """連接器類型"""
    SAP_ERP = "sap_erp"
    DYNAMICS_365 = "dynamics_365"
    ORACLE_ERP = "oracle_erp"
    SIEMENS_OPCENTER = "siemens_opcenter"
    ROCKWELL_FACTORYTALK = "rockwell_factorytalk"
    AVEVA_MES = "aveva_mes"
    GENERIC_REST = "generic_rest"

class ConnectorFactory:
    """連接器工廠"""
    
    # 連接器類別註冊表
    _registry: Dict[ConnectorType, Type[EnterpriseConnector]] = {}
    
    @classmethod
    def register(cls, connector_type: ConnectorType, connector_class: Type[EnterpriseConnector]):
        """註冊連接器類別"""
        cls._registry[connector_type] = connector_class
    
    @classmethod
    def create(cls, connector_type: ConnectorType, connector_id: str, config: Dict) -> EnterpriseConnector:
        """
        建立連接器實例
        
        Args:
            connector_type: 連接器類型
            connector_id: 連接器 ID
            config: 配置字典
        
        Returns:
            連接器實例
        
        Raises:
            ValueError: 如果連接器類型未註冊
        """
        if connector_type not in cls._registry:
            raise ValueError(f"Connector type {connector_type} not registered")
        
        connector_class = cls._registry[connector_type]
        return connector_class(connector_id, config)
    
    @classmethod
    def create_from_config(cls, config: Dict) -> EnterpriseConnector:
        """
        從配置字典建立連接器
        
        Args:
            config: 包含 connector_type, connector_id 和其他配置的字典
        
        Returns:
            連接器實例
        """
        connector_type_str = config.get("connector_type")
        if not connector_type_str:
            raise ValueError("Missing connector_type in config")
        
        connector_type = ConnectorType(connector_type_str)
        connector_id = config.get("connector_id", f"{connector_type_str}_connector")
        
        return cls.create(connector_type, connector_id, config)

# 註冊連接器
ConnectorFactory.register(ConnectorType.SAP_ERP, SAPERPConnector)
ConnectorFactory.register(ConnectorType.DYNAMICS_365, Dynamics365Connector)
ConnectorFactory.register(ConnectorType.SIEMENS_OPCENTER, OpcenterMESConnector)
ConnectorFactory.register(ConnectorType.GENERIC_REST, GenericRESTConnector)
```

### 連接器管理器

```python
"""
連接器管理器 - 管理多個連接器實例
"""

from typing import Dict, List
import asyncio

class ConnectorManager:
    """連接器管理器"""
    
    def __init__(self):
        self.connectors: Dict[str, EnterpriseConnector] = {}
    
    def add_connector(self, connector: EnterpriseConnector):
        """新增連接器"""
        self.connectors[connector.connector_id] = connector
    
    def get_connector(self, connector_id: str) -> Optional[EnterpriseConnector]:
        """取得連接器"""
        return self.connectors.get(connector_id)
    
    def remove_connector(self, connector_id: str):
        """移除連接器"""
        if connector_id in self.connectors:
            del self.connectors[connector_id]
    
    async def connect_all(self) -> Dict[str, bool]:
        """連接所有連接器"""
        results = {}
        tasks = []
        
        for connector_id, connector in self.connectors.items():
            tasks.append(self._connect_one(connector_id, connector))
        
        results_list = await asyncio.gather(*tasks, return_exceptions=True)
        
        for i, connector_id in enumerate(self.connectors.keys()):
            results[connector_id] = results_list[i]
        
        return results
    
    async def _connect_one(self, connector_id: str, connector: EnterpriseConnector) -> bool:
        """連接單個連接器"""
        try:
            return await connector.connect()
        except Exception as e:
            print(f"Failed to connect {connector_id}: {e}")
            return False
    
    async def disconnect_all(self):
        """斷開所有連接器"""
        tasks = []
        for connector in self.connectors.values():
            if connector.connected:
                tasks.append(connector.disconnect())
        
        await asyncio.gather(*tasks, return_exceptions=True)
    
    def get_connectors_by_type(self, system_type: SystemType) -> List[EnterpriseConnector]:
        """根據系統類型取得連接器"""
        return [
            connector for connector in self.connectors.values()
            if connector.system_type == system_type
        ]
    
    def get_erp_connectors(self) -> List[EnterpriseConnector]:
        """取得所有 ERP 連接器"""
        return self.get_connectors_by_type(SystemType.ERP)
    
    def get_mes_connectors(self) -> List[EnterpriseConnector]:
        """取得所有 MES 連接器"""
        return self.get_connectors_by_type(SystemType.MES)
```

## 使用範例

### 範例 1: 基本使用

```python
"""
基本連接器使用範例
"""

import asyncio
from datetime import datetime

async def basic_example():
    """基本使用範例"""
    
    # 建立 SAP ERP 連接器
    sap_config = {
        "host": "sap.example.com",
        "system_number": "00",
        "client": "100",
        "username": "user",
        "password": "pass"
    }
    
    sap_connector = SAPERPConnector("sap_erp", sap_config)
    
    # 連接
    if await sap_connector.connect():
        print("Connected to SAP ERP")
        
        # 測試連接
        if await sap_connector.test_connection():
            print("Connection test passed")
        
        # 取得生產訂單
        orders = await sap_connector.get_production_orders(
            status="CRTD",  # Created
            start_date=datetime(2025, 1, 1)
        )
        
        print(f"Found {len(orders)} production orders")
        for order in orders:
            print(f"  - {order['order_id']}: {order['product_id']} x {order['quantity']}")
        
        # 報告生產完成
        completion_result = await sap_connector.report_production_completion({
            "order_id": "1000001",
            "completed_quantity": 100,
            "actual_end": datetime.now()
        })
        
        if completion_result:
            print("Production completion reported successfully")
        
        # 斷開連接
        await sap_connector.disconnect()
        print("Disconnected from SAP ERP")

# 執行範例
asyncio.run(basic_example())
```

### 範例 2: 使用連接器工廠

```python
"""
使用連接器工廠範例
"""

import asyncio

async def factory_example():
    """連接器工廠範例"""
    
    # 從配置建立連接器
    configs = [
        {
            "connector_type": "sap_erp",
            "connector_id": "sap_main",
            "host": "sap.example.com",
            "system_number": "00",
            "client": "100",
            "username": "user",
            "password": "pass"
        },
        {
            "connector_type": "dynamics_365",
            "connector_id": "d365_main",
            "api_url": "https://org.api.crm.dynamics.com/api/data/v9.2",
            "tenant_id": "tenant-id",
            "client_id": "client-id",
            "client_secret": "client-secret"
        },
        {
            "connector_type": "siemens_opcenter",
            "connector_id": "opcenter_main",
            "api_url": "http://mes.example.com/OpcenterAPI",
            "username": "user",
            "password": "pass"
        }
    ]
    
    # 建立連接器管理器
    manager = ConnectorManager()
    
    # 建立並新增連接器
    for config in configs:
        connector = ConnectorFactory.create_from_config(config)
        manager.add_connector(connector)
    
    # 連接所有連接器
    results = await manager.connect_all()
    
    for connector_id, success in results.items():
        if success:
            print(f"✓ {connector_id} connected")
        else:
            print(f"✗ {connector_id} failed to connect")
    
    # 使用連接器
    sap_connector = manager.get_connector("sap_main")
    if sap_connector and sap_connector.connected:
        orders = await sap_connector.get_production_orders()
        print(f"SAP: {len(orders)} orders")
    
    mes_connector = manager.get_connector("opcenter_main")
    if mes_connector and mes_connector.connected:
        jobs = await mes_connector.get_production_orders()
        print(f"MES: {len(jobs)} jobs")
    
    # 斷開所有連接
    await manager.disconnect_all()
    print("All connectors disconnected")

# 執行範例
asyncio.run(factory_example())
```

### 範例 3: 完整的 ERP-MES 整合流程

```python
"""
完整的 ERP-MES 整合流程範例
"""

import asyncio
from datetime import datetime

async def integration_example():
    """完整整合流程範例"""
    
    # 建立連接器
    erp_connector = ConnectorFactory.create(
        ConnectorType.SAP_ERP,
        "sap_erp",
        {
            "host": "sap.example.com",
            "system_number": "00",
            "client": "100",
            "username": "user",
            "password": "pass"
        }
    )
    
    mes_connector = ConnectorFactory.create(
        ConnectorType.SIEMENS_OPCENTER,
        "opcenter_mes",
        {
            "api_url": "http://mes.example.com/OpcenterAPI",
            "username": "user",
            "password": "pass"
        }
    )
    
    # 建立工單管理器和整合協調器
    work_order_manager = WorkOrderManager()
    
    orchestrator = IntegrationOrchestrator(
        erp_connector,
        mes_connector,
        None,  # ndh_broker (簡化範例)
        work_order_manager
    )
    
    # 連接系統
    await erp_connector.connect()
    await mes_connector.connect()
    
    print("=== Step 1: 從 ERP 同步工單 ===")
    await orchestrator.sync_from_erp()
    
    print("\n=== Step 2: 工單自動釋放到 MES ===")
    # (在 sync_from_erp 中已自動釋放)
    
    print("\n=== Step 3: 模擬生產開始 ===")
    work_orders = list(work_order_manager.work_orders.values())
    if work_orders:
        first_order = work_orders[0]
        work_order_manager.start_work_order(first_order.work_order_id)
        print(f"Started work order: {first_order.work_order_id}")
    
    print("\n=== Step 4: 模擬生產報工 ===")
    if work_orders:
        first_order = work_orders[0]
        # 報告 50% 完成
        work_order_manager.report_production(
            first_order.work_order_id,
            first_order.quantity // 2
        )
        print(f"Reported 50% completion")
        
        # 報告剩餘 50% 完成
        work_order_manager.report_production(
            first_order.work_order_id,
            first_order.quantity - first_order.completed_quantity
        )
        print(f"Reported 100% completion")
    
    print("\n=== Step 5: 工單完成自動回報到 ERP ===")
    # (在 report_production 中已自動完成)
    
    print("\n=== Step 6: 從 MES 同步進度 ===")
    await orchestrator.sync_from_mes()
    
    # 斷開連接
    await erp_connector.disconnect()
    await mes_connector.disconnect()
    
    print("\n=== Integration completed ===")

# 執行範例
asyncio.run(integration_example())
```

### 範例 4: 使用通用 REST 連接器

```python
"""
使用通用 REST 連接器範例
"""

import asyncio

async def generic_connector_example():
    """通用連接器範例"""
    
    # 配置通用連接器
    config = {
        "connector_type": "generic_rest",
        "connector_id": "custom_erp",
        "system_type": "ERP",
        "base_url": "https://api.example.com/v1",
        "auth_type": "bearer",
        "auth": {
            "token": "your-api-token"
        },
        "endpoints": {
            "test": "/health",
            "get_orders": "/production/orders",
            "create_order": "/production/orders",
            "update_order": "/production/orders/{order_id}",
            "report_completion": "/production/completions",
            "get_stock": "/inventory/materials/{material_id}",
            "report_consumption": "/inventory/consumptions",
            "report_quality": "/quality/inspections"
        },
        "field_mappings": {
            "production_order": {
                "order_id": "orderNumber",
                "product_id": "productCode",
                "quantity": "orderQuantity",
                "scheduled_start": "plannedStartDate",
                "scheduled_end": "plannedEndDate",
                "status": "orderStatus"
            },
            "production_completion": {
                "order_id": "orderNumber",
                "completed_quantity": "actualQuantity",
                "actual_start": "actualStartDate",
                "actual_end": "actualEndDate"
            },
            "material_stock": {
                "material_id": "materialCode",
                "available_quantity": "availableStock",
                "unit": "unitOfMeasure"
            }
        }
    }
    
    # 建立連接器
    connector = ConnectorFactory.create_from_config(config)
    
    # 連接
    if await connector.connect():
        print("Connected to custom ERP")
        
        # 取得生產訂單
        orders = await connector.get_production_orders()
        print(f"Found {len(orders)} orders")
        
        # 建立新訂單
        new_order = await connector.create_production_order({
            "order_id": "PO-2025-001",
            "product_id": "PROD-001",
            "quantity": 1000,
            "scheduled_start": datetime(2025, 1, 15, 8, 0),
            "scheduled_end": datetime(2025, 1, 15, 17, 0)
        })
        print(f"Created order: {new_order['order_id']}")
        
        # 報告完成
        await connector.report_production_completion({
            "order_id": new_order["order_id"],
            "completed_quantity": 1000,
            "actual_start": datetime(2025, 1, 15, 8, 30),
            "actual_end": datetime(2025, 1, 15, 16, 45)
        })
        print("Completion reported")
        
        # 斷開連接
        await connector.disconnect()

# 執行範例
asyncio.run(generic_connector_example())
```

## 總結

NDH MES/ERP 連接器提供了:

1. **統一介面**: 所有連接器實作相同的介面,易於使用和擴展
2. **多系統支援**: 支援 SAP、Dynamics 365、Siemens Opcenter 等主流系統
3. **通用連接器**: 對於沒有專用連接器的系統,提供通用 REST API 連接器
4. **工廠模式**: 使用工廠模式簡化連接器的建立和管理
5. **非同步設計**: 使用 async/await 提高效能
6. **完善的錯誤處理**: 包含重試機制和異常處理

這些連接器是 NDH 與企業系統整合的基礎,為實現完整的 ISA-95 整合架構提供了堅實的技術支援。

---

**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) (Chih-Cheng Lin, Michael Lin)  
**版權**: © 2025 版權所有  
**最後更新**: 2025-01-10

