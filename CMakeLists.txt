cmake_minimum_required(VERSION 3.28)
project(JackCompiler C)

set(CMAKE_C_STANDARD 11)

add_executable(JackCompiler main.c
        JackAnalyzer.c
        JackTokenizer.c
        KeyWord.c
        KeyWord.h
        JackTokenizer.h
        CompilationEngine.c
        CompilationEngine.h
        JackAnalyzer.h)
