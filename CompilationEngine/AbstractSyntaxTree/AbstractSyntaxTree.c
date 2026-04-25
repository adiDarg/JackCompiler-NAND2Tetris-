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
ASTnodeType getNodeTypeFromTokenType(TokenType type) {
    switch (type) {
        case TT_KEYWORD: {
            type = NODE_KEYWORD;
            break;
        }
        case TT_SYMBOL: {
            type = NODE_SYMBOL;
            break;
        }
        case TT_EOF_TOKEN: {
            type = NODE_EOF_TOKEN;
            break;
        }
        case TT_INT_CONST: {
            type = NODE_INTEGER_CONSTANT;
            break;
        }
        case TT_IDENTIFIER: {
            type = NODE_IDENTIFIER;
            break;
        }
        case TT_STRING_CONST: {
            type = NODE_STRING_CONSTANT;
            break;
        }
        case TT_UNKNOWN_TOKEN: {
            type = NODE_UNKNOWN;
            break;
        }
    }
    return type;
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
    const ASTnodeType type = getNodeTypeFromTokenType(token->type);

    NodeAST *node = construct_ast_node(type,ast_curr,0,token,tokenizer->dt_size);
    ast_curr->children[ast_curr->currChildIndex] = node;
    ast_curr->currChildIndex++;
}
void ast_node(NodeAST* ast_curr, const ASTnodeType type, const size_t childrenCount,const size_t dt_size) {
    while (ast_curr->currChildIndex >= ast_curr->childrenCount) {
        ast_curr->childrenCount *= 2;
        ast_curr->children = realloc(ast_curr->children,ast_curr->childrenCount * sizeof(NodeAST*));
        if (ast_curr->children == NULL) {
            printf("Unable to allocate children for AST");
            return;
        }
    }
    NodeAST *node = construct_ast_node(type,ast_curr,childrenCount,NULL,dt_size);
    ast_curr->children[ast_curr->currChildIndex] = node;
    ast_curr->currChildIndex++;
}