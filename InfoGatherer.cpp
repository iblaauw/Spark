#include "InfoGatherer.h"

#include "Exceptions.h"

namespace Spark
{
    InfoGatherer::InfoGatherer()
        : newRow(true), tokens()
    {}

    void InfoGatherer::Verify() const
    {
        if (tokens.size() == 0)
            throw SparkException("A rule must call 'Add' at least once. To get an empty string user 'AddEmpty'");
    }

    void InfoGatherer::AddInternal(RuleToken tok)
    {
        TryAddNewRow();

        tokens.back().push_back(tok);
    }

    void InfoGatherer::EndInternal()
    {
        newRow = true;
    }

    void InfoGatherer::AddEmptyInternal()
    {
        if (!newRow)
            throw SparkAssertionException("Empty string inserted into an option sequence.");

        TryAddNewRow();

        newRow = true;
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

