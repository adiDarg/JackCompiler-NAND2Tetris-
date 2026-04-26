//
// Created by Owner on 17/03/2026.
//

#ifndef TOKEN_H
#define TOKEN_H
#include "../../JackTokenizer/Keyword/keyword.h"
#include "../../JackTokenizer/jackTokenizer.h"
typedef union TokenInfo { //8
    Keyword keyword;
    char symbol;
    char *identifier;
    int intVal;
    char *stringVal;
} TokenInfo;
typedef struct Token {
    TokenInfo info; //8
    int line; //4
    TokenType type; //3 bits - 4 bytes
}Token;
Token* createToken(JackTokenizer *tokenizer);
#endif //TOKEN_H
