#include "ActionNode.h"
#include <cstring>
#include <iostream>
#include <cmath>
using std::move;
using std::make_unique;
using std::memset;
using std::pow;

ActionNode::ActionNode(Node* parent, int player, int numHands)
{
    this->numHands = numHands;
    this->parent = parent;
    this->player = player;
}

void ActionNode::add_child(unique_ptr<Node> child, unique_ptr<Action> action)
{
    numActions++;
    children.push_back(move(child));
    actions.push_back(move(action));
}

int ActionNode::get_child_count()
{
    return children.size();
}

Node* ActionNode::get_child(int index)
{
    return children.at(index).get();
}

void ActionNode::initialize_strategySum()
{
    strategySum = vector<float>(numHands * numActions);
}

void ActionNode::initialize_regretSum()
{
    regretSum = vector<float>(numHands * numActions);
}

vector<float> ActionNode::get_average_strategy()
{
	vector<float> averageStrategy = vector<float>(numHands*numActions);

    for (int hand = 0; hand < numHands; hand++)
    {
        float total = 0;
        
        for (int action = 0; action < numActions; action++)
            total += strategySum[hand + action*numHands];
        
        if (total > 0)
            for (int action = 0; action < numActions; action++)
                averageStrategy[hand + action*numHands] = strategySum[hand + action*numHands] / total;
        else
            for (int action = 0; action < numActions; action++)
                averageStrategy[hand + action*numHands] = 1.0f / numActions;
    }
		
	return averageStrategy;
}

