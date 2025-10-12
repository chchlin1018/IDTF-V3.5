#include "assetmodel.h"
#include <QFile>
#include <QTextStream>

AssetModel::AssetModel(QObject *parent)
    : QStandardItemModel(parent)
{
    setHorizontalHeaderLabels(QStringList() << "Assets");
}

void AssetModel::loadExampleAssets()
{
    // Create root items for asset hierarchy
    AssetItem *siteItem = createAssetItem("Chemical Plant", "Site", "Main chemical processing facility");
    
    // Create area items
    AssetItem *productionArea = createAssetItem("Production", "Area", "Production area");
    AssetItem *utilitiesArea = createAssetItem("Utilities", "Area", "Utilities area");
    
    siteItem->appendRow(productionArea);
    siteItem->appendRow(utilitiesArea);
    
    // Create process unit items
    AssetItem *reactorUnit = createAssetItem("Reactor Unit", "Process Unit", "Main reactor processing unit");
    AssetItem *separationUnit = createAssetItem("Separation Unit", "Process Unit", "Product separation unit");
    
    productionArea->appendRow(reactorUnit);
    productionArea->appendRow(separationUnit);
    
    // Create equipment items
    AssetItem *reactor = createAssetItem("R-101", "Reactor", "Main reactor vessel");
    AssetItem *pump = createAssetItem("P-101", "Pump", "Feed pump");
    AssetItem *valve = createAssetItem("V-101", "Valve", "Flow control valve");
    
    reactorUnit->appendRow(reactor);
    reactorUnit->appendRow(pump);
    reactorUnit->appendRow(valve);
    
    // Create component items
    AssetItem *motor = createAssetItem("M-101", "Motor", "Pump motor");
    AssetItem *sensor = createAssetItem("TT-101", "Temperature Sensor", "Temperature transmitter");
    
    pump->appendRow(motor);
    reactor->appendRow(sensor);
    
    // Add the site to the model
    appendRow(siteItem);
}

AssetItem* AssetModel::createAssetItem(const QString &name, const QString &type, const QString &description)
{
    AssetItem *item = new AssetItem();
    item->setName(name);
    item->setAssetType(type);
    item->setDescription(description);
    
    // Load example content based on type
    if (type == "Pump") {
        QFile file(":/examples/pump.yaml");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            item->setContent(in.readAll());
            file.close();
        }
    } else if (type == "Valve") {
        QFile file(":/examples/valve.yaml");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            item->setContent(in.readAll());
            file.close();
        }
    } else if (type == "Motor") {
        QFile file(":/examples/motor.yaml");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            item->setContent(in.readAll());
            file.close();
        }
    } else {
        // Default content template
        QString content = "name: " + name + "\n";
        content += "type: " + type + "\n";
        content += "description: " + description + "\n";
        content += "properties:\n";
        content += "  id: " + name.toLower().replace(" ", "_") + "\n";
        item->setContent(content);
    }
    
    return item;
}
