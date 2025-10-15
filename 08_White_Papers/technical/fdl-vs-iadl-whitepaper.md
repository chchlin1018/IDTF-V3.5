# 技術白皮書：IADL 與 FDL 的協同作用 - 從資產藍圖到工廠級實例化

**文件版本**: 1.0  
**發布日期**: 2025-10-12  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)  
**參考資料**: [1, 2, 3]

---

## 1. 執行摘要

在建構複雜的工業數位分身 (Digital Twin) 系統時，一個關鍵的挑戰是如何有效地區分「資產類型定義」與「資產實例部署」。傳統方法常將兩者混為一談，導致模型重用性差、擴展性不足。為了解決此問題，IDTF (Industrial Digital Twin Framework) 提出了一個創新的雙語言模型：

- **IADL (Industrial Asset Data Language)**: 作為資產的「藍圖」或「類別」，專注於定義一個資產類型是什麼，包含哪些屬性、行為和資料介面。
- **FDL (Factory Design Language)**: 作為工廠的「部署計畫」或「實例化配置」，專注於聲明需要多少個特定類型的資產、它們被部署在哪裡，以及它們的初始設定。

本白皮書將深入探討 IADL 與 FDL 的根本區別、它們如何協同工作，以及 **NDH (Neutral Data Hub)** 如何利用 FDL 檔案來動態生成、部署和管理成百上千個資產物件的實際分身 (Instance)，從而實現前所未有的靈活性、可擴展性和可維護性。

> **核心價值主張**：透過 IADL 的「定義」與 FDL 的「實例化」分離，實現資產模型的最大化重用，並賦予工廠佈局設計和數位分身部署極大的靈活性。

---

## 2. 概念的二元性：IADL vs. FDL

將 IADL 和 FDL 視為物件導向程式設計 (OOP) 中的 `Class` 和 `Instance` 的概念，可以幫助我們更好地理解它們的關係。

- **IADL 就是 `Class`**：它定義了一個物件的結構和能力，但它本身不是一個活的物件。
- **FDL 就是 `new MyClass()` 的聲明**：它觸發了創建一個或多個活的、可操作的物件實例的過程。

下表清晰地展示了兩者在職責上的根本區別：

| 特性 | IADL (資產藍圖) | FDL (工廠部署計畫) |
| :--- | :--- | :--- |
| **目的** | **定義 (Definition)** | **聲明 (Declaration) & 實例化 (Instantiation)** |
| **核心問題** | 「一個資產**是什麼**？」 | 「**哪裡**需要**多少個**什麼樣的資產？」 |
| **內容** | 屬性、資料類型、行為 (方法)、事件、資料介面 | 實例數量、唯一 ID、具體位置、初始參數、實例間的連接關係 |
| **範疇** | 通用的、可重用的資產「類型」 | 特定場景的、具體的資產「實例」 |
| **比喻** | 汽車的設計圖紙 | 停車場的車輛清單與停放位置圖 |
| **產出** | 一個可被引用的資產模板 | 一系列在 NDH 中運行的活躍物件實體 |

這種關注點分離 (Separation of Concerns) 的設計，是整個 IDTF 架構能夠保持清晰和高效的基石。

---

## 3. 實例詳解：從「泵的定義」到「冷卻系統的部署」

讓我們透過一個具體的例子，來看看 IADL 和 FDL 如何協同工作。

### 3.1. IADL：定義一個標準泵 (Pump) 的藍圖

首先，我們在一個 IADL 檔案中定義一個名為 `StandardPump` 的資產類型。這個檔案定義了所有標準泵都應具備的屬性和行為。

**檔案**: `iadl_types/pump.yml`
```yaml
# IADL - 資產類型定義 (Asset Type Definition)
asset_types:
  - name: StandardPump
    description: "一個標準的離心泵，包含基本監控和控制功能"
    properties:
      - name: Status
        type: string
        enum: ["Stopped", "Running", "Error"]
        default: "Stopped"
      - name: FlowRate
        type: float
        unit: "m³/h"
        default: 0.0
      - name: Pressure
        type: float
        unit: "bar"
        default: 0.0
    behaviors:
      - name: start
        description: "啟動泵"
        params: []
        returns: { type: "boolean" }
      - name: stop
        description: "停止泵"
        params: []
        returns: { type: "boolean" }
```

### 3.2. FDL：部署一個包含多個泵的冷卻系統

接下來，我們使用 FDL 來設計一個具體的工廠佈局。在這個例子中，我們要在 `CoolingArea` 部署三台泵，其中兩台是主泵，一台是備用泵。

**檔案**: `fdl_layouts/cooling_system.yml`
```yaml
# FDL - 工廠設計與實例化 (Factory Design & Instantiation)
factory_layout:
  - area: CoolingArea
    description: "主冷卻循環區"
    instances:
      # 聲明需要實例化的資產
      - type: StandardPump  # 引用 IADL 中定義的類型
        count: 2
        naming_prefix: "Main_Pump_"
        initial_properties:
          # 可選：設定實例的初始狀態
          - asset_id: "Main_Pump_1"
            properties: { Status: "Stopped" }
          - asset_id: "Main_Pump_2"
            properties: { Status: "Stopped" }

      - type: StandardPump
        count: 1
        naming_prefix: "Backup_Pump"
        initial_properties:
          - asset_id: "Backup_Pump_1"
            properties: { Status: "Stopped" }
```

如上所示，FDL 檔案本身並不關心 `StandardPump` 的內部細節（如它有哪些屬性），它只關心「**用 `StandardPump` 這個藍圖，在 `CoolingArea` 創建 3 個實例**」。

---

## 4. NDH 實例化引擎：架構與工作流程

