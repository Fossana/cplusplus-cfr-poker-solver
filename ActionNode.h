#ifndef ACTION_NODE_H
#define ACTION_NODE_H

#include "Node.h"
#include "Action.h"
#include <vector>
#include <memory>
using std::unique_ptr;
using std::vector;

class ActionNode : public Node
{
    private:
        vector<float> strategySum;
        vector<float> regretSum;

    public:
        int numHands = 0;
        int numActions = 0;

        vector<unique_ptr<Action>> actions;
        vector<unique_ptr<Node>> children;
        int player;
        ActionNode(Node* parent, int player, int numHands);
        void add_child(unique_ptr<Node> child, unique_ptr<Action> action);
        void initialize_strategySum();
        void initialize_regretSum();
		vector<float> get_average_strategy();
        int get_child_count();
        Node* get_child(int index);
		vector<float> get_current_strategy();
        void update_regretSum_part_one(vector<float>& actionUtilities, int actionIndex);
        void update_regretSum_part_two(vector<float>& utilities, int iterationCount);
        void update_strategySum(vector<float>& strategy, vector<float>& reachProbs, int iterationCount);
};

#endif