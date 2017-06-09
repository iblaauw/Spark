#pragma once

namespace Spark
{
    // This is actually more of a queue than a stack
    class SearchStack
    {
    private:
        struct Node
        {
            Node* next;
            int value;
        };

        Node* first;
        Node* last;
    public:
        SearchStack();

        // Big 5. Do not allow copies to be made, that is just a bad idea
        ~SearchStack();
        SearchStack(const SearchStack& other) = delete;
        SearchStack& operator=(const SearchStack& other) = delete;
        SearchStack(SearchStack&& other);
        SearchStack& operator=(SearchStack&& other) = delete;

        void Push(int val);
        void Merge(SearchStack& other);
        int Pop();
        void Clear();

        bool empty() const { return first == nullptr && last == nullptr; }
    };
}
