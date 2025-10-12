#include "etapexporter.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDebug>

EtapExporter::EtapExporter(QObject *parent)
    : QObject(parent)
{
}

EtapExporter::~EtapExporter()
{
}

ImportExportInterface::SystemType EtapExporter::systemType() const
{
    return SystemType::ETAP;
}

QString EtapExporter::systemName() const
{
    return "ETAP";
}

QString EtapExporter::systemDescription() const
{
    return "ETAP - Electrical Transient Analyzer Program";
}

QStringList EtapExporter::supportedFileExtensions() const
{
    return QStringList() << "etap" << "xml";
}

bool EtapExporter::validateFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    
    // 對於導出，文件可以不存在
    if (!fileInfo.exists()) {
        return true;
    }
    
    QString ext = fileInfo.suffix().toLower();
    if (!supportedFileExtensions().contains(ext)) {
        emit errorOccurred(tr("不支持的文件格式: %1").arg(ext));
        return false;
    }
    
    return true;
}

QList<AssetData> EtapExporter::importAssets(
    const QString &filePath, 
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始導入 ETAP 文件..."));
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        emit errorOccurred(tr("文件不存在: %1").arg(filePath));
        return QList<AssetData>();
    }
    
    emit progressUpdated(10, tr("正在解析 ETAP 文件..."));
    
    QList<AssetData> assets = parseEtapXmlFile(filePath, options);
    
    emit progressUpdated(100, tr("導入完成，共導入 %1 個資產").arg(assets.size()));
    
    return assets;
}

QList<AssetData> EtapExporter::parseEtapXmlFile(
    const QString &filePath,
    const QMap<QString, QVariant> &options)
{
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(tr("無法打開文件: %1").arg(filePath));
        return assets;
    }
    
    bool includePowerFlow = options.value("includePowerFlow", true).toBool();
    bool includeShortCircuit = options.value("includeShortCircuit", false).toBool();
    bool includeProtection = options.value("includeProtection", false).toBool();
    
    QXmlStreamReader xml(&file);
    
    AssetData currentAsset;
    QString currentElement;
    
    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();
        
        if (token == QXmlStreamReader::StartElement) {
            currentElement = xml.name().toString();
            
            if (currentElement == "Component") {
                currentAsset = AssetData();
                
                // 讀取屬性
                QXmlStreamAttributes attributes = xml.attributes();
                if (attributes.hasAttribute("ID")) {
                    currentAsset.id = attributes.value("ID").toString();
                }
                if (attributes.hasAttribute("Name")) {
                    currentAsset.name = attributes.value("Name").toString();
                }
                if (attributes.hasAttribute("Type")) {
                    currentAsset.type = attributes.value("Type").toString();
                }
                
            } else if (currentElement == "Property" && !currentAsset.id.isEmpty()) {
                QXmlStreamAttributes attributes = xml.attributes();
                QString propName = attributes.value("Name").toString();
                QString propValue = xml.readElementText();
                currentAsset.properties[propName] = propValue;
            }
            
        } else if (token == QXmlStreamReader::EndElement) {
            if (xml.name().toString() == "Component" && !currentAsset.id.isEmpty()) {
                assets.append(currentAsset);
                currentAsset = AssetData();
            }
        }
    }
    
    if (xml.hasError()) {
        emit errorOccurred(tr("XML 解析錯誤: %1").arg(xml.errorString()));
    }
    
    file.close();
    
    return assets;
}

bool EtapExporter::exportAssets(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始導出到 ETAP 文件..."));
    
    if (assets.isEmpty()) {
        emit errorOccurred(tr("沒有資產可導出"));
        return false;
    }
    
    if (!validateFile(filePath)) {
        return false;
    }
    
    emit progressUpdated(10, tr("正在生成 ETAP 文件..."));
    
    bool success = generateEtapXmlFile(filePath, assets, options);
    
    if (success) {
        emit progressUpdated(100, tr("導出完成，共導出 %1 個資產").arg(assets.size()));
    } else {
        emit errorOccurred(tr("導出失敗"));
    }
    
    return success;
}

