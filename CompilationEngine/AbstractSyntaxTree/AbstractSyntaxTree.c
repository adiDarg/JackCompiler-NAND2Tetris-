#include "AbstractSyntaxTree.h"

#include <stdio.h>
void printNode(const NodeAST* node);
NodeAST* construct_ast_node(const ASTnodeType nodeType,NodeAST *parent,
                            const size_t childrenCount, Token *token,const size_t dt_size) {
    NodeAST *node = malloc(sizeof(NodeAST));
    node->nodeType = nodeType;
    node->childrenCount = childrenCount;
    node->children = malloc(childrenCount * sizeof(NodeAST*));
    node->parent = parent;
    node->token = token;
    node->currChildIndex = 0;
    node->dataType = malloc(dt_size);
    return node;
}
void destory_node(NodeAST *node) {
    if (node == NULL) return;

    if (node->children != NULL) {
        for (size_t i = 0; i < node->currChildIndex; i++) {
            destory_node(node->children[i]);
        }
        free(node->children);
    }

    if (node->token != NULL) {
        free(node->token);
    }

    free(node);
}
void printNode(const NodeAST* node) {
    printf("Node:\nType:%d\nChildren count:%d\nToken:%d\n\n",
        node->nodeType,(int)node->childrenCount,node->token == NULL? -1: node->token->type);
}
void token_ast_node(JackTokenizer* tokenizer,NodeAST* ast_curr) {
    while (ast_curr->currChildIndex >= ast_curr->childrenCount) {
        ast_curr->childrenCount *= 2;
        ast_curr->children = realloc(ast_curr->children,ast_curr->childrenCount * sizeof(NodeAST*));
        if (ast_curr->children == NULL) {
            printf("Unable to allocate children for AST");
            return;
        }
    }
    Token *token = createToken(tokenizer);
    NodeAST *node = construct_ast_node(NODE_KEYWORD,ast_curr,0,token);
    ast_curr->children[ast_curr->currChildIndex] = node;
    ast_curr->currChildIndex++;
}
void ast_node(NodeAST* ast_curr, const ASTnodeType type, const size_t childrenCount) {
    while (ast_curr->currChildIndex >= ast_curr->childrenCount) {
        ast_curr->childrenCount *= 2;
        ast_curr->children = realloc(ast_curr->children,ast_curr->childrenCount);
        if (ast_curr->children == NULL) {
            printf("Unable to allocate children for AST");
            return;
        }
    }
    NodeAST *node = construct_ast_node(type,ast_curr,childrenCount,NULL);
    ast_curr->children[ast_curr->currChildIndex] = node;
    ast_curr->currChildIndex++;
}