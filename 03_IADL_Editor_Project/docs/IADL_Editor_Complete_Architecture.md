# IADL Editor (Asset Designer) 完整架構設計

**版本**: 1.1  
**日期**: 2025-10-15  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)

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
- **ISA-95**: 企業與控制系統整合標準

---

## 1. 概述

**IADL Editor (Asset Designer)** 是 IDTF V3.5 的核心工具，負責在資產的整個生命週期中進行定義、設計、更新和維護。本文件描述了 IADL Editor 在兩個關鍵時期的完整架構：

1.  **設計時期 (Creation/Design Time)**: 資產的初始設計和定義階段，著重於資產藍圖的創建、豐富數據模型的建立、ISA-95 階層的定義以及與外部工程系統的整合。
2.  **運營時期 (Runtime/Maintain/Update Time)**: 工廠運營中的資產維護和更新階段，側重於資產實例的管理、生命週期狀態的追蹤、即時數據的檢視以及與運營管理系統的互動。

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
│  │  - ISA-95 階層定義  │          │  - 生命週期狀態管理      │  │
│  │  - 次要階層配置     │          │  - 豐富屬性更新          │  │
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

在設計時期，IADL Editor 扮演著資產藍圖設計師的角色，它不僅要整合來自工程設計系統的數據，更要賦予資產豐富的語義和行為定義，並確保其符合工業標準。

### 3.1 核心功能強化

IADL Editor 在設計時期應提供以下強化功能，以支援 IADL Spec 的修正建議：

1.  **ISA-95 階層配置工具**：提供視覺化介面，讓使用者能夠定義資產在 ISA-95 八級階層中的位置，並支援自訂階層深度和命名慣例。
2.  **多維度視圖管理**：允許使用者創建和管理「次要階層」（如學科、維護、安全分組），並將主要階層中的資產引用到這些邏輯分組中。
3.  **豐富屬性編輯器**：提供全面的屬性編輯介面，支援 IADL 擴展的核心屬性，包括識別資訊、階層關聯、工程屬性、營運屬性、維護屬性、財務屬性、狀態資訊、關聯文件和連接關係。
4.  **生命週期狀態定義**：允許使用者為資產定義其生命週期狀態，並配置狀態轉換規則和相關事件。
5.  **範本化建立與繼承**：提供 IADL 範本庫，支援從通用範本派生特定資產類型，並實現屬性繼承和定制化。
6.  **中央類別庫整合**：與基於 ISO 15926/CFIHOS 的數據字典整合，確保資產屬性、單位和允許值的一致性。

### 3.2 與 CAD/PLM 系統整合

IADL Editor 與 CAD/PLM 系統的整合是資產設計的起點，旨在自動化從工程設計到數位分身定義的過程。

#### 3.2.1 支援的 CAD 系統

| CAD 系統 | 整合方式 | 支援功能 |
|---------|---------|---------|
| **Autodesk Inventor** | COM API | 3D 模型匯入、BOM 提取、參數同步 |
| **SolidWorks** | SolidWorks API | 3D 模型匯入、配置變體、裝配體結構 |
| **CATIA** | CAA API | 3D 模型匯入、產品結構、PMI 數據 |
| **Siemens NX** | NX Open API | 3D 模型匯入、裝配體、製造資訊 |

#### 3.2.2 支援的 PLM 系統

| PLM 系統 | 整合方式 | 支援功能 |
|---------|---------|---------|
| **Siemens Teamcenter** | REST API + SOA | 零件資訊、BOM、變更管理、文件管理、**資產生命週期狀態同步** |
| **PTC Windchill** | REST API | 零件資訊、BOM、生命週期狀態 |
| **Dassault ENOVIA** | REST API | 產品結構、文件、變更單 |
| **SAP PLM** | OData API | 物料主數據、BOM、ECN/ECO |

#### 3.2.3 CAD/PLM 整合工作流

