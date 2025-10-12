#include "ignitionexporter.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QDateTime>

IgnitionExporter::IgnitionExporter(QObject *parent)
    : QObject(parent)
{
}

IgnitionExporter::~IgnitionExporter()
{
}

ImportExportInterface::SystemType IgnitionExporter::systemType() const
{
    return SystemType::IGNITION;
}

QString IgnitionExporter::systemName() const
{
    return "Ignition";
}

QString IgnitionExporter::systemDescription() const
{
    return "Ignition SCADA/MES/IIoT Platform - Tag and Resource Import/Export";
}

QStringList IgnitionExporter::supportedFileExtensions() const
{
    return QStringList() << "json" << "xml" << "csv" << "txt";
}

bool IgnitionExporter::validateFile(const QString &filePath)
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

QList<AssetData> IgnitionExporter::importAssets(
    const QString &filePath, 
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始匯入 Ignition 標籤資料..."));
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        m_lastError = tr("檔案不存在: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return QList<AssetData>();
    }
    
    emit progressUpdated(10, tr("正在解析 Ignition 檔案..."));
    
    QList<AssetData> assets;
    
    QString ext = fileInfo.suffix().toLower();
    if (ext == "json") {
        assets = parseIgnitionJson(filePath, options);
    } else if (ext == "xml") {
        assets = parseIgnitionXml(filePath, options);
    } else if (ext == "csv" || ext == "txt") {
        assets = parseIgnitionCsv(filePath, options);
    }
    
    emit progressUpdated(100, tr("匯入完成，共匯入 %1 個標籤").arg(assets.size()));
    
    return assets;
}

QList<AssetData> IgnitionExporter::parseIgnitionJson(
    const QString &filePath,
    const QMap<QString, QVariant> &options)
{
    Q_UNUSED(options);
    
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = tr("無法開啟檔案: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return assets;
    }
    
    QByteArray jsonData = file.readAll();
    file.close();
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        m_lastError = tr("JSON 解析錯誤: %1").arg(parseError.errorString());
        emit errorOccurred(m_lastError);
        return assets;
    }
    
    if (doc.isObject()) {
        QJsonObject root = doc.object();
        
        // 檢查是否有 tags 陣列
        if (root.contains("tags") && root["tags"].isArray()) {
            QJsonArray tagsArray = root["tags"].toArray();
            
            for (int i = 0; i < tagsArray.size(); ++i) {
                if (tagsArray[i].isObject()) {
                    parseJsonTagObject(tagsArray[i].toObject(), assets);
                }
                
                if ((i + 1) % 100 == 0) {
                    int progress = 10 + ((i + 1) * 80 / tagsArray.size());
                    emit progressUpdated(progress, tr("已處理 %1/%2 個標籤...").arg(i + 1).arg(tagsArray.size()));
                }
            }
        }
    } else if (doc.isArray()) {
        // 直接是標籤陣列
        QJsonArray tagsArray = doc.array();
        
        for (int i = 0; i < tagsArray.size(); ++i) {
            if (tagsArray[i].isObject()) {
                parseJsonTagObject(tagsArray[i].toObject(), assets);
            }
            
            if ((i + 1) % 100 == 0) {
                int progress = 10 + ((i + 1) * 80 / tagsArray.size());
                emit progressUpdated(progress, tr("已處理 %1/%2 個標籤...").arg(i + 1).arg(tagsArray.size()));
            }
        }
    }
    
    return assets;
}

void IgnitionExporter::parseJsonTagObject(const QJsonObject &obj, QList<AssetData> &assets)
{
    QVariantMap tag = obj.toVariantMap();
    
    if (validateTagData(tag)) {
        AssetData asset = convertIgnitionTagToAsset(tag);
        if (!asset.id.isEmpty()) {
            assets.append(asset);
        }
    }
    
    // 遞迴處理子標籤
    if (obj.contains("tags") && obj["tags"].isArray()) {
        QJsonArray childTags = obj["tags"].toArray();
        for (int i = 0; i < childTags.size(); ++i) {
            if (childTags[i].isObject()) {
                parseJsonTagObject(childTags[i].toObject(), assets);
            }
        }
    }
}

