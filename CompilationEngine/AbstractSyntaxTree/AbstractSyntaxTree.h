//
// Created by Owner on 17/03/2026.
//
#ifndef ABSTRACTSYNTAXTREE_H
#define ABSTRACTSYNTAXTREE_H
#include "../Token/Token.h"
#include "../DataType.h"
#include "../../JackTokenizer/jackTokenizer.h"
#include <stdlib.h>

typedef enum ASTnodeType {
    NODE_ROOT, NODE_CLASS_VAR_DEC, NODE_TYPE, NODE_SUBROUTINE_DEC, NODE_PARAMETER_LIST,
    NODE_SUBROUTINE_BODY, NODE_VAR_DEC, NODE_STATEMENTS, NODE_STATEMENT, NODE_LET_STATEMENT,
    NODE_IF_STATEMENT, NODE_WHILE_STATEMENT, NODE_DO_STATEMENT, NODE_RETURN_STATEMENT,
    NODE_EXPRESSION, NODE_E1, NODE_E2, NODE_E3, NODE_TERM, NODE_SUBROUTINE_CALL, NODE_EXPRESSION_LIST,
    NODE_OP,NODE_UNARY_OP, NODE_KEYWORD_CONSTANT,NODE_KEYWORD, NODE_SYMBOL, NODE_INTEGER_CONSTANT,
    NODE_STRING_CONSTANT, NODE_IDENTIFIER
} ASTnodeType;
typedef struct AbstractSyntaxTreeNode {
    ASTnodeType nodeType;
    struct AbstractSyntaxTreeNode *parent;
    struct AbstractSyntaxTreeNode **children;
    int currChildIndex;
    size_t childrenCount;
    Token *token;
    char *dataType;
} NodeAST;
NodeAST* construct_ast_node(const ASTnodeType nodeType,NodeAST *parent,
    const size_t childrenCount, Token *token, const size_t dt_size);
void destory_node(NodeAST *node);
void token_ast_node(JackTokenizer* tokenizer,NodeAST* ast_curr);
void ast_node(NodeAST* ast_curr,const ASTnodeType type, const size_t childrenCount);
#endif //ABSTRACTSYNTAXTREE_H
