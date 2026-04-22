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
        SemanticAnalysis/SymbolTable/SymbolTable.c
        SemanticAnalysis/SymbolTable/SymbolTable.h
        VMWriter/VMWriter.c
        VMWriter/VMWriter.h
        CompilationEngine/Token/Token.h
        CompilationEngine/DataType.h
        CompilationEngine/Token/Token.c
        CompilationEngine/AbstractSyntaxTree/AbstractSyntaxTree.c
        CompilationEngine/AbstractSyntaxTree/AbstractSyntaxTree.h
        SemanticAnalysis/SemanticAnalyzer.c
        SemanticAnalysis/SemanticAnalyzer.h
        SemanticAnalysis/RoutineTable/RoutineTable.c
        SemanticAnalysis/RoutineTable/RoutineTable.h
        SemanticAnalysis/ClassTable/ClassTable.c
        SemanticAnalysis/ClassTable/ClassTable.h
        SemanticAnalysis/hashingFunctions/HashingFunctions.c
        SemanticAnalysis/hashingFunctions/HashingFunctions.h
)
