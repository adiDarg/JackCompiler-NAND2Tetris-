#include "AbstractSyntaxTree.h"

#include <stdio.h>
void printNode(const NodeAST* node);
NodeAST* construct_ast_node(const ASTnodeType nodeType,NodeAST *parent,
                            const size_t childrenCount, Token *token,const size_t dt_size) {
    NodeAST *node = calloc(1, sizeof(NodeAST));
    if (node == NULL)
        return NULL;

    node->nodeType = nodeType;
    node->childrenCount = childrenCount;
    node->parent = parent;
    node->token = token;
    node->currChildIndex = 0;

    node->children = calloc(node->childrenCount, sizeof(NodeAST*));
    if (dt_size > 0) {
        node->dataType = calloc(dt_size, 1);
    }
    return node;
}
void destory_node(NodeAST *node) {
    if (node == NULL) return;

    if (node->children != NULL) {
        for (size_t i = 0; i < node->childrenCount; i++) {
            if (node->children[i] != NULL) {
                destory_node(node->children[i]);
            }
        }
        free(node->children);
    }

    if (node->token != NULL) {
        free(node->token);
    }
    if (node->dataType != NULL) {
        free(node->dataType);
    }
    free(node);
}
void printNode(const NodeAST* node) {
    printf("Node:\nType:%d\nChildren count:%d\nToken:%d\n\n",
        node->nodeType,(int)node->childrenCount,node->token == NULL? -1: node->token->type);
}
void printHeapStatus() {
    const int heapstatus = _heapchk();
    switch( heapstatus )
    {
        case _HEAPOK:
            printf(" OK - heap is fine\n" );
        break;
        case _HEAPEMPTY:
            printf(" OK - heap is empty\n" );
        break;
        case _HEAPBADBEGIN:
            printf( "ERROR - bad start of heap\n" );
        break;
        case _HEAPBADNODE:
            printf( "ERROR - bad node in heap\n" );
        break;
        case _HEAPBADPTR:
            printf( "ERROR - bad pointer in heap\n" );
        break;
        default:
            break;
    }
}
ASTnodeType getNodeTypeFromTokenType(CompilerTokenType type) {
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
        ast_curr->childrenCount = (ast_curr->childrenCount == 0) ? 1 : ast_curr->childrenCount * 2;
        ast_curr->children = realloc(ast_curr->children,ast_curr->childrenCount * sizeof(NodeAST*));
        if (ast_curr->children == NULL) {
            printf("Unable to allocate children for AST - token\n");
            printf("Line: %d\n",tokenizer->line);
            printHeapStatus();
            exit(EXIT_FAILURE);
        }
        for (int i = ast_curr->currChildIndex; i < ast_curr->childrenCount; i++) {
            ast_curr->children[i] = NULL;
        }
    }
    Token *token = createToken(tokenizer);
    const ASTnodeType type = getNodeTypeFromTokenType(token->type);

    NodeAST *node = construct_ast_node(type,ast_curr,0,token,tokenizer->dt_size);
    ast_curr->children[ast_curr->currChildIndex++] = node;
}
void ast_node(NodeAST* ast_curr, const ASTnodeType type, const size_t childrenCount,const size_t dt_size) {
    while (ast_curr->currChildIndex >= ast_curr->childrenCount) {
        ast_curr->childrenCount = (ast_curr->childrenCount == 0) ? 1 : ast_curr->childrenCount * 2;
        ast_curr->children = realloc(ast_curr->children,ast_curr->childrenCount * sizeof(NodeAST*));
        if (ast_curr->children == NULL) {
            printf("Unable to allocate children for AST\n");
            if (ast_curr->token != NULL) {
                printf("Line: %d\n",ast_curr->token->line);
            }
            printHeapStatus();
            exit(EXIT_FAILURE);
        }
        for (int i = ast_curr->currChildIndex; i < ast_curr->childrenCount; i++) {
            ast_curr->children[i] = NULL;
        }
    }
    NodeAST *node = construct_ast_node(type,ast_curr,childrenCount,NULL,dt_size);
    ast_curr->children[ast_curr->currChildIndex] = node;
    ast_curr->currChildIndex++;
}