#include "statement.h"

QStringList rem_stmt::to_HTML() const {
    return { "<font color=skyblue>REM</font>", "&nbsp;&nbsp;&nbsp;&nbsp;<font color=grey>" + comment + "</font>" };
}

QStringList let_stmt::to_HTML() const {
    QStringList exp_HTML = exp->to_HTML();
    for (QString &line_seq : exp_HTML) { line_seq.push_front("&nbsp;&nbsp;&nbsp;&nbsp;"); }
    return QStringList{ "<font color=skyblue>LET</font>&nbsp;<font color=grey>=</font>",
        "&nbsp;&nbsp;&nbsp;&nbsp;<font color=yellow>" + var + "</font>" } + exp_HTML;
}

QStringList print_stmt::to_HTML() const {
    QStringList exp_HTML = exp->to_HTML();
    for (QString &line_seq : exp_HTML) { line_seq.push_front("&nbsp;&nbsp;&nbsp;&nbsp;"); }
    return QStringList{ "<font color=skyblue>PRINT</font>" } + exp_HTML;
}

QStringList input_stmt::to_HTML() const {
    return { "<font color=skyblue>INPUT</font>", "&nbsp;&nbsp;&nbsp;&nbsp;<font color=white>" + var + "</font>" };
}

QStringList goto_stmt::to_HTML() const {
    return { "<font color=skyblue>GOTO</font>",
        "&nbsp;&nbsp;&nbsp;&nbsp;<font color=green>" + QString::number(dest) + "</font>" };
}

QStringList if_stmt::to_HTML() const {
    QStringList cond_HTML = cond->to_HTML();
    for (QString &line_seq : cond_HTML) { line_seq.push_front("&nbsp;&nbsp;&nbsp;&nbsp;"); }
    return QStringList{ "<font color=skyblue>IF&nbsp;THEN</font>" } + cond_HTML +
        QStringList{ "&nbsp;&nbsp;&nbsp;&nbsp;<font color=green>" + QString::number(dest) + "</font>" };
}

QStringList end_stmt::to_HTML() const {
    return { "<font color=skyblue>END</font>" };
}

stmt_node *gen_stmt(token_node *token) {
    if (!token) { return nullptr; }
    if (token->type == TK_KEYWORD) {
        switch (((kw_token *)token)->val) {
            case KW_REM: return new rem_stmt(token->next ? ((str_token *)token->next)->val : "");
            case KW_LET: {
                token = token->next;
                if (!token || token->type != TK_IDENTIFIER) { throw QString("missing variable name."); }
                QString var = ((id_token *)token)->val;
                token = token->next;
                if (!token || token->type != TK_OPERATOR || ((op_token *)token)->val != EQ) {
                    throw QString("missing assignment operator after variable name.");
                } exp_node *exp;
                try { exp = gen_exp(token->next); }
                catch (QString err_seq) { throw err_seq; }
                return new let_stmt(var, exp); }
            case KW_PRINT: {
                exp_node *exp;
                try { exp = gen_exp(token->next); }
                catch (QString err_seq) { throw err_seq; }
                return new print_stmt(exp); }
            case KW_INPUT: {
                token = token->next;
                if (!token || token->type != TK_IDENTIFIER) { throw QString("missing variable name."); }
                if (token->next) { throw QString("excess statement after variable name."); }
                return new input_stmt(((id_token *)token)->val); }
            case KW_GOTO: {
                token = token->next;
                if (!token || token->type != TK_CONSTANT) { throw QString("destination must be constant."); }
                if (token->next) { throw QString("excess statement after destination."); }
                return new goto_stmt(((const_token *)token)->val); }
            case KW_IF: {
                token_node *token1 = token;
                while (token1->next && (
                    token1->next->type != TK_KEYWORD ||
                    ((kw_token *)token1->next)->val != KW_THEN)) {
                    token1 = token1->next;
                } if (!token1->next) { throw QString("missing THEN in if statement."); }
                token_node *token2 = token1->next;
                token_node *token3 = token2->next;
                if (!token3 || token3->type != TK_CONSTANT) { throw QString("destination must be constant."); }
                if (token3->next) { throw QString("excess statement after destination."); }
                token1->next = nullptr;
                exp_node *cond;
                try { cond = gen_exp(token->next); }
                catch (QString err_seq) { token1->next = token2; throw err_seq; }
                token1->next = token2;
                return new if_stmt(cond, ((const_token *)token3)->val); }
            case KW_END:
                if (token->next) { throw QString("excess statement after END"); }
                return new end_stmt();
        }
    } throw QString("illegal statement.");
}
