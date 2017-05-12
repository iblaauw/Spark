#pragma once

#include <memory>

#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"

#include  "Utils.h"

namespace llvm
{
class Target;
}

namespace Spark
{
    class LLVMManager
    {
    private:
        llvm::LLVMContext globalContext;
        llvm::Module* currentModule;
        static LLVMManager* instance;

        LLVMManager(std::string name);
    public:
        static void Init(std::string name);
        static inline LLVMManager& Instance() { return *instance; }
        static inline llvm::LLVMContext& Context() { return instance->globalContext; }


        llvm::FunctionType* GetFuncSignature(llvm::Type* returnType, std::vector<llvm::Type*> argumentTypes);

        template <class... Args>
        llvm::FunctionType* GetFuncSignature(llvm::Type* returnType, Args... args)
        {
            std::vector<llvm::Type*> argumentVec = Vectorify<llvm::Type*, Args...>(args...);
            return GetFuncSignature(returnType, argumentVec);
        }

        llvm::Function* DeclareFunction(std::string name, llvm::FunctionType* funcSignature, llvm::GlobalValue::LinkageTypes linkage = llvm::GlobalValue::ExternalLinkage);

        llvm::IRBuilder<> Implement(llvm::Function* func);

        void Compile(std::string outfile);

        void Dump() const;
        void Verify() const;

    private:
        //const llvm::Target* GetTarget();
    };
}

