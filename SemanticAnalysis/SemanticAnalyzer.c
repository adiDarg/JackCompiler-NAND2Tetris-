//
// Created by Owner on 18/03/2026.
//

#include "SemanticAnalyzer.h"

#include <stdio.h>
#include <string.h>

#include "RoutineTable/RoutineTable.h"
#include "SymbolTable/SymbolTable.h"

SemanticData* construct_semantic_data(NodeAST *root,const size_t errorSize,
    const size_t symbolTableSize, const size_t routineTableSize, const size_t class_table_size,
    const size_t dt_size) {
    SemanticData *semantic_data = malloc(sizeof(SemanticData));
    semantic_data->root = root;
    semantic_data->current = root;
    semantic_data->error_size = errorSize;
    semantic_data->error = malloc(errorSize);
    semantic_data->symbol_table = symbol_table_constructor(symbolTableSize);
    semantic_data->routine_table = routine_table_constructor(routineTableSize);
    semantic_data->class_table = class_table_constructor(class_table_size);
    semantic_data->isError = 0;
    semantic_data->dt_size = dt_size;
    return semantic_data;
}
void reportError(SemanticData* self,const char *error,const int line) {
    self->isError = 1;
    snprintf(self->error,self->error_size,"line %d: %s",line,error);
}

//Interface
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
char AnalyzeE1(SemanticData *self);
char AnalyzeE2(SemanticData *self);
char AnalyzeE3(SemanticData *self);
char AnalyzeSubroutineCall(SemanticData *self);
char AnalyzeTerm(SemanticData *self);
//char AnalyzeOperator(SemanticData *self); Redundant
char AnalyzeExpressionList(SemanticData *self);

//Helper functions
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
RoutineKind getRoutineKindOfNode(const NodeAST *routine_kind_node);
char* getRoutineType(const NodeAST *routine_type_node, const ClassTable *class_table);
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
    const char* name = self->current->token->info.identifier;
    const char* type = typeOf(self->symbol_table,name,strlen(name));
    if (strcmp(type,"") == 0) {
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
        if (strcmp(type,"Array")) {
            reportError(self,"Can't index non-array data type",node->children[1]->token->line);
            return 0;
        }
        //Verify expression to be a valid index
        self->current = node->children[3];
        if (!AnalyzeExpression(self)) {
            return 0;
        }
        if (strcmp(type,"int")) {
            reportError(self,"Expected integer as index for array",node->children[1]->token->line);
            return 0;
        }
        //If array is shorter than the index -> illegal indexing
        if (lengthOf(self->symbol_table,name,strlen(name)) < self->current->token->info.intVal) {
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

    if ((strcmp(type,"Array")==0 && strcmp(expression_node->dataType,"int")) ||
        strcmp(expression_node->dataType,type)) {
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
    if (strcmp(self->current->dataType,"boolean")) {
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
    if (strcmp(self->current->dataType,"boolean")) {
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
    if (strcmp(type_routine,"void") == 0) {
        reportError(self,"Expected no return type for void function",self->current->token->line);
        return 0;
    }
    if (strcmp(type_routine,self->current->dataType)) {
        reportError(self,
            strcat(strcat("expected ",type_routine)," return type"),
            self->current->token->line);
        return 0;
    }
    self->current = node;
    return 1;
}

char AnalyzeExpression(SemanticData *self) {
    NodeAST *node = self->current;
    //E1 & E1 -> Boolean, E1 | E1 -> Boolean, E1 -> Data type of E1
    self->current = node->children[0];
    if (!AnalyzeE1(self)) {
        return 0;
    }
    if (node->currChildIndex == 1) {
        node->dataType = self->current->dataType;
        self->current = node;
        return 1;
    }
    if (strcmp(self->current->dataType,"boolean")) {
        reportError(self,"Expected boolean value for expression",node->children[1]->token->line);
        return 0;
    }
    self->current = node->children[2];
    if (!AnalyzeE1(self)) {
        return 0;
    }
    if (strcmp(self->current->dataType,"boolean")) {
        reportError(self,"Expected boolean value for expression",node->children[1]->token->line);
        return 0;
    }
    strncpy(node->dataType,"boolean",self->dt_size);
    self->current = node;
    return 1;
}
char AnalyzeE1(SemanticData *self) {
    NodeAST *node = self->current;
    //E2 < E2 -> Boolean, E2 > E2 -> Boolean,E2=E2 -> Boolean, E2 -> Data type of E2
    NodeAST *operand1 = node->children[0];
    NodeAST *operand2 = node->children[2];
    self->current = operand1;
    if (!AnalyzeE2(self)) {
        return 0;
    }
    if (node->currChildIndex == 1) {
        node->dataType = self->current->dataType;
        self->current = node;
        return 1;
    }
    self->current = operand2;
    if (!AnalyzeE2(self)) {
        return 0;
    }
    const Token *operator_token = node->children[1]->token;
    if (operand1->dataType != operand2->dataType) {
        reportError(self,"Mismatched data types",operator_token->line);
        return 0;
    }
    if (operator_token->info.symbol != '=' && strcmp(operand1->dataType,"int")) {
        reportError(self,"Expected int data type",operator_token->line);
        return 0;
    }
    strncpy(node->dataType,"boolean",self->dt_size);
    self->current = node;
    return 1;
}
char AnalyzeE2(SemanticData *self) {
    NodeAST *node = self->current;
    //E3 + E3 -> int, E3 - E3 -> int,E2=E2 -> Boolean, E3 -> Data type of E3
    self->current = node->children[0];
    if (!AnalyzeE3(self)) {
        return 0;
    }
    if (node->currChildIndex == 1) {
        node->dataType = self->current->dataType;
        self->current = node;
        return 1;
    }

    if (strcmp(self->current->dataType,"int")) {
        reportError(self,"Expected int value for expression",node->children[1]->token->line);
        return 0;
    }
    self->current = node->children[2];
    if (!AnalyzeE2(self)) {
        return 0;
    }
    if (strcmp(self->current->dataType,"int")) {
        reportError(self,"Expected int value for expression",node->children[1]->token->line);
        return 0;
    }
    strncpy(node->dataType,"int",self->dt_size);
    self->current = node;
    return 1;
}
char AnalyzeE3(SemanticData *self) {
    NodeAST *node = self->current;
    //term * term -> int, term / term -> int, term -> Data type of term
    self->current = node->children[0];
    if (!AnalyzeTerm(self)) {
        return 0;
    }
    if (node->currChildIndex == 1) {
        node->dataType = self->current->dataType;
        self->current = node;
        return 1;
    }

    if (strcmp(self->current->dataType,"int")) {
        reportError(self,"Expected int value for expression",node->children[1]->token->line);
        return 0;
    }
    self->current = node->children[2];
    if (!AnalyzeTerm(self)) {
        return 0;
    }
    if (strcmp(self->current->dataType,"int")) {
        reportError(self,"Expected int value for expression",node->children[1]->token->line);
        return 0;
    }
    strncpy(node->dataType,"boolean",self->dt_size);
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
            node->dataType = self->current->dataType;
            self->current = node;
            return res;
        }

        case NODE_SUBROUTINE_CALL: {
            self->current = node->children[0];
            const char res = AnalyzeSubroutineCall(self);
            node->dataType = self->current->dataType;
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