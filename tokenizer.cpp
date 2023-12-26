#include "tokenizer.h"

token_node *scan(const QString &code) {
    token_node *head = nullptr, *curr = nullptr;
#define APPEND(new_node) \
    if (head) { \
        curr->next = new_node; \
        curr = curr->next; \
    } else { curr = head = new_node; }

    for (auto it = code.begin(); it != code.end(); ++it) {
        if (it->isSpace()) { continue; }
        if (curr && curr->type == TK_KEYWORD && ((kw_token *)curr)->val == KW_REM) {
            curr->next = new str_token(code.mid(it - code.begin()).trimmed());
            return head;
        } else if (it->isLetter() || *it == '_') {
            QString val{ *it };
            while (it + 1 != code.end() && ((it + 1)->isLetterOrNumber() || *(it + 1) == '_')) { val.append(*(++it)); }
            if (val == "REM") {
                APPEND(new kw_token(KW_REM));
            } else if (val == "LET") {
                APPEND(new kw_token(KW_LET));
            } else if (val == "PRINT") {
                APPEND(new kw_token(KW_PRINT));
            } else if (val == "INPUT") {
                APPEND(new kw_token(KW_INPUT));
            } else if (val == "GOTO") {
                APPEND(new kw_token(KW_GOTO));
            } else if (val == "IF") {
                APPEND(new kw_token(KW_IF));
            } else if (val == "THEN") {
                APPEND(new kw_token(KW_THEN));
            } else if (val == "END") {
                APPEND(new kw_token(KW_END));
            } else if (val == "MOD") {
                APPEND(new op_token(MOD));
            } else { APPEND(new id_token(val)); }
        } else if (it->isDigit()) {
            qint64 val = it->digitValue();
            while (it + 1 != code.end() && (it + 1)->isDigit()) { val = val * 10 + (++it)->digitValue(); }
            APPEND(new const_token(val));
        } else if (*it == '+') {
            if (curr && (curr->type == TK_CONSTANT || curr->type == TK_IDENTIFIER ||
                (curr->type == TK_OPERATOR && ((op_token *)curr)->val == RPAREN))) {
                APPEND(new op_token(ADD));
            } else { APPEND(new op_token(POS)); }
        } else if (*it == '-') {
            if (curr && (curr->type == TK_CONSTANT || curr->type == TK_IDENTIFIER ||
                (curr->type == TK_OPERATOR && ((op_token *)curr)->val == RPAREN))) {
                APPEND(new op_token(SUB));
            } else { APPEND(new op_token(NEG)); }
        } else if (*it == '*') {
            if (it + 1 != code.end() && *(it + 1) == '*') {
                APPEND(new op_token(POW));
                ++it;
            } else { APPEND(new op_token(MUL)); }
        } else if (*it == '/') {
            APPEND(new op_token(DIV));
        } else if (*it == '<') {
            if (it + 1 != code.end() && *(it + 1) == '=') {
                APPEND(new op_token(LE));
                ++it;
            } else if (it + 1 != code.end() && *(it + 1) == '>') {
                APPEND(new op_token(NE));
                ++it;
            } else { APPEND(new op_token(LT)); }
        } else if (*it == '>') {
            if (it + 1 != code.end() && *(it + 1) == '=') {
                APPEND(new op_token(GE));
                ++it;
            } else { APPEND(new op_token(GT)); }
        } else if (*it == '=') {
            APPEND(new op_token(EQ));
        } else if (*it == '(') {
            APPEND(new op_token(LPAREN));
        } else if (*it == ')') {
            APPEND(new op_token(RPAREN));
        } else {
            APPEND(new str_token(code.mid(it - code.begin()).trimmed()));
            throw head;
        }
    }
    return head;
#undef APPEND
}

QString to_HTML(token_node *token) {
    QString str;
    for (; token; token = token->next) {
        if (token->type == TK_KEYWORD) {
            str += "<font color=skyblue>" + kw_str[((kw_token *)token)->val] + "</font>&nbsp;";
        } else if (token->type == TK_IDENTIFIER) {
            str += "<font color=white>" + ((id_token *)token)->val + "</font>&nbsp;";
        } else if (token->type == TK_CONSTANT) {
            str += "<font color=darkorange>" + QString::number(((const_token *)token)->val) + "</font>&nbsp;";
        } else if (token->type == TK_STRING) {
            str += "<font color=grey>" + ((str_token *)token)->val + "</font>";
        } else if (token->type == TK_OPERATOR) {
            if (((op_token *)token)->val == POS ||
                ((op_token *)token)->val == NEG ||
                ((op_token *)token)->val == LPAREN) {
                str += "<font color=gold>" + op_str[((op_token *)token)->val] + "</font>";
            } else {
                if (((op_token *)token)->val == RPAREN) {
                    if (str.mid(str.length() - 6) == "&nbsp;") { str.remove(str.length() - 6, 6); }
                }
                str += "<font color=gold>" + op_str[((op_token *)token)->val] + "</font>&nbsp;";
            }
        }
    }
    return str;
}
