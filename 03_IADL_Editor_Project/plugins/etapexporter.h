#ifndef ETAPEXPORTER_H
#define ETAPEXPORTER_H

#include "../importexportinterface.h"
#include <QObject>

class EtapExporter : public QObject, public ImportExportInterface
{
    Q_OBJECT
    
public:
    explicit EtapExporter(QObject *parent = nullptr);
    ~EtapExporter() override;
    
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
    bool generateEtapXmlFile(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    QString convertAssetToEtapComponent(const AssetData &asset);
    QString generateEtapElectricalProperties(const AssetData &asset);
    QString generateEtapProtectionDevice(const AssetData &asset);
    bool isPowerEquipment(const QString &assetType);
    QString getEtapComponentType(const QString &assetType);
    QList<AssetData> parseEtapXmlFile(const QString &filePath, const QMap<QString, QVariant> &options);
};

#endif // ETAPEXPORTER_H
