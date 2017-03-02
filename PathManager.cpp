#include "PathManager.h"

#include <cassert>

Spark::PathManager::PathManager() : values(), position(0)
{}

void Spark::PathManager::TrimTop()
{
    if (values.size() > 0)
    {
        values.pop_front();
        position--;
    }
}

void Spark::PathManager::Enter()
{
    if (position >= values.size())
    {
        values.push_back(-1);
    }
    position++;
}

void Spark::PathManager::Exit()
{
    assert(position > 0);
    position--;
}

bool Spark::PathManager::HasPath() const
{
    return position < values.size() && values.at(position) != -1;
}

int Spark::PathManager::GetPath() const
{
    return values.at(position);
}

void Spark::PathManager::SetPath(int value)
{
    values.at(position) = value;
}

