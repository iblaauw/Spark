#include "AST/ContextingNode.h"

void ContextingNode::GatherTypes(CompileContext& context)
{
    SymbolTable* originalTable = context.symbolTable;
    table.SetParent(originalTable);

    context.symbolTable = &table;
    CustomNode::GatherTypes(context);
    context.symbolTable = originalTable;
}

void ContextingNode::VerifyTypes(CompileContext& context)
{
    SymbolTable* originalTable = context.symbolTable;

    context.symbolTable = &table;
    CustomNode::VerifyTypes(context);
    context.symbolTable = originalTable;
}

void ContextingNode::GatherSymbols(CompileContext& context)
{
    SymbolTable* originalTable = context.symbolTable;

    context.symbolTable = &table;
    CustomNode::GatherSymbols(context);
    context.symbolTable = originalTable;
}

void ContextingNode::Generate(CompileContext& context)
{
    SymbolTable* originalTable = context.symbolTable;

    context.symbolTable = &table;
    CustomNode::Generate(context);
    context.symbolTable = originalTable;
}

void ContextingNode::Wrap(CompileContext& context)
{
    context.symbolTable = &table;
}

void ContextingNode::Unwrap(CompileContext& context)
{
    context.symbolTable = table.GetParent();
}
