#pragma once

#include <deque>
#include <iostream>

#include "message_exception.h"

namespace Spark
{
    class InputBuffer
    {
    private:
        static const int BLOCK_SIZE = 100;
        static const int SHRINK_LIMIT = 10;

        std::istream& input;
        std::deque<char> buffer;
        bool lastBlock;
        int lineNum = 0;
        int charNum = 0;
    public:
        InputBuffer(std::istream& input);

        int size() const;
        bool IsDone() const;

        inline int LineNum() { return lineNum; }
        inline int CharNum() { return charNum; }

        char GetNext();
        char PeekAt(int index) const;
        void LoadNextBlock();
    };

    DECL_EXCEPTION(InputBufferException);
}

