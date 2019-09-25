#ifndef CFR_TASK_H
#define CFR_TASK_H

#include "Node.h"
#include "ChanceNode.h"
#include "TerminalNode.h"
#include "BestResponse.h"
#include <memory>
#include <array>
#include <stdint.h>
#include <vector>
#include "tbb/task.h"
#include "RangeManager.h"
using tbb::task;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

class CfrTask : public task
{
	public:
		shared_ptr<RangeManager> rangeManager;

		Node* node;
		int hero;
		int villain;
		vector<float>* villainReachProbs;
		uint8_t board[5];
		int iterationCount;

		vector<float>* result;

		task* execute();
		//CfrTask(shared_ptr<PreflopRangeManager> prm, shared_ptr<RiverRangeManager> rrm, vector<float>* result, Node* node, int hero, int villain, vector<float> heroReachProbs, vector<float> villainReachProbs, int board[5], int iterationCount);
		CfrTask(shared_ptr<RangeManager> rangeManager, vector<float>* result, Node* node, int hero, int villain, vector<float>* villainReachProbs, uint8_t board[5], int iterationCount);
		vector<float> chance_node_utility(ChanceNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5], int iterationCount);
		vector<float> terminal_node_utility(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5], int iterationCount);
		vector<float> allin_utility(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5], int iterationCount);
		vector<float> showdown_utility(TerminalNode* node, const int hero, const int villain, const vector<float>& villainReachProbs, uint8_t board[5], const int iterationCount);
		vector<float> uncontested_utility(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5], int iterationCount);
};

#endif