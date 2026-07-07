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
    code_gen_info->labelCount = 0;
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
void GenerateSubroutineCall(CodeGenInfo *self);
void GenerateE1(CodeGenInfo *self);
void GenerateE2(CodeGenInfo *self);
void GenerateE3(CodeGenInfo *self);
void GenerateTerm(CodeGenInfo *self);

Segment getSegmentFromSK(const SymbolKind kind){
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
Command getCommandFromSymbol(const char symbol) {
    switch (symbol) {
        case '=': return CMD_EQ;
        case '>': return CMD_GT;
        case '<': return CMD_LT;
        case '|': return CMD_OR;
        case '+': return CMD_ADD;
        case '&': return CMD_AND;
        case '~': return CMD_NOT;
        case '-': return CMD_SUB;
        default: return CMD_UNKNOWN;
    }
}
void generateIdentifierPop(const char *identifier,const int length, const CodeGenInfo *self) {
    const SymbolKind kind = kindOf(self->symbol_table,identifier,length);
    const int index = indexOf(self->symbol_table,identifier,length);
    writePop(self->vm_writer,getSegmentFromSK(kind),index);
}
void generateIdentifierPush(const char *identifier,const int length, const CodeGenInfo *self) {
    const SymbolKind kind = kindOf(self->symbol_table,identifier,length);
    const int index = indexOf(self->symbol_table,identifier,length);
    writePush(self->vm_writer,getSegmentFromSK(kind),index);
}
char* generate_label(CodeGenInfo *self) {
    // Determine required size: base + underscore + ~10 digits + null terminator
    int size = snprintf(NULL, 0, "%s_%d", self->class, self->labelCount) + 1;
    if (size <= 0) return NULL;

    char* buffer = malloc(size);
    if (buffer) {
        snprintf(buffer, size, "%s_%d", self->class, self->labelCount++);
    }
    return buffer;
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
void GenerateIfStatement(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    self->curr_tree_node = node->children[2];
    GenerateExpression(self);
    writeArithmetic(self->vm_writer,CMD_NOT);
    char *label1 = generate_label(self);
    char *label2 = generate_label(self);

    writeIf(self->vm_writer,label2);

    self->curr_tree_node = node->children[5];
    GenerateStatements(self);

    writeLabel(self->vm_writer,label2);
    if (node->currChildIndex == 11) {
        self->curr_tree_node = node->children[9];
        GenerateStatements(self);
    }

    writeGoTo(self->vm_writer,label1);

    self->curr_tree_node = node;
    free(label1);
    free(label2);
}
void GenerateWhileStatement(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    //Get labels for loop
    char *label1 = generate_label(self);
    char *label2 = generate_label(self);
    //Enter loop
    writeLabel(self->vm_writer,label1);

    //Check loop condition
    self->curr_tree_node = node->children[2];
    GenerateExpression(self);
    writeArithmetic(self->vm_writer,CMD_NOT);
    writeIf(self->vm_writer,label2);

    //Write loop contents
    self->curr_tree_node = node->children[5];
    GenerateStatements(self);
    //Go back to loop start
    writeGoTo(self->vm_writer,label1);

    //Write exit label
    writeLabel(self->vm_writer,label2);
    free(label1);
    free(label2);
    self->curr_tree_node = node;
}
void GenerateDoStatement(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    self->curr_tree_node = node->children[1];
    GenerateSubroutineCall(self);
    writePop(self->vm_writer,SEG_TEMP,0);
    self->curr_tree_node = node;
}
void GenerateReturnStatement(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    if (node->children[1]->nodeType == NODE_EXPRESSION) {
        self->curr_tree_node = node->children[1];
        GenerateExpression(self);
    }
    else {
        writePush(self->vm_writer,SEG_CONST,0);
    }
    writeReturn(self->vm_writer);
    self->curr_tree_node = node;
}
void GenerateExpression(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    for (int i = 0; i < node->currChildIndex; i+=2) {
        self->curr_tree_node = node->children[i];
        GenerateE1(self);
        if (i+1 < node->currChildIndex) {
            const NodeAST *operand = node->children[i+1];
            const char symbol = operand->token->info.symbol;
            const Command command = getCommandFromSymbol(symbol);
            writeArithmetic(self->vm_writer,command);
        }
    }
    self->curr_tree_node = node;
}
void GenerateSubroutineCall(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    char *routine_name;
    int expressionListIndex;
    if (node->children[1]->token->info.symbol == '.') {
        expressionListIndex = 4;

        const char *object = node->children[0]->token->info.identifier;
        const char *function = node->children[2]->token->info.identifier;
        size_t size = strlen(object) + strlen(function) + 2;
        routine_name = malloc(size);
        strcpy_s(routine_name,size,object);
        strcat_s(routine_name,size,".");
        strcat_s(routine_name,size,function);
    }
    else {
        expressionListIndex = 2;
        const char *function = node->children[0]->token->info.identifier;
        routine_name = _strdup(function);
    }

    const NodeAST *expression_list = node->children[expressionListIndex];
    for (int i = 0; i < expression_list->currChildIndex; i++) {
        self->curr_tree_node = expression_list->children[i];
        GenerateExpression(self);
    }

    writeCall(self->vm_writer,routine_name,expression_list->currChildIndex);

    self->curr_tree_node = node;
}
void GenerateE1(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    for (int i = 0; i < node->currChildIndex; i+=2) {
        self->curr_tree_node = node->children[i];
        GenerateE2(self);
        if (i+1 < node->currChildIndex) {
            const NodeAST *operand = node->children[i+1];
            const char symbol = operand->token->info.symbol;
            const Command command = getCommandFromSymbol(symbol);
            writeArithmetic(self->vm_writer,command);
        }
    }
    self->curr_tree_node = node;
}
void GenerateE2(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    for (int i = 0; i < node->currChildIndex; i+=2) {
        self->curr_tree_node = node->children[i];
        GenerateE3(self);
        if (i+1 < node->currChildIndex) {
            const NodeAST *operand = node->children[i+1];
            const char symbol = operand->token->info.symbol;
            const Command command = getCommandFromSymbol(symbol);
            writeArithmetic(self->vm_writer,command);
        }
    }
    self->curr_tree_node = node;
}
void GenerateE3(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    for (int i = 0; i < node->currChildIndex; i+=2) {
        self->curr_tree_node = node->children[i];
        GenerateTerm(self);
        if (i+1 < node->currChildIndex) {
            const NodeAST *operand = node->children[i+1];
            const char symbol = operand->token->info.symbol;
            const Command command = getCommandFromSymbol(symbol);
            writeArithmetic(self->vm_writer,command);
        }
    }
    self->curr_tree_node = node;
}
void GenerateTerm(CodeGenInfo *self) {
    NodeAST *node = self->curr_tree_node;
    switch (node->children[0]->nodeType) {
        case NODE_UNARY_OP: {
            const Command command = getCommandFromSymbol(node->children[0]->token->info.symbol);
            writeArithmetic(self->vm_writer,command);
            self->curr_tree_node = node->children[1];
            GenerateTerm(self);
            self->curr_tree_node = node;
            break;
        }

        case NODE_SUBROUTINE_CALL: {
            node->children[0];
            GenerateSubroutineCall(self);
            self->curr_tree_node = node;
            break;
        }

        case NODE_INTEGER_CONSTANT: {
            writePush(self->vm_writer,SEG_CONST,node->children[0]->token->info.intVal);
            break;
        }
        case NODE_STRING_CONSTANT: {
            const char* string = node->children[0]->token->info.stringVal;
            writePush(self->vm_writer,SEG_CONST,strlen(string));
            writeCall(self->vm_writer,"String.new",1);
            int i = 0;
            while (string[i++] != '\0') {
                writePush(self->vm_writer,SEG_CONST,string[i]);
                writeCall(self->vm_writer,"String.appendChar",2);
            }
            break;
        }
        case NODE_KEYWORD: {
            const Keyword keyword = node->children[0]->token->info.keyword;
            switch (keyword) {
                case KW_TRUE: {
                    writePush(self->vm_writer,SEG_CONST,1);
                    writeArithmetic(self->vm_writer,CMD_NEG);
                    break;
                }
                case KW_FALSE: case KW_NULL: {
                    writePush(self->vm_writer,SEG_CONST,0);
                    break;
                }
                case KW_THIS: {
                    writePush(self->vm_writer,SEG_POINTER,0);
                    break;
                }
                default: {
                    break;
                }
            }
        }

        case NODE_IDENTIFIER: {
            const char *identifier = node->children[0]->token->info.identifier;
            const Segment segment = getSegmentFromSK(kindOf(self->symbol_table,identifier,strlen(identifier)));
            const int index = indexOf(self->symbol_table,identifier,strlen(identifier));
            writePush(self->vm_writer,segment,index);
            break;
        }

        case NODE_SYMBOL: {
            self->curr_tree_node = node->children[1];
            GenerateExpression(self);
            break;
        }

        default: {
            break;
        }
    }
    self->curr_tree_node = node;
}