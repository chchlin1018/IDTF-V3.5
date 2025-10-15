# 技術白皮書：NDH 的核心機制 - 從 IADL 定義到動態物件實體的生成

**文件版本**: 1.0  
**發布日期**: 2025-10-12  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)  
**參考資料**: [1, 2, 3]

---

## 1. 執行摘要

在工業 4.0 的宏偉藍圖中，數位分身 (Digital Twin) 的核心挑戰在於如何將抽象的資產模型轉化為可操作、可互動的軟體實體。**NDH (Neutral Data Hub)** 透過其精巧的架構，完美地回答了這個問題。NDH 的核心機制之一，便是能夠動態地讀取 **IADL (Industrial Asset Data Language)** 的資產定義，並在記憶體中生成功能完整、可遠端存取的對映物件實體。

本白皮書將深入剖析此一核心機制，詳細闡述 NDH 如何借鏡 **CORBA (Common Object Request Broker Architecture)** 的設計哲學，透過 `IADLConnector`、`AssetServant`、`AssetAdapter` 及 `NamingService` 等一系列組件的協同工作，實現從靜態的 IADL 模型到動態的物件實體的完整生命週期管理。理解此一機制，是掌握 NDH 如何建構一個統一、分散式且事件驅動的工業數位分身生態系統的關鍵。

> **核心理念**：實現「一次建模，到處使用」(Model Once, Use Everywhere) 的願景，將 IADL 的標準化定義轉化為在整個企業架構中可被重複使用的動態軟體組件。

---

## 2. 設計哲學：CORBA 啟發的分散式物件架構

要理解 NDH 的物件生成機制，首先必須了解其深受 CORBA 影響的設計哲學 [2]。NDH 並非重新發明輪子，而是將 CORBA 歷經數十年驗證的穩定分散式物件模型，以現代化的 Python 技術棧重新詮釋。下表展示了兩者之間的核心概念對應關係：

| CORBA 核心概念 | NDH 對應組件 | 職責說明 |
| :--- | :--- | :--- |
| **IDL (Interface Definition Language)** | **IADL** | 定義資產的介面、屬性和行為，是所有物件的「契約」。 |
| **Servant** | **`AssetServant`** | 資產物件的**實際記憶體實作**，包含了狀態資料和業務邏輯。 |
| **POA (Portable Object Adapter)** | **`AssetAdapter`** | **生命週期管理器**，負責啟動 (activate) 和停用 (deactivate) `AssetServant`。 |
| **ORB (Object Request Broker)** | **NDH Broker (`NDH_ORB`)** | 核心請求代理，負責路由客戶端的請求到正確的物件實體。 |
| **IOR (Interoperable Object Reference)** | **`AssetReference`** | **分散式物件的「位址」**，提供一個全域唯一的標識符，讓任何客戶端都能找到該物件。 |
| **Naming Service** | **`NamingService`** | **物件的「電話簿」**，將人類可讀的名稱 (如 "Pump_001") 綁定到機器可讀的 `AssetReference`。 |

這種架構使得 NDH 中的每一個資產，都能被視為一個獨立、可定址的分散式物件，為系統帶來了極高的靈活性和可擴展性。

---

## 3. 物件生成與生命週期管理詳解

從一份 IADL 檔案到一個可用的物件實體，NDH 內部經歷了一系列定義明確的步驟。以下是完整的流程圖與各階段的詳細說明：

![NDH 物件生命週期流程圖](../images/ndh-object-lifecycle.png)

### 步驟 1 & 2：IADL 定義載入

流程的起點是 `IADLConnector`。當此連接器啟動時，它會讀取在設定中指定的 IADL 檔案路徑。

> **程式碼片段 (`iadl_connector.py`)** [3]:
> ```python
> class IADLConnector(BaseConnector):
>     async def connect(self) -> bool:
>         # ... 驗證設定 ...
>         file_path = self.config.get("iadl_file_path")
>         self.iadl_file = Path(file_path)
>         
>         # 載入 IADL 檔案並剖析
>         await self._load_iadl_file()
>         
>         self._set_status(ConnectorStatus.CONNECTED)
>         return True
> ```

### 步驟 3 & 4：剖析與實例化 `AssetServant`

`_load_iadl_file` 方法會根據檔案類型（JSON 或 YAML/IADL）進行剖析，將其轉換為一個 Python 字典 `self.assets`。接著，一個未在程式碼中明確顯示的**工廠或管理器** (Factory/Manager) 會遍歷這個字典，為每一個資產定義**實例化**一個 `AssetServant` 物件。

> **程式碼片段 (`CORBA_INSPIRED_ARCHITECTURE.md`)** [2]:
> ```python
> class AssetServant(AssetInterface):
>     """資產僕人 (對應 CORBA Servant)，實際的資產物件實作"""
>     def __init__(self, asset_id: str, asset_type: str, location: str):
>         self.asset_id = asset_id
>         self.asset_type = asset_type
>         self.location = location
>         self.attributes: Dict[str, Any] = {} # IADL 中定義的屬性會填入此處
>         self.state = AssetLifecycleState.INACTIVE
> ```

在這個階段，IADL 中定義的 `properties` 會被填入 `AssetServant` 實例的 `attributes` 字典中。`behaviors` 則會對應到 `AssetServant` 中可被 `invoke_operation` 呼叫的方法。

### 步驟 5：啟動物件 (`AssetAdapter`)

一個被實例化的 `AssetServant` 僅僅是記憶體中的一個被動物件。要讓它「上線」並能回應請求，就必須透過 `AssetAdapter` 進行**啟動 (Activation)**。

