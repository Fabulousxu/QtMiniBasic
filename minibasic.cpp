#include "minibasic.h"

void MiniBasic::set_code(qsizetype line, const QString &code) {
	auto [flag, pos] = find_line(line);
	auto [err_flag, err_pos] = find_err_line(line);
	QString err_str;
	token_node *token;
	stmt_node *stmt;
	QStringList tree;
	std::function<void()> proc;

	try { stmt = gen_stmt(token = scanner(code)); }
	catch (QString err) { err_str = err; stmt = nullptr; }
	if (!err_str.isEmpty()) {
		if (!err_flag) {
			err_line.insert(err_pos, line);
			err_seq.insert(err_pos, err_str);
		} else { err_seq[err_pos] = err_str; }
		proc = [] { throw QString("exist syntax error."); };
		tree = QStringList{ "<font color = red>error:</font>&nbsp;" + err_str };
	} else {
		if (err_flag) {
			err_line.remove(err_pos);
			err_seq.remove(err_pos);
		} proc = gen_proc(stmt);
		tree = stmt ? stmt->to_strlist() : QStringList{ "" };
	}
	if (flag) {
		code_seq[pos] = to_str(token);
		tree_seq[pos] = tree;
		if (stmt_seq[pos]) { delete stmt_seq[pos]; }
		stmt_seq[pos] = stmt;
		proc_seq[pos] = proc;
	} else {
		line_seq.insert(pos, line);
		code_seq.insert(pos, to_str(token));
		tree_seq.insert(pos, tree);
		stmt_seq.insert(pos, stmt);
		proc_seq.insert(pos, proc);
	}
	if (!err_str.isEmpty()) { throw err_str; }
}

void MiniBasic::rm_code(qsizetype line) {
	auto [flag, pos] = find_line(line);
	auto [err_flag, err_pos] = find_err_line(line);
	if (!flag) { throw QString("no such line."); }
	line_seq.remove(pos);
	code_seq.remove(pos);
	tree_seq.remove(pos);
	stmt_seq.remove(pos);
	proc_seq.remove(pos);
	if (err_flag) { err_seq.remove(err_pos); }
}

void MiniBasic::load(const QString &filename) {
	QFile *file = new QFile(filename);
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) { throw QString("no such file."); }
	clear();
	QString code;
	qsizetype line;
	bool isNum;
	QTextStream stream(file);
	while (!(code = file->readLine()).isEmpty()) {
		code = code.trimmed();
		line = code.section(' ', 0, 0).toLongLong(&isNum);
		if (!isNum) { continue; }
		code = code.section(' ', 1);
		try { set_code(line, code); }
		catch (QString err) {}
	} file->close();
}

void MiniBasic::run() {
	for (auto &stmt : stmt_seq) { if (stmt) { stmt->zero_out(); } }
	if (!err_line.isEmpty()) { throw QString("exist syntax error, can't run."); }
	for (curr_pos = 0, eval.clear(); curr_pos < proc_seq.size(); ++curr_pos) {
		try { proc_seq[curr_pos](); }
		catch (QString err) {
			throw "<font color=lightgreen>line " + QString::number(line_seq[curr_pos]) + ":</font>&nbsp;" + err;
		}
	}
	curr_pos = 0;
	eval.clear();
}

void MiniBasic::clear() {
	line_seq.clear();
	code_seq.clear();
	tree_seq.clear();
	stmt_seq.clear();
	proc_seq.clear();
	err_line.clear();
	err_seq.clear();
	curr_pos = 0;
}

QStringList MiniBasic::code_strlist() {
	QStringList codes;
	if (code_seq.empty()) { return codes; }
	qsizetype max_length = QString::number(line_seq.back()).length();
	QString line;
	for (int i = 0; i < line_seq.size(); ++i) {
		line = QString::number(line_seq[i]);
		codes.append("<font color=lightgreen>" + QString(max_length - line.length(), '0') +
			line + "</font>&nbsp;" + code_seq[i]);
	}
	return codes;
}

