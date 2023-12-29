#include "minibasic.h"

void MiniBasic::setCode(size_t line, const QString &code) {
	QList<Token *> tokenList;
	Statement *statement = nullptr;
	QString error;
	std::function<void()> procedure;
	QStringList tree;

	try { statement = parser(tokenList = scan(code)); }
	catch (QPair<QList<Token *>, ScanError> scanError) {
		tokenList = scanError.first;
		error = scanError.second.what();
	}
	catch (ParserError parserError) { error = parserError.what(); }
	if (!error.isEmpty()) {
		errorList[line] = error;
		procedure = [] { throw RuntimeError("exist syntax error."); };
		tree = QStringList{ makeFont(errorColor, "error:") + "&nbsp;" + error };
	} else {
		errorList.remove(line);
		procedure = generateProcedure(statement);
		tree = statement ? statement->toHTML() : QStringList{ "" };
	}
	auto iter = lineList.find(line);
	if (iter != lineList.end()) {
		delete iter->statement; 
		iter.value() = { toHTML(tokenList), tree, statement, procedure };
	} else { lineList[line] = { toHTML(tokenList),tree,statement,procedure }; }
	clearTokenList(tokenList);
	if (!error.isEmpty()) { throw error; }
}

void MiniBasic::removeCode(size_t line) {
	auto iter = lineList.find(line);
	if (iter == lineList.end()) { throw QString("no such line."); }
	delete iter->statement;
	lineList.erase(iter);
	errorList.remove(line);
}

void MiniBasic::loadCode(const QString &filename) {
	QFile *file = new QFile(filename);
	if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) { throw QString("no such file."); }
	QString code;
	size_t lineList;
	bool isNum;
	QTextStream stream(file);
	while (!(code = file->readLine()).isEmpty()) {
		code = code.trimmed();
		lineList = code.section(' ', 0, 0).toLongLong(&isNum);
		if (!isNum) { continue; }
		code = code.section(' ', 1);
		try { setCode(lineList, code); }
		catch (QString error) {}
	} 
	file->close();
}

void MiniBasic::run() {
	if (!errorList.isEmpty()) { throw QString("exist syntax error, can't run."); }
	for (auto &line : lineList) { if (line.statement) { line.statement->zeroOut(); } }
	running = true;
	evaluationState.clear();
	currentPosition = lineList.begin();
	while (running && currentPosition != lineList.end()) {
		try { currentPosition->procedure(); }
		catch (RuntimeError error) {
			running = false;
			throw makeFont(lineNumberColor, "line&nbsp;" + QString::number(currentPosition.key()) + ":&nbsp;")
				+ error.what();
		}
	}
	kill();
}

void MiniBasic::clearCode() {
	for (auto &line : lineList) { delete line.statement; }
	lineList.clear();
	errorList.clear();
	currentPosition = lineList.begin();
	running = false;
}

QStringList MiniBasic::codeHTML() {
	QStringList code;
	if (lineList.empty()) { return code; }
	size_t maxLength = QString::number(lineList.lastKey()).length();
	QString line;
	for (auto it = lineList.begin(); it != lineList.end(); ++it) {
		line = QString::number(it.key());
		line = makeFont(lineNumberColor, QString(maxLength - line.length(), '0') + line) + "&nbsp;";
		code.append(line + it->code);
	}
	return code;
}

