#include "InputBuffer.h"

Spark::InputBuffer::InputBuffer(std::istream& input) :
    input(input),
    buffer(),
    lastBlock(false)
{
    LoadNextBlock();
}

int Spark::InputBuffer::size() const
{
    return buffer.size();
}

bool Spark::InputBuffer::IsDone() const
{
    return buffer.size() == 0 && lastBlock;
}

char Spark::InputBuffer::GetNext()
{
    if (IsDone())
        throw InputBufferException("Can't get next on empty input buffer");

    if (buffer.size() <= SHRINK_LIMIT)
    {
        LoadNextBlock();
    }

    char c = buffer.front();
    buffer.pop_front();

    if (c == '\n')
    {
        lineNum++;
        charNum = -1;
    }
    else
    {
        charNum++;
    }

    return c;
}

char Spark::InputBuffer::PeekAt(int index) const
{
    return buffer[index];
}

void Spark::InputBuffer::LoadNextBlock()
{
    if (lastBlock)
        return;

    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        int val = input.get();
        if (val == -1)
        {
            lastBlock = true;
            return;
        }

        char c = val;
        buffer.push_back(c);
    }
}



