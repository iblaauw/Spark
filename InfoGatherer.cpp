#include "InfoGatherer.h"

#include "Exceptions.h"

namespace Spark
{
    InfoGatherer::InfoGatherer()
        : newRow(true), hasCustomType(false), sflatten(false), tokens(), flatten()
    {}

    void InfoGatherer::Verify() const
    {
        if (tokens.size() == 0)
            throw SparkException("A rule must call 'Add' at least once. To get an empty string use 'AddEmpty'");
    }

    NodePtr InfoGatherer::Factory(std::vector<NodePtr>& vec)
    {
        if (!hasCustomType)
            throw SparkAssertionException("Error: attempting to create a custom node when no custom type has been set.");

        return factory(vec);
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

    void InfoGatherer::RequestFlatten()
    {
        flatten.at(flatten.size() - 1) = true;
    }

    void InfoGatherer::SetNodeTypeInternal(NodeFactory factory)
    {
        this->factory = factory;
        hasCustomType = true;
    }

    void InfoGatherer::TryAddNewRow()
    {
        if (newRow)
        {
            tokens.emplace_back();
            flatten.push_back(false);
            newRow = false;
        }
    }


} // end namespace

