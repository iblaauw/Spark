#pragma once

#include <vector>

#include "IRuleBuilder.h"
#include "InputBuffer.h"
#include "PathManager.h"
#include "RuleSearcher.h"

namespace Spark
{
    class RuleBuilder : public IRuleBuilder
    {
    private:
        InputBuffer& buffer;
        PathManager& path;

        bool foundType;
        bool isOption;

        int optionCount = 0;
        RuleSearcher* searcher = nullptr;
    public:
        RuleBuilder(InputBuffer& buffer, PathManager& path);

        virtual void Add(RuleFunc rule) override;
        virtual void Add(char c) override;

    protected:
        virtual void AddOptionInternal(RuleFunc func) override;
        virtual void AddOptionInternal(char c) override;
        virtual void EndOptionInternal() override;

    private:
        RuleBuilder(RuleBuilder& other);

        void LockType(bool option);
        void DoAdd(RuleFunc rule);
        void DoAdd(char c);
    };
}

