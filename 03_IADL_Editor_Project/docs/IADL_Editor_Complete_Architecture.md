# IADL Editor (Asset Designer) 完整架構設計

**版本**: 1.0  
**日期**: 2025-10-15  
**作者**: 林志錚 Michael Lin (Chih Cheng Lin)

---

## 📘 術語表

- **IADL**: Industrial Asset Definition Language（工業資產定義語言）
- **NDH**: Neutral Data Hub（中立數據中樞）
- **CAD**: Computer-Aided Design（電腦輔助設計）
- **PLM**: Product Lifecycle Management（產品生命週期管理）
- **PLC**: Programmable Logic Controller（可程式邏輯控制器）
- **SCADA**: Supervisory Control and Data Acquisition（監控與資料擷取系統）
- **MES**: Manufacturing Execution System（製造執行系統）
- **ERP**: Enterprise Resource Planning（企業資源規劃）
- **BOM**: Bill of Materials（物料清單）
- **CMMS**: Computerized Maintenance Management System（電腦化維護管理系統）

---

## 1. 概述

**IADL Editor (Asset Designer)** 是 IDTF V3.5 的核心工具，負責在資產的整個生命週期中進行定義、設計、更新和維護。本文件描述了 IADL Editor 在兩個關鍵時期的完整架構：

1. **設計時期 (Creation/Design Time)**: 資產的初始設計和定義階段
2. **運營時期 (Runtime/Maintain/Update Time)**: 工廠運營中的資產維護和更新階段

---

## 2. 整體架構

### 2.1 架構概覽

```
┌─────────────────────────────────────────────────────────────────┐
│                    IADL Editor (Asset Designer)                  │
│                                                                   │
│  ┌─────────────────────┐          ┌──────────────────────────┐  │
│  │  Design Time Mode   │          │  Runtime/Maintain Mode   │  │
│  │                     │          │                          │  │
│  │  - CAD/PLM 整合     │          │  - Factory Asset Library │  │
│  │  - AVEVA Dev Studio │          │  - 即時資料檢視          │  │
│  │  - 供應商資訊管理   │          │  - 維護記錄管理          │  │
│  │  - 文件管理         │          │  - 版本控制              │  │
│  └─────────────────────┘          └──────────────────────────┘  │
│                                                                   │
└───────────────────────┬───────────────────────────────────────────┘
                        │
          ┌─────────────┴─────────────┐
          │                           │
    ┌─────▼──────┐            ┌──────▼────────┐
    │    NDH     │            │  External     │
    │  Runtime   │            │  Systems      │
    │            │            │               │
    │ - Document │            │ - CAD/PLM     │
    │   Server   │            │ - AVEVA       │
    │ - Factory  │            │ - ERP         │
    │   Asset    │            │ - MES         │
    │   Library  │            │               │
    └────────────┘            └───────────────┘
```

---

## 3. 設計時期 (Creation/Design Time)

### 3.1 與 CAD/PLM 系統整合

#### 3.1.1 支援的 CAD 系統

| CAD 系統 | 整合方式 | 支援功能 |
|---------|---------|---------|
| **Autodesk Inventor** | COM API | 3D 模型匯入、BOM 提取、參數同步 |
| **SolidWorks** | SolidWorks API | 3D 模型匯入、配置變體、裝配體結構 |
| **CATIA** | CAA API | 3D 模型匯入、產品結構、PMI 數據 |
| **Siemens NX** | NX Open API | 3D 模型匯入、裝配體、製造資訊 |

#### 3.1.2 支援的 PLM 系統

| PLM 系統 | 整合方式 | 支援功能 |
|---------|---------|---------|
| **Siemens Teamcenter** | REST API + SOA | 零件資訊、BOM、變更管理、文件管理 |
| **PTC Windchill** | REST API | 零件資訊、BOM、生命週期狀態 |
| **Dassault ENOVIA** | REST API | 產品結構、文件、變更單 |
| **SAP PLM** | OData API | 物料主數據、BOM、ECN/ECO |

#### 3.1.3 CAD/PLM 整合工作流

```
1. 在 CAD 中設計資產 3D 模型
   ↓
2. 在 PLM 中管理零件和 BOM
   ↓
3. IADL Editor 從 PLM 讀取資產資訊
   ↓
4. 自動生成 IADL 資產定義
   - 3D 幾何（從 CAD 匯入 USD）
   - 零件清單（從 PLM 讀取 BOM）
   - 技術規格（從 PLM 讀取屬性）
   ↓
5. 工程師在 IADL Editor 中補充
   - 數據標籤（sensors, actuators）
   - 行為腳本（behaviors）
   - 控制邏輯（PLC/SCADA）
   ↓
6. 保存到 Factory Asset Library
```

#### 3.1.4 實作範例：從 Teamcenter 讀取資產

