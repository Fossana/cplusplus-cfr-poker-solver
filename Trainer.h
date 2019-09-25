#ifndef TRAINER_H
#define TRAINER_H

#include "Node.h"
#include "RangeManager.h"
#include "ChanceNode.h"
#include "TerminalNode.h"
#include "BestResponse.h"
#include <memory>
#include <array>
#include <vector>
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

class Trainer
{
    private:
        unique_ptr<BestResponse> br;
        shared_ptr<RangeManager> rangeManager;
        int initialPot;
        int inPositionPlayer;
		uint8_t initialBoard[5];

		vector<float> cfr(int hero, int villain, Node *root, int iterationCount);

    public:
        Trainer(shared_ptr<RangeManager> rangeManager, uint8_t initialBoard[5], int initialPot, int inPositionPlayer);
        void train(Node* root, int numIterations);
};

#endif