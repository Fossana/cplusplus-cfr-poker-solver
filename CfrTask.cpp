#include "CfrTask.h"
#include "card_utility.h"
#include "ShowdownTask.h"
#include "NodeTypeEnum.h"
#include <iostream>
#include "tbb/task_group.h"
using tbb::task_group;

CfrTask::CfrTask(shared_ptr<RangeManager> rangeManager, vector<float>* result, Node* node, int hero, int villain, vector<float>* villainReachProbs, uint8_t board[5], int iterationCount)
{
	this->rangeManager = rangeManager;
	this->result = result;
	this->node = node;
	this->hero = hero;
	this->villain = villain;
	this->villainReachProbs = villainReachProbs;
	for (int i = 0; i < 5; i++)
		this->board[i] = board[i];
	this->iterationCount = iterationCount;
}

task* CfrTask::execute()
{
	if (typeid(*node) == typeid(TerminalNode))
	{
		*result = terminal_node_utility(static_cast<TerminalNode*>(node), hero, villain, *villainReachProbs, board, iterationCount);
		return NULL;
	}

	if (typeid(*node) == typeid(ChanceNode))
	{
		*result = chance_node_utility(static_cast<ChanceNode*>(node), hero, villain, *villainReachProbs, board, iterationCount);
		return NULL;
	}

	ActionNode* actionNode = static_cast<ActionNode*>(node);

	int numHeroHands = rangeManager->get_num_hands(hero, board);
	int numVillainHands = rangeManager->get_num_hands(villain, board);
	int numActions = actionNode->numActions;

	if (hero == actionNode->player)
	{
		*result = vector<float>(numHeroHands);
		vector<vector<float>> results(numActions);

		vector<float> strategy = actionNode->get_current_strategy();
		vector<CfrTask*> cfrTasks = vector<CfrTask*>(numActions);

		vector<float>& utilities = *result;

		set_ref_count(numActions + 1);

		for (int action = 0; action < numActions; action++)
			cfrTasks[action] = new (allocate_child()) CfrTask(rangeManager, &results[action], actionNode->get_child(action), hero, villain, villainReachProbs, board, iterationCount);

		for (int action = 0; action < numActions-1; action++)
			spawn(*cfrTasks[action]);

		spawn_and_wait_for_all(*cfrTasks[numActions-1]);

		switch (numActions)
		{
			case 2:
			{
				vector<float>& actionUtilities0 = results[0];
				vector<float>& actionUtilities1 = results[1];

				actionNode->update_regretSum_part_one(actionUtilities0, 0);
				actionNode->update_regretSum_part_one(actionUtilities1, 1);

				float* __restrict strategy0 = &strategy[0];
				float* __restrict strategy1 = &strategy[numHeroHands];

				for (int hand = 0; hand < numHeroHands; hand++)
					utilities[hand] =
						strategy0[hand] * actionUtilities0[hand]
						+ strategy1[hand] * actionUtilities1[hand];
			}
			break;
			case 3:
			{
				vector<float>& actionUtilities0 = results[0];
				vector<float>& actionUtilities1 = results[1];
				vector<float>& actionUtilities2 = results[2];

				actionNode->update_regretSum_part_one(actionUtilities0, 0);
				actionNode->update_regretSum_part_one(actionUtilities1, 1);
				actionNode->update_regretSum_part_one(actionUtilities2, 2);

				float* __restrict strategy0 = &strategy[0];
				float* __restrict strategy1 = &strategy[numHeroHands];
				float* __restrict strategy2 = &strategy[numHeroHands + numHeroHands];

				for (int hand = 0; hand < numHeroHands; hand++)
					utilities[hand] =
						strategy0[hand] * actionUtilities0[hand]
						+ strategy1[hand] * actionUtilities1[hand]
						+ strategy2[hand] * actionUtilities2[hand];
			}
			break;
			case 4:
			{
				vector<float>& actionUtilities0 = results[0];
				vector<float>& actionUtilities1 = results[1];
				vector<float>& actionUtilities2 = results[2];
				vector<float>& actionUtilities3 = results[3];

				actionNode->update_regretSum_part_one(actionUtilities0, 0);
				actionNode->update_regretSum_part_one(actionUtilities1, 1);
				actionNode->update_regretSum_part_one(actionUtilities2, 2);
				actionNode->update_regretSum_part_one(actionUtilities3, 3);

				float* __restrict strategy0 = &strategy[0];
				float* __restrict strategy1 = &strategy[numHeroHands];
				float* __restrict strategy2 = &strategy[numHeroHands + numHeroHands];
				float* __restrict strategy3 = &strategy[numHeroHands + numHeroHands + numHeroHands];

				for (int hand = 0; hand < numHeroHands; hand++)
					utilities[hand] =
						strategy0[hand] * actionUtilities0[hand]
						+ strategy1[hand] * actionUtilities1[hand]
						+ strategy2[hand] * actionUtilities2[hand]
						+ strategy3[hand] * actionUtilities3[hand];
			}
			break;
			default:
			{
				for (int action = 0; action < numActions; action++)
				{
					vector<float>& actionUtilities = results[action];

					actionNode->update_regretSum_part_one(actionUtilities, action);
					int index = action * numHeroHands;
					for (int hand = 0; hand < numHeroHands; hand++)
						utilities[hand] += strategy[index++] * actionUtilities[hand];
				}
			}
		}

		actionNode->update_regretSum_part_two(*result, iterationCount);
	}
	else
	{
		*result = vector<float>(numHeroHands);
		vector<vector<float>> results(numActions);
		vector<vector<float>> newVillainReachProbss(numActions);

		vector<float>& villainReachProbsRef = (*villainReachProbs);

		vector<float>& utilities = *result;

		vector<CfrTask*> cfrTasks = vector<CfrTask*>(numActions);

		vector<float> strategy = actionNode->get_current_strategy();

		set_ref_count(numActions + 1);

		for (int action = 0; action < numActions; action++)
		{
			newVillainReachProbss[action] = vector<float>(numVillainHands);
			vector<float>& newVillainReachProbs = newVillainReachProbss[action];

			int index = action * numVillainHands;
			for (int hand = 0; hand < numVillainHands; hand++)
				newVillainReachProbs[hand] = strategy[index++] * villainReachProbsRef[hand];

			cfrTasks[action] = new (allocate_child()) CfrTask(rangeManager, &results[action], actionNode->get_child(action), hero, villain, &newVillainReachProbs, board, iterationCount);
		}

		for (int action = 0; action < numActions-1; action++)
			spawn(*cfrTasks[action]);

		spawn_and_wait_for_all(*cfrTasks[numActions-1]);

		switch (numActions)
		{
			case 2:
			{
				vector<float>& subgameUtilities0 = results[0];
				vector<float>& subgameUtilities1 = results[1];

				for (int hand = 0; hand < numHeroHands; hand++)
					utilities[hand] = subgameUtilities0[hand] + subgameUtilities1[hand];
			}
			break;
			case 3:
			{
				vector<float>& subgameUtilities0 = results[0];
				vector<float>& subgameUtilities1 = results[1];
				vector<float>& subgameUtilities2 = results[2];

				for (int hand = 0; hand < numHeroHands; hand++)
					utilities[hand] = subgameUtilities0[hand] + subgameUtilities1[hand] + subgameUtilities2[hand];
			}
			break;
			case 4:
			{
				vector<float>& subgameUtilities0 = results[0];
				vector<float>& subgameUtilities1 = results[1];
				vector<float>& subgameUtilities2 = results[2];
				vector<float>& subgameUtilities3 = results[3];

				for (int hand = 0; hand < numHeroHands; hand++)
					utilities[hand] = subgameUtilities0[hand] + subgameUtilities1[hand] + subgameUtilities2[hand] + subgameUtilities3[hand];
			}
			break;
			default:
			{
				for (int action = 0; action < numActions; action++)
				{
					vector<float>& subgameUtilities = results[action];

					for (int hand = 0; hand < numHeroHands; hand++)
						utilities[hand] += subgameUtilities[hand];
				}
			}
		}

		actionNode->update_strategySum(strategy, *villainReachProbs, iterationCount);
	}

	return NULL;
}

