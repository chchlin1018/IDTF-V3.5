#include "bimimporter.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
#include <QRegularExpression>

BimImporter::BimImporter(QObject *parent)
    : QObject(parent)
{
}

BimImporter::~BimImporter()
{
}

ImportExportInterface::SystemType BimImporter::systemType() const
{
    return SystemType::BIM;
}

QString BimImporter::systemName() const
{
    return "BIM (IFC)";
}

QString BimImporter::systemDescription() const
{
    return "Building Information Modeling - Industry Foundation Classes format";
}

QStringList BimImporter::supportedFileExtensions() const
{
    return QStringList() << "ifc" << "ifcxml" << "ifczip";
}

bool BimImporter::validateFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        emit errorOccurred(tr("文件不存在: %1").arg(filePath));
        return false;
    }
    
    QString ext = fileInfo.suffix().toLower();
    if (!supportedFileExtensions().contains(ext)) {
        emit errorOccurred(tr("不支持的文件格式: %1").arg(ext));
        return false;
    }
    
    // 檢查文件頭
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(tr("無法打開文件: %1").arg(filePath));
        return false;
    }
    
    QTextStream in(&file);
    QString firstLine = in.readLine();
    file.close();
    
    // IFC 文件應該以 ISO-10303-21 開頭
    if (!firstLine.contains("ISO-10303-21")) {
        emit errorOccurred(tr("無效的 IFC 文件格式"));
        return false;
    }
    
    return true;
}

QList<AssetData> BimImporter::importAssets(
    const QString &filePath, 
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始導入 IFC 文件..."));
    
    if (!validateFile(filePath)) {
        return QList<AssetData>();
    }
    
    emit progressUpdated(10, tr("正在解析 IFC 文件..."));
    
    QList<AssetData> assets = parseIfcFile(filePath, options);
    
    emit progressUpdated(100, tr("導入完成，共導入 %1 個資產").arg(assets.size()));
    
    return assets;
}

QList<AssetData> BimImporter::parseIfcFile(
    const QString &filePath, 
    const QMap<QString, QVariant> &options)
{
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(tr("無法打開文件: %1").arg(filePath));
        return assets;
    }
    
    bool includeGeometry = options.value("includeGeometry", true).toBool();
    bool includeProperties = options.value("includeProperties", true).toBool();
    bool includeRelationships = options.value("includeRelationships", true).toBool();
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    emit progressUpdated(30, tr("正在提取資產資訊..."));
    
    // 解析 IFC 實體
    // 這是一個簡化的解析器，實際應用中應使用專門的 IFC 解析庫
    QRegularExpression entityRegex(R"(#(\d+)=\s*IFC(\w+)\((.*?)\);)");
    QRegularExpressionMatchIterator it = entityRegex.globalMatch(content);
    
    int entityCount = 0;
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString entityId = match.captured(1);
        QString entityType = match.captured(2);
        QString entityParams = match.captured(3);
        
        // 只處理設備相關的實體
        if (entityType.contains("EQUIPMENT") || 
            entityType.contains("PUMP") ||
            entityType.contains("VALVE") ||
            entityType.contains("MOTOR") ||
            entityType.contains("SENSOR") ||
            entityType.contains("ACTUATOR")) {
            
            AssetData asset;
            asset.id = QString("IFC_%1").arg(entityId);
            asset.name = QString("%1_%2").arg(entityType).arg(entityId);
            asset.type = entityType;
            
            // 提取參數
            QStringList params = entityParams.split(",");
            if (params.size() > 0) {
                QString name = params[0].trimmed();
                name.remove("\'");
                if (!name.isEmpty()) {
                    asset.name = name;
                }
            }
            
            // 添加屬性
            if (includeProperties) {
                asset.properties["ifc_entity_id"] = entityId;
                asset.properties["ifc_entity_type"] = entityType;
                asset.properties["source_file"] = QFileInfo(filePath).fileName();
            }
            
            // 添加幾何資訊(如果需要)
            if (includeGeometry) {
                asset.properties["has_geometry"] = true;
            }
            
            assets.append(asset);
            entityCount++;
            
            if (entityCount % 10 == 0) {
                int progress = 30 + (entityCount * 60 / qMax(1, entityCount + 100));
                emit progressUpdated(progress, 
                    tr("已處理 %1 個實體...").arg(entityCount));
            }
        }
    }
    
    emit progressUpdated(90, tr("正在處理關係資訊..."));
    
    // 處理關係(如果需要)
    if (includeRelationships) {
        QRegularExpression relRegex(R"(#(\d+)=\s*IFCREL(\w+)\((.*?)\);)");
        QRegularExpressionMatchIterator relIt = relRegex.globalMatch(content);
        
        while (relIt.hasNext()) {
            QRegularExpressionMatch match = relIt.next();
            QString relType = match.captured(2);
            // 這裡可以處理關係資訊
        }
    }
    
    return assets;
}

