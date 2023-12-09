#include "expression.h"

QStringList var_exp::to_strlist() const {
	return QStringList{ "<font color=white>" + val + "</font>" };
}

QStringList num_exp::to_strlist() const {
	return QStringList{ "<font color=darkorange>" + QString::number(val) + "</font>" };
}

QStringList unary_exp::to_strlist() const {
	QStringList child_strlist = child->to_strlist();
	for (QString &line : child_strlist) { line.push_front("&nbsp;&nbsp;&nbsp;&nbsp;"); }
	return QStringList{ "<font color=gold>" + op_str[val] + "</font>" } + child_strlist;
}

QStringList bin_exp::to_strlist() const {
	QStringList left_strlist = left->to_strlist();
	QStringList right_strlist = right->to_strlist();
	for (QString &line : left_strlist) { line.push_front("&nbsp;&nbsp;&nbsp;&nbsp;"); }
	for (QString &line : right_strlist) { line.push_front("&nbsp;&nbsp;&nbsp;&nbsp;"); }
	return QStringList{ "<font color=gold>" + op_str[val] + "</font>" } + left_strlist + right_strlist;
}

exp_node *gen_exp(token_node *token) {
	QStack<op_t> op_stack;
	QStack<exp_node *> exp_stack;
#define GEN_EXP(op) \
	while (!op_stack.empty() && op_stack.top() < op) { \
		if (op_stack.top() == POS || op_stack.top() == NEG) { \
			if (exp_stack.empty()) { \
				throw "missing operand of operator \"" + op_str[op_stack.top()] + "\"."; \
			} else { exp_stack.push(new unary_exp(op_stack.pop(), exp_stack.pop())); } \
		} else { \
			if (exp_stack.size() < 2) { \
				throw "missing operand of operator \"" + op_str[op_stack.top()] + "\"."; \
			} exp_node *right = exp_stack.pop(); \
			exp_stack.push(new bin_exp(op_stack.pop(), exp_stack.pop(), right)); \
		} \
	}

	for (; token; token = token->next) {
		if (token->type == TK_IDENTIFIER) {
			exp_stack.push(new var_exp(((id_token *)token)->val));
		} else if (token->type == TK_CONSTANT) {
			exp_stack.push(new num_exp(((const_token *)token)->val));
		} else if (token->type == TK_OPERATOR) {
			switch (((op_token *)token)->val) {
				case MUL: case DIV: case MOD: GEN_EXP(ADD); break;
				case ADD: case SUB: GEN_EXP(LT); break;
				case LT: case LE: case GE: case GT: GEN_EXP(EQ); break;
				case EQ: case NE: GEN_EXP(LPAREN); break;
			}
			if (((op_token *)token)->val == RPAREN) {
				GEN_EXP(LPAREN);
				if (op_stack.empty()) { throw QString("missing left paren."); }
				op_stack.pop();
			} else { op_stack.push(((op_token *)token)->val); }
		} else { throw QString("excess statement after expression."); }
	} GEN_EXP(LPAREN);
	if (!op_stack.empty()) { throw QString("missing right paren."); }
	if (exp_stack.empty()) { throw QString("empty expression."); }
	if (exp_stack.size() > 1) { throw QString("illegal expression."); }
	return exp_stack.top();
#undef GEN_EXP
}
