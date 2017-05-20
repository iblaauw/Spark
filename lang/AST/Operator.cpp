#include "AST/Operator.h"

#include <functional>

RULE(Operator)
{
    Autoname(builder);
    builder.Add("+");
    builder.Add("-");
    builder.Add("*");
    builder.Add("/");

    builder.SetNodeType<OperatorNode>();
}

class OperatorImpl
{
public:
    std::function<LangType*(LangType*, LangType*, CompileContext&)> type;
    std::function<llvm::Value*(llvm::Value*, llvm::Value*, CompileContext&)> value;
};

static LangType* IntOnly(LangType* a, LangType* b, CompileContext& context)
{
    LangType* intType = context.symbolTable.types.Get("int");
    if (a != intType || b != intType)
        return nullptr;
    return intType;
}

static llvm::Value* PlusVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateAdd(a, b, "int_add");
}

static llvm::Value* SubtractVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateSub(a, b, "int_sub");
}

static llvm::Value* MultiplyVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateMul(a,b, "int_mul");
}

static llvm::Value* DivideVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    return context.builder.CreateSDiv(a,b, "int_div");
}


void OperatorNode::Process()
{
    StringValueNode::Process();

    impl = std::make_shared<OperatorImpl>();

    std::string op = GetValue();
    if (op == "+")
    {
        impl->type = IntOnly;
        impl->value = PlusVal;
    }
    else if (op == "-")
    {
        impl->type = IntOnly;
        impl->value = SubtractVal;
    }
    else if (op == "*")
    {
        impl->type = IntOnly;
        impl->value = MultiplyVal;
    }
    else if (op == "/")
    {
        impl->type = IntOnly;
        impl->value = DivideVal;
    }
    else
    {
        std::cerr << "Internal Error: unknown operator '" << op << "'" << std::endl;
    }
}

LangType* OperatorNode::GetResultType(LangType* lhs, LangType* rhs, CompileContext& context)
{
    return impl->type(lhs, rhs, context);
}

llvm::Value* OperatorNode::Create(llvm::Value* lhs, llvm::Value* rhs, CompileContext& context)
{
    return impl->value(lhs, rhs, context);
}


