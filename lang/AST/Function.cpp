#include "AST/Function.h"
#include "AST/Common.h"
#include "AST/Statement.h"
#include "TypeConverter.h"
#include "LLVMManager.h"


class FuncParamChain : public ChainingNode
{
public:
    FuncParamChain(std::vector<NodePtr>& nodes) : ChainingNode(nodes) {}
    std::string GetType() const override { return "FuncParamChain"; }
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

    funcName = PtrCast<IdentifierNode>(customChildren[0])->GetValue();

    auto& manager = Spark::LLVMManager::Instance();
    llvm::Type* voidType = Spark::TypeConverter::Get<void>();
    auto signature = manager.GetFuncSignature(voidType);
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
