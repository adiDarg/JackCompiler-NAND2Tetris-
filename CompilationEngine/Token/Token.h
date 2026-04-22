//
// Created by Owner on 17/03/2026.
//

#ifndef TOKEN_H
#define TOKEN_H
#include "../../JackTokenizer/Keyword/keyword.h"
#include "../../JackTokenizer/jackTokenizer.h"
typedef union TokenInfo {
    Keyword keyword;
    char symbol;
    char *identifier;
    int intVal;
    char *stringVal;
} TokenInfo;
typedef struct Token {
    TokenType type;
    TokenInfo info;
    int line;
}Token;
Token* createToken(JackTokenizer *tokenizer);
#endif //TOKEN_H
