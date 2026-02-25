//
// Created by Owner on 18/12/2025.
//

#include "CompilationEngine.h"
#include "../JackTokenizer/jackTokenizer.h"
#include "../VMWriter/VMWriter.h"
#include "../Keyword/keyword.h"

#include <stdlib.h>
#include <string.h>
#define DEFINESYMBOL() define(self->symbol_table,st_info->name,st_info->type,st_info->kind);

CompilationEngine* Construct_Engine(JackTokenizer *jack_tokenizer, VMWriter *vm_writer) {
    CompilationEngine* self = malloc(sizeof(CompilationEngine));
    self->jack_tokenizer = jack_tokenizer;
    self->vm_writer = vm_writer;
    self->symbol_table = constructor();
    self->out = malloc(8192);
    self->out[0] = '\0';
    self->tab=0;
    self->cap=8192;
    self->len = 0;
    self->error[0] = '\0';
    self->stInfo = malloc(sizeof(SymbolTableInfo));
    self->stInfo->nameLength = 100;
    self->stInfo->name[self->stInfo->nameLength];
    self->stInfo->typeLength = 100;
    self->stInfo->type[self->stInfo->typeLength];
    return self;
}
void calcSymbolKind(const CompilationEngine *self) {
    if (tokenType(self->jack_tokenizer) != KEYWORD) {
        return;
    }
    switch (keyword(self->jack_tokenizer)) {
        case KW_STATIC: {
            self->stInfo->kind = SK_STATIC;
            break;
        }
        case KW_FIELD: {
            self->stInfo->kind = SK_FIELD;
            break;
        }
        case KW_VAR: {
            self->stInfo->kind = SK_VAR;
            break;
        }
        default: {
            break;
        }
    }
}
void calcSymbolType(const CompilationEngine *self) {
    JackTokenizer *tokenizer = self->jack_tokenizer;
    if (tokenType(tokenizer) == IDENTIFIER) {
        strncpy(self->stInfo->type,tokenizer->buffer,self->stInfo->typeLength);
        return;
    }
    if (tokenType(tokenizer) != KEYWORD) {
        return;
    }
    switch (keyword(tokenizer)) {
        case KW_INT: {
            strncpy(self->stInfo->type,"int",self->stInfo->typeLength);
            break;
        }
        case KW_CHAR: {
            strncpy(self->stInfo->type,"char",self->stInfo->typeLength);
        }
        case KW_BOOLEAN: {
            strncpy(self->stInfo->type,"boolean",self->stInfo->typeLength);
        }
        default:
            break;
    }
}
void calcSymbolName(const CompilationEngine* self) {
    JackTokenizer *j_tokenizer = self->jack_tokenizer;
    if (tokenType(j_tokenizer) == IDENTIFIER) {
        strncpy(self->stInfo->name,j_tokenizer->buffer,self->stInfo->nameLength);
    }
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
    if (tokenType(tokenizer) != KEYWORD || keyword(tokenizer) != kw) {
        sprintf(self->error,"line %d: Expected %s",self->jack_tokenizer->line,keyword_to_text(kw));
        return 0;
    }
    writeOut(self,"<keyword> ");
    const int temp = self->tab;
    self->tab = 0;
    writeOut(self,keyword_to_text(kw));
    writeOut(self," </keyword>\n");
    self->tab = temp;
    advance(tokenizer);
    return 1;
}
int compileKeywords(CompilationEngine* self, const Keyword* arr,const int len){
    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (tokenType(tokenizer) != KEYWORD) {
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
    if (tokenType(self->jack_tokenizer) != IDENTIFIER) {
        sprintf(self->error,"line %d: Expected Identifier",self->jack_tokenizer->line);
        return 0;
    }
    writeOut(self, "<identifier> ");
    const int temp = self->tab;
    self->tab = 0;
    writeOut(self, identifier(self->jack_tokenizer));
    writeOut(self," </identifier>\n");
    self->tab = temp;
    advance(self->jack_tokenizer);
    return 1;
}
int compileSymbol(CompilationEngine* self, char sym) {
    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    if (tokenType(tokenizer) != SYMBOL || symbol(tokenizer) != sym) {
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
    advance(tokenizer);
    return 1;
}
int compileSymbols(CompilationEngine* self, const char* arr,const int len){
    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    if (tokenType(tokenizer) != SYMBOL) {
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
    if (tokenType(self->jack_tokenizer) != INT_CONST) {
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
    advance(self->jack_tokenizer);
    return 1;
}
int compileStringConstant(CompilationEngine* self) {
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,self->jack_tokenizer->error);
        return 0;
    }
    if (tokenType(self->jack_tokenizer) != STRING_CONST) {
        sprintf(self->error,"Expected string constant");
        return 0;
    }
    writeOut(self, "<stringConstant> ");
    const int temp = self->tab;
    self->tab = 0;
    writeOut(self, stringVal(self->jack_tokenizer));
    writeOut(self," </stringConstant>\n");
    self->tab = temp;
    advance(self->jack_tokenizer);
    return 1;
}
int compileType(CompilationEngine* self) {
    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (self->jack_tokenizer->isError == 1) {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    const int primitive = tokenType(tokenizer) == KEYWORD &&
        ((keyword(tokenizer) == KW_INT) || (keyword(tokenizer) == KW_CHAR) || (keyword(tokenizer) == KW_BOOLEAN));
    const int className = tokenType(tokenizer) == IDENTIFIER;
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
    const int constant = tokenType(tokenizer) == INT_CONST || tokenType(tokenizer) == STRING_CONST;
    const Keyword kw = keyword(tokenizer);
    const int keywordConstant = tokenType(tokenizer) == KEYWORD &&
        (kw == KW_TRUE || kw == KW_FALSE || kw == KW_NULL || kw==KW_THIS);
    const int identifier = tokenType(tokenizer) == IDENTIFIER;
    const char symb = symbol(tokenizer);
    const int symbols = tokenType(tokenizer) == SYMBOL &&
        (symb == '(' || symb=='-' || symb=='~');
    return constant || keywordConstant || identifier || symbols;
}
int CompileClass(CompilationEngine *self) {
    writeOut(self,"<class>\n");
    self->tab++;
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
    while (tokenType(tokenizer) == KEYWORD &&
        (keyword(tokenizer) == KW_STATIC || keyword(tokenizer) == KW_FIELD)) {
        if (!CompileClassVarDec(self)) {
            return 0;
        }
    }
    while (tokenType(tokenizer) == KEYWORD &&
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
    SymbolTableInfo *st_info = self->stInfo;
    writeOut(self,"<classVarDec>\n");
    self->tab++;
    const Keyword arr[] = {KW_STATIC,KW_FIELD};
    calcSymbolKind(self);
    if (!compileKeywords(self, arr, 2)) {
        return 0;
    }
    calcSymbolType(self);
    if (!compileType(self)) {
        return 0;
    };
    calcSymbolName(self);
    if (!compileIdentifier(self)) {
        return 0;
    }
    DEFINESYMBOL();
    while (compileSymbol(self,',')) {
        calcSymbolName(self);
        if (!compileIdentifier(self)) {
            return 0;
        }
        DEFINESYMBOL();
    }
    compileSymbol(self,';');
    self->tab--;
    writeOut(self,"</classVarDec>\n");
    return 1;
}
int CompileSubroutineBody(CompilationEngine* self) {
    writeOut(self,"<subroutineBody>\n");
    self->tab++;
    JackTokenizer* tokenizer = self->jack_tokenizer;
    if (!compileSymbol(self,'{')) {
        return 0;
    }
    while (tokenType(tokenizer) == KEYWORD && keyword(tokenizer) == KW_VAR) {
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
    return 1;
}
int CompileSubroutineDec(CompilationEngine* self) {
    writeOut(self,"<subroutineDec>\n");
    self->tab++;
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
    return 1;
}
int CompileParameterList(CompilationEngine* self) {
    writeOut(self,"<parameterList>\n");
    SymbolTableInfo *st_info = self->stInfo;
    st_info->kind = SK_ARG;
    self->tab++;
    JackTokenizer* tokenizer = self->jack_tokenizer;
    while (
        tokenType(tokenizer) == KEYWORD &&
        ((keyword(tokenizer) == KW_INT) || (keyword(tokenizer) == KW_CHAR) || (keyword(tokenizer) == KW_BOOLEAN))
        ||
        tokenType(tokenizer) == IDENTIFIER
        ) {
        calcSymbolType(self);
        if (!compileType(self)) {
            return 0;
        }
        calcSymbolName(self);
        if (!compileIdentifier(self)) {
            return 0;
        }
        DEFINESYMBOL();
        if (!compileSymbol(self,',')) {
            break;
        };
    }
    self->tab--;
    writeOut(self,"</parameterList>\n");
    return 1;
}
int CompileVarDec(CompilationEngine* self) {
    SymbolTableInfo *st_info = self->stInfo;
    st_info->kind = SK_VAR;
    writeOut(self,"<varDec>\n");
    self->tab++;
    if (!compileKeyword(self,KW_VAR)) {
        return 0;
    }
    calcSymbolType(self);
    if (!compileType(self)) {
        return 0;
    }
    calcSymbolName(self);
    if (!compileIdentifier(self)) {
        return 0;
    }
    DEFINESYMBOL();
    while (compileSymbol(self,',')) {
        calcSymbolName(self);
        if (!compileIdentifier(self)) {
            return 0;
        }
        DEFINESYMBOL();
    }
    if (!compileSymbol(self,';')) {
        return 0;
    }
    self->tab--;
    writeOut(self,"</varDec>\n");
    return 1;
}
int CompileStatements(CompilationEngine* self) {
    writeOut(self,"<statements>\n");
    self->tab++;

    int finish = 0;
    while (tokenType(self->jack_tokenizer) == KEYWORD && !finish) {
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
    return 1;
}
int CompileLet(CompilationEngine* self) {
    writeOut(self,"<letStatement>\n");
    self->tab++;
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
    return 1;
}
int CompileIf(CompilationEngine* self) {
    writeOut(self,"<ifStatement>\n");
    self->tab++;
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
    return 1;
}
int CompileWhile(CompilationEngine* self) {
    writeOut(self,"<whileStatement>\n");
    self->tab++;
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
    return 1;
}
int CompileDo(CompilationEngine* self) {
    writeOut(self,"<doStatement>\n");
    self->tab++;
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
    return 1;
}
int CompileReturn(CompilationEngine* self) {
    writeOut(self,"<returnStatement>\n");
    self->tab++;
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
    return 1;
}
int CompileExpression(CompilationEngine* self) {
    writeOut(self,"<expression>\n");
    self->tab++;
    if (!CompileTerm(self)) {
        return 0;
    }
    while (compileOperator(self)) {
        if (!CompileTerm(self)) {
            return 0;
        };
    }
    self->tab--;
    writeOut(self,"</expression>\n");
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
    if (compileIntConstant(self)) {
        self->tab--;
        writeOut(self,"</term>\n");
        return 1;
    }
    if (compileStringConstant(self)) {
        self->tab--;
        writeOut(self,"</term>\n");
        return 1;
    }
    const Keyword kws[] = {KW_TRUE,KW_FALSE,KW_NULL,KW_THIS};
    if (compileKeywords(self,kws,4)) {
        self->tab--;
        writeOut(self,"</term>\n");
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
        return 1;
    }
    if (compileUnaryOperator(self)) {
        if(CompileTerm(self)) {
            self->tab--;
            writeOut(self,"</term>\n");
            return 1;
        }
        return 0;
    }
    sprintf(self->error,"line %d: Expected a term",self->jack_tokenizer->line);
    return 0;
}
int CompileSubroutineCall(CompilationEngine* self) {
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
    return 1;
}
int CompileExpressionList(CompilationEngine* self) {
    writeOut(self,"<expressionList>\n");
    self->tab++;
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
    return 1;
}