//
// Created by Owner on 03/05/2026.
//

#include "Loader.h"
char* getRoutineType(const NodeAST *routine_type_node, const ClassTable *class_table) {
    if (routine_type_node->token->type == TT_IDENTIFIER) {
        char *class = routine_type_node->token->info.identifier;
        if (doesClassExist(class_table,class)) {
            return class;
        }
        return "";
    }
    switch (routine_type_node->token->info.keyword) {
        case KW_INT: {
            return "int";
        }
        case KW_CHAR: {
            return "char";
        }
        case KW_BOOLEAN: {
            return "boolean";
        }
        case KW_VOID: {
            return "void";
        }
        default: {
            return "";
        }
    }
}

void LoadToTables(const NodeAST *node,const char *class,
    RoutineTable *table, ClassTable *class_table) {
    if (node == NULL) {
        return;
    }
    if (node->nodeType == NODE_SUBROUTINE_DEC) {
        RoutineKind kind;
        switch (node->children[0]->token->info.keyword) {
            case KW_CONSTRUCTOR: {
                kind = ROUTINE_CONSTRUCTOR;
                break;
            }
            case KW_FUNCTION: {
                kind = ROUTINE_FUNCTION;
                break;
            }
            case KW_METHOD: {
                kind = ROUTINE_METHOD;
                break;
            }
            default: {
                kind = ROUTINE_NONE;
                break;
            }
        }
        const char *routine_type = getRoutineType(node->children[1],class_table);
        const char *routine_name = node->children[2]->token->info.identifier;
        defineRoutine(table,kind,routine_name,routine_type,class);
        return;
    }
    if (node->nodeType == NODE_ROOT) {
        defineClass(class_table,class);
    }
    for (int i = 0; i < node->currChildIndex ; i++) {
        LoadToTables(node->children[i],class,table,class_table);
    }
}