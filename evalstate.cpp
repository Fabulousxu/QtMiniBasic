#include "evalstate.h"

qint64 EvaluationState::getValue(Expression *expression) {
	if (expression->type == EXPRESSION_NUMBER) { return ((NumberExpression *)expression)->value; }
	if (expression->type == EXPRESSION_VARIABLE) {
		if (variableMap.contains(((VariableExpression *)expression)->value)) {
			++variableMap[((VariableExpression *)expression)->value].second;
			return variableMap[((VariableExpression *)expression)->value].first;
		} else {
			throw RuntimeError(
				"variable \"" + ((VariableExpression *)expression)->value + "\" is being used without initialized."
			);
		}
	}

#define GET_VALUE_OF_UNARY_OPERATOR_EXPRESSION(op) { \
	qint64 value; \
	try { value = getValue(((UnaryOperatorExpression *)expression)->child); } \
	catch (RuntimeError error) { throw error; } \
	return op value; \
	}

#define GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION(op) { \
	qint64 val1, val2; \
	try { val1 = getValue(((BinaryOperatorExpression *)expression)->left); } \
	catch (RuntimeError error) { throw error; } \
	try { val2 = getValue(((BinaryOperatorExpression *)expression)->right); } \
	catch (RuntimeError error) { throw error; } \
	return val1 op val2; \
	}

	switch (((OperatorExpression *)expression)->value) {
		case POWER: {
			qint64 val1, val2;
			try { val1 = getValue(((BinaryOperatorExpression *)expression)->left); }
			catch (RuntimeError error) { throw error; }
			try { val2 = getValue(((BinaryOperatorExpression *)expression)->right); }
			catch (RuntimeError error) { throw error; }
			if (!val1 && !val2) { throw RuntimeError("0 power 0."); }
			return pow(val1, val2); }
		case POSITIVE: GET_VALUE_OF_UNARY_OPERATOR_EXPRESSION(+)
		case NEGATIVE: GET_VALUE_OF_UNARY_OPERATOR_EXPRESSION(-)
		case TIMES: GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION(*)
		case OVER: {
			qint64 val1, val2;
			try { val1 = getValue(((BinaryOperatorExpression *)expression)->left); }
			catch (RuntimeError error) { throw error; }
			try { val2 = getValue(((BinaryOperatorExpression *)expression)->right); }
			catch (RuntimeError error) { throw error; }
			if (!val2) { throw RuntimeError("divide zero."); }
			return val1 / val2;
		}
		case MOD: {
			qint64 val1, val2;
			try { val1 = getValue(((BinaryOperatorExpression *)expression)->left); }
			catch (RuntimeError error) { throw error; }
			try { val2 = getValue(((BinaryOperatorExpression *)expression)->right); }
			catch (RuntimeError error) { throw error; }
			if (!val2) { throw RuntimeError("mod zero."); }
			if (!val1) { return 0; }
			if ((val1 > 0 && val2 > 0) || (val1 < 0 && val2 < 0)) {
				return val1 % val2;
			} else {
				qint64 quotient = val1 / val2;
				if (val1 % val2) { --quotient; }
				return val1 - quotient * val2;
			};
		}
		case PLUS: GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION(+)
		case MINUS: GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION(-)
		case LESS: GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION(< )
		case LESS_EQUAL: GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION(<= )
		case GREATER_EQUAL: GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION(>= )
		case GREATER: GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION(> )
		case EQUAL: GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION(== )
		case NOT_EQUAL: GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION(!= )
	}
#undef GET_VALUE_OF_UNARY_OPERATOR_EXPRESSION
#undef GET_VALUE_OF_BINARY_OPERATOR_EXPRESSION
}

qint64 EvaluationState::getValue(const QString &variable) {
	if (variableMap.contains(variable)) {
		return variableMap[variable].first;
	} else { throw QString("variable \"" + variable + "\" hasn't been defined."); }
}

size_t EvaluationState::getCount(const QString &variable)
{
	if (variableMap.contains(variable)) {
		return variableMap[variable].second;
	} else return 0;
}

void EvaluationState::setValue(const QString &variable, qint64 value)
{
	if (variableMap.contains(variable)) {
		++variableMap[variable].second;
	} else { variableMap[variable].second = 0; }
	variableMap[variable].first = value;
}
