#include "assetitem.h"

AssetItem::AssetItem()
    : QStandardItem()
{
}

AssetItem::AssetItem(const QString &text)
    : QStandardItem(text)
    , m_name(text)
{
}

QString AssetItem::name() const
{
    return m_name;
}

void AssetItem::setName(const QString &name)
{
    m_name = name;
    setText(name);
}

QString AssetItem::assetType() const
{
    return m_type;
}

void AssetItem::setAssetType(const QString &type)
{
    m_type = type;
}



QString AssetItem::description() const
{
    return m_description;
}

void AssetItem::setDescription(const QString &description)
{
    m_description = description;
}

QString AssetItem::content() const
{
    return m_content;
}

void AssetItem::setContent(const QString &content)
{
    m_content = content;
}

QVariant AssetItem::property(const QString &key) const
{
    return m_properties.value(key);
}

void AssetItem::setProperty(const QString &key, const QVariant &value)
{
    m_properties[key] = value;
}

QVariantMap AssetItem::properties() const
{
    return m_properties;
}

void AssetItem::setProperties(const QVariantMap &properties)
{
    m_properties = properties;
}
