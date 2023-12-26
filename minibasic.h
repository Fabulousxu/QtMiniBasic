#pragma once

#include "statement.h"
#include "evalstate.h"
#include <functional>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>

struct line_node {
	QString code;
	QStringList tree;
	stmt_node *stmt = nullptr;
	std::function<void()> proc;
};

class MiniBasic {
public:
	MiniBasic() :isRun(false) {}
	std::function<void(qint64)> print;//print interface defined by users
	std::function<qint64()> input;//input interface defined by users

	void set_code(size_t line_seq, const QString &code);
	void rm_code(size_t line_seq);
	void load(const QString &filename);
	void clear();

	void run();
	void kill();

	QStringList code_HTML();
	QStringList tree_HTML();

private:
	eval_state eval;
	QMap<size_t, line_node> line_seq;
	QMap<size_t, QString> err_seq;
	QMap<size_t, line_node>::iterator curr_pos;
	bool isRun;

	std::function<void()> gen_proc(stmt_node *stmt);
};
