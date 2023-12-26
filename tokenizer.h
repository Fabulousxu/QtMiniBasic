#pragma once

#include <QtCore/qstring.h>
#include <QtCore/qpair.h>

enum token_t { TK_KEYWORD, TK_IDENTIFIER, TK_CONSTANT, TK_OPERATOR, TK_STRING };
enum keyword_t { KW_REM, KW_LET, KW_PRINT, KW_INPUT, KW_GOTO, KW_IF, KW_THEN, KW_END };
enum op_t { POW, POS, NEG, MUL, DIV, MOD, ADD, SUB, LT, LE, GE, GT, EQ, NE, LPAREN, RPAREN };
const QString kw_str[]{ "REM", "LET", "PRINT", "INPUT", "GOTO", "IF", "THEN", "END" };
const QString op_str[]{ "**", "+", "-", "*", "/", "MOD", "+", "-", "<", "<=", ">=", ">", "=", "<>", "(", ")" };

struct token_node {
	token_t type;
	token_node *next;
	token_node(token_t type) :type(type), next(nullptr) {}
	~token_node() { delete next; }
};

struct kw_token : token_node {
	keyword_t val;
	kw_token(keyword_t val) : token_node(TK_KEYWORD), val(val) {}
};

struct id_token : token_node {
	QString val;
	id_token(const QString &val) : token_node(TK_IDENTIFIER), val(val) {}
};

struct const_token : token_node {
	qint64 val;
	const_token(qint64 val) : token_node(TK_CONSTANT), val(val) {}
};

struct op_token : token_node {
	op_t val;
	op_token(op_t val) : token_node(TK_OPERATOR), val(val) {}
};

struct str_token : token_node {
	QString val;
	str_token(const QString &val) : token_node(TK_STRING), val(val) {}
};

token_node *scan(const QString &code);

QString to_HTML(token_node *token);
