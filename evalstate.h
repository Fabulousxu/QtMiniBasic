#pragma once

#include "expression.h"
#include "exception.h"
#include <qmap.h>
#include <qpair.h>

class EvaluationState {
public:
	qint64 getValue(Expression *expression);
	qint64 getValue(const QString &variable);
	size_t getCount(const QString &variable);
	void setValue(const QString &name, qint64 value);
	void clear() { variableMap.clear(); }

private:
	QMap<QString, QPair<qint64, size_t>> variableMap;
};
