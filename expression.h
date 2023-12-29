#pragma once

#include "token.h"
#include "preference.h"
#include <QtCore/qstringlist.h>

enum ExpressionType {
	EXPRESSION_VARIABLE,
	EXPRESSION_NUMBER,
	EXPRESSION_OPERATOR
};

struct Expression {
	ExpressionType type;
	Expression(ExpressionType type) : type(type) {}
	virtual ~Expression() {}
	virtual QStringList toHTML() const = 0;
};

struct VariableExpression : Expression {
	QString value;
	VariableExpression(const QString &value) : Expression(EXPRESSION_VARIABLE), value(value) {}
	QStringList toHTML() const;
};

struct NumberExpression : Expression {
	qint64 value;
	NumberExpression(qint64 value) : Expression(EXPRESSION_NUMBER), value(value) {}
	QStringList toHTML() const;
};

struct OperatorExpression : Expression {
	OperatorType value;
	OperatorExpression(OperatorType value) : Expression(EXPRESSION_OPERATOR), value(value) {}
};

struct UnaryOperatorExpression : OperatorExpression {
	Expression *child;
	UnaryOperatorExpression(OperatorType value, Expression *child) : OperatorExpression(value), child(child) {}
	~UnaryOperatorExpression();
	QStringList toHTML() const;
};

struct BinaryOperatorExpression : OperatorExpression {
	Expression *left;
	Expression *right;
	BinaryOperatorExpression(OperatorType value, Expression *left, Expression *right)
		: OperatorExpression(value), left(left), right(right) {}
	~BinaryOperatorExpression();
	QStringList toHTML() const;
};
