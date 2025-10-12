#ifndef INTOUCHEXPORTER_H
#define INTOUCHEXPORTER_H

#include "../importexportinterface.h"
#include <QObject>

/**
 * @brief AVEVA InTouch 匯入/匯出插件
 * 
 * 支援 AVEVA InTouch SCADA 系統的標籤資料庫匯入和匯出
 * 檔案格式: CSV (標籤資料庫匯出格式)
 */
class InTouchExporter : public QObject, public ImportExportInterface
{
    Q_OBJECT

public:
    explicit InTouchExporter(QObject *parent = nullptr);
    ~InTouchExporter() override;

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

    // InTouch 標籤類型
    enum TagType {
        MemoryDiscrete,      // 記憶體離散標籤
        MemoryInteger,       // 記憶體整數標籤
        MemoryReal,          // 記憶體實數標籤
        MemoryMessage,       // 記憶體訊息標籤
        IODiscrete,          // I/O 離散標籤
        IOInteger,           // I/O 整數標籤
        IOReal,              // I/O 實數標籤
        GroupVar,            // 群組變數
        IndirectDiscrete,    // 間接離散標籤
        IndirectAnalog       // 間接類比標籤
    };

    // 解析 InTouch CSV 檔案
    QList<AssetData> parseInTouchCsv(const QString &filePath, const QMap<QString, QVariant> &options);
    
    // 生成 InTouch CSV 檔案
    bool generateInTouchCsv(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options);
    
    // 將資產轉換為 InTouch 標籤
    QString convertAssetToInTouchTag(const AssetData &asset);
    
    // 將 InTouch 標籤轉換為資產
    AssetData convertInTouchTagToAsset(const QStringList &tagFields);
    
    // 取得標籤類型字串
    QString getTagTypeString(TagType type) const;
    
    // 從字串取得標籤類型
    TagType getTagTypeFromString(const QString &typeStr) const;
    
    // 驗證標籤資料
    bool validateTagData(const QStringList &fields) const;
};

#endif // INTOUCHEXPORTER_H
