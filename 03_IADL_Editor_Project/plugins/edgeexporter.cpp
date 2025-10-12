#include "edgeexporter.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QDateTime>

EdgeExporter::EdgeExporter(QObject *parent)
    : QObject(parent)
{
}

EdgeExporter::~EdgeExporter()
{
}

ImportExportInterface::SystemType EdgeExporter::systemType() const
{
    return SystemType::EDGE;
}

QString EdgeExporter::systemName() const
{
    return "AVEVA EDGE";
}

QString EdgeExporter::systemDescription() const
{
    return "AVEVA EDGE (InduSoft Web Studio) - Tag Database Import/Export";
}

QStringList EdgeExporter::supportedFileExtensions() const
{
    return QStringList() << "xml" << "csv" << "txt";
}

bool EdgeExporter::validateFile(const QString &filePath)
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

QList<AssetData> EdgeExporter::importAssets(
    const QString &filePath, 
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始匯入 EDGE 標籤資料庫..."));
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        m_lastError = tr("檔案不存在: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return QList<AssetData>();
    }
    
    emit progressUpdated(10, tr("正在解析 EDGE 檔案..."));
    
    QList<AssetData> assets;
    
    QString ext = fileInfo.suffix().toLower();
    if (ext == "xml") {
        assets = parseEdgeXml(filePath, options);
    } else if (ext == "csv" || ext == "txt") {
        assets = parseEdgeCsv(filePath, options);
    }
    
    emit progressUpdated(100, tr("匯入完成，共匯入 %1 個標籤").arg(assets.size()));
    
    return assets;
}

