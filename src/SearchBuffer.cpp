#include "SearchBuffer.h"

Spark::SearchBuffer::SearchBuffer(InputBuffer& buffer) : buffer(buffer), index(0), debugInfo(buffer.GetDebug())
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

    if (!IsDone())
        debugInfo.Advance(Current());
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

    debugInfo = other.debugInfo;
}


