#ifndef IGNITIONEXPORTER_H
#define IGNITIONEXPORTER_H

#include "../importexportinterface.h"
#include <QObject>

/**
 * @brief Ignition 匯入/匯出插件
 * 
 * 支援 Ignition SCADA/MES/IIoT 平台的標籤和資源資料匯入和匯出
 * 檔案格式: JSON, XML, CSV (Ignition 匯出格式)
 */
class IgnitionExporter : public QObject, public ImportExportInterface
{
    Q_OBJECT

public:
    explicit IgnitionExporter(QObject *parent = nullptr);
    ~IgnitionExporter() override;

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

    // Ignition 標籤類型
    enum TagType {
        OPC,                 // OPC 標籤
        Memory,              // 記憶體標籤
        Expression,          // 表達式標籤
        Query,               // 查詢標籤
        Derived,             // 衍生標籤
        UDT,                 // 使用者定義類型
        Folder               // 資料夾
    };

    // Ignition 資料類型
    enum DataType {
        Int1,                // 布林 (1-bit integer)
        Int2,                // 16-bit 整數
        Int4,                // 32-bit 整數
        Int8,                // 64-bit 整數
        Float4,              // 32-bit 浮點數
        Float8,              // 64-bit 浮點數
        String,              // 字串
        DateTime,            // 日期時間
        DataSet,             // 資料集
        Document             // 文件
    };

    // Ignition 標籤品質
    enum TagQuality {
        Good,                // 良好
        Bad,                 // 不良
        BadNotConnected,     // 未連接
        BadDeviceFailure,    // 裝置故障
        BadSensorFailure,    // 感測器故障
        BadLastKnownValue,   // 最後已知值
        BadCommFailure,      // 通訊故障
        BadOutOfService,     // 停止服務
        Uncertain,           // 不確定
        UncertainLastUsableValue, // 最後可用值
        UncertainSensorNotAccurate, // 感測器不準確
        UncertainEUExceeded, // 超出工程單位範圍
        UncertainSubNormal   // 次正常
    };

    // Ignition 標籤權限
    enum TagSecurity {
        ReadOnly,            // 唯讀
        ReadWrite,           // 讀寫
        Custom               // 自訂
    };

    // 解析 Ignition JSON 檔案
    QList<AssetData> parseIgnitionJson(const QString &filePath, const QMap<QString, QVariant> &options);
    
    // 解析 Ignition XML 檔案
    QList<AssetData> parseIgnitionXml(const QString &filePath, const QMap<QString, QVariant> &options);
    
    // 解析 Ignition CSV 檔案
    QList<AssetData> parseIgnitionCsv(const QString &filePath, const QMap<QString, QVariant> &options);
    
    // 生成 Ignition JSON 檔案
    bool generateIgnitionJson(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    
    // 生成 Ignition XML 檔案
    bool generateIgnitionXml(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    
    // 生成 Ignition CSV 檔案
    bool generateIgnitionCsv(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    
    // 將資產轉換為 Ignition 標籤
    QVariantMap convertAssetToIgnitionTag(const AssetData &asset);
    
    // 將 Ignition 標籤轉換為資產
    AssetData convertIgnitionTagToAsset(const QVariantMap &tag);
    
    // 取得標籤類型字串
    QString getTagTypeString(TagType type) const;
    
    // 從字串取得標籤類型
    TagType getTagTypeFromString(const QString &typeStr) const;
    
    // 取得資料類型字串
    QString getDataTypeString(DataType type) const;
    
    // 從字串取得資料類型
    DataType getDataTypeFromString(const QString &typeStr) const;
    
    // 取得標籤品質字串
    QString getTagQualityString(TagQuality quality) const;
    
    // 從字串取得標籤品質
    TagQuality getTagQualityFromString(const QString &qualityStr) const;
    
    // 取得標籤權限字串
    QString getTagSecurityString(TagSecurity security) const;
    
    // 從字串取得標籤權限
    TagSecurity getTagSecurityFromString(const QString &securityStr) const;
    
    // 驗證標籤資料
    bool validateTagData(const QVariantMap &tag) const;
    
    // 解析 JSON 標籤物件
    void parseJsonTagObject(const QJsonObject &obj, QList<AssetData> &assets);
    
    // 生成 JSON 標籤物件
    QJsonObject generateJsonTagObject(const AssetData &asset);
};

#endif // IGNITIONEXPORTER_H
