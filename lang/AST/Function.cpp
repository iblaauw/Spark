#include "AST/Function.h"
#include "AST/Common.h"
#include "AST/Statement.h"
#include "TypeConverter.h"
#include "LLVMManager.h"

#include <iostream>

class FuncParamChain : public ChainingNode
{
public:
    FuncParamChain(std::vector<NodePtr>& nodes) : ChainingNode(nodes) {}
    std::string GetType() const override { return "FuncParamChain"; }

    const std::vector<Ptr<CustomNode>>& GetNodes() const { return customChildren; }
};


RULE(Function)
{
    Autoname(builder);
    builder.Add("func ", Identifier, '(', FunctionParameterList, ')', OptionalWhitespace, '{', OptionalWhitespace, StatementBlock, OptionalWhitespace, '}');

    builder.Ignore(5);
    builder.Ignore(7);
    builder.Ignore(9);

    builder.SetNodeType<FunctionNode>();
}

RULE(_FunctionParameterChain)
{
    Autoname(builder);
    builder.Add(FunctionParameter, OptionalWhitespace, ',', OptionalWhitespace, _FunctionParameterChain);
    builder.Add(FunctionParameter);

    builder.Ignore(1);
    builder.Ignore(2);
    builder.Ignore(3);

    builder.SetNodeType<FuncParamChain>();
}

RULE(FunctionParameterList)
{
    Autoname(builder);
    builder.Add(OptionalWhitespace, _FunctionParameterChain, OptionalWhitespace);
    builder.AddEmpty();

    builder.Ignore(0);
    builder.Ignore(2);

    builder.SetNodeType<FuncParamListNode>();
}

RULE(FunctionParameter)
{
    Autoname(builder);
    builder.Add(Type, Whitespace, Identifier);

    builder.Ignore(1);

    builder.SetNodeType<FuncParameterNode>();
}

void FunctionNode::Process()
{
    ConvertToOnlyCustom();

    CustomNode::Process();
}

void FunctionNode::GatherSymbols(CompileContext& context)
{
    // TODO: make this more safe and robust, check that function doesn't already exist
    auto nameNode = SafeGet<IdentifierNode>(0, "IdentifierNode");
    funcName = nameNode->GetValue();

    auto paramListNode = SafeGet<FuncParamListNode>(1, "FuncParamListNode");
    if (paramListNode == nullptr)
    {
        std::cerr << "Internal Error: Invalid parameter list in FunctionNode" << std::endl;
        return;
    }

    std::vector<llvm::Type*> paramTypes;
    paramListNode->GetParamTypes(paramTypes);

    std::cout << "Num Func Params: " << paramTypes.size() << std::endl;

    auto& manager = Spark::LLVMManager::Instance();
    llvm::Type* voidType = Spark::TypeConverter::Get<void>();
    auto signature = manager.GetFuncSignature(voidType, paramTypes);
    funcDefinition = manager.DeclareFunction(funcName, signature);

    context.symbolTable.AddFunction(funcName, funcDefinition);

    // Recurse
    CustomNode::GatherSymbols(context);
}

void FunctionNode::Generate(CompileContext& context)
{
    // TODO: make this more safe and robust, check that function does already exist

    auto& manager = Spark::LLVMManager::Instance();

    auto* bb = manager.Implement(funcDefinition);

    context.builder.SetInsertPoint(bb);

    CustomNode::Generate(context);

    context.builder.CreateRetVoid();
}

void FuncParamListNode::GetParamTypes(std::vector<llvm::Type*>& vecOut)
{
    // Case where there are no parameters
    if (customChildren.size() == 0)
        return;

    vecOut.clear();

    Ptr<FuncParamChain> chainNode = SafeGet<FuncParamChain>(0, "FuncParamChain");

    if (chainNode == nullptr)
    {
        std::cerr << "Internal Error: Invalid parameter chain in FuncParamListNode" << std::endl;
        return;
    }

    auto& nodes = chainNode->GetNodes();

    for (auto param : nodes)
    {
        if (param->GetType() != "FuncParameterNode")
        {
            std::cerr << "Internal Error: invalid node type in parameter list" << std::endl;
            continue;
        }

        Ptr<FuncParameterNode> p = PtrCast<FuncParameterNode>(param);

        auto typenode = p->GetParamType();
        if (typenode == nullptr)
            continue;

        std::cout << "push" << std::endl;
        vecOut.push_back(typenode->GetIRType()->GetIR());
    }
}

Ptr<IdentifierNode> FuncParameterNode::GetIdentifier() const
{
    auto val = SafeGet<IdentifierNode>(1, "IdentifierNode");
    if (val == nullptr)
    {
        std::cerr << "Internal Error: invalid identifier node for FuncParameterNode" << std::endl;
    }
    return val;
}

Ptr<TypeNode> FuncParameterNode::GetParamType() const
{
    auto val = SafeGet<TypeNode>(0, "TypeNode");
    if (val == nullptr)
    {
        std::cerr << "Internal Error: invalid type node for FuncParameterNode" << std::endl;
    }
    return val;
}
