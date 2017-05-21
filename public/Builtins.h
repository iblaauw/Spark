#pragma once

#include "llvm/IR/Function.h"

#include "SymbolTable.h"

void AddBuiltinTypes(SymbolTable& symbolTable);

void AddBuiltinFunctions(SymbolTable& symbolTable);