QList<AssetData> EdgeExporter::parseEdgeXml(
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
                if (attributes.hasAttribute("Name")) {
                    currentTag["name"] = attributes.value("Name").toString();
                }
                if (attributes.hasAttribute("Type")) {
                    currentTag["type"] = attributes.value("Type").toString();
                }
                if (attributes.hasAttribute("Scope")) {
                    currentTag["scope"] = attributes.value("Scope").toString();
                }
                
            } else if (!currentTag.isEmpty()) {
                // 讀取標籤的其他屬性
                QString value = xml.readElementText();
                currentTag[currentElement.toLower()] = value;
            }
            
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name().toString() == "Tag" && !currentTag.isEmpty()) {
                if (validateTagData(currentTag)) {
                    AssetData asset = convertEdgeTagToAsset(currentTag);
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

QList<AssetData> EdgeExporter::parseEdgeCsv(
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
        
        if (fields.size() < headers.size()) {
            qWarning() << "Invalid tag data at line" << lineCount;
            continue;
        }
        
        QVariantMap tag;
        for (int i = 0; i < headers.size() && i < fields.size(); ++i) {
            tag[headers[i].toLower().trimmed()] = fields[i].trimmed();
        }
        
        if (validateTagData(tag)) {
            AssetData asset = convertEdgeTagToAsset(tag);
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

AssetData EdgeExporter::convertEdgeTagToAsset(const QVariantMap &tag)
{
    AssetData asset;
    
    asset.id = tag.value("name").toString();
    asset.name = asset.id;
    asset.type = tag.value("type", "Real").toString();
    
    // 基本屬性
    asset.properties["edge_tag_name"] = asset.id;
    asset.properties["edge_tag_type"] = asset.type;
    asset.properties["edge_tag_scope"] = tag.value("scope", "Server").toString();
    
    // 其他屬性
    if (tag.contains("description")) {
        asset.properties["description"] = tag.value("description");
    }
    if (tag.contains("address")) {
        asset.properties["address"] = tag.value("address");
    }
    if (tag.contains("minvalue")) {
        asset.properties["min_value"] = tag.value("minvalue");
    }
    if (tag.contains("maxvalue")) {
        asset.properties["max_value"] = tag.value("maxvalue");
    }
    if (tag.contains("initialvalue")) {
        asset.properties["initial_value"] = tag.value("initialvalue");
    }
    if (tag.contains("units")) {
        asset.properties["units"] = tag.value("units");
    }
    if (tag.contains("alarmhigh")) {
        asset.properties["alarm_high"] = tag.value("alarmhigh");
    }
    if (tag.contains("alarmlow")) {
        asset.properties["alarm_low"] = tag.value("alarmlow");
    }
    
    return asset;
}

bool EdgeExporter::exportAssets(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始匯出到 EDGE 標籤資料庫..."));
    
    if (assets.isEmpty()) {
        m_lastError = tr("沒有資產可匯出");
        emit errorOccurred(m_lastError);
        return false;
    }
    
    if (!validateFile(filePath)) {
        return false;
    }
    
    emit progressUpdated(10, tr("正在生成 EDGE 檔案..."));
    
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();
    
    bool success = false;
    if (ext == "xml") {
        success = generateEdgeXml(filePath, assets, options);
    } else if (ext == "csv" || ext == "txt") {
        success = generateEdgeCsv(filePath, assets, options);
    }
    
    if (success) {
        emit progressUpdated(100, tr("匯出完成，共匯出 %1 個標籤").arg(assets.size()));
    } else {
        emit errorOccurred(tr("匯出失敗"));
    }
    
    return success;
}

bool EdgeExporter::generateEdgeXml(
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
    
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    
    // 寫入 XML 宣告
    xml.writeStartDocument();
    
    // 寫入根元素
    xml.writeStartElement("EdgeProject");
    xml.writeAttribute("Version", "1.0");
    xml.writeAttribute("ExportedFrom", "IADL Editor");
    xml.writeAttribute("ExportDate", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    // 寫入標籤資料庫
    xml.writeStartElement("TagDatabase");
    
    emit progressUpdated(30, tr("正在寫入標籤資訊..."));
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        xml.writeStartElement("Tag");
        xml.writeAttribute("Name", asset.properties.value("edge_tag_name", asset.id).toString());
        xml.writeAttribute("Type", asset.properties.value("edge_tag_type", "Real").toString());
        xml.writeAttribute("Scope", asset.properties.value("edge_tag_scope", "Server").toString());
        
        // 寫入描述
        if (asset.properties.contains("description")) {
            xml.writeTextElement("Description", asset.properties["description"].toString());
        }
        
        // 寫入位址
        if (asset.properties.contains("address")) {
            xml.writeTextElement("Address", asset.properties["address"].toString());
        }
        
        // 寫入數值範圍
        if (asset.properties.contains("min_value")) {
            xml.writeTextElement("MinValue", asset.properties["min_value"].toString());
        }
        if (asset.properties.contains("max_value")) {
            xml.writeTextElement("MaxValue", asset.properties["max_value"].toString());
        }
        
        // 寫入初始值
        if (asset.properties.contains("initial_value")) {
            xml.writeTextElement("InitialValue", asset.properties["initial_value"].toString());
        }
        
        // 寫入單位
        if (asset.properties.contains("units")) {
            xml.writeTextElement("Units", asset.properties["units"].toString());
        }
        
        // 寫入警報設定
        if (asset.properties.contains("alarm_high")) {
            xml.writeTextElement("AlarmHigh", asset.properties["alarm_high"].toString());
        }
        if (asset.properties.contains("alarm_low")) {
            xml.writeTextElement("AlarmLow", asset.properties["alarm_low"].toString());
        }
        
        xml.writeEndElement(); // Tag
        
        if ((i + 1) % 10 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, tr("已寫入 %1/%2 個標籤...").arg(i + 1).arg(assets.size()));
        }
    }
    
    xml.writeEndElement(); // TagDatabase
    xml.writeEndElement(); // EdgeProject
    xml.writeEndDocument();
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

bool EdgeExporter::generateEdgeCsv(
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
    out << "Name,Type,Scope,Description,Address,MinValue,MaxValue,InitialValue,Units,AlarmHigh,AlarmLow\n";
    
    emit progressUpdated(30, tr("正在寫入標籤資訊..."));
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        out << asset.properties.value("edge_tag_name", asset.id).toString() << ",";
        out << asset.properties.value("edge_tag_type", "Real").toString() << ",";
        out << asset.properties.value("edge_tag_scope", "Server").toString() << ",";
        out << "\"" << asset.properties.value("description", asset.name).toString() << "\",";
        out << asset.properties.value("address", "").toString() << ",";
        out << asset.properties.value("min_value", "0").toString() << ",";
        out << asset.properties.value("max_value", "100").toString() << ",";
        out << asset.properties.value("initial_value", "0").toString() << ",";
        out << asset.properties.value("units", "").toString() << ",";
        out << asset.properties.value("alarm_high", "").toString() << ",";
        out << asset.properties.value("alarm_low", "").toString() << "\n";
        
        if ((i + 1) % 100 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, tr("已寫入 %1/%2 個標籤...").arg(i + 1).arg(assets.size()));
        }
    }
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

QString EdgeExporter::convertAssetToEdgeTag(const AssetData &asset)
{
    QString result;
    QTextStream out(&result);
    
    out << "<Tag Name=\"" << asset.id << "\" ";
    out << "Type=\"" << asset.properties.value("edge_tag_type", "Real").toString() << "\" ";
    out << "Scope=\"" << asset.properties.value("edge_tag_scope", "Server").toString() << "\">\n";
    
    if (asset.properties.contains("description")) {
        out << "  <Description>" << asset.properties["description"].toString() << "</Description>\n";
    }
    
    out << "</Tag>\n";
    
    return result;
}

QString EdgeExporter::getTagTypeString(TagType type) const
{
    switch (type) {
        case Boolean: return "Boolean";
        case Integer: return "Integer";
        case Real: return "Real";
        case String: return "String";
        case DateTime: return "DateTime";
        case Quality: return "Quality";
        case ArrayBoolean: return "ArrayBoolean";
        case ArrayInteger: return "ArrayInteger";
        case ArrayReal: return "ArrayReal";
        case ArrayString: return "ArrayString";
        default: return "Real";
    }
}

EdgeExporter::TagType EdgeExporter::getTagTypeFromString(const QString &typeStr) const
{
    if (typeStr == "Boolean") return Boolean;
    if (typeStr == "Integer") return Integer;
    if (typeStr == "Real") return Real;
    if (typeStr == "String") return String;
    if (typeStr == "DateTime") return DateTime;
    if (typeStr == "Quality") return Quality;
    if (typeStr == "ArrayBoolean") return ArrayBoolean;
    if (typeStr == "ArrayInteger") return ArrayInteger;
    if (typeStr == "ArrayReal") return ArrayReal;
    if (typeStr == "ArrayString") return ArrayString;
    return Real;
}

QString EdgeExporter::getTagScopeString(TagScope scope) const
{
    switch (scope) {
        case Server: return "Server";
        case Local: return "Local";
        case Public: return "Public";
        case Class: return "Class";
        default: return "Server";
    }
}

EdgeExporter::TagScope EdgeExporter::getTagScopeFromString(const QString &scopeStr) const
{
    if (scopeStr == "Server") return Server;
    if (scopeStr == "Local") return Local;
    if (scopeStr == "Public") return Public;
    if (scopeStr == "Class") return Class;
    return Server;
}

bool EdgeExporter::validateTagData(const QVariantMap &tag) const
{
    if (!tag.contains("name") || tag.value("name").toString().isEmpty()) {
        return false;
    }
    
    if (!tag.contains("type") || tag.value("type").toString().isEmpty()) {
        return false;
    }
    
    return true;
}
