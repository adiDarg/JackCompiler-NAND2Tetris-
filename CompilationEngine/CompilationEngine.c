//
// Created by Owner on 18/12/2025.
//

#include "CompilationEngine.h"

#include <stdio.h>

#include "../JackTokenizer/jackTokenizer.h"
#include "../JackTokenizer/Keyword/keyword.h"

#include <stdlib.h>
#include <string.h>

CompilationEngine* Construct_Engine(JackTokenizer* jack_tokenizer) {
    CompilationEngine* self = malloc(sizeof(CompilationEngine));
    self->jack_tokenizer = jack_tokenizer;
    self->out = malloc(8192);
    self->out[0] = '\0';
    self->tab=0;
    self->cap=8192;
    self->len = 0;
    self->error[0] = '\0';
    return self;
}
void writeOut(CompilationEngine* self,const char str[]) {
    const size_t n = strlen(str);
    if (self->len + self->tab * 2 + n + 1 > self->cap) {
        self->cap += n + 1 + self->tab * 2;
        self->cap *= 2;
        char *tmp = realloc(self->out, self->cap);
        if (!(int)tmp) {
            printf("Failed to alocate for output");
            return;
        };
        self->out = tmp;
    }
    for (int i = 0; i < self->tab; i++) {
        strcat(self->out,"  ");
    }
    self->len += self->tab * 2;
    strncat(self->out,str,n);
    self->len += n;
}
int compileKeyword(CompilationEngine* self, Keyword kw) {
    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    if (tokenType(tokenizer) != TT_KEYWORD || keyword(tokenizer) != kw) {
        sprintf(self->error,"line %d: Expected %s",self->jack_tokenizer->line,keyword_to_text(kw));
        return 0;
    }
    writeOut(self,"<keyword> ");
    const int temp = self->tab;
    self->tab = 0;
    writeOut(self,keyword_to_text(kw));
    writeOut(self," </keyword>\n");
    self->tab = temp;
    token_ast_node(tokenizer,self->ast_curr);
    advance(tokenizer);
    return 1;
}
int compileKeywords(CompilationEngine* self, const Keyword* arr,const int len){
    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (tokenType(tokenizer) != TT_KEYWORD) {
        snprintf(self->error,100,"line %d: Expected Keyword: %s",
            self->jack_tokenizer->line, keywords_to_text(arr,len));
        return 0;
    }
    const Keyword kw = keyword(tokenizer);
    for (int i = 0; i < len; i++) {
        if (kw == arr[i]) {
            writeOut(self,"<keyword> ");
            const int temp = self->tab;
            self->tab = 0;
            writeOut(self,keyword_to_text(kw));
            writeOut(self," </keyword>\n");
            self->tab = temp;
            token_ast_node(tokenizer,self->ast_curr);
            advance(tokenizer);
            strcpy(self->error,"\0");
            return 1;
        }
    }
    strcpy(self->error,"Expected keyword: ");
    strncat(self->error,keywords_to_text(arr,len),80);
    return 0;
}
int compileIdentifier(CompilationEngine* self) {
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,self->jack_tokenizer->error);
        return 0;
    }
    if (tokenType(self->jack_tokenizer) != TT_IDENTIFIER) {
        sprintf(self->error,"line %d: Expected Identifier",self->jack_tokenizer->line);
        return 0;
    }
    writeOut(self, "<identifier> ");
    const int temp = self->tab;
    self->tab = 0;
    writeOut(self, identifier(self->jack_tokenizer));
    writeOut(self," </identifier>\n");
    self->tab = temp;
    token_ast_node(self->jack_tokenizer,self->ast_curr);
    advance(self->jack_tokenizer);
    return 1;
}
int compileSymbol(CompilationEngine* self, char sym) {
    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    if (tokenType(tokenizer) != TT_SYMBOL || symbol(tokenizer) != sym) {
        sprintf(self->error,"line %d: Expected %c",self->jack_tokenizer->line,sym);
        return 0;
    }
    writeOut(self,"<symbol> ");
    const int temp = self->tab;
    self->tab = 0;
    char str[6];
    str[0] = symbol(self->jack_tokenizer);
    str[1] = '\0';
    switch (str[0]) {
        case '<':
            strcpy(str,"&lt;");
            break;
        case '>':
            strcpy(str,"&gt;");
            break;
        case '"':
            strcpy(str,"&quot;");
            break;
        case '&':
            strcpy(str,"&amp;");
            break;
        default:
            break;
    }
    writeOut(self,str);
    writeOut(self, " </symbol>\n");
    self->tab = temp;
    token_ast_node(tokenizer,self->ast_curr);
    advance(tokenizer);
    return 1;
}
int compileSymbols(CompilationEngine* self, const char* arr,const int len){
    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    if (tokenType(tokenizer) != TT_SYMBOL) {
        strcpy(self->error,"Expected symbol: ");
        for (int i = 0; i < len; i++) {
            char char_as_string[2];
            char_as_string[0] = arr[i];
            char_as_string[1] = '\0';
            strncat(self->error,char_as_string,2);
            strcat(self->error,",");
        }
        return 0;
    }
    const char ch = symbol(tokenizer);
    for (int i = 0; i < len; i++) {
        if (ch == arr[i]) {
            return compileSymbol(self,ch);
        }
    }
    strcpy(self->error,"Expected symbol: ");
    for (int i = 0; i < len; i++) {
        char char_as_string[2];
        char_as_string[0] = arr[i];
        char_as_string[1] = '\0';
        strncat(self->error,char_as_string,2);
        strcat(self->error,",");
    }
    return 0;
}
int compileIntConstant(CompilationEngine* self) {
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,self->jack_tokenizer->error);
        return 0;
    }
    if (tokenType(self->jack_tokenizer) != TT_INT_CONST) {
        sprintf(self->error,"Expected int constant");
        return 0;
    }
    writeOut(self, "<integerConstant> ");
    const int temp = self->tab;
    self->tab = 0;
    char str[20];
    sprintf(str, "%d", intVal(self->jack_tokenizer));
    writeOut(self, str);
    writeOut(self," </integerConstant>\n");
    self->tab = temp;
    token_ast_node(self->jack_tokenizer,self->ast_curr);
    advance(self->jack_tokenizer);
    return 1;
}
int compileStringConstant(CompilationEngine* self) {
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,self->jack_tokenizer->error);
        return 0;
    }
    if (tokenType(self->jack_tokenizer) != TT_STRING_CONST) {
        sprintf(self->error,"Expected string constant");
        return 0;
    }
    writeOut(self, "<stringConstant> ");
    const int temp = self->tab;
    self->tab = 0;
    writeOut(self, stringVal(self->jack_tokenizer));
    writeOut(self," </stringConstant>\n");
    self->tab = temp;
    token_ast_node(self->jack_tokenizer,self->ast_curr);
    advance(self->jack_tokenizer);
    return 1;
}
int compileType(CompilationEngine* self) {
    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    const int primitive = tokenType(tokenizer) == TT_KEYWORD &&
        ((keyword(tokenizer) == KW_INT) || (keyword(tokenizer) == KW_CHAR) || (keyword(tokenizer) == KW_BOOLEAN));
    const int className = tokenType(tokenizer) == TT_IDENTIFIER;
    if (!primitive && !className){
        sprintf(self->error,"line %d: Expected a type",self->jack_tokenizer->line);
        return 0;
    }
    if (primitive) {
        const Keyword arr[] = {KW_INT,KW_CHAR,KW_BOOLEAN};
        compileKeywords(self,arr,3);
    }
    else {
        compileIdentifier(self);
    }
    return 1;
}
int compileOperator(CompilationEngine* self) {
    const char symbols[] = {'+','-','*','/','&','|','<','>','='};
    return compileSymbols(self,symbols,9);
}
int compileUnaryOperator(CompilationEngine* self) {
    const char symbols[] = {'-','~'};
    return compileSymbols(self,symbols,2);
}
int isTerm(const CompilationEngine* self) {
    JackTokenizer* tokenizer = self->jack_tokenizer;
    const int constant = tokenType(tokenizer) == TT_INT_CONST || tokenType(tokenizer) == TT_STRING_CONST;
    const Keyword kw = keyword(tokenizer);
    const int keywordConstant = tokenType(tokenizer) == TT_KEYWORD &&
        (kw == KW_TRUE || kw == KW_FALSE || kw == KW_NULL || kw==KW_THIS);
    const int identifier = tokenType(tokenizer) == TT_IDENTIFIER;
    const char symb = symbol(tokenizer);
    const int symbols = tokenType(tokenizer) == TT_SYMBOL &&
        (symb == '(' || symb=='-' || symb=='~');
    return constant || keywordConstant || identifier || symbols;
}
int CompileClass(CompilationEngine *self) {
    writeOut(self,"<class>\n");
    self->tab++;

    self->ast_root = construct_ast_node(NODE_ROOT,NULL,4,NULL);
    self->ast_curr = self->ast_root;

    JackTokenizer* tokenizer = self->jack_tokenizer;
    advance(tokenizer);
    strcpy(tokenizer->error,"\0");
    tokenizer->isError = 0;

    if (!compileKeyword(self,KW_CLASS)) {
        return 0;
    }
    if (!compileIdentifier(self)) {
        return 0;
    }
    if (!compileSymbol(self,'{')) {
        return 0;
    }
    while (tokenType(tokenizer) == TT_KEYWORD &&
        (keyword(tokenizer) == KW_STATIC || keyword(tokenizer) == KW_FIELD)) {
        if (!CompileClassVarDec(self)) {
            return 0;
        }
    }
    while (tokenType(tokenizer) == TT_KEYWORD &&
    keyword(tokenizer) == KW_CONSTRUCTOR || keyword(tokenizer) == KW_FUNCTION || keyword(tokenizer) == KW_METHOD) {
        if (!CompileSubroutineDec(self)) {
            return 0;
        }
    }
    if (!compileSymbol(self,'}')) {
        return 0;
    }
    self->tab--;
    writeOut(self, "</class>\n");
    return 1;
}
int CompileClassVarDec(CompilationEngine* self) {
    writeOut(self,"<classVarDec>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_CLASS_VAR_DEC,self->ast_curr,4,NULL);
    self->ast_curr = node;

    const Keyword arr[] = {KW_STATIC,KW_FIELD};
    if (!compileKeywords(self, arr, 2)) {
        return 0;
    }
    if (!compileType(self)) {
        return 0;
    };
    if (!compileIdentifier(self)) {
        return 0;
    }
    while (compileSymbol(self,',')) {
        if (!compileIdentifier(self)) {
            return 0;
        }
    }
    compileSymbol(self,';');
    self->tab--;
    writeOut(self,"</classVarDec>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileSubroutineBody(CompilationEngine* self) {
    writeOut(self,"<subroutineBody>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_SUBROUTINE_BODY,self->ast_curr,3,NULL);
    self->ast_curr = node;

    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (!compileSymbol(self,'{')) {
        return 0;
    }
    while (tokenType(tokenizer) == TT_KEYWORD && keyword(tokenizer) == KW_VAR) {
        if (!CompileVarDec(self)) {
            return 0;
        }
    }
    if (!CompileStatements(self)) {
        return 0;
    }
    if (!compileSymbol(self,'}')) {
        return 0;
    }
    self->tab--;
    writeOut(self,"</subroutineBody>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileSubroutineDec(CompilationEngine* self) {
    writeOut(self,"<subroutineDec>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_SUBROUTINE_DEC,self->ast_curr,7,NULL);
    self->ast_curr = node;

    const Keyword arr[] = {KW_CONSTRUCTOR,KW_FUNCTION,KW_METHOD};
    if (!compileKeywords(self,arr,3)) {
        return 0;
    }
    if (!compileKeyword(self,KW_VOID)) {
        if (!compileType(self)) {
            sprintf(self->error,"line %d: Expected a type (or void)",self->jack_tokenizer->line);
            return 0;
        };
    }
    if (!compileIdentifier(self)) {
        return 0;
    }
    if (!compileSymbol(self,'(')) {
        return 0;
    }
    if (!CompileParameterList(self)) {
        return 0;
    }
    if (!compileSymbol(self,')')) {
        return 0;
    }
    if (!CompileSubroutineBody(self)) {
        return 0;
    }
    self->tab--;
    writeOut(self,"</subroutineDec>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileParameterList(CompilationEngine* self) {
    writeOut(self,"<parameterList>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_PARAMETER_LIST,self->ast_curr,0,NULL);
    self->ast_curr = node;

    JackTokenizer* tokenizer = self->jack_tokenizer;
    while (
        tokenType(tokenizer) == TT_KEYWORD &&
        ((keyword(tokenizer) == KW_INT) || (keyword(tokenizer) == KW_CHAR) || (keyword(tokenizer) == KW_BOOLEAN))
        ||
        tokenType(tokenizer) == TT_IDENTIFIER
        ) {
        if (!compileType(self)) {
            return 0;
        }
        if (!compileIdentifier(self)) {
            return 0;
        }
        if (!compileSymbol(self,',')) {
            break;
        };
    }
    self->tab--;
    writeOut(self,"</parameterList>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileVarDec(CompilationEngine* self) {
    writeOut(self,"<varDec>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_VAR_DEC,self->ast_curr,4,NULL);
    self->ast_curr = node;

    if (!compileKeyword(self,KW_VAR)) {
        return 0;
    }
    if (!compileType(self)) {
        return 0;
    }
    if (!compileIdentifier(self)) {
        return 0;
    }
    while (compileSymbol(self,',')) {
        if (!compileIdentifier(self)) {
            return 0;
        }
    }
    if (!compileSymbol(self,';')) {
        return 0;
    }
    self->tab--;
    writeOut(self,"</varDec>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileStatements(CompilationEngine* self) {
    writeOut(self,"<statements>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_STATEMENTS,self->ast_curr,1,NULL);
    self->ast_curr = node;
    int finish = 0;
    while (tokenType(self->jack_tokenizer) == TT_KEYWORD && !finish) {
        switch (keyword(self->jack_tokenizer)) {
            case KW_LET:
                if (!CompileLet(self)) {
                    return 0;
                }
                break;
            case KW_IF:
                if (!CompileIf(self)) {
                    return 0;
                }
                break;
            case KW_WHILE:
                if (!CompileWhile(self)) {
                    return 0;
                }
                break;
            case KW_DO:
                if (!CompileDo(self)) {
                    return 0;
                }
                break;
            case KW_RETURN:
                if (!CompileReturn(self)) {
                    return 0;
                }
                break;
            default:
                finish = 1;
                break;
        }
    }
    self->tab--;
    writeOut(self,"</statements>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileLet(CompilationEngine* self) {
    writeOut(self,"<letStatement>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_LET_STATEMENT,self->ast_curr,5,NULL);
    self->ast_curr = node;

    if (!compileKeyword(self,KW_LET)) {
        return 0;
    }
    if (!compileIdentifier(self)) {
        return 0;
    }
    if (compileSymbol(self,'[')) {
        if (!CompileExpression(self)){
            return 0;
        }
        if (!compileSymbol(self,']')) {
            return 0;
        }
    }
    else {
        strncpy(self->error,"\0",1);
    }
    if (!compileSymbol(self,'=')) {
        return 0;
    }
    if (!CompileExpression(self)) {
        return 0;
    }
    if (!compileSymbol(self,';')) {
        return 0;
    }
    self->tab--;
    writeOut(self,"</letStatement>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileIf(CompilationEngine* self) {
    writeOut(self,"<ifStatement>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_IF_STATEMENT,self->ast_curr,7,NULL);
    self->ast_curr = node;

    if (!compileKeyword(self,KW_IF)) {
        return 0;
    }
    if (!compileSymbol(self,'(')) {
        return 0;
    }
    if (!CompileExpression(self)) {
        return 0;
    }
    if (!compileSymbol(self,')')) {
        return 0;
    }
    if (!compileSymbol(self,'{')) {
        return 0;
    }
    if (!CompileStatements(self)) {
        return 0;
    };
    if (!compileSymbol(self,'}')) {
        return 0;
    }
    if (compileKeyword(self,KW_ELSE)) {
        if (!compileSymbol(self,'{')) {
            return 0;
        }
        if (!CompileStatements(self)) {
            return 0;
        }
        if (!compileSymbol(self,'}')) {
            return 0;
        }
    }
    self->tab--;
    writeOut(self,"</ifStatement>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileWhile(CompilationEngine* self) {
    writeOut(self,"<whileStatement>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_WHILE_STATEMENT,self->ast_curr,7,NULL);
    self->ast_curr = node;

    if (!compileKeyword(self,KW_WHILE)) {
        return 0;
    }
    if (!compileSymbol(self,'(')) {
        return 0;
    }
    if (!CompileExpression(self)) {
        return 0;
    }
    if (!compileSymbol(self,')')) {
        return 0;
    }
    if (!compileSymbol(self,'{')) {
        return 0;
    }
    if (!CompileStatements(self)) {
        return 0;
    }
    if (!compileSymbol(self,'}')) {
        return 0;
    }
    self->tab--;
    writeOut(self,"</whileStatement>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileDo(CompilationEngine* self) {
    writeOut(self,"<doStatement>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_DO_STATEMENT,self->ast_curr,3,NULL);
    self->ast_curr = node;

    if (!compileKeyword(self,KW_DO)) {
        return 0;
    }
    if (!CompileSubroutineCall(self)) {
        return 0;
    }
    if (!compileSymbol(self,';')) {
        return 0;
    }
    self->tab--;
    writeOut(self,"</doStatement>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileReturn(CompilationEngine* self) {
    writeOut(self,"<returnStatement>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_RETURN_STATEMENT,self->ast_curr,3,NULL);
    self->ast_curr = node;

    if (!compileKeyword(self,KW_RETURN)) {
        return 0;
    }
    if (isTerm(self)) {
        if (!CompileExpression(self)) {
            return 0;
        }
    }
    if (!compileSymbol(self,';')) {
        return 0;
    }
    self->tab--;
    writeOut(self,"</returnStatement>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}

//Compile expression with x_presedence
int compile_e1_pre(CompilationEngine* self);
int compile_e2_pre(CompilationEngine* self);
int compile_e3_pre(CompilationEngine* self);
int CompileExpression(CompilationEngine* self) {
    writeOut(self,"<expression>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_EXPRESSION,self->ast_curr,3,NULL);
    self->ast_curr = node;

    if (!compile_e1_pre(self)) {
        return 0;
    }
    if (compileSymbol(self,"&") || compileSymbol(self,"|")) {
        if (!compile_e1_pre(self)) {
            return 0;
        }
    }
    else {
        self->jack_tokenizer->isError = 0;
        strcpy(self->error,"");
    }
    self->tab--;
    writeOut(self,"</expression>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int compile_e1_pre(CompilationEngine* self) {
    NodeAST* node = construct_ast_node(NODE_E1,self->ast_curr,3,NULL);
    self->ast_curr = node;

    if (!compile_e2_pre(self)) {
        return 0;
    }
    if (compileSymbol(self,"<") || compileSymbol(self,">"),compileSymbol(self,"=")) {
        if (!compile_e2_pre(self)) {
            return 0;
        }
    }
    else {
        self->jack_tokenizer->isError = 0;
        strcpy(self->error,"");
    }
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int compile_e2_pre(CompilationEngine* self) {
    NodeAST* node = construct_ast_node(NODE_E2,self->ast_curr,3,NULL);
    self->ast_curr = node;

    if (!compile_e3_pre(self)) {
        return 0;
    }
    if (compileSymbol(self,"+") || compileSymbol(self,"-")) {
        if (!compile_e3_pre(self)) {
            return 0;
        }
    }
    else {
        self->jack_tokenizer->isError = 0;
        strcpy(self->error,"");
    }
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int compile_e3_pre(CompilationEngine* self) {
    NodeAST* node = construct_ast_node(NODE_E3,self->ast_curr,3,NULL);
    self->ast_curr = node;

    if (!CompileTerm(self)) {
        return 0;
    }
    if (compileSymbol(self,"*") || compileSymbol(self,"/")) {
        if (!CompileTerm(self)) {
            return 0;
        }
    }
    else {
        self->jack_tokenizer->isError = 0;
        strcpy(self->error,"");
    }
    self->ast_curr = self->ast_curr->parent;
    return 1;
}

void writeAndRealloc(size_t* errors_size,char** errors,const CompilationEngine* self) {
    if (*errors_size > 1)
        strncat(*errors,"\n",1);
    const size_t used = strlen(*errors);
    const size_t add = strlen(self->error);
    if (used + add + 2 > *errors_size) {
        size_t new_size = *errors_size;
        while (used + add + 2 > new_size) {
            new_size *= 2;
        }
        char* tmp = realloc(*errors, new_size);
        if (tmp == NULL) {
            printf("Allocation fail on writeAndRealloc");
            return;
        }
        *errors = tmp;
        *errors_size *= 2;
    }
    const size_t remaining = *errors_size - used - 1;
    strncat(*errors,self->error,remaining);
}
int CompileTerm(CompilationEngine* self) {
    writeOut(self,"<term>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_TERM,self->ast_curr,4,NULL);
    self->ast_curr = node;

    if (compileIntConstant(self)) {
        self->tab--;
        writeOut(self,"</term>\n");
        self->ast_curr = self->ast_curr->parent;
        return 1;
    }
    if (compileStringConstant(self)) {
        self->tab--;
        writeOut(self,"</term>\n");
        self->ast_curr = self->ast_curr->parent;
        return 1;
    }
    const Keyword kws[] = {KW_TRUE,KW_FALSE,KW_NULL,KW_THIS};
    if (compileKeywords(self,kws,4)) {
        self->tab--;
        writeOut(self,"</term>\n");
        self->ast_curr = self->ast_curr->parent;
        return 1;
    }
    if (!compileIdentifier(self)) {
    }
    else if (compileSymbol(self,'[')) {
        if (!CompileExpression(self)) {
            return 0;
        }
        if (!compileSymbol(self,']')) {
            return 0;
        }
        self->tab--;
        writeOut(self,"</term>\n");
        self->ast_curr = self->ast_curr->parent;
        return 1;
    }
    else if (compileSymbol(self,'.')) {
        if (!compileIdentifier(self)) {
            return 0;
        }
        if (!compileSymbol(self,'(')) {
            return 0;
        }
        if (!CompileExpressionList(self)) {
            return 0;
        }
        if (!compileSymbol(self,')')) {
            return 0;
        }
        self->tab--;
        writeOut(self,"</term>\n");
        return 1;
    }
    else {
        self->tab--;
        writeOut(self,"</term>\n");
        self->ast_curr = self->ast_curr->parent;
        return 1;
    }
    if (compileSymbol(self,'(')) {
        if (!CompileExpression(self)) {
            return 0;
        }
        if (!compileSymbol(self,')')) {
            return 0;
        }
        self->tab--;
        writeOut(self,"</term>\n");
        self->ast_curr = self->ast_curr->parent;
        return 1;
    }
    if (compileUnaryOperator(self)) {
        if(CompileTerm(self)) {
            self->tab--;
            writeOut(self,"</term>\n");
            self->ast_curr = self->ast_curr->parent;
            return 1;
        }
        return 0;
    }
    sprintf(self->error,"line %d: Expected a term",self->jack_tokenizer->line);
    return 0;
}
int CompileSubroutineCall(CompilationEngine* self) {
    writeOut(self,"<subroutineCall>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_SUBROUTINE_CALL,self->ast_curr,6,NULL);
    self->ast_curr = node;
    if (!compileIdentifier(self)) {
        return 0;
    }
    if (compileSymbol(self,'.')) {
        if (!compileIdentifier(self)) {
            return 0;
        }
    }
    if (!compileSymbol(self,'(')) {
        return 0;
    }
    if (!CompileExpressionList(self)) {
        return 0;
    }
    if (!compileSymbol(self,')')) {
        return 0;
    }

    self->tab--;
    writeOut(self,"</subroutineCall>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}
int CompileExpressionList(CompilationEngine* self) {
    writeOut(self,"<expressionList>\n");
    self->tab++;

    NodeAST* node = construct_ast_node(NODE_EXPRESSION_LIST,self->ast_curr,1,NULL);
    self->ast_curr = node;

    if (!isTerm(self)) {
        self->tab--;
        writeOut(self,"</expressionList>\n");
        return 1;
    }
    int first = 1;
    while (first || compileSymbol(self,',')) {
        if (!CompileExpression(self)) {
            return 0;
        }
        if (first) {
            first = 0;
        }
    }
    self->tab--;
    writeOut(self,"</expressionList>\n");
    self->ast_curr = self->ast_curr->parent;
    return 1;
}