cmake_minimum_required(VERSION 3.28)
project(JackCompiler C)

set(CMAKE_C_STANDARD 11)

add_executable(JackCompiler main.c
        JackAnalyzer/JackAnalyzer.c
        JackTokenizer/JackTokenizer.c
        Keyword/KeyWord.c
        Keyword/keyword.h
        JackTokenizer/jackTokenizer.h
        CompilationEngine/CompilationEngine.c
        CompilationEngine/CompilationEngine.h
        JackAnalyzer/JackAnalyzer.h)