```python
# teamcenter_connector.py
import requests
from typing import Dict, List

class TeamcenterConnector:
    """Teamcenter PLM 連接器"""
    
    def __init__(self, base_url: str, username: str, password: str):
        self.base_url = base_url
        self.session = requests.Session()
        self._login(username, password)
    
    def _login(self, username: str, password: str):
        """登入 Teamcenter"""
        login_url = f"{self.base_url}/tc/rest/v1/login"
        response = self.session.post(login_url, json={
            "username": username,
            "password": password
        })
        response.raise_for_status()
    
    def get_item(self, item_id: str) -> Dict:
        """獲取零件資訊"""
        url = f"{self.base_url}/tc/rest/v1/items/{item_id}"
        response = self.session.get(url)
        response.raise_for_status()
        return response.json()
    
    def get_bom(self, item_id: str) -> List[Dict]:
        """獲取 BOM"""
        url = f"{self.base_url}/tc/rest/v1/items/{item_id}/bom"
        response = self.session.get(url)
        response.raise_for_status()
        return response.json()["children"]
    
    def get_documents(self, item_id: str) -> List[Dict]:
        """獲取相關文件"""
        url = f"{self.base_url}/tc/rest/v1/items/{item_id}/documents"
        response = self.session.get(url)
        response.raise_for_status()
        return response.json()["documents"]
    
    def export_to_iadl(self, item_id: str) -> Dict:
        """匯出為 IADL 格式"""
        item = self.get_item(item_id)
        bom = self.get_bom(item_id)
        documents = self.get_documents(item_id)
        
        iadl = {
            "asset_type": {
                "name": item["item_id"],
                "display_name": item["object_name"],
                "description": item.get("object_desc", ""),
                "category": item.get("item_type", "equipment"),
                "manufacturer": item.get("manufacturer", ""),
                "model": item.get("model_number", ""),
                "version": item.get("revision_id", "A"),
                
                # 3D 模型（從 CAD 匯入）
                "geometry": {
                    "usd_file": f"assets/{item['item_id']}.usd",
                    "thumbnail": f"assets/{item['item_id']}_thumb.png"
                },
                
                # BOM
                "components": [
                    {
                        "id": child["item_id"],
                        "name": child["object_name"],
                        "quantity": child.get("quantity", 1),
                        "unit": child.get("unit", "EA")
                    }
                    for child in bom
                ],
                
                # 文件連結
                "documents": [
                    {
                        "type": doc["document_type"],
                        "name": doc["object_name"],
                        "url": f"ndh://documents/{doc['uid']}"
                    }
                    for doc in documents
                ],
                
                # 供應商資訊（從 ERP 讀取，稍後補充）
                "vendor_info": {
                    "supplier_id": "",
                    "manufacturer_id": "",
                    "installer_id": "",
                    "maintainer_id": ""
                },
                
                # 參數（需要工程師手動補充）
                "parameters": [],
                
                # 數據標籤（需要工程師手動補充）
                "data_tags": [],
                
                # 行為（需要工程師手動補充）
                "behaviors": []
            }
        }
        
        return iadl
```

---

### 3.2 與 AVEVA Dev Studio 整合

#### 3.2.1 AVEVA Dev Studio 概述

**AVEVA Dev Studio** 是 AVEVA 的工業自動化開發環境，支援：
- PLC 程式設計（IEC 61131-3）
- SCADA 組態設計
- HMI 畫面設計
- 控制邏輯模擬

#### 3.2.2 整合目標

IADL Editor 與 AVEVA Dev Studio 整合，實現：

1. **自動生成 PLC 程式**：根據 IADL 定義的資產行為，自動生成 PLC 控制邏輯
2. **自動生成 SCADA 組態**：根據 IADL 定義的數據標籤，自動生成 SCADA 標籤和畫面
3. **雙向同步**：AVEVA Dev Studio 中的變更可以同步回 IADL

#### 3.2.3 整合工作流

```
1. 在 IADL Editor 中定義資產
   - 數據標籤（sensors, actuators）
   - 行為（behaviors）
   ↓
2. IADL Editor 自動生成 AVEVA Dev Studio 專案
   - PLC 程式（Structured Text, Ladder Diagram）
   - SCADA 標籤（OPC UA, Modbus）
   - HMI 畫面（圖形化介面）
   ↓
3. 工程師在 AVEVA Dev Studio 中調整和測試
   ↓
4. 同步回 IADL Editor
   - 更新行為腳本
   - 更新數據標籤映射
   ↓
5. 保存到 Factory Asset Library
```

#### 3.2.4 實作範例：生成 PLC 程式

