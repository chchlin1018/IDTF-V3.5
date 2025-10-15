# 技術白皮書：Factory Designer - 連結設計與執行的橋樑

**文件版本**: 1.0  
**發布日期**: 2025-10-12  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin)  
**參考資料**: [1, 2, 3, 4]

---

## 1. 執行摘要

在 IDTF 生態系統中，我們已經擁有了 **IADL** (資產藍圖語言)、**IADL Editor** (資產建模工具) 和 **NDH** (執行時資料中樞)。然而，從「設計一個資產」到「部署一整座工廠」之間，仍然存在一個關鍵的缺口：**如何將數百甚至數千個資產實例有組織地佈局在工廠中，並將這個佈局資訊傳遞給 NDH，讓 NDH 能夠自動生成和管理這些實例？**

**Factory Designer** 正是為了填補這個缺口而設計的工具。它是一個專注於**工廠級佈局設計**的應用程式，使用 **FDL (Factory Design Language)** 作為其核心語言。Factory Designer 的核心價值在於：它從 IADL Editor 接收資產藍圖，讓工程師在一個視覺化的環境中設計工廠佈局，然後生成 FDL 檔案並直接推送到 NDH，觸發大規模的資產實例化和部署。

本白皮書將詳細闡述 Factory Designer 的專案定位、其與 IADL Editor 和 NDH 的整合架構，以及其核心功能和技術實作建議。我們將展示 Factory Designer 如何成為連結「離線設計」與「線上執行」的關鍵橋樑，從而完成 IDTF 生態系統的最後一塊拼圖。

> **核心定位**：Factory Designer 是一個**工廠級佈局設計工具**，負責將 IADL 資產藍圖組織成完整的工廠配置 (FDL)，並與 NDH 直接連結，實現從設計到執行的無縫過渡。

---

## 2. 專案定位：設計與執行之間的橋樑

Factory Designer 在 IDTF 生態系統中的定位，可以用以下的工作流程圖清晰地展示：

![Factory Designer 工作流程圖](../images/factory-designer-workflow.png)

### 2.1. 生態系統中的角色定位

| 工具 | 核心職責 | 輸入 | 輸出 | 階段 |
| :--- | :--- | :--- | :--- | :--- |
| **IADL Editor** | 資產建模與格式轉換 | CAD/BIM/PLM 資料 | IADL 檔案 (資產藍圖) | 設計階段 (離線) |
| **Factory Designer** | 工廠佈局設計與實例化配置 | IADL 檔案 (資產藍圖) | FDL 檔案 (工廠佈局) | 設計階段 (離線) |
| **NDH** | 資產實例生成與即時資料交換 | FDL 檔案 (工廠佈局) | 活躍的資產物件實例 | 執行階段 (線上) |

Factory Designer 是唯一一個同時「向後連結 IADL Editor」和「向前連結 NDH」的工具。它承上啟下，將靜態的資產定義轉化為動態的工廠配置。

### 2.2. 為什麼需要 Factory Designer？

在沒有 Factory Designer 的情況下，工程師需要手動編寫 FDL 檔案。這對於一個包含數千個感測器、執行器和設備的大型工廠來說，是一項極其繁瑣且容易出錯的工作。Factory Designer 提供了以下關鍵價值：

**視覺化設計**：透過拖放式的圖形介面，工程師可以直觀地在 2D 或 3D 平面上佈局資產，而不是手動編寫 YAML/JSON。

**自動化配置生成**：根據視覺化的佈局，自動生成符合 FDL 規範的配置檔案，包括資產的唯一 ID、位置資訊和初始參數。

**與 NDH 的即時連結**：提供「一鍵部署」功能，將 FDL 檔案直接推送到 NDH 的 API，並即時查看實例化的狀態和結果。

**版本控制與協作**：支援工廠佈局的版本管理，讓團隊可以協作設計，並追蹤變更歷史。

---

## 3. 核心功能設計

### 3.1. 資產庫管理 (Asset Library Management)

Factory Designer 的第一個核心功能是**資產庫管理**。它需要能夠從 IADL Editor 匯入或直接讀取 IADL 檔案，並將這些資產藍圖組織成一個可搜尋、可瀏覽的資產庫。

**功能細節**：

- **匯入 IADL 檔案**：支援批次匯入多個 IADL 檔案，自動解析其中的資產類型定義。
- **資產分類與標籤**：根據資產類型 (Equipment, Sensor, Actuator 等) 自動分類，並支援使用者自訂標籤。
- **資產預覽**：顯示每個資產的關鍵屬性、3D 模型縮圖 (如果有) 和簡要描述。
- **搜尋與篩選**：提供強大的搜尋功能，讓使用者快速找到所需的資產類型。

### 3.2. 工廠佈局設計器 (Factory Layout Designer)

這是 Factory Designer 的核心介面，提供視覺化的工廠佈局設計環境。

**2D 佈局模式**：

