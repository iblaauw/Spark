#include "TypeConverter.h"

namespace Spark
{
    // Specializations of TypeConverter::Create

    template<>
    Type* TypeConverter::Get<bool>()
    {
        return llvm::Type::getInt1Ty(LLVMManager::Context());
    }

    template<>
    Type* TypeConverter::Get<char>()
    {
        return llvm::Type::getInt8Ty(LLVMManager::Context());
    }

    template<>
    Type* TypeConverter::Get<short>()
    {
        return llvm::Type::getInt16Ty(LLVMManager::Context());
    }

    template<>
    Type* TypeConverter::Get<int>()
    {
        return llvm::Type::getInt32Ty(LLVMManager::Context());
    }

    template<>
    Type* TypeConverter::Get<float>()
    {
        return llvm::Type::getFloatTy(LLVMManager::Context());
    }

    template<>
    Type* TypeConverter::Get<double>()
    {
        return llvm::Type::getDoubleTy(LLVMManager::Context());
    }

    template<>
    Type* TypeConverter::Get<void>()
    {
        return llvm::Type::getVoidTy(LLVMManager::Context());
    }
}