```python
# aveva_connector.py
from typing import Dict, List

class AVEVADevStudioConnector:
    """AVEVA Dev Studio 連接器"""
    
    def __init__(self, project_path: str):
        self.project_path = project_path
    
    def generate_plc_program(self, asset: Dict) -> str:
        """根據 IADL 資產定義生成 PLC 程式（Structured Text）"""
        
        # 提取數據標籤
        sensors = [tag for tag in asset.get("data_tags", []) if tag["type"] == "sensor"]
        actuators = [tag for tag in asset.get("data_tags", []) if tag["type"] == "actuator"]
        
        # 提取行為
        behaviors = asset.get("behaviors", [])
        
        # 生成 Structured Text 程式
        st_code = f"""
PROGRAM {asset['name']}_Control
VAR
    (* Sensors *)
"""
        
        for sensor in sensors:
            st_code += f"    {sensor['name']}: {self._get_plc_type(sensor['data_type'])};\n"
        
        st_code += "\n    (* Actuators *)\n"
        for actuator in actuators:
            st_code += f"    {actuator['name']}: {self._get_plc_type(actuator['data_type'])};\n"
        
        st_code += "\n    (* Internal Variables *)\n"
        st_code += "    state: INT := 0;\n"
        st_code += "    error: BOOL := FALSE;\n"
        st_code += "END_VAR\n\n"
        
        # 生成控制邏輯
        for behavior in behaviors:
            if behavior["name"] == "start":
                st_code += self._generate_start_logic(behavior, actuators)
            elif behavior["name"] == "stop":
                st_code += self._generate_stop_logic(behavior, actuators)
            elif behavior["name"] == "emergency_stop":
                st_code += self._generate_emergency_stop_logic(behavior, actuators)
        
        st_code += "\nEND_PROGRAM\n"
        
        return st_code
    
    def _get_plc_type(self, data_type: str) -> str:
        """將 IADL 數據類型映射到 PLC 數據類型"""
        type_mapping = {
            "float": "REAL",
            "int": "INT",
            "bool": "BOOL",
            "string": "STRING"
        }
        return type_mapping.get(data_type, "REAL")
    
    def _generate_start_logic(self, behavior: Dict, actuators: List[Dict]) -> str:
        """生成啟動邏輯"""
        logic = "(* Start Behavior *)\n"
        logic += "IF state = 0 THEN\n"
        
        for actuator in actuators:
            if "motor" in actuator["name"].lower():
                logic += f"    {actuator['name']} := TRUE;\n"
        
        logic += "    state := 1;\n"
        logic += "END_IF;\n\n"
        
        return logic
    
    def _generate_stop_logic(self, behavior: Dict, actuators: List[Dict]) -> str:
        """生成停止邏輯"""
        logic = "(* Stop Behavior *)\n"
        logic += "IF state = 1 THEN\n"
        
        for actuator in actuators:
            if "motor" in actuator["name"].lower():
                logic += f"    {actuator['name']} := FALSE;\n"
        
        logic += "    state := 0;\n"
        logic += "END_IF;\n\n"
        
        return logic
    
    def _generate_emergency_stop_logic(self, behavior: Dict, actuators: List[Dict]) -> str:
        """生成緊急停止邏輯"""
        logic = "(* Emergency Stop Behavior *)\n"
        logic += "IF error = TRUE THEN\n"
        
        for actuator in actuators:
            logic += f"    {actuator['name']} := FALSE;\n"
        
        logic += "    state := -1;\n"
        logic += "END_IF;\n\n"
        
        return logic
    
    def generate_scada_tags(self, asset: Dict) -> List[Dict]:
        """生成 SCADA 標籤配置"""
        tags = []
        
        for data_tag in asset.get("data_tags", []):
            tag = {
                "name": f"{asset['name']}.{data_tag['name']}",
                "description": data_tag.get("description", ""),
                "data_type": data_tag["data_type"],
                "access": "ReadWrite" if data_tag["type"] == "actuator" else "ReadOnly",
                "address": data_tag.get("address", ""),
                "protocol": data_tag.get("protocol", "OPC UA"),
                "scan_rate": data_tag.get("scan_rate_ms", 1000)
            }
            tags.append(tag)
        
        return tags
```

---

### 3.3 資產供應商資訊管理

#### 3.3.1 供應商資訊結構

每個資產應該包含完整的供應商生命週期資訊：

```yaml
asset_type:
  name: centrifugal_pump_001
  
  # 供應商資訊
  vendor_info:
    # 製造商
    manufacturer:
      id: "MFG-001"
      name: "Grundfos"
      contact: "sales@grundfos.com"
      phone: "+45-8750-1400"
      address: "Poul Due Jensens Vej 7, 8850 Bjerringbro, Denmark"
      erp_link: "erp://vendors/MFG-001"
    
    # 供應商
    supplier:
      id: "SUP-001"
      name: "Industrial Equipment Supply Co."
      contact: "orders@ies.com"
      phone: "+886-2-1234-5678"
      erp_link: "erp://vendors/SUP-001"
    
    # 安裝商
    installer:
      id: "INS-001"
      name: "Taiwan Installation Services"
      contact: "service@tis.com.tw"
      phone: "+886-4-2345-6789"
      erp_link: "erp://vendors/INS-001"
    
    # 維修商
    maintainer:
      id: "MNT-001"
      name: "Pump Maintenance Taiwan"
      contact: "support@pmt.com.tw"
      phone: "+886-3-3456-7890"
      erp_link: "erp://vendors/MNT-001"
  
  # 採購資訊
  procurement_info:
    purchase_order: "PO-2024-001234"
    purchase_date: "2024-01-15"
    delivery_date: "2024-03-20"
    warranty_start: "2024-04-01"
    warranty_end: "2026-03-31"
    warranty_terms: "2 years parts and labor"
    unit_cost: 125000.00
    currency: "TWD"
    erp_link: "erp://purchase_orders/PO-2024-001234"
  
  # 安裝資訊
  installation_info:
    installation_date: "2024-04-01"
    installation_company: "Taiwan Installation Services"
    installation_engineer: "王大明"
    installation_report: "ndh://documents/install-report-001.pdf"
    commissioning_date: "2024-04-05"
    commissioning_report: "ndh://documents/commissioning-report-001.pdf"
  
  # 維護資訊
  maintenance_info:
    maintenance_schedule: "quarterly"
    next_maintenance_date: "2025-01-15"
    maintenance_contract: "MC-2024-001"
    maintenance_company: "Pump Maintenance Taiwan"
    maintenance_cost_annual: 15000.00
    currency: "TWD"
    cmms_link: "cmms://assets/centrifugal_pump_001"
  
  # 文件連結
  documents:
    - type: "manual"
      name: "Operation Manual"
      url: "ndh://documents/pump-001-manual.pdf"
    - type: "datasheet"
      name: "Technical Datasheet"
      url: "ndh://documents/pump-001-datasheet.pdf"
    - type: "drawing"
      name: "Installation Drawing"
      url: "ndh://documents/pump-001-drawing.pdf"
    - type: "certificate"
      name: "CE Certificate"
      url: "ndh://documents/pump-001-ce-cert.pdf"
    - type: "warranty"
      name: "Warranty Certificate"
      url: "ndh://documents/pump-001-warranty.pdf"
```

