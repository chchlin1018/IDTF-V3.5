#ifndef HISTORIANEXPORTER_H
#define HISTORIANEXPORTER_H

#include "../importexportinterface.h"
#include <QObject>

/**
 * @brief AVEVA Historian 匯入/匯出插件
 * 
 * 支援 AVEVA Historian (前身為 Wonderware Historian) 的標籤和歷史資料匯入和匯出
 * 檔案格式: CSV, XML (Historian 匯出格式)
 */
class HistorianExporter : public QObject, public ImportExportInterface
{
    Q_OBJECT

public:
    explicit HistorianExporter(QObject *parent = nullptr);
    ~HistorianExporter() override;

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

    // Historian 標籤類型
    enum TagType {
        Analog,              // 類比標籤
        Discrete,            // 離散標籤
        String,              // 字串標籤
        AnalogSummary,       // 類比摘要標籤
        DiscreteSummary,     // 離散摘要標籤
        Counter,             // 計數器標籤
        Accumulator          // 累加器標籤
    };

    // Historian 儲存類型
    enum StorageType {
        Cyclic,              // 循環儲存
        DeltaCompression,    // 差值壓縮
        SwingingDoor,        // 擺動門壓縮
        BoxcarAndFill,       // 箱型車填充
        NoCompression        // 無壓縮
    };

    // Historian 資料品質
    enum DataQuality {
        Good,                // 良好
        Bad,                 // 不良
        Uncertain,           // 不確定
        NotConnected,        // 未連接
        DeviceFailure,       // 裝置故障
        SensorFailure,       // 感測器故障
        CommFailure,         // 通訊故障
        OutOfService         // 停止服務
    };

    // 解析 Historian CSV 檔案
    QList<AssetData> parseHistorianCsv(const QString &filePath, const QMap<QString, QVariant> &options);
    
    // 解析 Historian XML 檔案
    QList<AssetData> parseHistorianXml(const QString &filePath, const QMap<QString, QVariant> &options);
    
    // 生成 Historian CSV 檔案
    bool generateHistorianCsv(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    
    // 生成 Historian XML 檔案
    bool generateHistorianXml(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    
    // 將資產轉換為 Historian 標籤
    QString convertAssetToHistorianTag(const AssetData &asset);
    
    // 將 Historian 標籤轉換為資產
    AssetData convertHistorianTagToAsset(const QVariantMap &tag);
    
    // 取得標籤類型字串
    QString getTagTypeString(TagType type) const;
    
    // 從字串取得標籤類型
    TagType getTagTypeFromString(const QString &typeStr) const;
    
    // 取得儲存類型字串
    QString getStorageTypeString(StorageType type) const;
    
    // 從字串取得儲存類型
    StorageType getStorageTypeFromString(const QString &typeStr) const;
    
    // 取得資料品質字串
    QString getDataQualityString(DataQuality quality) const;
    
    // 從字串取得資料品質
    DataQuality getDataQualityFromString(const QString &qualityStr) const;
    
    // 驗證標籤資料
    bool validateTagData(const QVariantMap &tag) const;
};

#endif // HISTORIANEXPORTER_H
