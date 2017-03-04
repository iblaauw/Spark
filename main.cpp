#include <iostream>
#include <fstream>

#include "GrammarEngine.h"
#include "IRuleBuilder.h"

bool Alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z');
}

void Value(Spark::IRuleBuilder& builder);

void ValueEnd(Spark::IRuleBuilder& builder)
{
    builder.Add(Value);
    builder.AddEmpty();
}

void Value(Spark::IRuleBuilder& builder)
{
    builder.Add(Alpha, ValueEnd);
}

//void Value(Spark::IRuleBuilder& builder)
//{
//    builder.AddStringOption(Alpha);
//}

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
    builder.Add("blah ", Expression, '\n');
}

//void Hello(Spark::IRuleBuilder& builder)
//{
//    builder.Add("hello");
//}
//
//void World(Spark::IRuleBuilder& builder)
//{
//    builder.Add("world");
//}
//
//void HelloWorld(Spark::IRuleBuilder& builder)
//{
//    builder.Add(Hello, ' ', World, '\n');
//}

int main()
{
    std::ifstream input("input.txt");

    Spark::GrammarEngine engine(input);
    engine.Start(Program);

    return 0;
}


