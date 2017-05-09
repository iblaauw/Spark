#pragma once

#include <vector>

#include "IRuleBuilder.h"
#include "RuleToken.h"

namespace Spark
{
    using NodeFactory = std::function<NodePtr(std::vector<NodePtr>&)>;

    class InfoGatherer : public IRuleBuilder
    {
    private:
        bool newRow;
        bool hasCustomType;
        bool sflatten;
        std::vector<std::vector<RuleToken>> tokens;
        std::vector<bool> flatten;
        std::string name;
        NodeFactory factory;

    public:
        InfoGatherer();

        inline int NumOptions() { return tokens.size(); }

        inline std::vector<RuleToken>& Get(int index)
        {
            return tokens[index];
        }

        void Verify() const;

        void SetName(std::string newname) override { name = newname; }
        std::string GetName() const { return name; }

        inline bool ShouldFlatten(int index) const { return flatten.at(index); }

        inline bool HasCustomType() const { return hasCustomType; }

        NodePtr Factory(std::vector<NodePtr>& vec);

    protected:
        virtual void AddInternal(RuleToken tok) override;
        virtual void EndInternal() override;
        virtual void AddEmptyInternal() override;
        virtual void RequestFlatten() override;
        virtual void SetNodeTypeInternal(NodeFactory factory);

    private:
        void TryAddNewRow();
    };
}


