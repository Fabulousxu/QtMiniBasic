#include "statement.h"

QStringList RemStatement::toHTML() const {
    return {
        makeFont(treeColor[TREE_STATEMENT], keywordString[KEYWORD_REM]) ,
        identation + makeFont(treeColor[TREE_STRING], comment)
    };
}

QStringList LetStatement::toHTML() const {
    QStringList expressionHTML = expression->toHTML();
    for (QString &line : expressionHTML) { line.push_front(identation); }
    return QStringList{
        makeFont(treeColor[TREE_STATEMENT], keywordString[KEYWORD_LET]),
        identation + makeFont(treeColor[TREE_VARIABLE_ASSIGNMENT], variable)
    } + expressionHTML;
}

QStringList PrintStatement::toHTML() const {
    QStringList expressionHTML = expression->toHTML();
    for (QString &line : expressionHTML) { line.push_front(identation); }
    return QStringList{ makeFont(treeColor[TREE_STATEMENT], keywordString[KEYWORD_PRINT]) } + expressionHTML;
}

QStringList InputStatement::toHTML() const {
    return {
        makeFont(treeColor[TREE_STATEMENT], keywordString[KEYWORD_INPUT]),
        identation + makeFont(treeColor[TREE_VARIABLE_ASSIGNMENT], variable)
    };
}

QStringList GotoStatement::toHTML() const {
    return {
        makeFont(treeColor[TREE_STATEMENT], keywordString[KEYWORD_GOTO]),
        identation + makeFont(treeColor[TREE_DESTINATION], QString::number(destination))
    };
}

QStringList IfStatement::toHTML() const {
    QStringList conditionHTML = condition->toHTML();
    for (QString &line : conditionHTML) { line.push_front(identation); }
    return QStringList{
        makeFont(treeColor[TREE_STATEMENT], keywordString[KEYWORD_IF] + "&nbsp;" + keywordString[KEYWORD_THEN])
    } + conditionHTML + QStringList{
        identation + makeFont(treeColor[TREE_DESTINATION], QString::number(destination))
    };
}

QStringList EndStatement::toHTML() const {
    return { makeFont(treeColor[TREE_STATEMENT], keywordString[KEYWORD_END]) };
}