vector<float> ActionNode::get_current_strategy()
{
	switch (numActions)
	{
		case 2:
		{
			vector<float> strategy = vector<float>(numHands * numActions);

			float const * __restrict regretSum0 = &regretSum[0];
			float const * __restrict regretSum1 = &regretSum[numHands];

			float* __restrict strategy0 = &strategy[0];
			float* __restrict strategy1 = &strategy[numHands];

			for (int hand = 0; hand < numHands; hand++)
			{
				if (regretSum0[hand] > 0.0f)
				{
					if (regretSum1[hand] > 0.0f)
					{
						float positiveRegretSum = regretSum0[hand] + regretSum1[hand];
						strategy0[hand] = regretSum0[hand] / positiveRegretSum;
						strategy1[hand] = regretSum1[hand] / positiveRegretSum;
					}
					else
					{
						strategy0[hand] = 1.0f;
					}
				}
				else if (regretSum1[hand] > 0.0f)
				{
					strategy1[hand] = 1.0f;
				}
				else
				{
					strategy0[hand] = 0.5f;
					strategy1[hand] = 0.5f;
				}
			}

			return strategy;
		}
		case 3:
		{
			vector<float> strategy = vector<float>(numHands * numActions);

			float const * __restrict regretSum0 = &regretSum[0];
			float const * __restrict regretSum1 = &regretSum[numHands];
			float const * __restrict regretSum2 = &regretSum[numHands + numHands];

			float* __restrict strategy0 = &strategy[0];
			float* __restrict strategy1 = &strategy[numHands];
			float* __restrict strategy2 = &strategy[numHands + numHands];

			for (int hand = 0; hand < numHands; hand++)
			{
				if (regretSum0[hand] > 0.0f)
				{
					if (regretSum1[hand] > 0.0f)
					{
						if (regretSum2[hand] > 0.0f) //0, 1, 3
						{
							float positiveRegretSum = regretSum0[hand] + regretSum1[hand] + regretSum2[hand];
							strategy0[hand] = regretSum0[hand] / positiveRegretSum;
							strategy1[hand] = regretSum1[hand] / positiveRegretSum;
							strategy2[hand] = regretSum2[hand] / positiveRegretSum;
						}
						else //0, 1
						{
							float positiveRegretSum = regretSum0[hand] + regretSum1[hand];
							strategy0[hand] = regretSum0[hand] / positiveRegretSum;
							strategy1[hand] = regretSum1[hand] / positiveRegretSum;
						}
					}
					else if (regretSum2[hand] > 0.0f) //0, 2
					{
						float positiveRegretSum = regretSum0[hand] + regretSum2[hand];
						strategy0[hand] = regretSum0[hand] / positiveRegretSum;
						strategy2[hand] = regretSum2[hand] / positiveRegretSum;
					}
					else //0
					{
						strategy0[hand] = 1.0f;
					}
				}
				else if (regretSum1[hand] > 0.0f)
				{
					if (regretSum2[hand] > 0.0f) //1, 2
					{
						float positiveRegretSum = regretSum1[hand] + regretSum2[hand];
						strategy1[hand] = regretSum1[hand] / positiveRegretSum;
						strategy2[hand] = regretSum2[hand] / positiveRegretSum;
					}
					else //1
					{
						strategy1[hand] = 1.0f;
					}
				}
				else if (regretSum2[hand] > 0.0f) //0
				{
					strategy2[hand] = 1.0f;
				}
				else
				{
					strategy0[hand] = (1.0f / 3.0f);
					strategy1[hand] = (1.0f / 3.0f);
					strategy2[hand] = (1.0f / 3.0f);
				}
			}

			return strategy;
		}
		case 4:
		{
			vector<float> strategy = vector<float>(numHands * numActions);

			float const * __restrict regretSum0 = &regretSum[0];
			float const * __restrict regretSum1 = &regretSum[numHands];
			float const * __restrict regretSum2 = &regretSum[numHands + numHands];
			float const * __restrict regretSum3 = &regretSum[numHands + numHands + numHands];

			float* __restrict strategy0 = &strategy[0];
			float* __restrict strategy1 = &strategy[numHands];
			float* __restrict strategy2 = &strategy[numHands + numHands];
			float* __restrict strategy3 = &strategy[numHands + numHands + numHands];

			for (int hand = 0; hand < numHands; hand++)
			{
				float positiveRegretSum = 0.0f;

				if (regretSum0[hand] > 0.0f)
					positiveRegretSum += regretSum0[hand];

				if (regretSum1[hand] > 0.0f)
					positiveRegretSum += regretSum1[hand];

				if (regretSum2[hand] > 0.0f)
					positiveRegretSum += regretSum2[hand];

				if (regretSum3[hand] > 0.0f)
					positiveRegretSum += regretSum3[hand];

				if (positiveRegretSum > 0.0f)
				{
					if (regretSum0[hand] > 0.0f)
						strategy0[hand] = regretSum0[hand] / positiveRegretSum;

					if (regretSum1[hand] > 0.0f)
						strategy1[hand] = regretSum1[hand] / positiveRegretSum;

					if (regretSum2[hand] > 0.0f)
						strategy2[hand] = regretSum2[hand] / positiveRegretSum;

					if (regretSum3[hand] > 0.0f)
						strategy3[hand] = regretSum3[hand] / positiveRegretSum;
				}
				else
				{
					strategy0[hand] = 0.25f;
					strategy1[hand] = 0.25f;
					strategy2[hand] = 0.25f;
					strategy3[hand] = 0.25f;
				}
			}

			return strategy;
		}
		default:
		{
			int size = numHands * numActions;
			vector<float> strategy = vector<float>(size);
			int index;

			for (int hand = 0; hand < numHands; hand++)
			{
				float positiveRegretSum = 0.0f;

				for (index = hand; index < hand + size; index += numHands)
					if (regretSum[index] > 0.0f)
						positiveRegretSum += regretSum[index];

				if (positiveRegretSum > 0.0f)
				{
					for (int index = hand; index < hand + size; index += numHands)
						if (regretSum[index] > 0.0f)
							strategy[index] = regretSum[index] / positiveRegretSum;
				}
				else
				{
					float uniformProbability = 1.0f / numActions;
					for (int index = hand; index < hand + size; index += numHands)
						strategy[index] = uniformProbability;
				}
			}
			return strategy;
		}
	}
}

