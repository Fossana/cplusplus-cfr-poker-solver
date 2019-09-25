#ifndef CHANCE_NODE_CHILD_H
#define CHANCE_NODE_CHILD_H

#include "Node.h"
#include <memory>
using std::unique_ptr;

class ChanceNodeChild
{
    public:
		uint8_t card;
        unique_ptr<Node> node;
        ChanceNodeChild(unique_ptr<Node> node, uint8_t card);
};

#endif