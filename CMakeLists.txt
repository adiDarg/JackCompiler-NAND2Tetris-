cmake_minimum_required(VERSION 3.28)
project(JackCompiler C)

set(CMAKE_C_STANDARD 11)

add_executable(JackCompiler main.c
        JackAnalyzer/JackAnalyzer.c
        JackTokenizer/JackTokenizer.c
        JackTokenizer/Keyword/KeyWord.c
        JackTokenizer/Keyword/keyword.h
        JackTokenizer/jackTokenizer.h
        CompilationEngine/CompilationEngine.c
        CompilationEngine/CompilationEngine.h
        JackAnalyzer/JackAnalyzer.h
        SymbolTable/SymbolTable.c
        SymbolTable/SymbolTable.h
        VMWriter/VMWriter.c
        VMWriter/VMWriter.h
        CompilationEngine/Token/Token.h
        CompilationEngine/DataType.h
        CompilationEngine/Token/Token.c
        CompilationEngine/AbstractSyntaxTree/AbstractSyntaxTree.c
        CompilationEngine/AbstractSyntaxTree/AbstractSyntaxTree.h)
