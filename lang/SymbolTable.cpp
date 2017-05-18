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

//void SymbolTable::AddFunction(std::string name, Function* func)
//{
//    funcTable[name] = func;
//}
//
//bool SymbolTable::ContainsFunction(std::string name) const
//{
//    if (funcTable.count(name) > 0)
//        return true;
//
//    if (parent == nullptr)
//        return false;
//
//    return parent->ContainsFunction(name);
//}
//
//Function* SymbolTable::GetFunction(std::string name) const
//{
//    auto iter = funcTable.find(name);
//    if (iter != funcTable.end())
//        return iter->second;
//
//    if (parent == nullptr)
//        return nullptr;
//
//    return parent->GetFunction(name);
//}
//
//
//void SymbolTable::AddType(std::string name, LangType* type)
//{
//    typeTable[name] = type;
//}
//
//bool SymbolTable::ContainsType(std::string name) const
//{
//    if (typeTable.count(name) > 0)
//        return true;
//
//    if (parent == nullptr)
//        return false;
//
//    return parent->ContainsType(name);
//}
//
//LangType* SymbolTable::GetType(std::string name) const
//{
//    auto iter = typeTable.find(name);
//    if (iter != typeTable.end())
//        return iter->second;
//
//    if (parent == nullptr)
//        return nullptr;
//
//    return parent->GetType(name);
//}
//
//
//
//void SymbolTable::AddVariable(std::string name, Ptr<Variable> type)
//{
//    varTable[name] = type;
//}
//
//bool SymbolTable::ContainsVariable(std::string name) const
//{
//    if (varTable.count(name) > 0)
//        return true;
//
//    if (parent == nullptr)
//        return false;
//
//    return parent->ContainsVariable(name);
//}
//
//Ptr<Variable> SymbolTable::GetVariable(std::string name) const
//{
//    auto iter = varTable.find(name);
//    if (iter != varTable.end())
//        return iter->second;
//
//    if (parent == nullptr)
//        return nullptr;
//
//    return parent->GetVariable(name);
//}

