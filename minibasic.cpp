#include "minibasic.h"

void MiniBasic::set_code(size_t line, const QString &code) {
	token_node *token = nullptr;
	stmt_node *stmt = nullptr;
	QString err_str;
	std::function<void()> proc;
	QStringList tree;

	try { token = scan(code); }
	catch (QPair<token_node *, QString> err_msg) {
		token = err_msg.first;
		err_str = err_msg.second;
	}
	if (err_str.isEmpty()) {
		try { stmt = gen_stmt(token); }
		catch (QString err_msg) { err_str = err_msg; }
	}
	if (!err_str.isEmpty()) {
		err_seq[line] = err_str;
		proc = [] { throw QString("exist syntax error."); };
		tree = QStringList{ "<font color = red>error:</font>&nbsp;" + err_str };
	} else {
		err_seq.remove(line);
		proc = gen_proc(stmt);
		tree = stmt ? stmt->to_HTML() : QStringList{ "" };
	}
	auto iter = line_seq.find(line);
	if (iter != line_seq.end()) { 
		delete iter->stmt; 
		iter.value() = { to_HTML(token), tree, stmt, proc };
	} else { line_seq[line] = { to_HTML(token),tree,stmt,proc }; }
	delete token;
	if (!err_str.isEmpty()) { throw err_str; }
}

void MiniBasic::rm_code(size_t line) {
	auto iter = line_seq.find(line);
	if (iter == line_seq.end()) { throw QString("no such line."); }
	delete iter->stmt;
	line_seq.erase(iter);
	err_seq.remove(line);
}

void MiniBasic::load(const QString &filename) {
	QFile *file = new QFile(filename);
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) { throw QString("no such file."); }
	QString code;
	qsizetype line_seq;
	bool isNum;
	QTextStream stream(file);
	while (!(code = file->readLine()).isEmpty()) {
		code = code.trimmed();
		line_seq = code.section(' ', 0, 0).toLongLong(&isNum);
		if (!isNum) { continue; }
		code = code.section(' ', 1);
		try { set_code(line_seq, code); }
		catch (QString err_seq) {}
	} file->close();
}

void MiniBasic::run() {
	if (!err_seq.isEmpty()) { throw QString("exist syntax error, can't run."); }
	for (auto &line : line_seq) { if (line.stmt) { line.stmt->zero_out(); } }
	kill();
	isRun = true;
	eval.clear();
	curr_pos = line_seq.begin();
	while (isRun && curr_pos != line_seq.end()) {
		try { curr_pos->proc(); }
		catch (QString err) {
			isRun = false;
			throw "<font color=lightgreen>line " + QString::number(curr_pos.key()) + ":</font>&nbsp;" + err;
		}
	}
}

void MiniBasic::kill()
{
	isRun = false;
}

void MiniBasic::clear() {
	for (auto &line : line_seq) { delete line.stmt; }
	line_seq.clear();
	err_seq.clear();
	curr_pos = line_seq.begin();
}

QStringList MiniBasic::code_HTML() {
	QStringList codes;
	if (line_seq.empty()) { return codes; }
	qsizetype max_length = QString::number(line_seq.lastKey()).length();
	QString line;
	for (auto iter = line_seq.begin(); iter != line_seq.end(); ++iter) {
		line = QString::number(iter.key());
		line = "<font color=lightgreen>" + QString(max_length - line.length(), '0') + line + "</font>&nbsp;";
		codes.append(line + iter->code);
	}
	return codes;
}

QStringList MiniBasic::tree_HTML() {
	QStringList trees, tree;
	if (line_seq.empty()) { return trees; }
	qsizetype max_length = QString::number(line_seq.lastKey()).length();
	QString line;
	for (auto iter = line_seq.begin(); iter != line_seq.end(); ++iter) {
		line = QString::number(iter.key());
		line = "<font color=lightgreen>" + QString(max_length - line.length(), '0') + line + "</font>&nbsp;";
		tree = iter->tree;
		tree[0].push_front(line);
		if (iter->stmt) {
			switch (iter->stmt->type) {
				case STMT_REM: break;
				case STMT_LET:
					tree[0].push_back("&nbsp;<font color=deeppink>" +
						QString::number(((let_stmt *)iter->stmt)->count) + "</font>");
					tree[1].push_back("&nbsp;<font color=purple>" +
						QString::number(eval.get_num(((let_stmt *)iter->stmt)->var)) + "</font>");
					break;
				case STMT_PRINT: tree[0].push_back("&nbsp;<font color=deeppink>" +
					QString::number(((print_stmt *)iter->stmt)->count) + "</font>"); break;
				case STMT_INPUT:
					tree[0].push_back("&nbsp;<font color=deeppink>" +
						QString::number(((input_stmt *)iter->stmt)->count) + "</font>");
					tree[1].push_back("&nbsp;<font color=deeppink>" +
						QString::number(eval.get_num(((input_stmt *)iter->stmt)->var)) + "</font>");
					break;
				case STMT_GOTO: tree[0].push_back("&nbsp;<font color=deeppink>" +
					QString::number(((goto_stmt *)iter->stmt)->count) + "</font>"); break;
				case STMT_END: tree[0].push_back("&nbsp;<font color=deeppink>" +
					QString::number(((end_stmt *)iter->stmt)->count) + "</font>"); break;
				case STMT_IF: tree[0].push_back("&nbsp;<font color=deeppink>" +
					QString::number(((if_stmt *)iter->stmt)->if_count) + "&nbsp;" +
					QString::number(((if_stmt *)iter->stmt)->then_count) + "</font>");
			}
		}
		trees.append(tree);
	}
	return trees;
}

std::function<void()> MiniBasic::gen_proc(stmt_node *stmt) {
	if (!stmt) { return [this] { ++curr_pos; }; }
	switch (stmt->type) {
		case STMT_REM: return [this] { ++curr_pos; };
		case STMT_LET: return [this, stmt] {
			++((let_stmt *)stmt)->count;
			qint64 val;
			try { val = eval.get_val(((let_stmt *)stmt)->exp); }
			catch (QString err_seq) { throw err_seq; }
			eval.set_val(((let_stmt *)stmt)->var, val);
			++curr_pos; };
		case STMT_PRINT: return [this, stmt] {
			++((print_stmt *)stmt)->count;
			qint64 val;
			try { val = eval.get_val(((print_stmt *)stmt)->exp); }
			catch (QString err_seq) { throw err_seq; }
			print(val);
			++curr_pos; };
		case STMT_INPUT: return [this, stmt] {
			++((input_stmt *)stmt)->count;
			eval.set_val(((input_stmt *)stmt)->var, input());
			++curr_pos; };
		case STMT_GOTO: return [this, stmt] {
			++((goto_stmt *)stmt)->count;
			curr_pos = line_seq.find(((goto_stmt *)stmt)->dest);
			if (curr_pos == line_seq.end()) { throw QString("goto invalid line."); } };
		case STMT_IF: return [this, stmt] {
			++((if_stmt *)stmt)->if_count;
			qint64 cond;
			try { cond = eval.get_val(((if_stmt *)stmt)->cond); }
			catch (QString err_seq) { throw err_seq; }
			if (!cond) { ++curr_pos; return; }
			++((if_stmt *)stmt)->then_count;
			curr_pos = line_seq.find(((if_stmt *)stmt)->dest);
			if (curr_pos == line_seq.end()) { throw QString("goto invalid line."); } };
		case STMT_END: return [this, stmt] {
			++((end_stmt *)stmt)->count;
			curr_pos = line_seq.end(); };
	}
}
