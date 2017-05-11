#pragma once

#include <type_traits>

#include "llvm/IR/Type.h"

namespace Spark
{
    using llvm::Type;
    class TypeConverter
    {
    public:
        template <class T>
        static Type* Get();

        static Type* GetSized(unsigned int size)
        {
            return llvm::Type::getIntNTy(LLVMManager::Context(), size);
        }

        template < class T, typename std::enable_if<std::is_floating_point<T>::value>::type* Enable = nullptr>
        static llvm::Constant* Create(T val)
        {
            Type* type = Get<T>();
            return llvm::ConstantFP::get(type, val);
        }

        template < class T, typename std::enable_if<!std::is_floating_point<T>::value>::type* Enable = nullptr>
        static llvm::Constant* Create(T val)
        {
            Type* type = Get<T>();
            return llvm::ConstantInt::getSigned(type, val);
        }

    };

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

