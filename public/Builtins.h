#pragma once

#include "llvm/IR/Function.h"

#include "SymbolTable.h"

llvm::Function* DeclarePrintf(SymbolTable& symbolTable);

void AddBuiltinTypes(SymbolTable& symbolTable);



