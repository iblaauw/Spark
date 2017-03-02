#include "RuleSearcher.h"

Spark::RuleSearcher::RuleSearcher(RuleSearcher& other)
    : buffer(other.buffer), path(other.path)
{
}

void Spark::RuleSearcher::Add(RuleFunc func)
{
    if (done)
        return;

    path.Enter();
    func(*this);
    path.Exit();
}

void Spark::RuleSearcher::Add(char c)
{
    if (done)
        return;

    if (buffer.IsDone())
    {
        done = true;
        result = false;
        return;
    }

    char current = buffer.Current();
    buffer.Advance();

    if (current != c)
    {
        done = true;
        result = false;
        return;
    }
}

void Spark::RuleSearcher::End()
{
    if (done)
        return;

    done = true;
    result = true;
}

void Spark::RuleSearcher::AddOptionInternal(RuleFunc func)
{
    if (done)
        return;

    if (nested == nullptr)
    {
        nested = new RuleSearcher(*this);
    }

    nested->Add(func);
}

void Spark::RuleSearcher::AddOptionInternal(char c)
{
    if (done)
        return;

    if (nested == nullptr)
    {
        nested = new RuleSearcher(*this);
    }

    nested->Add(c);
}

void Spark::RuleSearcher::EndOptionInternal()
{
    if (nested == nullptr)
        throw SparkAssertionException("Internal Error: Cannot end option that doesn't exist!");

    nested->End();

    if (nested->Succeeded())
    {
        done = true;
        result = true;
        path.SetPath(optionCount);
    }

    delete nested;
    nested = nullptr;

    optionCount++;
}


