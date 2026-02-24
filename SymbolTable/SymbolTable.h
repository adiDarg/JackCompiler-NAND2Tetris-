//
// Created by Owner on 18/02/2026.
//

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
enum SymbolKind {
    STATIC,
    FIELD,
    ARG,
    VAR,
    NONE
} typedef SymbolKind;
struct SymbolValue {
    char name[];
    char type[];
    SymbolKind kind;
    int index;
} typedef Symbol;
struct SymbolList {
    Symbol symbol;
    struct SymbolList* next;
}typedef SymbolList;

struct SymbolTable {
    SymbolList **classScope;
    SymbolList **subroutineScope;
    int size;
    int staticIndex;
    int fieldIndex;
    int argIndex;
    int varIndex;
} typedef SymbolTable;

SymbolTable* constructor();
void startSubroutine(SymbolTable* self);
void define(SymbolTable* self,char name[],char type[], SymbolKind kind);
int varCount(SymbolTable* self, SymbolKind kind);
SymbolKind kindOf(SymbolTable* self, char name[]);
char* typeOf(SymbolTable* self, char name[]);
int indexOf(SymbolTable* self, char name[]);
#endif //SYMBOLTABLE_H
