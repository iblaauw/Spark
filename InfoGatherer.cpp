#include "InfoGatherer.h"

#include "Exceptions.h"

namespace Spark
{
    InfoGatherer::InfoGatherer()
        : hasType(false), typeIsOption(false), newRow(true), tokens()
    {}

    void InfoGatherer::Verify() const
    {
        if (!hasType || tokens.size() == 0)
            throw SparkException("A rule must call either 'Add' or 'AddOption'. To get an empty string use 'AddOptionEmpty'.");

        if (tokens.size() == 1 && typeIsOption)
            throw SparkException("The rule consists of a single 'AddOption' call. Use 'Add' instead for rules with only a single option.");

    }

    void InfoGatherer::AddInternal(RuleToken tok)
    {
        TypeGuard(false);

        TryAddNewRow();

        tokens.back().push_back(tok);
    }

    void InfoGatherer::AddOptionInternal(RuleToken tok)
    {
        TypeGuard(true);

        TryAddNewRow();

        tokens.back().push_back(tok);
    }

    void InfoGatherer::EndOptionInternal()
    {
        if (!hasType || !typeIsOption)
            throw SparkAssertionException("Internal Error: Empty option sequence!");

        newRow = true;
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



} // end namespace

