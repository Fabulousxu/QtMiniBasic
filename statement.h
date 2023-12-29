#pragma once

#include "expression.h"

enum StatementType {
	STATEMENT_REM,
	STATEMENT_LET,
	STATEMENT_PRINT,
	STATEMENT_INPUT,
	STATEMENT_GOTO,
	STATEMENT_IF,
	STATEMENT_END
};

struct Statement {
	StatementType type;
	Statement(StatementType type) : type(type) {}
	virtual ~Statement() {}
	virtual QStringList toHTML() const = 0;
	virtual void zeroOut() = 0;
};

struct RemStatement : Statement {
	QString comment;
	RemStatement(const QString &comment) : Statement(STATEMENT_REM), comment(comment) {}
	QStringList toHTML() const;
	void zeroOut() {}
};

struct LetStatement : Statement {
	QString variable;
	Expression *expression;
	size_t count;
	LetStatement(const QString &variable, Expression *expression) 
		: Statement(STATEMENT_LET), variable(variable), expression(expression), count(0) {}
	~LetStatement() { delete expression; }
	QStringList toHTML() const;
	void zeroOut() { count = 0; }
};

struct PrintStatement : Statement {
	Expression *expression;
	size_t count;
	PrintStatement(Expression *expression) : Statement(STATEMENT_PRINT), expression(expression), count(0) {}
	~PrintStatement() { delete expression; }
	QStringList toHTML() const;
	void zeroOut() { count = 0; }
};

struct InputStatement : Statement {
	QString variable;
	size_t count;
	InputStatement(const QString &variable) : Statement(STATEMENT_INPUT), variable(variable), count(0) {}
	QStringList toHTML() const;
	void zeroOut() { count = 0; }
};

struct GotoStatement : Statement {
	size_t destination;
	size_t count;
	GotoStatement(size_t destination) : Statement(STATEMENT_GOTO), destination(destination), count(0) {}
	QStringList toHTML() const;
	void zeroOut() { count = 0; }
};

struct IfStatement : Statement {
	Expression *condition;
	size_t destination;
	size_t ifCount;
	size_t thenCount;
	IfStatement(Expression *condition, size_t destination) 
		: Statement(STATEMENT_IF), condition(condition), destination(destination), ifCount(0), thenCount(0) {}
	~IfStatement() { delete condition; }
	QStringList toHTML() const;
	void zeroOut() { ifCount = thenCount = 0; }
};

struct EndStatement : Statement {
	size_t count;
	EndStatement() : Statement(STATEMENT_END), count(0) {}
	QStringList toHTML() const;
	void zeroOut() { count = 0; }
};
