#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "../Keyword/keyword.h"
#include "jackTokenizer.h"
//
// Created by Owner on 17/12/2025.
//



JackTokenizer* JT_Constructor(char *source) {
    JackTokenizer *self = malloc(sizeof(JackTokenizer));
    self->source = source;
    self->cursor = source;
    self->buffer[0] = '_';
    self->buffer[1] = '\0';
    return self;
}
int hasMoreTokens(const JackTokenizer *self) {
    return tokenType(self)!=EOF_TOKEN;
}
int isWhitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}
void skipWhitespace(JackTokenizer *self) {
    while (*self->cursor && isWhitespace(*self->cursor)) {
        self->cursor++;
    }
}
void skipComments(JackTokenizer *self) {
    if (!self || !self->cursor) {
        return;
    }
    while (self->cursor[0] == '/' && (self->cursor[1] == '/' || self->cursor[1] == '*')) {
        if (self->cursor[1] == '/') {
            while (*self->cursor != '\n' && *self->cursor != '\0') {
                self->cursor++;
            }
            self->cursor++;
        }
        else if (self->cursor[1] == '*') {
            while (!(*self->cursor == '*' && self->cursor[1] == '/') && *self->cursor != '\0') {
                self->cursor++;
            }
            self->cursor+=2;
        }
        skipWhitespace(self);
    }
}
void advance(JackTokenizer *self) {
    if (tokenType(self) == EOF_TOKEN) {
        return;
    }
    self->buffer[0] = '\0';
    skipWhitespace(self);
    skipComments(self);
    skipWhitespace(self);
    if (isSymbol(*self->cursor,1)) {
        self->buffer[0] = *self->cursor;
        self->buffer[1] = '\0';
        self->cursor++;
    }
    else if (*self->cursor == '"') {
        size_t len = 0;
        do {
            if (len >= sizeof(self->buffer) - 1) {
                // handle error: string too long
                break;
            }
            self->buffer[len++] = *self->cursor;
            self->cursor++;
        }while (*self->cursor != '\0' && *self->cursor != '"');
        if (*self->cursor == '"') {
            self->buffer[len++] = *self->cursor;
            self->buffer[len] = '\0';
            self->cursor++;
        }
    }
    else if (isdigit(*self->cursor)) {
        size_t len = 0;
        while (*self->cursor != '\0' && isdigit(*self->cursor)) {
            if (len >= sizeof(self->buffer) - 1) {
                // handle error: integer too long
                break;
            }
            self->buffer[len++] = *self->cursor;
            self->cursor++;
        }
        if (isWhitespace(*self->cursor) || *self->cursor == '\0' || isSymbol(*self->cursor,1)) {
            self->buffer[len] = '\0';
        }
    }
    else {
        size_t len = 0;
        while (*self->cursor && !isWhitespace(*self->cursor) && !isSymbol(*self->cursor,1)) {
            if (len >= sizeof(self->buffer) - 1) {
                //handle identifier name too long
                break;
            }
            self->buffer[len++] = *self->cursor++;
        }
        self->buffer[len] = '\0';
    }
}
int isSymbol(char token,int len);
int isIntConst(const char* token, int len);
int isStringConst(const char* token, int len);
int isIdentifier(const char* token, int len);
TokenType tokenType(const JackTokenizer *self) {
    const char* currentToken = self->buffer;
    const int len = strlen(self->buffer);
    if (len == 0) {
        return EOF_TOKEN;
    }
    if (keyword_lookup(currentToken,NULL)) {
        return KEYWORD;
    }
    if (isSymbol(*self->buffer,len)) {
        return SYMBOL;
    }
    if (isIntConst(currentToken,len)) {
        return INT_CONST;
    }
    if (isStringConst(currentToken,len)) {
        return STRING_CONST;
    }
    if (isIdentifier(currentToken,len)) {
        return IDENTIFIER;
    }
    return UNKNOWN_TOKEN;
}
//Helper functions for tokenType
int isSymbol(const char token, const int len) {
    if (len != 1) {
        return 0;
    }
    switch (token) {
        case '{': case '}': case '(': case ')': case '[': case ']':
        case '.': case ',': case ';': case '+': case '-': case '*': case '/':
        case '&': case '|': case '<': case '>': case '=': case '~':
            return 1;
        default:
            return 0;
    }
}
int isIntConst(const char* token,const int len) {
    for (int i = 0; i < len; i++) {
        if (!isdigit(token[i])) {
            return 0;
        }
    }
    return 1;
}
int isStringConst(const char* token, const int len) {
    if (token[0] != '"' || token[len-1] != '"') {
        return 0;
    }
    for (int i = 1; i < len -1 ; i++) {
        if (token[i] == '"' || token[i] == '\n') {
            return 0;
        }
    }
    return 1;
}
int isIdentifier(const char* token, const int len) {
    if (!isalpha(*token)) {
        return 0;
    }
    for (int i = 1; i < len; i++) {
        if (!isalpha(token[i]) && !isdigit(token[i]) && token[i] != '_') {
            return 0;
        }
    }
    return 1;
}
//End
Keyword keyword(const JackTokenizer *self) {
    Keyword out;
    keyword_lookup(self->buffer,&out);
    return out;
}
char symbol(const JackTokenizer *self) {
    return *self->buffer;
}
char* identifier(const JackTokenizer *self) {
    return strdup(self->buffer);
}
int intVal(const JackTokenizer *self) {
    return atoi(self->buffer);
}
char* stringVal(const JackTokenizer *self) {
    const size_t len = strlen(self->buffer);
    char *res = malloc(len-1);
    if (res == NULL) {
        return NULL;
    }
    memcpy(res,self->buffer + 1,len - 2);
    res[len-2] = '\0';
    return res;
}
