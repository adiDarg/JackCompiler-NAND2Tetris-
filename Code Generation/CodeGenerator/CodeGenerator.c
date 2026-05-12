//
// Created by Owner on 09/05/2026.
//

#include "CodeGenerator.h"

#include <stdlib.h>
#include <string.h>

#include "../VMWriter/VMWriter.h"

CodeGenInfo *ConstructCodeGenerator(const char *destination, const size_t dest_size, NodeAST *root,
    SymbolTable *symbol_table, RoutineTable *routine_table, ClassTable *class_table) {
    CodeGenInfo *code_gen_info = malloc(sizeof(CodeGenInfo));
    code_gen_info->vm_writer = vm_constructor(destination,dest_size);
    code_gen_info->root = root;
    code_gen_info->curr_tree_node = root;
    code_gen_info->class = NULL;
    code_gen_info->symbol_table = symbol_table;
    code_gen_info->class_table = class_table;
    code_gen_info->routine_table = routine_table;
    return code_gen_info;
}
void GenerateClass(CodeGenInfo *self);
void GenerateSubroutineDec(CodeGenInfo *self);
void GenerateSubroutineBody(CodeGenInfo *self);
void GenerateStatements(CodeGenInfo *self);
void GenerateLetStatement(CodeGenInfo *self);
void GenerateIfStatement(CodeGenInfo *self);
void GenerateWhileStatement(CodeGenInfo *self);
void GenerateDoStatement(CodeGenInfo *self);
void GenerateReturnStatement(CodeGenInfo *self);
void GenerateExpression(CodeGenInfo *self);

Segment getSegmentFromSK(const SymbolKind kind) {
    Segment segment = SEG_NONE;
    switch (kind) {
        case SK_ARG: {
            segment = SEG_ARG;
            break;
        }
        case SK_VAR: {
            segment = SEG_LOCAL;
            break;
        }
        case SK_STATIC: {
            segment = SEG_STATIC;
            break;
        }
        case SK_FIELD: {
            segment = SEG_THIS;
            break;
        }
        case SK_NONE: {
            break;
        }
    }
    return segment;
}
void generateIdentifierPop(const char *identifier,const int length, const CodeGenInfo *self) {
    const SymbolKind kind = kindOf(self->symbol_table,identifier,length);
    const int index = indexOf(self->symbol_table,identifier,length);
    writePop(self->vm_writer,getSegmentFromSK(kind),index);
}
void generateIdentifierPush(const char *identifier,const int length, const CodeGenInfo *self) {
    const SymbolKind kind = kindOf(self->symbol_table,identifier,length);
    const int index = indexOf(self->symbol_table,identifier,length);
    writePop(self->vm_writer,getSegmentFromSK(kind),index);
}

void GenerateCode(CodeGenInfo *self) {
    GenerateClass(self);
}
void GenerateClass(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    self->class = _strdup(node->children[1]->token->info.identifier);
    for (int i = 0; i < node->currChildIndex; i++) {
        if (node->children[i]->nodeType == NODE_SUBROUTINE_DEC) {
            self->curr_tree_node = node->children[i];
            GenerateSubroutineDec(self);
        }
    }
    self->curr_tree_node = node;
}
void GenerateSubroutineDec(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    NodeAST *subroutineBody = node->children[6];
    //currChildIndex = number of children - 1. Number of varDecs = number of children - 3 ('{',statements,'}')
    const int nLocals = subroutineBody->currChildIndex - 2;
    writeFunction(self->vm_writer,
        node->children[2]->token->info.identifier,
        self->class,
        nLocals);
    self->curr_tree_node = subroutineBody;
    GenerateSubroutineBody(self);
    self->curr_tree_node = node;
}
void GenerateSubroutineBody(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    self->curr_tree_node = node->children[2];
    GenerateStatements(self);
    self->curr_tree_node = node;
}
void GenerateStatements(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    for (int i = 0; i < node->currChildIndex; i++) {
        self->curr_tree_node = node->children[i];
        switch (self->curr_tree_node->nodeType) {
            case NODE_LET_STATEMENT: {
                GenerateLetStatement(self);
                break;
            }
            case NODE_DO_STATEMENT: {
                GenerateDoStatement(self);
                break;
            }
            case NODE_IF_STATEMENT: {
                GenerateIfStatement(self);
                break;
            }
            case NODE_WHILE_STATEMENT: {
                GenerateWhileStatement(self);
                break;
            }
            case NODE_RETURN_STATEMENT: {
                GenerateReturnStatement(self);
                break;
            }
            default: {
                break;
            }
        }
    }
    self->curr_tree_node = node;
}
void GenerateLetStatement(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    //let varName = expression | let varName[index] = expression
    const char *varName = node->children[1]->token->info.identifier;

    //is it an indexed expression
    int expIndex = 3;
    if (node->children[2]->token->info.symbol == '[') {
        self->curr_tree_node = node->children[3];
        GenerateExpression(self);
        generateIdentifierPush(varName,strlen(varName),self);
        writeArithmetic(self->vm_writer,CMD_ADD);
        writePop(self->vm_writer,SEG_POINTER,1);
        expIndex = 6;
    }

    self->curr_tree_node = node->children[expIndex];
    GenerateExpression(self);

    if (node->children[2]->token->info.symbol == '[') {
        writePop(self->vm_writer,SEG_THAT,0);
    }
    else {
        generateIdentifierPop(varName,strlen(varName),self);
    }

    self->curr_tree_node = node;
}