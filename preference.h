#pragma once

#include <QtCore/qstring.h>
#include <QtCore/qmap.h>

const QString tokenColor[]{
	"skyblue",//TOKEN_KEYWORD
	"white",//TOKEN_IDENTIFIER
	"darkorange",//TOKEN_CONSTANT
	"gold",//TOKEN_OPERATOR
	"grey"//TOKEN_STRING
};

enum TreeElementType {
	TREE_STATEMENT,
	TREE_STATEMENT_COUNT,
	TREE_VARIABLE_COUNT,
	TREE_VARIABLE_ASSIGNMENT,
	TREE_VARIABLE_EXPRESSION,
	TREE_DESTINATION,
	TREE_NUMBER,
	TREE_OPERATOR,
	TREE_STRING
};

const QString treeColor[]{
	"skyblue",//TREE_STATEMENT
	"deeppink",//TREE_STATEMENT_COUNT
	"pink",//TREE_VARIABLE_COUNT
	"yellow",//TREE_VARIABLE_ASSIGNMENT
	"white",//TREE_VARIABLE_EXPRESSION
	"green",//TREE_DESTINATION
	"darkorange",//TREE_NUMBER
	"gold",//TREE_OPERATOR
	"grey"//TREE_STRING
};

const QString lineNumberColor("lightgreen");
const QString errorColor("red");
const QString warningColor("red");

inline QString makeFont(const QString &color, const QString &string) {
	return QString("<font color = ") + color + QString(">") + string + QString("</font>");
}
const QString identation("&nbsp;&nbsp;&nbsp;&nbsp;");
