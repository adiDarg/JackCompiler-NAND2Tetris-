//
// Created by Owner on 18/03/2026.
//

#include "RoutineTable.h"

#include <string.h>
#include "../HashingFunctions/HashingFunctions.h"
RoutineTable* routine_table_constructor(const size_t tableSize) {
    RoutineTable* table = malloc(sizeof(RoutineTable));
    table->size = tableSize;
    table->routines = malloc(tableSize * sizeof(RoutineList*));
    return table;
}



char defineRoutine(const RoutineTable *self,const RoutineKind kind, const char *name, const int nameLength,
    const char *type, const int typeLength) {
    Routine *routine = malloc(sizeof(Routine));
    routine->kind = kind;
    routine->name = malloc(nameLength);
    strncpy(routine->name,name,nameLength);
    routine->type = malloc(typeLength);
    strncpy(routine->type,type,typeLength);
    const int hashValue = fnv1a_hash(name,nameLength) % self->size;
    RoutineList **listPtr = &self->routines[hashValue];
    while (*listPtr != NULL) {
        if (strcmp((*listPtr)->routine->name,name) == 0) {
            return 0;
        }
        *listPtr = (*listPtr)->next;
    }
    *listPtr = malloc(sizeof(RoutineList));
    (*listPtr)->routine = routine;
    (*listPtr)->next = NULL;
    return 1;
}
Routine* getRoutine(const RoutineTable *self,const char *name, const int nameLength) {
    const int hashVal = fnv1a_hash(name,nameLength);
    const RoutineList *list = self->routines[hashVal];
    while (list != NULL) {
        if (strcmp(list->routine->name,name) == 0) {
            return list->routine;
        }
        list = list->next;
    }
    return NULL;
}