//
// Created by Owner on 03/05/2026.
//

#include "JackAnalyzerForDirectories.h"
#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>

#include "JackAnalyzer.h"
#include "../SemanticAnalysis/FunctionLoader/Loader.h"

int operateOnDirectory(char path[],char intermediate[],RoutineTable *routine_table, ClassTable *class_table) {
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s/*.jack", path);
    const LPCSTR fileName = searchPath;
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        printf("No .jack files found or directory missing.\n");
        return 1;
    }
    const int sec_pass_data_len = 20;
    SemanticData *sec_pass_data[sec_pass_data_len];
    int sec_pass_data_index = 0;
    do {
        // Skip directories if they happen to match
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char *fullName = malloc(strlen(path) + 1 + strlen(findData.cFileName) + 1);
            strcpy(fullName,path);
            strcat(fullName,"\\");
            strcat(fullName,findData.cFileName);
            printf("Found file: %s\n", fullName);
            if (sec_pass_data_index >= sec_pass_data_len) {
                printf("Too many files\n");
                for (int i = 0; i < sec_pass_data_index; i++) {
                    destroySemanticData(sec_pass_data[i],0);
                }
                destroyRoutineTable(routine_table);
                destroyClassTable(class_table);
                FindClose(hFind);
                return 1;
            }
            char uniquePath[100];

            // Combine the base path with the index safely
            snprintf(uniquePath, sizeof(uniquePath), "%s_%d.xml", intermediate, sec_pass_data_index);

            // Pass uniquePath to your function
            sec_pass_data[sec_pass_data_index++] = operateFirstPass(fullName, uniquePath, routine_table, class_table);
            SemanticData *data = sec_pass_data[sec_pass_data_index-1];
            LoadToTables(data->root,data->root->children[1]->token->info.identifier,
                data->routine_table,data->class_table);
            free(fullName);
        }
    } while (FindNextFile(hFind, &findData));
    for (int i = 0; i < sec_pass_data_index; i++) {
        operateSecondPass(sec_pass_data[i]);
        destroySemanticData(sec_pass_data[i],0);
    }
    destroyRoutineTable(routine_table);
    destroyClassTable(class_table);
    FindClose(hFind);
    return 0;
}
