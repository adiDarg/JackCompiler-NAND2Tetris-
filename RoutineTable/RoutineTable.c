//
// Created by Owner on 18/03/2026.
//

#include "RoutineTable.h"

#include <string.h>

RoutineTable* routine_table_constructor(const size_t tableSize) {
    RoutineTable* table = malloc(sizeof(RoutineTable));
    table->size = tableSize;
    table->routines = malloc(tableSize * sizeof(RoutineList*));
    return table;
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

void defineRoutine(const RoutineTable *self,const RoutineKind kind, const char *name, const int nameLength, const char *type, const int typeLength) {
    Routine *routine = malloc(sizeof(Routine));
    routine->kind = kind;
    routine->name = malloc(nameLength);
    strncpy(routine->name,name,nameLength);
    routine->type = malloc(typeLength);
    strncpy(routine->type,type,typeLength);
    const int hashValue = hash(name,nameLength) % self->size;
    RoutineList **listPtr = &self->routines[hashValue];
    while (*listPtr != NULL) {
        *listPtr = (*listPtr)->next;
    }
    *listPtr = malloc(sizeof(RoutineList));
    (*listPtr)->routine = routine;
    (*listPtr)->next = NULL;
}
Routine* getRoutine(const RoutineTable *self,const char *name, const int nameLength) {
    const int hashVal = hash(name,nameLength);
    const RoutineList *list = self->routines[hashVal];
    while (list != NULL) {
        if (strcmp(list->routine->name,name)) {
            return list->routine;
        }
        list = list->next;
    }
    return NULL;
}