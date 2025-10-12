# IADL Editor 匯入/匯出功能

## 概述

IADL Editor 現在支援多種工業系統的資料匯入和匯出功能,讓使用者能夠在不同系統之間轉換和共享資產資訊。

## 支援的系統

### 1. AVEVA PI System
- **檔案格式**: CSV
- **功能**: 匯出資產資料到 AVEVA PI System 格式
- **用途**: 與 AVEVA PI 資料歷史系統整合

### 2. JSON
- **檔案格式**: JSON
- **功能**: 匯入/匯出資產資料
- **用途**: 通用資料交換格式,適合與其他系統整合

### 3. BIM (IFC)
- **檔案格式**: IFC, IFCXML, IFCZIP
- **功能**: 匯入/匯出建築資訊模型資料
- **用途**: 與 BIM 工具(如 Revit, ArchiCAD)整合
- **特色**:
  - 支援設備實體解析
  - 保留幾何資訊
  - 支援屬性和關係資訊

### 4. NVIDIA Omniverse (USD)
- **檔案格式**: USD, USDA, USDC
- **功能**: 匯入/匯出 Universal Scene Description 格式
- **用途**: 與 NVIDIA Omniverse 平台整合,支援 3D 視覺化和模擬
- **特色**:
  - 支援自定義屬性
  - 可選物理屬性
  - 支援視覺化資訊

### 5. ETAP
- **檔案格式**: ETAP, XML
- **功能**: 匯入/匯出電力系統分析資料
- **用途**: 與 ETAP 電力系統分析軟體整合
- **特色**:
  - 自動識別電力設備
  - 支援潮流分析配置
  - 支援短路分析配置
  - 支援保護設備配置

## 使用方式

### 匯入資產

1. 點選選單 **檔案 > 匯入資產**
2. 選擇要匯入的檔案
3. 系統會自動識別檔案格式並載入相應的匯入插件
4. 匯入完成後,資產會顯示在資產樹狀結構中

### 匯出資產

1. 點選選單 **檔案 > 匯出資產**
2. 選擇要匯出的目標系統
3. 設定匯出選項(如果有)
4. 選擇儲存位置並確認匯出

## 匯出選項

### BIM (IFC) 匯出選項
- **IFC 版本**: IFC2X3, IFC4
- **包含幾何資訊**: 是/否
- **包含屬性**: 是/否

### Omniverse (USD) 匯出選項
- **包含視覺化資訊**: 是/否
- **包含物理屬性**: 是/否

### ETAP 匯出選項
- **包含潮流分析**: 是/否
- **包含短路分析**: 是/否
- **包含保護設備**: 是/否

## 插件架構

匯入/匯出功能採用插件架構設計,方便未來擴展新的系統支援。

### 核心元件

1. **ImportExportInterface**: 定義匯入/匯出插件的介面
2. **ImportExportManager**: 管理所有匯入/匯出插件
3. **ImportExportDialog**: 提供使用者介面

### 新增自訂插件

要新增新的匯入/匯出插件,需要:

1. 繼承 `ImportExportInterface` 介面
2. 實作以下方法:
   - `importAssets()`: 匯入資產
   - `exportAssets()`: 匯出資產
   - `supportedFileExtensions()`: 支援的檔案副檔名
   - `systemType()`: 系統類型
   - `systemName()`: 系統名稱
   - `systemDescription()`: 系統描述
   - `validateFile()`: 驗證檔案

3. 在 `ImportExportManager` 中註冊新插件

## 資料結構

### AssetData

```cpp
struct AssetData {
    QString id;                      // 資產 ID
    QString name;                    // 資產名稱
    QString type;                    // 資產類型
    QMap<QString, QVariant> properties;  // 資產屬性
};
```

## 技術細節

- **框架**: Qt 6
- **語言**: C++17
- **設計模式**: 插件模式
- **檔案處理**: QFile, QXmlStreamReader/Writer, QJsonDocument

## 已知限制

1. BIM (IFC) 匯入/匯出功能使用簡化的解析器,實際應用中建議使用專門的 IFC SDK
2. Omniverse (USD) 匯入/匯出功能不包含完整的幾何資訊,僅支援基本的場景結構
3. ETAP 匯出僅支援基本的電力設備,不支援複雜的網路拓撲

## 未來改進

- [ ] 支援更多工業系統(如 OSIsoft PI, Honeywell, Siemens)
- [ ] 改進 IFC 解析器,支援更複雜的 BIM 模型
- [ ] 新增批次匯入/匯出功能
- [ ] 支援資料映射和轉換規則
- [ ] 新增匯入/匯出進度顯示
- [ ] 支援增量匯入/匯出

## 授權

本專案採用 MIT 授權。詳見 LICENSE 檔案。

## 貢獻

歡迎提交 Issue 和 Pull Request 來改進匯入/匯出功能。

## 聯絡方式

如有問題或建議,請透過 GitHub Issues 聯絡我們。


### 6. AVEVA InTouch
- **檔案格式**: CSV, TXT
- **功能**: 匯入/匯出 AVEVA InTouch 標籤資料庫
- **用途**: 與 AVEVA InTouch SCADA 系統整合

### 7. AVEVA System Platform
- **檔案格式**: XML, aaPlatform
- **功能**: 匯入/匯出 AVEVA System Platform 物件模型
- **用途**: 與 AVEVA System Platform 整合

### 8. AVEVA EDGE
- **檔案格式**: XML, CSV, TXT
- **功能**: 匯入/匯出 AVEVA EDGE 標籤和畫面資料
- **用途**: 與 AVEVA EDGE (InduSoft Web Studio) 整合

### 9. AVEVA Historian
- **檔案格式**: CSV, XML, TXT
- **功能**: 匯入/匯出 AVEVA Historian 標籤和歷史資料
- **用途**: 與 AVEVA Historian 資料歷史系統整合



### 10. Ignition
- **檔案格式**: JSON, XML, CSV, TXT
- **功能**: 匯入/匯出 Ignition 標籤和資源資料
- **用途**: 與 Ignition SCADA/MES/IIoT 平台整合



### 11. Emerson DeltaV
- **檔案格式**: XML, CSV, TXT
- **功能**: 匯入/匯出 Emerson DeltaV DCS 模組和參數
- **用途**: 與 Emerson DeltaV 分散式控制系統整合

### 12. GE iFIX
- **檔案格式**: CSV, XML, TXT
- **功能**: 匯入/匯出 GE iFIX SCADA 資料庫區塊和標籤
- **用途**: 與 GE iFIX HMI/SCADA 系統整合

