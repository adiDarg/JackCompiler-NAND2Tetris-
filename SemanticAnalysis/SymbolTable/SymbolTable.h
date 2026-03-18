//
// Created by Owner on 18/02/2026.
//

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
enum SymbolKind {
    SK_STATIC,
    SK_FIELD,
    SK_ARG,
    SK_VAR,
    SK_NONE
} typedef SymbolKind;
struct SymbolValue {
    SymbolKind kind;
    int index;
    char *name;
    char *type;
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

SymbolTable* symbol_table_constructor(const int tableSize);
void startSubroutine(const SymbolTable *self);
void define(SymbolTable* self,char name[],const int nameLength, char type[],const int typeLength, SymbolKind kind);
int varCount(const SymbolTable* self, SymbolKind kind);
SymbolKind kindOf(const SymbolTable* self,const char name[], const int length);
char* typeOf(const SymbolTable* self,const char name[], const int length);
int indexOf(const SymbolTable* self,const char name[], const int length);
#endif //SYMBOLTABLE_H