void ActionNode::update_regretSum_part_one(vector<float>& actionUtilities, int actionIndex)
{
	float* __restrict regretSumAction = &regretSum[actionIndex * numHands];
	#pragma code_align 32
	for (int hand = 0; hand < numHands; hand++)
		regretSumAction[hand] += actionUtilities[hand];
}

void ActionNode::update_regretSum_part_two(vector<float>& utilities, int iterationCount)
{
	switch (numActions)
	{
		case 2:
		{
			float x = pow(iterationCount, 1.5f);
			x = x / (x + 1);

			float* __restrict regretSum0 = &regretSum[0];
			float* __restrict regretSum1 = &regretSum[numHands];

			for (int hand = 0; hand < numHands; hand++)
			{
				regretSum0[hand] -= utilities[hand];
				if (regretSum0[hand] > 0.0f)
					regretSum0[hand] *= x;
				else if (regretSum0[hand] < 0.0f)
					regretSum0[hand] *= 0.5f;

				regretSum1[hand] -= utilities[hand];
				if (regretSum1[hand] > 0.0f)
					regretSum1[hand] *= x;
				else if (regretSum1[hand] < 0.0f)
					regretSum1[hand] *= 0.5f;
			}
		}
		break;
		case 3:
		{
			float x = pow(iterationCount, 1.5f);
			x = x / (x + 1);

			float* __restrict regretSum0 = &regretSum[0];
			float* __restrict regretSum1 = &regretSum[numHands];
			float* __restrict regretSum2 = &regretSum[numHands + numHands];

			for (int hand = 0; hand < numHands; hand++)
			{
				regretSum0[hand] -= utilities[hand];
				if (regretSum0[hand] > 0.0f)
					regretSum0[hand] *= x;
				else if (regretSum0[hand] < 0.0f)
					regretSum0[hand] *= 0.5f;

				regretSum1[hand] -= utilities[hand];
				if (regretSum1[hand] > 0.0f)
					regretSum1[hand] *= x;
				else if (regretSum1[hand] < 0.0f)
					regretSum1[hand] *= 0.5f;

				regretSum2[hand] -= utilities[hand];
				if (regretSum2[hand] > 0.0f)
					regretSum2[hand] *= x;
				else if (regretSum2[hand] < 0.0f)
					regretSum2[hand] *= 0.5f;
			}
		}
		break;
		case 4:
		{
			float x = pow(iterationCount, 1.5f);
			x = x / (x + 1);

			float* __restrict regretSum0 = &regretSum[0];
			float* __restrict regretSum1 = &regretSum[numHands];
			float* __restrict regretSum2 = &regretSum[numHands + numHands];
			float* __restrict regretSum3 = &regretSum[numHands + numHands + numHands];

			for (int hand = 0; hand < numHands; hand++)
			{
				regretSum0[hand] -= utilities[hand];
				if (regretSum0[hand] > 0.0f)
					regretSum0[hand] *= x;
				else if (regretSum0[hand] < 0.0f)
					regretSum0[hand] *= 0.5f;

				regretSum1[hand] -= utilities[hand];
				if (regretSum1[hand] > 0.0f)
					regretSum1[hand] *= x;
				else if (regretSum1[hand] < 0.0f)
					regretSum1[hand] *= 0.5f;

				regretSum2[hand] -= utilities[hand];
				if (regretSum2[hand] > 0.0f)
					regretSum2[hand] *= x;
				else if (regretSum2[hand] < 0.0f)
					regretSum2[hand] *= 0.5f;

				regretSum3[hand] -= utilities[hand];
				if (regretSum3[hand] > 0.0f)
					regretSum3[hand] *= x;
				else if (regretSum3[hand] < 0.0f)
					regretSum3[hand] *= 0.5f;
			}
		}
		break;
		default:
		{
			float x = pow(iterationCount, 1.5f);
			x = x / (x + 1);
			int index;

			for (int action = 0; action < numActions; action++)
			{
				index = action * numHands;
				for (int hand = 0; hand < numHands; hand++)
				{
					regretSum[index] -= utilities[hand];
					if (regretSum[index] > 0.0f)
						regretSum[index] *= x;
					else if (regretSum[index] < 0.0f)
						regretSum[index] *= 0.5f;
					index++;
				}
			}
		}
	}
}

