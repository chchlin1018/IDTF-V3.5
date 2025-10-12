#include "systemplatformexporter.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QDateTime>

SystemPlatformExporter::SystemPlatformExporter(QObject *parent)
    : QObject(parent)
{
}

SystemPlatformExporter::~SystemPlatformExporter()
{
}

ImportExportInterface::SystemType SystemPlatformExporter::systemType() const
{
    return SystemType::SYSTEM_PLATFORM;
}

QString SystemPlatformExporter::systemName() const
{
    return "AVEVA System Platform";
}

QString SystemPlatformExporter::systemDescription() const
{
    return "AVEVA System Platform - Object Model Import/Export";
}

QStringList SystemPlatformExporter::supportedFileExtensions() const
{
    return QStringList() << "xml" << "aaPlatform";
}

bool SystemPlatformExporter::validateFile(const QString &filePath)
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

QList<AssetData> SystemPlatformExporter::importAssets(
    const QString &filePath, 
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始匯入 System Platform 物件模型..."));
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        m_lastError = tr("檔案不存在: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return QList<AssetData>();
    }
    
    emit progressUpdated(10, tr("正在解析 System Platform XML 檔案..."));
    
    QList<AssetData> assets = parseSystemPlatformXml(filePath, options);
    
    emit progressUpdated(100, tr("匯入完成，共匯入 %1 個物件").arg(assets.size()));
    
    return assets;
}

