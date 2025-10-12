#ifndef JSONEXPORTER_H
#define JSONEXPORTER_H

#include "../importexportinterface.h"
#include <QObject>

class JsonExporter : public QObject, public ImportExportInterface
{
    Q_OBJECT

public:
    explicit JsonExporter(QObject *parent = nullptr);
    ~JsonExporter() override;

    QList<AssetData> importAssets(const QString &filePath, const QMap<QString, QVariant> &options) override;
    bool exportAssets(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options) override;
    QStringList supportedFileExtensions() const override;
    SystemType systemType() const override;
    QString systemName() const override;
    QString systemDescription() const override;
    bool validateFile(const QString &filePath) override;

signals:
    void errorOccurred(const QString &errorMessage);
};

#endif // JSONEXPORTER_H
