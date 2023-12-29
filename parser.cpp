#include "parser.h"

QList<Token *> scan(const QString &code) {
    QList<Token *> tokenList;
    for (auto it = code.cbegin(); it != code.cend(); ++it) {
        if (it->isSpace()) { continue; }
        if (!tokenList.isEmpty() && tokenList.back()->type == TOKEN_KEYWORD &&
            ((KeywordToken *)tokenList.back())->value == KEYWORD_REM) {
            tokenList.append(new StringToken(code.mid(it - code.cbegin()).trimmed()));
            return tokenList;
        } else if (it->isLetter() || *it == '_') {
            QString value{ *it };
            while (it + 1 != code.cend() && ((it + 1)->isLetterOrNumber() || *(it + 1) == '_')) {
                value.append(*(++it));
            }
            if (value == keywordString[KEYWORD_REM]) {
                tokenList.append(new KeywordToken(KEYWORD_REM));
            } else if (value == keywordString[KEYWORD_LET]) {
                tokenList.append(new KeywordToken(KEYWORD_LET));
            } else if (value == keywordString[KEYWORD_PRINT]) {
                tokenList.append(new KeywordToken(KEYWORD_PRINT));
            } else if (value == keywordString[KEYWORD_INPUT]) {
                tokenList.append(new KeywordToken(KEYWORD_INPUT));
            } else if (value == keywordString[KEYWORD_GOTO]) {
                tokenList.append(new KeywordToken(KEYWORD_GOTO));
            } else if (value == keywordString[KEYWORD_IF]) {
                tokenList.append(new KeywordToken(KEYWORD_IF));
            } else if (value == keywordString[KEYWORD_THEN]) {
                tokenList.append(new KeywordToken(KEYWORD_THEN));
            } else if (value == keywordString[KEYWORD_END]) {
                tokenList.append(new KeywordToken(KEYWORD_END));
            } else if (value == keywordString[KEYWORD_MOD]) {
                tokenList.append(new OperatorToken(MOD));
            } else { tokenList.append(new IdentifierToken(value)); }
        } else if (it->isDigit()) {
            qint64 value = it->digitValue();
            while (it + 1 != code.cend() && (it + 1)->isDigit()) {
                value = value * 10 + (++it)->digitValue();
            }
            tokenList.append(new ConstantToken(value));
        } else if (*it == '+') {
            if (!tokenList.isEmpty() && (
                tokenList.back()->type == TOKEN_CONSTANT || 
                tokenList.back()->type == TOKEN_IDENTIFIER ||(
                    tokenList.back()->type == TOKEN_OPERATOR && 
                    ((OperatorToken *)tokenList.back())->value == RIGHT_PAREN))) {
                tokenList.append(new OperatorToken(PLUS));
            } else { tokenList.append(new OperatorToken(POSITIVE)); }
        } else if (*it == '-') {
            if (!tokenList.isEmpty() && (
                tokenList.back()->type == TOKEN_CONSTANT ||
                tokenList.back()->type == TOKEN_IDENTIFIER || (
                    tokenList.back()->type == TOKEN_OPERATOR &&
                    ((OperatorToken *)tokenList.back())->value == RIGHT_PAREN))) {
                tokenList.append(new OperatorToken(MINUS));
            } else { tokenList.append(new OperatorToken(NEGATIVE)); }
        } else if (*it == '*') {
            if (it + 1 != code.cend() && *(it + 1) == '*') {
                tokenList.append(new OperatorToken(POWER));
                ++it;
            } else { tokenList.append(new OperatorToken(TIMES)); }
        } else if (*it == '/') {
            tokenList.append(new OperatorToken(OVER));
        } else if (*it == '<') {
            if (it + 1 != code.cend() && *(it + 1) == '=') {
                tokenList.append(new OperatorToken(LESS_EQUAL));
                ++it;
            } else if (it + 1 != code.cend() && *(it + 1) == '>') {
                tokenList.append(new OperatorToken(NOT_EQUAL));
                ++it;
            } else { tokenList.append(new OperatorToken(LESS)); }
        } else if (*it == '>') {
            if (it + 1 != code.cend() && *(it + 1) == '=') {
                tokenList.append(new OperatorToken(GREATER_EQUAL));
                ++it;
            } else { tokenList.append(new OperatorToken(GREATER)); }
        } else if (*it == '=') {
            tokenList.append(new OperatorToken(EQUAL));
        } else if (*it == '(') {
            tokenList.append(new OperatorToken(LEFT_PAREN));
        } else if (*it == ')') {
            tokenList.append(new OperatorToken(RIGHT_PAREN));
        } else {
            tokenList.append(new StringToken(code.mid(it - code.cbegin()).trimmed()));
            throw QPair<QList<Token *>, ScanError>(tokenList, "illegal character.");
        }
    }
    return tokenList;
}

