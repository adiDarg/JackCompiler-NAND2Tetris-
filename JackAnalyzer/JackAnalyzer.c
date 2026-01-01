//
// Created by Owner on 17/12/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../JackTokenizer/jackTokenizer.h"
#include "JackAnalyzer.h"

#include "../CompilationEngine/CompilationEngine.h"

void operateOnFile(char path[],char destination[]) {
    struct stat path_stat;
    stat(path, &path_stat);
    const int reg = (path_stat.st_mode & _S_IFREG);
    if (reg) {
        FILE *fp = fopen(path, "rb");   // binary mode
        if (!fp) {
            printf("Can't open file");
            return;
        }

        fseek(fp, 0, SEEK_END);
        const long fsize = ftell(fp);
        rewind(fp);

        char *source_code = malloc(fsize + 1);
        if (source_code) {
            const size_t read = fread(source_code,1,fsize,fp);
            source_code[read] = '\0';
            JackTokenizer *jack_tokenizer = JT_Constructor(source_code);
            CompilationEngine* compilation_engine = Construct_Engine(jack_tokenizer);
            const int success = CompileClass(compilation_engine);
            printf(success? "Success!\n": "Fail!\n");
            printf(!success? compilation_engine->error: "");
            printf("\n");
            struct stat path_stat_dest;
            stat(destination, &path_stat_dest);
            const int reg_dest = S_ISREG(path_stat_dest.st_mode);
            if (reg_dest) {
                FILE *fp_dest = fopen(destination, "wb");   // binary mode
                if (!fp_dest) return;

                fseek(fp, 0, SEEK_END);
                rewind(fp_dest);
                const int printSuccess = fprintf(fp_dest,success? compilation_engine->out: "<compilationError></compilationError>");
                printf(printSuccess? "written to file":"failed to write to file");
            }
            free(source_code);
        }
        fclose(fp);
    }
    else {
        printf("Problem with file path");
    }
}
