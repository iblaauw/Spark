#pragma once

#include <map>

class LangType;
namespace llvm { class Type; }

class SpecialTypeCache
{
private:
    LangType* owner;
    LangType* pointerType;
    llvm::Type* arrayType;
    std::map<int, LangType*> arrayMap;
public:
    SpecialTypeCache(LangType* owner) : owner(owner), pointerType(nullptr), arrayType(nullptr) {}
    ~SpecialTypeCache();

    LangType* GetPointer();
    LangType* GetArray(int size);
};


