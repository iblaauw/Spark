#include "AST/WhileStatement.h"

#include "AST/Common.h"
#include "AST/Expression.h"
#include "AST/Statement.h"

RULE(WhileStatement)
{
    Autoname(builder);
    builder.Add("while", OptionalWhitespace, '(', OptionalWhitespace, ExpressionTree, OptionalWhitespace, ')', OptionalWhitespace, StatementBlockBraced);

    builder.Ignore(1);
    builder.Ignore(3);
    builder.Ignore(5);
    builder.Ignore(7);

    builder.SetNodeType<WhileStatementNode>();
}


void WhileStatementNode::Generate(CompileContext& context)
{
    Wrap(context);

    Assert(customChildren.size() >= 2, "Invalid Node Structure");
    Assert(context.currentFunction != nullptr, "If statement not inside function");

    // Create the different basic blocks

    auto& manager = Spark::LLVMManager::Instance();
    auto* func = context.currentFunction->GetIR();

    llvm::BasicBlock* cond_bb = manager.AddBlock("while.cond", func);
    llvm::BasicBlock* body_bb = manager.AddBlock("while.body", func);
    llvm::BasicBlock* cont_bb = manager.AddBlock("while.cont", func);

    context.builder.CreateBr(cond_bb);

    // Insert conditional check

    context.builder.SetInsertPoint(cond_bb);

    auto condition = customChildren[0]->Evaluate(context); // Recurse
    if (condition == nullptr)
        return;

    LangType* ctype = condition->GetType();
    LangType* boolType = context.symbolTable->types.Get("bool");

    if (!boolType->IsAssignableFrom(ctype))
    {
        Error("Cannot convert from type '", ctype->GetName(), "' to 'bool'. Expected a bool value for condition of while statement.");
        return;
    }

    auto* condValue = condition->GetValue(context);
    context.builder.CreateCondBr(condValue, body_bb, cont_bb);

    // Insert while body

    context.builder.SetInsertPoint(body_bb);

    customChildren[1]->Generate(context); // Recurse

    auto* endBB = context.builder.GetInsertBlock();
    if (endBB->getTerminator() == nullptr)
    {
        context.builder.CreateBr(cond_bb);
    }

    // Continue in the continue section
    context.builder.SetInsertPoint(cont_bb);

    Unwrap(context);
}

