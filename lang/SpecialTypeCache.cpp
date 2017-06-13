#include "SpecialTypeCache.h"

#include "LangType.h"

SpecialTypeCache::~SpecialTypeCache()
{
    delete pointerType;
    pointerType = nullptr;
}

LangType* SpecialTypeCache::GetPointer()
{
    if (pointerType == nullptr)
    {
        pointerType = new PointerType(owner);
    }

    return pointerType;
}

LangType* SpecialTypeCache::GetArray(int size)
{
    auto it = arrayMap.find(size);
    if (it != arrayMap.end())
        return it->second;

    LangType* type = new ArrayType(owner, size);
    arrayMap[size] = type;
    return type;
}

