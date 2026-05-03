//
// Created by Owner on 18/12/2025.
//

#ifndef JACKANALYZER_H
#define JACKANALYZER_H
#include "../SemanticAnalysis/SemanticAnalyzer.h"
SemanticData *operateFirstPass(char path[],char intermediate[],RoutineTable *routine_table,ClassTable *class_table);
void operateSecondPass(SemanticData *semantic_data);
#endif //JACKANALYZER_H
