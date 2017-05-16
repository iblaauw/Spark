#pragma once

#include <map>
#include <string>

#include "llvm/IR/Function.h"
#include "LangType.h"

class SymbolTable
{
private:
    SymbolTable* parent;

    std::map<std::string, llvm::Function*> funcTable;
    std::map<std::string, LangType*> typeTable;
public:
    SymbolTable();
    SymbolTable(SymbolTable* parent);

    void SetParent(SymbolTable* parent);

    void AddFunction(std::string name, llvm::Function* func);
    bool ContainsFunction(std::string name) const;
    // Gets the function. Returns null if not found
    llvm::Function* GetFunction(std::string name) const;

    void AddType(std::string name, LangType* type);
    bool ContainsType(std::string name) const;
    // Gets the type. Returns null if not found
    LangType* GetType(std::string name) const;
};


