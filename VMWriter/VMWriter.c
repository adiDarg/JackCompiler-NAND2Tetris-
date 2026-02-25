//
// Created by Owner on 25/02/2026.
//

#include "VMWriter.h"

#include <stdlib.h>

VMWriter* constructor(const char pathStr[],const int strLength) {
    VMWriter* vm_writer = malloc(sizeof(VMWriter));
    vm_writer->fptr = fopen(pathStr,"w");
    if (vm_writer->fptr == NULL) {
        printf("Failed to open file for writing: %s", pathStr);
    }
    return vm_writer;
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
void writePop(const VMWriter *self, const Segment segment, const int index) {
    const char *segmentString = getSegmentString(segment);
    fprintf(self->fptr,"pop %s %d\n",segmentString,index);
}
void writeArithmetic(const VMWriter *self, const Command command) {
    const char *commandString = getCommandString(command);
    fprintf(self->fptr,"%s\n",commandString);
}
void writeLabel(const VMWriter *self, const char label[]) {
    fprintf(self->fptr,"(%s)\n",label);
}
void writeGoTo(const VMWriter *self, const char label[]) {
    fprintf(self->fptr,"goto %s\n",label);
}
void writeIf(const VMWriter *self, const char label[]) {
    fprintf(self->fptr,"if-goto %s\n",label);
}
void writeCall(const VMWriter *self, const char name[], const int nArgs) {
    fprintf(self->fptr,"call %s %d\n",name,nArgs);
}
void writeFunction(const VMWriter *self, const char name[], const int nLocals) {
    fprintf(self->fptr,"function %s %d\n",name,nLocals);
}
void writeReturn(const VMWriter *self) {
    fprintf(self->fptr,"return\n");
}
void close(VMWriter *self) {
    fclose(self->fptr);
    free(self);
}




