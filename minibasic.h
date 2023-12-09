#pragma once

#include "statement.h"
#include "evalstate.h"
#include <functional>
#include <qfile.h>
#include <qtextstream.h>

class MiniBasic {
public:
	MiniBasic() :curr_pos(0) {}
	std::function<void(qint64)> print;//print interface defined by users
	std::function<qint64()> input;//input interface defined by users

	void set_code(qsizetype line, const QString &code);
	void rm_code(qsizetype line);
	void load(const QString &filename);
	void clear();

	void run();
	void kill() { eval.clear(); curr_pos = proc_seq.size(); }

	QStringList code_strlist();
	QStringList tree_strlist();

private:
	eval_state eval;
	QList<qsizetype> line_seq;
	QStringList code_seq;
	QList<QStringList> tree_seq;
	QList<stmt_node *> stmt_seq;
	QList<std::function<void()>> proc_seq;
	QList<qsizetype> err_line;
	QStringList err_seq;
	qsizetype curr_pos;

	QPair<bool, qsizetype> find_line(qsizetype line) const;
	QPair<bool, qsizetype> find_err_line(qsizetype line) const;
	std::function<void()> gen_proc(stmt_node *stmt);
};
