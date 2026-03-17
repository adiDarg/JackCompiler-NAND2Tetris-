#include <string.h>
#include "KeyWord.h"

#include <stdlib.h>
//
// Created by Owner on 17/12/2025.
//

const typedef struct {
    const char *text;
    Keyword kw;
} TextKeywordStruct;
TextKeywordStruct keyword_table[] = {
    {"class", KW_CLASS}, {"constructor", KW_CONSTRUCTOR},
    {"function", KW_FUNCTION}, {"method", KW_METHOD},
    {"field", KW_FIELD}, {"static", KW_STATIC},
    {"var", KW_VAR}, {"int", KW_INT}, {"char", KW_CHAR},
    {"boolean", KW_BOOLEAN}, {"void", KW_VOID},
    {"true", KW_TRUE}, {"false", KW_FALSE},
    {"null", KW_NULL}, {"this", KW_THIS},
    {"let", KW_LET}, {"do", KW_DO}, {"if", KW_IF},
    {"else", KW_ELSE}, {"while", KW_WHILE}, {"return", KW_RETURN}
};
int keyword_lookup(const char *s, Keyword *out) {
    for (size_t i = 0; i < sizeof keyword_table / sizeof *keyword_table; i++) {
        if (!strcmp(s, keyword_table[i].text)) {
            if (out != NULL)
                *out = keyword_table[i].kw;
            return 1;
        }
    }
    return 0;
}
const char *keyword_to_text(const Keyword kw) {
    for (size_t i = 0;
         i < sizeof keyword_table / sizeof *keyword_table;
         i++) {
        if (keyword_table[i].kw == kw) {
            return keyword_table[i].text;
        }
         }
    return NULL;  // not a keyword
}
const char *keywords_to_text(const Keyword* kw,const int len) {
    char* res = malloc(12 * len);
    for (int i = 0; i < len; i++) {
        strcat(res,keyword_to_text(kw[i]));
        strcat(res,",");
    }
    return res;
}