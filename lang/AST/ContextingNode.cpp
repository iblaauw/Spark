#include "AST/ContextingNode.h"

void ContextingNode::GatherTypes(CompileContext& context)
{
    table.SetParent(context.symbolTable);

    CompileContext newContext;
    newContext.symbolTable = &table;
    CustomNode::GatherTypes(newContext);
}

void ContextingNode::VerifyTypes(CompileContext& context)
{
    CompileContext newContext;
    newContext.symbolTable = &table;
    CustomNode::VerifyTypes(newContext);
}

void ContextingNode::GatherSymbols(CompileContext& context)
{
    CompileContext newContext;
    newContext.symbolTable = &table;
    CustomNode::GatherSymbols(newContext);
}

void ContextingNode::Generate(CompileContext& context)
{
    CompileContext newContext;
    newContext.symbolTable = &table;
    CustomNode::Generate(newContext);
}

