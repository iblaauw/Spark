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
        std::string name;
    public:
        Node() : children(), name("<anon>") {}
        Node(std::initializer_list<NodePtr> init) : children(init), name("<anon>") {}
        Node(const std::vector<NodePtr>& init, std::string name) : children(init), name(name) {}
        Node(const std::vector<NodePtr>& init) : children(init), name("<anon>") {}

        inline NodePtr& operator[](int index) { return children.at(index); }
        inline const NodePtr& operator[](int index) const { return children.at(index); }

        inline void Add(NodePtr ptr) { children.push_back(ptr); }
        inline void Remove(int index) { children.erase(children.begin() + index); }

        inline int size() const { return children.size(); }

        virtual std::string GetType() const { return "Node"; }
        virtual bool IsBuiltin() const { return true; }

        std::string GetName() const { return name; }
        void SetName(std::string newname) { name = newname; }
    };

    class CharNode : public Node
    {
    private:
        char c;
    public:
        CharNode(char c) : c(c) {}

        inline char Get() { return c; }

        std::string GetType() const override { return "CharNode"; }
    };

    class StringNode : public Node
    {
    private:
        std::string str;
    public:
        StringNode(std::string str) : str(str) {}

        inline std::string Get() { return str; }

        std::string GetType() const override { return "StringNode"; }
    };

    // TODO: move to utils file
    template <class T>
    inline NodePtr AsNode(std::shared_ptr<T> ptr)
    {
        return std::static_pointer_cast<Node>(ptr);
    }

} // end namespace

