#include <stdio.h>
#include <stdlib.h>
#include "JackAnalyzer/JackAnalyzer.h"
#include "JackAnalyzer/JackAnalyzerForDirectories.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Expected usage: program.exe path_to_jack_files_dir path_to_intermediate_output_dest");
        return 0;
    }

    FILE *fp = fopen(argv[1],"r");
    char *intermediate = argv[2];
    RoutineTable *routine_table = routine_table_constructor(200);
    ClassTable *class_table = class_table_constructor(50);
    if (fp != NULL) {
        fclose(fp);
        operateFirstPass(argv[1],intermediate,routine_table,class_table);
        return 0;
    }
    return operateOnDirectory(argv[1],intermediate,routine_table,class_table);
}