#### 3.3.2 與 ERP 系統整合

IADL Editor 應該能夠從 ERP 系統讀取和更新供應商資訊：

```python
# erp_connector.py
import requests
from typing import Dict, List

class ERPConnector:
    """ERP 系統連接器（以 SAP S/4HANA 為例）"""
    
    def __init__(self, base_url: str, api_key: str):
        self.base_url = base_url
        self.headers = {
            "APIKey": api_key,
            "Content-Type": "application/json"
        }
    
    def get_vendor(self, vendor_id: str) -> Dict:
        """獲取供應商資訊"""
        url = f"{self.base_url}/sap/opu/odata/sap/API_BUSINESS_PARTNER/A_BusinessPartner('{vendor_id}')"
        response = requests.get(url, headers=self.headers)
        response.raise_for_status()
        
        data = response.json()["d"]
        return {
            "id": data["BusinessPartner"],
            "name": data["BusinessPartnerFullName"],
            "contact": data.get("EmailAddress", ""),
            "phone": data.get("PhoneNumber", ""),
            "address": f"{data.get('StreetName', '')}, {data.get('CityName', '')}, {data.get('Country', '')}"
        }
    
    def get_purchase_order(self, po_number: str) -> Dict:
        """獲取採購訂單資訊"""
        url = f"{self.base_url}/sap/opu/odata/sap/API_PURCHASEORDER_PROCESS_SRV/A_PurchaseOrder('{po_number}')"
        response = requests.get(url, headers=self.headers)
        response.raise_for_status()
        
        data = response.json()["d"]
        return {
            "purchase_order": data["PurchaseOrder"],
            "purchase_date": data["PurchaseOrderDate"],
            "delivery_date": data.get("DeliveryDate", ""),
            "vendor_id": data["Supplier"],
            "total_amount": float(data["TotalNetAmount"]),
            "currency": data["DocumentCurrency"]
        }
    
    def get_material(self, material_number: str) -> Dict:
        """獲取物料主數據"""
        url = f"{self.base_url}/sap/opu/odata/sap/API_MATERIAL_STOCK_SRV/A_Material('{material_number}')"
        response = requests.get(url, headers=self.headers)
        response.raise_for_status()
        
        data = response.json()["d"]
        return {
            "material_number": data["Material"],
            "description": data["MaterialDescription"],
            "manufacturer": data.get("Manufacturer", ""),
            "model": data.get("ManufacturerPartNmbr", ""),
            "unit_of_measure": data["BaseUnit"]
        }
    
    def enrich_asset_with_erp_data(self, asset: Dict) -> Dict:
        """用 ERP 數據豐富資產定義"""
        
        # 獲取供應商資訊
        if "vendor_info" in asset and "manufacturer" in asset["vendor_info"]:
            mfg_id = asset["vendor_info"]["manufacturer"].get("id")
            if mfg_id:
                mfg_data = self.get_vendor(mfg_id)
                asset["vendor_info"]["manufacturer"].update(mfg_data)
        
        # 獲取採購資訊
        if "procurement_info" in asset:
            po_number = asset["procurement_info"].get("purchase_order")
            if po_number:
                po_data = self.get_purchase_order(po_number)
                asset["procurement_info"].update(po_data)
        
        return asset
```

---

### 3.4 NDH Document Server 整合

#### 3.4.1 Document Server 架構

NDH Document Server 是一個集中式的文件管理系統，用於存儲所有與資產相關的文件：

