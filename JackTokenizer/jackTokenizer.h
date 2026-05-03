//
// Created by Owner on 18/12/2025.
//

#ifndef JACKTOKENIZER_H
#define JACKTOKENIZER_H
#include <stddef.h>

#include "Keyword/keyword.h"

typedef enum {
    TT_KEYWORD,
    TT_SYMBOL,
    TT_INT_CONST,
    TT_STRING_CONST,
    TT_IDENTIFIER,
    TT_UNKNOWN_TOKEN,
    TT_EOF_TOKEN
} CompilerTokenType ;

typedef struct {
    char buffer[100]; //100
    char error[100]; //100
    char *source; //8
    char *cursor; //8
    size_t dt_size; //8
    int isError; //4
    int line; //4
} JackTokenizer;


JackTokenizer* JT_Constructor(char *source,const size_t dt_size);
int hasMoreTokens(JackTokenizer *self);
void advance(JackTokenizer *self);
char *lookAhead(JackTokenizer *self);
int isSymbol(char token,int len);
int isIntConst(const char* token, int len);
int isStringConst(const char* token, int len);
int isIdentifier(const char* token, int len);
CompilerTokenType tokenType(JackTokenizer *self);
Keyword keyword(const JackTokenizer *self);
char symbol(const JackTokenizer *self);
char* identifier(const JackTokenizer *self);
int intVal(const JackTokenizer *self);
char* stringVal(const JackTokenizer *self);

#endif //JACKTOKENIZER_H
