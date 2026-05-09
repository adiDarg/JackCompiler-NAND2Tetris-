//
// Created by Owner on 18/12/2025.
//

#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H
#include "../JackTokenizer/jackTokenizer.h"
#include "AbstractSyntaxTree/AbstractSyntaxTree.h"
typedef struct {
    char error[100]; //100
    JackTokenizer* jack_tokenizer; //8
    char *out; //8
    //Abstract Syntax Tree root and current nodes
    NodeAST *ast_root; //8
    NodeAST *ast_curr; //8
    size_t len; //8
    size_t cap; //8
    size_t dt_size; //8
    size_t error_size; //8
    int tab; //4
}CompilationEngine;
CompilationEngine* Construct_Engine(JackTokenizer* jack_tokenizer);
int CompileClass(CompilationEngine *self);
int CompileClassVarDec(CompilationEngine *self);
int CompileSubroutineDec(CompilationEngine *self);
int CompileParameterList(CompilationEngine *self);
int CompileVarDec(CompilationEngine *self);
int CompileStatements(CompilationEngine *self);
int CompileDo(CompilationEngine *self);
int CompileLet(CompilationEngine *self);
int CompileWhile(CompilationEngine *self);
int CompileReturn(CompilationEngine *self);
int CompileIf(CompilationEngine *self);
int CompileExpression(CompilationEngine *self);
int CompileTerm(CompilationEngine *self);
int CompileSubroutineCall(CompilationEngine* self);
int CompileExpressionList(CompilationEngine *self);

#endif //COMPILATIONENGINE_H
