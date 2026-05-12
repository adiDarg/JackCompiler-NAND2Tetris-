//
// Created by Owner on 09/05/2026.
//

#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H
#include "../VMWriter/VMWriter.h"
#include "../../CompilationEngine/AbstractSyntaxTree/AbstractSyntaxTree.h"
#include "../../SemanticAnalysis/ClassTable/ClassTable.h"
#include "../../SemanticAnalysis/SymbolTable/SymbolTable.h"
#include "../../SemanticAnalysis/RoutineTable/RoutineTable.h"
typedef struct {
    NodeAST *root; //8
    NodeAST *curr_tree_node; //8
    VMWriter *vm_writer; //8
    SymbolTable *symbol_table; //8
    RoutineTable *routine_table; //8
    ClassTable *class_table; //8
    char *class; //8
} CodeGenInfo;
#endif //CODEGENERATOR_H
