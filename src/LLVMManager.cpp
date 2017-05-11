#include "LLVMManager.h"
#include "Exceptions.h"

#include "llvm/Analysis/Verifier.h"

namespace Spark
{
    /*static*/ LLVMManager* LLVMManager::instance;

    LLVMManager::LLVMManager(std::string name)
    {
        currentModule = new llvm::Module("my_module", globalContext);
    }

    /*static*/ void LLVMManager::Init(std::string name)
    {
        instance = new LLVMManager(name);
    }

    llvm::FunctionType* LLVMManager::GetFuncSignature(llvm::Type* returnType, std::vector<llvm::Type*> argumentTypes)
    {
        return llvm::FunctionType::get(returnType, argumentTypes, false);
    }

    llvm::Function* LLVMManager::DeclareFunction(std::string name, llvm::FunctionType* funcSignature, llvm::GlobalValue::LinkageTypes linkage)
    {
        return llvm::Function::Create(funcSignature, linkage, name, currentModule);
    }

    llvm::IRBuilder<> LLVMManager::Implement(llvm::Function* func)
    {
        if (!func->empty())
            throw SparkException("Error: attempting to reimplement function.");

        llvm::IRBuilder<> builder(globalContext);
        llvm::BasicBlock* bb = llvm::BasicBlock::Create(globalContext, "entry", func);
        builder.SetInsertPoint(bb);
        return builder;
    }

    void LLVMManager::Dump() const
    {
        currentModule->dump();
    }

    void LLVMManager::Verify() const
    {
        llvm::verifyModule(*currentModule);
    }
}

