#include "LLVMManager.h"
#include "Exceptions.h"

#include <iostream>

#include "llvm/Analysis/Verifier.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/Host.h"
#include "llvm/ADT/Optional.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Bitcode/ReaderWriter.h"

namespace Spark
{
    /*static*/ LLVMManager* LLVMManager::instance;

    /*static*/ llvm::PassManagerBuilder LLVMManager::passBuilder;

    LLVMManager::LLVMManager(std::string name)
    {
        currentModule = new llvm::Module("my_module", globalContext);
        funcPassManager = new llvm::FunctionPassManager(currentModule);

        passBuilder.populateFunctionPassManager(*funcPassManager);
        passBuilder.populateModulePassManager(passManager);

        funcPassManager->doInitialization();
    }

    /*static*/ void LLVMManager::Init(std::string name, int optimizationLevel)
    {
        passBuilder.OptLevel = optimizationLevel;
        instance = new LLVMManager(name);
    }

    llvm::FunctionType* LLVMManager::GetFuncSignature(llvm::Type* returnType, std::vector<llvm::Type*> argumentTypes)
    {
        return llvm::FunctionType::get(returnType, argumentTypes, false);
    }

    llvm::FunctionType* LLVMManager::GetFuncSignatureVarargs(llvm::Type* returnType, std::vector<llvm::Type*> argumentTypes)
    {
        return llvm::FunctionType::get(returnType, argumentTypes, true);
    }


    llvm::Function* LLVMManager::DeclareFunction(std::string name, llvm::FunctionType* funcSignature, llvm::GlobalValue::LinkageTypes linkage)
    {
        return llvm::Function::Create(funcSignature, linkage, name, currentModule);
    }

    llvm::BasicBlock* LLVMManager::Implement(llvm::Function* func)
    {
        if (!func->empty())
            throw SparkException("Error: attempting to reimplement function.");

        llvm::BasicBlock* bb = llvm::BasicBlock::Create(globalContext, "entry", func);
        return bb;
    }

    llvm::GlobalVariable* LLVMManager::CreateGlobalConstant(std::string name, llvm::Constant* value, llvm::GlobalValue::LinkageTypes linkage)
    {
        llvm::GlobalVariable* gvar = new llvm::GlobalVariable(
            *currentModule, value->getType(), true, linkage, value, name);
        return gvar;
    }


    void LLVMManager::OptimizeFunction(llvm::Function* func)
    {
        funcPassManager->run(*func);
    }

    void LLVMManager::CompileBC(std::string outfile)
    {
        // Open the output file
        std::string error = "";
        llvm::raw_fd_ostream out(outfile.c_str(), error, llvm::raw_fd_ostream::F_Binary);

        if (error != "")
        {
            std::cerr << "Error opening file '" << outfile << "':" << std::endl;
            std::cerr << error << std::endl;
            return;
        }

        funcPassManager->doFinalization();

        passManager.run(*currentModule);

        llvm::WriteBitcodeToFile(currentModule, out);
    }

    void LLVMManager::Compile(std::string outfile)
    {
        // Initialize the TargetRegistry for looking up targets
        llvm::InitializeAllTargetInfos();
        llvm::InitializeAllTargets();
        llvm::InitializeAllTargetMCs();
        llvm::InitializeAllAsmParsers();
        llvm::InitializeAllAsmPrinters();

        std::string targetTriple = llvm::sys::getDefaultTargetTriple();

        auto target = GetTarget();
        if (!target)
            return;

        // Create target machine
        std::string cpuType = "generic";
        std::string features = "";
        llvm::TargetOptions options;
        auto RM = llvm::Reloc::Default;
        auto targetMachine = target->createTargetMachine(targetTriple, cpuType, features, options, RM);

        // Update the module with target and layout info
        currentModule->setTargetTriple(targetTriple);

        auto* dataLayout = targetMachine->getDataLayout();
        if (dataLayout != nullptr)
        {
            currentModule->setDataLayout(dataLayout->getStringRepresentation());
        }

        // Open the output file
        std::string error = "";
        llvm::raw_fd_ostream out(outfile.c_str(), error, llvm::raw_fd_ostream::F_Binary);

        if (error != "")
        {
            std::cerr << "Error opening file '" << outfile << "':" << std::endl;
            std::cerr << error << std::endl;
            return;
        }

        llvm::formatted_raw_ostream out_format(out);

        // Set up passes
        auto outfileType = llvm::TargetMachine::CGFT_ObjectFile;
        targetMachine->addPassesToEmitFile(passManager, out_format, outfileType);

        // Run!
        funcPassManager->doFinalization();
        passManager.run(*currentModule);
    }

    void LLVMManager::Dump() const
    {
        currentModule->dump();
    }

    void LLVMManager::Verify() const
    {
        llvm::verifyModule(*currentModule);
    }

    const llvm::Target* LLVMManager::GetTarget()
    {
        std::string targetTriple = llvm::sys::getDefaultTargetTriple();

        std::string error;
        auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

        if (target != nullptr)
            return target;

        std::cerr << "Error: could not find compile target for '" << targetTriple << "'." << std::endl;
        std::cerr << error << std::endl;
        return nullptr;

        //error = "";

        //llvm::Triple triple(targetTriple);

        //target = llvm::TargetRegistry::lookupTarget(triple.getArchName(), triple, error);
        //if (target == nullptr)
        //{
        //    std::cerr << "Error: couldn't find a matching compile target." << std::endl;
        //    std::cerr << error << std::endl;
        //}

        //return target;
    }
}

