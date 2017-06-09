#pragma once

#include <deque>
#include <iostream>

#include "message_exception.h"
#include "DebugInfo.h"

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

        DebugInfo debugInfo;
    public:
        InputBuffer(std::istream& input);

        int size() const;
        bool IsDone() const;

        inline int LineNum() { return debugInfo.lineNum; }
        inline int CharNum() { return debugInfo.charNum; }

        char GetNext();
        char PeekAt(int index) const;
        void LoadNextBlock();

        const DebugInfo& GetDebug() const { return debugInfo; }

        std::istream& GetRawStream() { return input; }
    };

    DECL_EXCEPTION(InputBufferException);
}

