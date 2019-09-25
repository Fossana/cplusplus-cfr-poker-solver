#ifndef TERMINAL_NODE_H
#define TERMINAL_NODE_H

#include "TerminalNodeTypeEnum.h"
#include "Node.h"
#include <memory>
using std::unique_ptr;

class TerminalNode : public Node
{
    public:
        TerminalNodeType type;
        int lastToAct;
        float value;
        int board[5];

        TerminalNode(Node* parent, TerminalNodeType type);
};

#endif