vector<float> CfrTask::chance_node_utility(ChanceNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5], int iterationCount)
{
	vector<Hand>& heroHands = rangeManager->get_hands(hero, board);
	vector<Hand>& villainHands = rangeManager->get_hands(villain, board);

	vector<float> utilities = vector<float>(heroHands.size());

	vector<unique_ptr<ChanceNodeChild>>& children = node->get_children();

	int childCount = children.size();

	vector<vector<float>> results(childCount);
	vector<vector<float>> newVillainReachProbss(childCount);

	vector<CfrTask*> cfrTasks = vector<CfrTask*>(childCount);

	set_ref_count(childCount + 1);

	for (int i = 0; i < childCount; i++)
	{
		Node* child = children[i]->node.get();
		uint8_t card = children[i]->card;

		uint8_t* newBoard = new uint8_t[5];
		for (int j = 0; j < 5; j++)
			newBoard[j] = board[j];

		if (board[3] == 52)
			newBoard[3] = card;
		else
			newBoard[4] = card;

		newVillainReachProbss[i] = rangeManager->get_reach_probs(villain, newBoard, villainReachProbs);

		cfrTasks[i] = new (allocate_child()) CfrTask(rangeManager, &results[i], child, hero, villain, &newVillainReachProbss[i], newBoard, iterationCount);
	}

	for (int i = 0; i < childCount-1; i++)
		spawn(*cfrTasks[i]);

	spawn_and_wait_for_all(*cfrTasks[childCount-1]);
			

	uint8_t newBoard[5];
	for (int j = 0; j < 5; j++)
		newBoard[j] = board[j];

	for (int i = 0; i < childCount; i++)
	{
		Node* child = children[i]->node.get();
		uint8_t card = children[i]->card;

		if (board[3] == 52)
			newBoard[3] = card;
		else
			newBoard[4] = card;

		vector<float>& subgameUtilities = results[i];
		vector<int>& reachProbsMapping = rangeManager->get_reach_probs_mapping(hero, newBoard);

		for (int i = 0; i < subgameUtilities.size(); i++)
			utilities[reachProbsMapping[i]] += subgameUtilities[i];
	}

	if (board[3] == 52)
		for (int hand = 0; hand < utilities.size(); hand++)
			utilities[hand] /= 45;
	else
		for (int hand = 0; hand < utilities.size(); hand++)
			utilities[hand] /= 44;

	return utilities;
}

