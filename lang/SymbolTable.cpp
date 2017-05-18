#include "SymbolTable.h"


SymbolTable::SymbolTable() : parent(nullptr) {}

SymbolTable::SymbolTable(SymbolTable* parent) : parent(nullptr)
{
    SetParent(parent);
}

void SymbolTable::SetParent(SymbolTable* parent)
{
    this->parent = parent;
    if (parent != nullptr)
    {
        functions.SetParent(&(parent->functions));
        types.SetParent(&(parent->types));
        variables.SetParent(&(parent->variables));
    }
}

