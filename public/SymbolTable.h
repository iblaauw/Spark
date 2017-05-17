#pragma once

#include <map>
#include <string>

#include "Function.h"
#include "LangType.h"

class SymbolTable
{
private:
    SymbolTable* parent;

    std::map<std::string, Function*> funcTable;
    std::map<std::string, LangType*> typeTable;
public:
    SymbolTable();
    SymbolTable(SymbolTable* parent);

    void SetParent(SymbolTable* parent);

    void AddFunction(std::string name, Function* func);
    bool ContainsFunction(std::string name) const;
    // Gets the function. Returns null if not found
    Function* GetFunction(std::string name) const;

    void AddType(std::string name, LangType* type);
    bool ContainsType(std::string name) const;
    // Gets the type. Returns null if not found
    LangType* GetType(std::string name) const;
};