vector<float> CfrTask::terminal_node_utility(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5], int iterationCount)
{
	if (node->type == TerminalNodeType::ALLIN)
		return allin_utility(node, hero, villain, villainReachProbs, board, iterationCount);
	else if (node->type == TerminalNodeType::UNCONTESTED)
		return uncontested_utility(node, hero, villain, villainReachProbs, board, iterationCount);
	else
		return showdown_utility(node, hero, villain, villainReachProbs, board, iterationCount);
}

vector<float> CfrTask::allin_utility(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5], int iterationCount)
{
	vector<Hand>& heroHands = rangeManager->get_hands(hero, board);
	vector<Hand>& villainHands = rangeManager->get_hands(villain, board);

	vector<float> evs = vector<float>(heroHands.size());

	uint8_t newBoard[5];
	for (int j = 0; j < 5; j++)
		newBoard[j] = board[j];

	//if (newBoard[3] != 52)
	//{
	//	int count = 0;
	//	uint8_t flushSuit = get_flush_suit(board);
	//	for (uint8_t rank = 2; rank <= 14; rank++)
	//	{
	//		for (uint8_t suit = 0; suit < 4; suit++)
	//		{
	//			uint8_t card = card_from_rank_and_suit2(rank, suit);
	//			if (overlap(card, board))
	//				continue;
	//			if (suit == flushSuit)
	//				count++;
	//		}
	//		count++;
	//	}

	//	std::cout << count << '\n';

	//	vector<vector<float>> results(count);
	//	vector<ShowdownTask*> showdownTasks = vector<ShowdownTask*>(count);
	//	vector<int> weights(count);
	//	vector<uint8_t> cards(count);

	//	int index = 0;
	//	for (uint8_t rank = 2; rank <= 14; rank++)
	//	{
	//		int canonicalWeight = 0;
	//		uint8_t canoncialCard;
	//		uint8_t flushCard = 52;

	//		for (uint8_t suit = 0; suit < 4; suit++)
	//		{
	//			uint8_t card = card_from_rank_and_suit2(rank, suit);
	//			if (overlap(card, board))
	//				continue;
	//			if (suit == flushSuit)
	//				flushCard = card;
	//			else
	//			{
	//				canoncialCard = card;
	//				canonicalWeight++;
	//			}
	//		}

	//		newBoard[4] = canoncialCard;
	//		showdownTasks[index] = new (allocate_child()) ShowdownTask(rangeManager, &results[index], node, hero, villain, rangeManager->get_reach_probs(villain, newBoard, villainReachProbs), newBoard);
	//		weights[index] = canonicalWeight;
	//		cards[index] = canoncialCard;
	//		newBoard[4] = 52;
	//		index++;

	//		if (flushCard != 52)
	//		{
	//			newBoard[4] = flushCard;
	//			showdownTasks[index] = new (allocate_child()) ShowdownTask(rangeManager, &results[index], node, hero, villain, rangeManager->get_reach_probs(villain, newBoard, villainReachProbs), newBoard);
	//			weights[index] = 1;
	//			cards[index] = flushCard;
	//			newBoard[4] = 52;
	//			index++;
	//		}
	//	}

	//	set_ref_count(count+1);

	//	for (int i = 0; i < count-1; i++)
	//		spawn(*showdownTasks[i]);

	//	spawn_and_wait_for_all(*showdownTasks[count-1]);

	//	for (int i = 0; i < count; i++)
	//	{
	//		newBoard[4] = cards[i];

	//		vector<Hand>& heroHands = rangeManager->get_hands(hero, newBoard);

	//		vector<float>& subgameEvs = results[i];

	//		for (int j = 0; j < subgameEvs.size(); j++)
	//			evs[heroHands[j].reachProbsIndex] += subgameEvs[j] * weights[i];
	//	}

	//	for (int i = 0; i < evs.size(); i++)
	//		evs[i] /= 44;
	//}
	if (newBoard[3] != 52)
	{
		set_ref_count(49);
		vector<vector<float>> results(48);
		vector<ShowdownTask*> showdownTasks = vector<ShowdownTask*>(48);
		//task_group g;

		int index = 0;
		for (uint8_t river = 0; river < 52; river++)
		{
			if (overlap(river, board))
				continue;

			newBoard[4] = river;

			//g.run([&results, index, &node, hero, villain, newVillainReachProbs, newBoard, iterationCount, this] {results[index] = showdown_utility(node, hero, villain, newVillainReachProbs, newBoard, iterationCount); });
			showdownTasks[index] = new (allocate_child()) ShowdownTask(rangeManager, &results[index], node, hero, villain, rangeManager->get_reach_probs(villain, newBoard, villainReachProbs), newBoard);
			index++;
		}

		//g.wait();

		for (int i = 0; i < index-1; i++)
			spawn(*showdownTasks[i]);

		spawn_and_wait_for_all(*showdownTasks[index-1]);

		index = 0;
		for (uint8_t river = 0; river < 52; river++)
		{
			if (overlap(river, board))
				continue;

			newBoard[4] = river;

			vector<int>& reachProbsMapping = rangeManager->get_reach_probs_mapping(hero, newBoard);

			vector<float>& subgameEvs = results[index];

			for (int i = 0; i < subgameEvs.size(); i++)
				evs[reachProbsMapping[i]] += subgameEvs[i];

			index++;
		}

		for (int i = 0; i < evs.size(); i++)
			evs[i] /= 44;
	}
	else
	{
		set_ref_count(49*48/2 + 1);
		vector<vector<float>> results(49*48/2);
		vector<ShowdownTask*> showdownTasks = vector<ShowdownTask*>(49*48/2);

		int index = 0;
		for (uint8_t turn = 0; turn < 52; turn++)
		{
			if (overlap(turn, board))
				continue;

			newBoard[3] = turn;

			vector<Hand>& heroTurnHands = rangeManager->get_hands(hero, newBoard);
			vector<float> newVillainReachProbs = rangeManager->get_reach_probs(villain, newBoard, villainReachProbs);

			for (uint8_t river = turn+1; river < 52; river++)
			{
				if (overlap(river, board))
					continue;

				newBoard[4] = river;

				showdownTasks[index] = new (allocate_child()) ShowdownTask(rangeManager, &results[index], node, hero, villain, rangeManager->get_reach_probs(villain, newBoard, newVillainReachProbs), newBoard);
				index++;
			}

			newBoard[4] = 52;
			newBoard[3] = 52;
		}
		

		for (int i = 0; i < index-1; i++)
			spawn(*showdownTasks[i]);
		
		spawn_and_wait_for_all(*showdownTasks[index-1]);

		index = 0;
		for (uint8_t turn = 0; turn < 52; turn++)
		{
			if (overlap(turn, board))
				continue;

			newBoard[3] = turn;

			vector<int>& reachProbsMapping = rangeManager->get_reach_probs_mapping(hero, newBoard);
			vector<float> turnEvs = vector<float>(reachProbsMapping.size());

			for (uint8_t river = turn + 1; river < 52; river++)
			{
				if (overlap(river, board))
					continue;

				newBoard[4] = river;

				vector<int>& reachProbsMapping = rangeManager->get_reach_probs_mapping(hero, newBoard);

				vector<float>& subgameEvs = results[index];
				for (int i = 0; i < subgameEvs.size(); i++)
					turnEvs[reachProbsMapping[i]] += subgameEvs[i];

				index++;
			}

			for (int i = 0; i < turnEvs.size(); i++)
				evs[reachProbsMapping[i]] += 2 * turnEvs[i];

			newBoard[4] = 52;
			newBoard[3] = 52;
		}

		for (int i = 0; i < evs.size(); i++)
			evs[i] /= 1980;
	}

	return evs;
}

