# 技術白皮書：IADL Editor - 串連數位分身生命週期的桌面工具

**文件版本**: 1.0  
**發布日期**: 2025-10-12  
**作者**: Manus AI  
**參考資料**: [1, 2, 3, 4]

---

## 1. 執行摘要

在 IADL 生態系統中，如果說 IADL 是「通用語言」，NDH 是「執行引擎」，那麼 **IADL Editor** 就是串連起整個數位分身生命週期的「**整合開發環境 (IDE)**」。它是一個功能強大的桌面應用程式，專為工業資產的建模、配置和管理而設計。IADL Editor 的核心價值在於其強大的**插件架構**，使其能夠與上下游超過 12 種主流工業軟體進行無縫的資料交換，從而打通從設計、製造到營運的完整數位線程。

本白皮書將深入探討 IADL Editor 的專案定位、其基於 Qt/C++ 的技術架構，以及其革命性的插件系統。我們將展示 IADL Editor 如何作為一個「離線配置中心」，匯入來自 CAD/BIM/PLM 系統的設計資料，並匯出為 NDH 的執行時配置、3D 視覺化平台的模型以及其他分析工具所需的資料格式，從而成為 IADL 生態中不可或缺的一環。

> **核心定位**：IADL Editor 是一個**離線的、以插件為中心**的桌面工具，作為 IADL 生態系統的**資料中樞和配置中心**，負責在設計階段 (Design-Time) 完成資產的建模與整合配置。

---

## 2. 專案定位：離線的整合開發環境 (IDE)

IADL Editor 與 NDH 的關係，類似於軟體開發中的 IDE (如 Visual Studio Code) 與伺服器 (如 Node.js) 的關係。IADL Editor 是一個在工程師桌面執行的**離線工具**，主要用於**設計階段**的工作。

| 特性 | IADL Editor (設計階段) | NDH (執行階段) |
| :--- | :--- | :--- |
| **類型** | 桌面應用程式 (Qt/C++) | 伺服器端服務 (Python/Node.js) |
| **核心職責** | **建模與配置**：定義資產、轉換格式、生成配置 | **執行與交換**：即時資料交換、系統連接、事件處理 |
| **資料狀態** | 靜態的、離線的 | 動態的、即時的 |
| **使用者** | 系統工程師、自動化工程師 | 系統管理員、DevOps 工程師 |

這種明確的職責劃分，使得整個生態系統架構清晰、易於維護。工程師可以在 IADL Editor 中完成所有複雜的建模和配置工作，然後將生成的設定檔「部署」到 NDH 服務中執行。

---

## 3. 核心架構：基於 Qt/C++ 的插件式系統

IADL Editor 採用了成熟穩定的 **Qt 6** 和 **C++17** 技術棧，確保了其跨平台能力 (Windows, macOS, Linux) 和高效性能。其架構的核心是**插件模式 (Plugin Pattern)**。

![IADL Editor 插件架構圖](../images/iadl-editor-plugins.png)

### 3.1. 插件管理器 (Plugin Manager)

`ImportExportManager` 是整個插件系統的核心。它負責：

- **動態載入**：在應用程式啟動時，自動掃描 `plugins` 目錄，載入所有可用的插件 (動態連結函式庫, .dll 或 .so)。
- **介面統一**：所有插件都必須實作 `ImportExportInterface` 介面，該介面定義了 `importData()` 和 `exportData()` 等標準方法。
- **工廠模式**：當使用者選擇匯入或匯出時，管理器會根據目標系統類型，像工廠一樣「生產」出對應的插件實例來處理任務。

### 3.2. 豐富的插件生態

`IADL-Editor-Test1` 專案展示了其強大的生態整合能力，內建了對超過 12 種主流工業系統和格式的支援 [4]。

**匯入插件 (Upstream)**：
- **CAD/BIM**: 從 Autodesk Revit, AVEVA E3D 等軟體匯入建築和工廠模型。
- **SCADA/DCS**: 從 Wonderware, Ignition, DeltaV, GE iFIX 等系統匯入標籤資料庫和控制邏輯。
- **PLM**: 從 PTC Windchill 等系統匯入產品生命週期和供應商資訊。

