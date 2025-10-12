#include "importexportdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QTextEdit>

ImportExportDialog::ImportExportDialog(Mode mode, QWidget *parent)
    : QDialog(parent)
    , m_mode(mode)
    , m_selectedSystemType(ImportExportInterface::SystemType::AVEVA_PI)
{
    setWindowTitle(mode == Import ? tr("導入資產") : tr("導出資產"));
    setMinimumWidth(600);
    setMinimumHeight(500);
    
    initUI();
    initConnections();
}

ImportExportDialog::~ImportExportDialog()
{
}

void ImportExportDialog::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 系統類型選擇
    QGroupBox *systemGroup = new QGroupBox(tr("系統類型"), this);
    QFormLayout *systemLayout = new QFormLayout(systemGroup);
    
    m_systemTypeCombo = new QComboBox(this);
    m_systemTypeCombo->addItem(tr("AVEVA PI System"), 
        static_cast<int>(ImportExportInterface::SystemType::AVEVA_PI));
    m_systemTypeCombo->addItem(tr("AVEVA HMI SCADA"), 
        static_cast<int>(ImportExportInterface::SystemType::AVEVA_HMI_SCADA));
    m_systemTypeCombo->addItem(tr("BIM (IFC)"), 
        static_cast<int>(ImportExportInterface::SystemType::BIM));
    m_systemTypeCombo->addItem(tr("ETAP"), 
        static_cast<int>(ImportExportInterface::SystemType::ETAP));
    m_systemTypeCombo->addItem(tr("NVIDIA Omniverse"), 
        static_cast<int>(ImportExportInterface::SystemType::OMNIVERSE));
    m_systemTypeCombo->addItem(tr("JSON"), 
        static_cast<int>(ImportExportInterface::SystemType::JSON));
    
    systemLayout->addRow(tr("系統:"), m_systemTypeCombo);
    mainLayout->addWidget(systemGroup);
    
    // 文件路徑選擇
    QGroupBox *fileGroup = new QGroupBox(tr("文件路徑"), this);
    QHBoxLayout *fileLayout = new QHBoxLayout(fileGroup);
    
    m_filePathEdit = new QLineEdit(this);
    m_browseButton = new QPushButton(tr("瀏覽..."), this);
    
    fileLayout->addWidget(m_filePathEdit);
    fileLayout->addWidget(m_browseButton);
    mainLayout->addWidget(fileGroup);
    
    // 選項面板
    QGroupBox *optionsGroup = new QGroupBox(tr("選項"), this);
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);
    
    m_optionsStack = new QStackedWidget(this);
    
    // 為每個系統類型創建選項面板
    m_optionsPanels[ImportExportInterface::SystemType::AVEVA_PI] = createAvevaPiOptionsPanel();
    m_optionsPanels[ImportExportInterface::SystemType::AVEVA_HMI_SCADA] = createAvevaHmiScadaOptionsPanel();
    m_optionsPanels[ImportExportInterface::SystemType::BIM] = createBimOptionsPanel();
    m_optionsPanels[ImportExportInterface::SystemType::ETAP] = createEtapOptionsPanel();
    m_optionsPanels[ImportExportInterface::SystemType::OMNIVERSE] = createOmniverseOptionsPanel();
    
    // 添加到堆疊窗口部件
    for (auto it = m_optionsPanels.begin(); it != m_optionsPanels.end(); ++it) {
        m_optionsStack->addWidget(it.value());
    }
    
    optionsLayout->addWidget(m_optionsStack);
    mainLayout->addWidget(optionsGroup);
    
    // 進度條
    m_progressBar = new QProgressBar(this);
    m_progressBar->setVisible(false);
    mainLayout->addWidget(m_progressBar);
    
    // 狀態標籤
    m_statusLabel = new QLabel(this);
    m_statusLabel->setVisible(false);
    mainLayout->addWidget(m_statusLabel);
    
    // 按鈕
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    m_executeButton = new QPushButton(m_mode == Import ? tr("導入") : tr("導出"), this);
    m_cancelButton = new QPushButton(tr("取消"), this);
    
    buttonLayout->addWidget(m_executeButton);
    buttonLayout->addWidget(m_cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    // 初始化選項面板
    updateOptionsPanel();
}

