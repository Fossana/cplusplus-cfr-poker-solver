#include "ChanceNode.h"
using std::make_unique;
using std::move;

ChanceNode::ChanceNode(Node* parent, ChanceNodeType type)
{
    this->parent = parent;
    this->type = type;
}

void ChanceNode::add_child(unique_ptr<Node> child, uint8_t card)
{
    children.push_back(make_unique<ChanceNodeChild>(move(child), card));
}

vector<unique_ptr<ChanceNodeChild>>& ChanceNode::get_children()
{
    return children;
}