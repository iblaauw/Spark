#pragma once

#include "llvm/IR/IRBuilder.h"

#include "SymbolTable.h"
#include "LLVMManager.h"

class Function;

class CompileContext
{
public:
    SymbolTable* symbolTable;
    llvm::IRBuilder<> builder;
    Function* currentFunction;

    CompileContext() : symbolTable(nullptr), builder(Spark::LLVMManager::Context()), currentFunction(nullptr) {}
};



