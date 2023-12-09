#pragma once

#include "expression.h"

enum stmt_t { STMT_REM, STMT_LET, STMT_PRINT, STMT_INPUT, STMT_GOTO, STMT_IF, STMT_END };

struct stmt_node {
	stmt_t type;
	stmt_node(stmt_t type) : type(type) {}
	virtual ~stmt_node() {}
	virtual QStringList to_strlist() const = 0;
	virtual void zero_out() = 0;
};

struct rem_stmt : stmt_node {
	QString comment;
	rem_stmt(const QString &comment) : stmt_node(STMT_REM), comment(comment) {}
	QStringList to_strlist() const;
	void zero_out() {}
};

struct let_stmt : stmt_node {
	QString var;
	exp_node *exp;
	qsizetype count;
	let_stmt(const QString &var, exp_node *exp) : stmt_node(STMT_LET), var(var), exp(exp), count(0) {}
	~let_stmt() { delete exp; }
	QStringList to_strlist() const;
	void zero_out() { count = 0; }
};

struct print_stmt : stmt_node {
	exp_node *exp;
	qsizetype count;
	print_stmt(exp_node *exp) : stmt_node(STMT_PRINT), exp(exp), count(0) {}
	~print_stmt() { delete exp; }
	QStringList to_strlist() const;
	void zero_out() { count = 0; }
};

struct input_stmt : stmt_node {
	QString var;
	qsizetype count;
	input_stmt(const QString &var) : stmt_node(STMT_INPUT), var(var), count(0) {}
	QStringList to_strlist() const;
	void zero_out() { count = 0; }
};

struct goto_stmt : stmt_node {
	qsizetype dest;
	qsizetype count;
	goto_stmt(qsizetype dest) : stmt_node(STMT_GOTO), dest(dest), count(0) {}
	QStringList to_strlist() const;
	void zero_out() { count = 0; }
};

struct if_stmt : stmt_node {
	exp_node *cond;
	qsizetype dest;
	qsizetype if_count;
	qsizetype then_count;
	if_stmt(exp_node *cond, qsizetype dest) : stmt_node(STMT_IF), cond(cond), dest(dest), if_count(0), then_count(0) {}
	~if_stmt() { delete cond; }
	QStringList to_strlist() const;
	void zero_out() { if_count = then_count = 0; }
};

struct end_stmt : stmt_node {
	qsizetype count;
	end_stmt() : stmt_node(STMT_END), count(0) {}
	QStringList to_strlist() const;
	void zero_out() { count = 0; }
};

stmt_node *gen_stmt(token_node *token);
