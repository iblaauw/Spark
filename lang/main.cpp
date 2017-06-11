#include <iostream>
#include <fstream>

#include "GrammarEngine.h"
#include "IRuleBuilder.h"
#include "LLVMManager.h"
#include "TypeConverter.h"
#include "Utils.h"
#include "AST/Program.h"
#include "Builtins.h"
#include "Errors.h"

class OperatorNode : public Spark::Node
{
public:
    OperatorNode(std::vector<Spark::NodePtr>& nodes) : Node(nodes) {}
    std::string GetType() const override { return "OperatorNode"; }
};

int main()
{
    std::ifstream input("input.txt");
    SetCurrentFile(input);

    Spark::GrammarEngine engine(input);

    auto tree = engine.Start(Program);

    Ptr<CustomNode> root = PtrCast<CustomNode>(tree);


    input.clear(); // Needed to clear the EOF state so that the file can be read again
    //PrintTree(tree);

    std::cout << std::endl << "Processing..." << std::endl << std::endl;

    root->Process();

    PrintTree(tree);

    Spark::LLVMManager::Init("my_module", 0); // NOTE: no optimizations enabled because of debugging

    SymbolTable builtinTable;
    SymbolTable globalSymbols(&builtinTable);

    AddBuiltinTypes(builtinTable);
    AddBuiltinFunctions(builtinTable);

    CompileContext globalContext;
    globalContext.symbolTable = &globalSymbols;
    globalContext.builtins = &builtinTable;

    if (HasError())
        return 1;

    root->GatherTypes(globalContext);

    if (HasError())
        return 1;

    root->VerifyTypes(globalContext);

    if (HasError())
        return 1;

    std::cout << std::endl << "Gathering Symbols..." << std::endl;

    root->GatherSymbols(globalContext);

    if (HasError())
        return 1;

    std::cout << std::endl << "Generating..." << std::endl;

    root->Generate(globalContext);

    if (HasError())
        return 1;

    std::cout << std::endl << "Begin module dump" << std::endl << std::endl;
    auto& manager = Spark::LLVMManager::Instance();
    manager.Dump();
    manager.Verify();

    //manager.CompileBC("input.bc");
    manager.Compile("input.o");

    return 0;
}