Expression *parserExpression(const QList<Token *> &tokenList) {
    QStack<OperatorType> operatorStack;
    QStack<Expression *> expressionStack;
#define MAKE_EXPRESSION(op) \
	while (!operatorStack.empty() && operatorStack.top() < op) { \
		if (operatorStack.top() == POSITIVE || operatorStack.top() == NEGATIVE) { \
			if (expressionStack.empty()) { \
				throw ParserError("missing operand of operator \"" + operatorString[operatorStack.top()] + "\"."); \
			} \
            expressionStack.push(new UnaryOperatorExpression(operatorStack.pop(), expressionStack.pop())); \
		} else { \
			if (expressionStack.size() < 2) { \
				throw ParserError("missing operand of operator \"" + operatorString[operatorStack.top()] + "\"."); \
			} \
            Expression *right = expressionStack.pop(); \
			expressionStack.push(new BinaryOperatorExpression(operatorStack.pop(), expressionStack.pop(), right)); \
		} \
	}

    for (auto it = tokenList.cbegin(); it != tokenList.cend(); ++it) {
        switch ((*it)->type) {
            case TOKEN_IDENTIFIER:
                expressionStack.push(new VariableExpression(((IdentifierToken *)*it)->value));
                break;
            case TOKEN_CONSTANT:
                expressionStack.push(new NumberExpression(((ConstantToken *)*it)->value));
                break;
            case TOKEN_OPERATOR:
                switch (((OperatorToken *)*it)->value) {
                    case TIMES:
                    case OVER:
                    case MOD:
                        MAKE_EXPRESSION(PLUS);
                        break;
                    case PLUS:
                    case MINUS:
                        MAKE_EXPRESSION(LESS);
                        break;
                    case LESS:
                    case LESS_EQUAL:
                    case GREATER_EQUAL:
                    case GREATER:
                        MAKE_EXPRESSION(EQUAL);
                        break;
                    case EQUAL:
                    case NOT_EQUAL:
                        MAKE_EXPRESSION(LEFT_PAREN);
                        break;
                    case RIGHT_PAREN:
                        MAKE_EXPRESSION(LEFT_PAREN);
                }
                if (((OperatorToken *)*it)->value == RIGHT_PAREN) {
                    if (operatorStack.empty()) { throw ParserError("missing left paren."); }
                    operatorStack.pop();
                } else { operatorStack.push(((OperatorToken *)*it)->value); }
                break;
            default:
                throw ParserError("excess statement after expression.");
        }
    }
    MAKE_EXPRESSION(LEFT_PAREN);
    if (!operatorStack.empty()) { throw ParserError("missing right paren."); }
    if (expressionStack.empty()) { throw ParserError("empty expression."); }
    if (expressionStack.size() > 1) { throw ParserError("illegal expression."); }
    return expressionStack.top();
#undef MAKE_EXPRESSION
}

Statement *parser(const QList<Token *> &tokenList) {
    if (tokenList.isEmpty()) { return nullptr; }
    auto it = tokenList.cbegin();
    if ((*it)->type != TOKEN_KEYWORD) { throw ParserError("illegal statement."); }
    switch (((KeywordToken *)*it)->value) {
        case KEYWORD_REM:
            return new RemStatement(++it == tokenList.cend() ? "" : ((StringToken *)*it)->value);
        case KEYWORD_LET: {
            if (++it == tokenList.cend()) { throw ParserError("missing variable name and assignment."); }
            if ((*it)->type != TOKEN_IDENTIFIER) { throw ParserError("illegal variable name."); }
            QString variable = ((IdentifierToken *)*it)->value;
            if (++it == tokenList.cend() ||
                (*it)->type != TOKEN_OPERATOR ||
                ((OperatorToken *)*it)->value != EQUAL) {
                throw ParserError("missing assignment operator after variable name.");
            }
            Expression *expression;
            try { expression = parserExpression(tokenList.mid(++it - tokenList.cbegin())); }
            catch (ParserError error) { throw error; }
            return new LetStatement(variable, expression);
        }
        case KEYWORD_PRINT: {
            Expression *expression;
            try { expression = parserExpression(tokenList.mid(++it - tokenList.cbegin())); }
            catch (ParserError error) { throw error; }
            return new PrintStatement(expression);
        }
        case KEYWORD_INPUT: {
            if (++it == tokenList.cend()) { throw ParserError("missing variable name."); }
            if ((*it)->type != TOKEN_IDENTIFIER) { throw ParserError("illegal variable name."); }
            if (it + 1 != tokenList.cend()) { throw ParserError("excess statement after variable name."); }
            return new InputStatement(((IdentifierToken *)*it)->value);
        }
        case KEYWORD_GOTO: {
            if (++it == tokenList.cend()) { throw ParserError("missing destination."); }
            if ((*it)->type != TOKEN_CONSTANT) { throw ParserError("destination must be constant."); };
            if (it + 1 != tokenList.cend()) { throw ParserError("excess statement after destination."); }
            return new GotoStatement(((ConstantToken *)*it)->value);
        }
        case KEYWORD_IF: {
            decltype(it) it0 = ++it;
            while (++it != tokenList.cend() && (
                (*it)->type != TOKEN_KEYWORD ||
                ((KeywordToken *)*it)->value != KEYWORD_THEN));
            if (it == tokenList.cend()) { throw ParserError("missing THEN in IF statement."); }
            Expression *condition;
            try { condition = parserExpression(tokenList.mid(it0 - tokenList.cbegin(), it - it0)); }
            catch (ParserError error) { throw error; }
            if (++it == tokenList.cend()) { throw ParserError("missing destination."); }
            if ((*it)->type != TOKEN_CONSTANT) { throw ParserError("destination must be constant."); }
            if (it + 1 != tokenList.cend()) { throw ParserError("excess statement after destination."); }
            return new IfStatement(condition, ((ConstantToken *)*it)->value);
        }
        case KEYWORD_END:
            if (it + 1 != tokenList.cend()) { throw ParserError("excess statement after END"); }
            return new EndStatement();
    }
}
