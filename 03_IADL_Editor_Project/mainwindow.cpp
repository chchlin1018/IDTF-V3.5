#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "importexportdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QSplitter>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    
    importExportManager = new ImportExportManager(this);
    
    setupUI();
    createActions();
    createMenus();
    createToolbars();
    createDockWindows();
    loadExampleAssets();
    
    setWindowTitle("IADL Editor - Prototype");
    setMinimumSize(1024, 768);
    statusBar()->showMessage("Ready");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    tabWidget = new QTabWidget(this);
    assetEditor = new AssetEditor(this);
    tabWidget->addTab(assetEditor, "Asset Editor");
    assetVisualizer = new AssetVisualizer(this);
    tabWidget->addTab(assetVisualizer, "Visualizer");
    setCentralWidget(tabWidget);
}

void MainWindow::createActions()
{
    QAction *newAct = new QAction(QIcon(":/icons/new.png"), "New", this);
    newAct->setShortcuts(QKeySequence::New);
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    
    QAction *openAct = new QAction(QIcon(":/icons/open.png"), "Open", this);
    openAct->setShortcuts(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    
    QAction *saveAct = new QAction(QIcon(":/icons/save.png"), "Save", this);
    saveAct->setShortcuts(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);
}

void MainWindow::createMenus()
{
    QMenu *fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("New");
    fileMenu->addAction("Open...");
    fileMenu->addAction("Save");
    fileMenu->addSeparator();
    
    QAction *importAct = fileMenu->addAction("Import from System...");
    connect(importAct, &QAction::triggered, this, &MainWindow::importAssets);
    
    QAction *exportAct = fileMenu->addAction("Export to System...");
    connect(exportAct, &QAction::triggered, this, &MainWindow::exportAssets);
    
    fileMenu->addSeparator();
    fileMenu->addAction("Exit");
}

void MainWindow::createToolbars()
{
    // Implementation for toolbars
}

void MainWindow::createDockWindows()
{
    // Implementation for dock windows
}

void MainWindow::loadExampleAssets()
{
    // Implementation for loading example assets
}

void MainWindow::newFile()
{
    // Implementation for new file
}

void MainWindow::openFile()
{
    // Implementation for opening a file
}

void MainWindow::saveFile()
{
    // Implementation for saving a file
}

void MainWindow::importAssets()
{
    ImportExportDialog dialog(ImportExportDialog::Import, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString filePath = dialog.selectedFilePath();
        ImportExportInterface::SystemType systemType = dialog.selectedSystemType();
        QMap<QString, QVariant> options = dialog.options();
        
        QList<AssetData> assets = importExportManager->importAssets(systemType, filePath, options);
        
        if (!assets.isEmpty()) {
            for (const auto &assetData : assets) {
                AssetItem *item = new AssetItem(assetData.name);
                item->setAssetType(assetData.type);
                item->setProperties(assetData.properties);
                assetModel->appendRow(item);
            }
            QMessageBox::information(this, "Import Complete", QString("Successfully imported %1 assets.").arg(assets.size()));
        } else {
            QMessageBox::warning(this, "Import Failed", "Could not import any assets.");
        }
    }
}

void MainWindow::exportAssets()
{
    ImportExportDialog dialog(ImportExportDialog::Export, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString filePath = dialog.selectedFilePath();
        ImportExportInterface::SystemType systemType = dialog.selectedSystemType();
        QMap<QString, QVariant> options = dialog.options();
        
        QList<AssetData> assets;
        for (int i = 0; i < assetModel->rowCount(); ++i) {
            AssetItem *item = static_cast<AssetItem*>(assetModel->item(i));
            if (item) {
                AssetData assetData;
                assetData.id = QString("asset_%1").arg(i);
                assetData.name = item->text();
                assetData.type = item->assetType();
                assetData.properties = item->properties();
                assets.append(assetData);
            }
        }
        
        if (assets.isEmpty()) {
            QMessageBox::warning(this, "Export Warning", "No assets to export.");
            return;
        }
        
        if (importExportManager->exportAssets(systemType, filePath, assets, options)) {
            QMessageBox::information(this, "Export Complete", QString("Successfully exported %1 assets.").arg(assets.size()));
        } else {
            QMessageBox::warning(this, "Export Failed", "Could not export assets.");
        }
    }
}

void MainWindow::validateIADL()
{
    // Implementation for validating IADL
}

void MainWindow::showAssetTypeLibrary()
{
    // Implementation for showing asset type library
}

void MainWindow::showAssetRelationships()
{
    // Implementation for showing asset relationships
}

void MainWindow::showAssetVisualization()
{
    // Implementation for showing asset visualization
}

void MainWindow::assetSelected(const QModelIndex &index)
{
    // Implementation for asset selection
}

