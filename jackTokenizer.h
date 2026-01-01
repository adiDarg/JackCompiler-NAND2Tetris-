//
// Created by Owner on 18/12/2025.
//

#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H
#include "keyword.h"

typedef enum {
    KEYWORD,
    SYMBOL,
    INT_CONST,
    STRING_CONST,
    IDENTIFIER,
    UNKNOWN_TOKEN,
    EOF_TOKEN
} TokenType ;

typedef struct {
    char *source;
    char *cursor;
    char buffer[100];
} JackTokenizer;


JackTokenizer* JT_Constructor(char *source);
int hasMoreTokens(const JackTokenizer *self);
void advance(JackTokenizer *self);
int isSymbol(char token,int len);
int isIntConst(const char* token, int len);
int isStringConst(const char* token, int len);
int isIdentifier(const char* token, int len);
TokenType tokenType(const JackTokenizer *self);
Keyword keyword(const JackTokenizer *self);
char symbol(const JackTokenizer *self);
char* identifier(const JackTokenizer *self);
int intVal(const JackTokenizer *self);
char* stringVal(const JackTokenizer *self);

#endif //JACKTOKENIZER_H
