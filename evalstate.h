#pragma once

#include "expression.h"
#include <qmap.h>

class eval_state {
public:
	qint64 get_val(exp_node *exp);
	qint64 get_val(QString var);
	void set_val(QString name, qint64 val) { var_map[name] = val; }
	void clear() { var_map.clear(); }

private:
	QMap<QString, qint64> var_map;
};
