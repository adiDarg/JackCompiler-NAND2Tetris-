//
// Created by Owner on 19/03/2026.
//

#ifndef CLASSTABLE_H
#define CLASSTABLE_H
#include <stddef.h>

struct ClassList {
    char *name;
    struct ClassList* next;
}typedef ClassList;

typedef struct ClassTable {
    ClassList **classes;
    size_t size;
} ClassTable;

ClassTable* routine_table_constructor(const size_t tableSize);
char defineClass(const ClassTable *self, char *name, const int nameLength);
char doesClassExist(const ClassTable *self,const char *name, const int nameLength);
#endif //CLASSTABLE_H