void ActionNode::update_strategySum(vector<float>& strategy, vector<float>& reachProbs, int iterationCount)
{
	//if (iterationCount <= 50)
	//	return;

	switch (numActions)
	{
		case 2:
		{
			float x = pow(((float)iterationCount) / (iterationCount + 1), 2);

			float* __restrict strategySum0 = &strategySum[0];
			float* __restrict strategySum1 = &strategySum[numHands];

			float* __restrict strategy0 = &strategy[0];
			float* __restrict strategy1 = &strategy[numHands];

			for (int hand = 0; hand < numHands; hand++)
			{
				strategySum0[hand] += strategy0[hand] * reachProbs[hand];
				strategySum0[hand] *= x;

				strategySum1[hand] += strategy1[hand] * reachProbs[hand];
				strategySum1[hand] *= x;
			}

			break;
		}
		case 3:
		{
			float x = pow(((float)iterationCount) / (iterationCount + 1), 2);

			float* __restrict strategySum0 = &strategySum[0];
			float* __restrict strategySum1 = &strategySum[numHands];
			float* __restrict strategySum2 = &strategySum[numHands + numHands];

			float* __restrict strategy0 = &strategy[0];
			float* __restrict strategy1 = &strategy[numHands];
			float* __restrict strategy2 = &strategy[numHands + numHands];

			for (int hand = 0; hand < numHands; hand++)
			{
				strategySum0[hand] += strategy0[hand] * reachProbs[hand];
				strategySum0[hand] *= x;

				strategySum1[hand] += strategy1[hand] * reachProbs[hand];
				strategySum1[hand] *= x;

				strategySum2[hand] += strategy2[hand] * reachProbs[hand];
				strategySum2[hand] *= x;
			}

			break;
		}
		case 4:
		{
			float x = pow(((float)iterationCount) / (iterationCount + 1), 2);

			float* __restrict strategySum0 = &strategySum[0];
			float* __restrict strategySum1 = &strategySum[numHands];
			float* __restrict strategySum2 = &strategySum[numHands + numHands];
			float* __restrict strategySum3 = &strategySum[numHands + numHands + numHands];

			float* __restrict strategy0 = &strategy[0];
			float* __restrict strategy1 = &strategy[numHands];
			float* __restrict strategy2 = &strategy[numHands + numHands];
			float* __restrict strategy3 = &strategy[numHands + numHands + numHands];

			for (int hand = 0; hand < numHands; hand++)
			{
				strategySum0[hand] += strategy0[hand] * reachProbs[hand];
				strategySum0[hand] *= x;

				strategySum1[hand] += strategy1[hand] * reachProbs[hand];
				strategySum1[hand] *= x;

				strategySum2[hand] += strategy2[hand] * reachProbs[hand];
				strategySum2[hand] *= x;

				strategySum3[hand] += strategy3[hand] * reachProbs[hand];
				strategySum3[hand] *= x;
			}

			break;
		}
		default:
		{
			float x = pow(((float)iterationCount) / (iterationCount + 1), 2);

			for (int action = 0; action < numActions; action++)
			{
				int index = action * numHands;
				for (int hand = 0; hand < numHands; hand++)
				{
					strategySum[index] += strategy[index] * reachProbs[hand];
					strategySum[index++] *= x;
				}
			}
		}
	}
}