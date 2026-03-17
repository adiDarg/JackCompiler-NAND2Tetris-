//
// Created by Owner on 17/03/2026.
//

#ifndef TOKEN_H
#define TOKEN_H
#include "../Keyword/keyword.h"
#include "../JackTokenizer/jackTokenizer.h"
typedef struct Token {
    TokenType type;
    typedef union TokenInfo {
        Keyword keyword;
        char symbol;
        char *identifier;
        int intVal;
        char *stringVal;
    } TokenInfo;
    int line;
}Token;
#endif //TOKEN_H
