//
// Created by Owner on 18/03/2026.
//

#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H
#include "../CompilationEngine/AbstractSyntaxTree/AbstractSyntaxTree.h"
#include "ClassTable/ClassTable.h"
#include "RoutineTable/RoutineTable.h"
#include "SymbolTable/SymbolTable.h"

typedef struct SemanticData {
    NodeAST *root;
    NodeAST *current;
    SymbolTable *symbol_table;
    RoutineTable *routine_table;
    ClassTable *class_table;
    unsigned char isError;
    char *error;
    size_t error_size;
    size_t dt_size;
} SemanticData;

#endif //SEMANTICANALYZER_H
