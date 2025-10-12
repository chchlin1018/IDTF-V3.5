#ifndef EDGEEXPORTER_H
#define EDGEEXPORTER_H

#include "../importexportinterface.h"
#include <QObject>

/**
 * @brief AVEVA EDGE 匯入/匯出插件
 * 
 * 支援 AVEVA EDGE (前身為 InduSoft Web Studio) 的標籤和畫面資料匯入和匯出
 * 檔案格式: XML, CSV (EDGE 專案匯出格式)
 */
class EdgeExporter : public QObject, public ImportExportInterface
{
    Q_OBJECT

public:
    explicit EdgeExporter(QObject *parent = nullptr);
    ~EdgeExporter() override;

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

    // EDGE 標籤類型
    enum TagType {
        Boolean,             // 布林標籤
        Integer,             // 整數標籤
        Real,                // 實數標籤
        String,              // 字串標籤
        DateTime,            // 日期時間標籤
        Quality,             // 品質標籤
        ArrayBoolean,        // 布林陣列標籤
        ArrayInteger,        // 整數陣列標籤
        ArrayReal,           // 實數陣列標籤
        ArrayString          // 字串陣列標籤
    };

    // EDGE 標籤範圍
    enum TagScope {
        Server,              // 伺服器標籤
        Local,               // 本地標籤
        Public,              // 公開標籤
        Class                // 類別標籤
    };

    // 解析 EDGE XML 檔案
    QList<AssetData> parseEdgeXml(const QString &filePath, const QMap<QString, QVariant> &options);
    
    // 解析 EDGE CSV 檔案
    QList<AssetData> parseEdgeCsv(const QString &filePath, const QMap<QString, QVariant> &options);
    
    // 生成 EDGE XML 檔案
    bool generateEdgeXml(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    
    // 生成 EDGE CSV 檔案
    bool generateEdgeCsv(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    
    // 將資產轉換為 EDGE 標籤
    QString convertAssetToEdgeTag(const AssetData &asset);
    
    // 將 EDGE 標籤轉換為資產
    AssetData convertEdgeTagToAsset(const QVariantMap &tag);
    
    // 取得標籤類型字串
    QString getTagTypeString(TagType type) const;
    
    // 從字串取得標籤類型
    TagType getTagTypeFromString(const QString &typeStr) const;
    
    // 取得標籤範圍字串
    QString getTagScopeString(TagScope scope) const;
    
    // 從字串取得標籤範圍
    TagScope getTagScopeFromString(const QString &scopeStr) const;
    
    // 驗證標籤資料
    bool validateTagData(const QVariantMap &tag) const;
};

#endif // EDGEEXPORTER_H
