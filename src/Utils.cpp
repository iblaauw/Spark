#include "Node.h"

#include <iostream>

namespace Spark
{
    // Internal helper func
    void PrintLevel(int level)
    {
        for (int i = 0; i < level * 2; i++)
        {
            std::cout << ' ';
        }
    }

    void PrintTree(NodePtr root, int level = 0)
    {
        PrintLevel(level);

        std::string type = root->GetType();
        if (type == "CharNode")
        {
            auto nodeptr = std::static_pointer_cast<Spark::CharNode>(root);
            std::cout << '\'' << nodeptr->Get() << '\'' << std::endl;
            return;
        }
        else if (type == "StringNode")
        {
            auto nodeptr = std::static_pointer_cast<Spark::StringNode>(root);
            std::cout << '"' << nodeptr->Get() << '"' << std::endl;
            return;
        }
        else
        {
            std::string name = root->GetName();
            if (name.size() == 0)
                name = "<unknown>";
            std::cout << name << " (" << type << ")" << std::endl;
        }

        Spark::Node& node = *root;

        for (int i = 0; i < root->size(); i++)
        {
            PrintTree( node[i], level+1);
        }
    }

}