> **程式碼片段 (`CORBA_INSPIRED_ARCHITECTURE.md`)** [2]:
> ```python
> class AssetAdapter:
>     """資產適配器 (對應 CORBA POA)，管理資產物件的生命週期"""
>     async def activate_object(self, servant: AssetServant) -> str:
>         """啟動物件，返回唯一的物件 ID"""
>         object_id = str(uuid.uuid4()) # 產生一個全域唯一的 ID
>         self.servants[servant.asset_id] = servant
>         self.active_objects[object_id] = servant.asset_id
>         await servant.activate() # 將僕人狀態設為 ACTIVE
>         return object_id
> ```

此步驟賦予了物件一個**唯一的執行時期 ID** (`object_id`)，並將其納入 `AssetAdapter` 的管理範圍，使其從一個普通的 Python 物件轉變為一個可被 NDH 核心路由的「活躍物件」。

### 步驟 6：註冊至命名服務 (`NamingService`)

活躍的物件還需要一個能被輕易找到的方式。這就是 `NamingService` 的職責。它會為物件建立一個 `AssetReference`，並將其與一個人類可讀的名稱綁定。

> **程式碼片段 (`CORBA_INSPIRED_ARCHITECTURE.md`)** [2]:
> ```python
> class NamingService:
>     """命名服務，提供名稱到物件參考的映射"""
>     async def bind(self, name: str, obj_ref: AssetReference, context: str = "default"):
>         full_name = f"{context}/{name}"
>         self.bindings[full_name] = obj_ref
> ```

這個 `AssetReference` 是分散式系統的關鍵，它包含了找到並與該物件通訊所需的所有資訊。

### 步驟 7：物件就緒

完成以上步驟後，一個源自 IADL 定義的資產物件實體便已完全就緒。系統中的任何其他組件或外部客戶端，現在都可以透過 `NamingService` 解析其名稱，獲得 `AssetReference`，並透過 NDH Broker 向其發送請求（如讀取屬性、呼叫操作）。

---

## 4. 核心組件剖析

### 4.1 `AssetServant`：物件實體的核心

`AssetServant` 是 IADL 資產在記憶體中的直接體現。它不僅儲存狀態，還定義了標準化的互動介面。

- **狀態儲存**: `self.attributes` 字典儲存了所有來自 IADL 的屬性值。
- **標準介面 (`AssetInterface`)**: 
  - `get_attribute(name)`: 讀取屬性。
  - `set_attribute(name, value)`: 設定屬性。
  - `invoke_operation(op_name, params)`: 呼叫在 IADL 中定義的行為。
  - `get_state()`: 獲取物件的完整當前狀態。

### 4.2 `AssetReference`：分散式物件的位址

`AssetReference` 是實現位置透明性的關鍵。它是一個資料類別，封裝了存取一個遠端物件所需的所有資訊。

> **程式碼片段 (`CORBA_INSPIRED_ARCHITECTURE.md`)** [2]:
> ```python
> @dataclass
> class AssetReference:
>     """資產參考 (對應 CORBA IOR)"""
>     asset_id: str      # IADL 中定義的 ID
>     uuid: str          # 執行時期唯一 ID
>     location: str      # 物理或邏輯位置
>     endpoint: str      # 網路端點 (e.g., 192.168.1.100:8000)
>     protocol: str      # 通訊協定 (http, ws, mqtt)
> 
>     def to_string(self) -> str:
>         """轉換為 ndhref:// 格式的字串"""
>         return f"ndhref://{self.protocol}@{self.endpoint}/{self.location}/{self.asset_id}?uuid={self.uuid}"
> ```

這個 `ndhref://` 格式的字串，就像一個網頁的 URL，可以在網路中自由傳遞，任何收到此參考的客戶端都能夠解析它並與對應的物件進行通訊，而無需關心該物件究竟在哪一台伺服器上運行。

---

## 5. 結論：從模型到實例的動態橋樑

NDH 的物件生成機制是其架構設計的精髓所在。它不僅僅是簡單的程式碼生成，而是一個**完整、動態、分散式的物件生命週期管理系統**。

透過借鏡 CORBA 的成熟模型並以現代化的方式實現，NDH 成功地搭建了一座從抽象的 IADL 模型到具體的記憶體物件實體之間的橋樑。這一機制賦予了 NDH 以下關鍵能力：

- **標準化與一致性**: 所有物件實體都源自 IADL 這單一事實來源，確保了整個系統的一致性。
- **動態與靈活性**: 可以在系統執行期間，動態地載入新的 IADL 模型，並生成、啟動新的物件實體，無需重啟服務。
- **位置透明性**: 客戶端與物件的互動是位置無關的，極大地簡化了分散式系統的開發與維護。
- **可擴展性**: 該架構天然支援水平擴展，可以將不同的 `AssetAdapter` 分散到多個伺服器節點上。

總而言之，從 IADL 到動態物件實體的生成機制，是 NDH 能夠實現其作為「中立資料中樞」承諾的基石，也是其能夠以極低成本實現複雜工業數位分身場景的根本原因。

---

## 6. 參考資料

[1] Lin, C. C. (2025). *idtf-v3: IDTF V3.1 完整技術白皮書*. GitHub. [https://github.com/chchlin1018/idtf-v3](https://github.com/chchlin1018/idtf-v3)

[2] Lin, C. C. (2025). *idtf-documentation: IDTF 完整文件、程式碼、範例和視覺化*. GitHub. [https://github.com/chchlin1018/idtf-documentation](https://github.com/chchlin1018/idtf-documentation)

[3] Lin, C. C. (2025). *NDH-Win-Test1: NDH 測試開發框架*. GitHub. [https://github.com/chchlin1018/NDH-Win-Test1](https://github.com/chchlin1018/NDH-Win-Test1)

