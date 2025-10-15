# IADL Editor 擴充插件開發指南

## 目錄

1. [簡介](#簡介)
2. [開發環境設定](#開發環境設定)
3. [插件架構概述](#插件架構概述)
4. [開發步驟](#開發步驟)
5. [API 參考](#api-參考)
6. [完整範例](#完整範例)
7. [最佳實踐](#最佳實踐)
8. [測試和除錯](#測試和除錯)
9. [部署和發布](#部署和發布)
10. [常見問題](#常見問題)

---

## 簡介

IADL Editor 採用插件架構設計,允許開發者輕鬆擴展系統功能,支援新的工業自動化系統和檔案格式。本指南將詳細說明如何開發自訂的匯入/匯出插件,並提供完整的範例程式碼。

### 為什麼需要插件?

工業自動化領域有數百種不同的系統和標準,每個系統都有其特定的資料格式和結構。透過插件架構,IADL Editor 可以:

- **靈活擴展**: 輕鬆新增對新系統的支援,無需修改核心程式碼
- **模組化設計**: 每個插件獨立開發和維護,降低系統複雜度
- **社群貢獻**: 允許社群開發者貢獻新的插件,豐富系統功能
- **客製化需求**: 企業可以開發專屬的插件,滿足特定需求

### 插件可以做什麼?

目前 IADL Editor 的插件系統主要支援:

- **資料匯入**: 從外部系統匯入資產資料到 IADL Editor
- **資料匯出**: 將 IADL Editor 中的資產資料匯出到外部系統
- **格式轉換**: 在不同系統格式之間進行資料轉換
- **資料驗證**: 驗證匯入/匯出資料的正確性和完整性

---

## 開發環境設定

### 必要工具

開發 IADL Editor 插件需要以下工具:

1. **Qt 6 開發環境**
   - Qt Creator (建議使用最新版本)
   - Qt 6.2 或更高版本
   - 包含 Qt Widgets、Qt Core、Qt XML 模組

2. **C++ 編譯器**
   - Linux: GCC 11.0 或更高版本
   - Windows: MSVC 2019 或更高版本
   - macOS: Clang 12.0 或更高版本

3. **建置工具**
   - qmake 或 CMake
   - make (Linux/macOS) 或 nmake (Windows)

4. **版本控制**
   - Git (建議)

5. **開發工具**
   - 文字編輯器或 IDE (Qt Creator、Visual Studio Code、CLion 等)
   - 除錯工具 (gdb、lldb 或 Visual Studio Debugger)

### 環境安裝

#### Linux (Ubuntu/Debian)

```bash
# 安裝 Qt 6 開發套件
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-tools-dev qt6-tools-dev-tools

# 安裝建置工具
sudo apt-get install build-essential cmake git

# 驗證安裝
qmake6 --version
g++ --version
```

#### Windows

1. 下載並安裝 Qt 6 (https://www.qt.io/download)
2. 選擇包含 MSVC 編譯器的版本
3. 安裝 Visual Studio 2019 或更高版本
4. 設定環境變數,將 Qt 的 bin 目錄加入 PATH

#### macOS

```bash
# 使用 Homebrew 安裝 Qt 6
brew install qt@6

# 設定環境變數
echo 'export PATH="/usr/local/opt/qt@6/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc

# 驗證安裝
qmake --version
```

### 取得 IADL Editor 原始碼

```bash
# 複製儲存庫
git clone https://github.com/chchlin1018/IADL-Editor-Test1.git
cd IADL-Editor-Test1

# 建置專案
mkdir build
cd build
qmake6 ..
make
```

---

## 插件架構概述

### 核心元件

IADL Editor 的插件系統由三個核心元件組成:

1. **ImportExportInterface**: 定義插件必須實作的介面
2. **ImportExportManager**: 管理所有插件的載入和執行
3. **ImportExportDialog**: 提供使用者介面,讓使用者選擇匯入/匯出選項

### 資料流程

```
使用者操作
    ↓
ImportExportDialog (UI)
    ↓
ImportExportManager (管理器)
    ↓
Plugin (插件實作)
    ↓
檔案系統 / 外部系統
```

### 插件生命週期

1. **載入階段**: ImportExportManager 在啟動時載入所有插件
2. **註冊階段**: 插件向管理器註冊其支援的系統類型和檔案格式
3. **執行階段**: 使用者觸發匯入/匯出操作時,管理器呼叫對應的插件
4. **清理階段**: 應用程式關閉時,管理器負責清理所有插件資源

---

## 開發步驟

### 步驟 1: 建立插件檔案

每個插件需要兩個檔案:

- **標頭檔** (`.h`): 定義插件類別和介面
- **實作檔** (`.cpp`): 實作插件的具體功能

檔案命名規範:
- 使用小寫字母和底線
- 以 `exporter` 或 `importer` 結尾
- 例如: `modbus_exporter.h`, `modbus_exporter.cpp`

### 步驟 2: 定義插件類別

插件類別必須:
- 繼承 `QObject` (用於 Qt 的信號/槽機制)
- 實作 `ImportExportInterface` 介面
- 使用 `Q_OBJECT` 巨集 (用於 Qt 的元物件系統)

基本結構:

```cpp
#ifndef YOURPLUGIN_H
#define YOURPLUGIN_H

#include <QObject>
#include "../importexportinterface.h"

class YourPlugin : public QObject, public ImportExportInterface
{
    Q_OBJECT

public:
    explicit YourPlugin(QObject *parent = nullptr);
    ~YourPlugin() override;

    // 實作 ImportExportInterface 的所有純虛擬函數
    QList<AssetData> importAssets(const QString &filePath, 
                                   const QMap<QString, QVariant> &options) override;
    bool exportAssets(const QString &filePath, 
                      const QList<AssetData> &assets, 
                      const QMap<QString, QVariant> &options) override;
    QStringList supportedFileExtensions() const override;
    SystemType systemType() const override;
    QString systemName() const override;
    QString systemDescription() const override;
    bool validateFile(const QString &filePath) override;

signals:
    void progressUpdated(int percentage);
    void errorOccurred(const QString &error);

private:
    QString m_lastError;
    // 其他私有成員
};

#endif // YOURPLUGIN_H
```

### 步驟 3: 實作必要的介面方法

#### 3.1 系統資訊方法

這些方法提供插件的基本資訊:

```cpp
ImportExportInterface::SystemType YourPlugin::systemType() const
{
    // 如果是新系統,需要先在 ImportExportInterface 中新增系統類型
    return SystemType::YOUR_SYSTEM;
}

QString YourPlugin::systemName() const
{
    return "Your System Name";
}

QString YourPlugin::systemDescription() const
{
    return "Brief description of your system";
}

QStringList YourPlugin::supportedFileExtensions() const
{
    // 返回支援的檔案副檔名列表
    return QStringList() << "xml" << "csv" << "json";
}
```

#### 3.2 檔案驗證方法

驗證檔案是否有效:

```cpp
bool YourPlugin::validateFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        m_lastError = "File does not exist: " + filePath;
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = "Cannot open file: " + filePath;
        return false;
    }
    
    // 可以在這裡加入更多驗證邏輯
    // 例如檢查檔案格式、檔案大小等
    
    file.close();
    return true;
}
```

#### 3.3 匯入方法

從檔案匯入資產資料:

```cpp
QList<AssetData> YourPlugin::importAssets(const QString &filePath, 
                                           const QMap<QString, QVariant> &options)
{
    QList<AssetData> assets;
    
    // 清除之前的錯誤
    m_lastError.clear();
    
    // 根據檔案副檔名選擇適當的解析器
    if (filePath.endsWith(".xml", Qt::CaseInsensitive)) {
        assets = importFromXML(filePath, options);
    } else if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        assets = importFromCSV(filePath, options);
    } else {
        m_lastError = "Unsupported file format";
        emit errorOccurred(m_lastError);
    }
    
    return assets;
}
```

#### 3.4 匯出方法

將資產資料匯出到檔案:

```cpp
bool YourPlugin::exportAssets(const QString &filePath, 
                               const QList<AssetData> &assets, 
                               const QMap<QString, QVariant> &options)
{
    // 清除之前的錯誤
    m_lastError.clear();
    
    // 根據檔案副檔名選擇適當的生成器
    if (filePath.endsWith(".xml", Qt::CaseInsensitive)) {
        return exportToXML(filePath, assets, options);
    } else if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        return exportToCSV(filePath, assets, options);
    } else {
        m_lastError = "Unsupported file format";
        emit errorOccurred(m_lastError);
        return false;
    }
}
```

### 步驟 4: 實作格式特定的處理函數

#### XML 匯入範例

```cpp
QList<AssetData> YourPlugin::importFromXML(const QString &filePath, 
                                            const QMap<QString, QVariant> &options)
{
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = "Failed to open file: " + filePath;
        emit errorOccurred(m_lastError);
        return assets;
    }
    
    QXmlStreamReader xml(&file);
    
    while (!xml.atEnd()) {
        xml.readNext();
        
        if (xml.isStartElement() && xml.name() == QString("Asset")) {
            AssetData asset;
            asset.id = xml.attributes().value("id").toString();
            asset.name = xml.attributes().value("name").toString();
            asset.type = xml.attributes().value("type").toString();
            
            // 讀取屬性
            while (!(xml.isEndElement() && xml.name() == QString("Asset"))) {
                xml.readNext();
                
                if (xml.isStartElement()) {
                    QString propertyName = xml.name().toString();
                    QString propertyValue = xml.readElementText();
                    asset.properties[propertyName] = propertyValue;
                }
            }
            
            assets.append(asset);
            
            // 更新進度
            if (assets.size() % 100 == 0) {
                emit progressUpdated(assets.size());
            }
        }
    }
    
    if (xml.hasError()) {
        m_lastError = "XML parsing error: " + xml.errorString();
        emit errorOccurred(m_lastError);
    }
    
    file.close();
    return assets;
}
```

#### CSV 匯出範例

```cpp
bool YourPlugin::exportToCSV(const QString &filePath, 
                              const QList<AssetData> &assets, 
                              const QMap<QString, QVariant> &options)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "Failed to create file: " + filePath;
        emit errorOccurred(m_lastError);
        return false;
    }
    
    QTextStream out(&file);
    
    // 寫入標頭
    out << "ID,Name,Type,Description\n";
    
    // 寫入資料
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        out << asset.id << ","
            << "\"" << asset.name << "\","
            << "\"" << asset.type << "\","
            << "\"" << asset.properties.value("description", "").toString() << "\"\n";
        
        // 更新進度
        if ((i + 1) % 100 == 0) {
            emit progressUpdated((i + 1) * 100 / assets.size());
        }
    }
    
    file.close();
    return true;
}
```

### 步驟 5: 註冊插件

在 `ImportExportManager::loadPlugins()` 中註冊你的插件:

```cpp
void ImportExportManager::loadPlugins()
{
    // 現有插件...
    m_plugins.append(new AvevaPiExporter());
    m_plugins.append(new JsonExporter());
    // ... 其他插件 ...
    
    // 新增你的插件
    m_plugins.append(new YourPlugin());
    
    // 填充查找表
    for (ImportExportInterface *plugin : m_plugins) {
        m_exporters[plugin->systemType()] = plugin;
        m_importers[plugin->systemType()] = plugin;
    }
}
```

### 步驟 6: 更新專案檔案

在 `.pro` 檔案中加入你的插件:

```qmake
SOURCES += \
    # ... 現有原始碼 ...
    plugins/yourplugin.cpp

HEADERS += \
    # ... 現有標頭檔 ...
    plugins/yourplugin.h
```

### 步驟 7: 新增系統類型 (如果需要)

如果你的插件支援的是全新的系統,需要在 `ImportExportInterface` 中新增系統類型:

```cpp
enum SystemType {
    AVEVA_PI,
    AVEVA_HMI_SCADA,
    // ... 現有類型 ...
    YOUR_SYSTEM  // 新增你的系統類型
};
```

---

## API 參考

### ImportExportInterface

這是所有插件必須實作的介面。

#### 資料結構

```cpp
struct AssetData {
    QString id;                          // 資產唯一識別碼
    QString name;                        // 資產名稱
    QString type;                        // 資產類型
    QVariantMap properties;              // 資產屬性 (鍵值對)
};
```

#### 系統類型

```cpp
enum SystemType {
    AVEVA_PI,           // AVEVA PI System
    AVEVA_HMI_SCADA,    // AVEVA HMI/SCADA
    BIM,                // BIM/IFC
    ETAP,               // ETAP
    OMNIVERSE,          // NVIDIA Omniverse
    JSON,               // JSON (通用格式)
    INTOUCH,            // AVEVA InTouch
    SYSTEM_PLATFORM,    // AVEVA System Platform
    EDGE,               // AVEVA EDGE
    HISTORIAN,          // AVEVA Historian
    IGNITION,           // Ignition
    DELTAV,             // Emerson DeltaV
    IFIX                // GE iFIX
};
```

#### 必須實作的方法

##### importAssets()

```cpp
virtual QList<AssetData> importAssets(
    const QString &filePath,                    // 要匯入的檔案路徑
    const QMap<QString, QVariant> &options      // 匯入選項
) = 0;
```

從指定檔案匯入資產資料。

**參數**:
- `filePath`: 要匯入的檔案完整路徑
- `options`: 匯入選項,可以包含使用者自訂的參數

**返回值**: 匯入的資產資料列表

**注意事項**:
- 如果匯入失敗,應該返回空列表並設定錯誤訊息
- 建議在處理大量資料時定期發送 `progressUpdated` 信號
- 應該處理所有可能的例外情況

##### exportAssets()

```cpp
virtual bool exportAssets(
    const QString &filePath,                    // 要匯出的檔案路徑
    const QList<AssetData> &assets,             // 要匯出的資產資料
    const QMap<QString, QVariant> &options      // 匯出選項
) = 0;
```

將資產資料匯出到指定檔案。

**參數**:
- `filePath`: 要匯出的檔案完整路徑
- `assets`: 要匯出的資產資料列表
- `options`: 匯出選項,可以包含使用者自訂的參數

**返回值**: 成功返回 `true`,失敗返回 `false`

**注意事項**:
- 如果匯出失敗,應該設定錯誤訊息並發送 `errorOccurred` 信號
- 建議在處理大量資料時定期發送 `progressUpdated` 信號
- 應該確保檔案寫入成功後才返回 `true`

##### supportedFileExtensions()

```cpp
virtual QStringList supportedFileExtensions() const = 0;
```

返回插件支援的檔案副檔名列表。

**返回值**: 檔案副檔名列表 (不包含點號)

**範例**:
```cpp
return QStringList() << "xml" << "csv" << "json";
```

##### systemType()

```cpp
virtual SystemType systemType() const = 0;
```

返回插件對應的系統類型。

**返回值**: 系統類型列舉值

##### systemName()

```cpp
virtual QString systemName() const = 0;
```

返回系統的顯示名稱。

**返回值**: 系統名稱字串

**範例**:
```cpp
return "Modbus RTU/TCP";
```

##### systemDescription()

```cpp
virtual QString systemDescription() const = 0;
```

返回系統的簡短描述。

**返回值**: 系統描述字串

**範例**:
```cpp
return "Modbus industrial communication protocol";
```

##### validateFile()

```cpp
virtual bool validateFile(const QString &filePath) = 0;
```

驗證檔案是否有效。

**參數**:
- `filePath`: 要驗證的檔案路徑

**返回值**: 檔案有效返回 `true`,否則返回 `false`

**注意事項**:
- 應該檢查檔案是否存在
- 應該檢查檔案是否可讀
- 可以進行基本的格式驗證

#### 錯誤處理

```cpp
QString getLastError() const { return m_lastError; }
```

取得最後一次操作的錯誤訊息。

**建議做法**:
- 在每次操作開始時清除 `m_lastError`
- 發生錯誤時設定 `m_lastError` 並發送 `errorOccurred` 信號

#### 信號 (Signals)

雖然不是介面的一部分,但建議插件實作以下信號:

```cpp
signals:
    void progressUpdated(int percentage);        // 進度更新 (0-100)
    void errorOccurred(const QString &error);    // 錯誤發生
```

---

## 完整範例

以下是一個完整的 Modbus 插件範例,展示如何實作一個功能完整的插件。

### modbus_exporter.h

```cpp
#ifndef MODBUSEXPORTER_H
#define MODBUSEXPORTER_H

#include <QObject>
#include "../importexportinterface.h"

/**
 * @brief Modbus RTU/TCP 匯入/匯出插件
 * 
 * 支援 Modbus 裝置配置的匯入和匯出
 * 檔案格式: XML, CSV, JSON
 */
class ModbusExporter : public QObject, public ImportExportInterface
{
    Q_OBJECT

public:
    explicit ModbusExporter(QObject *parent = nullptr);
    ~ModbusExporter() override;

    // ImportExportInterface 實作
    QList<AssetData> importAssets(const QString &filePath, 
                                   const QMap<QString, QVariant> &options) override;
    bool exportAssets(const QString &filePath, 
                      const QList<AssetData> &assets, 
                      const QMap<QString, QVariant> &options) override;
    QStringList supportedFileExtensions() const override;
    SystemType systemType() const override;
    QString systemName() const override;
    QString systemDescription() const override;
    bool validateFile(const QString &filePath) override;

signals:
    void progressUpdated(int percentage);
    void errorOccurred(const QString &error);

private:
    // Modbus 裝置類型
    enum DeviceType {
        RTU,        // Modbus RTU (序列埠)
        TCP,        // Modbus TCP (乙太網路)
        ASCII       // Modbus ASCII (序列埠)
    };

    // Modbus 資料類型
    enum DataType {
        Coil,               // 線圈 (讀寫,1 bit)
        DiscreteInput,      // 離散輸入 (唯讀,1 bit)
        HoldingRegister,    // 保持暫存器 (讀寫,16 bit)
        InputRegister       // 輸入暫存器 (唯讀,16 bit)
    };

    // 匯入函數
    QList<AssetData> importFromXML(const QString &filePath);
    QList<AssetData> importFromCSV(const QString &filePath);
    QList<AssetData> importFromJSON(const QString &filePath);

    // 匯出函數
    bool exportToXML(const QString &filePath, const QList<AssetData> &assets);
    bool exportToCSV(const QString &filePath, const QList<AssetData> &assets);
    bool exportToJSON(const QString &filePath, const QList<AssetData> &assets);

    // 輔助函數
    QString getDeviceTypeString(DeviceType type) const;
    DeviceType getDeviceTypeFromString(const QString &typeStr) const;
    QString getDataTypeString(DataType type) const;
    DataType getDataTypeFromString(const QString &typeStr) const;

    QString m_lastError;
};

#endif // MODBUSEXPORTER_H
```

### modbus_exporter.cpp

```cpp
#include "modbusexporter.h"
#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

ModbusExporter::ModbusExporter(QObject *parent)
    : QObject(parent)
{
}

ModbusExporter::~ModbusExporter()
{
}

ImportExportInterface::SystemType ModbusExporter::systemType() const
{
    // 注意: 需要先在 ImportExportInterface 中新增 MODBUS 類型
    return SystemType::JSON; // 暫時使用 JSON,實際應該是 MODBUS
}

QString ModbusExporter::systemName() const
{
    return "Modbus RTU/TCP";
}

QString ModbusExporter::systemDescription() const
{
    return "Modbus industrial communication protocol for PLCs and field devices";
}

QStringList ModbusExporter::supportedFileExtensions() const
{
    return QStringList() << "xml" << "csv" << "json";
}

bool ModbusExporter::validateFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        m_lastError = "File does not exist: " + filePath;
        return false;
    }
    
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = "Cannot open file: " + filePath;
        return false;
    }
    
    // 檢查檔案大小 (限制為 100MB)
    if (file.size() > 100 * 1024 * 1024) {
        m_lastError = "File too large: " + QString::number(file.size() / 1024 / 1024) + " MB";
        file.close();
        return false;
    }
    
    file.close();
    return true;
}

QList<AssetData> ModbusExporter::importAssets(const QString &filePath, 
                                               const QMap<QString, QVariant> &options)
{
    Q_UNUSED(options);
    
    m_lastError.clear();
    
    if (filePath.endsWith(".xml", Qt::CaseInsensitive)) {
        return importFromXML(filePath);
    } else if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        return importFromCSV(filePath);
    } else if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
        return importFromJSON(filePath);
    } else {
        m_lastError = "Unsupported file format for import";
        emit errorOccurred(m_lastError);
        return QList<AssetData>();
    }
}

bool ModbusExporter::exportAssets(const QString &filePath, 
                                   const QList<AssetData> &assets, 
                                   const QMap<QString, QVariant> &options)
{
    Q_UNUSED(options);
    
    m_lastError.clear();
    
    if (filePath.endsWith(".xml", Qt::CaseInsensitive)) {
        return exportToXML(filePath, assets);
    } else if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
        return exportToCSV(filePath, assets);
    } else if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
        return exportToJSON(filePath, assets);
    } else {
        m_lastError = "Unsupported file format for export";
        emit errorOccurred(m_lastError);
        return false;
    }
}

QList<AssetData> ModbusExporter::importFromXML(const QString &filePath)
{
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = "Failed to open file: " + filePath;
        emit errorOccurred(m_lastError);
        return assets;
    }
    
    QXmlStreamReader xml(&file);
    
    while (!xml.atEnd()) {
        xml.readNext();
        
        if (xml.isStartElement() && xml.name() == QString("ModbusDevice")) {
            AssetData asset;
            asset.id = xml.attributes().value("id").toString();
            asset.name = xml.attributes().value("name").toString();
            asset.type = "Modbus Device";
            
            // 讀取裝置屬性
            while (!(xml.isEndElement() && xml.name() == QString("ModbusDevice"))) {
                xml.readNext();
                
                if (xml.isStartElement()) {
                    QString elementName = xml.name().toString();
                    QString elementValue = xml.readElementText();
                    
                    if (elementName == "DeviceType") {
                        asset.properties["deviceType"] = elementValue;
                    } else if (elementName == "SlaveAddress") {
                        asset.properties["slaveAddress"] = elementValue.toInt();
                    } else if (elementName == "IPAddress") {
                        asset.properties["ipAddress"] = elementValue;
                    } else if (elementName == "Port") {
                        asset.properties["port"] = elementValue.toInt();
                    } else if (elementName == "BaudRate") {
                        asset.properties["baudRate"] = elementValue.toInt();
                    } else if (elementName == "DataBits") {
                        asset.properties["dataBits"] = elementValue.toInt();
                    } else if (elementName == "StopBits") {
                        asset.properties["stopBits"] = elementValue.toInt();
                    } else if (elementName == "Parity") {
                        asset.properties["parity"] = elementValue;
                    } else if (elementName == "Description") {
                        asset.properties["description"] = elementValue;
                    } else if (elementName == "Register") {
                        // 處理暫存器資訊
                        QString regInfo = elementValue;
                        asset.properties["register_" + regInfo] = regInfo;
                    }
                }
            }
            
            assets.append(asset);
            
            // 更新進度
            if (assets.size() % 10 == 0) {
                emit progressUpdated(assets.size());
            }
        }
    }
    
    if (xml.hasError()) {
        m_lastError = "XML parsing error: " + xml.errorString();
        emit errorOccurred(m_lastError);
    }
    
    file.close();
    return assets;
}

QList<AssetData> ModbusExporter::importFromCSV(const QString &filePath)
{
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = "Failed to open file: " + filePath;
        emit errorOccurred(m_lastError);
        return assets;
    }
    
    QTextStream in(&file);
    
    // 讀取標頭
    QString header = in.readLine();
    QStringList headers = header.split(',');
    
    // 讀取資料列
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(',');
        
        if (values.size() >= headers.size()) {
            AssetData asset;
            asset.type = "Modbus Device";
            
            for (int i = 0; i < headers.size(); ++i) {
                QString key = headers[i].trimmed();
                QString value = values[i].trimmed();
                
                // 移除引號
                if (value.startsWith('"') && value.endsWith('"')) {
                    value = value.mid(1, value.length() - 2);
                }
                
                if (key == "ID") {
                    asset.id = value;
                } else if (key == "Name") {
                    asset.name = value;
                } else if (key == "DeviceType") {
                    asset.properties["deviceType"] = value;
                } else if (key == "SlaveAddress") {
                    asset.properties["slaveAddress"] = value.toInt();
                } else if (key == "IPAddress") {
                    asset.properties["ipAddress"] = value;
                } else if (key == "Port") {
                    asset.properties["port"] = value.toInt();
                } else {
                    asset.properties[key.toLower()] = value;
                }
            }
            
            if (!asset.name.isEmpty()) {
                assets.append(asset);
            }
        }
        
        // 更新進度
        if (assets.size() % 10 == 0) {
            emit progressUpdated(assets.size());
        }
    }
    
    file.close();
    return assets;
}

QList<AssetData> ModbusExporter::importFromJSON(const QString &filePath)
{
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = "Failed to open file: " + filePath;
        emit errorOccurred(m_lastError);
        return assets;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        m_lastError = "Invalid JSON format";
        emit errorOccurred(m_lastError);
        return assets;
    }
    
    QJsonObject root = doc.object();
    QJsonArray devicesArray = root["devices"].toArray();
    
    for (const QJsonValue &value : devicesArray) {
        QJsonObject deviceObj = value.toObject();
        
        AssetData asset;
        asset.id = deviceObj["id"].toString();
        asset.name = deviceObj["name"].toString();
        asset.type = "Modbus Device";
        
        // 讀取屬性
        QJsonObject propsObj = deviceObj["properties"].toObject();
        for (auto it = propsObj.begin(); it != propsObj.end(); ++it) {
            asset.properties[it.key()] = it.value().toVariant();
        }
        
        assets.append(asset);
        
        // 更新進度
        if (assets.size() % 10 == 0) {
            emit progressUpdated(assets.size());
        }
    }
    
    return assets;
}

bool ModbusExporter::exportToXML(const QString &filePath, const QList<AssetData> &assets)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "Failed to create file: " + filePath;
        emit errorOccurred(m_lastError);
        return false;
    }
    
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument();
    
    xml.writeStartElement("ModbusConfiguration");
    xml.writeAttribute("Version", "1.0");
    xml.writeAttribute("ExportedFrom", "IADL Editor");
    xml.writeAttribute("ExportDate", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    xml.writeStartElement("Devices");
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        xml.writeStartElement("ModbusDevice");
        xml.writeAttribute("id", asset.id);
        xml.writeAttribute("name", asset.name);
        
        xml.writeTextElement("DeviceType", asset.properties.value("deviceType", "RTU").toString());
        xml.writeTextElement("SlaveAddress", QString::number(asset.properties.value("slaveAddress", 1).toInt()));
        
        if (asset.properties.contains("ipAddress")) {
            xml.writeTextElement("IPAddress", asset.properties["ipAddress"].toString());
            xml.writeTextElement("Port", QString::number(asset.properties.value("port", 502).toInt()));
        }
        
        if (asset.properties.contains("baudRate")) {
            xml.writeTextElement("BaudRate", QString::number(asset.properties["baudRate"].toInt()));
            xml.writeTextElement("DataBits", QString::number(asset.properties.value("dataBits", 8).toInt()));
            xml.writeTextElement("StopBits", QString::number(asset.properties.value("stopBits", 1).toInt()));
            xml.writeTextElement("Parity", asset.properties.value("parity", "None").toString());
        }
        
        xml.writeTextElement("Description", asset.properties.value("description", "").toString());
        
        xml.writeEndElement(); // ModbusDevice
        
        // 更新進度
        if ((i + 1) % 10 == 0) {
            emit progressUpdated((i + 1) * 100 / assets.size());
        }
    }
    
    xml.writeEndElement(); // Devices
    xml.writeEndElement(); // ModbusConfiguration
    xml.writeEndDocument();
    
    file.close();
    return true;
}

bool ModbusExporter::exportToCSV(const QString &filePath, const QList<AssetData> &assets)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "Failed to create file: " + filePath;
        emit errorOccurred(m_lastError);
        return false;
    }
    
    QTextStream out(&file);
    
    // 寫入標頭
    out << "ID,Name,DeviceType,SlaveAddress,IPAddress,Port,BaudRate,DataBits,StopBits,Parity,Description\n";
    
    // 寫入資料
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        out << asset.id << ","
            << "\"" << asset.name << "\","
            << "\"" << asset.properties.value("deviceType", "RTU").toString() << "\","
            << asset.properties.value("slaveAddress", 1).toInt() << ","
            << "\"" << asset.properties.value("ipAddress", "").toString() << "\","
            << asset.properties.value("port", 502).toInt() << ","
            << asset.properties.value("baudRate", 9600).toInt() << ","
            << asset.properties.value("dataBits", 8).toInt() << ","
            << asset.properties.value("stopBits", 1).toInt() << ","
            << "\"" << asset.properties.value("parity", "None").toString() << "\","
            << "\"" << asset.properties.value("description", "").toString() << "\"\n";
        
        // 更新進度
        if ((i + 1) % 10 == 0) {
            emit progressUpdated((i + 1) * 100 / assets.size());
        }
    }
    
    file.close();
    return true;
}

bool ModbusExporter::exportToJSON(const QString &filePath, const QList<AssetData> &assets)
{
    QJsonObject root;
    root["version"] = "1.0";
    root["exportedFrom"] = "IADL Editor";
    root["exportDate"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    
    QJsonArray devicesArray;
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        QJsonObject deviceObj;
        deviceObj["id"] = asset.id;
        deviceObj["name"] = asset.name;
        deviceObj["type"] = asset.type;
        
        QJsonObject propsObj;
        for (auto it = asset.properties.begin(); it != asset.properties.end(); ++it) {
            propsObj[it.key()] = QJsonValue::fromVariant(it.value());
        }
        deviceObj["properties"] = propsObj;
        
        devicesArray.append(deviceObj);
        
        // 更新進度
        if ((i + 1) % 10 == 0) {
            emit progressUpdated((i + 1) * 100 / assets.size());
        }
    }
    
    root["devices"] = devicesArray;
    
    QJsonDocument doc(root);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = "Failed to create file: " + filePath;
        emit errorOccurred(m_lastError);
        return false;
    }
    
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    return true;
}

QString ModbusExporter::getDeviceTypeString(DeviceType type) const
{
    switch (type) {
        case RTU: return "RTU";
        case TCP: return "TCP";
        case ASCII: return "ASCII";
        default: return "RTU";
    }
}

ModbusExporter::DeviceType ModbusExporter::getDeviceTypeFromString(const QString &typeStr) const
{
    if (typeStr.compare("TCP", Qt::CaseInsensitive) == 0) {
        return TCP;
    } else if (typeStr.compare("ASCII", Qt::CaseInsensitive) == 0) {
        return ASCII;
    } else {
        return RTU;
    }
}

QString ModbusExporter::getDataTypeString(DataType type) const
{
    switch (type) {
        case Coil: return "Coil";
        case DiscreteInput: return "DiscreteInput";
        case HoldingRegister: return "HoldingRegister";
        case InputRegister: return "InputRegister";
        default: return "HoldingRegister";
    }
}

ModbusExporter::DataType ModbusExporter::getDataTypeFromString(const QString &typeStr) const
{
    if (typeStr.compare("Coil", Qt::CaseInsensitive) == 0) {
        return Coil;
    } else if (typeStr.compare("DiscreteInput", Qt::CaseInsensitive) == 0) {
        return DiscreteInput;
    } else if (typeStr.compare("InputRegister", Qt::CaseInsensitive) == 0) {
        return InputRegister;
    } else {
        return HoldingRegister;
    }
}
```

### 使用範例

#### XML 格式範例

```xml
<ModbusConfiguration Version="1.0">
  <Devices>
    <ModbusDevice id="MB001" name="Temperature Sensor">
      <DeviceType>RTU</DeviceType>
      <SlaveAddress>1</SlaveAddress>
      <BaudRate>9600</BaudRate>
      <DataBits>8</DataBits>
      <StopBits>1</StopBits>
      <Parity>None</Parity>
      <Description>Main reactor temperature sensor</Description>
    </ModbusDevice>
    <ModbusDevice id="MB002" name="Flow Controller">
      <DeviceType>TCP</DeviceType>
      <SlaveAddress>2</SlaveAddress>
      <IPAddress>192.168.1.100</IPAddress>
      <Port>502</Port>
      <Description>Inlet flow controller</Description>
    </ModbusDevice>
  </Devices>
</ModbusConfiguration>
```

#### CSV 格式範例

```csv
ID,Name,DeviceType,SlaveAddress,IPAddress,Port,BaudRate,DataBits,StopBits,Parity,Description
MB001,Temperature Sensor,RTU,1,,0,9600,8,1,None,Main reactor temperature sensor
MB002,Flow Controller,TCP,2,192.168.1.100,502,0,0,0,None,Inlet flow controller
```

#### JSON 格式範例

```json
{
  "version": "1.0",
  "exportedFrom": "IADL Editor",
  "exportDate": "2024-10-09T10:30:00",
  "devices": [
    {
      "id": "MB001",
      "name": "Temperature Sensor",
      "type": "Modbus Device",
      "properties": {
        "deviceType": "RTU",
        "slaveAddress": 1,
        "baudRate": 9600,
        "dataBits": 8,
        "stopBits": 1,
        "parity": "None",
        "description": "Main reactor temperature sensor"
      }
    },
    {
      "id": "MB002",
      "name": "Flow Controller",
      "type": "Modbus Device",
      "properties": {
        "deviceType": "TCP",
        "slaveAddress": 2,
        "ipAddress": "192.168.1.100",
        "port": 502,
        "description": "Inlet flow controller"
      }
    }
  ]
}
```

---

## 最佳實踐

### 1. 錯誤處理

**良好的錯誤處理範例**:

```cpp
QList<AssetData> YourPlugin::importAssets(const QString &filePath, 
                                           const QMap<QString, QVariant> &options)
{
    QList<AssetData> assets;
    
    // 清除之前的錯誤
    m_lastError.clear();
    
    // 驗證檔案
    if (!validateFile(filePath)) {
        emit errorOccurred(m_lastError);
        return assets;
    }
    
    try {
        // 執行匯入操作
        assets = performImport(filePath, options);
    } catch (const std::exception &e) {
        m_lastError = QString("Import failed: %1").arg(e.what());
        emit errorOccurred(m_lastError);
        return QList<AssetData>();
    }
    
    return assets;
}
```

### 2. 進度報告

對於大型檔案,應該定期報告進度:

```cpp
for (int i = 0; i < totalItems; ++i) {
    // 處理項目
    processItem(items[i]);
    
    // 每處理 100 個項目或每 1% 進度報告一次
    if ((i + 1) % 100 == 0 || (i + 1) % (totalItems / 100) == 0) {
        int percentage = ((i + 1) * 100) / totalItems;
        emit progressUpdated(percentage);
    }
}
```

### 3. 資源管理

確保正確關閉檔案和釋放資源:

```cpp
bool YourPlugin::exportAssets(const QString &filePath, 
                               const QList<AssetData> &assets, 
                               const QMap<QString, QVariant> &options)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        m_lastError = "Cannot create file: " + filePath;
        emit errorOccurred(m_lastError);
        return false;
    }
    
    // 使用 RAII 確保檔案被關閉
    // 或使用 QFile 的自動管理
    
    bool success = writeData(&file, assets);
    
    file.close();  // 明確關閉檔案
    
    return success;
}
```

### 4. 資料驗證

在匯入和匯出時驗證資料:

```cpp
bool YourPlugin::validateAssetData(const AssetData &asset) const
{
    // 檢查必要欄位
    if (asset.id.isEmpty()) {
        return false;
    }
    
    if (asset.name.isEmpty()) {
        return false;
    }
    
    // 檢查資料類型
    if (asset.properties.contains("port")) {
        bool ok;
        int port = asset.properties["port"].toInt(&ok);
        if (!ok || port < 0 || port > 65535) {
            return false;
        }
    }
    
    return true;
}
```

### 5. 效能最佳化

對於大型檔案,考慮使用緩衝和批次處理:

```cpp
QList<AssetData> YourPlugin::importFromCSV(const QString &filePath)
{
    QList<AssetData> assets;
    assets.reserve(1000);  // 預先分配記憶體
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return assets;
    }
    
    QTextStream in(&file);
    in.setCodec("UTF-8");  // 設定編碼
    
    // 使用緩衝讀取
    QString line;
    while (in.readLineInto(&line)) {
        AssetData asset = parseLine(line);
        if (validateAssetData(asset)) {
            assets.append(asset);
        }
    }
    
    file.close();
    return assets;
}
```

### 6. 編碼處理

正確處理不同的字元編碼:

```cpp
QFile file(filePath);
if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
}

QTextStream in(&file);
in.setCodec("UTF-8");  // 或根據需要設定其他編碼

QString content = in.readAll();
file.close();
```

### 7. 向後相容性

支援多個版本的檔案格式:

```cpp
QList<AssetData> YourPlugin::importFromXML(const QString &filePath)
{
    QFile file(filePath);
    // ... 開啟檔案 ...
    
    QXmlStreamReader xml(&file);
    
    QString version = "1.0";  // 預設版本
    
    // 讀取版本資訊
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement() && xml.name() == QString("Configuration")) {
            version = xml.attributes().value("Version").toString();
            break;
        }
    }
    
    // 根據版本選擇不同的解析方法
    if (version.startsWith("1.")) {
        return parseVersion1(xml);
    } else if (version.startsWith("2.")) {
        return parseVersion2(xml);
    } else {
        m_lastError = "Unsupported version: " + version;
        return QList<AssetData>();
    }
}
```

### 8. 日誌記錄

使用 Qt 的日誌系統記錄重要事件:

```cpp
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(modbusPlugin, "iadl.plugin.modbus")

QList<AssetData> ModbusExporter::importAssets(const QString &filePath, 
                                               const QMap<QString, QVariant> &options)
{
    qCInfo(modbusPlugin) << "Importing from:" << filePath;
    
    QList<AssetData> assets = performImport(filePath, options);
    
    qCInfo(modbusPlugin) << "Imported" << assets.size() << "assets";
    
    return assets;
}
```

---

## 測試和除錯

### 單元測試

建立測試檔案 `test_modbus_exporter.cpp`:

```cpp
#include <QtTest/QtTest>
#include "../plugins/modbusexporter.h"

class TestModbusExporter : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testSystemInfo();
    void testFileValidation();
    void testXMLImport();
    void testCSVExport();
    void testJSONRoundTrip();

private:
    ModbusExporter *m_plugin;
};

void TestModbusExporter::initTestCase()
{
    m_plugin = new ModbusExporter();
}

void TestModbusExporter::cleanupTestCase()
{
    delete m_plugin;
}

void TestModbusExporter::testSystemInfo()
{
    QCOMPARE(m_plugin->systemName(), QString("Modbus RTU/TCP"));
    QVERIFY(m_plugin->supportedFileExtensions().contains("xml"));
    QVERIFY(m_plugin->supportedFileExtensions().contains("csv"));
    QVERIFY(m_plugin->supportedFileExtensions().contains("json"));
}

void TestModbusExporter::testFileValidation()
{
    // 測試不存在的檔案
    QVERIFY(!m_plugin->validateFile("/nonexistent/file.xml"));
    
    // 測試有效的檔案
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    QVERIFY(m_plugin->validateFile(tempFile.fileName()));
}

void TestModbusExporter::testXMLImport()
{
    // 建立測試 XML 檔案
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    
    QTextStream out(&tempFile);
    out << "<?xml version=\"1.0\"?>\n";
    out << "<ModbusConfiguration Version=\"1.0\">\n";
    out << "  <Devices>\n";
    out << "    <ModbusDevice id=\"MB001\" name=\"Test Device\">\n";
    out << "      <DeviceType>RTU</DeviceType>\n";
    out << "      <SlaveAddress>1</SlaveAddress>\n";
    out << "    </ModbusDevice>\n";
    out << "  </Devices>\n";
    out << "</ModbusConfiguration>\n";
    tempFile.close();
    
    // 測試匯入
    QList<AssetData> assets = m_plugin->importAssets(tempFile.fileName(), QMap<QString, QVariant>());
    
    QCOMPARE(assets.size(), 1);
    QCOMPARE(assets[0].id, QString("MB001"));
    QCOMPARE(assets[0].name, QString("Test Device"));
}

void TestModbusExporter::testCSVExport()
{
    // 建立測試資料
    QList<AssetData> assets;
    AssetData asset;
    asset.id = "MB001";
    asset.name = "Test Device";
    asset.type = "Modbus Device";
    asset.properties["deviceType"] = "RTU";
    asset.properties["slaveAddress"] = 1;
    assets.append(asset);
    
    // 匯出到臨時檔案
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    tempFile.close();
    
    QVERIFY(m_plugin->exportAssets(tempFile.fileName(), assets, QMap<QString, QVariant>()));
    
    // 驗證檔案內容
    QVERIFY(tempFile.open());
    QString content = tempFile.readAll();
    QVERIFY(content.contains("MB001"));
    QVERIFY(content.contains("Test Device"));
}

void TestModbusExporter::testJSONRoundTrip()
{
    // 建立測試資料
    QList<AssetData> originalAssets;
    AssetData asset;
    asset.id = "MB001";
    asset.name = "Test Device";
    asset.type = "Modbus Device";
    asset.properties["deviceType"] = "RTU";
    asset.properties["slaveAddress"] = 1;
    originalAssets.append(asset);
    
    // 匯出
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    tempFile.close();
    
    QVERIFY(m_plugin->exportAssets(tempFile.fileName(), originalAssets, QMap<QString, QVariant>()));
    
    // 匯入
    QList<AssetData> importedAssets = m_plugin->importAssets(tempFile.fileName(), QMap<QString, QVariant>());
    
    // 驗證資料一致性
    QCOMPARE(importedAssets.size(), originalAssets.size());
    QCOMPARE(importedAssets[0].id, originalAssets[0].id);
    QCOMPARE(importedAssets[0].name, originalAssets[0].name);
}

QTEST_MAIN(TestModbusExporter)
#include "test_modbus_exporter.moc"
```

### 除錯技巧

#### 1. 使用 qDebug() 輸出除錯資訊

```cpp
#include <QDebug>

QList<AssetData> YourPlugin::importAssets(const QString &filePath, 
                                           const QMap<QString, QVariant> &options)
{
    qDebug() << "Importing from:" << filePath;
    qDebug() << "Options:" << options;
    
    QList<AssetData> assets = performImport(filePath, options);
    
    qDebug() << "Imported" << assets.size() << "assets";
    
    return assets;
}
```

#### 2. 使用 Qt Creator 的除錯器

1. 在程式碼中設定中斷點
2. 以除錯模式執行 (F5)
3. 檢查變數值和呼叫堆疊
4. 單步執行程式碼 (F10/F11)

#### 3. 記憶體洩漏檢測

使用 Valgrind (Linux) 或 Visual Studio 的記憶體分析工具:

```bash
valgrind --leak-check=full ./iadl-editor-test1
```

#### 4. 效能分析

使用 Qt Creator 的效能分析工具或 perf (Linux):

```bash
perf record ./iadl-editor-test1
perf report
```

---

## 部署和發布

### 1. 文件撰寫

為你的插件撰寫完整的文件:

```markdown
# Modbus RTU/TCP 插件

## 概述

本插件提供 Modbus RTU 和 Modbus TCP 裝置配置的匯入和匯出功能。

## 支援的格式

- XML: 完整的裝置配置
- CSV: 簡化的表格格式
- JSON: 結構化的 JSON 格式

## 使用方式

### 匯入

1. 選擇 **檔案 > 匯入資產**
2. 選擇 Modbus 配置檔案
3. 系統自動載入裝置資訊

### 匯出

1. 選擇 **檔案 > 匯出資產**
2. 選擇 **Modbus RTU/TCP** 作為目標系統
3. 選擇檔案格式並儲存

## 範例

[包含範例檔案]

## 已知限制

- 目前不支援 Modbus Plus
- 不支援自訂功能碼

## 授權

MIT License
```

### 2. 版本控制

使用語意化版本號 (Semantic Versioning):

```cpp
QString ModbusExporter::pluginVersion() const
{
    return "1.0.0";  // 主版本.次版本.修訂版本
}
```

### 3. 發布檢查清單

- [ ] 所有功能都已實作並測試
- [ ] 單元測試通過
- [ ] 文件已完成
- [ ] 範例檔案已準備
- [ ] 已在目標平台上測試
- [ ] 記憶體洩漏已檢查
- [ ] 效能已最佳化
- [ ] 錯誤處理已完善
- [ ] 程式碼已審查
- [ ] 授權資訊已加入

### 4. 提交 Pull Request

如果要將插件貢獻到 IADL Editor 專案:

1. Fork 專案儲存庫
2. 建立功能分支
3. 提交你的變更
4. 撰寫清晰的提交訊息
5. 建立 Pull Request
6. 等待程式碼審查

---

## 常見問題

### Q1: 如何支援新的檔案格式?

**A**: 在插件中加入新的匯入/匯出函數,並在 `importAssets()` 和 `exportAssets()` 中根據檔案副檔名呼叫對應的函數。

```cpp
QList<AssetData> YourPlugin::importAssets(const QString &filePath, 
                                           const QMap<QString, QVariant> &options)
{
    if (filePath.endsWith(".xml")) {
        return importFromXML(filePath, options);
    } else if (filePath.endsWith(".newformat")) {
        return importFromNewFormat(filePath, options);
    }
    // ...
}
```

### Q2: 如何處理大型檔案?

**A**: 使用串流讀取和批次處理:

```cpp
QList<AssetData> YourPlugin::importFromLargeFile(const QString &filePath)
{
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return assets;
    }
    
    QTextStream in(&file);
    const int batchSize = 1000;
    int count = 0;
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        AssetData asset = parseLine(line);
        assets.append(asset);
        
        count++;
        if (count % batchSize == 0) {
            // 定期更新進度
            emit progressUpdated(count);
            // 可以在這裡處理批次資料
        }
    }
    
    file.close();
    return assets;
}
```

### Q3: 如何支援多語言?

**A**: 使用 Qt 的翻譯系統:

```cpp
QString ModbusExporter::systemName() const
{
    return tr("Modbus RTU/TCP");
}

QString ModbusExporter::systemDescription() const
{
    return tr("Modbus industrial communication protocol");
}
```

然後建立翻譯檔案 (`.ts`) 並使用 `lupdate` 和 `lrelease` 工具。

### Q4: 如何處理不同的字元編碼?

**A**: 使用 Qt 的編碼轉換功能:

```cpp
QFile file(filePath);
if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
}

QTextStream in(&file);

// 嘗試偵測編碼
QByteArray data = file.readAll();
file.seek(0);

if (data.contains('\0')) {
    // 可能是 UTF-16
    in.setCodec("UTF-16");
} else {
    // 預設使用 UTF-8
    in.setCodec("UTF-8");
}

QString content = in.readAll();
```

### Q5: 如何實作進階的資料驗證?

**A**: 建立專門的驗證類別:

```cpp
class ModbusDataValidator
{
public:
    static bool validateDevice(const AssetData &asset, QString &error)
    {
        // 檢查必要欄位
        if (asset.id.isEmpty()) {
            error = "Device ID is required";
            return false;
        }
        
        // 檢查從站位址
        if (!asset.properties.contains("slaveAddress")) {
            error = "Slave address is required";
            return false;
        }
        
        int slaveAddress = asset.properties["slaveAddress"].toInt();
        if (slaveAddress < 1 || slaveAddress > 247) {
            error = "Slave address must be between 1 and 247";
            return false;
        }
        
        // 檢查裝置類型特定的屬性
        QString deviceType = asset.properties.value("deviceType", "RTU").toString();
        if (deviceType == "TCP") {
            if (!asset.properties.contains("ipAddress")) {
                error = "IP address is required for TCP devices";
                return false;
            }
            // 驗證 IP 位址格式
            // ...
        }
        
        return true;
    }
};

// 在插件中使用
bool ModbusExporter::exportAssets(const QString &filePath, 
                                   const QList<AssetData> &assets, 
                                   const QMap<QString, QVariant> &options)
{
    // 驗證所有資產
    for (const AssetData &asset : assets) {
        QString error;
        if (!ModbusDataValidator::validateDevice(asset, error)) {
            m_lastError = error;
            emit errorOccurred(m_lastError);
            return false;
        }
    }
    
    // 繼續匯出...
}
```

### Q6: 如何實作增量匯入/匯出?

**A**: 追蹤已處理的項目:

```cpp
class IncrementalExporter
{
public:
    bool exportIncremental(const QString &filePath, 
                          const QList<AssetData> &newAssets,
                          const QList<AssetData> &modifiedAssets,
                          const QStringList &deletedAssetIds)
    {
        // 讀取現有檔案
        QList<AssetData> existingAssets = readExistingFile(filePath);
        
        // 移除已刪除的資產
        for (const QString &id : deletedAssetIds) {
            existingAssets.removeIf([&id](const AssetData &asset) {
                return asset.id == id;
            });
        }
        
        // 更新修改的資產
        for (const AssetData &modified : modifiedAssets) {
            auto it = std::find_if(existingAssets.begin(), existingAssets.end(),
                                  [&modified](const AssetData &asset) {
                                      return asset.id == modified.id;
                                  });
            if (it != existingAssets.end()) {
                *it = modified;
            }
        }
        
        // 加入新資產
        existingAssets.append(newAssets);
        
        // 寫回檔案
        return writeToFile(filePath, existingAssets);
    }
};
```

### Q7: 如何實作自訂選項對話框?

**A**: 建立自訂的 Qt 對話框:

```cpp
class ModbusExportOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModbusExportOptionsDialog(QWidget *parent = nullptr)
        : QDialog(parent)
    {
        setupUi();
    }
    
    QMap<QString, QVariant> getOptions() const
    {
        QMap<QString, QVariant> options;
        options["includeRegisters"] = m_includeRegistersCheckBox->isChecked();
        options["deviceType"] = m_deviceTypeComboBox->currentText();
        return options;
    }

private:
    void setupUi()
    {
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        m_includeRegistersCheckBox = new QCheckBox("Include register definitions", this);
        layout->addWidget(m_includeRegistersCheckBox);
        
        m_deviceTypeComboBox = new QComboBox(this);
        m_deviceTypeComboBox->addItems({"All", "RTU Only", "TCP Only"});
        layout->addWidget(m_deviceTypeComboBox);
        
        QDialogButtonBox *buttonBox = new QDialogButtonBox(
            QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
        connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
        layout->addWidget(buttonBox);
    }
    
    QCheckBox *m_includeRegistersCheckBox;
    QComboBox *m_deviceTypeComboBox;
};

// 在匯出前顯示對話框
bool ModbusExporter::exportAssets(const QString &filePath, 
                                   const QList<AssetData> &assets, 
                                   const QMap<QString, QVariant> &options)
{
    // 如果沒有提供選項,顯示對話框
    QMap<QString, QVariant> exportOptions = options;
    if (exportOptions.isEmpty()) {
        ModbusExportOptionsDialog dialog;
        if (dialog.exec() == QDialog::Accepted) {
            exportOptions = dialog.getOptions();
        } else {
            return false;  // 使用者取消
        }
    }
    
    // 使用選項進行匯出...
}
```

---

## 附錄

### A. 完整的專案結構

```
iadl-editor-test1/
├── main.cpp
├── mainwindow.h
├── mainwindow.cpp
├── mainwindow.ui
├── importexportinterface.h
├── importexportmanager.h
├── importexportmanager.cpp
├── importexportdialog.h
├── importexportdialog.cpp
├── plugins/
│   ├── avevapiexporter.h
│   ├── avevapiexporter.cpp
│   ├── jsonexporter.h
│   ├── jsonexporter.cpp
│   ├── bimimporter.h
│   ├── bimimporter.cpp
│   ├── omniverseexporter.h
│   ├── omniverseexporter.cpp
│   ├── etapexporter.h
│   ├── etapexporter.cpp
│   ├── intouchexporter.h
│   ├── intouchexporter.cpp
│   ├── systemplatformexporter.h
│   ├── systemplatformexporter.cpp
│   ├── edgeexporter.h
│   ├── edgeexporter.cpp
│   ├── historianexporter.h
│   ├── historianexporter.cpp
│   ├── ignitionexporter.h
│   ├── ignitionexporter.cpp
│   ├── deltavexporter.h
│   ├── deltavexporter.cpp
│   ├── ifixexporter.h
│   ├── ifixexporter.cpp
│   ├── modbusexporter.h        # 你的新插件
│   └── modbusexporter.cpp      # 你的新插件
├── tests/
│   └── test_modbus_exporter.cpp
├── docs/
│   ├── PLUGIN_DEVELOPMENT_GUIDE.md
│   └── MODBUS_PLUGIN_README.md
├── iadl-editor-test1.pro
└── README.md
```

### B. 參考資源

**Qt 文件**:
- Qt 6 Documentation: https://doc.qt.io/qt-6/
- Qt Widgets: https://doc.qt.io/qt-6/qtwidgets-index.html
- Qt XML: https://doc.qt.io/qt-6/qtxml-index.html

**C++ 參考**:
- C++ Reference: https://en.cppreference.com/
- C++17 Features: https://en.cppreference.com/w/cpp/17

**工業協定**:
- Modbus Protocol: https://modbus.org/
- OPC UA: https://opcfoundation.org/
- IEC 61850: https://www.iec.ch/

**開發工具**:
- Qt Creator: https://www.qt.io/product/development-tools
- CMake: https://cmake.org/
- Git: https://git-scm.com/

### C. 社群和支援

**IADL Editor 社群**:
- GitHub: https://github.com/chchlin1018/IADL-Editor-Test1
- Issues: https://github.com/chchlin1018/IADL-Editor-Test1/issues
- Discussions: https://github.com/chchlin1018/IADL-Editor-Test1/discussions

**Qt 社群**:
- Qt Forum: https://forum.qt.io/
- Qt Discord: https://discord.gg/qt

---

## 結論

本指南提供了完整的 IADL Editor 插件開發流程,從環境設定到部署發布。透過遵循本指南的最佳實踐和範例,你可以輕鬆開發出高品質的插件,擴展 IADL Editor 的功能。

如果你在開發過程中遇到問題,歡迎在 GitHub Issues 中提問,或參考現有插件的實作。我們期待看到更多社群貢獻的插件,讓 IADL Editor 成為更強大的工業資產管理工具。

---

**文件版本**: 1.0  
**最後更新**: 2024-10-09  
**作者**: 林志錚 Michael Lin(Chih Cheng Lin)(Chih Cheng Lin) Michael Lin(Chih Cheng Lin) Assistant  
**授權**: MIT License

