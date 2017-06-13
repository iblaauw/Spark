#pragma once

#include <map>

class LangType;

class SpecialTypeCache
{
private:
    LangType* owner;
    LangType* pointerType;
    std::map<int, LangType*> arrayMap;
public:
    SpecialTypeCache(LangType* owner) : owner(owner), pointerType(nullptr) {}
    ~SpecialTypeCache();

    LangType* GetPointer();
    LangType* GetArray(int size);
};


