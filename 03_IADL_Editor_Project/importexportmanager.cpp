#include "importexportmanager.h"
#include "plugins/avevapiexporter.h"
#include "plugins/jsonexporter.h"
#include "plugins/bimimporter.h"
#include "plugins/omniverseexporter.h"
#include "plugins/etapexporter.h"
#include "plugins/intouchexporter.h"
#include "plugins/systemplatformexporter.h"
#include "plugins/edgeexporter.h"
#include "plugins/historianexporter.h"
#include "plugins/ignitionexporter.h"

ImportExportManager::ImportExportManager(QObject *parent)
    : QObject(parent)
{
    loadPlugins();
}

ImportExportManager::~ImportExportManager()
{
    // The plugins are QObjects and will be deleted by their parent if set,
    // but since we are not setting a parent, we should delete them.
    qDeleteAll(m_plugins);
}

void ImportExportManager::loadPlugins()
{
    // Manually instantiate plugins and add them to the list
    m_plugins.append(new AvevaPiExporter());
    m_plugins.append(new JsonExporter());
    m_plugins.append(new BimImporter());
    m_plugins.append(new OmniverseExporter());
    m_plugins.append(new EtapExporter());
    m_plugins.append(new InTouchExporter());
    m_plugins.append(new SystemPlatformExporter());
    m_plugins.append(new EdgeExporter());
    m_plugins.append(new HistorianExporter());
    m_plugins.append(new IgnitionExporter());

    // Populate the maps for quick lookup
    for (ImportExportInterface *plugin : m_plugins) {
        // Assuming one plugin can handle both import and export for its system type
        m_exporters[plugin->systemType()] = plugin;
        m_importers[plugin->systemType()] = plugin;
    }
}

QList<AssetData> ImportExportManager::importAssets(ImportExportInterface::SystemType systemType, const QString &filePath, const QMap<QString, QVariant> &options)
{
    if (m_importers.contains(systemType)) {
        return m_importers[systemType]->importAssets(filePath, options);
    }
    // Return an empty list if no suitable importer is found
    return QList<AssetData>();
}

bool ImportExportManager::exportAssets(ImportExportInterface::SystemType systemType, const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options)
{
    if (m_exporters.contains(systemType)) {
        return m_exporters[systemType]->exportAssets(filePath, assets, options);
    }
    // Return false if no suitable exporter is found
    return false;
}

QMap<ImportExportInterface::SystemType, ImportExportInterface*> ImportExportManager::getAvailableExporters()
{
    return m_exporters;
}

QMap<ImportExportInterface::SystemType, ImportExportInterface*> ImportExportManager::getAvailableImporters()
{
    return m_importers;
}