- 提供一個類似 CAD 軟體的 2D 平面圖編輯器。
- 使用者可以從資產庫中拖放資產到平面圖上。
- 支援定義區域 (Area)、樓層 (Floor) 和建築物 (Building) 的階層結構。
- 自動生成資產的唯一 ID（例如，`Area1_Pump_001`）。

**3D 佈局模式 (進階功能)**：

- 整合 Three.js 或 Babylon.js 等 WebGL 引擎，提供 3D 視覺化。
- 支援從 BIM 軟體 (如 Revit) 匯入工廠建築的 3D 模型作為底圖。
- 在 3D 空間中精確定位資產的位置 (x, y, z 座標)。

**連接與關係定義**：

- 支援在資產之間繪製連接線，定義管線、電纜或邏輯連接。
- 自動將這些連接關係寫入 FDL 的 `relationships` 部分。

### 3.3. FDL 生成與編輯器 (FDL Generator & Editor)

Factory Designer 會根據視覺化的佈局，自動生成 FDL 檔案。

**自動生成邏輯**：

```yaml
# 自動生成的 FDL 範例
factory_layout:
  - area: ProductionFloor1
    building: MainBuilding
    floor: 1
    instances:
      - type: StandardPump          # 從 IADL 資產庫引用
        count: 5                    # 使用者在佈局中放置了 5 個
        naming_prefix: "Pump_"
        location: "x:10, y:20, z:0" # 從 2D/3D 佈局中提取
        initial_params:
          power: 15
          status: "Stopped"
      - type: TemperatureSensor
        count: 20
        naming_prefix: "TempSensor_"
```

**內建 FDL 編輯器**：

- 提供語法高亮的文字編輯器，讓進階使用者可以手動微調 FDL。
- 支援 FDL 語法驗證，即時提示錯誤。
- 雙向同步：手動修改 FDL 後，視覺化佈局也會相應更新。

### 3.4. NDH 整合與部署 (NDH Integration & Deployment)

Factory Designer 的最大創新在於其與 NDH 的**直接連結**。

**NDH 連線配置**：

- 在設定中輸入 NDH 的 API 端點 (例如 `http://ndh.factory.com:8000`)。
- 支援 API 金鑰或 OAuth 2.0 認證。

**一鍵部署**：

- 點擊「部署到 NDH」按鈕，Factory Designer 會：
  1. 驗證 FDL 檔案的正確性。
  2. 透過 HTTP POST 請求，將 FDL 檔案推送到 NDH 的 `/api/deploy` 端點。
  3. NDH 接收 FDL 後，觸發實例化引擎，開始創建資產物件實例。

**即時狀態監控**：

- 透過 WebSocket 連線，即時接收 NDH 的部署進度和狀態更新。
- 顯示每個資產實例的創建狀態 (Pending, Active, Error)。
- 如果部署失敗，顯示詳細的錯誤訊息和建議的修復方法。

**回饋與迭代**：

- 部署完成後，可以從 NDH 查詢實際運行中的資產實例資料。
- 支援「反向同步」：將 NDH 中的實際配置拉回 Factory Designer，用於故障排查或版本比對。

---

## 4. 技術架構建議

### 4.1. 技術棧選擇

**前端 (推薦 Web 應用)**：

- **框架**: React 或 Vue.js (提供響應式的使用者介面)
- **2D 繪圖**: Konva.js 或 Fabric.js (Canvas 操作)
- **3D 視覺化**: Three.js 或 Babylon.js (WebGL 渲染)
- **狀態管理**: Redux 或 Vuex
- **UI 組件庫**: Ant Design 或 Material-UI

**後端 (可選，用於複雜邏輯)**：

- **框架**: Node.js (Express.js) 或 Python (FastAPI)
- **用途**: FDL 驗證、版本控制、使用者管理

**資料儲存**：

- **本地儲存**: IndexedDB (瀏覽器端) 或 SQLite (桌面應用)
- **雲端儲存**: PostgreSQL + S3 (用於團隊協作)

**與 NDH 通訊**：

- **協議**: RESTful API (部署) + WebSocket (即時狀態)
- **資料格式**: JSON (FDL 檔案以 JSON 格式傳輸)

### 4.2. 核心模組設計

**AssetLibraryModule**：
- 負責 IADL 檔案的解析和資產庫的管理。

**LayoutDesignerModule**：
- 提供 2D/3D 佈局編輯器的核心邏輯。

**FDLGeneratorModule**：
- 根據佈局資料自動生成 FDL 檔案。

**NDHClientModule**：
- 封裝與 NDH API 的所有互動邏輯。

**VersionControlModule**：
- 整合 Git 或自訂的版本控制系統，管理工廠佈局的歷史版本。

---

## 5. 使用場景與工作流程

### 5.1. 典型工作流程

**步驟 1：匯入資產藍圖**
- 工程師從 IADL Editor 匯出 IADL 檔案。
- 在 Factory Designer 中匯入這些 IADL 檔案到資產庫。

