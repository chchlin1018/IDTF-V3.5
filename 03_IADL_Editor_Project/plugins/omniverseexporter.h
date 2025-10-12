#ifndef OMNIVERSEEXPORTER_H
#define OMNIVERSEEXPORTER_H

#include "../importexportinterface.h"
#include <QObject>

class OmniverseExporter : public QObject, public ImportExportInterface
{
    Q_OBJECT
    
public:
    explicit OmniverseExporter(QObject *parent = nullptr);
    ~OmniverseExporter() override;
    
    QList<AssetData> importAssets(const QString &filePath, const QMap<QString, QVariant> &options) override;
    bool exportAssets(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options) override;
    QStringList supportedFileExtensions() const override;
    SystemType systemType() const override;
    QString systemName() const override;
    QString systemDescription() const override;
    bool validateFile(const QString &filePath) override;

signals:
    void progressUpdated(int progress, const QString &message);
    void errorOccurred(const QString &errorMessage);

private:
    bool generateUsdFile(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    QString convertAssetToUsdPrim(const AssetData &asset, bool includeVisualization, bool includePhysics);
    QString generateUsdMaterial(const QString &assetType);
    QString generateUsdPhysicsProperties(const AssetData &asset);
    QString generateUsdCustomAttributes(const QMap<QString, QVariant> &properties);
    QList<AssetData> parseUsdFile(const QString &filePath, const QMap<QString, QVariant> &options);
};

#endif // OMNIVERSEEXPORTER_H
