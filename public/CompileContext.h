#pragma once

#include "llvm/IR/IRBuilder.h"

#include "SymbolTable.h"
#include "LLVMManager.h"

class CompileContext
{
public:
    SymbolTable symbolTable;
    llvm::IRBuilder<> builder;

    CompileContext() : symbolTable(nullptr), builder(Spark::LLVMManager::Context()) {}
};



