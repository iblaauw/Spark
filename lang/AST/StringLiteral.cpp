#include "AST/StringLiteral.h"
#include "LLVMManager.h"
#include "TypeConverter.h"

CHARSET(StringLiteralChar)
{
    return c != '"' && c != '\n';
}

RULE(_StringContents)
{
    Autoname(builder);
    builder.AddString(StringLiteralChar);
}

RULE(StringLiteral)
{
    Autoname(builder);
    builder.Add('"', _StringContents, '"');
    builder.Ignore(0);
    builder.Ignore(2);

    builder.SetNodeType<StringLiteralNode>();
}

void StringLiteralNode::Process()
{
    // TODO: make more robust
    NodePtr child = children[0];
    NodePtr grandchild = (*child)[0];

    if (grandchild->GetType() != "StringNode")
        return;

    literal = PtrCast<Spark::StringNode>(grandchild)->Get();

    children.clear();
    children.push_back(grandchild);
}

Ptr<RValue> StringLiteralNode::Evaluate(CompileContext& context)
{
    llvm::Constant* val = llvm::ConstantDataArray::getString(
        Spark::LLVMManager::Context(),
        literal);

    auto* gvar = Spark::LLVMManager::Instance().CreateGlobalConstant(".str", val);

    auto zero = Spark::TypeConverter::Create<int>(0);
    std::vector<llvm::Value*> indices { zero, zero };

    LangType* type = context.symbolTable.GetType("string");
    llvm::Value* instruction = context.builder.CreateGEP(gvar, indices, "str_literal");
    auto ptrVal = std::make_shared<GeneralRValue>(instruction, type);
    return PtrCast<RValue>(ptrVal);
}


