#pragma once

#include <deque>

namespace Spark
{
    class PathManager
    {
    private:
        std::deque<int> values;
        int position;
    public:
        PathManager();

        void TrimTop();
        void Enter();
        void Exit();

        bool HasPath() const;
        int GetPath() const;
        void SetPath(int value);
    };
}
