//
// Created by Owner on 17/12/2025.
//

#ifndef KEYWORD_H
#define KEYWORD_H

typedef enum {
    KW_CLASS, KW_METHOD, KW_FUNCTION, KW_CONSTRUCTOR,
    KW_INT, KW_BOOLEAN, KW_CHAR,
    KW_VOID, KW_VAR, KW_STATIC,
    KW_FIELD, KW_LET, KW_DO, KW_IF,
    KW_ELSE, KW_WHILE,
    KW_RETURN, KW_TRUE,
    KW_FALSE, KW_NULL, KW_THIS,
    KW_UNKNOWN_KEYWORD
} Keyword;

int keyword_lookup(const char *s, Keyword *out);
const char *keyword_to_text(const Keyword kw);
const char *keywords_to_text(const Keyword* kw,const int len);

#endif
