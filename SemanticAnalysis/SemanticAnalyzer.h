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
    NodeAST *root; //8
    NodeAST *current; //8
    SymbolTable *symbol_table; //8
    RoutineTable *routine_table; //8
    ClassTable *class_table; //8
    char *error; //8
    size_t error_size; //8
    size_t dt_size; //8
    unsigned char isError; //1
} SemanticData;
void LoadFileToTables(const SemanticData *self);
SemanticData* construct_semantic_data(NodeAST *root,const size_t errorSize,
    const size_t symbolTableSize, RoutineTable *routineTable, ClassTable *classTable,
    const size_t dt_size);
char Analyze(SemanticData *self);
void destroySemanticData(SemanticData *self,const char fullDest);
#endif //SEMANTICANALYZER_H
