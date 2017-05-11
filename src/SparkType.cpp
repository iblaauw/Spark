#include "SparkType.h"

#include "llvm/IR/DerivedTypes.h"

#include "LLVMManager.h"

namespace Spark
{
    /*static*/ std::map<std::string, SparkType*> SparkType::mapping;

    SparkType::SparkType(std::string name) : name(name)
    {
        mapping[name] = this;
    }

    /*static*/ SparkType& SparkType::Lookup(std::string name)
    {
        return *(mapping.at(name));
    }

    /*static*/ bool SparkType::Exists(std::string name)
    {
        return mapping.count(name) > 0;
    }

    llvm::Type* BasicType::Resolve()
    {
        return type;
    }

    llvm::Type* CompoundType::Resolve()
    {
        std::vector<llvm::Type*> resolvedTypes;
        for (SparkType* sub : subtypes)
        {
            auto t = sub->Resolve();
            resolvedTypes.push_back(t);
        }

        return llvm::StructType::get(LLVMManager::Context(), resolvedTypes);
    }

    llvm::Type* PointerType::Resolve()
    {
        return subtype->Resolve()->getPointerTo();
    }

    llvm::Type* ArrayType::Resolve()
    {
        return llvm::ArrayType::get(subtype->Resolve(), numElems);
    }

}


