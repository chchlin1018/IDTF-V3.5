#ifndef SYSTEMPLATFORMEXPORTER_H
#define SYSTEMPLATFORMEXPORTER_H

#include "../importexportinterface.h"
#include <QObject>

/**
 * @brief AVEVA System Platform 匯入/匯出插件
 * 
 * 支援 AVEVA System Platform (前身為 Wonderware System Platform) 的物件模型匯入和匯出
 * 檔案格式: XML (aaPlatform 匯出格式)
 */
class SystemPlatformExporter : public QObject, public ImportExportInterface
{
    Q_OBJECT

public:
    explicit SystemPlatformExporter(QObject *parent = nullptr);
    ~SystemPlatformExporter() override;

    // ImportExportInterface 介面實作
    QList<AssetData> importAssets(const QString &filePath, const QMap<QString, QVariant> &options) override;
    bool exportAssets(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options) override;
    QStringList supportedFileExtensions() const override;
    SystemType systemType() const override;
    QString systemName() const override;
    QString systemDescription() const override;
    bool validateFile(const QString &filePath) override;

signals:
    void errorOccurred(const QString &errorMessage);
    void progressUpdated(int percentage, const QString &message);

private:
    QString m_lastError;

    // System Platform 物件類型
    enum ObjectType {
        UserDefined,         // 使用者定義物件
        ApplicationObject,   // 應用程式物件
        AreaObject,          // 區域物件
        EquipmentObject,     // 設備物件
        DeviceObject,        // 裝置物件
        TemplateObject,      // 範本物件
        InstanceObject       // 實例物件
    };

    // 解析 System Platform XML 檔案
    QList<AssetData> parseSystemPlatformXml(const QString &filePath, const QMap<QString, QVariant> &options);
    
    // 生成 System Platform XML 檔案
    bool generateSystemPlatformXml(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    
    // 將資產轉換為 System Platform 物件
    QString convertAssetToSystemPlatformObject(const AssetData &asset);
    
    // 將 System Platform 物件轉換為資產
    AssetData convertSystemPlatformObjectToAsset(const QVariantMap &object);
    
    // 取得物件類型字串
    QString getObjectTypeString(ObjectType type) const;
    
    // 從字串取得物件類型
    ObjectType getObjectTypeFromString(const QString &typeStr) const;
    
    // 解析屬性
    QMap<QString, QVariant> parseAttributes(const QString &attributesXml);
    
    // 生成屬性 XML
    QString generateAttributesXml(const QMap<QString, QVariant> &attributes);
    
    // 解析階層結構
    void parseHierarchy(const QString &xml, QList<AssetData> &assets);
};

#endif // SYSTEMPLATFORMEXPORTER_H
