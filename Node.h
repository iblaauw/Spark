#pragma once

#include <vector>
#include <memory>

namespace Spark
{
    class Node;
    using NodePtr = std::shared_ptr<Node>;

    class Node
    {
    protected:
        std::vector<NodePtr> children;
    public:
        Node() : children() {}
        Node(std::initializer_list<NodePtr> init) : children(init) {}
        Node(const std::vector<NodePtr>& init) : children(init) {}

        inline NodePtr& operator[](int index) { return children.at(index); }
        inline const NodePtr& operator[](int index) const { return children.at(index); }

        inline void Add(NodePtr ptr) { children.push_back(ptr); }
        inline void Remove(int index) { children.erase(children.begin() + index); }
    };

    class CharNode : public Node
    {
    private:
        char c;
    public:
        CharNode(char c) : c(c) {}

        inline char Get() { return c; }
    };

    class StringNode : public Node
    {
    private:
        std::string str;
    public:
        StringNode(std::string str) : str(str) {}

        inline std::string Get() { return str; }
    };

    // TODO: move to utils file
    template <class T>
    inline NodePtr AsNode(std::shared_ptr<T> ptr)
    {
        return std::static_pointer_cast<Node>(ptr);
    }

} // end namespace