bool EtapExporter::generateEtapXmlFile(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred(tr("無法創建文件: %1").arg(filePath));
        return false;
    }
    
    bool includePowerFlow = options.value("includePowerFlow", true).toBool();
    bool includeShortCircuit = options.value("includeShortCircuit", false).toBool();
    bool includeProtection = options.value("includeProtection", false).toBool();
    
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    
    // 寫入 XML 聲明
    xml.writeStartDocument();
    
    // 寫入根元素
    xml.writeStartElement("ETAPProject");
    xml.writeAttribute("Version", "21.0");
    xml.writeAttribute("ExportedFrom", "IADL Editor");
    xml.writeAttribute("ExportDate", QDateTime::currentDateTime().toString(Qt::ISODate));
    
    // 寫入專案資訊
    xml.writeStartElement("ProjectInfo");
    xml.writeTextElement("Name", "IADL Export");
    xml.writeTextElement("Description", "Exported from IADL Editor");
    xml.writeEndElement(); // ProjectInfo
    
    emit progressUpdated(30, tr("正在寫入資產資訊..."));
    
    // 寫入元件
    xml.writeStartElement("Components");
    
    int powerEquipmentCount = 0;
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        // 只導出電力設備
        if (isPowerEquipment(asset.type)) {
            xml.writeStartElement("Component");
            xml.writeAttribute("ID", asset.id);
            xml.writeAttribute("Name", asset.name);
            xml.writeAttribute("Type", getEtapComponentType(asset.type));
            
            // 寫入屬性
            xml.writeStartElement("Properties");
            
            // 寫入基本屬性
            for (auto it = asset.properties.begin(); it != asset.properties.end(); ++it) {
                xml.writeStartElement("Property");
                xml.writeAttribute("Name", it.key());
                xml.writeAttribute("Type", it.value().typeName());
                xml.writeCharacters(it.value().toString());
                xml.writeEndElement(); // Property
            }
            
            xml.writeEndElement(); // Properties
            
            // 寫入電氣屬性
            if (includePowerFlow) {
                xml.writeStartElement("ElectricalProperties");
                xml.writeTextElement("Voltage", 
                    asset.properties.value("voltage", "0").toString());
                xml.writeTextElement("Current", 
                    asset.properties.value("current", "0").toString());
                xml.writeTextElement("Power", 
                    asset.properties.value("power", "0").toString());
                xml.writeEndElement(); // ElectricalProperties
            }
            
            // 寫入保護設備配置
            if (includeProtection && asset.type.contains("Breaker", Qt::CaseInsensitive)) {
                xml.writeStartElement("ProtectionDevice");
                xml.writeTextElement("Type", "CircuitBreaker");
                xml.writeTextElement("RatedCurrent", 
                    asset.properties.value("rated_current", "0").toString());
                xml.writeTextElement("TripCurve", "Standard");
                xml.writeEndElement(); // ProtectionDevice
            }
            
            xml.writeEndElement(); // Component
            
            powerEquipmentCount++;
        }
        
        if ((i + 1) % 10 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, 
                tr("已處理 %1/%2 個資產...").arg(i + 1).arg(assets.size()));
        }
    }
    
    xml.writeEndElement(); // Components
    
    // 寫入連接資訊
    xml.writeStartElement("Connections");
    // 這裡可以添加設備之間的連接資訊
    xml.writeEndElement(); // Connections
    
    // 寫入分析配置
    if (includePowerFlow) {
        xml.writeStartElement("PowerFlowStudy");
        xml.writeTextElement("Method", "NewtonRaphson");
        xml.writeTextElement("Tolerance", "0.001");
        xml.writeEndElement(); // PowerFlowStudy
    }
    
    if (includeShortCircuit) {
        xml.writeStartElement("ShortCircuitStudy");
        xml.writeTextElement("Method", "IEC60909");
        xml.writeTextElement("FaultType", "3Phase");
        xml.writeEndElement(); // ShortCircuitStudy
    }
    
    xml.writeEndElement(); // ETAPProject
    xml.writeEndDocument();
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    emit progressUpdated(100, tr("成功導出 %1 個電力設備").arg(powerEquipmentCount));
    
    return true;
}

