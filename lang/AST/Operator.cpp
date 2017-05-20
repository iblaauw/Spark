#include "AST/Operator.h"

#include <functional>

RULE(Operator)
{
    Autoname(builder);
    builder.Add("+");
    builder.Add("-");
    //builder.Add("*");
    //builder.Add("/");

    builder.SetNodeType<OperatorNode>();
}

class OperatorImpl
{
public:
    std::function<LangType*(LangType*, LangType*, CompileContext&)> type;
    std::function<llvm::Value*(llvm::Value*, llvm::Value*, CompileContext&)> value;
};


static LangType* Plus(LangType* a, LangType* b, CompileContext& context)
{
    // For now only supporting int addition
    LangType* intType = context.symbolTable.types.Get("int");
    if (a != intType || b != intType)
        return nullptr;
    return intType;
}

static LangType* Subtract(LangType* a, LangType* b, CompileContext& context)
{
    // For now only supporting int subtraction
    LangType* intType = context.symbolTable.types.Get("int");
    if (a != intType || b != intType)
        return nullptr;
    return intType;
}

static llvm::Value* PlusVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    std::cerr << "Not Implemented" << std::endl;
    return nullptr;
}

static llvm::Value* SubtractVal(llvm::Value* a, llvm::Value* b, CompileContext& context)
{
    std::cerr << "Not Implemented" << std::endl;
    return nullptr;
}


void OperatorNode::Process()
{
    StringValueNode::Process();

    impl = std::make_shared<OperatorImpl>();

    std::string op = GetValue();
    if (op == "+")
    {
        impl->type = Plus;
        impl->value = PlusVal;
    }
    else if (op == "-")
    {
        impl->type = Subtract;
        impl->value = SubtractVal;
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


