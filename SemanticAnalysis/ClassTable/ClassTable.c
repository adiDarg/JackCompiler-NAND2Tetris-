//
// Created by Owner on 19/03/2026.
//

#include "ClassTable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../HashingFunctions/HashingFunctions.h"
void addStandardLibClasses(const ClassTable *self);
ClassTable* class_table_constructor(const size_t tableSize) {
    ClassTable *table = malloc(sizeof(ClassTable));
    table->size = tableSize;
    table->classes = malloc(sizeof(ClassList*)*tableSize);
    for (int i = 0; i < tableSize; i++) {
        table->classes[i] = NULL;
    }
    addStandardLibClasses(table);
    return table;
}
char defineClass(const ClassTable *self,char *name, const int nameLength) {
    const int hash = fnv1a_hash(name,nameLength) % self->size;
    ClassList **listPtr = &self->classes[hash];

    while (*listPtr != NULL) {
        if (strcmp((*listPtr)->name,name) == 0) {
            return 0;
        }
        listPtr = &((*listPtr)->next);
    }
    *listPtr = malloc(sizeof(ClassList));
    if (*listPtr == NULL) return 0;
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

typedef struct {
    char *name;
    int len;
} OSClass;
#define CLASS_ENTRY(n) { n, sizeof(n) - 1 }

void addStandardLibClasses(const ClassTable *self) {
    static const OSClass classes[] = {
        CLASS_ENTRY("Math"),    CLASS_ENTRY("String"),
        CLASS_ENTRY("Array"),   CLASS_ENTRY("Output"),
        CLASS_ENTRY("Screen"),  CLASS_ENTRY("Keyboard"),
        CLASS_ENTRY("Memory"),  CLASS_ENTRY("Sys")
    };

    for (int i = 0; i < sizeof(classes) / sizeof(OSClass); i++) {
        defineClass(self, classes[i].name, classes[i].len);
    }
}