```
NDH Document Server
├── documents/
│   ├── manuals/
│   │   ├── pump-001-manual.pdf
│   │   └── motor-002-manual.pdf
│   ├── datasheets/
│   │   ├── pump-001-datasheet.pdf
│   │   └── motor-002-datasheet.pdf
│   ├── drawings/
│   │   ├── pump-001-drawing.pdf
│   │   └── motor-002-drawing.pdf
│   ├── certificates/
│   │   ├── pump-001-ce-cert.pdf
│   │   └── motor-002-ul-cert.pdf
│   ├── installation/
│   │   ├── install-report-001.pdf
│   │   └── commissioning-report-001.pdf
│   └── maintenance/
│       ├── maintenance-log-001.pdf
│       └── inspection-report-001.pdf
└── metadata.db
```

#### 3.4.2 Document Server API

```python
# document_server_connector.py
import requests
from typing import Dict, List, BinaryIO

class DocumentServerConnector:
    """NDH Document Server 連接器"""
    
    def __init__(self, base_url: str, api_key: str):
        self.base_url = base_url
        self.headers = {"Authorization": f"Bearer {api_key}"}
    
    def upload_document(self, file: BinaryIO, metadata: Dict) -> str:
        """上傳文件到 Document Server"""
        url = f"{self.base_url}/api/v1/documents"
        
        files = {"file": file}
        data = {
            "asset_id": metadata["asset_id"],
            "document_type": metadata["document_type"],
            "name": metadata["name"],
            "description": metadata.get("description", ""),
            "tags": ",".join(metadata.get("tags", []))
        }
        
        response = requests.post(url, headers=self.headers, files=files, data=data)
        response.raise_for_status()
        
        document_id = response.json()["document_id"]
        return f"ndh://documents/{document_id}"
    
    def get_document(self, document_url: str) -> bytes:
        """下載文件"""
        document_id = document_url.split("/")[-1]
        url = f"{self.base_url}/api/v1/documents/{document_id}"
        
        response = requests.get(url, headers=self.headers)
        response.raise_for_status()
        
        return response.content
    
    def list_documents(self, asset_id: str) -> List[Dict]:
        """列出資產的所有文件"""
        url = f"{self.base_url}/api/v1/documents"
        params = {"asset_id": asset_id}
        
        response = requests.get(url, headers=self.headers, params=params)
        response.raise_for_status()
        
        return response.json()["documents"]
    
    def delete_document(self, document_url: str):
        """刪除文件"""
        document_id = document_url.split("/")[-1]
        url = f"{self.base_url}/api/v1/documents/{document_id}"
        
        response = requests.delete(url, headers=self.headers)
        response.raise_for_status()
```

#### 3.4.3 IADL Editor 中的文件管理

在 IADL Editor 中，工程師可以：

1. **上傳文件**：拖放文件到 IADL Editor，自動上傳到 Document Server
2. **瀏覽文件**：在 IADL Editor 中瀏覽資產的所有文件
3. **下載文件**：從 Document Server 下載文件到本地
4. **刪除文件**：從 Document Server 刪除過時的文件
5. **版本控制**：Document Server 自動保存文件的所有版本

---

### 3.5 與 MES 系統整合

#### 3.5.1 MES 整合目標

IADL Editor 應該能夠從 MES 系統讀取生產相關資訊，並將其關聯到資產定義：

- **製程參數**：溫度、壓力、速度等
- **品質標準**：公差、檢驗項目、合格標準
- **生產配方**：原料配比、製程步驟
- **工單資訊**：生產計畫、實際產量、良率

#### 3.5.2 MES 整合工作流

```
1. 在 IADL Editor 中定義資產
   ↓
2. 連接到 MES 系統
   ↓
3. 從 MES 讀取製程參數和品質標準
   ↓
4. 自動填充到 IADL 資產定義
   - parameters（製程參數）
   - quality_standards（品質標準）
   - recipes（生產配方）
   ↓
5. 保存到 Factory Asset Library
   ↓
6. NDH Runtime 使用這些資訊進行生產監控
```

#### 3.5.3 實作範例：從 MES 讀取製程參數

