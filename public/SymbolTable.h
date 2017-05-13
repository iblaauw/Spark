#pragma once

#include <map>
#include <string>

#include "llvm/IR/Function.h"

class SymbolTable
{
private:
    SymbolTable* parent;

    std::map<std::string, llvm::Function*> funcTable;
public:
    SymbolTable();
    SymbolTable(SymbolTable* parent);

    void SetParent(SymbolTable* parent);

    void AddFunction(std::string name, llvm::Function* func);
    bool ContainsFunction(std::string name) const;
    // Gets the function. Returns null if not found
    llvm::Function* GetFunction(std::string name) const;
};


