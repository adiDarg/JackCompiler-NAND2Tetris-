cmake_minimum_required(VERSION 3.28)
project(JackCompiler C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
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
        "Code Generation/VMWriter/VMWriter.c"
        "Code Generation/VMWriter/VMWriter.h"
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
        SemanticAnalysis/FunctionLoader/Loader.c
        SemanticAnalysis/FunctionLoader/Loader.h
        JackAnalyzer/JackAnalyzerForDirectories.c
        JackAnalyzer/JackAnalyzerForDirectories.h
        debugging/debuggingTools.c
        debugging/debuggingTools.h
        "Code Generation/CodeGenerator/CodeGenerator.c"
        "Code Generation/CodeGenerator/CodeGenerator.h"
        "Code Generation/CodeGenerator/CodeGenerator.h"
)