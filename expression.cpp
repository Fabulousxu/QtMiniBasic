#include "expression.h"

QStringList VariableExpression::toHTML() const {
	return { makeFont(treeColor[TREE_VARIABLE_EXPRESSION], value) };
}

QStringList NumberExpression::toHTML() const {
	return { makeFont(treeColor[TREE_NUMBER], QString::number(value)) };
}

UnaryOperatorExpression::~UnaryOperatorExpression() {
	if (child && child->type == EXPRESSION_OPERATOR) {
		if (((OperatorExpression *)child)->value == POSITIVE ||
			((OperatorExpression *)child)->value == NEGATIVE) {
			delete ((UnaryOperatorExpression *)child)->child;
			((UnaryOperatorExpression *)child)->child = nullptr;
		} else {
			delete ((BinaryOperatorExpression *)child)->left;
			((BinaryOperatorExpression *)child)->left = nullptr;
			delete ((BinaryOperatorExpression *)child)->right;
			((BinaryOperatorExpression *)child)->right = nullptr;
		}
	}
	delete child;
}

QStringList UnaryOperatorExpression::toHTML() const {
	QStringList childHTML = child->toHTML();
	for (QString &line : childHTML) { line.push_front(identation); }
	return QStringList{ makeFont(treeColor[TREE_OPERATOR],operatorString[value]) } + childHTML;
}

BinaryOperatorExpression::~BinaryOperatorExpression() {
	if (left && left->type == EXPRESSION_OPERATOR) {
		if (((OperatorExpression *)left)->value == POSITIVE ||
			((OperatorExpression *)left)->value == NEGATIVE) {
			delete ((UnaryOperatorExpression *)left)->child;
			((UnaryOperatorExpression *)left)->child = nullptr;
		} else {
			delete ((BinaryOperatorExpression *)left)->left;
			((BinaryOperatorExpression *)left)->left = nullptr;
			delete ((BinaryOperatorExpression *)left)->right;
			((BinaryOperatorExpression *)left)->right = nullptr;
		}
	}
	delete left;
	if (right && right->type == EXPRESSION_OPERATOR) {
		if (((OperatorExpression *)right)->value == POSITIVE ||
			((OperatorExpression *)right)->value == NEGATIVE) {
			delete ((UnaryOperatorExpression *)right)->child;
			((UnaryOperatorExpression *)right)->child = nullptr;
		} else {
			delete ((BinaryOperatorExpression *)right)->left;
			((BinaryOperatorExpression *)right)->left = nullptr;
			delete ((BinaryOperatorExpression *)right)->right;
			((BinaryOperatorExpression *)right)->right = nullptr;
		}
	}
	delete right;
}

QStringList BinaryOperatorExpression::toHTML() const {
	QStringList leftHTML = left->toHTML();
	QStringList rightHTML = right->toHTML();
	for (QString &line : leftHTML) { line.push_front(identation); }
	for (QString &line : rightHTML) { line.push_front(identation); }
	return QStringList{ makeFont(treeColor[TREE_OPERATOR], operatorString[value]) } + leftHTML + rightHTML;
}
