#ifndef ASSETITEM_H
#define ASSETITEM_H

#include <QStandardItem>
#include <QVariantMap>

class AssetItem : public QStandardItem
{
public:
    AssetItem();
    explicit AssetItem(const QString &text);
    
    QString name() const;
    void setName(const QString &name);
    
    QString assetType() const;
    void setAssetType(const QString &type);
    
    QString description() const;
    void setDescription(const QString &description);
    
    QString content() const;
    void setContent(const QString &content);
    
    // Property management
    QVariant property(const QString &key) const;
    void setProperty(const QString &key, const QVariant &value);
    QVariantMap properties() const;
    void setProperties(const QVariantMap &properties);
    
private:
    QString m_name;
    QString m_type;
    QString m_description;
    QString m_content;
    QVariantMap m_properties;
};

#endif // ASSETITEM_H
