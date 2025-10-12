#ifndef IMPORTEXPORTMANAGER_H
#define IMPORTEXPORTMANAGER_H

#include <QObject>
#include <QMap>
#include <QList>
#include <memory>
#include "importexportinterface.h"

class ImportExportManager : public QObject
{
    Q_OBJECT

public:
    explicit ImportExportManager(QObject *parent = nullptr);
    ~ImportExportManager();

    QList<AssetData> importAssets(ImportExportInterface::SystemType systemType, const QString &filePath, const QMap<QString, QVariant> &options);
    bool exportAssets(ImportExportInterface::SystemType systemType, const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);

    QMap<ImportExportInterface::SystemType, ImportExportInterface*> getAvailableExporters();
    QMap<ImportExportInterface::SystemType, ImportExportInterface*> getAvailableImporters();

private:
    void loadPlugins();
    QList<ImportExportInterface*> m_plugins;
    QMap<ImportExportInterface::SystemType, ImportExportInterface*> m_exporters;
    QMap<ImportExportInterface::SystemType, ImportExportInterface*> m_importers;
};

#endif // IMPORTEXPORTMANAGER_H
