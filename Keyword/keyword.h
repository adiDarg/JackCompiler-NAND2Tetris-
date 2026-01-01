//
// Created by Owner on 17/12/2025.
//

#ifndef KEYWORD_H
#define KEYWORD_H

typedef enum {
    CLASS, METHOD, FUNCTION, CONSTRUCTOR,
    INT, BOOLEAN, CHAR,
    VOID, VAR, STATIC,
    FIELD, LET, DO, IF,
    ELSE, WHILE,
    RETURN, TRUE,
    FALSE, KW_NULL, THIS,
    UNKNOWN_KEYWORD
} Keyword;

int keyword_lookup(const char *s, Keyword *out);
const char *keyword_to_text(const Keyword kw);

#endif
