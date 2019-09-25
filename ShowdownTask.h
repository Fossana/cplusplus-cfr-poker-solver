#pragma once
#ifndef SHOWDOWN_TASK_H
#define SHOWDOWN_TASK_H

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

class ShowdownTask : public task
{

public:
	shared_ptr<RangeManager> rangeManager;
	TerminalNode* node;
	int hero;
	int villain;
	vector<float> villainReachProbs;
	uint8_t board[5];

	vector<float>* result;
	task* execute();

	ShowdownTask(shared_ptr<RangeManager> rangeManager, vector<float>* result, TerminalNode* node, int hero, int villain, vector<float> villainReachProbs, uint8_t board[5]);
};

#endif