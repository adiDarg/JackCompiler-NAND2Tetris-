//
// Created by Owner on 18/12/2025.
//

#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H
#include "jackTokenizer.h"
#include <stdio.h>
typedef struct {
    JackTokenizer* jack_tokenizer;
    char *out;
    size_t len;
    size_t cap;
    char error[100];
    int tab;
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