vector<float> CfrTask::showdown_utility(TerminalNode* node, const int hero, const int villain, const vector<float>& villainReachProbs, uint8_t board[5], const int iterationCount)
{
	vector<Hand>& heroHands = rangeManager->get_hands(hero, board);
	vector<Hand>& villainHands = rangeManager->get_hands(villain, board);

	int numHeroHands = heroHands.size();
	int numVillainHands = villainHands.size();

	vector<float> utilities(heroHands.size());

	float value = node->value;

	float sum = 0;
	float cardSum[52];
	memset(cardSum, 0, sizeof(cardSum));

	for (int i = 0; i < numVillainHands; i++)
	{
		cardSum[villainHands[i].card1] -= villainReachProbs[i];
		cardSum[villainHands[i].card2] -= villainReachProbs[i];
		sum -= villainReachProbs[i];
	}

	int k, m;
	int j = 0;
	int i = 0;

	while (i < numHeroHands)
	{
		while (j < numVillainHands && heroHands[i].rank > villainHands[j].rank)
		{
			cardSum[villainHands[j].card1] += villainReachProbs[j];
			cardSum[villainHands[j].card2] += villainReachProbs[j];
			sum += villainReachProbs[j];

			++j;
		}
		k = j;

		while (j < numVillainHands && heroHands[i].rank == villainHands[j].rank)
		{
			cardSum[villainHands[j].card1] += villainReachProbs[j];
			cardSum[villainHands[j].card2] += villainReachProbs[j];
			sum += villainReachProbs[j];

			++j;
		}

		utilities[i] = (sum
			- cardSum[heroHands[i].card1]
			- cardSum[heroHands[i].card2])
			* value;

		for (m = i+1; m < numHeroHands && heroHands[m].rank == heroHands[i].rank; ++m)
			utilities[m] = (sum
				- cardSum[heroHands[m].card1]
				- cardSum[heroHands[m].card2])
			* value;

		while (k < j)
		{
			cardSum[villainHands[k].card1] += villainReachProbs[k];
			cardSum[villainHands[k].card2] += villainReachProbs[k];
			sum += villainReachProbs[k];

			++k;
		}

		i = m;
	}

	return utilities;
}

vector<float> CfrTask::uncontested_utility(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5], int iterationCount)
{
	vector<Hand>& heroHands = rangeManager->get_hands(hero, board);
	vector<Hand>& villainHands = rangeManager->get_hands(villain, board);

	int numHeroHands = heroHands.size();
	int numVillainHands = villainHands.size();

	float villainSum = 0;
	float villainCardSum[52];
	memset(villainCardSum, 0, sizeof(villainCardSum));

	for (int i = 0; i < numVillainHands; i++)
	{
		villainCardSum[villainHands[i].card1] += villainReachProbs[i];
		villainCardSum[villainHands[i].card2] += villainReachProbs[i];
		villainSum += villainReachProbs[i];
	}

	float value = (hero == node->lastToAct) ? -node->value : node->value;

	vector<float> utilities = vector<float>(numHeroHands);

	for (int i = 0; i < numHeroHands; i++)
	{
		utilities[i] = (villainSum
			- villainCardSum[heroHands[i].card1]
			- villainCardSum[heroHands[i].card2]
			+ villainReachProbs[i])
			* value;
	}

	return utilities;
}