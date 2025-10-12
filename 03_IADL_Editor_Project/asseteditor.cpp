#include "asseteditor.h"
#include "ui_asseteditor.h"
#include <QVBoxLayout>
#include <QFont>

AssetEditor::AssetEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AssetEditor),
    currentAsset(nullptr)
{
    ui->setupUi(this);
    
    // Create text editor
    textEditor = new QTextEdit(this);
    QFont font("Courier New", 10);
    font.setFixedPitch(true);
    textEditor->setFont(font);
    
    // Create syntax highlighter
    highlighter = new IADLSyntaxHighlighter(textEditor->document());
    
    // Set layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(textEditor);
    setLayout(layout);
}

AssetEditor::~AssetEditor()
{
    delete ui;
}

void AssetEditor::loadAsset(AssetItem *item)
{
    if (item) {
        currentAsset = item;
        textEditor->setPlainText(item->content());
    }
}

void AssetEditor::setContent(const QString &content)
{
    textEditor->setPlainText(content);
}

QString AssetEditor::content() const
{
    return textEditor->toPlainText();
}