QList<AssetData> SystemPlatformExporter::parseSystemPlatformXml(
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
    
    AssetData currentAsset;
    QString currentElement;
    QMap<QString, QVariant> currentAttributes;
    
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        
        if (token == QXmlStreamReader::StartElement) {
            currentElement = xml.name().toString();
            
            if (currentElement == "Object") {
                currentAsset = AssetData();
                currentAttributes.clear();
                
                // 讀取物件屬性
                QXmlStreamAttributes attributes = xml.attributes();
                if (attributes.hasAttribute("Name")) {
                    currentAsset.name = attributes.value("Name").toString();
                    currentAsset.id = currentAsset.name;
                }
                if (attributes.hasAttribute("Type")) {
                    currentAsset.type = attributes.value("Type").toString();
                }
                if (attributes.hasAttribute("Template")) {
                    currentAsset.properties["template"] = attributes.value("Template").toString();
                }
                
            } else if (currentElement == "Attribute" && !currentAsset.id.isEmpty()) {
                QXmlStreamAttributes attributes = xml.attributes();
                QString attrName = attributes.value("Name").toString();
                QString attrValue = xml.readElementText();
                currentAttributes[attrName] = attrValue;
            }
            
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name().toString() == "Object" && !currentAsset.id.isEmpty()) {
                // 將屬性加入資產
                for (auto it = currentAttributes.begin(); it != currentAttributes.end(); ++it) {
                    currentAsset.properties[it.key()] = it.value();
                }
                
                // 加入 System Platform 特定屬性
                currentAsset.properties["system_platform_object"] = true;
                
                assets.append(currentAsset);
                currentAsset = AssetData();
                currentAttributes.clear();
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

AssetData SystemPlatformExporter::convertSystemPlatformObjectToAsset(const QVariantMap &object)
{
    AssetData asset;
    
    asset.id = object.value("name").toString();
    asset.name = asset.id;
    asset.type = object.value("type").toString();
    
    QVariantMap attributes = object.value("attributes").toMap();
    for (auto it = attributes.begin(); it != attributes.end(); ++it) {
        asset.properties[it.key()] = it.value();
    }
    
    return asset;
}

bool SystemPlatformExporter::exportAssets(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始匯出到 System Platform 物件模型..."));
    
    if (assets.isEmpty()) {
        m_lastError = tr("沒有資產可匯出");
        emit errorOccurred(m_lastError);
        return false;
    }
    
    if (!validateFile(filePath)) {
        return false;
    }
    
    emit progressUpdated(10, tr("正在生成 System Platform XML 檔案..."));
    
    bool success = generateSystemPlatformXml(filePath, assets, options);
    
    if (success) {
        emit progressUpdated(100, tr("匯出完成，共匯出 %1 個物件").arg(assets.size()));
    } else {
        emit errorOccurred(tr("匯出失敗"));
    }
    
    return success;
}

bool SystemPlatformExporter::generateSystemPlatformXml(
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
    xml.writeStartElement("aaPlatformExport");
    xml.writeAttribute("Version", "1.0");
    xml.writeAttribute("ExportedFrom", "IADL Editor");
    xml.writeAttribute("ExportDate", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    // 寫入物件模型
    xml.writeStartElement("ObjectModel");
    
    emit progressUpdated(30, tr("正在寫入物件資訊..."));
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        xml.writeStartElement("Object");
        xml.writeAttribute("Name", asset.name);
        xml.writeAttribute("Type", asset.type);
        
        // 寫入範本資訊
        if (asset.properties.contains("template")) {
            xml.writeAttribute("Template", asset.properties["template"].toString());
        }
        
        // 寫入屬性
        xml.writeStartElement("Attributes");
        
        for (auto it = asset.properties.begin(); it != asset.properties.end(); ++it) {
            // 跳過內部屬性
            if (it.key() == "template" || it.key() == "system_platform_object") {
                continue;
            }
            
            xml.writeStartElement("Attribute");
            xml.writeAttribute("Name", it.key());
            xml.writeAttribute("Type", it.value().typeName());
            xml.writeCharacters(it.value().toString());
            xml.writeEndElement(); // Attribute
        }
        
        xml.writeEndElement(); // Attributes
        
        xml.writeEndElement(); // Object
        
        if ((i + 1) % 10 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, tr("已寫入 %1/%2 個物件...").arg(i + 1).arg(assets.size()));
        }
    }
    
    xml.writeEndElement(); // ObjectModel
    
    xml.writeEndElement(); // aaPlatformExport
    xml.writeEndDocument();
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

QString SystemPlatformExporter::convertAssetToSystemPlatformObject(const AssetData &asset)
{
    QString result;
    QTextStream out(&result);
    
    out << "<Object Name=\"" << asset.name << "\" Type=\"" << asset.type << "\">\n";
    out << "  <Attributes>\n";
    
    for (auto it = asset.properties.begin(); it != asset.properties.end(); ++it) {
        out << "    <Attribute Name=\"" << it.key() << "\">";
        out << it.value().toString();
        out << "</Attribute>\n";
    }
    
    out << "  </Attributes>\n";
    out << "</Object>\n";
    
    return result;
}

QString SystemPlatformExporter::getObjectTypeString(ObjectType type) const
{
    switch (type) {
        case UserDefined: return "UserDefined";
        case ApplicationObject: return "ApplicationObject";
        case AreaObject: return "AreaObject";
        case EquipmentObject: return "EquipmentObject";
        case DeviceObject: return "DeviceObject";
        case TemplateObject: return "TemplateObject";
        case InstanceObject: return "InstanceObject";
        default: return "UserDefined";
    }
}

SystemPlatformExporter::ObjectType SystemPlatformExporter::getObjectTypeFromString(const QString &typeStr) const
{
    if (typeStr == "UserDefined") return UserDefined;
    if (typeStr == "ApplicationObject") return ApplicationObject;
    if (typeStr == "AreaObject") return AreaObject;
    if (typeStr == "EquipmentObject") return EquipmentObject;
    if (typeStr == "DeviceObject") return DeviceObject;
    if (typeStr == "TemplateObject") return TemplateObject;
    if (typeStr == "InstanceObject") return InstanceObject;
    return UserDefined;
}

QMap<QString, QVariant> SystemPlatformExporter::parseAttributes(const QString &attributesXml)
{
    QMap<QString, QVariant> attributes;
    
    QXmlStreamReader xml(attributesXml);
    
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        
        if (token == QXmlStreamReader::StartElement && xml.name().toString() == "Attribute") {
            QXmlStreamAttributes attrs = xml.attributes();
            QString name = attrs.value("Name").toString();
            QString value = xml.readElementText();
            attributes[name] = value;
        }
    }
    
    return attributes;
}

QString SystemPlatformExporter::generateAttributesXml(const QMap<QString, QVariant> &attributes)
{
    QString result;
    QTextStream out(&result);
    
    out << "<Attributes>\n";
    for (auto it = attributes.begin(); it != attributes.end(); ++it) {
        out << "  <Attribute Name=\"" << it.key() << "\">";
        out << it.value().toString();
        out << "</Attribute>\n";
    }
    out << "</Attributes>\n";
    
    return result;
}

void SystemPlatformExporter::parseHierarchy(const QString &xml, QList<AssetData> &assets)
{
    // 解析階層結構的實作
    // 這裡可以根據需要實作更複雜的階層解析邏輯
}