**步驟 2：設計工廠佈局**
- 在 2D/3D 佈局設計器中，拖放資產到工廠平面圖上。
- 定義區域、樓層和建築物的階層結構。
- 繪製資產之間的連接關係。

**步驟 3：生成 FDL 配置**
- Factory Designer 自動生成 FDL 檔案。
- 工程師可以在內建編輯器中檢視和微調 FDL。

**步驟 4：部署到 NDH**
- 點擊「部署到 NDH」按鈕。
- Factory Designer 將 FDL 推送到 NDH 的 API。
- NDH 接收 FDL 並開始實例化資產物件。

**步驟 5：監控與驗證**
- 透過 Factory Designer 的監控面板，即時查看部署進度。
- 驗證所有資產實例都已成功創建並處於活躍狀態。

**步驟 6：迭代與優化**
- 根據實際運行情況，在 Factory Designer 中調整佈局。
- 重新部署更新後的 FDL 到 NDH。

### 5.2. 進階場景

**多廠部署**：
- 使用同一套資產藍圖，在 Factory Designer 中設計多個工廠的佈局。
- 分別生成不同的 FDL 檔案，部署到不同地點的 NDH 實例。

**虛擬調試**：
- 在部署到實體工廠之前，先部署到一個模擬的 NDH 環境。
- 進行虛擬調試，驗證控制邏輯和資料流程。

**災難恢復**：
- 當實體工廠發生故障時，可以從 Factory Designer 的版本庫中快速恢復之前的工廠配置。

---

## 6. 與現有生態系統的整合

### 6.1. 與 IADL Editor 的整合

**資料流向**：IADL Editor → IADL 檔案 → Factory Designer

**整合方式**：
- **檔案匯入**: Factory Designer 提供「從 IADL Editor 匯入」功能，支援批次選擇 IADL 檔案。
- **API 整合 (進階)**: IADL Editor 可以提供一個「推送到 Factory Designer」的功能，直接透過 API 將 IADL 檔案傳送到 Factory Designer 的資產庫。

### 6.2. 與 NDH 的整合

**資料流向**：Factory Designer → FDL 檔案 → NDH

**整合方式**：
- **RESTful API**: NDH 提供一個 `/api/deploy` 端點，接收 FDL 檔案 (JSON 格式)。
- **WebSocket**: NDH 提供一個 WebSocket 端點，用於推送部署進度和狀態更新。
- **認證與授權**: 使用 API 金鑰或 OAuth 2.0 確保只有授權的 Factory Designer 實例可以部署到 NDH。

### 6.3. 與 BIM/CAD 系統的整合 (可選)

**資料流向**：Revit/AVEVA E3D → 3D 模型 → Factory Designer

**整合方式**：
- Factory Designer 可以匯入 IFC 或 USD 格式的 3D 建築模型，作為佈局設計的底圖。
- 這使得工程師可以在真實的工廠建築模型上精確定位資產。

---

## 7. 結論：完成生態系統的閉環

Factory Designer 的加入，標誌著 IDTF 生態系統從「概念」到「完整可用」的關鍵躍進。它填補了從「資產建模」到「工廠部署」之間的最後一個缺口，實現了真正的端到端工作流程：

**IADL Editor** (資產建模) → **Factory Designer** (工廠佈局) → **NDH** (執行與監控) → **實體工廠** (物理世界)

其核心貢獻在於：

1.  **視覺化設計**：將複雜的 FDL 編寫工作轉化為直觀的拖放操作。
2.  **自動化部署**：一鍵將設計推送到執行環境，極大地縮短了從設計到上線的時間。
3.  **即時回饋**：與 NDH 的直接連結，讓工程師可以即時驗證設計的正確性。
4.  **生態閉環**：作為連結離線設計與線上執行的橋樑，完成了整個 IDTF 生態系統的閉環。

總而言之，Factory Designer 不僅是一個工具，更是 IDTF 願景得以完整實現的關鍵基石。它讓「一次建模，到處使用」的理想，真正成為每一位工業工程師觸手可及的現實。

---

## 8. 參考資料

[1] Lin, C. C. (2025). *idtf-v3: IDTF V3.1 完整技術白皮書*. GitHub. [https://github.com/chchlin1018/idtf-v3](https://github.com/chchlin1018/idtf-v3)

[2] Lin, C. C. (2025). *idtf-documentation: IDTF 完整文件、程式碼、範例和視覺化*. GitHub. [https://github.com/chchlin1018/idtf-documentation](https://github.com/chchlin1018/idtf-documentation)

[3] Lin, C. C. (2025). *NDH-Win-Test1: NDH 測試開發框架*. GitHub. [https://github.com/chchlin1018/NDH-Win-Test1](https://github.com/chchlin1018/NDH-Win-Test1)

[4] Lin, C. C. (2025). *IADL-Editor-Test1: IADL 編輯器測試專案*. GitHub. [https://github.com/chchlin1018/IADL-Editor-Test1](https://github.com/chchlin1018/IADL-Editor-Test1)

