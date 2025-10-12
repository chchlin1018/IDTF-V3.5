#ifndef IMPORTEXPORTINTERFACE_H
#define IMPORTEXPORTINTERFACE_H

#include <QString>
#include <QList>
#include <QVariantMap>

// AssetData is a plain struct for data transfer.
struct AssetData {
    QString id;
    QString name;
    QString type;
    QVariantMap properties;
};

// The interface is a pure abstract class.
class ImportExportInterface
{
public:
    enum SystemType {
        AVEVA_PI,
        AVEVA_HMI_SCADA,
        BIM,
        ETAP,
        OMNIVERSE,
        JSON,
        INTOUCH,
        SYSTEM_PLATFORM,
        EDGE,
        HISTORIAN,
        IGNITION
    };

    virtual ~ImportExportInterface() = default;

    // Methods for the interface
    virtual QList<AssetData> importAssets(const QString &filePath, const QMap<QString, QVariant> &options) = 0;
    virtual bool exportAssets(const QString &filePath, const QList<AssetData> &assets, const QMap<QString, QVariant> &options) = 0;
    virtual QStringList supportedFileExtensions() const = 0;
    virtual SystemType systemType() const = 0;
    virtual QString systemName() const = 0;
    virtual QString systemDescription() const = 0;
    virtual bool validateFile(const QString &filePath) = 0;

    // For error reporting from plugins
    QString getLastError() const { return m_lastError; }

protected:
    // Plugins can set this error string.
    mutable QString m_lastError;
};

#endif // IMPORTEXPORTINTERFACE_H