void ImportExportDialog::initConnections()
{
    connect(m_systemTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ImportExportDialog::onSystemTypeChanged);
    connect(m_browseButton, &QPushButton::clicked,
            this, &ImportExportDialog::onBrowseClicked);
    connect(m_executeButton, &QPushButton::clicked,
            this, &ImportExportDialog::onExecute);
    connect(m_cancelButton, &QPushButton::clicked,
            this, &ImportExportDialog::onCancel);
}

QWidget* ImportExportDialog::createAvevaPiOptionsPanel()
{
    QWidget *panel = new QWidget(this);
    QFormLayout *layout = new QFormLayout(panel);
    
    QLineEdit *serverEdit = new QLineEdit(panel);
    serverEdit->setObjectName("server");
    serverEdit->setPlaceholderText(tr("例如: pi-server.company.com"));
    layout->addRow(tr("PI Server:"), serverEdit);
    
    QLineEdit *usernameEdit = new QLineEdit(panel);
    usernameEdit->setObjectName("username");
    layout->addRow(tr("用戶名:"), usernameEdit);
    
    QLineEdit *passwordEdit = new QLineEdit(panel);
    passwordEdit->setObjectName("password");
    passwordEdit->setEchoMode(QLineEdit::Password);
    layout->addRow(tr("密碼:"), passwordEdit);
    
    QCheckBox *includeHistoryCheck = new QCheckBox(tr("包含歷史數據"), panel);
    includeHistoryCheck->setObjectName("includeHistory");
    layout->addRow(includeHistoryCheck);
    
    QSpinBox *daysSpinBox = new QSpinBox(panel);
    daysSpinBox->setObjectName("historyDays");
    daysSpinBox->setRange(1, 365);
    daysSpinBox->setValue(30);
    daysSpinBox->setSuffix(tr(" 天"));
    layout->addRow(tr("歷史數據天數:"), daysSpinBox);
    
    return panel;
}

QWidget* ImportExportDialog::createAvevaHmiScadaOptionsPanel()
{
    QWidget *panel = new QWidget(this);
    QFormLayout *layout = new QFormLayout(panel);
    
    QLineEdit *projectPathEdit = new QLineEdit(panel);
    projectPathEdit->setObjectName("projectPath");
    layout->addRow(tr("專案路徑:"), projectPathEdit);
    
    QCheckBox *includeGraphicsCheck = new QCheckBox(tr("包含圖形"), panel);
    includeGraphicsCheck->setObjectName("includeGraphics");
    includeGraphicsCheck->setChecked(true);
    layout->addRow(includeGraphicsCheck);
    
    QCheckBox *includeAlarmsCheck = new QCheckBox(tr("包含警報"), panel);
    includeAlarmsCheck->setObjectName("includeAlarms");
    includeAlarmsCheck->setChecked(true);
    layout->addRow(includeAlarmsCheck);
    
    return panel;
}

QWidget* ImportExportDialog::createBimOptionsPanel()
{
    QWidget *panel = new QWidget(this);
    QFormLayout *layout = new QFormLayout(panel);
    
    QComboBox *ifcVersionCombo = new QComboBox(panel);
    ifcVersionCombo->setObjectName("ifcVersion");
    ifcVersionCombo->addItem("IFC2x3");
    ifcVersionCombo->addItem("IFC4");
    ifcVersionCombo->addItem("IFC4.3");
    ifcVersionCombo->setCurrentIndex(1);
    layout->addRow(tr("IFC 版本:"), ifcVersionCombo);
    
    QCheckBox *includeGeometryCheck = new QCheckBox(tr("包含幾何資訊"), panel);
    includeGeometryCheck->setObjectName("includeGeometry");
    includeGeometryCheck->setChecked(true);
    layout->addRow(includeGeometryCheck);
    
    QCheckBox *includePropertiesCheck = new QCheckBox(tr("包含屬性資訊"), panel);
    includePropertiesCheck->setObjectName("includeProperties");
    includePropertiesCheck->setChecked(true);
    layout->addRow(includePropertiesCheck);
    
    QCheckBox *includeRelationshipsCheck = new QCheckBox(tr("包含關係資訊"), panel);
    includeRelationshipsCheck->setObjectName("includeRelationships");
    includeRelationshipsCheck->setChecked(true);
    layout->addRow(includeRelationshipsCheck);
    
    return panel;
}

