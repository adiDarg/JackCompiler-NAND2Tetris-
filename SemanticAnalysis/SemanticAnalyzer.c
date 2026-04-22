//
// Created by Owner on 18/03/2026.
//

#include "SemanticAnalyzer.h"

#include <stdio.h>
#include <string.h>

#include "RoutineTable/RoutineTable.h"
#include "SymbolTable/SymbolTable.h"
void getDT(SemanticData *data) {
    const Token *token = data->current->token;
    const SymbolTable *symbol_table = data->symbol_table;
    const RoutineTable *routine_table = data->routine_table;
    const ClassTable *class_table = data->class_table;
    DataType data_type = TYPE_UNKNOWN;
    switch (token->type) {
        case TT_SYMBOL:
            case TT_STRING_CONST: {
            data_type = TYPE_CHAR;
            break;
        }
        case TT_INT_CONST: {
            data_type = TYPE_INT;
            break;
        }
        case TT_KEYWORD: {
            if (token->info.keyword == KW_TRUE || token->info.keyword == KW_FALSE) {
                data_type = TYPE_BOOLEAN;
            }
            else if (token->info.keyword == KW_THIS) {
                data_type = TYPE_OBJECT;
            }
            break;
        }
        case TT_IDENTIFIER: {
            const char *type = typeOf(symbol_table,token->info.identifier,strlen(token->info.identifier));
            if (strcmp(type,"int") == 0) {
                data_type = TYPE_INT;
            }
            else if (strcmp(type,"char") == 0) {
                data_type = TYPE_CHAR;
            }
            else if (strcmp(type,"boolean") == 0) {
                data_type = TYPE_BOOLEAN;
            }
            else if (strcmp(type,"")) {
                if (doesClassExist(class_table,type,strlen(type))) {
                    data_type = TYPE_OBJECT;
                }
            }

            const Routine *routine = getRoutine(routine_table,
            token->info.identifier,strlen(token->info.identifier));
            if (routine != NULL) {
                type = routine->type;
                if (strcmp(type,"int") == 0) {
                    data_type = TYPE_INT;
                }
                else if (strcmp(type,"char") == 0) {
                    data_type = TYPE_CHAR;
                }
                else if (strcmp(type,"boolean") == 0) {
                    data_type = TYPE_BOOLEAN;
                }
                else if (strcmp(type,"void") == 0) {
                    data_type = TYPE_VOID;
                }
                else if (strcmp(type,"")) {
                    if (doesClassExist(class_table,type,strlen(type))) {
                        data_type = TYPE_OBJECT;
                    }
                }
            }
            else {
                data->isError = 1;
                snprintf(data->error,data->error_size,"line %d: Undefined identifier '%s'",
                    token->line, token->info.identifier);
            }
            break;
        }
        default: {
            data->isError = 1;
            snprintf(data->error,data->error_size,"line %d: Invalid type",token->line);
        }
    }
    data->current->dataType = data_type;
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
char generalVarDec(SemanticData *self,const SymbolKind kind);
char AnalyzeClassVarDec(SemanticData *self);
char AnalyzeSubRoutineDec(SemanticData *self);
char AnalyzeParameterList(SemanticData *self);
char AnalyzeSubroutineBody(SemanticData *self);
char AnalyzeVarDec(SemanticData *self);
char AnalyzeStatements(SemanticData *self);
char AnalyzeStatement(SemanticData *self);
char AnalyzeLetStatement(SemanticData *self);
char AnalyzeIfStatement(SemanticData *self);
char AnalyzeWhileStatement(SemanticData *self);
char AnalyzeDoStatement(SemanticData *self);
char AnalyzeReturnStatement(SemanticData *self);
char AnalyzeExpression(SemanticData *self);
char AnalyzeSubroutineCall(SemanticData *self);
char AnalyzeTerm(SemanticData *self);
//char AnalyzeOperator(SemanticData *self); Redundant
char isOperatorLegal(const DataType data,const NodeAST *symbol);
char AnalyzeExpressionList(SemanticData *self);

char Analyze(SemanticData *self) {
    return AnalyzeClass(self);
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
char* type_of_node(const NodeAST *type_node,const ClassTable *class_table);
char generalVarDec(SemanticData *self,const SymbolKind kind) {
    const NodeAST *node = self->current;
    const NodeAST *type_node = node->children[1];
    char* typeStr = type_of_node(type_node,self->class_table);
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
char AnalyzeClassVarDec(SemanticData *self) {
    const NodeAST* node = self->current;
    const NodeAST *scope_node = node->children[0];
    const SymbolKind kind = subroutine_scope_of_node(scope_node);
    if (kind == SK_NONE) {
        reportError(self,"invalid scope type for variable",scope_node->token->line);
        return 0;
    }
    return generalVarDec(self,kind);
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
char* type_of_node(const NodeAST *type_node,const ClassTable *class_table) {
    //A type can be int,char,boolean or className
    //int,char,boolean are keyword tokens
    //className is an identifier token
    //Therefore: tokenType of the type node would be a typeOfType
    const TokenType typeOfType = type_node->token->type;
    char *typeStr = malloc(100);
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
                free(typeStr);
                return "";
        }
    }
    else if (typeOfType == TT_IDENTIFIER) {
        const char *identifier = type_node->token->info.identifier;
        if (!doesClassExist(class_table,identifier,strlen(identifier))) {
            //Class is not defined
            free(typeStr);
            return "";
        }
        strncpy(typeStr,identifier,sizeof(typeStr));
    }
    else {
        free(typeStr);
        return "";
    }
    return typeStr;
}

RoutineKind getRoutineKindOfNode(const NodeAST *routine_kind_node);
char* getRoutineType(const NodeAST *routine_type_node, const ClassTable *class_table);
char AnalyzeSubRoutineDec(SemanticData *self) {
    NodeAST *node = self->current;
    const NodeAST *routine_kind_node = node->children[0];
    const RoutineKind kind = getRoutineKindOfNode(routine_kind_node);
    if (kind == ROUTINE_NONE) {
        reportError(self,"Routine kind should be constructor, function or method",routine_kind_node->token->line);
        return 0;
    }

    const NodeAST *routine_type_node = node->children[1];
    const char* routine_type = getRoutineType(routine_kind_node,self->class_table);
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
RoutineKind getRoutineKindOfNode(const NodeAST *routine_kind_node) {
    const Keyword keyword = routine_kind_node->token->info.keyword;
    if (keyword == KW_CONSTRUCTOR) {
        return ROUTINE_CONSTRUCTOR;
    }
    if (keyword == KW_METHOD) {
        return ROUTINE_METHOD;
    }
    if (keyword == KW_FUNCTION) {
        return ROUTINE_FUNCTION;
    }
    return ROUTINE_NONE;
}
char* getRoutineType(const NodeAST *routine_type_node, const ClassTable *class_table) {
    if (routine_type_node->token->type == TT_IDENTIFIER) {
        char *class = routine_type_node->token->info.identifier;
        if (doesClassExist(class_table,class,strlen(class))) {
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

char AnalyzeParameterList(SemanticData *self) {
    const NodeAST *node = self->current;
    for (int i = 0; i < node->currChildIndex; i+=3) {
        const NodeAST *type_node = node->children[i];
        const NodeAST *ident_node = node->children[i+1];
        char *type_str = type_of_node(type_node,self->class_table);
        char *identifier = ident_node->token->info.identifier;
        if (kindOf(self->symbol_table,identifier,strlen(identifier)) == SK_ARG) {
            //Symbol already defined as an argument
            reportError(self,strcat(identifier," already defined as an argument"),ident_node->token->line);
            free(type_str);
            return 0;
        }
        define(self->symbol_table,identifier,strlen(identifier),
            type_str,strlen(type_str),SK_ARG);
    }
    return 1;
}

char AnalyzeSubroutineBody(SemanticData *self) {
    NodeAST *node = self->current;
    int i;
    for (i = 1; i < node->currChildIndex - 2; i++) {
        self->current = node->children[i];
        if (!AnalyzeVarDec(self)) {
            return 0;
        }
    }
    self->current = node->children[i];
    const char success = AnalyzeStatements(self);
    self->current = node;
    return success;
}
char AnalyzeVarDec(SemanticData *self) {
    return generalVarDec(self,SK_VAR);
}

char AnalyzeStatements(SemanticData *self) {
    NodeAST *node = self->current;
    for (int i = 0; i < node->currChildIndex; i++) {
        self->current = node->children[i];
        if (!AnalyzeStatement(self)) {
            return 0;
        }
    }
    self->current = node;
    return 1;
}
char AnalyzeStatement(SemanticData *self) {
    switch (self->current->nodeType) {
        case NODE_LET_STATEMENT: {
            return AnalyzeLetStatement(self);
        }
        case NODE_IF_STATEMENT: {
            return AnalyzeIfStatement(self);
        }
        case NODE_WHILE_STATEMENT: {
            return AnalyzeWhileStatement(self);
        }
        case NODE_DO_STATEMENT: {
            return AnalyzeDoStatement(self);
        }
        case NODE_RETURN_STATEMENT: {
            return AnalyzeReturnStatement(self);
        }
        default: {
            return 0;
        }
    }
}

char AnalyzeLetStatement(SemanticData *self) {
    NodeAST *node = self->current;

    //datatype of variable
    self->current = node->children[1];
    getDT(self);
    const DataType data_type = self->current->dataType;
    if (data_type == TYPE_UNKNOWN) {
        reportError(self,"Undefined identifier",self->current->token->line);
        return 0;
    }

    //Expression index = where the expression is in the statement
    int expressionIndex;
    //let varName('['expression']')? '=' ...
    //children[2] = '[' -> varName[expression]=...
    //children[2] = '=' -> varName=...
    if (node->children[2]->token->info.symbol == '[') {
        //Verify variable to be of array type
        const char *ident = node->children[1]->token->info.identifier;
        const char *type = typeOf(self->symbol_table,ident,strlen(ident));
        if (strcmp(type,"Array")) {
            reportError(self,"Can't index non-array data type",node->children[1]->token->line);
            return 0;
        }
        //Verify expression to be a valid index
        self->current = node->children[3];
        if (!AnalyzeExpression(self)) {
            return 0;
        }
        if (self->current->dataType != TYPE_INT) {
            reportError(self,"Expected integer as index for array",node->children[1]->token->line);
            return 0;
        }
        //If array is shorter than the index -> illegal indexing
        if (lengthOf(self->symbol_table,ident,strlen(ident)) < self->current->token->info.intVal) {
            reportError(self,"Index larger than array size",node->children[3]->token->line);
            return 0;
        }
        self->current = node;
        expressionIndex = 6;
    }
    else {
        expressionIndex = 3;
    }
    NodeAST *expression_node = node->children[expressionIndex];
    self->current = expression_node;
    if (!AnalyzeExpression(self)) {
        return 0;
    }

    if ((data_type == TYPE_OBJECT && expression_node->dataType != TYPE_INT) ||
        expression_node->dataType != data_type) {
        reportError(self,"Type mismatch",node->children[1]->token->line);
        return 0;
    }

    self->current = node;
    return 1;
}
char AnalyzeIfStatement(SemanticData *self) {
    NodeAST* node = self->current;
    self->current = node->children[2];
    if (!AnalyzeExpression(self)) {
        return 0;
    }
    if (self->current->dataType != TYPE_BOOLEAN) {
        return 0;
    }
    self->current = node->children[5];
    if (!AnalyzeStatements(self)) {
        return 0;
    }
    //Check for existance of else statement
    if (node->currChildIndex == 10) {
        self->current = node->children[9];
        if (!AnalyzeStatements(self)) {
            return 0;
        }
    }
    self->current = node;
    return 1;
}
char AnalyzeWhileStatement(SemanticData *self) {
    NodeAST* node = self->current;
    self->current = node->children[2];
    if (!AnalyzeExpression(self)) {
        return 0;
    }
    if (self->current->dataType != TYPE_BOOLEAN) {
        return 0;
    }
    self->current = node->children[5];
    if (!AnalyzeStatements(self)) {
        return 0;
    }
    self->current = node;
    return 1;
}
char AnalyzeDoStatement(SemanticData *self) {
    NodeAST *node = self->current;
    if (!AnalyzeSubroutineCall(self)) {
        return 0;
    }
    self->current = node;
    return 1;
}
char AnalyzeReturnStatement(SemanticData *self) {
    NodeAST *node = self->current;
    self->current = node->children[1];
    if (!AnalyzeExpression(self)) {
        return 0;
    }
    //current = expression
    //node = returnStatement
    //parent = statement
    //parent^2 = statements
    //parent^3 = subroutineBody
    //parent^4 = subroutineDec
    const NodeAST *subroutineDecNode = node->parent->parent->parent->parent;
    const char *name = subroutineDecNode->children[2]->token->info.identifier;
    const Routine *routine = getRoutine(self->routine_table,name,strlen(name));
    const char* type_routine = routine->type;
    switch (self->current->dataType) {
        case TYPE_INT: {
            if (strcmp(type_routine,"int")) {
                reportError(self,"expected int return type",self->current->token->line);
                return 0;
            }
            break;
        }
        case TYPE_CHAR: {
            if (strcmp(type_routine,"char")) {
                reportError(self,"expected char return type",self->current->token->line);
                return 0;
            }
            break;
        }
        case TYPE_OBJECT: {
            if (self->current->token == NULL ||
                strcmp(self->current->token->info.identifier,type_routine)) {
                reportError(self,"expected Object return type",self->current->token->line);
                return 0;
            }
            break;
        }
        case TYPE_BOOLEAN: {
            if (strcmp(type_routine,"boolean")) {
                reportError(self,"expected boolean return type",self->current->token->line);
                return 0;
            }
            break;
        }
        case TYPE_VOID: {
            reportError(self,"expected no return type",self->current->token->line);
            return 0;
        }
        default: {
            return 0;
        }
    }
    self->current = node;
    return 1;
}

char isOperatorLegal(const DataType data,const NodeAST *symbol) {
    switch (symbol->token->info.symbol) {
        case '+': case '-': case '*': case '/': case '<': case '>': {
            return data == TYPE_INT;
        }
        case '&': case '|': {
            return data == TYPE_BOOLEAN;
        }
        default: {
            return 1;
        }
    }
}
char AnalyzeExpression(SemanticData *self) {
    NodeAST *node = self->current;
    self->current = node->children[0];
    if (!AnalyzeTerm(self)) {
        return 0;
    }
    const DataType type = self->current->dataType;
    node->dataType = type;
    for (int i = 1; i < node->currChildIndex; i+=2) {
        self->current = node->children[i];
        //No need to analyze operator
        self->current = node->children[i+1];
        if (!AnalyzeTerm(self)) {
            return 0;
        }
        if (self->current->dataType != type) {
            reportError(self,"Type mismatch in expression",self->current->token->line);
            return 0;
        }
        if (!isOperatorLegal(type,node->children[i])) {
            reportError(self,"Illegal operator for data type",self->current->token->line);
            return 0;
        }
    }
    self->current = node;
    return 1;
}

char AnalyzeSubroutineCall(SemanticData *self) {
    NodeAST *node = self->current;
    //subName(expressionList) vs object.subName(expressionList)
    char *class;
    char *subroutine_name;
    NodeAST *expression_list;
    int line;
    if (node->children[2]->nodeType == NODE_EXPRESSION) {
        class = self->root->children[1]->token->info.identifier;
        subroutine_name = node->children[0]->token->info.identifier;
        expression_list = node->children[2];
        line = node->children[0]->token->line;
    }
    else {
        const Token *ident_token = node->children[0]->token;
        const char *ident = ident_token->info.identifier;
        line = ident_token->line;
        class = typeOf(self->symbol_table,ident,strlen(ident));
        subroutine_name = node->children[2]->token->info.identifier;
        expression_list = node->children[4];
    }
    if (strcmp(class,"") == 0) {
        reportError(self,"Undefined variable",line);
        return 0;
    }
    const Routine *routine = getRoutine(self->routine_table,subroutine_name,strlen(subroutine_name));
    if (routine == NULL) {
        reportError(self,"Undefined routine called",line);
        return 0;
    }
    if (strcmp(routine->class,class)) {
        reportError(self,"Routine can't be called from this class",line);
    }
    self->current = expression_list;
    if (!AnalyzeExpressionList(self)) {
        return 0;
    }
    self->current = node;
    return 1;
}
char AnalyzeTerm(SemanticData *self) {
    NodeAST *node = self->current;
    switch (node->children[0]->nodeType) {
        case NODE_UNARY_OP: {
            self->current = node->children[1];
            const char res = AnalyzeTerm(self);
            self->current = node;
            return res;
        }

        case NODE_SUBROUTINE_CALL: {
            self->current = node->children[0];
            const char res = AnalyzeSubroutineCall(self);
            self->current = node;
            return res;
        }

        case NODE_INTEGER_CONSTANT: case NODE_STRING_CONSTANT: case NODE_KEYWORD_CONSTANT: {
            return 1;
        }

        case NODE_IDENTIFIER: {
            const char *name = node->children[0]->token->info.identifier;
            const char *ident_type = typeOf(self->symbol_table,name,strlen(name));
            if (strcmp(ident_type,"") == 0) {
                reportError(self,"Undefined variable",node->children[0]->token->line);
                return 0;
            }

            if (node->currChildIndex > 1) {
                self->current = node->children[2];
                if (!AnalyzeExpression(self)) {
                    return 0;
                }
                if (strcmp(ident_type,"Array")) {
                    reportError(self,"Expected array type",node->children[2]->token->line);
                    return 0;
                }
                //No length check for now - overflow possible
            }
            return 1;
        }

        case NODE_SYMBOL: {
            self->current = node->children[1];
            const char res = AnalyzeExpression(self);
            self->current = node;
            return res;
        }

        default: {
            return 0;
        }
    }
}