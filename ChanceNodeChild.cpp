#include "ChanceNodeChild.h"

using std::move;

ChanceNodeChild::ChanceNodeChild(unique_ptr<Node> node, uint8_t card)
{
    this->node = move(node);
    this->card = card;
}