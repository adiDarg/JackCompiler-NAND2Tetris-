//
// Created by Owner on 03/05/2026.
//

#ifndef FUNCTIONLOADER_H
#define FUNCTIONLOADER_H
#include "../RoutineTable/RoutineTable.h"
#include "../../CompilationEngine/AbstractSyntaxTree/AbstractSyntaxTree.h"
#include "../ClassTable/ClassTable.h"

void LoadFunctionsToSymbolTable(const NodeAST *node,const char *class,
    RoutineTable *table, ClassTable *class_table);
char* getRoutineType(const NodeAST *routine_type_node, const ClassTable *class_table);
#endif //FUNCTIONLOADER_H
