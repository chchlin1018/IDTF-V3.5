#ifndef BIMIMPORTER_H
#define BIMIMPORTER_H

#include "../importexportinterface.h"
#include <QObject>

class BimImporter : public QObject, public ImportExportInterface
{
    Q_OBJECT
    
public:
    explicit BimImporter(QObject *parent = nullptr);
    ~BimImporter() override;
    
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
    QList<AssetData> parseIfcFile(const QString &filePath, const QMap<QString, QVariant> &options);
    AssetData extractAssetFromIfcEntity(const QVariantMap &entity);
    bool generateIfcFile(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    QVariantMap convertAssetToIfcEntity(const AssetData &asset);
    QString readIfcVersion(const QString &filePath);
};

#endif // BIMIMPORTER_H
