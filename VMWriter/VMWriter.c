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
        case SEG_CONST:
            return "constant";
        case SEG_ARG:
            return "argument";
        case SEG_LOCAL:
            return "local";
        case SEG_STATIC:
            return "static";
        case SEG_THIS:
            return "this";
        case SEG_THAT:
            return "that";
        case SEG_POINTER:
            return "pointer";
        case SEG_TEMP:
            return "temp";
        default:
            return "";
    }
}
char* getCommandString(const Command command) {
    switch (command) {
        case CMD_ADD:
            return "add";
        case CMD_SUB:
            return "sub";
        case CMD_NEG:
            return "neg";
        case CMD_EQ:
            return "eq";
        case CMD_GT:
            return "gt";
        case CMD_LT:
            return "lt";
        case CMD_AND:
            return "and";
        case CMD_OR:
            return "or";
        case CMD_NOT:
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