QWidget* ImportExportDialog::createEtapOptionsPanel()
{
    QWidget *panel = new QWidget(this);
    QFormLayout *layout = new QFormLayout(panel);
    
    QLineEdit *projectFileEdit = new QLineEdit(panel);
    projectFileEdit->setObjectName("projectFile");
    layout->addRow(tr("ETAP 專案檔:"), projectFileEdit);
    
    QCheckBox *includePowerFlowCheck = new QCheckBox(tr("包含潮流分析"), panel);
    includePowerFlowCheck->setObjectName("includePowerFlow");
    includePowerFlowCheck->setChecked(true);
    layout->addRow(includePowerFlowCheck);
    
    QCheckBox *includeShortCircuitCheck = new QCheckBox(tr("包含短路分析"), panel);
    includeShortCircuitCheck->setObjectName("includeShortCircuit");
    layout->addRow(includeShortCircuitCheck);
    
    QCheckBox *includeProtectionCheck = new QCheckBox(tr("包含保護設備"), panel);
    includeProtectionCheck->setObjectName("includeProtection");
    layout->addRow(includeProtectionCheck);
    
    return panel;
}

QWidget* ImportExportDialog::createOmniverseOptionsPanel()
{
    QWidget *panel = new QWidget(this);
    QFormLayout *layout = new QFormLayout(panel);
    
    QLineEdit *serverEdit = new QLineEdit(panel);
    serverEdit->setObjectName("omniverseServer");
    serverEdit->setPlaceholderText(tr("例如: omniverse://localhost"));
    layout->addRow(tr("Omniverse Server:"), serverEdit);
    
    QLineEdit *stagePathEdit = new QLineEdit(panel);
    stagePathEdit->setObjectName("stagePath");
    stagePathEdit->setPlaceholderText(tr("例如: /Projects/Factory"));
    layout->addRow(tr("Stage 路徑:"), stagePathEdit);
    
    QCheckBox *includeVisualizationCheck = new QCheckBox(tr("包含視覺化資訊"), panel);
    includeVisualizationCheck->setObjectName("includeVisualization");
    includeVisualizationCheck->setChecked(true);
    layout->addRow(includeVisualizationCheck);
    
    QCheckBox *includePhysicsCheck = new QCheckBox(tr("包含物理屬性"), panel);
    includePhysicsCheck->setObjectName("includePhysics");
    layout->addRow(includePhysicsCheck);
    
    return panel;
}

void ImportExportDialog::onSystemTypeChanged(int index)
{
    m_selectedSystemType = static_cast<ImportExportInterface::SystemType>(
        m_systemTypeCombo->itemData(index).toInt());
    updateOptionsPanel();
}

void ImportExportDialog::updateOptionsPanel()
{
    if (m_optionsPanels.contains(m_selectedSystemType)) {
        m_optionsStack->setCurrentWidget(m_optionsPanels[m_selectedSystemType]);
    }
}

void ImportExportDialog::onBrowseClicked()
{
    QString filter;
    QString defaultSuffix;
    
    switch (m_selectedSystemType) {
        case ImportExportInterface::SystemType::AVEVA_PI:
            filter = tr("PI 配置文件 (*.xml *.json);;所有文件 (*.*)");
            defaultSuffix = "xml";
            break;
        case ImportExportInterface::SystemType::AVEVA_HMI_SCADA:
            filter = tr("SCADA 專案 (*.aaPKG);;所有文件 (*.*)");
            defaultSuffix = "aaPKG";
            break;
        case ImportExportInterface::SystemType::BIM:
            filter = tr("IFC 文件 (*.ifc);;所有文件 (*.*)");
            defaultSuffix = "ifc";
            break;
        case ImportExportInterface::SystemType::ETAP:
            filter = tr("ETAP 專案 (*.etap);;所有文件 (*.*)");
            defaultSuffix = "etap";
            break;
        case ImportExportInterface::SystemType::OMNIVERSE:
            filter = tr("USD 文件 (*.usd *.usda *.usdc);;所有文件 (*.*)");
            defaultSuffix = "usd";
            break;
        case ImportExportInterface::SystemType::JSON:
            filter = tr("JSON 文件 (*.json);;所有文件 (*.*)");
            defaultSuffix = "json";
            break;
        default:
            filter = tr("所有文件 (*.*)");
            break;
    }
    
    QString filePath;
    if (m_mode == Import) {
        filePath = QFileDialog::getOpenFileName(this, tr("選擇文件"), 
            QString(), filter);
    } else {
        filePath = QFileDialog::getSaveFileName(this, tr("保存文件"), 
            QString(), filter);
    }
    
    if (!filePath.isEmpty()) {
        m_filePathEdit->setText(filePath);
    }
}

