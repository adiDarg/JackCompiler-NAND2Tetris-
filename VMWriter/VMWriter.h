//
// Created by Owner on 25/02/2026.
//

#ifndef VMWRITER_H
#define VMWRITER_H
#include <stdio.h>
enum Segment {
    SEG_CONST, SEG_ARG,
    SEG_LOCAL, SEG_STATIC,
    SEG_THIS, SEG_THAT,
    SEG_POINTER, SEG_TEMP
} typedef Segment;
enum Command {
    CMD_ADD, CMD_SUB,
    CMD_NEG, CMD_EQ,
    CMD_GT, CMD_LT,
    CMD_AND, CMD_OR, CMD_NOT
} typedef Command;
typedef struct {
    FILE* fptr;
} VMWriter;
VMWriter* constructor(const char pathStr[],const int strLength);
void writePop(const VMWriter *self, const Segment segment, const int index);
void writeArithmetic(const VMWriter *self, const Command command);
void writeLabel(const VMWriter *self, const char label[]);
void writeGoTo(const VMWriter *self, const char label[]);
void writeIf(const VMWriter *self, const char label[]);
void writeCall(const VMWriter *self, const char name[], const int nArgs);
void writeFunction(const VMWriter *self, const char name[], const int nLocals);
void writeReturn(const VMWriter *self);
void close(VMWriter *self);


#endif //VMWRITER_H
