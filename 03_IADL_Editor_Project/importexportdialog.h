#ifndef IMPORTEXPORTDIALOG_H
#define IMPORTEXPORTDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QStackedWidget>
#include <QMap>
#include <QVariant>
#include "importexportinterface.h"

/**
 * @brief 導入/導出對話框類
 * 
 * 這個類提供了一個用戶界面，用於選擇導入/導出格式和配置選項。
 */
class ImportExportDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief 對話框模式枚舉
     */
    enum Mode {
        Import, ///< 導入模式
        Export  ///< 導出模式
    };
    
    /**
     * @brief 構造函數
     * @param mode 對話框模式
     * @param parent 父窗口
     */
    explicit ImportExportDialog(Mode mode, QWidget *parent = nullptr);
    
    /**
     * @brief 析構函數
     */
    ~ImportExportDialog();
    
    /**
     * @brief 獲取選擇的系統類型
     * @return 系統類型枚舉值
     */
    ImportExportInterface::SystemType selectedSystemType() const;
    
    /**
     * @brief 獲取選擇的文件路徑
     * @return 文件路徑
     */
    QString selectedFilePath() const;
    
    /**
     * @brief 獲取配置的選項
     * @return 選項的鍵值對
     */
    QMap<QString, QVariant> options() const;

private slots:
    /**
     * @brief 系統類型改變時的處理函數
     * @param index 當前索引
     */
    void onSystemTypeChanged(int index);
    
    /**
     * @brief 瀏覽文件按鈕點擊處理函數
     */
    void onBrowseClicked();
    
    /**
     * @brief 執行導入/導出操作
     */
    void onExecute();
    
    /**
     * @brief 取消操作
     */
    void onCancel();
    
    /**
     * @brief 進度更新處理函數
     * @param progress 進度值(0-100)
     * @param message 進度消息
     */
    void onProgressUpdated(int progress, const QString &message);
    
    /**
     * @brief 錯誤處理函數
     * @param errorMessage 錯誤消息
     */
    void onErrorOccurred(const QString &errorMessage);
    
    /**
     * @brief 完成處理函數
     * @param success 是否成功
     * @param message 完成消息
     */
    void onCompleted(bool success, const QString &message);

private:
    /**
     * @brief 初始化UI
     */
    void initUI();
    
    /**
     * @brief 初始化連接
     */
    void initConnections();
    
    /**
     * @brief 更新選項面板
     */
    void updateOptionsPanel();
    
    /**
     * @brief 創建AVEVA PI選項面板
     * @return 選項面板窗口部件
     */
    QWidget* createAvevaPiOptionsPanel();
    
    /**
     * @brief 創建AVEVA HMI SCADA選項面板
     * @return 選項面板窗口部件
     */
    QWidget* createAvevaHmiScadaOptionsPanel();
    
    /**
     * @brief 創建BIM選項面板
     * @return 選項面板窗口部件
     */
    QWidget* createBimOptionsPanel();
    
    /**
     * @brief 創建ETAP選項面板
     * @return 選項面板窗口部件
     */
    QWidget* createEtapOptionsPanel();
    
    /**
     * @brief 創建Omniverse選項面板
     * @return 選項面板窗口部件
     */
    QWidget* createOmniverseOptionsPanel();
    
    /**
     * @brief 收集選項值
     */
    void collectOptions();
    
    Mode m_mode;
    QComboBox *m_systemTypeCombo;
    QLineEdit *m_filePathEdit;
    QPushButton *m_browseButton;
    QStackedWidget *m_optionsStack;
    QPushButton *m_executeButton;
    QPushButton *m_cancelButton;
    QProgressBar *m_progressBar;
    QLabel *m_statusLabel;
    
    ImportExportInterface::SystemType m_selectedSystemType;
    QString m_selectedFilePath;
    QMap<QString, QVariant> m_options;
    QMap<ImportExportInterface::SystemType, QWidget*> m_optionsPanels;
};

#endif // IMPORTEXPORTDIALOG_H
