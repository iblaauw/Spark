#include "AST/Function.h"

#include "AST/Common.h"
#include "AST/Statement.h"
#include "TypeConverter.h"
#include "LLVMManager.h"
#include "Variable.h"

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
    builder.Add(Type, Whitespace, Identifier, '(', FunctionParameterList, ')', OptionalWhitespace, '{', OptionalWhitespace, StatementBlock, OptionalWhitespace, '}');

    builder.Ignore(6);
    builder.Ignore(8);
    builder.Ignore(10);

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

    ContextingNode::Process();
}

void FunctionNode::GatherSymbols(CompileContext& context)
{
    // TODO: make this more safe and robust, check that function doesn't already exist
    auto nameNode = SafeGet<IdentifierNode>(1, "IdentifierNode");
    std::string funcName = nameNode->GetValue();

    if (context.symbolTable->functions.Contains(funcName))
    {
        std::cerr << "Error: a function with the name '" << funcName << "' already exists" << std::endl;
        return;
    }

    auto paramListNode = SafeGet<FuncParamListNode>(2, "FuncParamListNode");
    if (paramListNode == nullptr)
    {
        std::cerr << "Internal Error: Invalid parameter list in FunctionNode" << std::endl;
        return;
    }

    auto retTypeNode = SafeGet<TypeNode>(0, "TypeNode");
    if (retTypeNode == nullptr)
    {
        std::cerr << "Internal Error: Invalid return type node in FunctionNode" << std::endl;
        return;
    }

    // Get types and names
    std::vector<LangType*> paramTypes;
    paramListNode->GetParamTypes(paramTypes);
    std::vector<std::string> paramNames;
    paramListNode->GetParamNames(paramNames);
    LangType* retType = retTypeNode->GetIRType();

    // Create the actual function
    funcDefinition = context.symbolTable->functions.Create(funcName, funcName, retType, paramTypes, paramNames);

    std::vector<llvm::Type*> paramIRTypes;
    funcDefinition->GetIRTypes(paramIRTypes);

    auto& manager = Spark::LLVMManager::Instance();
    auto signature = manager.GetFuncSignature(retType->GetIR(), paramIRTypes);
    llvm::Function* definitionIR = manager.DeclareFunction(funcName, signature);
    funcDefinition->SetIR(definitionIR);

    // Create the function variables
    // TODO: this is all really unsafe...
    auto iter = definitionIR->arg_begin();
    for (unsigned int i = 0; i < paramTypes.size(); i++)
    {
        LangType* type = paramTypes[i];
        std::string name = paramNames[i];

        RegisterVariable* var = new RegisterVariable(name, type);
        this->table.variables.Add(name, var);
        this->funcDefinition->allocationSet.push_back(var);

        llvm::Value* varIR = static_cast<llvm::Argument*>(iter);
        var->SetValue(varIR);

        iter++;
    }

    // Recurse
    context.currentFunction = funcDefinition;
    ContextingNode::GatherSymbols(context);
    context.currentFunction = nullptr;
}

void FunctionNode::Generate(CompileContext& context)
{
    // TODO: make this more safe and robust, check that function does already exist
    auto& manager = Spark::LLVMManager::Instance();

    auto* bb = manager.Implement(funcDefinition->GetIR());

    context.builder.SetInsertPoint(bb);
    context.currentFunction = funcDefinition;

    // Allocate and initialize
    for (Variable* var : funcDefinition->allocationSet)
    {
        var->Allocate(context);
    }

    for (Variable* var : funcDefinition->allocationSet)
    {
        var->Initialize(context);
    }

    // Recurse
    ContextingNode::Generate(context);

    // Check for missing return statement
    auto& builder = context.builder;
    llvm::BasicBlock* finalBB = builder.GetInsertBlock();

    if (finalBB->getTerminator() == nullptr)
    {
        LangType* voidtype = context.symbolTable->types.Get("void");
        if (funcDefinition->ReturnType() == voidtype)
        {
            // Implicit "return;", insert it automatically
            builder.CreateRetVoid();
        }
        else
        {
            // No return in a non-void function
            std::cerr << "Error: no return statement in function '" << funcDefinition->GetName() << "' that expects a return value." << std::endl;
            return;
        }
    }

    // Apply optimizations
    manager.OptimizeFunction(funcDefinition->GetIR());

    context.currentFunction = nullptr;
    context.builder.ClearInsertionPoint();
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

void FuncParamListNode::GetParamTypes(std::vector<LangType*>& vecOut)
{
    auto converter = [](Ptr<FuncParameterNode> p) -> LangType* {
        Ptr<TypeNode> type = p->GetParamType();
        if (type == nullptr) return nullptr;
        return type->GetIRType();
    };

    auto filter = [](LangType* p) { return p != nullptr; };

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