當一份 FDL 檔案被提交給 NDH 時，其內部的「實例化引擎」會被觸發。這個引擎負責將 FDL 的聲明轉化為活躍的 `AssetServant` 物件。

### 4.1. 工作流程圖

![FDL 實例化流程圖](../images/fdl-instantiation-flow.png)

1.  **FDL 處理器 (FDL Processor)**：一個新的或擴展的 NDH 連接器，專門負責讀取和剖析 FDL 檔案的語法。
2.  **IADL 類型註冊表 (IADL Type Registry)**：NDH 在啟動時會載入所有可用的 IADL 檔案，並將 `StandardPump` 這類的資產類型定義儲存在一個可快速查詢的註冊表中。
3.  **實例化迴圈 (Instantiation Loop)**：FDL 處理器遍歷 FDL 檔案中的 `instances` 列表。對於每一項聲明：
    - 它會從聲明中提取 `type`（例如 `StandardPump`）、`count`（例如 2）和 `naming_prefix`。
    - 它會拿 `type` 去 IADL 類型註冊表中查詢對應的資產藍圖。
    - 如果找到，它會根據 `count` 執行一個迴圈。
4.  **啟動物件 (AssetAdapter)**：在迴圈中，引擎會為每個即將創建的實例生成一個唯一的 `asset_id`（例如 `Main_Pump_1`），然後呼叫 `AssetAdapter` 來創建並啟動一個新的 `AssetServant` 實例。IADL 藍圖中的屬性和行為會被複製到這個新實例中。
5.  **註冊物件 (NamingService)**：新啟動的物件會被註冊到 `NamingService` 中，使其可以被系統的其他部分透過其 `asset_id` 找到。

### 4.2. 概念性實作

以下是一段 Python 偽代碼，展示了實例化引擎的核心邏輯：

```python
class InstantiationEngine:
    def __init__(self, iadl_registry, asset_adapter, naming_service):
        self.iadl_registry = iadl_registry
        self.asset_adapter = asset_adapter
        self.naming_service = naming_service

    async def process_fdl(self, fdl_data: dict):
        """處理剖析後的 FDL 資料"""
        for area in fdl_data.get("factory_layout", []):
            for instance_decl in area.get("instances", []):
                asset_type = instance_decl.get("type")
                count = instance_decl.get("count", 1)
                prefix = instance_decl.get("naming_prefix", asset_type)

                # 從註冊表查找 IADL 藍圖
                iadl_blueprint = self.iadl_registry.get_type(asset_type)
                if not iadl_blueprint:
                    logger.error(f"資產類型 {asset_type} 未在 IADL 中定義")
                    continue

                # 根據 count 進行實例化
                for i in range(1, count + 1):
                    asset_id = f"{prefix}{i}"
                    location = area.get("area", "default")

                    # 1. 創建 Servant 實例
                    servant = self._create_servant_from_blueprint(
                        asset_id, iadl_blueprint, location
                    )

                    # 2. 啟動物件
                    object_id = await self.asset_adapter.activate_object(servant)

                    # 3. 創建並註冊物件參考
                    asset_ref = await self.orb.create_asset_reference(asset_id, location)
                    await self.naming_service.bind(asset_id, asset_ref)

                    logger.info(f"成功創建資產實例: {asset_id} (ObjectID: {object_id})")

    def _create_servant_from_blueprint(self, asset_id, blueprint, location):
        # 根據藍圖創建 AssetServant 物件
        # 將藍圖中的 properties 和 behaviors 複製到 servant 實例中
        servant = AssetServant(asset_id, blueprint.name, location)
        servant.attributes = blueprint.get_default_properties()
        # ... 複製行為 ...
        return servant
```

---

## 5. 分離設計的優勢

將 IADL 和 FDL 分離帶來了巨大的架構優勢：

- **高度重用性**: 一個 IADL 資產類型（如 `StandardPump`）可以被無數個 FDL 檔案在不同的工廠佈局中重用。
- **大規模擴展性**: 部署數千個感測器只需在 FDL 中修改 `count` 值，而無需改動任何 IADL 定義。
- **關注點分離**: IADL 設計師可以專注於資產的內在邏輯，而 FDL 設計師（通常是工廠規劃師或系統整合商）可以專注於工廠的宏觀佈局。
- **靈活性與模擬**: 可以輕鬆地創建多個版本的 FDL 檔案來模擬不同的工廠佈局或生產線配置，而無需修改任何底層的資產模型。

---

## 6. 結論

FDL 的引入，是 IDTF 框架從一個純粹的「資料建模語言」演進為一個真正的「工業數位分身平台」的關鍵一步。它在 IADL 的「靜態定義」和 NDH 的「動態執行」之間建立了一座至關重要的橋樑。

透過 **IADL（定義藍圖）** 和 **FDL（聲明實例）** 的明確分離，NDH 實現了一個強大而靈活的範式：它允許企業以一種前所未有的、可擴展且可維護的方式，來設計、部署和管理其數位化工廠的完整生命週期。這不僅僅是一個技術上的創新，更是實現工業 4.0 願景的核心實踐。

---

## 7. 參考資料

[1] Lin, C. C. (2025). *idtf-v3: IDTF V3.1 完整技術白皮書*. GitHub. [https://github.com/chchlin1018/idtf-v3](https://github.com/chchlin1018/idtf-v3)

[2] Lin, C. C. (2025). *idtf-documentation: IDTF 完整文件、程式碼、範例和視覺化*. GitHub. [https://github.com/chchlin1018/idtf-documentation](https://github.com/chchlin1018/idtf-documentation)

[3] Lin, C. C. (2025). *NDH-Win-Test1: NDH 測試開發框架*. GitHub. [https://github.com/chchlin1018/NDH-Win-Test1](https://github.com/chchlin1018/NDH-Win-Test1)

