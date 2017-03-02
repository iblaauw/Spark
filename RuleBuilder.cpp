#include "RuleBuilder.h"

#include <sstream>

Spark::RuleBuilder::RuleBuilder(InputBuffer& buffer, PathManager& path)
    : buffer(buffer), path(path), foundType(false), isOption(false)
{}

Spark::RuleBuilder::RuleBuilder(RuleBuilder& other)
    : buffer(other.buffer), path(other.path), foundType(false), isOption(false)
{}

void Spark::RuleBuilder::Add(RuleFunc rule)
{
    LockType(false);

    DoAdd(rule);
}

void Spark::RuleBuilder::Add(char c)
{
    LockType(false);

    DoAdd(c);
}

void Spark::RuleBuilder::AddOptionInternal(RuleFunc func)
{
    LockType(true);

    if (path.HasPath())
    {
        if (path.GetPath() == optionCount)
        {
            DoAdd(func);
        }
        return;
    }

    if (searcher == nullptr)
    {
        searcher = new RuleSearcher(buffer, path);
    }

    searcher->Add(func);
}

void Spark::RuleBuilder::AddOptionInternal(char c)
{
    LockType(true);

    if (path.HasPath())
    {
        if (path.GetPath() == optionCount)
        {
            DoAdd(c);
        }
    }

    if (searcher == nullptr)
    {
        searcher = new RuleSearcher(buffer, path);
    }

    searcher->Add(c);
}

void Spark::RuleBuilder::EndOptionInternal()
{
    if (!foundType || !isOption)
        throw SparkAssertionException("Internal Error: Empty or invalid type.");

    if (searcher != nullptr)
    {
        searcher->End();

        if (searcher->Succeeded())
        {
            path.SetPath(optionCount);
        }

        delete searcher;
        searcher = nullptr;
    }

    optionCount++;
}

void Spark::RuleBuilder::LockType(bool option)
{
    if (!foundType)
    {
        foundType = true;
        isOption = option;

        if (!option)
        {
            path.TrimTop();
        }
    }
    else if (isOption != option)
    {
        throw SparkException("Cannot mix calls to 'Add' and 'AddOption' in the same rule.");
    }
}

// Does the add, without locking in the type
void Spark::RuleBuilder::DoAdd(RuleFunc func)
{
    if (buffer.IsDone())
        throw ParseException("Unexpected end of file.");

    RuleBuilder next(*this);

    path.Enter();
    func(next);
    path.Exit();

    if (next.isOption) // first pass found the correct path, next will execute it
    {
        path.Enter();

        if (!path.HasPath()) // no correct path found!! This is a parse failure
        {
            std::stringstream ss;
            ss << "At line " << buffer.LineNum() << ", character " << buffer.CharNum() << ":";
            ss << std::endl;
            ss << "Rule match failed.";
            throw ParseException(ss.str());
        }

        RuleBuilder next2(*this);

        func(next);
        path.Exit();
    }
}

void Spark::RuleBuilder::DoAdd(char c)
{
    if (buffer.IsDone())
        throw ParseException("Unexpected end of file.");

    char next = buffer.GetNext();
    if (next != c)
    {
        std::stringstream ss;
        ss << "At line " << buffer.LineNum() << ", character " << buffer.CharNum() << ":";
        ss << std::endl;
        ss << "Unexpected token '" << next << "'. Expecting character '" << c << "'.";
        throw ParseException(ss.str());
    }
}

