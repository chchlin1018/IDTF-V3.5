#include "intouchexporter.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>

InTouchExporter::InTouchExporter(QObject *parent)
    : QObject(parent)
{
}

InTouchExporter::~InTouchExporter()
{
}

ImportExportInterface::SystemType InTouchExporter::systemType() const
{
    return SystemType::INTOUCH;
}

QString InTouchExporter::systemName() const
{
    return "AVEVA InTouch";
}

QString InTouchExporter::systemDescription() const
{
    return "AVEVA InTouch SCADA System - Tag Database Import/Export";
}

QStringList InTouchExporter::supportedFileExtensions() const
{
    return QStringList() << "csv" << "txt";
}

bool InTouchExporter::validateFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    
    if (!fileInfo.exists()) {
        return true; // 對於匯出,檔案可以不存在
    }
    
    QString ext = fileInfo.suffix().toLower();
    if (!supportedFileExtensions().contains(ext)) {
        m_lastError = tr("不支援的檔案格式: %1").arg(ext);
        emit errorOccurred(m_lastError);
        return false;
    }
    
    return true;
}

QList<AssetData> InTouchExporter::importAssets(
    const QString &filePath, 
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始匯入 InTouch 標籤資料庫..."));
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        m_lastError = tr("檔案不存在: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return QList<AssetData>();
    }
    
    emit progressUpdated(10, tr("正在解析 InTouch CSV 檔案..."));
    
    QList<AssetData> assets = parseInTouchCsv(filePath, options);
    
    emit progressUpdated(100, tr("匯入完成，共匯入 %1 個標籤").arg(assets.size()));
    
    return assets;
}

QList<AssetData> InTouchExporter::parseInTouchCsv(
    const QString &filePath,
    const QMap<QString, QVariant> &options)
{
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = tr("無法開啟檔案: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return assets;
    }
    
    QTextStream in(&file);
    
    // 讀取標頭行
    QString headerLine = in.readLine();
    QStringList headers = headerLine.split(",");
    
    int lineCount = 0;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) {
            continue;
        }
        
        QStringList fields = line.split(",");
        
        if (!validateTagData(fields)) {
            qWarning() << "Invalid tag data at line" << lineCount;
            continue;
        }
        
        AssetData asset = convertInTouchTagToAsset(fields);
        if (!asset.id.isEmpty()) {
            assets.append(asset);
        }
        
        lineCount++;
        if (lineCount % 100 == 0) {
            int progress = 10 + (lineCount * 80 / qMax(1, lineCount + 100));
            emit progressUpdated(progress, tr("已處理 %1 個標籤...").arg(lineCount));
        }
    }
    
    file.close();
    
    return assets;
}

AssetData InTouchExporter::convertInTouchTagToAsset(const QStringList &tagFields)
{
    AssetData asset;
    
    if (tagFields.size() < 3) {
        return asset;
    }
    
    // 基本資訊
    asset.id = tagFields[0].trimmed();  // 標籤名稱
    asset.name = asset.id;
    asset.type = tagFields[1].trimmed();  // 標籤類型
    
    // 屬性
    asset.properties["intouch_tag_name"] = asset.id;
    asset.properties["intouch_tag_type"] = asset.type;
    
    if (tagFields.size() > 2) {
        asset.properties["description"] = tagFields[2].trimmed();
    }
    
    if (tagFields.size() > 3) {
        asset.properties["access_name"] = tagFields[3].trimmed();
    }
    
    if (tagFields.size() > 4) {
        asset.properties["item_name"] = tagFields[4].trimmed();
    }
    
    if (tagFields.size() > 5) {
        asset.properties["min_value"] = tagFields[5].trimmed();
    }
    
    if (tagFields.size() > 6) {
        asset.properties["max_value"] = tagFields[6].trimmed();
    }
    
    if (tagFields.size() > 7) {
        asset.properties["initial_value"] = tagFields[7].trimmed();
    }
    
    if (tagFields.size() > 8) {
        asset.properties["alarm_enabled"] = tagFields[8].trimmed();
    }
    
    return asset;
}

bool InTouchExporter::exportAssets(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始匯出到 InTouch 標籤資料庫..."));
    
    if (assets.isEmpty()) {
        m_lastError = tr("沒有資產可匯出");
        emit errorOccurred(m_lastError);
        return false;
    }
    
    if (!validateFile(filePath)) {
        return false;
    }
    
    emit progressUpdated(10, tr("正在生成 InTouch CSV 檔案..."));
    
    bool success = generateInTouchCsv(filePath, assets, options);
    
    if (success) {
        emit progressUpdated(100, tr("匯出完成，共匯出 %1 個標籤").arg(assets.size()));
    } else {
        emit errorOccurred(tr("匯出失敗"));
    }
    
    return success;
}

