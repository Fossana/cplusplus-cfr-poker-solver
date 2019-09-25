#ifndef GAME_TREE_H
#define GAME_TREE_H

#include "TreeBuildSettings.h"
#include "State.h"
#include "Action.h"
#include "Node.h"
#include "ActionNode.h"
#include "ChanceNode.h"
#include "TerminalNode.h"
#include <vector>
#include <memory>
using std::unique_ptr;
using std::vector;

class GameTree {
    private:
        unique_ptr<State> get_initial_state();
        void build_action(
            Node* actionNode,
            State& state,
            int p1NumHands,
            int p2NumHands,
            unique_ptr<Action> action);
        unique_ptr<Node> build_action_nodes(Node* parent, State& state);
        unique_ptr<Node> build_chance_nodes(Node* parent, State& state);
        unique_ptr<Node> build_terminal_nodes(Node* parent, State& state);
    
    public:
        unique_ptr<TreeBuildSettings> treeBuildSettings;
        
        void print_tree(Node* root, int tabCount);
        GameTree(unique_ptr<TreeBuildSettings> treeBuildSettings);
        unique_ptr<Node> build();
};

#endif