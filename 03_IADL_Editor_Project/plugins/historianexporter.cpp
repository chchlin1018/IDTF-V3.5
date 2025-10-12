#include "historianexporter.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDebug>
#include <QDateTime>

HistorianExporter::HistorianExporter(QObject *parent)
    : QObject(parent)
{
}

HistorianExporter::~HistorianExporter()
{
}

ImportExportInterface::SystemType HistorianExporter::systemType() const
{
    return SystemType::HISTORIAN;
}

QString HistorianExporter::systemName() const
{
    return "AVEVA Historian";
}

QString HistorianExporter::systemDescription() const
{
    return "AVEVA Historian - Tag Database and Historical Data Import/Export";
}

QStringList HistorianExporter::supportedFileExtensions() const
{
    return QStringList() << "csv" << "xml" << "txt";
}

bool HistorianExporter::validateFile(const QString &filePath)
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

QList<AssetData> HistorianExporter::importAssets(
    const QString &filePath, 
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始匯入 Historian 標籤資料庫..."));
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        m_lastError = tr("檔案不存在: %1").arg(filePath);
        emit errorOccurred(m_lastError);
        return QList<AssetData>();
    }
    
    emit progressUpdated(10, tr("正在解析 Historian 檔案..."));
    
    QList<AssetData> assets;
    
    QString ext = fileInfo.suffix().toLower();
    if (ext == "xml") {
        assets = parseHistorianXml(filePath, options);
    } else if (ext == "csv" || ext == "txt") {
        assets = parseHistorianCsv(filePath, options);
    }
    
    emit progressUpdated(100, tr("匯入完成，共匯入 %1 個標籤").arg(assets.size()));
    
    return assets;
}

