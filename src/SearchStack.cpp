#include "SearchStack.h"

namespace Spark
{
    SearchStack::SearchStack() : first(nullptr), last(nullptr)
    {}

    void SearchStack::Push(int val)
    {
        Node* newNode = new Node { nullptr, val };
        if (empty())
        {
            first = newNode;
            last = newNode;
        }
        else
        {
            last->next = newNode;
            last = newNode;
        }
    }

    void SearchStack::Merge(SearchStack& other)
    {
        if (other.empty())
            return;

        if (empty())
        {
            first = other.first;
            last = other.last;
        }
        else
        {
            last->next = other.first;
            last = other.last;
        }

        other.first = nullptr;
        other.last = nullptr;
    }

    int SearchStack::Pop()
    {
        int val = first->value;
        Node* next = first->next;
        delete first;
        first = next;

        if (first == nullptr)
        {
            last = nullptr;
        }

        return val;
    }

    void SearchStack::Clear()
    {
        while(!empty()) { Pop(); }
    }

    /// Big 5

    SearchStack::~SearchStack()
    {
        Clear();
    }

    SearchStack::SearchStack(SearchStack&& other) : first(other.first), last(other.last)
    {
        other.first = nullptr;
        other.last = nullptr;
    }
}

