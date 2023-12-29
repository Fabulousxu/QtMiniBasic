#pragma once

#include "parser.h"
#include "evalstate.h"
#include <functional>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>

struct Line {
	QString code;
	QStringList tree;
	Statement *statement = nullptr;
	std::function<void()> procedure;
};

class MiniBasic {
public:
	MiniBasic() : running(false) {}
	std::function<void(qint64)> print;//print interface defined by users
	std::function<qint64()> input;//input interface defined by users

	void setCode(size_t line, const QString &code);
	void removeCode(size_t line);
	void loadCode(const QString &filename);
	void clearCode();

	void run();
	void kill() { running = false; }
	bool isRunning() { return running; }

	QStringList codeHTML();
	QStringList treeHTML();

private:
	EvaluationState evaluationState;
	QMap<size_t, Line> lineList;
	QMap<size_t, QString> errorList;
	QMap<size_t, Line>::iterator currentPosition;
	bool running;

	std::function<void()> generateProcedure(Statement *statement);
};
