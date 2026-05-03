//
// Created by Owner on 17/12/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../JackTokenizer/jackTokenizer.h"
#include "JackAnalyzer.h"
#include "../CompilationEngine/CompilationEngine.h"
#include "../SemanticAnalysis/SemanticAnalyzer.h"

SemanticData *operateFirstPass(char path[],char intermediate[],RoutineTable *routine_table,ClassTable *class_table) {
    SemanticData *result = NULL;
    struct stat path_stat;
    stat(path, &path_stat);
    const int reg = (path_stat.st_mode & _S_IFREG);
    if (reg) {
        FILE *fp = fopen(path, "rb");   // binary mode
        if (fp == NULL) {
            printf("Can't open file");
            return NULL;
        }

        fseek(fp, 0, SEEK_END);
        const long fsize = ftell(fp);
        rewind(fp);

        char *source_code = malloc(fsize + 1);
        if (source_code) {
            const size_t read = fread(source_code,1,fsize,fp);
            source_code[read] = '\0';

            JackTokenizer *jack_tokenizer = JT_Constructor(source_code,30);
            CompilationEngine* compilation_engine = Construct_Engine(jack_tokenizer);

            int success = CompileClass(compilation_engine);
            NodeAST *root = compilation_engine->ast_root;
            //Here, if success then we run semantic analysis

            printf("Syntax analysis\n");
            printf(success? "Success!\n": "Fail!\n");
            printf(!success? compilation_engine->error: "");
            printf("\n");
            if (success) {
                result = construct_semantic_data(root,100,100,routine_table,class_table,30);
            }

            printf("%s\n",intermediate);
            FILE *fp_intermediate = fopen(intermediate, "w");
            if (fp_intermediate == NULL) {
                printf("failed to write intermediate file\n");
            }
            else {
                const int printSuccess = fprintf(fp_intermediate,success? compilation_engine->out: "<compilationError></compilationError>");
                printf(printSuccess? "written to file\n":"failed to write to file\n");
                fclose(fp_intermediate);
            }

            free(source_code);
            free(compilation_engine);
            free(jack_tokenizer);
        }
        fclose(fp);
    }
    else {
        printf("Problem with file path: %s\n",path);
    }
    return result;
}
void operateSecondPass(SemanticData *semantic_data) {
    const char success = Analyze(semantic_data);
    printf("Semantic analysis\n");
    printf(success? "Success!\n": "Fail!\n");
    printf(!success? semantic_data->error: "");
    printf("\n");
}