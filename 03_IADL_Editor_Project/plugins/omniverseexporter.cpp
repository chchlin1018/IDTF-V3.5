#include "omniverseexporter.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>

OmniverseExporter::OmniverseExporter(QObject *parent)
    : QObject(parent)
{
}

OmniverseExporter::~OmniverseExporter()
{
}

ImportExportInterface::SystemType OmniverseExporter::systemType() const
{
    return SystemType::OMNIVERSE;
}

QString OmniverseExporter::systemName() const
{
    return "NVIDIA Omniverse";
}

QString OmniverseExporter::systemDescription() const
{
    return "NVIDIA Omniverse - Universal Scene Description (USD) format";
}

QStringList OmniverseExporter::supportedFileExtensions() const
{
    return QStringList() << "usd" << "usda" << "usdc";
}

bool OmniverseExporter::validateFile(const QString &filePath)
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

QList<AssetData> OmniverseExporter::importAssets(
    const QString &filePath, 
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始導入 USD 文件..."));
    
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists() || !fileInfo.isFile()) {
        emit errorOccurred(tr("文件不存在: %1").arg(filePath));
        return QList<AssetData>();
    }
    
    emit progressUpdated(10, tr("正在解析 USD 文件..."));
    
    QList<AssetData> assets = parseUsdFile(filePath, options);
    
    emit progressUpdated(100, tr("導入完成，共導入 %1 個資產").arg(assets.size()));
    
    return assets;
}

QList<AssetData> OmniverseExporter::parseUsdFile(
    const QString &filePath,
    const QMap<QString, QVariant> &options)
{
    QList<AssetData> assets;
    
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(tr("無法打開文件: %1").arg(filePath));
        return assets;
    }
    
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    
    // 簡化的 USD 解析
    // 實際應用中應使用 USD SDK
    QStringList lines = content.split("\n");
    
    AssetData currentAsset;
    bool inDef = false;
    
    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        
        if (trimmed.startsWith("def Xform") || 
            trimmed.startsWith("def Mesh") ||
            trimmed.startsWith("def \"")) {
            
            if (inDef && !currentAsset.id.isEmpty()) {
                assets.append(currentAsset);
            }
            
            currentAsset = AssetData();
            inDef = true;
            
            // 提取名稱
            int start = trimmed.indexOf("\"");
            int end = trimmed.indexOf("\"", start + 1);
            if (start >= 0 && end > start) {
                currentAsset.name = trimmed.mid(start + 1, end - start - 1);
                currentAsset.id = currentAsset.name;
            }
            
        } else if (inDef && trimmed.startsWith("custom string")) {
            // 提取自定義屬性
            int eqPos = trimmed.indexOf("=");
            if (eqPos > 0) {
                QString propName = trimmed.mid(14, eqPos - 14).trimmed();
                QString propValue = trimmed.mid(eqPos + 1).trimmed();
                propValue.remove("\"");
                currentAsset.properties[propName] = propValue;
            }
        } else if (trimmed == "}") {
            if (inDef && !currentAsset.id.isEmpty()) {
                assets.append(currentAsset);
                currentAsset = AssetData();
            }
            inDef = false;
        }
    }
    
    return assets;
}

bool OmniverseExporter::exportAssets(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    emit progressUpdated(0, tr("開始導出到 USD 文件..."));
    
    if (assets.isEmpty()) {
        emit errorOccurred(tr("沒有資產可導出"));
        return false;
    }
    
    if (!validateFile(filePath)) {
        return false;
    }
    
    emit progressUpdated(10, tr("正在生成 USD 文件..."));
    
    bool success = generateUsdFile(filePath, assets, options);
    
    if (success) {
        emit progressUpdated(100, tr("導出完成，共導出 %1 個資產").arg(assets.size()));
    } else {
        emit errorOccurred(tr("導出失敗"));
    }
    
    return success;
}

