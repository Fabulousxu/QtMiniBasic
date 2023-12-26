#pragma once

#include "expression.h"
#include <qmap.h>
#include <qpair.h>
#include <qdebug.h>

class eval_state {
public:
	qint64 get_val(exp_node *exp);
	qint64 get_val(QString var);
	qsizetype get_num(QString var);
	void set_val(QString name, qint64 val);
	void clear() { var_map.clear(); }

private:
	QMap<QString, QPair<qint64, qsizetype>> var_map;
};
