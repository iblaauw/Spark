#pragma once

#include "llvm/IR/IRBuilder.h"

#include "SymbolTable.h"
#include "LLVMManager.h"

class Function;
class Variable;

class CompileContext
{
public:
    SymbolTable* symbolTable;
    SymbolTable* builtins;
    llvm::IRBuilder<> builder;
    Function* currentFunction;

    CompileContext() : symbolTable(nullptr), builtins(nullptr), builder(Spark::LLVMManager::Context()), currentFunction(nullptr) {}
};



