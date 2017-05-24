#pragma once

class LangType;

class SpecialTypeCache
{
private:
    LangType* owner;
    LangType* pointerType;
public:
    SpecialTypeCache(LangType* owner) : owner(owner), pointerType(nullptr) {}
    ~SpecialTypeCache();

    LangType* GetPointer();
};