QStringList MiniBasic::tree_strlist() {
	QStringList trees, tree;
	if (tree_seq.empty()) { return trees; }
	qsizetype max_length = QString::number(line_seq.back()).length();
	QString line;
	for (int i = 0; i < line_seq.size(); ++i) {
		line = QString::number(line_seq[i]);
		line = "<font color=lightgreen>" + QString(max_length - line.length(), '0') + line + "</font>&nbsp;";
		tree = tree_seq[i];
		tree[0].push_front(line);
		if (stmt_seq[i]) {
			switch (stmt_seq[i]->type) {
				case STMT_REM: break;
				case STMT_LET: tree[0].push_back("&nbsp;<font color=deeppink>" +
					QString::number(((let_stmt *)stmt_seq[i])->count) + "</font>"); break;
				case STMT_PRINT: tree[0].push_back("&nbsp;<font color=deeppink>" +
					QString::number(((print_stmt *)stmt_seq[i])->count) + "</font>"); break;
				case STMT_INPUT: tree[0].push_back("&nbsp;<font color=deeppink>" +
					QString::number(((input_stmt *)stmt_seq[i])->count) + "</font>"); break;
				case STMT_GOTO: tree[0].push_back("&nbsp;<font color=deeppink>" +
					QString::number(((goto_stmt *)stmt_seq[i])->count) + "</font>"); break;
				case STMT_END: tree[0].push_back("&nbsp;<font color=deeppink>" +
					QString::number(((end_stmt *)stmt_seq[i])->count) + "</font>"); break;
				case STMT_IF: tree[0].push_back("&nbsp;<font color=deeppink>" +
					QString::number(((if_stmt *)stmt_seq[i])->if_count) + "&nbsp;" +
					QString::number(((if_stmt *)stmt_seq[i])->then_count) + "</font>");
			}
		} trees.append(tree);
	}
	return trees;
}

QPair<bool, qsizetype> MiniBasic::find_line(qsizetype line) const {
	qsizetype low = 0, high = line_seq.size(), mid;
	if (high == 0 || line > line_seq[high - 1]) { return { false, high }; }
	if (line == line_seq[high - 1]) { return { true, high - 1 }; }
	--high;
	while (low < high) {
		mid = (low + high) / 2;
		if (line < line_seq[mid]) {
			high = mid;
		} else if (line > line_seq[mid]) {
			low = mid + 1;
		} else { return { true, mid }; }
	} return { false, low };
}

QPair<bool, qsizetype> MiniBasic::find_err_line(qsizetype line) const {
	qsizetype low = 0, high = err_line.size(), mid;
	if (high == 0 || line > err_line[high - 1]) { return { false, high }; }
	if (line == err_line[high - 1]) { return { true, high - 1 }; }
	--high;
	while (low < high) {
		mid = (low + high) / 2;
		if (line < err_line[mid]) {
			high = mid;
		} else if (line > err_line[mid]) {
			low = mid + 1;
		} else { return { true, mid }; }
	} return { false, low };
}

std::function<void()> MiniBasic::gen_proc(stmt_node *stmt) {
	if (!stmt) { return [] {}; }
	switch (stmt->type) {
		case STMT_REM: return [] {};
		case STMT_LET: return [this, stmt] {
			++((let_stmt *)stmt)->count;
			qint64 val;
			try { val = eval.get_val(((let_stmt *)stmt)->exp); }
			catch (QString err) { throw err; }
			eval.set_val(((let_stmt *)stmt)->var, val); };
		case STMT_PRINT: return [this, stmt] {
			++((print_stmt *)stmt)->count;
			qint64 val;
			try { val = eval.get_val(((print_stmt *)stmt)->exp); }
			catch (QString err) { throw err; }
			print(val); };
		case STMT_INPUT: return [this, stmt] {
			++((input_stmt *)stmt)->count;
			eval.set_val(((input_stmt *)stmt)->var, input()); };
		case STMT_GOTO: return [this, stmt] {
			++((goto_stmt *)stmt)->count;
			auto [flag, pos] = find_line(((goto_stmt *)stmt)->dest);
			if (!flag) { throw QString("goto invalid line."); }
			curr_pos = pos - 1; };
		case STMT_IF: return [this, stmt] {
			++((if_stmt *)stmt)->if_count;
			qint64 cond;
			try { cond = eval.get_val(((if_stmt *)stmt)->cond); }
			catch (QString err) { throw err; }
			if (!cond) { return; }
			++((if_stmt *)stmt)->then_count;
			auto [flag, pos] = find_line(((if_stmt *)stmt)->dest);
			if (!flag) { throw QString("goto invalid line."); }
			curr_pos = pos - 1; };
		case STMT_END: return [this, stmt] {
			++((end_stmt *)stmt)->count;
			curr_pos = proc_seq.size() - 1; };
	}
}
