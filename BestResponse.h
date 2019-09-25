#ifndef BEST_RESPONSE_H
#define BEST_RESPONSE_H

#include <memory>
#include "RangeManager.h"
#include "Node.h"
#include "ActionNode.h"
#include "ChanceNode.h"
#include "TerminalNode.h"
#include <stdint.h>
#include <vector>
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

class BestResponse
{
    private:
        shared_ptr<RangeManager> rangeManager;
		vector<float> p1RelativeProbs;
		vector<float> p2RelativeProbs;

    public:
		Node* root;
		uint8_t initialBoard[5];
		int initialPot;
		int inPositionPlayer;

        BestResponse(shared_ptr<RangeManager> rangeManager, Node* root, uint8_t initialBoard[5], int initialPot, int inPositionPlayer);
        float get_best_response_Ev(int hero, int villain);
        void print_exploitability();
		float get_unblocked_combo_count(Hand& heroHand, vector<Hand>& villainHands);
		void set_relative_probabilities(uint8_t initialBoard[5]);
};

#endif