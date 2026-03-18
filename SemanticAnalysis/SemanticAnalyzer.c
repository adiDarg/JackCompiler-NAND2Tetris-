//
// Created by Owner on 18/03/2026.
//

#include "SemanticAnalyzer.h"

#include <stdio.h>
#include "RoutineTable/RoutineTable.h"
#include "SymbolTable/SymbolTable.h"
DataType getDT(SemanticData *data) {
    const Token *token = data->current->token;
    const SymbolTable *symbol_table = data->symbol_table;
    const RoutineTable *routine_table = data->routine_table;
    switch (token->type) {
        case TT_SYMBOL:
            case TT_STRING_CONST: {
            return TYPE_CHAR;
        }
        case TT_INT_CONST: {
            return TYPE_INT;
        }
        case TT_KEYWORD: {
            if (token->info.keyword == KW_TRUE || token->info.keyword == KW_FALSE) {
                return TYPE_BOOLEAN;
            }
            if (token->info.keyword == KW_THIS) {
                return TYPE_OBJECT;
            }
        }
        case TT_IDENTIFIER: {
            const char *type = typeOf(symbol_table,token->info.identifier,sizeof(token->info.identifier));
            if (type == "int") {
                return TYPE_INT;
            }
            if (type == "char") {
                return TYPE_CHAR;
            }
            if (type == "boolean") {
                return TYPE_BOOLEAN;
            }
            if (type != "") {
                return TYPE_OBJECT;
            }

            const Routine *routine = getRoutine(routine_table,
            token->info.identifier,sizeof(token->info.identifier));
            if (routine != NULL) {
                type = routine->type;
                if (type == "int") {
                    return TYPE_INT;
                }
                if (type == "char") {
                    return TYPE_CHAR;
                }
                if (type == "boolean") {
                    return TYPE_BOOLEAN;
                }
                if (type == "void") {
                    return TYPE_VOID;
                }
                return TYPE_OBJECT;
            }
            data->isError = 1;
            snprintf(data->error,data->error_size,"line %d: Undefined identifier '%s'",
                token->line, token->info.identifier);
            return TYPE_UNKNOWN;
        }
        default: {
            data->isError = 1;
            snprintf(data->error,data->error_size,"line %d: Invalid type",token->line);
            return TYPE_UNKNOWN;
        }
    }
}

SemanticData* construct_semantic_data(NodeAST *root,const size_t errorSize,
    const size_t symbolTableSize, const size_t routineTableSize) {
    SemanticData *semantic_data = malloc(sizeof(SemanticData));
    semantic_data->root = root;
    semantic_data->current = root;
    semantic_data->error_size = errorSize;
    semantic_data->error = malloc(errorSize);
    semantic_data->symbol_table = symbol_table_constructor(symbolTableSize);
    semantic_data->routine_table = routine_table_constructor(routineTableSize);
    semantic_data->isError = 0;
    return semantic_data;
}
char Analyze(SemanticData *self) {
    NodeAST *node = self->current;
    if (node == NULL) {
        return 1;
    }
    switch (node->nodeType) {
        case NODE_IDENTIFIER: case NODE_INTEGER_CONSTANT:
        case NODE_STRING_CONSTANT:case NODE_SYMBOL:case NODE_KEYWORD: {
            node->dataType = getDT(self);
            break;
        }
        case NODE_ROOT: {

        }
        default: {
            for (int i = 0; i < node->currChildIndex; i++) {
                self->current = node->children[i];
                self->current = node;
            }
            break;
        }
    }
    if (self->isError) {
        return 0;
    }
    return 1;
}