//
// Created by Owner on 18/02/2026.
//

#include "SymbolTable.h"

#include <stdlib.h>
#include <string.h>

SymbolTable* constructor(const int tableSize) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    table->size = tableSize;
    table->classScope = malloc(tableSize * sizeof(SymbolList*));
    table->subroutineScope = malloc(tableSize * sizeof(SymbolList*));
    for (int i = 0; i < tableSize; i++) {
        table->classScope[i] = NULL;
        table->subroutineScope[i] = NULL;
    }
    table->argIndex = 0;
    table->fieldIndex = 0;
    table->staticIndex = 0;
    table->varIndex = 0;
    return table;
}
SymbolList** getScope(const SymbolTable* self,const SymbolKind kind) {
    return (kind == FIELD || kind == STATIC) ?
        self->classScope :
        self->subroutineScope;
}

void startSubroutine(const SymbolTable* self) {
    for (int i = 0; i < self->size; i++) {
        self->subroutineScope[i] = NULL;
    }
}
unsigned int hash(const char s[], const int length) {
    //FNV-1a
    unsigned int val = 2166136261u;
    for (int i = 0; i < length; i++) {
        val ^= (unsigned char) s[i];
        val *= 16777619u;
    }
    return val;
}
void addSymbolToTable(const SymbolTable *self,const SymbolKind kind, const int hashVal,const Symbol symbol) {
    //Init listPtr with correct scope
    SymbolList** listPtr = &getScope(self,kind)[hashVal];
    //when *listPtr == NULL, listPtr points to pointer to empty bucket / pointer to empty next
    while (*listPtr != NULL) {
        listPtr = &(*listPtr)->next;
    }
    *listPtr = malloc(sizeof(SymbolList));
    (*listPtr)->symbol = symbol;
    (*listPtr)->next = NULL;
}
void define(SymbolTable* self, char name[],const int nameLength, char type[], const SymbolKind kind) {
    //Initialize symbol for symbol table - saves info of identifier
    Symbol symbol;
    symbol.kind = kind;
    strcpy(symbol.name,name);
    strcpy(symbol.type,type);
    switch (kind) {
        case FIELD: {
            symbol.index = self->fieldIndex;
            self->fieldIndex++;
            break;
        }
        case STATIC: {
            symbol.index = self->staticIndex;
            self->staticIndex++;
            break;
        }
        case ARG: {
            symbol.index = self->argIndex;
            self->argIndex++;
            break;
        }
        case VAR: {
            symbol.index = self->varIndex;
            self->varIndex++;
            break;
        }
        default:
            symbol.index = 0;
            break;
    }
    //Get hash value for symbol based on name
    const int hashVal = hash(name,nameLength) % self->size;
    //Assign memory based on scope in hashtable + linkedlist structure
    addSymbolToTable(self,kind,hashVal,symbol);
}
int varCount(const SymbolTable *self,const SymbolKind kind) {
    switch (kind) {
        case FIELD:
            return self->fieldIndex;
        case STATIC:
            return self->staticIndex;
        case VAR:
            return self->varIndex;
        case ARG:
            return self->argIndex;
        default:
            return 0;
    }
}
SymbolKind kindOf(const SymbolTable *self,const char name[], const int length) {
    const int hashVal = hash(name,length) % self->size;
    const SymbolList* list = self->subroutineScope[hashVal];
    while (list != NULL) {
        if (strcmp(list->symbol.name,name) == 0) {
            return list->symbol.kind;
        }
        list = list->next;
    }
    list = self->classScope[hashVal];
    while (list != NULL) {
        if (strcmp(list->symbol.name,name) == 0) {
            return list->symbol.kind;
        }
        list = list->next;
    }
    return NONE;
}
char* typeOf(const SymbolTable *self,const char name[], const int length) {
    const int hashVal = hash(name,length) % self->size;
    const SymbolList* list = self->subroutineScope[hashVal];
    while (list != NULL) {
        if (strcmp(list->symbol.name,name) == 0) {
            return list->symbol.type;
        }
        list = list->next;
    }
    list = self->classScope[hashVal];
    while (list != NULL) {
        if (strcmp(list->symbol.name,name) == 0) {
            return list->symbol.type;
        }
        list = list->next;
    }
    return "";
}
int indexOf(const SymbolTable *self,const char name[], const int length) {
    const int hashVal = hash(name,length) % self->size;
    const SymbolList* list = self->subroutineScope[hashVal];
    while (list != NULL) {
        if (strcmp(list->symbol.name,name) == 0) {
            return list->symbol.index;
        }
        list = list->next;
    }
    list = self->classScope[hashVal];
    while (list != NULL) {
        if (strcmp(list->symbol.name,name) == 0) {
            return list->symbol.index;
        }
        list = list->next;
    }
    return -1;
}
void destroySymbolList(SymbolList* list) {
    while (list != NULL) {
        SymbolList* temp = list;
        list = list->next;
        free(temp);
    }
}

void destroySymbolTable(SymbolTable* self) {
    if (self == NULL) return;

    for (int i = 0; i < self->size; i++) {
        destroySymbolList(self->classScope[i]);
        destroySymbolList(self->subroutineScope[i]);
    }

    free(self->classScope);
    free(self->subroutineScope);
    free(self);
}