QList<AssetData> HistorianExporter::parseHistorianCsv(
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
            AssetData asset = convertHistorianTagToAsset(tag);
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

QList<AssetData> HistorianExporter::parseHistorianXml(
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
                if (attributes.hasAttribute("StorageType")) {
                    currentTag["storagetype"] = attributes.value("StorageType").toString();
                }
                
            } else if (!currentTag.isEmpty()) {
                // 讀取標籤的其他屬性
                QString value = xml.readElementText();
                currentTag[currentElement.toLower()] = value;
            }
            
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name().toString() == "Tag" && !currentTag.isEmpty()) {
                if (validateTagData(currentTag)) {
                    AssetData asset = convertHistorianTagToAsset(currentTag);
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

AssetData HistorianExporter::convertHistorianTagToAsset(const QVariantMap &tag)
{
    AssetData asset;
    
    asset.id = tag.value("name").toString();
    asset.name = asset.id;
    asset.type = tag.value("type", "Analog").toString();
    
    // 基本屬性
    asset.properties["historian_tag_name"] = asset.id;
    asset.properties["historian_tag_type"] = asset.type;
    asset.properties["historian_storage_type"] = tag.value("storagetype", "SwingingDoor").toString();
    
    // 其他屬性
    if (tag.contains("description")) {
        asset.properties["description"] = tag.value("description");
    }
    if (tag.contains("engunit")) {
        asset.properties["eng_unit"] = tag.value("engunit");
    }
    if (tag.contains("mineu")) {
        asset.properties["min_eu"] = tag.value("mineu");
    }
    if (tag.contains("maxeu")) {
        asset.properties["max_eu"] = tag.value("maxeu");
    }
    if (tag.contains("acquisitionrate")) {
        asset.properties["acquisition_rate"] = tag.value("acquisitionrate");
    }
    if (tag.contains("storagerate")) {
        asset.properties["storage_rate"] = tag.value("storagerate");
    }
    if (tag.contains("compressiondeviation")) {
        asset.properties["compression_deviation"] = tag.value("compressiondeviation");
    }
    if (tag.contains("compressiontimeout")) {
        asset.properties["compression_timeout"] = tag.value("compressiontimeout");
    }
    if (tag.contains("retentiondays")) {
        asset.properties["retention_days"] = tag.value("retentiondays");
    }
    
    return asset;
}

bool HistorianExporter::exportAssets(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始匯出到 Historian 標籤資料庫..."));
    
    if (assets.isEmpty()) {
        m_lastError = tr("沒有資產可匯出");
        emit errorOccurred(m_lastError);
        return false;
    }
    
    if (!validateFile(filePath)) {
        return false;
    }
    
    emit progressUpdated(10, tr("正在生成 Historian 檔案..."));
    
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();
    
    bool success = false;
    if (ext == "xml") {
        success = generateHistorianXml(filePath, assets, options);
    } else if (ext == "csv" || ext == "txt") {
        success = generateHistorianCsv(filePath, assets, options);
    }
    
    if (success) {
        emit progressUpdated(100, tr("匯出完成，共匯出 %1 個標籤").arg(assets.size()));
    } else {
        emit errorOccurred(tr("匯出失敗"));
    }
    
    return success;
}

bool HistorianExporter::generateHistorianCsv(
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
    out << "Name,Type,StorageType,Description,EngUnit,MinEU,MaxEU,AcquisitionRate,StorageRate,CompressionDeviation,CompressionTimeout,RetentionDays\n";
    
    emit progressUpdated(30, tr("正在寫入標籤資訊..."));
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        out << asset.properties.value("historian_tag_name", asset.id).toString() << ",";
        out << asset.properties.value("historian_tag_type", "Analog").toString() << ",";
        out << asset.properties.value("historian_storage_type", "SwingingDoor").toString() << ",";
        out << "\"" << asset.properties.value("description", asset.name).toString() << "\",";
        out << asset.properties.value("eng_unit", "").toString() << ",";
        out << asset.properties.value("min_eu", "0").toString() << ",";
        out << asset.properties.value("max_eu", "100").toString() << ",";
        out << asset.properties.value("acquisition_rate", "1000").toString() << ",";
        out << asset.properties.value("storage_rate", "5000").toString() << ",";
        out << asset.properties.value("compression_deviation", "0.1").toString() << ",";
        out << asset.properties.value("compression_timeout", "30000").toString() << ",";
        out << asset.properties.value("retention_days", "365").toString() << "\n";
        
        if ((i + 1) % 100 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, tr("已寫入 %1/%2 個標籤...").arg(i + 1).arg(assets.size()));
        }
    }
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

bool HistorianExporter::generateHistorianXml(
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
    xml.writeStartElement("HistorianExport");
    xml.writeAttribute("Version", "1.0");
    xml.writeAttribute("ExportedFrom", "IADL Editor");
    xml.writeAttribute("ExportDate", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    // 寫入標籤資料庫
    xml.writeStartElement("TagDatabase");
    
    emit progressUpdated(30, tr("正在寫入標籤資訊..."));
    
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        xml.writeStartElement("Tag");
        xml.writeAttribute("Name", asset.properties.value("historian_tag_name", asset.id).toString());
        xml.writeAttribute("Type", asset.properties.value("historian_tag_type", "Analog").toString());
        xml.writeAttribute("StorageType", asset.properties.value("historian_storage_type", "SwingingDoor").toString());
        
        // 寫入描述
        if (asset.properties.contains("description")) {
            xml.writeTextElement("Description", asset.properties["description"].toString());
        }
        
        // 寫入工程單位
        if (asset.properties.contains("eng_unit")) {
            xml.writeTextElement("EngUnit", asset.properties["eng_unit"].toString());
        }
        
        // 寫入工程單位範圍
        if (asset.properties.contains("min_eu")) {
            xml.writeTextElement("MinEU", asset.properties["min_eu"].toString());
        }
        if (asset.properties.contains("max_eu")) {
            xml.writeTextElement("MaxEU", asset.properties["max_eu"].toString());
        }
        
        // 寫入採集和儲存速率
        if (asset.properties.contains("acquisition_rate")) {
            xml.writeTextElement("AcquisitionRate", asset.properties["acquisition_rate"].toString());
        }
        if (asset.properties.contains("storage_rate")) {
            xml.writeTextElement("StorageRate", asset.properties["storage_rate"].toString());
        }
        
        // 寫入壓縮設定
        if (asset.properties.contains("compression_deviation")) {
            xml.writeTextElement("CompressionDeviation", asset.properties["compression_deviation"].toString());
        }
        if (asset.properties.contains("compression_timeout")) {
            xml.writeTextElement("CompressionTimeout", asset.properties["compression_timeout"].toString());
        }
        
        // 寫入保留天數
        if (asset.properties.contains("retention_days")) {
            xml.writeTextElement("RetentionDays", asset.properties["retention_days"].toString());
        }
        
        xml.writeEndElement(); // Tag
        
        if ((i + 1) % 10 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, tr("已寫入 %1/%2 個標籤...").arg(i + 1).arg(assets.size()));
        }
    }
    
    xml.writeEndElement(); // TagDatabase
    xml.writeEndElement(); // HistorianExport
    xml.writeEndDocument();
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

QString HistorianExporter::convertAssetToHistorianTag(const AssetData &asset)
{
    QString result;
    QTextStream out(&result);
    
    out << "<Tag Name=\"" << asset.id << "\" ";
    out << "Type=\"" << asset.properties.value("historian_tag_type", "Analog").toString() << "\" ";
    out << "StorageType=\"" << asset.properties.value("historian_storage_type", "SwingingDoor").toString() << "\">\n";
    
    if (asset.properties.contains("description")) {
        out << "  <Description>" << asset.properties["description"].toString() << "</Description>\n";
    }
    
    out << "</Tag>\n";
    
    return result;
}

QString HistorianExporter::getTagTypeString(TagType type) const
{
    switch (type) {
        case Analog: return "Analog";
        case Discrete: return "Discrete";
        case String: return "String";
        case AnalogSummary: return "AnalogSummary";
        case DiscreteSummary: return "DiscreteSummary";
        case Counter: return "Counter";
        case Accumulator: return "Accumulator";
        default: return "Analog";
    }
}

HistorianExporter::TagType HistorianExporter::getTagTypeFromString(const QString &typeStr) const
{
    if (typeStr == "Analog") return Analog;
    if (typeStr == "Discrete") return Discrete;
    if (typeStr == "String") return String;
    if (typeStr == "AnalogSummary") return AnalogSummary;
    if (typeStr == "DiscreteSummary") return DiscreteSummary;
    if (typeStr == "Counter") return Counter;
    if (typeStr == "Accumulator") return Accumulator;
    return Analog;
}

QString HistorianExporter::getStorageTypeString(StorageType type) const
{
    switch (type) {
        case Cyclic: return "Cyclic";
        case DeltaCompression: return "DeltaCompression";
        case SwingingDoor: return "SwingingDoor";
        case BoxcarAndFill: return "BoxcarAndFill";
        case NoCompression: return "NoCompression";
        default: return "SwingingDoor";
    }
}

HistorianExporter::StorageType HistorianExporter::getStorageTypeFromString(const QString &typeStr) const
{
    if (typeStr == "Cyclic") return Cyclic;
    if (typeStr == "DeltaCompression") return DeltaCompression;
    if (typeStr == "SwingingDoor") return SwingingDoor;
    if (typeStr == "BoxcarAndFill") return BoxcarAndFill;
    if (typeStr == "NoCompression") return NoCompression;
    return SwingingDoor;
}

QString HistorianExporter::getDataQualityString(DataQuality quality) const
{
    switch (quality) {
        case Good: return "Good";
        case Bad: return "Bad";
        case Uncertain: return "Uncertain";
        case NotConnected: return "NotConnected";
        case DeviceFailure: return "DeviceFailure";
        case SensorFailure: return "SensorFailure";
        case CommFailure: return "CommFailure";
        case OutOfService: return "OutOfService";
        default: return "Good";
    }
}

HistorianExporter::DataQuality HistorianExporter::getDataQualityFromString(const QString &qualityStr) const
{
    if (qualityStr == "Good") return Good;
    if (qualityStr == "Bad") return Bad;
    if (qualityStr == "Uncertain") return Uncertain;
    if (qualityStr == "NotConnected") return NotConnected;
    if (qualityStr == "DeviceFailure") return DeviceFailure;
    if (qualityStr == "SensorFailure") return SensorFailure;
    if (qualityStr == "CommFailure") return CommFailure;
    if (qualityStr == "OutOfService") return OutOfService;
    return Good;
}

bool HistorianExporter::validateTagData(const QVariantMap &tag) const
{
    if (!tag.contains("name") || tag.value("name").toString().isEmpty()) {
        return false;
    }
    
    if (!tag.contains("type") || tag.value("type").toString().isEmpty()) {
        return false;
    }
    
    return true;
}
