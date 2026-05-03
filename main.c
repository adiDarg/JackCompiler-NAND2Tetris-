#include <stdio.h>
#include <stdlib.h>
#include "JackAnalyzer/JackAnalyzer.h"
#include "JackAnalyzer/JackAnalyzerForDirectories.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Expected 1 argument: file path for Compiler");
    }

    FILE *fp = fopen(argv[1],"r");
    char *intermediate = "./data/output/intermediate.xml";
    RoutineTable *routine_table = routine_table_constructor(200);
    ClassTable *class_table = class_table_constructor(50);
    if (fp != NULL) {
        fclose(fp);
        operateFirstPass(argv[1],intermediate,routine_table,class_table);
        return 0;
    }
    return operateOnDirectory(argv[1],intermediate,routine_table,class_table);
    return 0;
}