**匯出插件 (Downstream)**：
- **NDH 配置**: 將 IADL 模型轉換為 NDH 可執行的 JSON/YAML 設定檔。
- **3D 視覺化**: 匯出為 NVIDIA Omniverse (USD 格式)，用於建立視覺化的數位分身場景。
- **電氣分析**: 匯出為 ETAP 格式，用於電力系統的模擬和分析。
- **通用格式**: 支援匯出為通用的 JSON, XML, CSV 等格式。

### 3.3. 插件開發指南

為了鼓勵社群貢獻，專案還提供了一份極其詳盡的《插件開發指南》(`PLUGIN_DEVELOPMENT_GUIDE.md`) [4]。該指南包含：

- **7 個明確的開發步驟**：從建立專案到最終部署的完整流程。
- **完整的 API 參考**：詳細解釋 `ImportExportInterface` 的所有方法。
- **Modbus 插件完整範例**：提供一個約 680 行的完整範例程式碼，作為開發新插件的起點。
- **8 個最佳實踐**：涵蓋錯誤處理、進度回報、資料驗證等方面。

這種開放、易於擴展的架構是 IADL Editor 能夠成為生態系統中心的關鍵。

---

## 4. IADL 語法與編輯器功能

除了插件系統，IADL Editor 本身也提供了強大的 IADL 建模功能。

### 4.1. 雙語語法手冊

專案內建了中英雙語的《IADL 語法手冊》(`IADL_SYNTAX_MANUAL.md`) [4]，詳細定義了 IADL 的所有語法細節，包括：

- **7 種核心資產類型**：Equipment, Device, Sensor, Actuator, Controller, System, Network。
- **5 大類常用屬性**：物理屬性、操作參數、維護資訊、元數據、IOT 參數。
- **3 種關係類型**：父子關係 (contains), 連接關係 (connectedTo), 邏輯關係 (controls)。

### 4.2. 核心編輯器功能

- **語法高亮**：`IADLSyntaxHighlighter` 類別提供了對 IADL 關鍵字和結構的顏色標記，提高了可讀性。
- **資產瀏覽器**：以樹狀結構顯示資產的階層關係，方便使用者導覽。
- **視覺化工具**：`AssetVisualizer` 可以將資產間的連接和邏輯關係以圖形化方式展示出來。
- **模型驗證**：提供基本的語法和語意驗證，確保模型的正確性。

---

## 5. 結論：設計階段的賦能者

IADL Editor 在整個 IDTF 框架中扮演著至關重要的「賦能者」角色。它將 IADL 從一個抽象的語言規範，變成了一個可以在實際工作中被高效創建、管理和應用的工程工具。

其核心貢獻在於：

1.  **降低建模門檻**：透過圖形化介面和語法高亮，使工程師可以輕鬆上手 IADL。
2.  **打通資料孤島**：強大的插件系統連接了設計、製造和營運的各個環節，實現了真正的數位線程。
3.  **賦能 NDH**：作為 NDH 的「配置中心」，為其提供了執行所需的完整、準確的配置資訊。
4.  **促進生態開放**：清晰的插件開發指南降低了社群貢獻的門檻，有助於生態系統的快速擴張。

總而言之，IADL Editor 是將 IADL 宏大願景落地的關鍵實踐，是每一位致力於建構數位分身的工程師不可或缺的工具。

---

## 6. 參考資料

[1] Lin, C. C. (2025). *idtf-v3: IDTF V3.1 完整技術白皮書*. GitHub. [https://github.com/chchlin1018/idtf-v3](https://github.com/chchlin1018/idtf-v3)

[2] Lin, C. C. (2025). *idtf-documentation: IDTF 完整文件、程式碼、範例和視覺化*. GitHub. [https://github.com/chchlin1018/idtf-documentation](https://github.com/chchlin1018/idtf-documentation)

[3] Lin, C. C. (2025). *NDH-Win-Test1: NDH 測試開發框架*. GitHub. [https://github.com/chchlin1018/NDH-Win-Test1](https://github.com/chchlin1018/NDH-Win-Test1)

[4] Lin, C. C. (2025). *IADL-Editor-Test1: IADL 編輯器測試專案*. GitHub. [https://github.com/chchlin1018/IADL-Editor-Test1](https://github.com/chchlin1018/IADL-Editor-Test1)

