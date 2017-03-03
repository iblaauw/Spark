#include "SearchBuffer.h"

Spark::SearchBuffer::SearchBuffer(InputBuffer& buffer) : buffer(buffer), index(0)
{}

void Spark::SearchBuffer::Advance()
{
    if (IsDone())
        throw InputBufferException("Cannot advance past end of search buffer.");

    index++;

    if (index >= buffer.size() - 1)
    {
        buffer.LoadNextBlock();
    }
}

bool Spark::SearchBuffer::IsDone() const
{
    return buffer.IsDone() && index >= buffer.size();
}

char Spark::SearchBuffer::Current() const
{
    return buffer.PeekAt(index);
}

void Spark::SearchBuffer::AdvanceTo(const SearchBuffer& other)
{
    index = other.index;
}


