//
// Created by Owner on 18/03/2026.
//

#include "SemanticAnalyzer.h"

#include <stdio.h>
#include <string.h>

#include "RoutineTable/RoutineTable.h"
#include "SymbolTable/SymbolTable.h"
DataType getDT(SemanticData *data) {
    const Token *token = data->current->token;
    const SymbolTable *symbol_table = data->symbol_table;
    const RoutineTable *routine_table = data->routine_table;
    const ClassTable *class_table = data->class_table;
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
            const char *type = typeOf(symbol_table,token->info.identifier,strlen(token->info.identifier));
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
                if (doesClassExist(class_table,type,strlen(type))) {
                    return TYPE_OBJECT;
                }
                return TYPE_UNKNOWN;
            }

            const Routine *routine = getRoutine(routine_table,
            token->info.identifier,strlen(token->info.identifier));
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
                if (doesClassExist(class_table,type,strlen(type))) {
                    return TYPE_OBJECT;
                }
                return TYPE_UNKNOWN;
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
    const size_t symbolTableSize, const size_t routineTableSize, const size_t class_table_size) {
    SemanticData *semantic_data = malloc(sizeof(SemanticData));
    semantic_data->root = root;
    semantic_data->current = root;
    semantic_data->error_size = errorSize;
    semantic_data->error = malloc(errorSize);
    semantic_data->symbol_table = symbol_table_constructor(symbolTableSize);
    semantic_data->routine_table = routine_table_constructor(routineTableSize);
    semantic_data->class_table = class_table_constructor(class_table_size);
    semantic_data->isError = 0;
    return semantic_data;
}
void reportError(SemanticData* self,const char *error,const int line) {
    self->isError = 1;
    snprintf(self->error,self->error_size,"line %d: %s",line,error);
}


char AnalyzeClass(SemanticData *self);
char AnalyzeClassVarDec(SemanticData *self);
char AnalyzeSubRoutineDec(SemanticData *self);
char AnalyzeParameterList(SemanticData *self);
char AnalyzeSubroutineBody(SemanticData *self);

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
            return AnalyzeClass(self);
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
char AnalyzeClass(SemanticData *self) {
    NodeAST* node = self->current;
    char result = 1;
    for (int i = 0; i < node->currChildIndex; i++) {
        NodeAST *child = node->children[i];
        if (child == NULL) {
            continue;
        }
        if (child->nodeType == NODE_IDENTIFIER) {
            defineClass(self->class_table,child->token->info.identifier,
                strlen(child->token->info.identifier));
        }
        else if (child->nodeType == NODE_CLASS_VAR_DEC) {
            self->current = child;
            result = 1 & AnalyzeClassVarDec(self);
            self->current = node;
        }
        else if (child->nodeType == NODE_SUBROUTINE_DEC) {
            self->current = child;
            result = 1 & AnalyzeSubRoutineDec(self);
            self->current = node;
        }
    }
    return result;
}

SymbolKind subroutine_scope_of_node(const NodeAST *scope_node);
char* type_of_node(const NodeAST *type_node,const ClassTable *class_table,const int len);
char AnalyzeClassVarDec(SemanticData *self) {
    const NodeAST* node = self->current;
    const NodeAST *scope_node = node->children[0];
    const SymbolKind kind = subroutine_scope_of_node(scope_node);
    if (kind == SK_NONE) {
        reportError(self,"invalid scope type for variable",scope_node->token->line);
        return 0;
    }
    const NodeAST *type_node = node->children[1];
    char* typeStr = type_of_node(type_node,self->class_table,100);
    if (typeStr == "") {
        reportError(self,"invalid type for variable",type_node->token->line);
        free(typeStr);
        return 0;
    }

    for (int i = 2; i < node->currChildIndex; i+=2) {
        const NodeAST *varName = node->children[i];
        char *identifier = varName->token->info.identifier;
        const int result = define(self->symbol_table,identifier,strlen(identifier),
            typeStr,strlen(typeStr),kind);
        if (!result) {
            reportError(self,"variable is already defined",varName->token->line);
            free(typeStr);
            return 0;
        }
    }
    free(typeStr);
    return 1;
}
SymbolKind subroutine_scope_of_node(const NodeAST *scope_node) {
    const Keyword scope_value = scope_node->token->info.keyword;
    if (scope_value == KW_STATIC) {
        return SK_STATIC;
    }
    if (scope_value == KW_FIELD) {
        return SK_FIELD;
    }
    return SK_NONE;
}
char* type_of_node(const NodeAST *type_node,const ClassTable *class_table,const int len) {
    //A type can be int,char,boolean or className
    //int,char,boolean are keyword tokens
    //className is an identifier token
    //Therefore: tokenType of the type node would be a typeOfType
    const TokenType typeOfType = type_node->token->type;
    char *typeStr = malloc(len);
    if (typeOfType == TT_KEYWORD) {
        //Needs to be int, char or boolean
        const Keyword type_value = type_node->token->info.keyword;
        switch (type_value) {
            case KW_INT: {
                strncpy(typeStr,"int",sizeof(typeStr));
                break;
            }
            case KW_CHAR: {
                strncpy(typeStr,"char",sizeof(typeStr));
                break;
            }
            case KW_BOOLEAN: {
                strncpy(typeStr,"boolean",sizeof(typeStr));
                break;
            }
            default:
                return "";
        }
    }
    else if (typeOfType == TT_IDENTIFIER) {
        const char *identifier = type_node->token->info.identifier;
        if (!doesClassExist(class_table,identifier,strlen(identifier))) {
            //Class is not defined
            return "";
        }
        strncpy(typeStr,identifier,sizeof(typeStr));
    }
    else {
        return "";
    }
    return typeStr;
}

RoutineKind getRoutineKindOfNode(const NodeAST *routine_kind_node);
char* getRoutineType(const NodeAST *routine_type_node);
char AnalyzeSubRoutineDec(SemanticData *self) {
    NodeAST *node = self->current;
    const NodeAST *routine_kind_node = node->children[0];
    const RoutineKind kind = getRoutineKindOfNode(routine_kind_node);
    if (kind == ROUTINE_NONE) {
        reportError(self,"Routine kind should be constructor, function or method",routine_kind_node->token->line);
        return 0;
    }

    const NodeAST *routine_type_node = node->children[1];
    const char* routine_type = getRoutineType(routine_kind_node);
    if (routine_type == "") {
        reportError(self,"Invalid routine type",routine_type_node->token->line);
        return 0;
    }

    const char* routine_name = node->children[2]->token->info.identifier;
    if (!defineRoutine(self->routine_table,kind,
        routine_name,strlen(routine_name),
        routine_type,strlen(routine_type))) {
        reportError(self,"Routine already defined",node->children[2]->token->line);
        return 0;
        }

    self->current = node->children[5];
    if (!AnalyzeParameterList(self)) {
        return 0;
    }
    self->current = node->children[7];
    if (!AnalyzeSubroutineBody(self)) {
        return 0;
    }
    self->current = node;
    return 1;
}