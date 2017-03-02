#pragma once

#include "IRuleBuilder.h"
#include "SearchBuffer.h"
#include "RuleFunc.h"
#include "PathManager.h"


namespace Spark
{
    class RuleSearcher : public IRuleBuilder
    {
    private:
        SearchBuffer buffer;
        PathManager& path;

        int optionCount = 0;
        RuleSearcher* nested = nullptr;

        bool done = false;
        bool result = false;
    public:
        RuleSearcher(InputBuffer& buffer, PathManager& path) : buffer(buffer), path(path) {}

        virtual void Add(RuleFunc rule) override;
        virtual void Add(char c) override;

        inline bool Succeeded() { return result; }

        void End();

    protected:
        virtual void AddOptionInternal(RuleFunc func) override;
        virtual void AddOptionInternal(char c) override;
        virtual void EndOptionInternal() override;

    private:
        RuleSearcher(RuleSearcher& other);
    };
}
