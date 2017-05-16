#include "SymbolTable.h"

SymbolTable::SymbolTable() : parent(nullptr) {}

SymbolTable::SymbolTable(SymbolTable* parent) : parent(parent)
{}

void SymbolTable::SetParent(SymbolTable* parent)
{
    this->parent = parent;
}

void SymbolTable::AddFunction(std::string name, llvm::Function* func)
{
    funcTable[name] = func;
}

bool SymbolTable::ContainsFunction(std::string name) const
{
    return funcTable.count(name) > 0;
}

llvm::Function* SymbolTable::GetFunction(std::string name) const
{
    auto iter = funcTable.find(name);
    if (iter != funcTable.end())
        return iter->second;

    if (parent == nullptr)
        return nullptr;

    return parent->GetFunction(name);
}


void SymbolTable::AddType(std::string name, LangType* type)
{
    typeTable[name] = type;
}

bool SymbolTable::ContainsType(std::string name) const
{
    return typeTable.count(name) > 0;
}

LangType* SymbolTable::GetType(std::string name) const
{
    auto iter = typeTable.find(name);
    if (iter != typeTable.end())
        return iter->second;

    if (parent == nullptr)
        return nullptr;

    return parent->GetType(name);
}



