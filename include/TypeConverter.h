#pragma once

#include <type_traits>

#include "llvm/IR/Type.h"
#include "llvm/IR/Constant.h"

#include "LLVMManager.h"


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

}

