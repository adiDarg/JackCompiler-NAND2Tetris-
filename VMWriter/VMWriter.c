//
// Created by Owner on 25/02/2026.
//

#include "VMWriter.h"

#include <stdlib.h>

void constructor(const char pathStr[],const int strLength) {
    fptr = fopen(pathStr,"w");
    if (fptr == NULL) {
        printf("Failed to open file for writing: %s", pathStr);
    }
}
char* getSegmentString(const Segment segment) {
    switch (segment) {
        case CONST:
            return "constant";
        case ARG:
            return "argument";
        case LOCAL:
            return "local";
        case STATIC:
            return "static";
        case THIS:
            return "this";
        case THAT:
            return "that";
        case POINTER:
            return "pointer";
        case TEMP:
            return "temp";
        default:
            return "";
    }
}
char* getCommandString(const Command command) {
    switch (command) {
        case ADD:
            return "add";
        case SUB:
            return "sub";
        case NEG:
            return "neg";
        case EQ:
            return "eq";
        case GT:
            return "gt";
        case LT:
            return "lt";
        case AND:
            return "and";
        case OR:
            return "or";
        case NOT:
            return "not";
        default:
            return "";
    }
}
void writePop(const Segment segment, const int index) {
    const char *segmentString = getSegmentString(segment);
    fprintf(fptr,"pop %s %d\n",segmentString,index);
}
void writeArithmetic(const Command command) {
    const char *commandString = getCommandString(command);
    fprintf(fptr,"%s\n",commandString);
}
void writeLabel(const char label[]) {
    fprintf(fptr,"(%s)\n",label);
}
void writeGoTo(const char label[]) {
    fprintf(fptr,"goto %s\n",label);
}
void writeIf(const char label[]) {
    fprintf(fptr,"if-goto %s\n",label);
}
void writeCall(const char name[], const int nArgs) {
    fprintf(fptr,"call %s %d\n",name,nArgs);
}
void writeFunction(const char name[], const int nLocals) {
    fprintf(fptr,"function %s %d\n",name,nLocals);
}
void writeReturn() {
    fprintf(fptr,"return\n");
}
void close() {
    fclose(fptr);
}




