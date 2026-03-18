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
#endif //ROUTINETABLE_H
