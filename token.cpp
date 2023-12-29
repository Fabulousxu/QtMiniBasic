#include "token.h"

QString toHTML(const QList<Token *> &tokenList) {
    QString HTML;
    for (auto it = tokenList.cbegin(); it != tokenList.cend(); ++it) {
        switch ((*it)->type) {
            case TOKEN_KEYWORD:
            case TOKEN_IDENTIFIER:
            case TOKEN_CONSTANT:
            case TOKEN_STRING:
                HTML += (*it)->toHTML() + "&nbsp;";
                break;
            case TOKEN_OPERATOR:
                switch (((OperatorToken *)*it)->value) {
                    case POSITIVE:
                    case NEGATIVE:
                    case LEFT_PAREN:
                        HTML += (*it)->toHTML();
                        break;
                    case RIGHT_PAREN:
                        if (HTML.mid(HTML.length() - 6) == "&nbsp;") {
                            HTML.remove(HTML.length() - 6, 6);
                        }
                    default:
                        HTML += (*it)->toHTML() + "&nbsp;";
                }
        }
    }
    return HTML;
}

void clearTokenList(QList<Token *> &tokenList) {
    for (auto &token : tokenList) { delete token; }
    tokenList.clear();
}
