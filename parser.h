#pragma once

#include "token.h"
#include "exception.h"
#include "statement.h"
#include "expression.h"
#include <QtCore/qlist.h>
#include <QtCore/qstack.h>

QList<Token *> scan(const QString &code);
Expression *parserExpression(const QList<Token *> &tokenList);
Statement *parser(const QList<Token *> &tokenList);
