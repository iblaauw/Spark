#include "InfoGatherer.h"

namespace Spark
{
    InfoGatherer::InfoGatherer()
        : hasType(false), typeIsOption(false), newRow(true), tokens()
    {}

    void InfoGatherer::Add(RuleFunc func)
    {
        DoAdd(func, false);
    }

    void InfoGatherer::Add(char c)
    {
        DoAdd(c, false);
    }

    void InfoGatherer::AddOptionInternal(RuleFunc func)
    {
        DoAdd(func, true);
    }

    void InfoGatherer::AddOptionInternal(char c)
    {
        DoAdd(c, true);
    }

    void InfoGatherer::EndOptionInternal()
    {
        if (!hasType || !typeIsOption)
            throw SparkAssertionException("Internal Error: Empty option sequence!");

        newRow = true;
    }

    void InfoGatherer::Verify() const
    {
        if (!hasType || tokens.size() == 0)
            throw SparkException("A rule must call either 'Add' or 'AddOption'. To get an empty string use 'AddOptionEmpty'.");

        if (tokens.size() == 1 && typeIsOption)
            throw SparkException("The rule consists of a single 'AddOption' call. Use 'Add' instead for rules with only a single option.");

    }

    void InfoGatherer::TypeGuard(bool isOption)
    {
        if (hasType)
        {
            if (typeIsOption != isOption)
                throw SparkException("Cannot mix 'Add' and 'AddOption' calls in the same rule!");
        }
        else
        {
            hasType = true;
            typeIsOption = isOption;
        }
    }

    void InfoGatherer::TryAddNewRow()
    {
        if (newRow)
        {
            tokens.emplace_back();
            newRow = false;
        }
    }
}

