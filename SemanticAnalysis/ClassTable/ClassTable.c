//
// Created by Owner on 19/03/2026.
//

#include "ClassTable.h"

#include <stdlib.h>
#include <string.h>

#include "../HashingFunctions/HashingFunctions.h"
ClassTable* class_table_constructor(const size_t tableSize) {
    ClassTable *table = malloc(sizeof(ClassTable));
    table->size = tableSize;
    table->classes = malloc(sizeof(ClassList*)*tableSize);
    return table;
}
char defineClass(const ClassTable *self,char *name, const int nameLength) {
    const int hash = fnv1a_hash(name,nameLength) % self->size;
    ClassList **listPtr = &self->classes[hash];
    while (*listPtr != NULL) {
        if (strcmp((*listPtr)->name,name) == 0) {
            return 0;
        }
        *listPtr = (*listPtr)->next;
    }
    *listPtr = malloc(sizeof(ClassList));
    (*listPtr)->name = name;
    (*listPtr)->next = NULL;
    return 1;
}
char doesClassExist(const ClassTable *self,const char *name, const int nameLength) {
    const int hash = fnv1a_hash(name,nameLength) % self->size;
    ClassList **listPtr = &self->classes[hash];
    while (*listPtr != NULL) {
        if (strcmp((*listPtr)->name,name) == 0) {
            return 1;
        }
        *listPtr = (*listPtr)->next;
    }
    return 0;
}