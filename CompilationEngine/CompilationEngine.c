//
// Created by Owner on 18/12/2025.
//

#include "CompilationEngine.h"
#include "../JackTokenizer/jackTokenizer.h"
#include "../Keyword/keyword.h"

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
    if (tokenizer->error != "\0") {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    if (tokenType(tokenizer) != KEYWORD || keyword(tokenizer) != kw) {
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
            return 1;
        }
    }
    return 0;
}
int compileIdentifier(CompilationEngine* self) {
    if (self->jack_tokenizer->error != "\0") {
        strcpy(self->error,self->jack_tokenizer->error);
        return 0;
    }
    if (tokenType(self->jack_tokenizer) != IDENTIFIER) {
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
    if (tokenizer->error != "\0") {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    if (tokenType(tokenizer) != SYMBOL || symbol(tokenizer) != sym) {
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
    if (tokenizer->error != "\0") {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    if (tokenType(tokenizer) != SYMBOL) {
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
    if (self->jack_tokenizer->error != "\0") {
        strcpy(self->error,self->jack_tokenizer->error);
        return 0;
    }
    if (tokenType(self->jack_tokenizer) != INT_CONST) {
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
    if (self->jack_tokenizer->error != "\0") {
        strcpy(self->error,self->jack_tokenizer->error);
        return 0;
    }
    if (tokenType(self->jack_tokenizer) != STRING_CONST) {
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
    const JackTokenizer* tokenizer = self->jack_tokenizer;
    if (tokenizer->error != "\0") {
        strcpy(self->error,tokenizer->error);
        return 0;
    }
    const int primitive = tokenType(tokenizer) == KEYWORD &&
        ((keyword(tokenizer) == INT) || (keyword(tokenizer) == CHAR) || (keyword(tokenizer) == BOOLEAN));
    const int className = tokenType(tokenizer) == IDENTIFIER;
    if (!primitive && !className){
        return 0;
    }
    if (primitive) {
        const Keyword arr[] = {INT,CHAR,BOOLEAN};
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
    const JackTokenizer* tokenizer = self->jack_tokenizer;
    const int constant = tokenType(tokenizer) == INT_CONST || tokenType(tokenizer) == STRING_CONST;
    const Keyword kw = keyword(tokenizer);
    const int keywordConstant = tokenType(tokenizer) == KEYWORD &&
        (kw == TRUE || kw == FALSE || kw == KW_NULL || kw==THIS);
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
    if (!compileKeyword(self,CLASS)) {
        return 0;
    }
    if (!compileIdentifier(self)) {
        return 0;
    }
    if (!compileSymbol(self,'{')) {
        return 0;
    }
    while (tokenType(tokenizer) == KEYWORD &&
        (keyword(tokenizer) == STATIC || keyword(tokenizer) == FIELD)) {
        if (!CompileClassVarDec(self)) {
            return 0;
        }
    }
    while (tokenType(tokenizer) == KEYWORD &&
    keyword(tokenizer) == CONSTRUCTOR || keyword(tokenizer) == FUNCTION || keyword(tokenizer) == METHOD) {
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
    const Keyword arr[] = {STATIC,FIELD};
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
    return 1;
}
int CompileSubroutineBody(CompilationEngine* self) {
    writeOut(self,"<subroutineBody>\n");
    self->tab++;
    const JackTokenizer* tokenizer = self->jack_tokenizer;
    if (!compileSymbol(self,'{')) {
        return 0;
    }
    while (tokenType(tokenizer) == KEYWORD && keyword(tokenizer) == VAR) {
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
    const Keyword arr[] = {CONSTRUCTOR,FUNCTION,METHOD};
    if (!compileKeywords(self,arr,3)) {
        return 0;
    }
    if (!compileKeyword(self,VOID)) {
        if (!compileType(self)) {
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
    self->tab++;
    const JackTokenizer* tokenizer = self->jack_tokenizer;
    while (
        tokenType(tokenizer) == KEYWORD &&
        ((keyword(tokenizer) == INT) || (keyword(tokenizer) == CHAR) || (keyword(tokenizer) == BOOLEAN))
        ||
        tokenType(tokenizer) == IDENTIFIER
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
    return 1;
}
int CompileVarDec(CompilationEngine* self) {
    writeOut(self,"<varDec>\n");
    self->tab++;
    if (!compileKeyword(self,VAR)) {
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
    return 1;
}
int CompileStatements(CompilationEngine* self) {
    writeOut(self,"<statements>\n");
    self->tab++;

    int finish = 0;
    while (tokenType(self->jack_tokenizer) == KEYWORD && !finish) {
        switch (keyword(self->jack_tokenizer)) {
            case LET:
                if (!CompileLet(self)) {
                    return 0;
                }
                break;
            case IF:
                if (!CompileIf(self)) {
                    return 0;
                }
                break;
            case WHILE:
                if (!CompileWhile(self)) {
                    return 0;
                }
                break;
            case DO:
                if (!CompileDo(self)) {
                    return 0;
                }
                break;
            case RETURN:
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
    if (!compileKeyword(self,LET)) {
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
    if (!compileKeyword(self,IF)) {
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
    if (compileKeyword(self,ELSE)) {
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
    if (!compileKeyword(self,WHILE)) {
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
    if (!compileKeyword(self,DO)) {
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
    if (!compileKeyword(self,RETURN)) {
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
    if (strlen(*errors) > 0.8 * *errors_size) {
        *errors = (char*) realloc(*errors,*errors_size * 2);
        *errors_size *= 2;
    }
    strncat(*errors,self->error,*errors_size/2);
    strncat(*errors,"\0",1);
}
int CompileTerm(CompilationEngine* self) {
    writeOut(self,"<term>\n");
    self->tab++;
    char* errors = malloc(sizeof(char) * 100);
    if (errors == NULL) {
        printf("Failed to alocate errors[100]");
        return 0;
    }
    errors[0] = '\0';
    size_t errors_size = 100;
    if (!compileIntConstant(self)) {
        writeAndRealloc(&errors_size,&errors,self);
    }
    else {
        self->tab--;
        writeOut(self,"</term>\n");
        free(errors);
        return 1;
    }
    if (!compileStringConstant(self)) {
        writeAndRealloc(&errors_size,&errors,self);
    }
    else {
        self->tab--;
        writeOut(self,"</term>\n");
        free(errors);
        return 1;
    }
    const Keyword kws[] = {TRUE,FALSE,KW_NULL,THIS};
    if (!compileKeywords(self,kws,4)) {
        writeAndRealloc(&errors_size,&errors,self);
    }
    else {
        self->tab--;
        writeOut(self,"</term>\n");
        free(errors);
        return 1;
    }
    if (!compileIdentifier(self)) {
        writeAndRealloc(&errors_size,&errors,self);
    }
    else if (compileSymbol(self,'[')) {
        if (!CompileExpression(self)) {
            free(errors);
            return 0;
        }
        if (!compileSymbol(self,']')) {
            free(errors);
            return 0;
        }
        self->tab--;
        writeOut(self,"</term>\n");
        free(errors);
        return 1;
    }
    else if (compileSymbol(self,'.')) {
        if (!compileIdentifier(self)) {
            free(errors);
            return 0;
        }
        if (!compileSymbol(self,'(')) {
            free(errors);
            return 0;
        }
        if (!CompileExpressionList(self)) {
            free(errors);
            return 0;
        }
        if (!compileSymbol(self,')')) {
            free(errors);
            return 0;
        }
        self->tab--;
        writeOut(self,"</term>\n");
        free(errors);
        return 1;
    }
    else {
        self->tab--;
        writeOut(self,"</term>\n");
        free(errors);
        return 1;
    }
    if (!compileSymbol(self,'(')) {
        writeAndRealloc(&errors_size,&errors,self);
    }
    else {
        if (!CompileExpression(self)) {
            free(errors);
            return 0;
        }
        if (!compileSymbol(self,')')) {
            free(errors);
            return 0;
        }
        self->tab--;
        writeOut(self,"</term>\n");
        free(errors);
        return 1;
    }
    if (!compileUnaryOperator(self)) {
        writeAndRealloc(&errors_size,&errors,self);
    }
    else {
        if(CompileTerm(self)) {
            self->tab--;
            writeOut(self,"</term>\n");
            free(errors);
            return 1;
        }
        free(errors);
        return 0;
    }
    strncpy(self->error,errors,100);
    free(errors);
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