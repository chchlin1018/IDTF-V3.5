#ifndef ASSETMODEL_H
#define ASSETMODEL_H

#include <QStandardItemModel>
#include "assetitem.h"

class AssetModel : public QStandardItemModel
{
    Q_OBJECT

public:
    explicit AssetModel(QObject *parent = nullptr);
    
    void loadExampleAssets();
    AssetItem* createAssetItem(const QString &name, const QString &type, const QString &description);
};

#endif // ASSETMODEL_H
