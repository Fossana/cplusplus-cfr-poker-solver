#ifndef NODE_H
#define NODE_H

#include "NodeTypeEnum.h"
#include <memory>
using std::unique_ptr;

class Node {
    public:
		NodeType type;
        Node* parent;
        virtual ~Node() = default;
};

#endif