bool InTouchExporter::generateInTouchCsv(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = tr("無法建立檔案: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return false;
    }
    
    QTextStream out(&file);
    
    // 寫入標頭
    out << "Tagname,Type,Description,AccessName,ItemName,MinValue,MaxValue,InitialValue,AlarmEnabled\n";
    
    emit progressUpdated(30, tr("正在寫入標籤資訊..."));
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        // 標籤名稱
        QString tagName = asset.properties.value("intouch_tag_name", asset.id).toString();
        out << tagName << ",";
        
        // 標籤類型
        QString tagType = asset.properties.value("intouch_tag_type", "MemoryReal").toString();
        out << tagType << ",";
        
        // 描述
        QString description = asset.properties.value("description", asset.name).toString();
        out << "\"" << description << "\",";
        
        // 存取名稱
        QString accessName = asset.properties.value("access_name", "").toString();
        out << accessName << ",";
        
        // 項目名稱
        QString itemName = asset.properties.value("item_name", "").toString();
        out << itemName << ",";
        
        // 最小值
        QString minValue = asset.properties.value("min_value", "0").toString();
        out << minValue << ",";
        
        // 最大值
        QString maxValue = asset.properties.value("max_value", "100").toString();
        out << maxValue << ",";
        
        // 初始值
        QString initialValue = asset.properties.value("initial_value", "0").toString();
        out << initialValue << ",";
        
        // 警報啟用
        QString alarmEnabled = asset.properties.value("alarm_enabled", "0").toString();
        out << alarmEnabled << "\n";
        
        if ((i + 1) % 100 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, tr("已寫入 %1/%2 個標籤...").arg(i + 1).arg(assets.size()));
        }
    }
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

QString InTouchExporter::convertAssetToInTouchTag(const AssetData &asset)
{
    QString result;
    QTextStream out(&result);
    
    out << asset.id << ",";
    out << asset.properties.value("intouch_tag_type", "MemoryReal").toString() << ",";
    out << "\"" << asset.name << "\",";
    out << asset.properties.value("access_name", "").toString() << ",";
    out << asset.properties.value("item_name", "").toString() << ",";
    out << asset.properties.value("min_value", "0").toString() << ",";
    out << asset.properties.value("max_value", "100").toString() << ",";
    out << asset.properties.value("initial_value", "0").toString() << ",";
    out << asset.properties.value("alarm_enabled", "0").toString();
    
    return result;
}

QString InTouchExporter::getTagTypeString(TagType type) const
{
    switch (type) {
        case MemoryDiscrete: return "MemoryDiscrete";
        case MemoryInteger: return "MemoryInteger";
        case MemoryReal: return "MemoryReal";
        case MemoryMessage: return "MemoryMessage";
        case IODiscrete: return "IODiscrete";
        case IOInteger: return "IOInteger";
        case IOReal: return "IOReal";
        case GroupVar: return "GroupVar";
        case IndirectDiscrete: return "IndirectDiscrete";
        case IndirectAnalog: return "IndirectAnalog";
        default: return "MemoryReal";
    }
}

InTouchExporter::TagType InTouchExporter::getTagTypeFromString(const QString &typeStr) const
{
    if (typeStr == "MemoryDiscrete") return MemoryDiscrete;
    if (typeStr == "MemoryInteger") return MemoryInteger;
    if (typeStr == "MemoryReal") return MemoryReal;
    if (typeStr == "MemoryMessage") return MemoryMessage;
    if (typeStr == "IODiscrete") return IODiscrete;
    if (typeStr == "IOInteger") return IOInteger;
    if (typeStr == "IOReal") return IOReal;
    if (typeStr == "GroupVar") return GroupVar;
    if (typeStr == "IndirectDiscrete") return IndirectDiscrete;
    if (typeStr == "IndirectAnalog") return IndirectAnalog;
    return MemoryReal;
}

bool InTouchExporter::validateTagData(const QStringList &fields) const
{
    if (fields.size() < 2) {
        return false;
    }
    
    // 檢查標籤名稱
    if (fields[0].trimmed().isEmpty()) {
        return false;
    }
    
    // 檢查標籤類型
    if (fields[1].trimmed().isEmpty()) {
        return false;
    }
    
    return true;
}