QString EtapExporter::convertAssetToEtapComponent(const AssetData &asset)
{
    QString result;
    QTextStream out(&result);
    
    out << "<Component>\n";
    out << "  <ID>" << asset.id << "</ID>\n";
    out << "  <Name>" << asset.name << "</Name>\n";
    out << "  <Type>" << getEtapComponentType(asset.type) << "</Type>\n";
    
    out << generateEtapElectricalProperties(asset);
    
    out << "</Component>\n";
    
    return result;
}

QString EtapExporter::generateEtapElectricalProperties(const AssetData &asset)
{
    QString result;
    QTextStream out(&result);
    
    out << "  <ElectricalProperties>\n";
    
    if (asset.properties.contains("voltage")) {
        out << "    <Voltage>" << asset.properties["voltage"].toString() << "</Voltage>\n";
    }
    
    if (asset.properties.contains("current")) {
        out << "    <Current>" << asset.properties["current"].toString() << "</Current>\n";
    }
    
    if (asset.properties.contains("power")) {
        out << "    <Power>" << asset.properties["power"].toString() << "</Power>\n";
    }
    
    if (asset.properties.contains("frequency")) {
        out << "    <Frequency>" << asset.properties["frequency"].toString() << "</Frequency>\n";
    }
    
    out << "  </ElectricalProperties>\n";
    
    return result;
}

QString EtapExporter::generateEtapProtectionDevice(const AssetData &asset)
{
    QString result;
    QTextStream out(&result);
    
    out << "  <ProtectionDevice>\n";
    out << "    <Type>CircuitBreaker</Type>\n";
    
    if (asset.properties.contains("rated_current")) {
        out << "    <RatedCurrent>" << asset.properties["rated_current"].toString() 
            << "</RatedCurrent>\n";
    }
    
    out << "    <TripCurve>Standard</TripCurve>\n";
    out << "  </ProtectionDevice>\n";
    
    return result;
}

bool EtapExporter::isPowerEquipment(const QString &assetType)
{
    QStringList powerTypes = {
        "Generator", "Motor", "Transformer", "Breaker", "Switch",
        "Bus", "Cable", "Load", "Capacitor", "Reactor",
        "UPS", "Battery", "Inverter", "Rectifier", "PDU",
        "Switchgear", "Panel"
    };
    
    for (const QString &type : powerTypes) {
        if (assetType.contains(type, Qt::CaseInsensitive)) {
            return true;
        }
    }
    
    return false;
}

QString EtapExporter::getEtapComponentType(const QString &assetType)
{
    QString type = assetType.toLower();
    
    if (type.contains("generator")) return "Generator";
    if (type.contains("motor")) return "Motor";
    if (type.contains("transformer")) return "Transformer";
    if (type.contains("breaker")) return "CircuitBreaker";
    if (type.contains("switch")) return "Switch";
    if (type.contains("bus")) return "Bus";
    if (type.contains("cable")) return "Cable";
    if (type.contains("load")) return "Load";
    if (type.contains("capacitor")) return "Capacitor";
    if (type.contains("reactor")) return "Reactor";
    if (type.contains("ups")) return "UPS";
    if (type.contains("battery")) return "Battery";
    if (type.contains("inverter")) return "Inverter";
    if (type.contains("rectifier")) return "Rectifier";
    if (type.contains("pdu")) return "PDU";
    if (type.contains("switchgear")) return "Switchgear";
    if (type.contains("panel")) return "Panel";
    
    return "Generic";
}