QStringList MiniBasic::treeHTML() {
	QStringList trees, tree;
	if (lineList.empty()) { return trees; }
	size_t maxLength = QString::number(lineList.lastKey()).length();
	QString line;
	for (auto it = lineList.begin(); it != lineList.end(); ++it) {
		line = QString::number(it.key());
		line = makeFont(lineNumberColor, QString(maxLength - line.length(), '0') + line) + "&nbsp;";
		tree = it->tree;
		tree[0].push_front(line);
		if (it->statement) {
			switch (it->statement->type) {
				case STATEMENT_REM: 
					break;
				case STATEMENT_LET:
					tree[0].push_back("&nbsp;" + makeFont(treeColor[TREE_STATEMENT_COUNT],
						QString::number(((LetStatement *)it->statement)->count))
					);
					tree[1].push_back("&nbsp;" + makeFont(treeColor[TREE_VARIABLE_COUNT],
						QString::number(evaluationState.getCount(((LetStatement *)it->statement)->variable)))
					);
					break;
				case STATEMENT_PRINT: 
					tree[0].push_back("&nbsp;" + makeFont(treeColor[TREE_STATEMENT_COUNT],
						QString::number(((PrintStatement *)it->statement)->count))
					);
					break;
				case STATEMENT_INPUT:
					tree[0].push_back("&nbsp;" + makeFont(treeColor[TREE_STATEMENT_COUNT],
						QString::number(((InputStatement *)it->statement)->count))
					);
					tree[1].push_back("&nbsp;" + makeFont(treeColor[TREE_VARIABLE_COUNT],
						QString::number(evaluationState.getCount(((InputStatement *)it->statement)->variable)))
					);
					break;
				case STATEMENT_GOTO: 
					tree[0].push_back("&nbsp;" + makeFont(treeColor[TREE_STATEMENT_COUNT],
						QString::number(((GotoStatement *)it->statement)->count) + "</font>")
					);
					break;
				case STATEMENT_IF: 
					tree[0].push_back("&nbsp;" + makeFont(treeColor[TREE_STATEMENT_COUNT],
						QString::number(((IfStatement *)it->statement)->ifCount) + "&nbsp;" +
						QString::number(((IfStatement *)it->statement)->thenCount))
					);
					break;
				case STATEMENT_END:
					tree[0].push_back("&nbsp;" + makeFont(treeColor[TREE_STATEMENT_COUNT],
						QString::number(((EndStatement *)it->statement)->count) + "</font>")
					);
					break;
			}
		}
		trees.append(tree);
	}
	return trees;
}

std::function<void()> MiniBasic::generateProcedure(Statement *statement) {
	if (!statement) { return [this] { ++currentPosition; }; }
	switch (statement->type) {
		case STATEMENT_REM: 
			return [this] { ++currentPosition; };
		case STATEMENT_LET: 
			return [this, statement] {
				++((LetStatement *)statement)->count;
				qint64 value;
				try { value = evaluationState.getValue(((LetStatement *)statement)->expression); }
				catch (QString errorList) { throw errorList; }
				evaluationState.setValue(((LetStatement *)statement)->variable, value);
				++currentPosition;
				};
		case STATEMENT_PRINT: 
			return [this, statement] {
				++((PrintStatement *)statement)->count;
				qint64 value;
				try { value = evaluationState.getValue(((PrintStatement *)statement)->expression); }
				catch (QString errorList) { throw errorList; }
				print(value);
				++currentPosition;
				};
		case STATEMENT_INPUT: 
			return [this, statement] {
				++((InputStatement *)statement)->count;
				evaluationState.setValue(((InputStatement *)statement)->variable, input());
				++currentPosition;
				};
		case STATEMENT_GOTO: 
			return [this, statement] {
				++((GotoStatement *)statement)->count;
				currentPosition = lineList.find(((GotoStatement *)statement)->destination);
				if (currentPosition == lineList.end()) { throw QString("goto invalid line."); }
				};
		case STATEMENT_IF: 
			return [this, statement] {
				++((IfStatement *)statement)->ifCount;
				qint64 condition;
				try { condition = evaluationState.getValue(((IfStatement *)statement)->condition); }
				catch (QString errorList) { throw errorList; }
				if (!condition) { ++currentPosition; return; }
				++((IfStatement *)statement)->thenCount;
				currentPosition = lineList.find(((IfStatement *)statement)->destination);
				if (currentPosition == lineList.end()) { throw QString("goto invalid line."); }
				};
		case STATEMENT_END: 
			return [this, statement] {
				++((EndStatement *)statement)->count;
				currentPosition = lineList.end();
				};
	}
}
