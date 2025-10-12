#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QTabWidget>
#include <QDockWidget>
#include <QTextEdit>
#include <QListView>
#include <QStandardItemModel>
#include "assetmodel.h"
#include "asseteditor.h"
#include "assetvisualizer.h"
#include "importexportmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void importAssets();
    void exportAssets();
    void validateIADL();
    void showAssetTypeLibrary();
    void showAssetRelationships();
    void showAssetVisualization();
    void assetSelected(const QModelIndex &index);

private:
    void setupUI();
    void createActions();
    void createMenus();
    void createToolbars();
    void createDockWindows();
    void loadExampleAssets();

    Ui::MainWindow *ui;
    
    // Central widget components
    QTabWidget *tabWidget;
    AssetEditor *assetEditor;
    AssetVisualizer *assetVisualizer;
    
    // Dock widgets
    QDockWidget *projectDock;
    QDockWidget *assetTypeDock;
    QDockWidget *propertiesDock;
    QDockWidget *validationDock;
    
    // Models and views
    AssetModel *assetModel;
    QTreeView *assetTreeView;
    QListView *assetTypeListView;
    QStandardItemModel *assetTypeModel;
    QTextEdit *propertiesTextEdit;
    QTextEdit *validationTextEdit;
    
    // Import/Export manager
    ImportExportManager *importExportManager;
    
    // Current file
    QString currentFile;
};
#endif // MAINWINDOW_H
