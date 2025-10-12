#include <QtTest>
#include "../importexportmanager.h"
#include "../assetmodel.h"
#include "../assetitem.h"

class TestImportExport : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testPluginLoading();
    void testJsonExport();
    void testJsonImport();
    void testBimExport();
    void testOmniverseExport();
    void testEtapExport();

private:
    ImportExportManager *m_manager;
    AssetModel *m_assetModel;
};

void TestImportExport::initTestCase()
{
    m_manager = new ImportExportManager();
    m_assetModel = new AssetModel();
    
    // Create some sample assets
    AssetItem *asset1 = new AssetItem("Pump_01");
    asset1->setAssetType("Pump");
    asset1->setProperty("flow_rate", 100.5);
    asset1->setProperty("pressure", 50.2);
    m_assetModel->appendRow(asset1);
    
    AssetItem *asset2 = new AssetItem("Valve_01");
    asset2->setAssetType("Valve");
    asset2->setProperty("size", 6);
    asset2->setProperty("material", "Stainless Steel");
    m_assetModel->appendRow(asset2);
    
    AssetItem *asset3 = new AssetItem("Generator_01");
    asset3->setAssetType("Generator");
    asset3->setProperty("voltage", 4160);
    asset3->setProperty("power", 2000);
    m_assetModel->appendRow(asset3);
}

void TestImportExport::cleanupTestCase()
{
    delete m_manager;
    delete m_assetModel;
}

void TestImportExport::testPluginLoading()
{
    QVERIFY(m_manager != nullptr);
    QVERIFY(m_manager->getAvailableExporters().size() > 0);
    QVERIFY(m_manager->getAvailableImporters().size() > 0);
    
    qDebug() << "Available exporters:" << m_manager->getAvailableExporters().keys();
    qDebug() << "Available importers:" << m_manager->getAvailableImporters().keys();
}

void TestImportExport::testJsonExport()
{
    QString filePath = "test_export.json";
    QMap<QString, QVariant> options;
    
    QList<ImportExportInterface::AssetData> assets;
    for (int i = 0; i < m_assetModel->rowCount(); ++i) {
        AssetItem *item = static_cast<AssetItem*>(m_assetModel->item(i));
        if (item) {
            ImportExportInterface::AssetData assetData;
            assetData.id = QString("asset_%1").arg(i);
            assetData.name = item->text();
            assetData.type = item->assetType();
            assetData.properties = item->properties();
            assets.append(assetData);
        }
    }
    
    bool success = m_manager->exportAssets(
        ImportExportInterface::SystemType::JSON, filePath, assets, options);
    
    QVERIFY(success);
    QVERIFY(QFile::exists(filePath));
    
    // Clean up
    QFile::remove(filePath);
}

void TestImportExport::testJsonImport()
{
    // First, export to a JSON file
    QString filePath = "test_import.json";
    testJsonExport();
    QFile::rename("test_export.json", filePath);
    
    // Now, import from the JSON file
    QMap<QString, QVariant> options;
    QList<ImportExportInterface::AssetData> importedAssets = 
        m_manager->importAssets(ImportExportInterface::SystemType::JSON, filePath, options);
        
    QCOMPARE(importedAssets.size(), m_assetModel->rowCount());
    
    // Check one asset
    QVERIFY(importedAssets[0].name == "Pump_01");
    QVERIFY(importedAssets[0].type == "Pump");
    QVERIFY(importedAssets[0].properties.value("flow_rate").toDouble() > 100.0);
    
    // Clean up
    QFile::remove(filePath);
}

void TestImportExport::testBimExport()
{
    QString filePath = "test_export.ifc";
    QMap<QString, QVariant> options;
    options["ifcVersion"] = "IFC4";
    
    QList<ImportExportInterface::AssetData> assets;
    for (int i = 0; i < m_assetModel->rowCount(); ++i) {
        AssetItem *item = static_cast<AssetItem*>(m_assetModel->item(i));
        if (item) {
            ImportExportInterface::AssetData assetData;
            assetData.id = QString("asset_%1").arg(i);
            assetData.name = item->text();
            assetData.type = item->assetType();
            assetData.properties = item->properties();
            assets.append(assetData);
        }
    }
    
    bool success = m_manager->exportAssets(
        ImportExportInterface::SystemType::BIM, filePath, assets, options);
    
    QVERIFY(success);
    QVERIFY(QFile::exists(filePath));
    
    // Clean up
    QFile::remove(filePath);
}

void TestImportExport::testOmniverseExport()
{
    QString filePath = "test_export.usda";
    QMap<QString, QVariant> options;
    options["includeVisualization"] = true;
    options["includePhysics"] = true;
    
    QList<ImportExportInterface::AssetData> assets;
    for (int i = 0; i < m_assetModel->rowCount(); ++i) {
        AssetItem *item = static_cast<AssetItem*>(m_assetModel->item(i));
        if (item) {
            ImportExportInterface::AssetData assetData;
            assetData.id = QString("asset_%1").arg(i);
            assetData.name = item->text();
            assetData.type = item->assetType();
            assetData.properties = item->properties();
            assets.append(assetData);
        }
    }
    
    bool success = m_manager->exportAssets(
        ImportExportInterface::SystemType::OMNIVERSE, filePath, assets, options);
    
    QVERIFY(success);
    QVERIFY(QFile::exists(filePath));
    
    // Clean up
    QFile::remove(filePath);
}

void TestImportExport::testEtapExport()
{
    QString filePath = "test_export.xml";
    QMap<QString, QVariant> options;
    options["includePowerFlow"] = true;
    
    QList<ImportExportInterface::AssetData> assets;
    for (int i = 0; i < m_assetModel->rowCount(); ++i) {
        AssetItem *item = static_cast<AssetItem*>(m_assetModel->item(i));
        if (item) {
            ImportExportInterface::AssetData assetData;
            assetData.id = QString("asset_%1").arg(i);
            assetData.name = item->text();
            assetData.type = item->assetType();
            assetData.properties = item->properties();
            assets.append(assetData);
        }
    }
    
    bool success = m_manager->exportAssets(
        ImportExportInterface::SystemType::ETAP, filePath, assets, options);
    
    QVERIFY(success);
    QVERIFY(QFile::exists(filePath));
    
    // Clean up
    QFile::remove(filePath);
}

QTEST_MAIN(TestImportExport)
#include "test_import_export.moc"