void ImportExportDialog::onExecute()
{
    // 收集選項
    collectOptions();
    
    // 驗證輸入
    if (m_filePathEdit->text().isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("請選擇文件路徑"));
        return;
    }
    
    m_selectedFilePath = m_filePathEdit->text();
    
    // 顯示進度
    m_progressBar->setVisible(true);
    m_progressBar->setValue(0);
    m_statusLabel->setVisible(true);
    m_statusLabel->setText(m_mode == Import ? tr("正在導入...") : tr("正在導出..."));
    
    // 禁用按鈕
    m_executeButton->setEnabled(false);
    m_cancelButton->setEnabled(false);
    
    // 這裡應該啟動實際的導入/導出操作
    // 在實際應用中，這會調用 ImportExportManager
    
    // 模擬進度
    m_progressBar->setValue(100);
    m_statusLabel->setText(m_mode == Import ? tr("導入完成") : tr("導出完成"));
    
    // 接受對話框
    accept();
}

void ImportExportDialog::onCancel()
{
    reject();
}

void ImportExportDialog::collectOptions()
{
    m_options.clear();
    
    QWidget *currentPanel = m_optionsStack->currentWidget();
    if (!currentPanel) return;
    
    // 遍歷當前面板中的所有子控件
    QList<QLineEdit*> lineEdits = currentPanel->findChildren<QLineEdit*>();
    for (QLineEdit *edit : lineEdits) {
        if (!edit->objectName().isEmpty()) {
            m_options[edit->objectName()] = edit->text();
        }
    }
    
    QList<QCheckBox*> checkBoxes = currentPanel->findChildren<QCheckBox*>();
    for (QCheckBox *checkBox : checkBoxes) {
        if (!checkBox->objectName().isEmpty()) {
            m_options[checkBox->objectName()] = checkBox->isChecked();
        }
    }
    
    QList<QSpinBox*> spinBoxes = currentPanel->findChildren<QSpinBox*>();
    for (QSpinBox *spinBox : spinBoxes) {
        if (!spinBox->objectName().isEmpty()) {
            m_options[spinBox->objectName()] = spinBox->value();
        }
    }
    
    QList<QComboBox*> comboBoxes = currentPanel->findChildren<QComboBox*>();
    for (QComboBox *comboBox : comboBoxes) {
        if (!comboBox->objectName().isEmpty()) {
            m_options[comboBox->objectName()] = comboBox->currentText();
        }
    }
}

void ImportExportDialog::onProgressUpdated(int progress, const QString &message)
{
    m_progressBar->setValue(progress);
    m_statusLabel->setText(message);
}

void ImportExportDialog::onErrorOccurred(const QString &errorMessage)
{
    m_statusLabel->setText(tr("錯誤: %1").arg(errorMessage));
    m_executeButton->setEnabled(true);
    m_cancelButton->setEnabled(true);
    
    QMessageBox::critical(this, tr("錯誤"), errorMessage);
}

void ImportExportDialog::onCompleted(bool success, const QString &message)
{
    m_progressBar->setValue(100);
    m_statusLabel->setText(message);
    m_executeButton->setEnabled(true);
    m_cancelButton->setEnabled(true);
    
    if (success) {
        QMessageBox::information(this, tr("完成"), message);
        accept();
    } else {
        QMessageBox::warning(this, tr("警告"), message);
    }
}

ImportExportInterface::SystemType ImportExportDialog::selectedSystemType() const
{
    return m_selectedSystemType;
}

QString ImportExportDialog::selectedFilePath() const
{
    return m_selectedFilePath;
}

QMap<QString, QVariant> ImportExportDialog::options() const
{
    return m_options;
}
