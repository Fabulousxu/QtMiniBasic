#pragma once

#include "tokenizer.h"
#include <qstack.h>
#include <qstringlist.h>

enum exp_t { EXP_VARIABLE, EXP_NUMBER, EXP_OPERATOR };

struct exp_node {
	exp_t type;
	exp_node(exp_t type) : type(type) {}
	virtual ~exp_node() {}
	virtual QStringList to_strlist() const = 0;
};

struct var_exp : exp_node {
	QString val;
	var_exp(const QString &val) : exp_node(EXP_VARIABLE), val(val) {}
	QStringList to_strlist() const;
};

struct num_exp : exp_node {
	qint64 val;
	num_exp(qint64 val) : exp_node(EXP_NUMBER), val(val) {}
	QStringList to_strlist() const;
};

struct op_exp : exp_node {
	op_t val;
	op_exp(op_t val) : exp_node(EXP_OPERATOR), val(val) {}
	virtual ~op_exp() {}
	virtual QStringList to_strlist() const = 0;
};

struct unary_exp : op_exp {
	exp_node *child;
	unary_exp(op_t val, exp_node *child) : op_exp(val), child(child) {}
	~unary_exp() { delete child; }
	QStringList to_strlist() const;
};

struct bin_exp : op_exp {
	exp_node *left;
	exp_node *right;
	bin_exp(op_t val, exp_node *l, exp_node *r) :op_exp(val), left(l), right(r) {}
	~bin_exp() { delete left, right; }
	QStringList to_strlist() const;
};

exp_node *gen_exp(token_node *token);