QList<AssetData> IgnitionExporter::parseIgnitionXml(
    const QString &filePath,
    const QMap<QString, QVariant> &options)
{
    Q_UNUSED(options);
    
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = tr("無法開啟檔案: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return assets;
    }
    
    QXmlStreamReader xml(&file);
    
    QVariantMap currentTag;
    QString currentElement;
    
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        
        if (token == QXmlStreamReader::StartElement) {
            currentElement = xml.name().toString();
            
            if (currentElement == "Tag") {
                currentTag.clear();
                
                // 讀取標籤屬性
                QXmlStreamAttributes attributes = xml.attributes();
                if (attributes.hasAttribute("name")) {
                    currentTag["name"] = attributes.value("name").toString();
                }
                if (attributes.hasAttribute("tagType")) {
                    currentTag["tagType"] = attributes.value("tagType").toString();
                }
                if (attributes.hasAttribute("dataType")) {
                    currentTag["dataType"] = attributes.value("dataType").toString();
                }
                
            } else if (!currentTag.isEmpty()) {
                // 讀取標籤的其他屬性
                QString value = xml.readElementText();
                currentTag[currentElement.toLower()] = value;
            }
            
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name().toString() == "Tag" && !currentTag.isEmpty()) {
                if (validateTagData(currentTag)) {
                    AssetData asset = convertIgnitionTagToAsset(currentTag);
                    if (!asset.id.isEmpty()) {
                        assets.append(asset);
                    }
                }
                currentTag.clear();
            }
        }
    }
    
    if (xml.hasError()) {
        m_lastError = tr("XML 解析錯誤: %1").arg(xml.errorString());
        emit errorOccurred(m_lastError);
    }
    
    file.close();
    
    return assets;
}