```python
# mes_connector.py
import requests
from typing import Dict, List

class MESConnector:
    """MES 系統連接器（以 Siemens Opcenter 為例）"""
    
    def __init__(self, base_url: str, api_key: str):
        self.base_url = base_url
        self.headers = {
            "Authorization": f"Bearer {api_key}",
            "Content-Type": "application/json"
        }
    
    def get_equipment_parameters(self, equipment_id: str) -> List[Dict]:
        """獲取設備的製程參數"""
        url = f"{self.base_url}/api/v1/equipment/{equipment_id}/parameters"
        response = requests.get(url, headers=self.headers)
        response.raise_for_status()
        
        parameters = []
        for param in response.json()["parameters"]:
            parameters.append({
                "name": param["parameter_name"],
                "display_name": param["display_name"],
                "data_type": param["data_type"],
                "unit": param["unit"],
                "min_value": param.get("min_value"),
                "max_value": param.get("max_value"),
                "default_value": param.get("default_value"),
                "description": param.get("description", "")
            })
        
        return parameters
    
    def get_quality_standards(self, product_id: str) -> List[Dict]:
        """獲取產品的品質標準"""
        url = f"{self.base_url}/api/v1/products/{product_id}/quality_standards"
        response = requests.get(url, headers=self.headers)
        response.raise_for_status()
        
        standards = []
        for std in response.json()["standards"]:
            standards.append({
                "name": std["standard_name"],
                "inspection_item": std["inspection_item"],
                "specification": std["specification"],
                "tolerance": std["tolerance"],
                "measurement_method": std["measurement_method"]
            })
        
        return standards
    
    def get_recipe(self, recipe_id: str) -> Dict:
        """獲取生產配方"""
        url = f"{self.base_url}/api/v1/recipes/{recipe_id}"
        response = requests.get(url, headers=self.headers)
        response.raise_for_status()
        
        recipe_data = response.json()
        return {
            "recipe_id": recipe_data["recipe_id"],
            "recipe_name": recipe_data["recipe_name"],
            "version": recipe_data["version"],
            "materials": [
                {
                    "material_id": mat["material_id"],
                    "material_name": mat["material_name"],
                    "quantity": mat["quantity"],
                    "unit": mat["unit"]
                }
                for mat in recipe_data["materials"]
            ],
            "steps": [
                {
                    "step_number": step["step_number"],
                    "step_name": step["step_name"],
                    "equipment_id": step["equipment_id"],
                    "duration": step["duration"],
                    "parameters": step["parameters"]
                }
                for step in recipe_data["steps"]
            ]
        }
    
    def enrich_asset_with_mes_data(self, asset: Dict, equipment_id: str) -> Dict:
        """用 MES 數據豐富資產定義"""
        
        # 獲取製程參數
        parameters = self.get_equipment_parameters(equipment_id)
        if "parameters" not in asset:
            asset["parameters"] = []
        asset["parameters"].extend(parameters)
        
        # 獲取品質標準（如果資產有關聯的產品）
        if "product_id" in asset:
            standards = self.get_quality_standards(asset["product_id"])
            asset["quality_standards"] = standards
        
        return asset
```

---

## 4. 運營時期 (Runtime/Maintain/Update Time)

### 4.1 Factory Asset Library

#### 4.1.1 概述

**Factory Asset Library** 是一個集中式的資產庫，存儲在 NDH Runtime 中，包含：

- **資產定義 (IADL)**：所有資產的定義和元數據
- **資產實例 (FDL)**：工廠中實際部署的資產實例
- **即時數據**：資產的當前狀態和歷史數據
- **維護記錄**：維護歷史、故障記錄、備件更換
- **文件**：操作手冊、維護指南、檢驗報告

#### 4.1.2 Factory Asset Library 架構

```
Factory Asset Library (存儲在 NDH Runtime)
├── asset_definitions/
│   ├── centrifugal_pump.iadl.yaml
│   ├── electric_motor.iadl.yaml
│   └── heat_exchanger.iadl.yaml
├── asset_instances/
│   ├── pump-001.instance.yaml
│   ├── motor-002.instance.yaml
│   └── hx-003.instance.yaml
├── asset_data/
│   ├── pump-001/
│   │   ├── realtime.json
│   │   └── history/ (InfluxDB)
│   ├── motor-002/
│   │   ├── realtime.json
│   │   └── history/ (InfluxDB)
│   └── hx-003/
│       ├── realtime.json
│       └── history/ (InfluxDB)
├── maintenance_records/
│   ├── pump-001/
│   │   ├── 2024-01-15-preventive.json
│   │   ├── 2024-03-20-corrective.json
│   │   └── 2024-06-10-inspection.json
│   └── motor-002/
│       └── 2024-02-28-preventive.json
└── documents/
    ├── pump-001/ -> NDH Document Server
    └── motor-002/ -> NDH Document Server
```

#### 4.1.3 Factory Asset Library API