```
1. 在 CAD 中設計資產 3D 模型
   ↓
2. 在 PLM 中管理零件、BOM 和資產生命週期狀態
   ↓
3. IADL Editor 從 PLM 讀取資產資訊，包括 ISA-95 階層相關屬性
   ↓
4. 自動生成 IADL 資產定義
   - 3D 幾何（從 CAD 匯入 USD）
   - 零件清單（從 PLM 讀取 BOM）
   - 技術規格（從 PLM 讀取屬性）
   - **ISA-95 階層歸屬** (如 `equipment_type`, `equipment`)
   - **初始生命週期狀態** (如 `design_in_progress`)
   - **工程屬性** (如 `design_pressure`, `material`)
   ↓
5. 工程師在 IADL Editor 中補充
   - 數據標籤（sensors, actuators）
   - 行為腳本（behaviors）
   - 控制邏輯（PLC/SCADA）
   - **次要階層分組** (如 `logical_groups`)
   - **營運、維護、財務屬性**
   - **關聯文件** (連結到 NDH Document Server)
   - **連接關係**
   ↓
6. 保存到 Factory Asset Library
```

#### 3.2.4 實作範例：從 Teamcenter 讀取資產 (更新)

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
                "lifecycle_status": item.get("lifecycle_status", "design_in_progress"), # 從 PLM 同步生命週期狀態
                "isa95_hierarchy_level": item.get("isa95_level", "equipment_type"), # 從 PLM 同步 ISA-95 層級
                
                # 3D 模型（從 CAD 匯入）
                "geometry": {
                    "usd_file": f"assets/{item["item_id"]}.usd",
                    "thumbnail": f"assets/{item["item_id"]}_thumb.png"
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
                
                # 文件連結 (從 PLM 獲取，並轉換為 NDH Document Server 連結)
                "documents": [
                    {
                        "type": doc["document_type"],
                        "name": doc["object_name"],
                        "url": f"ndh://documents/{doc["uid"]}" # 假設 PLM 提供了 UID
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
                
                # 參數（需要工程師手動補充，或從 CAD/PLM 參數同步）
                "parameters": [],
                
                # 數據標籤（需要工程師手動補充）
                "data_tags": [],
                
                # 行為（需要工程師手動補充）
                "behaviors": [],
                
                # 新增：邏輯分組（需要工程師手動補充）
                "logical_groups": []
            }
        }
        
        return iadl
```

---

### 3.3 與 AVEVA Dev Studio 整合

#### 3.3.1 AVEVA Dev Studio 概述

**AVEVA Dev Studio** 是 AVEVA 的工業自動化開發環境，支援：
- PLC 程式設計（IEC 61131-3）
- SCADA 組態設計
- HMI 畫面設計
- 控制邏輯模擬

#### 3.3.2 整合目標

IADL Editor 與 AVEVA Dev Studio 整合，實現：

1.  **自動生成 PLC 程式**：根據 IADL 定義的資產行為和數據標籤，自動生成 PLC 控制邏輯。
2.  **自動生成 SCADA 組態**：根據 IADL 定義的數據標籤和其 ISA-95 階層歸屬，自動生成 SCADA 標籤和畫面。
3.  **雙向同步**：AVEVA Dev Studio 中的變更可以同步回 IADL，更新行為腳本和數據標籤映射。

#### 3.3.3 整合工作流

```
1. 在 IADL Editor 中定義資產
   - 數據標籤（sensors, actuators）
   - 行為（behaviors）
   - **ISA-95 階層歸屬**
   ↓
2. IADL Editor 自動生成 AVEVA Dev Studio 專案
   - PLC 程式（Structured Text, Ladder Diagram）
   - SCADA 標籤（OPC UA, Modbus），**並根據 ISA-95 階層組織標籤結構**
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

#### 3.3.4 實作範例：生成 PLC 程式 (保持不變)

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
PROGRAM {asset["name"]}_Control
VAR
    (* Sensors *)
"""
        
        for sensor in sensors:
            st_code += f"    {sensor["name"]}: {self._get_plc_type(sensor["data_type"])};\n"
        
        st_code += "\n    (* Actuators *)\n"
        for actuator in actuators:
            st_code += f"    {actuator["name"]}: {self._get_plc_type(actuator["data_type"])};\n"
        
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
        logic = f"""
IF {behavior.get("trigger", "StartButton")} THEN
    state := 1;
END_IF;

IF state = 1 THEN
    (* Activate actuators *)
"""
        for act in actuators:
            if act["name"] in behavior.get("actions", []):
                logic += f"    {act["name"]} := TRUE;\n"
        logic += "    state := 2;\nEND_IF;\n"
        return logic
    
    def _generate_stop_logic(self, behavior: Dict, actuators: List[Dict]) -> str:
        """生成停止邏輯"""
        logic = f"""
IF {behavior.get("trigger", "StopButton")} THEN
    state := 0;
END_IF;

IF state = 0 THEN
    (* Deactivate actuators *)
"""
        for act in actuators:
            if act["name"] in behavior.get("actions", []):
                logic += f"    {act["name"]} := FALSE;\n"
        logic += "END_IF;\n"
        return logic
    
    def _generate_emergency_stop_logic(self, behavior: Dict, actuators: List[Dict]) -> str:
        """生成緊急停止邏輯"""
        logic = f"""
IF {behavior.get("trigger", "EmergencyStopButton")} THEN
    error := TRUE;
END_IF;

IF error THEN
    (* Deactivate all actuators *)
"""
        for act in actuators:
            logic += f"    {act["name"]} := FALSE;\n"
        logic += "    state := 0; (* Reset state *)\nEND_IF;\n"
        return logic
```

---

### 3.4 與 ERP 系統整合

IADL Editor 應能從 ERP 系統讀取供應商、採購和物料主數據，以豐富資產的財務和供應鏈相關屬性。

#### 3.4.1 ERP 整合目標

- **供應商資訊**：製造商、供應商、安裝商、維護商的詳細資訊。
- **採購資訊**：採購訂單號、採購日期、成本、交貨日期。
- **物料主數據**：物料編號、描述、製造商零件號、單位。

#### 3.4.2 ERP 整合工作流

```
1. 在 IADL Editor 中定義資產
   ↓
2. 連接到 ERP 系統
   ↓
3. 從 ERP 讀取供應商、採購和物料主數據
   ↓
4. 自動填充到 IADL 資產定義
   - vendor_info（供應商資訊）
   - procurement_info（採購資訊）
   - financial_attributes（財務屬性，如採購成本、折舊資訊）
   ↓
5. 保存到 Factory Asset Library
```

#### 3.4.3 實作範例：從 SAP ERP 讀取數據 (保持不變)

```python
# erp_connector.py
import requests
from typing import Dict, List

class SAPERPConnector:
    """SAP ERP 連接器（以 OData API 為例）"""
    
    def __init__(self, base_url: str, client_id: str, client_secret: str):
        self.base_url = base_url
        self.client_id = client_id
        self.client_secret = client_secret
        self.access_token = self._get_access_token()
        self.headers = {"Authorization": f"Bearer {self.access_token}"}
    
    def _get_access_token(self) -> str:
        """獲取 OAuth 2.0 訪問令牌"""
        token_url = f"{self.base_url}/oauth/token"
        response = requests.post(token_url, data={
            "grant_type": "client_credentials",
            "client_id": self.client_id,
            "client_secret": self.client_secret
        })
        response.raise_for_status()
        return response.json()["access_token"]
    
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
            "address": f"{data.get("StreetName", "")}, {data.get("CityName", "")}, {data.get("Country", "")}"
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

### 3.5 NDH Document Server 整合

NDH Document Server 是資產相關文件的集中式存儲庫，IADL Editor 應提供無縫的文件管理功能。

#### 3.5.1 Document Server 架構 (保持不變)

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

#### 3.5.2 Document Server API (保持不變)

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

#### 3.5.3 IADL Editor 中的文件管理 (保持不變)

在 IADL Editor 中，工程師可以：

1.  **上傳文件**：拖放文件到 IADL Editor，自動上傳到 Document Server
2.  **瀏覽文件**：在 IADL Editor 中瀏覽資產的所有文件
3.  **下載文件**：從 Document Server 下載文件到本地
4.  **刪除文件**：從 Document Server 刪除過時的文件
5.  **版本控制**：Document Server 自動保存文件的所有版本

---

### 3.6 與 MES 系統整合

IADL Editor 應能夠從 MES 系統讀取生產相關資訊，並將其關聯到資產定義，特別是營運屬性和品質標準。

#### 3.6.1 MES 整合目標

IADL Editor 應該能夠從 MES 系統讀取生產相關資訊，並將其關聯到資產定義：

-   **製程參數**：溫度、壓力、速度等
-   **品質標準**：公差、檢驗項目、合格標準
-   **生產配方**：原料配比、製程步驟
-   **工單資訊**：生產計畫、實際產量、良率
-   **營運屬性**：資產的當前運營模式、生產狀態等。

#### 3.6.2 MES 整合工作流

```
1. 在 IADL Editor 中定義資產
   ↓
2. 連接到 MES 系統
   ↓
3. 從 MES 讀取製程參數、品質標準和營運屬性
   ↓
4. 自動填充到 IADL 資產定義
   - parameters（製程參數）
   - quality_standards（品質標準）
   - recipes（生產配方）
   - operational_attributes (營運屬性)
   ↓
5. 保存到 Factory Asset Library
   ↓
6. NDH Runtime 使用這些資訊進行生產監控
```

#### 3.6.3 實作範例：從 MES 讀取製程參數 (保持不變)

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

在運營時期，IADL Editor 轉變為資產運營和維護的智慧助手，提供對工廠中實際部署資產的全面視圖和管理能力。

### 4.1 Factory Asset Library

#### 4.1.1 概述

**Factory Asset Library** 是一個集中式的資產庫，存儲在 NDH Runtime 中，包含：

-   **資產定義 (IADL)**：所有資產的定義和元數據，包括其 ISA-95 階層、多維度視圖和生命週期狀態定義。
-   **資產實例 (FDL)**：工廠中實際部署的資產實例，包含實例特有的屬性、ISA-95 階層路徑和當前生命週期狀態。
-   **即時數據**：資產的當前狀態和歷史數據。
-   **維護記錄**：維護歷史、故障記錄、備件更換。
-   **文件**：操作手冊、維護指南、檢驗報告。

#### 4.1.2 Factory Asset Library 架構 (保持不變)

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

#### 4.1.3 Factory Asset Library API (保持不變)

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

1.  **資產檢視**：查看工廠中所有資產的定義和實例，包括其 ISA-95 階層、次要階層分組和豐富屬性。
2.  **即時監控**：查看資產的即時狀態和數據，並可追溯歷史數據。
3.  **維護管理**：記錄維護活動、查看維護歷史，並可更新資產的維護相關屬性。
4.  **資產更新**：更新資產定義和實例（支援版本控制），包括生命週期狀態的變更。
5.  **文件管理**：上傳、下載、查看資產相關文件。

#### 4.2.2 Runtime Mode UI 設計

```
┌─────────────────────────────────────────────────────────────────┐
│ IADL Editor - Runtime Mode                                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌───────────────┐  ┌─────────────────────────────────────────┐ │
│  │ Asset Tree    │  │ Asset Details                           │ │
│  │               │  │                                         │ │
│  │ ├─ Enterprise │  │ Asset: Centrifugal Pump 001            │ │
│  │ │  ├─ SiteA   │  │ Type: centrifugal_pump                 │ │
│  │ │  │  ├─ Area1│  │ Status: Running                        │ │
│  │ │  │  │  ├─ Pump_001 │                                         │ │
│  │ │  │  │  └─ Pump_002 │ [Realtime Data] [History] [Maintenance]│ │
│  │ │  ├─ SiteB   │  │ [Documents] [Update] [Lifecycle]       │ │
│  │ ├─ Logical Grp │  │                                         │ │
│  │ │  ├─ Mech.   │  │ ┌─────────────────────────────────────┐│ │
│  │ │  │  ├─ Pump_001 │ │ Realtime Data                       ││ │
│  │ │  └─ Elec.   │  │ │                                     ││ │
│  │ └─ ...        │  │ │ Flow Rate:    100.5 m³/h           ││ │
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

#### 4.2.3 資產檢視功能 (更新)

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
        self.details_tabs.addTab(self.create_lifecycle_tab(), "Lifecycle") # 新增生命週期標籤頁
        
        # 載入資產樹
        self.load_asset_tree()
    
    def load_asset_tree(self):
        """載入資產樹，支援 ISA-95 階層和次要階層"""
        self.asset_tree.clear()
        instances = self.asset_library.list_asset_instances()
        
        # 構建 ISA-95 主要階層
        isa95_root = QTreeWidgetItem(self.asset_tree, ["ISA-95 Hierarchy"])
        isa95_nodes = {}
        
        # 構建邏輯分組次要階層
        logical_groups_root = QTreeWidgetItem(self.asset_tree, ["Logical Groups"])
        logical_group_nodes = {}
        
        for instance in instances:
            instance_id = instance.get("instance_id")
            display_name = instance.get("display_name", instance_id)
            
            # 處理 ISA-95 階層
            isa95_path = instance.get("isa95_hierarchy_path")
            if isa95_path:
                path_elements = isa95_path.split("/")
                current_parent = isa95_root
                for i, element in enumerate(path_elements):
                    if element not in isa95_nodes:
                        node = QTreeWidgetItem(current_parent, [element])
                        isa95_nodes[element] = node
                    current_parent = isa95_nodes[element]
                QTreeWidgetItem(current_parent, [display_name]).setData(0, 1, instance_id) # 儲存 instance_id
            
            # 處理邏輯分組次要階層
            logical_groups = instance.get("logical_group_references", [])
            for group_ref in logical_groups:
                group_type = list(group_ref.keys())[0] # e.g., 'discipline'
                group_name = group_ref[group_type] # e.g., 'mechanical'
                
                if group_type not in logical_group_nodes:
                    type_node = QTreeWidgetItem(logical_groups_root, [group_type.capitalize()])
                    logical_group_nodes[group_type] = {}
                    logical_group_nodes[group_type][group_name] = QTreeWidgetItem(type_node, [group_name.capitalize()])
                elif group_name not in logical_group_nodes[group_type]:
                    type_node = logical_group_nodes[group_type][list(logical_group_nodes[group_type].keys())[0]].parent()
                    logical_group_nodes[group_type][group_name] = QTreeWidgetItem(type_node, [group_name.capitalize()])
                
                QTreeWidgetItem(logical_group_nodes[group_type][group_name], [display_name]).setData(0, 1, instance_id)

        self.asset_tree.expandAll()

    def on_asset_selected(self, item: QTreeWidgetItem, column: int):
        """當資產被選中時顯示詳情"""
        instance_id = item.data(0, 1) # 從 item 中獲取 instance_id
        if instance_id:
            asset_instance = self.asset_library.get_asset_instance(instance_id)
            self.display_asset_details(asset_instance)

    def display_asset_details(self, asset_instance: Dict):
        """在詳情面板中顯示資產資訊"""
        # 這裡需要更新各個標籤頁的內容
        # 例如：
        # self.realtime_tab.update_data(self.asset_library.get_realtime_data(asset_instance["instance_id"]))
        # self.lifecycle_tab.update_data(asset_instance["lifecycle_status"], asset_instance["lifecycle_events"])
        pass

    def create_realtime_tab(self):
        # 實現即時數據標籤頁
        return QWidget()

    def create_history_tab(self):
        # 實現歷史數據標籤頁
        return QWidget()

    def create_maintenance_tab(self):
        # 實現維護管理標籤頁
        return QWidget()

    def create_documents_tab(self):
        # 實現文件管理標籤頁
        return QWidget()

    def create_update_tab(self):
        # 實現資產更新標籤頁
        return QWidget()

    def create_lifecycle_tab(self):
        # 實現生命週期管理標籤頁
        return QWidget()
```

---

## 5. 結論

IADL Editor (Asset Designer) 在 IDTF V3.5 中扮演著至關重要的角色，它不僅是資產藍圖的創建工具，更是連接工程設計與工廠運營的橋樑。透過整合 IADL Spec 修正建議中提出的 ISA-95 標準符合性、多維度視圖、豐富數據模型和生命週期管理等概念，IADL Editor 的架構設計得到了顯著強化。

這些改進使得 IADL Editor 能夠：

-   **提供更精確和標準化的資產定義**：確保資產模型與 ISA-95 階層模型深度整合，提升數據的互操作性。
-   **支援更靈活的資產組織方式**：透過主要階層和次要階層的結合，滿足不同角色和應用場景對資產視圖的需求。
-   **實現更全面的資產數據管理**：涵蓋資產從設計到退役的全生命週期數據，為數位分身提供豐富的語義上下文。
-   **促進更高效的工程與運營協同**：自動化數據流轉，減少手動錯誤，加速從設計到部署的過程。

未來，IADL Editor 將持續演進，以適應工業數位轉型中不斷變化的需求，為構建智慧工廠和實現工業 4.0 願景提供強大的支持。

