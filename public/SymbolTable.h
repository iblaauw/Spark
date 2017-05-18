#pragma once

#include <map>
#include <string>

#include "Function.h"
#include "LangType.h"
#include "Variable.h"
#include "PtrUtils.h"

template <class T>
class SymbolSubtable
{
private:
    SymbolSubtable<T>* parent;
    std::map<std::string, Ptr<T>> table;
public:
    SymbolSubtable() : parent(nullptr) {}
    SymbolSubtable(SymbolSubtable<T>* parent) : parent(parent) {}

    void SetParent(SymbolSubtable<T>* parent)
    {
        this->parent = parent;
    }

    void Add(std::string name, Ptr<T> val)
    {
        table[name] = val;
    }

    bool Contains(std::string name) const
    {
        if (table.count(name) > 0)
            return true;

        if (parent == nullptr)
            return false;

        return parent->Contains(name);
    }

    Ptr<T> Get(std::string name) const
    {
        auto iter = table.find(name);
        if (iter != table.end())
            return iter->second;

        if (parent == nullptr)
            return nullptr;

        return parent->Get(name);
    }

    template <class... Args>
    void Emplace(std::string name, Args... args)
    {
        Add(name, std::make_shared<T>(args...));
    }
};

class SymbolTable
{
private:
    SymbolTable* parent;

    std::map<std::string, Function*> funcTable;
    std::map<std::string, LangType*> typeTable;
    std::map<std::string, Ptr<Variable>> varTable;
public:
    SymbolSubtable<Function> functions;
    SymbolSubtable<LangType> types;
    SymbolSubtable<Variable> variables;

    SymbolTable();
    SymbolTable(SymbolTable* parent);

    void SetParent(SymbolTable* parent);

    //void AddFunction(std::string name, Function* func);
    //bool ContainsFunction(std::string name) const;
    //// Gets the function. Returns null if not found
    //Function* GetFunction(std::string name) const;

    //void AddType(std::string name, LangType* type);
    //bool ContainsType(std::string name) const;
    //// Gets the type. Returns null if not found
    //LangType* GetType(std::string name) const;

    //void AddVariable(std::string name, Ptr<Variable> var);
    //bool ContainsVariable(std::string name) const;
    //Ptr<Variable> GetVariable(std::string name) const;
};


