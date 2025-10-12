#ifndef IADLSYNTAXHIGHLIGHTER_H
#define IADLSYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QRegularExpression>

class IADLSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    IADLSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat valueFormat;
    QTextCharFormat stringFormat;
    QTextCharFormat commentFormat;
};

#endif // IADLSYNTAXHIGHLIGHTER_H
