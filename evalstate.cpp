#include "evalstate.h"

qint64 eval_state::get_val(exp_node *exp) {
	if (exp->type == EXP_NUMBER) { return ((num_exp *)exp)->val; }
	if (exp->type == EXP_VARIABLE) {
		if (var_map.contains(((var_exp *)exp)->val)) {
			++var_map[((var_exp *)exp)->val].second;
			return var_map[((var_exp *)exp)->val].first;
		} else { throw "variable \"" + ((var_exp *)exp)->val + "\" is being used without initialized."; }
	}

#define UNARY_OP(op) { \
	qint64 val; \
	try { val = get_val(((unary_exp *)exp)->child); } \
	catch (QString err_seq) { throw err_seq; } \
	return op val; }

#define BIN_OP(op) { \
	qint64 val1, val2; \
	try { val1 = get_val(((bin_exp *)exp)->left); } \
	catch (QString err_seq) { throw err_seq; } \
	try { val2 = get_val(((bin_exp *)exp)->right); } \
	catch (QString err_seq) { throw err_seq; } \
	return val1 op val2; }

#define NOT_ZERO_OP(op) { \
	qint64 val1, val2; \
	try { val1 = get_val(((bin_exp *)exp)->left); } \
	catch (QString err_seq) { throw err_seq; } \
	try { val2 = get_val(((bin_exp *)exp)->right); } \
	catch (QString err_seq) { throw err_seq; } \
	if (!val2) { throw QString("divide or mod zero."); } \
	return val1 op val2; }

	switch (((op_exp *)exp)->val) {
		case POW: {
			qint64 val1, val2;
			try { val1 = get_val(((bin_exp *)exp)->left); }
			catch (QString err_seq) { throw err_seq; }
			try { val2 = get_val(((bin_exp *)exp)->right); }
			catch (QString err_seq) { throw err_seq; }
			if (!val1 && !val2) { throw QString("0 power 0."); }
			return pow(val1, val2); }
		case POS: UNARY_OP(+)
		case NEG: UNARY_OP(-)
		case MUL: BIN_OP(*)
		case DIV: NOT_ZERO_OP(/ )
		case MOD: NOT_ZERO_OP(%)
		case ADD: BIN_OP(+)
		case SUB: BIN_OP(-)
		case LT: BIN_OP(< )
		case LE: BIN_OP(<= )
		case GE: BIN_OP(>= )
		case GT: BIN_OP(> )
		case EQ: BIN_OP(== )
		case NE: BIN_OP(!= )
	}
#undef UNARY_OP
#undef BIN_OP
#undef NOT_ZERO_OP
}

qint64 eval_state::get_val(QString var) {
	if (var_map.contains(var)) {
		return var_map[var].first;
	} else { throw "variable \"" + var + "\" hasn't been defined."; }
}

qsizetype eval_state::get_num(QString var)
{
	if (var_map.contains(var)) {
		return var_map[var].second;
	} else return 0;
}

void eval_state::set_val(QString name, qint64 val)
{
	if (var_map.contains(name)) {
		++var_map[name].second;
	} else { var_map[name].second = 0; }
	var_map[name].first = val;
}