```python
# factory_asset_library.py
from typing import Dict, List, Optional
import yaml
import json

class FactoryAssetLibrary:
    """Factory Asset Library API"""
    
    def __init__(self, ndh_base_url: str, api_key: str):
        self.ndh_base_url = ndh_base_url
        self.api_key = api_key
    
    # ========== 資產定義管理 ==========
    
    def create_asset_definition(self, iadl: Dict) -> str:
        """創建資產定義"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/definitions"
        response = requests.post(url, json=iadl, headers=self._get_headers())
        response.raise_for_status()
        return response.json()["asset_id"]
    
    def get_asset_definition(self, asset_type: str) -> Dict:
        """獲取資產定義"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/definitions/{asset_type}"
        response = requests.get(url, headers=self._get_headers())
        response.raise_for_status()
        return response.json()
    
    def update_asset_definition(self, asset_type: str, iadl: Dict):
        """更新資產定義"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/definitions/{asset_type}"
        response = requests.put(url, json=iadl, headers=self._get_headers())
        response.raise_for_status()
    
    def list_asset_definitions(self) -> List[Dict]:
        """列出所有資產定義"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/definitions"
        response = requests.get(url, headers=self._get_headers())
        response.raise_for_status()
        return response.json()["definitions"]
    
    # ========== 資產實例管理 ==========
    
    def create_asset_instance(self, instance: Dict) -> str:
        """創建資產實例"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/instances"
        response = requests.post(url, json=instance, headers=self._get_headers())
        response.raise_for_status()
        return response.json()["instance_id"]
    
    def get_asset_instance(self, instance_id: str) -> Dict:
        """獲取資產實例"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/instances/{instance_id}"
        response = requests.get(url, headers=self._get_headers())
        response.raise_for_status()
        return response.json()
    
    def update_asset_instance(self, instance_id: str, instance: Dict):
        """更新資產實例"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/instances/{instance_id}"
        response = requests.put(url, json=instance, headers=self._get_headers())
        response.raise_for_status()
    
    def list_asset_instances(self, asset_type: Optional[str] = None) -> List[Dict]:
        """列出資產實例"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/instances"
        params = {"asset_type": asset_type} if asset_type else {}
        response = requests.get(url, headers=self._get_headers(), params=params)
        response.raise_for_status()
        return response.json()["instances"]
    
    # ========== 即時數據查詢 ==========
    
    def get_realtime_data(self, instance_id: str) -> Dict:
        """獲取資產的即時數據"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/instances/{instance_id}/realtime"
        response = requests.get(url, headers=self._get_headers())
        response.raise_for_status()
        return response.json()
    
    def get_historical_data(self, instance_id: str, start_time: str, end_time: str) -> List[Dict]:
        """獲取資產的歷史數據"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/instances/{instance_id}/history"
        params = {"start": start_time, "end": end_time}
        response = requests.get(url, headers=self._get_headers(), params=params)
        response.raise_for_status()
        return response.json()["data"]
    
    # ========== 維護記錄管理 ==========
    
    def create_maintenance_record(self, instance_id: str, record: Dict) -> str:
        """創建維護記錄"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/instances/{instance_id}/maintenance"
        response = requests.post(url, json=record, headers=self._get_headers())
        response.raise_for_status()
        return response.json()["record_id"]
    
    def list_maintenance_records(self, instance_id: str) -> List[Dict]:
        """列出維護記錄"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/instances/{instance_id}/maintenance"
        response = requests.get(url, headers=self._get_headers())
        response.raise_for_status()
        return response.json()["records"]
    
    # ========== 文件管理 ==========
    
    def list_documents(self, instance_id: str) -> List[Dict]:
        """列出資產的所有文件"""
        url = f"{self.ndh_base_url}/api/v1/asset_library/instances/{instance_id}/documents"
        response = requests.get(url, headers=self._get_headers())
        response.raise_for_status()
        return response.json()["documents"]
    
    def _get_headers(self) -> Dict:
        return {
            "Authorization": f"Bearer {self.api_key}",
            "Content-Type": "application/json"
        }
```

---

### 4.2 IADL Editor Runtime Mode

#### 4.2.1 概述

在運營時期，IADL Editor 切換到 **Runtime Mode**，提供以下功能：

1. **資產檢視**：查看工廠中所有資產的定義和實例
2. **即時監控**：查看資產的即時狀態和數據
3. **維護管理**：記錄維護活動、查看維護歷史
4. **資產更新**：更新資產定義和實例（版本控制）
5. **文件管理**：上傳、下載、查看資產相關文件

#### 4.2.2 Runtime Mode UI 設計

```
┌─────────────────────────────────────────────────────────────────┐
│ IADL Editor - Runtime Mode                                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌───────────────┐  ┌─────────────────────────────────────────┐ │
│  │ Asset Tree    │  │ Asset Details                           │ │
│  │               │  │                                         │ │
│  │ ├─ Pumps      │  │ Asset: Centrifugal Pump 001            │ │
│  │ │  ├─ pump-001│  │ Type: centrifugal_pump                 │ │
│  │ │  └─ pump-002│  │ Status: Running                        │ │
│  │ ├─ Motors     │  │                                         │ │
│  │ │  ├─ motor-001  │ [Realtime Data] [History] [Maintenance]│ │
│  │ │  └─ motor-002  │ [Documents] [Update]                   │ │
│  │ └─ Heat Exch. │  │                                         │ │
│  │    └─ hx-001  │  │ ┌─────────────────────────────────────┐│ │
│  │               │  │ │ Realtime Data                       ││ │
│  │               │  │ │                                     ││ │
│  │               │  │ │ Flow Rate:    100.5 m³/h           ││ │
│  │               │  │ │ Pressure:     5.2 bar              ││ │
│  │               │  │ │ Temperature:  45.3 °C              ││ │
│  │               │  │ │ Vibration:    2.1 mm/s             ││ │
│  │               │  │ │ Power:        15.8 kW              ││ │
│  │               │  │ │                                     ││ │
│  │               │  │ │ Last Updated: 2025-10-15 14:32:15  ││ │
│  │               │  │ └─────────────────────────────────────┘│ │
│  └───────────────┘  └─────────────────────────────────────────┘ │
│                                                                   │
└─────────────────────────────────────────────────────────────────┘
```

#### 4.2.3 資產檢視功能

