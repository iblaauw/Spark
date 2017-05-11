#pragma once

#include <string>
#include <map>
#include "llvm/IR/Type.h"

namespace Spark
{
    template<class Type, class... Args>
    Type* CreateType(Args... args)
    {
        return new Type(args...);
    }

    class SparkType
    {
    private:
        static std::map<std::string, SparkType*> mapping;
        std::string name;
    protected:
        SparkType(std::string name);
    public:
        inline std::string GetName() const { return name; }
        virtual llvm::Type* Resolve() = 0;

        static SparkType& Lookup(std::string name);
        static bool Exists(std::string name);
    };


    class BasicType : public SparkType
    {
    private:
        llvm::Type* type;
        BasicType(std::string name, llvm::Type* type) : SparkType(name), type(type) {}
    public:
        virtual llvm::Type* Resolve() override;

        template <class... Args>
        friend BasicType* CreateType(Args... args);
    };

    class CompoundType : public SparkType
    {
    private:
        std::vector<SparkType*> subtypes;
        CompoundType(std::string name, const std::vector<SparkType*>& subtypes) : SparkType(name), subtypes(subtypes) {}
    public:
        virtual llvm::Type* Resolve() override;

        template <class... Args>
        friend CompoundType* CreateType(Args... args);
    };

    class PointerType : public SparkType
    {
    private:
        SparkType* subtype;
        PointerType(std::string name, SparkType* subtype) : SparkType(name), subtype(subtype) {}
    public:
        virtual llvm::Type* Resolve() override;

        template <class... Args>
        friend PointerType* CreateType(Args... args);
    };

    class ArrayType : public SparkType
    {
    private:
        SparkType* subtype;
        int numElems;
        ArrayType(std::string name, SparkType* subtype, int numElems)
            : SparkType(name), subtype(subtype), numElems(numElems) {}
    public:
        virtual llvm::Type* Resolve() override;

        template <class... Args>
        friend ArrayType* CreateType(Args... args);
    };
}

