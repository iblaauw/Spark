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

    root->Generate();

    std::cout << std::endl << "Begin module dump" << std::endl << std::endl;
    auto& manager = Spark::LLVMManager::Instance();
    manager.Dump();
    manager.Verify();

    manager.Compile("input.bc");

    //{
    //    Spark::LLVMManager::Init("my_module_yay");
    //    Spark::LLVMManager& manager = Spark::LLVMManager::Instance();
    //    std::cout << std::endl << "Begin module dump" << std::endl << std::endl;

    //    //llvm::Type* intType = llvm::Type::getInt32Ty(Spark::LLVMManager::Context());
    //    llvm::Type* intType = Spark::TypeConverter::Get<int>();
    //    llvm::Type* floatType = Spark::TypeConverter::Get<float>();
    //    auto sig = manager.GetFuncSignature(intType, intType, floatType);
    //    auto fooFunc = manager.DeclareFunction("foo", sig);
    //    llvm::IRBuilder<> builder = manager.Implement(fooFunc);

    //    llvm::Constant* c = Spark::TypeConverter::Create<int>(5);
    //    builder.CreateRet(c);


    //    manager.Dump();
    //    manager.Verify();
    //}


    return 0;
}




