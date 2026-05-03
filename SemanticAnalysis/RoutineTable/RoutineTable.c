//
// Created by Owner on 18/03/2026.
//

#include "RoutineTable.h"

#include <string.h>
#include "../HashingFunctions/HashingFunctions.h"

//TODO: add handling for function parameters
void addStandardLibRoutines(const RoutineTable *self);
RoutineTable* routine_table_constructor(const size_t tableSize) {
    RoutineTable* table = malloc(sizeof(RoutineTable));
    table->size = tableSize;
    table->routines = malloc(tableSize * sizeof(RoutineList*));
    for (int i = 0; i < tableSize; i++) {
        table->routines[i] = NULL;
    }
    addStandardLibRoutines(table);
    return table;
}
void destroyRoutineTable(RoutineTable *self) {
    for (int i = 0; i < self->size; i++) {
        if (self->routines[i] != NULL) {
            RoutineList *curr = self->routines[i];
            while (curr != NULL) {
                RoutineList *next = curr->next;
                free(curr->routine->class);
                free(curr->routine->name);
                free(curr->routine->type);
                free(curr->routine);
                free(curr);
                curr = next;
            }
        }
    }
    free(self);
}
char defineRoutine(const RoutineTable *self,const RoutineKind kind, const char *name,
    const char *type, const char *class) {
    Routine *routine = malloc(sizeof(Routine));
    routine->kind = kind;
    routine->name = strdup(name);
    routine->type = strdup(type);
    routine->class = strdup(class);
    const int hashValue = fnv1a_hash(name,strlen(name)) % self->size;
    RoutineList **listPtr = &self->routines[hashValue];
    while (*listPtr != NULL) {
        if (strcmp((*listPtr)->routine->name, name) == 0 &&
            strcmp((*listPtr)->routine->class, class) == 0) {
            return 0;
        }
        listPtr = &(*listPtr)->next;
    }
    *listPtr = malloc(sizeof(RoutineList));
    (*listPtr)->routine = routine;
    (*listPtr)->next = NULL;
    listPtr = &self->routines[hashValue];
    return 1;
}
Routine* getRoutine(const RoutineTable *self,const char *name, const char *class) {
    const int hashVal = fnv1a_hash(name,strlen(name)) % self->size;
    const RoutineList *list = self->routines[hashVal];
    while (list != NULL) {
        if (strcmp(list->routine->name,name) == 0 &&
            strcmp(list->routine->class,class) == 0) {
            return list->routine;
        }
        list = list->next;
    }
    return NULL;
}


typedef struct {
    RoutineKind kind;
    char *name;
    char *type;
    char *class;
} OSroutine;
#define ROUTINE_ENTRY(k,n,t,c) { k, n, t, c,}
void addStandardLibRoutines(const RoutineTable *self) {
    static const OSroutine routines[] = {
        //Math
        ROUTINE_ENTRY(ROUTINE_FUNCTION,"init","void","Math"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION,"abs","int","Math"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION,"multiply","int","Math"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION,"divide","int","Math"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION,"min","int","Math"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION,"max","int","Math"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION,"sqrt","int","Math"),

        //String
        ROUTINE_ENTRY(ROUTINE_CONSTRUCTOR,"new","String","String"),
        ROUTINE_ENTRY(ROUTINE_METHOD,"dispose","void","String"),
        ROUTINE_ENTRY(ROUTINE_METHOD,"length","int","String"),
        ROUTINE_ENTRY(ROUTINE_METHOD,"charAt","char","String"),
        ROUTINE_ENTRY(ROUTINE_METHOD,"setCharAt","void","String"),
        ROUTINE_ENTRY(ROUTINE_METHOD,"appendChar","String","String"),
        ROUTINE_ENTRY(ROUTINE_METHOD,"eraseLastChar","void","String"),
        ROUTINE_ENTRY(ROUTINE_METHOD,"intValue","int","String"),
        ROUTINE_ENTRY(ROUTINE_METHOD,"setInt","void","String"),
        ROUTINE_ENTRY(ROUTINE_METHOD,"backSpace","char","String"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION,"doubleQuote","char","String"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION,"newLine","char","String"),

        //Array
        ROUTINE_ENTRY(ROUTINE_CONSTRUCTOR, "new", "Array", "Array"),
        ROUTINE_ENTRY(ROUTINE_METHOD, "dispose", "void", "Array"),

        //Output
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "init", "void", "Output"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "moveCursor", "void", "Output"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "printChar", "void", "Output"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "printString", "void", "Output"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "printInt", "void", "Output"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "println", "void", "Output"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "backSpace", "void", "Output"),

        //Screen
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "init", "void", "Screen"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "clearScreen", "void", "Screen"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "setColor", "void", "Screen"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "drawPixel", "void", "Screen"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "drawLine", "void", "Screen"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "drawRectangle", "void", "Screen"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "drawCircle", "void", "Screen"),

        //Keyboard
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "init", "void", "Keyboard"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "keyPressed", "char", "Keyboard"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "readChar", "char", "Keyboard"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "readLine", "String", "Keyboard"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "readInt", "int", "Keyboard"),

        //Memory
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "init", "void", "Memory"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "peek", "int", "Memory"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "poke", "void", "Memory"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "alloc", "Array", "Memory"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "deAlloc", "void", "Memory"),

        //Sys
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "init", "void", "Sys"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "halt", "void", "Sys"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "error", "void", "Sys"),
        ROUTINE_ENTRY(ROUTINE_FUNCTION, "wait", "void", "Sys"),
    };
    for (int i = 0; i < sizeof(routines) / sizeof(OSroutine); i++) {
        defineRoutine(self,routines[i].kind,
            routines[i].name,
            routines[i].type,
            routines[i].class);
    }
}