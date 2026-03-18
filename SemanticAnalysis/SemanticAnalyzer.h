//
// Created by Owner on 18/03/2026.
//

#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H
#include "../CompilationEngine/AbstractSyntaxTree/AbstractSyntaxTree.h"
#include "RoutineTable/RoutineTable.h"
#include "SymbolTable/SymbolTable.h"

typedef struct SemanticData {
    NodeAST *root;
    NodeAST *current;
    SymbolTable *symbol_table;
    size_t symbol_table_size;
    RoutineTable *routine_table;
    size_t routine_table_size;
    unsigned char isError;
    char *error;
    size_t error_size;
} SemanticData;

#endif //SEMANTICANALYZER_H
