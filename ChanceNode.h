#ifndef CHANCE_NODE_H
#define CHANCE_NODE_H

#include "Node.h"
#include <memory>
#include <vector>
#include "ChanceNodeChild.h"
#include "ChanceNodeTypeEnum.h"
using std::unique_ptr;
using std::vector;

class ChanceNode : public Node
{
    private:
        vector<unique_ptr<ChanceNodeChild>> children;

    public:
        ChanceNodeType type;
        ChanceNode(Node* parent, ChanceNodeType type);
        void add_child(unique_ptr<Node> child, uint8_t card);
        vector<unique_ptr<ChanceNodeChild>>& get_children();
};

#endif