AssetData BimImporter::extractAssetFromIfcEntity(
    const QVariantMap &entity)
{
    AssetData asset;
    
    asset.id = entity.value("id").toString();
    asset.name = entity.value("name").toString();
    asset.type = entity.value("type").toString();
    
    QVariantMap properties = entity.value("properties").toMap();
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        asset.properties[it.key()] = it.value();
    }
    
    return asset;
}

bool BimImporter::exportAssets(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始導出到 IFC 文件..."));
    
    if (assets.isEmpty()) {
        emit errorOccurred(tr("沒有資產可導出"));
        return false;
    }
    
    emit progressUpdated(10, tr("正在生成 IFC 文件..."));
    
    bool success = generateIfcFile(filePath, assets, options);
    
    if (success) {
        emit progressUpdated(100, tr("導出完成，共導出 %1 個資產").arg(assets.size()));
    } else {
        emit errorOccurred(tr("導出失敗"));
    }
    
    return success;
}

bool BimImporter::generateIfcFile(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred(tr("無法創建文件: %1").arg(filePath));
        return false;
    }
    
    QString ifcVersion = options.value("ifcVersion", "IFC4").toString();
    bool includeGeometry = options.value("includeGeometry", true).toBool();
    bool includeProperties = options.value("includeProperties", true).toBool();
    
    QTextStream out(&file);
    
    // 寫入 IFC 文件頭
    out << "ISO-10303-21;\n";
    out << "HEADER;\n";
    out << "FILE_DESCRIPTION((\'ViewDefinition [CoordinationView]\'),\'2;1\');\n";
    out << "FILE_NAME(\'exported_from_iadl.ifc\',\'";
    out << QDateTime::currentDateTime().toString(Qt::ISODate);
    out << "\',(\'\'),(\'\'),\'IADL Editor\',\'IADL Editor\',\'\');\n";
    out << "FILE_SCHEMA((\'" << ifcVersion << "\'));\n";
    out << "ENDSEC;\n\n";
    
    out << "DATA;\n";
    
    emit progressUpdated(30, tr("正在寫入資產資訊..."));
    
    // 寫入基本實體
    int entityId = 100;
    
    // 寫入專案
    out << QString("#%1=IFCPROJECT(\'project_id\',\'$\',\'IADL Export Project\',$,$,$,$,$,$);\n")
           .arg(entityId++);
    
    // 寫入資產
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        // 根據資產類型選擇合適的 IFC 實體類型
        QString ifcType = "IFCEQUIPMENTELEMENT";
        if (asset.type.contains("Pump", Qt::CaseInsensitive)) {
            ifcType = "IFCPUMP";
        } else if (asset.type.contains("Valve", Qt::CaseInsensitive)) {
            ifcType = "IFCVALVE";
        } else if (asset.type.contains("Motor", Qt::CaseInsensitive)) {
            ifcType = "IFCMOTOR";
        }
        
        out << QString("#%1=%2(\'%3\',$,\'%4\',$,$,$,$,$);\n")
               .arg(entityId++)
               .arg(ifcType)
               .arg(asset.id)
               .arg(asset.name);
        
        // 寫入屬性(如果需要)
        if (includeProperties && !asset.properties.isEmpty()) {
            out << QString("#%1=IFCPROPERTYSET(\'%2_props\',$,$,(")
                   .arg(entityId++)
                   .arg(asset.id);
            
            QStringList propRefs;
            for (auto it = asset.properties.begin(); it != asset.properties.end(); ++it) {
                out << QString("#%1").arg(entityId);
                propRefs.append(QString("#%1").arg(entityId));
                entityId++;
            }
            
            out << "));\n";
            
            // 寫入屬性值
            entityId -= propRefs.size();
            for (auto it = asset.properties.begin(); it != asset.properties.end(); ++it) {
                out << QString("#%1=IFCPROPERTYSINGLEVALUE(\'%2\',$,IFCTEXT(\'%3\'),$);\n")
                       .arg(entityId++)
                       .arg(it.key())
                       .arg(it.value().toString());
            }
        }
        
        if ((i + 1) % 10 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, 
                tr("已寫入 %1/%2 個資產...").arg(i + 1).arg(assets.size()));
        }
    }
    
    out << "ENDSEC;\n";
    out << "END-ISO-10303-21;\n";
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

QVariantMap BimImporter::convertAssetToIfcEntity(const AssetData &asset)
{
    QVariantMap entity;
    
    entity["id"] = asset.id;
    entity["name"] = asset.name;
    entity["type"] = asset.type;
    entity["properties"] = asset.properties;
    
    return entity;
}

QString BimImporter::readIfcVersion(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return "IFC4";
    }
    
    QTextStream in(&file);
    QString content = in.read(1000); // 只讀取前 1000 個字符
    file.close();
    
    QRegularExpression versionRegex(R"(FILE_SCHEMA\(\(\'(IFC\w+)\'\)\))");
    QRegularExpressionMatch match = versionRegex.match(content);
    
    if (match.hasMatch()) {
        return match.captured(1);
    }
    
    return "IFC4";
}