bool OmniverseExporter::generateUsdFile(
    const QString &filePath, 
    const QList<AssetData> &assets,
    const QMap<QString, QVariant> &options)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        emit errorOccurred(tr("無法創建文件: %1").arg(filePath));
        return false;
    }
    
    bool includeVisualization = options.value("includeVisualization", true).toBool();
    bool includePhysics = options.value("includePhysics", false).toBool();
    
    QTextStream out(&file);
    
    // 寫入 USD 文件頭
    out << "#usda 1.0\n";
    out << "(\n";
    out << "    defaultPrim = \"World\"\n";
    out << "    doc = \"Exported from IADL Editor\"\n";
    out << "    metersPerUnit = 1\n";
    out << "    upAxis = \"Z\"\n";
    out << ")\n\n";
    
    // 創建根節點
    out << "def Xform \"World\" (\n";
    out << "    kind = \"component\"\n";
    out << ")\n";
    out << "{\n";
    
    emit progressUpdated(30, tr("正在寫入資產資訊..."));
    
    // 寫入資產
    for (int i = 0; i < assets.size(); ++i) {
        const AssetData &asset = assets[i];
        
        out << convertAssetToUsdPrim(asset, includeVisualization, includePhysics);
        out << "\n";
        
        if ((i + 1) % 10 == 0) {
            int progress = 30 + ((i + 1) * 60 / assets.size());
            emit progressUpdated(progress, 
                tr("已寫入 %1/%2 個資產...").arg(i + 1).arg(assets.size()));
        }
    }
    
    out << "}\n";
    
    file.close();
    
    emit progressUpdated(90, tr("正在完成..."));
    
    return true;
}

QString OmniverseExporter::convertAssetToUsdPrim(
    const AssetData &asset, 
    bool includeVisualization,
    bool includePhysics)
{
    QString result;
    QTextStream out(&result);
    
    // 清理名稱(USD 不允許某些字符)
    QString safeName = asset.name;
    safeName.replace(" ", "_");
    safeName.replace("-", "_");
    
    out << "    def Xform \"" << safeName << "\" (\n";
    out << "        kind = \"component\"\n";
    out << "    )\n";
    out << "    {\n";
    
    // 添加自定義屬性
    out << "        custom string iadl_id = \"" << asset.id << "\"\n";
    out << "        custom string iadl_type = \"" << asset.type << "\"\n";
    out << "        custom string iadl_name = \"" << asset.name << "\"\n";
    
    // 添加其他屬性
    out << generateUsdCustomAttributes(asset.properties);
    
    // 添加視覺化資訊
    if (includeVisualization) {
        out << "\n";
        out << "        def Mesh \"Geometry\"\n";
        out << "        {\n";
        out << "            # 幾何資訊將在這裡定義\n";
        out << "            # 實際應用中應包含頂點、面等資訊\n";
        out << "        }\n";
    }
    
    // 添加物理屬性
    if (includePhysics) {
        out << generateUsdPhysicsProperties(asset);
    }
    
    out << "    }\n";
    
    return result;
}

QString OmniverseExporter::generateUsdMaterial(const QString &assetType)
{
    QString result;
    QTextStream out(&result);
    
    out << "        def Material \"Material\"\n";
    out << "        {\n";
    out << "            token outputs:surface.connect = </World/Materials/DefaultMaterial.outputs:surface>\n";
    out << "        }\n";
    
    return result;
}

QString OmniverseExporter::generateUsdPhysicsProperties(const AssetData &asset)
{
    QString result;
    QTextStream out(&result);
    
    out << "\n";
    out << "        # 物理屬性\n";
    out << "        custom float physics:mass = 1.0\n";
    out << "        custom bool physics:rigidBodyEnabled = true\n";
    
    // 從屬性中提取物理相關資訊
    if (asset.properties.contains("mass")) {
        out << "        custom float physics:mass = " 
            << asset.properties["mass"].toDouble() << "\n";
    }
    
    if (asset.properties.contains("density")) {
        out << "        custom float physics:density = " 
            << asset.properties["density"].toDouble() << "\n";
    }
    
    return result;
}

QString OmniverseExporter::generateUsdCustomAttributes(
    const QMap<QString, QVariant> &properties)
{
    QString result;
    QTextStream out(&result);
    
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        QString key = it.key();
        QVariant value = it.value();
        
        // 跳過已經處理的屬性
        if (key == "mass" || key == "density") {
            continue;
        }
        
        // 根據值類型選擇 USD 類型
        QString usdType;
        QString usdValue;
        
        switch (value.typeId()) {
            case QMetaType::Int:
            case QMetaType::LongLong:
                usdType = "int";
                usdValue = value.toString();
                break;
            case QMetaType::Double:
                usdType = "float";
                usdValue = value.toString();
                break;
            case QMetaType::Bool:
                usdType = "bool";
                usdValue = value.toBool() ? "true" : "false";
                break;
            default:
                usdType = "string";
                usdValue = QString("\"%1\"").arg(value.toString());
                break;
        }
        
        out << "        custom " << usdType << " " << key 
            << " = " << usdValue << "\n";
    }
    
    return result;
}

