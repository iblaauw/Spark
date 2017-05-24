#pragma once

#include "llvm/IR/Type.h"
#include "PtrUtils.h"
#include "SpecialTypeCache.h"

class CompileContext;

class LangType
{
private:
    SpecialTypeCache cache;
public:
    LangType() : cache(this) {}
    virtual ~LangType() {}
    virtual std::string GetName() const = 0;
    virtual llvm::Type* GetIR() const = 0;

    virtual bool IsPointer() const { return false; }

    virtual bool IsAssignableFrom(LangType* otherType) const = 0;
    virtual void InsertConversion(LangType* fromType, CompileContext& context) const = 0;

    LangType* GetPointerTo() { return cache.GetPointer(); }

    // TODO: add inheritance and const-ness
    // TODO: add a map of members
};

class BasicType : public LangType
{
private:
    std::string name;
    llvm::Type* type;
public:
    BasicType(std::string name);

    std::string GetName() const override { return name; }

    llvm::Type* GetIR() const override { return type; }
    void SetIR(llvm::Type* type) { this->type = type; }

    bool IsAssignableFrom(LangType* otherType) const override;
    void InsertConversion(LangType* fromType, CompileContext& context) const override;
};

class PointerType : public LangType
{
private:
    LangType* subType;

    PointerType(LangType* subType);
public:
    std::string GetName() const override { return subType->GetName() + "*"; }
    llvm::Type* GetIR() const override;

    bool IsPointer() const override { return true; }

    bool IsAssignableFrom(LangType* otherType) const override;
    void InsertConversion(LangType* fromType, CompileContext& context) const override;

    LangType* GetSubType() const { return subType; }

    friend class SpecialTypeCache;
};

