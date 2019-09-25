#include "TerminalNode.h"

TerminalNode::TerminalNode(Node* parent, TerminalNodeType type)
{
    this->parent = parent;
    this->type = type;
}