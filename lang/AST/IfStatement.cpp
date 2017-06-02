#include "AST/IfStatement.h"

#include "AST/Common.h"
#include "AST/Expression.h"
#include "AST/Statement.h"

RULE(IfStatement)
{
    Autoname(builder);
    builder.Add("if", OptionalWhitespace, '(', OptionalWhitespace, ExpressionTree, OptionalWhitespace, ')', OptionalWhitespace, StatementBlockBraced, OptionalWhitespace, "else", OptionalWhitespace, StatementBlockBraced);
    builder.Add("if", OptionalWhitespace, '(', OptionalWhitespace, ExpressionTree, OptionalWhitespace, ')', OptionalWhitespace, StatementBlockBraced);

    builder.Ignore(1);
    builder.Ignore(3);
    builder.Ignore(5);
    builder.Ignore(7);
    builder.Ignore(9);
    builder.Ignore(11);

    builder.SetNodeType<IfStatementNode>();
}


void IfStatementNode::Generate(CompileContext& context)
{
    Assert(customChildren.size() >= 2, "Invalid Node Structure");

    bool hasElse = customChildren.size() > 2;

    auto condition = customChildren[0]->Evaluate(context);

    LangType* ctype = condition->GetType();
    LangType* boolType = context.symbolTable->types.Get("bool");

    if (!boolType->IsAssignableFrom(ctype))
    {
        Error("Cannot convert from type '", ctype->GetName(), "' to 'bool'. Expected a bool value for condition of if statement.");
        return;
    }

    Assert(context.currentFunction != nullptr, "If statement not inside function");

    auto& manager = Spark::LLVMManager::Instance();
    auto* func = context.currentFunction->GetIR();
    llvm::BasicBlock* bb_then = manager.AddBlock("if.then", func);
    llvm::BasicBlock* bb_else;
    if (hasElse)
        bb_else = manager.AddBlock("if.else", func);
    llvm::BasicBlock* bb_cont = manager.AddBlock("if.cont", func);

    if (!hasElse)
        bb_else = bb_cont;

    llvm::Value* condVal = condition->GetValue(context);
    context.builder.CreateCondBr(condVal, bb_then, bb_else);

    // Recurse
    context.builder.SetInsertPoint(bb_then);
    customChildren[1]->Generate(context);

    if (bb_then->getTerminator() == nullptr)
    {
        // Branch from the if back to main control flow
        context.builder.CreateBr(bb_cont);
    }

    if (hasElse)
    {
        context.builder.SetInsertPoint(bb_else);
        customChildren[2]->Generate(context);
        if (bb_else->getTerminator() == nullptr)
        {
            // Branch from the else back to main control flow
            context.builder.CreateBr(bb_cont);
        }
    }

    // Continue to add code after if, but in a new block
    context.builder.SetInsertPoint(bb_cont);
}

