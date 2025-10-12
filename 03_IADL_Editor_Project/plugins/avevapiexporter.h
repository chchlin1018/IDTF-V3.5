#ifndef AVEVAPIEXPORTER_H
#define AVEVAPIEXPORTER_H

#include "../importexportinterface.h"
#include <QObject>

class AvevaPiExporter : public QObject, public ImportExportInterface
{
    Q_OBJECT

public:
    explicit AvevaPiExporter(QObject *parent = nullptr);
    ~AvevaPiExporter() override;

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

#endif // AVEVAPIEXPORTER_H
