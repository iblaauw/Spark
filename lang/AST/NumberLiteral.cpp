#include "AST/NumberLiteral.h"
#include "TypeConverter.h"

CHARSET(NumberChar)
{
    return c >= '0' && c <= '9';
}

RULE(NumberLiteral)
{
    Autoname(builder);
    builder.AddString(NumberChar);

    builder.SetNodeType<NumberLiteralNode>();
}

void NumberLiteralNode::Process()
{
    StringValueNode::Process();

    std::string value = GetValue();

    if (value == "")
        return;

    this->value = std::stoi(value);
}

Ptr<RValue> NumberLiteralNode::Evaluate(CompileContext& context)
{
    Ptr<LangType> intType = context.symbolTable.types.Get("int");
    llvm::Value* val = Spark::TypeConverter::Create<int>(this->value);
    return std::make_shared<GeneralRValue>(val, intType);
}


