#ifndef ASSETEDITOR_H
#define ASSETEDITOR_H

#include <QWidget>
#include <QTextEdit>
#include "assetitem.h"
#include "iadlsyntaxhighlighter.h"

namespace Ui {
class AssetEditor;
}

class AssetEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AssetEditor(QWidget *parent = nullptr);
    ~AssetEditor();
    
    void loadAsset(AssetItem *item);
    void setContent(const QString &content);
    QString content() const;

private:
    Ui::AssetEditor *ui;
    QTextEdit *textEditor;
    IADLSyntaxHighlighter *highlighter;
    AssetItem *currentAsset;
};

#endif // ASSETEDITOR_H
