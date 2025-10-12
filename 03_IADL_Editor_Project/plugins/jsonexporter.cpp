#include "jsonexporter.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

JsonExporter::JsonExporter(QObject *parent)
    : QObject(parent)
{
}

JsonExporter::~JsonExporter()
{
}

QList<AssetData> JsonExporter::importAssets(const QString &filePath, const QMap<QString, QVariant> &options)
{
    Q_UNUSED(options);
    QList<AssetData> assets;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = "Cannot open file for reading: " + file.errorString();
        emit errorOccurred(m_lastError);
        return assets;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) {
        m_lastError = "JSON file is not an array.";
        emit errorOccurred(m_lastError);
        return assets;
    }

    QJsonArray assetArray = doc.array();
    for (const QJsonValue &value : assetArray) {
        QJsonObject obj = value.toObject();
        AssetData asset;
        asset.id = obj["id"].toString();
        asset.name = obj["name"].toString();
        asset.type = obj["type"].toString();
        asset.properties = obj["properties"].toObject().toVariantMap();
        assets.append(asset);
    }

    return assets;
}

bool JsonExporter::exportAssets(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options)
{
    Q_UNUSED(options);
    QJsonArray assetArray;
    for (const AssetData &asset : assets) {
        QJsonObject obj;
        obj["id"] = asset.id;
        obj["name"] = asset.name;
        obj["type"] = asset.type;
        obj["properties"] = QJsonObject::fromVariantMap(asset.properties);
        assetArray.append(obj);
    }

    QJsonDocument doc(assetArray);
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        m_lastError = "Cannot open file for writing: " + file.errorString();
        emit errorOccurred(m_lastError);
        return false;
    }

    file.write(doc.toJson(QJsonDocument::Indented));
    return true;
}

QStringList JsonExporter::supportedFileExtensions() const
{
    return QStringList() << "json";
}

ImportExportInterface::SystemType JsonExporter::systemType() const
{
    return SystemType::JSON;
}

QString JsonExporter::systemName() const
{
    return "JSON";
}

QString JsonExporter::systemDescription() const
{
    return "Export data to or import data from a JSON file.";
}

bool JsonExporter::validateFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        m_lastError = "File does not exist.";
        return false;
    }
    return true;
}
