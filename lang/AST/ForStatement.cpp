#include "AST/ForStatement.h"

#include "AST/Common.h"
#include "AST/Expression.h"
#include "AST/Statement.h"
#include "AST/ExpressionStatement.h"
#include "AST/VariableDeclare.h"

class ForStatementExprNode : public CustomNode
{
public:
    ForStatementExprNode(std::vector<NodePtr>& nodes) : CustomNode(nodes) {}
    std::string GetType() const override { return "ForStatementExprNode"; }
};

RULE(ForStatementExpr)
{
    Autoname(builder);
    builder.Add(NonControlStatement_NoSemicolon);
    builder.AddEmpty();

    builder.SetNodeType<ForStatementExprNode>();
}

RULE(ForStatement)
{
    Autoname(builder);
    builder.Add("for", OptionalWhitespace,
                "(", OptionalWhitespace,
                ForStatementExpr, OptionalWhitespace,
                ';', OptionalWhitespace,
                ExpressionTree, OptionalWhitespace,
                ';', OptionalWhitespace,
                ForStatementExpr, OptionalWhitespace,
                ')', OptionalWhitespace,
                StatementBlockBraced);

    builder.Ignore(1);
    builder.Ignore(3);
    builder.Ignore(5);
    builder.Ignore(7);
    builder.Ignore(9);
    builder.Ignore(11);
    builder.Ignore(13);
    builder.Ignore(15);

    builder.SetNodeType<ForStatementNode>();
}


void ForStatementNode::Generate(CompileContext& context)
{
    Assert(customChildren.size() >= 4, "Invalid Node Structure");
    Assert(context.currentFunction != nullptr, "for loop not in a function");

    Wrap(context);

    // Insert the different blocks
    auto* func = context.currentFunction->GetIR();
    auto& manager = Spark::LLVMManager::Instance();

    llvm::BasicBlock* cond_bb = manager.AddBlock("for.cond", func);
    llvm::BasicBlock* body_bb = manager.AddBlock("for.body", func);
    llvm::BasicBlock* incr_bb = manager.AddBlock("for.incr", func);
    llvm::BasicBlock* cont_bb = manager.AddBlock("for.cont", func);

    // Insert beginning statement
    customChildren[0]->Generate(context);
    context.builder.CreateBr(cond_bb);

    // Insert the conditional
    context.builder.SetInsertPoint(cond_bb);
    auto condition = customChildren[1]->Evaluate(context);
    if (condition == nullptr)
        return;

    LangType* condType = condition->GetType();
    LangType* boolType = context.symbolTable->types.Get("bool");
    if (!boolType->IsAssignableFrom(condType))
    {
        Error("Cannot convert from type '", condType->GetName(), "' to 'bool'. Expected a bool value for condition of for statement.");
        return;
    }

    auto condVal = condition->GetValue(context);
    context.builder.CreateCondBr(condVal, body_bb, cont_bb);

    // Insert the body
    context.builder.SetInsertPoint(body_bb);
    customChildren[3]->Generate(context);

    auto* currentBlock = context.builder.GetInsertBlock();
    if (currentBlock->getTerminator() == nullptr)
    {
        context.builder.CreateBr(incr_bb);
    }

    // Insert the increment
    context.builder.SetInsertPoint(incr_bb);
    customChildren[2]->Generate(context);
    context.builder.CreateBr(cond_bb);

    // Continue
    context.builder.SetInsertPoint(cont_bb);

    Unwrap(context);
}

