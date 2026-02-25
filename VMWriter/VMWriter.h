//
// Created by Owner on 25/02/2026.
//

#ifndef VMWRITER_H
#define VMWRITER_H
#include <stdio.h>
enum Segment {
    CONST, ARG,
    LOCAL, STATIC,
    THIS, THAT,
    POINTER, TEMP
} typedef Segment;
enum Command {
    ADD, SUB,
    NEG, EQ,
    GT, LT,
    AND, OR, NOT
} typedef Command;
FILE* fptr;
void constructor(const char pathStr[],const int strLength);
void writePop(const Segment segment, const int index);
void writeArithmetic(const Command command);
void writeLabel(const char label[]);
void writeGoTo(const char label[]);
void writeIf(const char label[]);
void writeCall(const char name[], const int nArgs);
void writeFunction(const char name[], const int nLocals);
void writeReturn();
void close();


#endif //VMWRITER_H
