#pragma once

#include "preference.h"
#include <QtCore/qstring.h>

enum TokenType {
	TOKEN_KEYWORD,
	TOKEN_IDENTIFIER,
	TOKEN_CONSTANT,
	TOKEN_OPERATOR,
	TOKEN_STRING
};

enum KeywordType {
	KEYWORD_REM,
	KEYWORD_LET,
	KEYWORD_PRINT,
	KEYWORD_INPUT,
	KEYWORD_GOTO,
	KEYWORD_IF,
	KEYWORD_THEN,
	KEYWORD_END,
	KEYWORD_MOD
};

enum OperatorType {
	POWER,
	POSITIVE,
	NEGATIVE,
	TIMES,
	OVER,
	MOD,
	PLUS,
	MINUS,
	LESS,
	LESS_EQUAL,
	GREATER_EQUAL,
	GREATER,
	EQUAL,
	NOT_EQUAL,
	LEFT_PAREN,
	RIGHT_PAREN
};

const QString keywordString[]{
	"REM",
	"LET",
	"PRINT",
	"INPUT",
	"GOTO",
	"IF",
	"THEN",
	"END",
	"MOD"
};

const QString operatorString[]{
	"**",
	"+",
	"-",
	"*",
	"/",
	"MOD",
	"+",
	"-",
	"<",
	"<=",
	">=",
	">",
	"=",
	"<>",
	"(",
	")"
};

struct Token {
	TokenType type;
	Token(TokenType type) : type(type) {}
	virtual QString toHTML() const = 0;
};

struct KeywordToken : Token {
	KeywordType value;
	KeywordToken(KeywordType value) : Token(TOKEN_KEYWORD), value(value) {}
	QString toHTML() const { return makeFont(tokenColor[type], keywordString[value]); }
};

struct IdentifierToken : Token {
	QString value;
	IdentifierToken(const QString &value) : Token(TOKEN_IDENTIFIER), value(value) {}
	QString toHTML() const { return makeFont(tokenColor[type], value); }
};

struct ConstantToken : Token {
	qint64 value;
	ConstantToken(qint64 value) : Token(TOKEN_CONSTANT), value(value) {}
	QString toHTML() const { return makeFont(tokenColor[type], QString::number(value)); }
};

struct OperatorToken : Token {
	OperatorType value;
	OperatorToken(OperatorType value) : Token(TOKEN_OPERATOR), value(value) {}
	QString toHTML() const { return makeFont(tokenColor[type], operatorString[value]); }
};

struct StringToken : Token {
	QString value;
	StringToken(const QString &value) : Token(TOKEN_STRING), value(value) {}
	QString toHTML() const { return makeFont(tokenColor[type], value); }
};

QString toHTML(const QList<Token *> &tokenList);

void clearTokenList(QList<Token *> &tokenList);