```python
# iadl_editor_runtime.py
from PyQt6.QtWidgets import QMainWindow, QTreeWidget, QTreeWidgetItem, QTabWidget
from factory_asset_library import FactoryAssetLibrary

class IADLEditorRuntimeMode(QMainWindow):
    """IADL Editor Runtime Mode"""
    
    def __init__(self):
        super().__init__()
        self.asset_library = FactoryAssetLibrary(
            ndh_base_url="http://ndh.factory.local",
            api_key="your-api-key"
        )
        self.init_ui()
    
    def init_ui(self):
        """初始化 UI"""
        self.setWindowTitle("IADL Editor - Runtime Mode")
        self.setGeometry(100, 100, 1200, 800)
        
        # 左側：資產樹
        self.asset_tree = QTreeWidget()
        self.asset_tree.setHeaderLabel("Factory Assets")
        self.asset_tree.itemClicked.connect(self.on_asset_selected)
        
        # 右側：資產詳情（標籤頁）
        self.details_tabs = QTabWidget()
        self.details_tabs.addTab(self.create_realtime_tab(), "Realtime Data")
        self.details_tabs.addTab(self.create_history_tab(), "History")
        self.details_tabs.addTab(self.create_maintenance_tab(), "Maintenance")
        self.details_tabs.addTab(self.create_documents_tab(), "Documents")
        self.details_tabs.addTab(self.create_update_tab(), "Update")
        
        # 載入資產樹
        self.load_asset_tree()
    
    def load_asset_tree(self):
        """載入資產樹"""
        # 獲取所有資產實例
        instances = self.asset_library.list_asset_instances()
        
        # 按類型分組
        asset_types = {}
        for instance in instances:
            asset_type = instance["asset_type"]
            if asset_type not in asset_types:
                asset_types[asset_type] = []
            asset_types[asset_type].append(instance)
        
        # 建立樹狀結構
        for asset_type, instances in asset_types.items():
            type_item = QTreeWidgetItem(self.asset_tree, [asset_type])
            for instance in instances:
                instance_item = QTreeWidgetItem(type_item, [instance["id"]])
                instance_item.setData(0, Qt.ItemDataRole.UserRole, instance)
    
    def on_asset_selected(self, item, column):
        """當資產被選中時"""
        instance = item.data(0, Qt.ItemDataRole.UserRole)
        if instance:
            self.load_asset_details(instance)
    
    def load_asset_details(self, instance: Dict):
        """載入資產詳情"""
        instance_id = instance["id"]
        
        # 載入即時數據
        realtime_data = self.asset_library.get_realtime_data(instance_id)
        self.update_realtime_tab(realtime_data)
        
        # 載入維護記錄
        maintenance_records = self.asset_library.list_maintenance_records(instance_id)
        self.update_maintenance_tab(maintenance_records)
        
        # 載入文件
        documents = self.asset_library.list_documents(instance_id)
        self.update_documents_tab(documents)
```

---

### 4.3 版本控制與變更管理

#### 4.3.1 資產版本控制

Factory Asset Library 應該支援資產定義的版本控制：

```yaml
asset_type:
  name: centrifugal_pump
  version: "1.2.0"  # 語義化版本號
  version_history:
    - version: "1.0.0"
      date: "2024-01-15"
      author: "林志錚"
      changes: "初始版本"
    - version: "1.1.0"
      date: "2024-06-20"
      author: "王大明"
      changes: "新增振動感測器"
    - version: "1.2.0"
      date: "2025-10-15"
      author: "李小華"
      changes: "更新維護排程"
```

#### 4.3.2 變更管理流程

```
1. 工程師在 IADL Editor 中修改資產定義
   ↓
2. IADL Editor 自動建立新版本
   - 版本號自動遞增
   - 記錄變更內容
   - 記錄作者和時間
   ↓
3. 提交變更到 Factory Asset Library
   ↓
4. Factory Asset Library 觸發審批流程（可選）
   ↓
5. 審批通過後，新版本生效
   ↓
6. NDH Runtime 自動更新資產實例
   - 保留舊版本數據
   - 應用新版本定義
   ↓
7. Omniverse 自動同步更新
```

---

## 5. 總結

IADL Editor (Asset Designer) 是一個貫穿資產全生命週期的核心工具：

### 設計時期

✅ **與 CAD/PLM 整合**：從 3D 模型和 BOM 自動生成 IADL  
✅ **與 AVEVA Dev Studio 整合**：自動生成 PLC/SCADA 程式  
✅ **供應商資訊管理**：連接 ERP，管理供應商、採購、維護資訊  
✅ **文件管理**：連接 NDH Document Server，集中管理所有文件  
✅ **生產資訊管理**：連接 MES，管理製程參數和品質標準

### 運營時期

✅ **Factory Asset Library**：集中式資產庫，存儲在 NDH Runtime  
✅ **即時監控**：查看資產的即時狀態和歷史數據  
✅ **維護管理**：記錄維護活動、查看維護歷史  
✅ **版本控制**：自動管理資產定義的版本和變更  
✅ **多工具連接**：IADL Editor、FDL Editor、NDH 都可以連接和更新

這個完整的架構確保了資產資訊的一致性、可追溯性和可維護性，為工業數位分身的成功實施奠定了堅實的基礎。

---

**IDTF V3.5 - Build Once, Use Everywhere!** 🚀

