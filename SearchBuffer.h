#pragma once

#include "InputBuffer.h"

namespace Spark
{
    class SearchBuffer
    {
    private:
        InputBuffer& buffer;
        int index;
    public:
        SearchBuffer(InputBuffer& buffer);
        SearchBuffer(SearchBuffer& other) = default;

        void Advance();
        bool IsDone() const;
        char Current() const;

        void AdvanceTo(const SearchBuffer& other);
    };
}