QList<AssetData> IgnitionExporter::parseIgnitionCsv(
    const QString &filePath,
    const QMap<QString, QVariant> &options)
{
    Q_UNUSED(options);
    
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
        
        if (fields.size() < headers.size()) {
            qWarning() << "Invalid tag data at line" << lineCount;
            continue;
        }
        
        QVariantMap tag;
        for (int i = 0; i < headers.size() && i < fields.size(); ++i) {
            tag[headers[i].toLower().trimmed()] = fields[i].trimmed();
        }
        
        if (validateTagData(tag)) {
            AssetData asset = convertIgnitionTagToAsset(tag);
            if (!asset.id.isEmpty()) {
                assets.append(asset);
            }
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

AssetData IgnitionExporter::convertIgnitionTagToAsset(const QVariantMap &tag)
{
    AssetData asset;
    
    asset.id = tag.value("name").toString();
    asset.name = asset.id;
    asset.type = tag.value("datatype", tag.value("dataType", "Int4")).toString();
    
    // 基本屬性
    asset.properties["ignition_tag_name"] = asset.id;
    asset.properties["ignition_tag_type"] = tag.value("tagtype", tag.value("tagType", "Memory")).toString();
    asset.properties["ignition_data_type"] = asset.type;
    
    // 其他屬性
    if (tag.contains("description")) {
        asset.properties["description"] = tag.value("description");
    }
    if (tag.contains("value")) {
        asset.properties["value"] = tag.value("value");
    }
    if (tag.contains("opcitempath") || tag.contains("opcItemPath")) {
        asset.properties["opc_item_path"] = tag.value("opcitempath", tag.value("opcItemPath"));
    }
    if (tag.contains("opcserver") || tag.contains("opcServer")) {
        asset.properties["opc_server"] = tag.value("opcserver", tag.value("opcServer"));
    }
    if (tag.contains("engunit") || tag.contains("engUnit")) {
        asset.properties["eng_unit"] = tag.value("engunit", tag.value("engUnit"));
    }
    if (tag.contains("englow") || tag.contains("engLow")) {
        asset.properties["eng_low"] = tag.value("englow", tag.value("engLow"));
    }
    if (tag.contains("enghigh") || tag.contains("engHigh")) {
        asset.properties["eng_high"] = tag.value("enghigh", tag.value("engHigh"));
    }
    if (tag.contains("scaledmin") || tag.contains("scaledMin")) {
        asset.properties["scaled_min"] = tag.value("scaledmin", tag.value("scaledMin"));
    }
    if (tag.contains("scaledmax") || tag.contains("scaledMax")) {
        asset.properties["scaled_max"] = tag.value("scaledmax", tag.value("scaledMax"));
    }
    if (tag.contains("enabled")) {
        asset.properties["enabled"] = tag.value("enabled");
    }
    if (tag.contains("historyprovider") || tag.contains("historyProvider")) {
        asset.properties["history_provider"] = tag.value("historyprovider", tag.value("historyProvider"));
    }
    if (tag.contains("historyenabled") || tag.contains("historyEnabled")) {
        asset.properties["history_enabled"] = tag.value("historyenabled", tag.value("historyEnabled"));
    }
    if (tag.contains("historysamplerate") || tag.contains("historySampleRate")) {
        asset.properties["history_sample_rate"] = tag.value("historysamplerate", tag.value("historySampleRate"));
    }
    
    return asset;
}

bool IgnitionExporter::exportAssets(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始匯出到 Ignition 標籤資料..."));
    
    if (assets.isEmpty()) {
        m_lastError = tr("沒有資產可匯出");
        emit errorOccurred(m_lastError);
        return false;
    }
    
    if (!validateFile(filePath)) {
        return false;
    }
    
    emit progressUpdated(10, tr("正在生成 Ignition 檔案..."));
    
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();
    
    bool success = false;
    if (ext == "json") {
        success = generateIgnitionJson(filePath, assets, options);
    } else if (ext == "xml") {
        success = generateIgnitionXml(filePath, assets, options);
    } else if (ext == "csv" || ext == "txt") {
        success = generateIgnitionCsv(filePath, assets, options);
    }
    
    if (success) {
        emit progressUpdated(100, tr("匯出完成，共匯出 %1 個標籤").arg(assets.size()));
    } else {
        emit errorOccurred(tr("匯出失敗"));
    }
    
    return success;
}

bool IgnitionExporter::generateIgnitionJson(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    Q_UNUSED(options);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = tr("無法建立檔案: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return false;
    }
    
    emit progressUpdated(30, tr("正在寫入標籤資訊..."));
    
    QJsonObject root;
    root["name"] = "IADL Editor Export";
    root["exportDate"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    root["tagCount"] = assets.size();
    
    QJsonArray tagsArray;
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        QJsonObject tagObj = generateJsonTagObject(asset);
        tagsArray.append(tagObj);
        
        if ((i + 1) % 100 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, tr("已寫入 %1/%2 個標籤...").arg(i + 1).arg(assets.size()));
        }
    }
    
    root["tags"] = tagsArray;
    
    QJsonDocument doc(root);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

QJsonObject IgnitionExporter::generateJsonTagObject(const AssetData &asset)
{
    QJsonObject obj;
    
    obj["name"] = asset.properties.value("ignition_tag_name", asset.id).toString();
    obj["tagType"] = asset.properties.value("ignition_tag_type", "Memory").toString();
    obj["dataType"] = asset.properties.value("ignition_data_type", "Int4").toString();
    
    if (asset.properties.contains("description")) {
        obj["description"] = asset.properties["description"].toString();
    }
    
    if (asset.properties.contains("value")) {
        obj["value"] = asset.properties["value"].toString();
    }
    
    if (asset.properties.contains("opc_item_path")) {
        obj["opcItemPath"] = asset.properties["opc_item_path"].toString();
    }
    
    if (asset.properties.contains("opc_server")) {
        obj["opcServer"] = asset.properties["opc_server"].toString();
    }
    
    if (asset.properties.contains("eng_unit")) {
        obj["engUnit"] = asset.properties["eng_unit"].toString();
    }
    
    if (asset.properties.contains("eng_low")) {
        obj["engLow"] = asset.properties["eng_low"].toDouble();
    }
    
    if (asset.properties.contains("eng_high")) {
        obj["engHigh"] = asset.properties["eng_high"].toDouble();
    }
    
    if (asset.properties.contains("scaled_min")) {
        obj["scaledMin"] = asset.properties["scaled_min"].toDouble();
    }
    
    if (asset.properties.contains("scaled_max")) {
        obj["scaledMax"] = asset.properties["scaled_max"].toDouble();
    }
    
    if (asset.properties.contains("enabled")) {
        obj["enabled"] = asset.properties["enabled"].toBool();
    }
    
    if (asset.properties.contains("history_provider")) {
        obj["historyProvider"] = asset.properties["history_provider"].toString();
    }
    
    if (asset.properties.contains("history_enabled")) {
        obj["historyEnabled"] = asset.properties["history_enabled"].toBool();
    }
    
    if (asset.properties.contains("history_sample_rate")) {
        obj["historySampleRate"] = asset.properties["history_sample_rate"].toInt();
    }
    
    return obj;
}

bool IgnitionExporter::generateIgnitionXml(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    Q_UNUSED(options);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = tr("無法建立檔案: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return false;
    }
    
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    
    // 寫入 XML 宣告
    xml.writeStartDocument();
    
    // 寫入根元素
    xml.writeStartElement("IgnitionExport");
    xml.writeAttribute("Version", "1.0");
    xml.writeAttribute("ExportedFrom", "IADL Editor");
    xml.writeAttribute("ExportDate", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    // 寫入標籤資料庫
    xml.writeStartElement("Tags");
    
    emit progressUpdated(30, tr("正在寫入標籤資訊..."));
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        xml.writeStartElement("Tag");
        xml.writeAttribute("name", asset.properties.value("ignition_tag_name", asset.id).toString());
        xml.writeAttribute("tagType", asset.properties.value("ignition_tag_type", "Memory").toString());
        xml.writeAttribute("dataType", asset.properties.value("ignition_data_type", "Int4").toString());
        
        // 寫入描述
        if (asset.properties.contains("description")) {
            xml.writeTextElement("Description", asset.properties["description"].toString());
        }
        
        // 寫入值
        if (asset.properties.contains("value")) {
            xml.writeTextElement("Value", asset.properties["value"].toString());
        }
        
        // 寫入 OPC 設定
        if (asset.properties.contains("opc_item_path")) {
            xml.writeTextElement("OpcItemPath", asset.properties["opc_item_path"].toString());
        }
        if (asset.properties.contains("opc_server")) {
            xml.writeTextElement("OpcServer", asset.properties["opc_server"].toString());
        }
        
        // 寫入工程單位
        if (asset.properties.contains("eng_unit")) {
            xml.writeTextElement("EngUnit", asset.properties["eng_unit"].toString());
        }
        if (asset.properties.contains("eng_low")) {
            xml.writeTextElement("EngLow", asset.properties["eng_low"].toString());
        }
        if (asset.properties.contains("eng_high")) {
            xml.writeTextElement("EngHigh", asset.properties["eng_high"].toString());
        }
        
        // 寫入縮放範圍
        if (asset.properties.contains("scaled_min")) {
            xml.writeTextElement("ScaledMin", asset.properties["scaled_min"].toString());
        }
        if (asset.properties.contains("scaled_max")) {
            xml.writeTextElement("ScaledMax", asset.properties["scaled_max"].toString());
        }
        
        // 寫入啟用狀態
        if (asset.properties.contains("enabled")) {
            xml.writeTextElement("Enabled", asset.properties["enabled"].toBool() ? "true" : "false");
        }
        
        // 寫入歷史記錄設定
        if (asset.properties.contains("history_provider")) {
            xml.writeTextElement("HistoryProvider", asset.properties["history_provider"].toString());
        }
        if (asset.properties.contains("history_enabled")) {
            xml.writeTextElement("HistoryEnabled", asset.properties["history_enabled"].toBool() ? "true" : "false");
        }
        if (asset.properties.contains("history_sample_rate")) {
            xml.writeTextElement("HistorySampleRate", asset.properties["history_sample_rate"].toString());
        }
        
        xml.writeEndElement(); // Tag
        
        if ((i + 1) % 10 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, tr("已寫入 %1/%2 個標籤...").arg(i + 1).arg(assets.size()));
        }
    }
    
    xml.writeEndElement(); // Tags
    xml.writeEndElement(); // IgnitionExport
    xml.writeEndDocument();
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

bool IgnitionExporter::generateIgnitionCsv(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    Q_UNUSED(options);
    
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        m_lastError = tr("無法建立檔案: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return false;
    }
    
    QTextStream out(&file);
    
    // 寫入標頭
    out << "Name,TagType,DataType,Description,Value,OpcItemPath,OpcServer,EngUnit,EngLow,EngHigh,ScaledMin,ScaledMax,Enabled,HistoryProvider,HistoryEnabled,HistorySampleRate\n";
    
    emit progressUpdated(30, tr("正在寫入標籤資訊..."));
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        out << asset.properties.value("ignition_tag_name", asset.id).toString() << ",";
        out << asset.properties.value("ignition_tag_type", "Memory").toString() << ",";
        out << asset.properties.value("ignition_data_type", "Int4").toString() << ",";
        out << "\"" << asset.properties.value("description", asset.name).toString() << "\",";
        out << asset.properties.value("value", "0").toString() << ",";
        out << asset.properties.value("opc_item_path", "").toString() << ",";
        out << asset.properties.value("opc_server", "").toString() << ",";
        out << asset.properties.value("eng_unit", "").toString() << ",";
        out << asset.properties.value("eng_low", "0").toString() << ",";
        out << asset.properties.value("eng_high", "100").toString() << ",";
        out << asset.properties.value("scaled_min", "0").toString() << ",";
        out << asset.properties.value("scaled_max", "100").toString() << ",";
        out << (asset.properties.value("enabled", true).toBool() ? "true" : "false") << ",";
        out << asset.properties.value("history_provider", "").toString() << ",";
        out << (asset.properties.value("history_enabled", false).toBool() ? "true" : "false") << ",";
        out << asset.properties.value("history_sample_rate", "1000").toString() << "\n";
        
        if ((i + 1) % 100 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, tr("已寫入 %1/%2 個標籤...").arg(i + 1).arg(assets.size()));
        }
    }
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

QVariantMap IgnitionExporter::convertAssetToIgnitionTag(const AssetData &asset)
{
    QVariantMap tag;
    
    tag["name"] = asset.properties.value("ignition_tag_name", asset.id);
    tag["tagType"] = asset.properties.value("ignition_tag_type", "Memory");
    tag["dataType"] = asset.properties.value("ignition_data_type", "Int4");
    
    if (asset.properties.contains("description")) {
        tag["description"] = asset.properties["description"];
    }
    
    return tag;
}

QString IgnitionExporter::getTagTypeString(TagType type) const
{
    switch (type) {
        case OPC: return "OPC";
        case Memory: return "Memory";
        case Expression: return "Expression";
        case Query: return "Query";
        case Derived: return "Derived";
        case UDT: return "UDT";
        case Folder: return "Folder";
        default: return "Memory";
    }
}

IgnitionExporter::TagType IgnitionExporter::getTagTypeFromString(const QString &typeStr) const
{
    if (typeStr == "OPC") return OPC;
    if (typeStr == "Memory") return Memory;
    if (typeStr == "Expression") return Expression;
    if (typeStr == "Query") return Query;
    if (typeStr == "Derived") return Derived;
    if (typeStr == "UDT") return UDT;
    if (typeStr == "Folder") return Folder;
    return Memory;
}

QString IgnitionExporter::getDataTypeString(DataType type) const
{
    switch (type) {
        case Int1: return "Int1";
        case Int2: return "Int2";
        case Int4: return "Int4";
        case Int8: return "Int8";
        case Float4: return "Float4";
        case Float8: return "Float8";
        case String: return "String";
        case DateTime: return "DateTime";
        case DataSet: return "DataSet";
        case Document: return "Document";
        default: return "Int4";
    }
}

IgnitionExporter::DataType IgnitionExporter::getDataTypeFromString(const QString &typeStr) const
{
    if (typeStr == "Int1") return Int1;
    if (typeStr == "Int2") return Int2;
    if (typeStr == "Int4") return Int4;
    if (typeStr == "Int8") return Int8;
    if (typeStr == "Float4") return Float4;
    if (typeStr == "Float8") return Float8;
    if (typeStr == "String") return String;
    if (typeStr == "DateTime") return DateTime;
    if (typeStr == "DataSet") return DataSet;
    if (typeStr == "Document") return Document;
    return Int4;
}

QString IgnitionExporter::getTagQualityString(TagQuality quality) const
{
    switch (quality) {
        case Good: return "Good";
        case Bad: return "Bad";
        case BadNotConnected: return "Bad_NotConnected";
        case BadDeviceFailure: return "Bad_DeviceFailure";
        case BadSensorFailure: return "Bad_SensorFailure";
        case BadLastKnownValue: return "Bad_LastKnownValue";
        case BadCommFailure: return "Bad_CommFailure";
        case BadOutOfService: return "Bad_OutOfService";
        case Uncertain: return "Uncertain";
        case UncertainLastUsableValue: return "Uncertain_LastUsableValue";
        case UncertainSensorNotAccurate: return "Uncertain_SensorNotAccurate";
        case UncertainEUExceeded: return "Uncertain_EUExceeded";
        case UncertainSubNormal: return "Uncertain_SubNormal";
        default: return "Good";
    }
}

IgnitionExporter::TagQuality IgnitionExporter::getTagQualityFromString(const QString &qualityStr) const
{
    if (qualityStr == "Good") return Good;
    if (qualityStr == "Bad") return Bad;
    if (qualityStr == "Bad_NotConnected") return BadNotConnected;
    if (qualityStr == "Bad_DeviceFailure") return BadDeviceFailure;
    if (qualityStr == "Bad_SensorFailure") return BadSensorFailure;
    if (qualityStr == "Bad_LastKnownValue") return BadLastKnownValue;
    if (qualityStr == "Bad_CommFailure") return BadCommFailure;
    if (qualityStr == "Bad_OutOfService") return BadOutOfService;
    if (qualityStr == "Uncertain") return Uncertain;
    if (qualityStr == "Uncertain_LastUsableValue") return UncertainLastUsableValue;
    if (qualityStr == "Uncertain_SensorNotAccurate") return UncertainSensorNotAccurate;
    if (qualityStr == "Uncertain_EUExceeded") return UncertainEUExceeded;
    if (qualityStr == "Uncertain_SubNormal") return UncertainSubNormal;
    return Good;
}

QString IgnitionExporter::getTagSecurityString(TagSecurity security) const
{
    switch (security) {
        case ReadOnly: return "ReadOnly";
        case ReadWrite: return "ReadWrite";
        case Custom: return "Custom";
        default: return "ReadWrite";
    }
}

IgnitionExporter::TagSecurity IgnitionExporter::getTagSecurityFromString(const QString &securityStr) const
{
    if (securityStr == "ReadOnly") return ReadOnly;
    if (securityStr == "ReadWrite") return ReadWrite;
    if (securityStr == "Custom") return Custom;
    return ReadWrite;
}

bool IgnitionExporter::validateTagData(const QVariantMap &tag) const
{
    if (!tag.contains("name") || tag.value("name").toString().isEmpty()) {
        return false;
    }
    
    return true;
}
