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

