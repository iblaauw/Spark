#pragma once

#include <vector>
#include <memory>

#include "DebugInfo.h"

namespace Spark
{
    class Node;
    using NodePtr = std::shared_ptr<Node>;

    class Node
    {
    protected:
        std::vector<NodePtr> children;
        std::string name;
        DebugInfo debugInfo;
    public:
        Node() : children(), name("<anon>") {}
        Node(std::initializer_list<NodePtr> init) : children(init), name("<anon>") {}
        Node(const std::vector<NodePtr>& init, std::string name, DebugInfo info) : children(init), name(name), debugInfo(info) {}
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

        const std::vector<NodePtr>& GetChildren() const { return children; }

        inline void SetDebugInfo(const DebugInfo& info) { debugInfo = info; }
        inline const DebugInfo& GetDebugInfo() const { return debugInfo; }
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

