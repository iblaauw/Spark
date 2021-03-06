#pragma once

#include <memory>

#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

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

        llvm::PassManager passManager;
        llvm::FunctionPassManager* funcPassManager;

        static LLVMManager* instance;

        LLVMManager(std::string name);
    public:
        static llvm::PassManagerBuilder passBuilder;

        static void Init(std::string name, int optimizationLevel = 2);
        static inline LLVMManager& Instance() { return *instance; }
        static inline llvm::LLVMContext& Context() { return instance->globalContext; }


        llvm::FunctionType* GetFuncSignature(llvm::Type* returnType, std::vector<llvm::Type*> argumentTypes);

        template <class... Args>
        llvm::FunctionType* GetFuncSignature(llvm::Type* returnType, Args... args)
        {
            std::vector<llvm::Type*> argumentVec = Vectorify<llvm::Type*, Args...>(args...);
            return GetFuncSignature(returnType, argumentVec);
        }

        llvm::FunctionType* GetFuncSignatureVarargs(llvm::Type* returnType, std::vector<llvm::Type*> argumentTypes);


        llvm::Function* DeclareFunction(std::string name, llvm::FunctionType* funcSignature, llvm::GlobalValue::LinkageTypes linkage = llvm::GlobalValue::ExternalLinkage);

        llvm::BasicBlock* Implement(llvm::Function* func);

        llvm::BasicBlock* AddBlock(std::string name, llvm::Function* func);

        llvm::GlobalVariable* CreateGlobalConstant(std::string name, llvm::Constant* value, llvm::GlobalValue::LinkageTypes linkage = llvm::GlobalValue::PrivateLinkage);

        llvm::Value* CreateStructValue(llvm::IRBuilder<>& builder, llvm::Type* structType, const std::vector<llvm::Value*>& values, std::string name);

        void OptimizeFunction(llvm::Function* func);

        void CompileBC(std::string outfile);
        void Compile(std::string outfile);

        void Dump() const;
        void Verify() const;

    private:
        const llvm::Target* GetTarget();
    };
}

