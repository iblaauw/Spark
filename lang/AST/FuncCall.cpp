#include "AST/FuncCall.h"

#include <iostream>

#include "AST/Common.h"
#include "AST/Expression.h"


class FuncCallArgsChain : public ChainingNode
{
public:
    FuncCallArgsChain(std::vector<NodePtr>& nodes) : ChainingNode(nodes) {}
    std::string GetType() const override { return "FuncCallArgsChain"; }
};

RULE(FuncCall)
{
    Autoname(builder);
    builder.Add('(', OptionalWhitespace, FuncCallArgsList, OptionalWhitespace, ')');
    builder.Ignore(1);
    builder.Ignore(3);

    builder.SetNodeType<FuncCallNode>();
}

RULE(_FuncCallArgsChain)
{
    Autoname(builder);
    builder.Add(ExpressionTree, OptionalWhitespace, ',', OptionalWhitespace, _FuncCallArgsChain);
    builder.Add(ExpressionTree);

    builder.SetNodeType<FuncCallArgsChain>();
}

RULE(FuncCallArgsList)
{
    Autoname(builder);
    builder.Add(_FuncCallArgsChain);
    builder.AddEmpty();

    builder.SetNodeType<FuncCallArgsListNode>();
}

/*
void FuncCallNode::Process()
{
    ConvertToOnlyCustom();
    CustomNode::Process();
}


UnknownPtr<RValue> FuncCallNode::Evaluate(CompileContext& context)
{
    auto funcNameNode = SafeGet<IdentifierNode>(0, "IdentifierNode");
    std::string funcName = funcNameNode->GetValue();

    Function* func = context.symbolTable->functions.Get(funcName);

    if (func == nullptr)
    {
        Error("a function named '", funcName, "' could not be found.");
        return nullptr;
    }

    auto argsNode = SafeGet<FuncCallArgsListNode>(1, "FuncCallArgsListNode");
    std::vector<UnknownPtr<RValue>> argVals;

    argsNode->EvalAll(context, argVals);

    if (!IsCompatible(argVals, func))
        return nullptr;

    llvm::Function* funcDef = func->GetIR();
    std::vector<llvm::Value*> args;
    auto converter = [&context](UnknownPtr<RValue> rv) { return rv->GetValue(context); };
    ::Map(converter, argVals, args);

    llvm::Value* value = context.builder.CreateCall(funcDef, args);

    LangType* retType = func->GetFuncType()->ReturnType();
    auto ptrVal = std::make_shared<GeneralRValue>(value, retType);
    return PtrCast<RValue>(ptrVal);
}

bool FuncCallNode::IsCompatible(const std::vector<UnknownPtr<RValue>>& args, Function* func)
{
    const auto& paramTypes = func->GetFuncType()->ParameterTypes();
    if (args.size() != paramTypes.size())
    {
        Error("wrong number of arguments to call function '", func->GetName(), "'\n",
                "Expected ", paramTypes.size(), " arguments but recieved ", args.size());
        return false;
    }

    auto converter = [](UnknownPtr<RValue> rv) { return rv->GetType(); };
    std::vector<LangType*> argTypes;
    ::Map(converter, args, argTypes);

    for (unsigned int i = 0; i < paramTypes.size(); i++)
    {
        LangType* a = argTypes[i];
        LangType* p = paramTypes[i];

        if (!p->IsAssignableFrom(a))
        {
            Error("invalid argument ", i, " when calling '", func->GetName(), "'\n",
                    "Cannot convert from type '", a->GetName(), "' to '", p->GetName(), "'");
            return false;
        }
    }

    return true;
}
*/

//// FuncCallNode2 ////

UnknownPtr<RValue> FuncCallNode::Create(UnknownPtr<RValue> lhs, CompileContext& context)
{
    LangType* ltype = lhs->GetType();

    SpecialOperatorImpl* opImpl = ltype->members.callOperator;
    if (opImpl == nullptr)
    {
        Error("Value of type '", ltype->GetName(), "' is not callable.");
        return nullptr;
    }

    auto argsNode = SafeGet<FuncCallArgsListNode>(0, "FuncCallArgsListNode");
    std::vector<UnknownPtr<RValue>> argValues;
    argsNode->EvalAll(context, argValues);

    return opImpl->Create(lhs, argValues, context);
}


//// FuncCallArgsListNode ////

void FuncCallArgsListNode::EvalAll(CompileContext& context, std::vector<UnknownPtr<RValue>>& evalOut)
{
    evalOut.clear();
    for (auto child : customChildren)
    {
        auto val = child->Evaluate(context);
        if (val != nullptr)
        {
            evalOut.push_back(val);
        }
    }
}



