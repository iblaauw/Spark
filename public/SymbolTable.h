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
    std::map<std::string, T*> table;
public:
    SymbolSubtable() : parent(nullptr) {}
    SymbolSubtable(SymbolSubtable<T>* parent) : parent(parent) {}

    ~SymbolSubtable()
    {
        for (auto kv_pair : table)
        {
            delete kv_pair.second;
        }
        table.clear();
    }

    void SetParent(SymbolSubtable<T>* parent)
    {
        this->parent = parent;
    }

    template <class... Args>
    T* Create(std::string name, Args... args)
    {
        T* val = new T(args...);
        table[name] = val;
        return val;
    }

    void Add(std::string name, T* val)
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

    T* Get(std::string name) const
    {
        auto iter = table.find(name);
        if (iter != table.end())
            return iter->second;

        if (parent == nullptr)
            return nullptr;

        return parent->Get(name);
    }
};

class SymbolTable
{
private:
    SymbolTable* parent;

public:
    SymbolSubtable<Function> functions;
    SymbolSubtable<LangType> types;
    SymbolSubtable<Variable> variables;

    SymbolTable();
    SymbolTable(SymbolTable* parent);

    void SetParent(SymbolTable* parent);
};


