#include "iadlsyntaxhighlighter.h"

IADLSyntaxHighlighter::IADLSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // Define formats
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    valueFormat.setForeground(Qt::darkGreen);
    
    stringFormat.setForeground(Qt::darkRed);
    
    commentFormat.setForeground(Qt::gray);
    commentFormat.setFontItalic(true);

    // Define highlighting rules
    
    // Keywords
    QStringList keywordPatterns;
    keywordPatterns << "\\bname:\\b" << "\\btype:\\b" << "\\bdescription:\\b"
                    << "\\bproperties:\\b" << "\\bsignals:\\b" << "\\brelationships:\\b"
                    << "\\binterfaces:\\b" << "\\bid:\\b" << "\\bversion:\\b";
    
    for (const QString &pattern : keywordPatterns) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    
    // Values (numbers, booleans)
    HighlightingRule rule;
    rule.pattern = QRegularExpression("\\b[0-9]+\\b|\\btrue\\b|\\bfalse\\b");
    rule.format = valueFormat;
    highlightingRules.append(rule);
    
    // Strings
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = stringFormat;
    highlightingRules.append(rule);
    
    // Comments
    rule.pattern = QRegularExpression("#[^\n]*");
    rule.format = commentFormat;
    highlightingRules.append(rule);
}

void IADLSyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
