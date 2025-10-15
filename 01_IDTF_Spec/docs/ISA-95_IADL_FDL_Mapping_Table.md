# ISA-95 與 IADL/FDL 正式對映表

**版本**: 1.0
**日期**: 2025-10-15
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)

---

## 1. 概述

本文件旨在提供 IDTF (Industrial Digital Twin Framework) 中 IADL (Industrial Asset Definition Language) 和 FDL (Factory Design Language) 與 ISA-95 標準之間詳細的正式對映關係。ISA-95 (Enterprise-Control System Integration) 是製造業中廣泛採用的標準，用於定義企業與控制系統之間的介面，特別是其設備階層模型 (Equipment Hierarchy Model) 對於資產的組織和管理至關重要。透過建立清晰的對映表，IDTF 旨在確保其數位分身模型與行業標準的高度一致性、互操作性及可驗證性。

本對映表將涵蓋 ISA-95 的設備階層元素、資產屬性以及與 ERP/MES 事件相關的狀態轉換，以提供一個全面的參考指南。

## 2. ISA-95 設備階層與 IDTF 資產模型對映

ISA-95 定義了從企業層到控制層的設備階層，通常包括企業 (Enterprise)、站點 (Site)、區域 (Area)、生產單元 (Process Cell)、工作單元 (Unit)、設備 (Equipment Module) 和控制模組 (Control Module)。IDTF 的 IADL 和 FDL 模型旨在靈活地映射這些階層。

| ISA-95 設備階層元素 | 描述 | IDTF 對映概念 (IADL/FDL) | 說明 |
|:---|:---|:---|:---|
| **Enterprise** | 整個企業組織，包含多個站點。 | **FDL Factory** (頂層) | FDL 的頂層 `Factory` 或 `Enterprise` 實例，代表整個企業的數位分身。 |
| **Site** | 企業內的一個地理位置或主要生產設施。 | **FDL Site** | FDL 中的 `Site` 實例，代表一個物理廠區或設施。 |
| **Area** | 站點內的一個邏輯或物理分區，例如一個車間或生產線。 | **FDL Area** | FDL 中的 `Area` 實例，代表廠區內的特定區域或生產線。 |
| **Process Cell** | 執行主要生產過程的單元，通常包含多個工作單元。 | **FDL ProcessCell** | FDL 中的 `ProcessCell` 實例，代表一個完整的生產流程單元。 |
| **Unit** | 執行特定生產操作的設備集合。 | **FDL Unit** | FDL 中的 `Unit` 實例，代表一個獨立的工作單元或設備組。 |
| **Equipment Module** | 執行特定控制功能的設備或設備組件。 | **IADL AssetType** / **FDL AssetInstance** | IADL 定義的資產類型或 FDL 實例化的具體設備。這是 IDTF 中最細粒度的可獨立管理的資產。 |
| **Control Module** | 執行基本控制功能的設備組件，如感測器、執行器。 | **IADL ComponentType** / **FDL ComponentInstance** | IADL 定義的組件類型或 FDL 實例化的具體組件。通常是 `AssetType` 的組成部分。 |

## 3. ISA-95 資產屬性與 IADL/FDL 屬性對映

ISA-95 定義了資產的各種屬性，這些屬性在 IDTF 的 IADL/FDL 中有直接或擴展的對映。

| ISA-95 屬性類別 | 典型 ISA-95 屬性 | IDTF 對映路徑 (IADL/FDL) | 說明 |
|:---|:---|:---|:---|
| **基本識別** | ID, Name, Description | `asset_type.id`, `asset_type.name`, `asset_type.description` | 資產的唯一識別符、名稱和描述。 |
| **物理屬性** | Manufacturer, Model, SerialNumber, InstallationDate | `asset_type.vendor_info.manufacturer.name`, `asset_type.model`, `asset_instance.serial_number`, `asset_instance.installation_date` | 製造商、型號、序號和安裝日期。 |
| **工程屬性** | DesignPressure, Material, Capacity | `asset_type.engineering_attributes.design_pressure`, `asset_type.engineering_attributes.material`, `asset_type.engineering_attributes.capacity` | 設計壓力、材料、容量等工程設計參數。 |
| **操作屬性** | OperatingMode, Setpoint, Status | `asset_instance.operational_attributes.operating_mode`, `asset_instance.operational_attributes.setpoint`, `asset_instance.operational_status` | 操作模式、設定點、即時操作狀態。 |
| **維護屬性** | MaintenanceSchedule, LastMaintenanceDate, MTBF | `asset_type.maintenance_attributes.schedule`, `asset_instance.maintenance_records.last_date`, `asset_type.maintenance_attributes.mtbf` | 維護計畫、上次維護日期、平均無故障時間。 |
| **財務屬性** | PurchaseCost, Depreciation, AssetAccountID | `asset_type.financial_attributes.purchase_cost`, `asset_type.financial_attributes.depreciation_info`, `asset_type.financial_attributes.asset_account_id` | 採購成本、折舊資訊、資產會計編號。 |
| **安全屬性** | SafetyRating, HazardLevel | `asset_type.safety_attributes.rating`, `asset_type.safety_attributes.hazard_level` | 安全等級、危險等級。 |
| **品質屬性** | QualityStandard, Tolerance | `asset_type.quality_standards`, `asset_type.quality_standards.tolerance` | 品質標準、公差。 |

## 4. ISA-95 狀態轉換與 ERP/MES 事件對映

資產的生命週期狀態轉換是動態的，通常由外部系統（如 ERP、MES）中的事件觸發。IDTF 的生命週期管理模型與這些事件緊密結合。

| ISA-95 相關狀態/事件 | 描述 | IDTF 資產實例生命週期狀態 | 觸發 ERP/MES 事件 |
|:---|:---|:---|:---|
| **設計完成** | 資產設計已批准並準備採購。 | `Design_Approved` | CAD/PLM 系統中的設計發布事件。 |
| **採購訂單創建** | 在 ERP 中為資產創建採購訂單。 | `Ordered` | ERP 系統中的採購訂單創建事件。 |
| **資產接收** | 資產已送達並驗收。 | `Received` | ERP 系統中的收貨確認事件。 |
| **安裝中** | 資產正在進行物理安裝。 | `Installed` | 維護管理系統 (CMMS) 或 MES 中的安裝工單啟動事件。 |
| **調試中** | 資產正在進行功能測試和調試。 | `Commissioning` | MES 或控制系統中的調試流程啟動事件。 |
| **生產運行** | 資產已投入正常生產運行。 | `Running` | MES 或控制系統中的設備啟動/生產開始事件。 |
| **停機/故障** | 資產因故障或計畫維護而停止運行。 | `Stopped` / `Faulted` | MES 或控制系統中的設備停機/故障警報事件。 |
| **維護中** | 資產正在進行維護活動。 | `Under_Maintenance` | CMMS 或 MES 中的維護工單啟動事件。 |
| **退役** | 資產已從生產中移除並準備報廢。 | `Decommissioned` | ERP 或 CMMS 中的資產報廢流程啟動事件。 |

## 5. 結論

本 ISA-95 與 IADL/FDL 正式對映表為 IDTF 的實施者和使用者提供了一個清晰的框架，以理解和驗證 IDTF 如何與主流工業標準對齊。透過精確的階層、屬性和狀態對映，IDTF 能夠更好地支援跨系統的數據互操作性，並為構建符合行業最佳實踐的數位分身提供堅實的基礎。這將有助於台灣高科技工廠在數位轉型過程中，更有效地利用 IDTF 平台，實現數據驅動的營運優化和決策。
