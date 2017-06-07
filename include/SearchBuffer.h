#pragma once

#include "InputBuffer.h"

namespace Spark
{
    class SearchBuffer
    {
    private:
        InputBuffer& buffer;
        int index;
        DebugInfo debugInfo;
    public:
        SearchBuffer(InputBuffer& buffer);
        SearchBuffer(SearchBuffer& other) = default;

        void Advance();
        bool IsDone() const;
        char Current() const;

        void AdvanceTo(const SearchBuffer& other);

        inline const DebugInfo& GetDebug() const { return debugInfo; }
    };
}
