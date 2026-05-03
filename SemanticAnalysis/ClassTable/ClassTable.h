//
// Created by Owner on 19/03/2026.
//

#ifndef CLASSTABLE_H
#define CLASSTABLE_H
#include <stddef.h>

struct ClassList {
    char *name; //8
    struct ClassList* next; //8
}typedef ClassList;

typedef struct ClassTable {
    ClassList **classes; //8
    size_t size; //8
} ClassTable;

ClassTable* class_table_constructor(const size_t tableSize);
char defineClass(const ClassTable *self,const char *name);
char doesClassExist(const ClassTable *self,const char *name);
void destroyClassTable(ClassTable *self);
#endif //CLASSTABLE_H
