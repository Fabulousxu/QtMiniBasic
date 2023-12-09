#include "evalstate.h"

qint64 eval_state::get_val(exp_node *exp) {
	if (exp->type == EXP_NUMBER) { return ((num_exp *)exp)->val; }
	if (exp->type == EXP_VARIABLE) {
		if (var_map.contains(((var_exp *)exp)->val)) {
			return var_map[((var_exp *)exp)->val];
		} else { throw "variable \"" + ((var_exp *)exp)->val + "\" is being used without initialized."; }
	}

#define UNARY_OP(op) { \
	qint64 val; \
	try { val = get_val(((unary_exp *)exp)->child); } \
	catch (QString err) { throw err; } \
	return op val; }

#define BIN_OP(op) { \
	qint64 val1, val2; \
	try { val1 = get_val(((bin_exp *)exp)->left); } \
	catch (QString err) { throw err; } \
	try { val2 = get_val(((bin_exp *)exp)->right); } \
	catch (QString err) { throw err; } \
	return val1 op val2; }

#define NOT_ZERO_OP(op) { \
	qint64 val1, val2; \
	try { val1 = get_val(((bin_exp *)exp)->left); } \
	catch (QString err) { throw err; } \
	try { val2 = get_val(((bin_exp *)exp)->right); } \
	catch (QString err) { throw err; } \
	if (!val2) { throw QString("divide or mod zero."); } \
	return val1 op val2; }

	switch (((op_exp *)exp)->val) {
		case POW: {
			qint64 val1, val2;
			try { val1 = get_val(((bin_exp *)exp)->left); }
			catch (QString err) { throw err; }
			try { val2 = get_val(((bin_exp *)exp)->right); }
			catch (QString err) { throw err; }
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

qint64 eval_state::get_val(QString var_name) {
	if (var_map.contains(var_name)) {
		return var_map[var_name];
	} else { throw "variable \"" + var_name + "\" hasn't been defined."; }
}
