#include <iostream>
#include <fstream>

#include "GrammarEngine.h"
#include "IRuleBuilder.h"

bool Alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z');
}

void Value(Spark::IRuleBuilder& builder)
{
    builder.AddString(Alpha);
}

void Operator(Spark::IRuleBuilder& builder)
{
    builder.Add('+');
    builder.Add('-');
    builder.Add('*');
    builder.Add('/');
}

void Expression(Spark::IRuleBuilder& builder);

void ExpressionEnd(Spark::IRuleBuilder& builder)
{
    builder.Add(' ', Operator, ' ', Expression);
}

void Expression(Spark::IRuleBuilder& builder)
{
    builder.Add(Value, ExpressionEnd);
    builder.Add(Value);
}

void Program(Spark::IRuleBuilder& builder)
{
    builder.Add(Expression, '\n');
}

int main()
{
    std::ifstream input("input.txt");

    Spark::GrammarEngine engine(input);
    engine.Start(Program);

    return 0;
}


