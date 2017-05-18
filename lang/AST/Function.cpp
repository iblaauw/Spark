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
};


RULE(FunctionRule)
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
    std::string funcName = nameNode->GetValue();

    if (context.symbolTable.functions.Contains(funcName))
    {
        std::cerr << "Error: a function with the name '" << funcName << "' already exists" << std::endl;
        return;
    }

    auto paramListNode = SafeGet<FuncParamListNode>(1, "FuncParamListNode");
    if (paramListNode == nullptr)
    {
        std::cerr << "Internal Error: Invalid parameter list in FunctionNode" << std::endl;
        return;
    }

    // Get types and names
    std::vector<Ptr<LangType>> paramTypes;
    paramListNode->GetParamTypes(paramTypes);
    std::vector<std::string> paramNames;
    paramListNode->GetParamNames(paramNames);
    Ptr<LangType> retType = context.symbolTable.types.Get("void"); // TODO: make not all void

    // Create the actual function
    funcDefinition = std::make_shared<Function>(funcName, retType, paramTypes, paramNames);

    std::vector<llvm::Type*> paramIRTypes;
    funcDefinition->GetIRTypes(paramIRTypes);

    auto& manager = Spark::LLVMManager::Instance();
    auto signature = manager.GetFuncSignature(retType->GetIR(), paramIRTypes);
    llvm::Function* definitionIR = manager.DeclareFunction(funcName, signature);
    funcDefinition->SetIR(definitionIR);

    context.symbolTable.functions.Add(funcName, funcDefinition);

    // Create the function variables
    // TODO: this is all really unsafe...
    auto iter = definitionIR->arg_begin();
    for (unsigned int i = 0; i < paramTypes.size(); i++)
    {
        Ptr<LangType> type = paramTypes[i];
        std::string name = paramNames[i];

        Ptr<Variable> var = std::make_shared<Variable>(name, type);
        context.symbolTable.variables.Add(name, var);

        llvm::Value* varIR = static_cast<llvm::Argument*>(iter);
        var->SetValue(varIR);

        iter++;
    }

    // Recurse
    CustomNode::GatherSymbols(context);
}

void FunctionNode::Generate(CompileContext& context)
{
    // TODO: make this more safe and robust, check that function does already exist

    auto& manager = Spark::LLVMManager::Instance();

    auto* bb = manager.Implement(funcDefinition->GetIR());

    context.builder.SetInsertPoint(bb);

    CustomNode::Generate(context);

    context.builder.CreateRetVoid();
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

void FuncParamListNode::Process()
{
    AbsorbingNode::Process();

    for (auto param : customChildren)
    {
        if (param->GetType() != "FuncParameterNode")
        {
            std::cerr << "Internal Error: invalid node type in parameter list" << std::endl;
            continue;
        }

        Ptr<FuncParameterNode> p = PtrCast<FuncParameterNode>(param);
        this->params.push_back(p);
    }
}

void FuncParamListNode::GetParamTypes(std::vector<Ptr<LangType>>& vecOut)
{
    auto converter = [](Ptr<FuncParameterNode> p) -> Ptr<LangType> {
        Ptr<TypeNode> type = p->GetParamType();
        if (type == nullptr) return nullptr;
        return type->GetIRType();
    };

    auto filter = [](Ptr<LangType> p) { return p != nullptr; };

    ::Map(converter, params, vecOut);
    ::Filter(filter, vecOut);
}

void FuncParamListNode::GetParamNames(std::vector<std::string>& vecOut)
{
    auto converter = [](Ptr<FuncParameterNode> p) -> Ptr<IdentifierNode> {
        Ptr<IdentifierNode> id = p->GetIdentifier();
        if (id == nullptr) return nullptr;
        return id;
    };

    auto filter = [](Ptr<IdentifierNode> p) { return p != nullptr; };

    auto converter2 = [](Ptr<IdentifierNode> p) { return p->GetValue(); };

    std::vector<Ptr<IdentifierNode>> idNodes;
    ::Map(converter, params, idNodes);
    ::Filter(filter, idNodes);
    ::Map(converter2, idNodes, vecOut);
}

