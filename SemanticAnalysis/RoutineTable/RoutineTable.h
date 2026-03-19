//
// Created by Owner on 18/03/2026.
//

#ifndef ROUTINETABLE_H
#define ROUTINETABLE_H
#include <stdlib.h>
typedef enum RoutineKind {
    ROUTINE_FUNCTION,
    ROUTINE_METHOD,
    ROUTINE_CONSTRUCTOR,
    ROUTINE_NONE
} RoutineKind;

struct RoutineValue {
    RoutineKind kind;
    char *name;
    char *type;
} typedef Routine;

struct RoutineList {
    Routine *routine;
    struct RoutineList* next;
}typedef RoutineList;

typedef struct RoutineTable {
    RoutineList **routines;
    size_t size;
} RoutineTable;

char defineRoutine(const RoutineTable *self,const RoutineKind kind, const char *name,
    const int nameLength, const char *type, const int typeLength);
Routine* getRoutine(const RoutineTable *self,const char *name, const int nameLength);
RoutineTable* routine_table_constructor(const size_t tableSize);
#endif //ROUTINETABLE_H
