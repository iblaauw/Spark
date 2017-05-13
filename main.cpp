#include <iostream>
#include <fstream>

#include "GrammarEngine.h"
#include "IRuleBuilder.h"
#include "LLVMManager.h"
#include "TypeConverter.h"
#include "Utils.h"

#include "Expressions.h"
#include "AST.h"

class OperatorNode : public Spark::Node
{
public:
    OperatorNode(std::vector<Spark::NodePtr>& nodes) : Node(nodes) {}
    std::string GetType() const override { return "OperatorNode"; }
};


// Temporarily in main.cpp, will be moved elsewhere at some-point
llvm::Function* DeclarePrintf(SymbolTable& symbolTable)
{

    llvm::Type* intType = Spark::TypeConverter::Get<int>();
    llvm::Type* charType = Spark::TypeConverter::Get<char>();

    auto& manager = Spark::LLVMManager::Instance();

    std::vector<llvm::Type*> args { charType->getPointerTo() };
    auto sig = manager.GetFuncSignatureVarargs(intType, args);
    llvm::Function* func = manager.DeclareFunction("printf", sig);

    symbolTable.AddFunction("print", func);
    return func;
}


int main()
{
    std::ifstream input("input.txt");

    Spark::GrammarEngine engine(input);
    auto tree = engine.Start(Program);

    Ptr<CustomNode> root = PtrCast<CustomNode>(tree);

    PrintTree(tree);

    std::cout << std::endl << "Processing..." << std::endl << std::endl;

    root->Process();

    PrintTree(tree);

    Spark::LLVMManager::Init("my_module");

    CompileContext globalContext;
    DeclarePrintf(globalContext.symbolTable);

    std::cout << std::endl << "Gathering Symbols..." << std::endl;

    root->GatherSymbols(globalContext);

    std::cout << std::endl << "Generating..." << std::endl;

    root->Generate(globalContext);


    std::cout << std::endl << "Begin module dump" << std::endl << std::endl;
    auto& manager = Spark::LLVMManager::Instance();
    manager.Dump();
    manager.Verify();

    //manager.CompileBC("input.bc");
    manager.Compile("input.o");

    return 0;
}




