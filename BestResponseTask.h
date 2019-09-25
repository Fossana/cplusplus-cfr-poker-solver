#pragma once
#ifndef BEST_RESPONSE_TASK_H
#define BEST_RESPONSE_TASK_H

#include <memory>
#include "RangeManager.h"
#include "Node.h"
#include "ActionNode.h"
#include "ChanceNode.h"
#include "TerminalNode.h"
#include "tbb/task.h"
#include <stdint.h>
using std::unique_ptr;
using std::shared_ptr;
using tbb::task;

class BestResponseTask : public task
{
private:
	shared_ptr<RangeManager> rangeManager;

	vector<float> chance_node_best_response(ChanceNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5]);
	vector<float> terminal_node_best_response(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5]);
	vector<float> showdown_best_response(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5]);
	vector<float> allin_best_response(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5]);
	vector<float> uncontested_best_response(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5]);

public:
	Node* node;
	int hero;
	int villain;
	vector<float>* villainReachProbs;
	uint8_t board[5];

	vector<float>* result;
	task* execute();

	BestResponseTask(shared_ptr<RangeManager> rangeManager, vector<float>* result, Node* node, int hero, int villain, vector<float>* villainReachProbs, uint8_t board[5]);
};

#endif