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
    char *class; //8
    char *name; //8
    char *type; //8
    RoutineKind kind; //4
} typedef Routine;

struct RoutineList {
    Routine *routine; //8
    struct RoutineList* next; //8
}typedef RoutineList;

typedef struct RoutineTable {
    RoutineList **routines; //8
    size_t size; //8
} RoutineTable;

char defineRoutine(const RoutineTable *self,const RoutineKind kind, const char *name,
    const char *type, const char *class);
Routine* getRoutine(const RoutineTable *self,const char *name, const char *class);
RoutineTable* routine_table_constructor(const size_t tableSize);
#endif //ROUTINETABLE_H
