#pragma once

#include "llvm/IR/Type.h"
#include "PtrUtils.h"
#include "SpecialTypeCache.h"
#include "UnknownPtr.h"

class CompileContext;
class LValue;
class RValue;

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
    virtual bool IsArray() const { return false; }

    virtual bool IsAssignableFrom(LangType* otherType) const = 0;
    virtual void InsertConversion(LangType* fromType, CompileContext& context) const = 0;

    virtual void CallConstructor(UnknownPtr<LValue> lval,
                                std::vector<UnknownPtr<RValue>>& arguments,
                                CompileContext& context) { }

    LangType* GetPointerTo() { return cache.GetPointer(); }
    LangType* GetArrayOf(int size) { return cache.GetArray(size); }

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

class ArrayType : public LangType
{
private:
    llvm::Type* type;
    LangType* subType;
    int size;

    ArrayType(LangType* subType, int size);
public:
    std::string GetName() const override { return subType->GetName() + "[" + std::to_string(size) + "]"; }
    llvm::Type* GetIR() const override;

    bool IsArray() const override { return true; }

    bool IsAssignableFrom(LangType* otherType) const override;
    void InsertConversion(LangType* fromType, CompileContext& context) const override;

    void CallConstructor(UnknownPtr<LValue> lval,
                        std::vector<UnknownPtr<RValue>>& arguments,
                        CompileContext& context) override;

    LangType* GetElementType() const { return subType; }
    int GetSize() const { return size; }


    friend class SpecialTypeCache;
};


