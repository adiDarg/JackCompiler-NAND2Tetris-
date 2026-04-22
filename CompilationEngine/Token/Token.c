#include "Token.h"

#include <stdlib.h>

Token* createToken(JackTokenizer *tokenizer) {
    Token *token = malloc(sizeof(Token));
    token->line = tokenizer->line;
    token->type = tokenType(tokenizer);
    switch (token->type) {
        case TT_KEYWORD: {
            token->info.keyword = keyword(tokenizer);
            break;
        }
        case TT_SYMBOL: {
            token->info.symbol = symbol(tokenizer);
            break;
        }
        case TT_INT_CONST: {
            token->info.intVal = intVal(tokenizer);
            break;
        }
        case TT_STRING_CONST: {
            token->info.stringVal = stringVal(tokenizer);
            break;
        }
        case TT_IDENTIFIER: {
            token->info.identifier = identifier(tokenizer);
            break;
        }
        default: {
            break;
        }
    }
    return token;
}
