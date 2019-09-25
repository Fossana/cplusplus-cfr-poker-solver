#include "BestResponseTask.h"
#include "card_utility.h"
#include <iostream>
#include <cstring>
#include "ChanceNodeTypeEnum.h"
#include "Hand.h"
#include "TerminalNodeTypeEnum.h"
#include "ShowdownTask.h"
#include <limits>
#include "NodeTypeEnum.h"
using std::cout;
using std::make_unique;
using std::memset;

BestResponseTask::BestResponseTask(shared_ptr<RangeManager> rangeManager, vector<float>* result, Node* node, int hero, int villain, vector<float>* villainReachProbs, uint8_t board[5])
{
	this->rangeManager = rangeManager;
	this->result = result;
	this->node = node;
	this->hero = hero;
	this->villain = villain;
	this->villainReachProbs = villainReachProbs;
	for (int i = 0; i < 5; i++)
		this->board[i] = board[i];
}

task* BestResponseTask::execute()
{
	if (typeid(*node) == typeid(TerminalNode))
	{
		*result = terminal_node_best_response(static_cast<TerminalNode*>(node), hero, villain, *villainReachProbs, board);
		return NULL;
	}

	if (typeid(*node) == typeid(ChanceNode))
	{
		*result = chance_node_best_response(static_cast<ChanceNode*>(node), hero, villain, *villainReachProbs, board);
		return NULL;
	}

	//action node
	ActionNode* actionNode = static_cast<ActionNode*>(node);

	int numHeroHands = rangeManager->get_num_hands(hero, board);
	int numVillainHands = rangeManager->get_num_hands(villain, board);
	int numActions = actionNode->numActions;

	if (hero == actionNode->player)
	{
		*result = vector<float>(numHeroHands, -std::numeric_limits<float>::max());
		vector<vector<float>> results(numActions);

		vector<BestResponseTask*> cfrTasks = vector<BestResponseTask*>(numActions);

		vector<float>& maxSubgameEvs = *result;

		set_ref_count(numActions + 1);

		for (int action = 0; action < numActions; action++)
			cfrTasks[action] = new (allocate_child()) BestResponseTask(rangeManager, &results[action], actionNode->get_child(action), hero, villain, villainReachProbs, board);

		for (int action = 0; action < numActions; action++)
			if (action == numActions - 1)
				spawn_and_wait_for_all(*cfrTasks[action]);
			else
				spawn(*cfrTasks[action]);

		for (int action = 0; action < numActions; action++)
		{
			vector<float>& subgameEvs = results[action];

			for (int hand = 0; hand < numHeroHands; hand++)
				if (subgameEvs[hand] > maxSubgameEvs[hand])
					maxSubgameEvs[hand] = subgameEvs[hand];
		}
	}
	else
	{
		vector<float> avgStrategy = actionNode->get_average_strategy();

		*result = vector<float>(numHeroHands);
		vector<vector<float>> results(numActions);
		vector<vector<float>> newVillainReachProbss(numActions);

		vector<BestResponseTask*> cfrTasks = vector<BestResponseTask*>(numActions);

		vector<float>& cumSubgameEvs = *result;

		set_ref_count(numActions + 1);

		for (int action = 0; action < numActions; action++)
		{
			newVillainReachProbss[action] = vector<float>(numVillainHands);
			vector<float>& newVillainReachProbs = newVillainReachProbss[action];

			int index = action * numVillainHands;
			for (int hand = 0; hand < numVillainHands; hand++)
				newVillainReachProbs[hand] = avgStrategy[index++] * (*villainReachProbs)[hand];

			cfrTasks[action] = new (allocate_child()) BestResponseTask(rangeManager, &results[action], actionNode->get_child(action), hero, villain, &newVillainReachProbs, board);
		}


		for (int action = 0; action < numActions; action++)
			if (action == numActions - 1)
				spawn_and_wait_for_all(*cfrTasks[action]);
			else
				spawn(*cfrTasks[action]);

		for (int action = 0; action < numActions; action++)
		{
			vector<float>& subgameEvs = results[action];

			for (int hand = 0; hand < numHeroHands; hand++)
				cumSubgameEvs[hand] += subgameEvs[hand];
		}
	}

	return NULL;
}

vector<float> BestResponseTask::chance_node_best_response(ChanceNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5])
{
	vector<Hand>& heroHands = rangeManager->get_hands(hero, board);
	vector<Hand>& villainHands = rangeManager->get_hands(villain, board);

	vector<float> utilities = vector<float>(heroHands.size());

	vector<unique_ptr<ChanceNodeChild>>& children = node->get_children();

	int childCount = children.size();

	vector<vector<float>> results(childCount);
	vector<vector<float>> newVillainReachProbss(childCount);

	vector<BestResponseTask*> cfrTasks = vector<BestResponseTask*>(childCount);

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

		cfrTasks[i] = new (allocate_child()) BestResponseTask(rangeManager, &results[i], child, hero, villain, &newVillainReachProbss[i], newBoard);
	}

	for (int i = 0; i < childCount; i++)
		if (i == childCount - 1)
			spawn_and_wait_for_all(*cfrTasks[i]);
		else
			spawn(*cfrTasks[i]);

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

	int weight = (board[3] == 52) ? 45 : 44;
	for (int hand = 0; hand < utilities.size(); hand++)
		utilities[hand] /= weight;

	return utilities;
}

vector<float> BestResponseTask::terminal_node_best_response(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5])
{
	if (node->type == TerminalNodeType::ALLIN)
		return allin_best_response(node, hero, villain, villainReachProbs, board);
	else if (node->type == TerminalNodeType::UNCONTESTED)
		return uncontested_best_response(node, hero, villain, villainReachProbs, board);
	else
		return showdown_best_response(node, hero, villain, villainReachProbs, board);
}

vector<float> BestResponseTask::showdown_best_response(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5])
{
	vector<Hand>& heroHands = rangeManager->get_hands(hero, board);
	vector<Hand>& villainHands = rangeManager->get_hands(villain, board);

	int numHeroHands = heroHands.size();
	int numVillainHands = villainHands.size();

	vector<float> evs = vector<float>(numHeroHands, 0);

	float value = node->value;

	float winSum = 0;
	float cardWinSum[52];
	memset(cardWinSum, 0, sizeof(cardWinSum));

	int k;

	int j = 0;
	for (int i = 0; i < numHeroHands;)
	{
		while (heroHands[i].rank > villainHands[j].rank)
		{
			winSum += villainReachProbs[j];

			cardWinSum[villainHands[j].card1] += villainReachProbs[j];
			cardWinSum[villainHands[j].card2] += villainReachProbs[j];

			j++;
		}

		for (k = i; (k < numHeroHands) && (heroHands[k].rank == heroHands[i].rank); k++)
			evs[k] = (winSum
				- cardWinSum[heroHands[k].card1]
				- cardWinSum[heroHands[k].card2])
			* value;
		i = k;
	}

	float loseSum = 0;
	float cardLoseSum[52];
	memset(cardLoseSum, 0, sizeof(cardLoseSum));

	j = numVillainHands - 1;
	for (int i = numHeroHands - 1; i >= 0;)
	{
		while (heroHands[i].rank < villainHands[j].rank)
		{
			loseSum += villainReachProbs[j];

			cardLoseSum[villainHands[j].card1] += villainReachProbs[j];
			cardLoseSum[villainHands[j].card2] += villainReachProbs[j];

			j--;
		}

		for (k = i; (k >= 0) && (heroHands[k].rank == heroHands[i].rank); k--)
			evs[k] -= (loseSum
				- cardLoseSum[heroHands[k].card1]
				- cardLoseSum[heroHands[k].card2])
			* value;
		i = k;
	}

	return evs;
}

vector<float> BestResponseTask::allin_best_response(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5])
{
	vector<Hand>& heroHands = rangeManager->get_hands(hero, board);
	vector<Hand>& villainHands = rangeManager->get_hands(villain, board);

	vector<float> evs = vector<float>(heroHands.size());

	uint8_t newBoard[5];
	for (int j = 0; j < 5; j++)
		newBoard[j] = board[j];

	if (board_has_turn(board))
	{
		set_ref_count(49);
		vector<vector<float>> results(48);
		vector<ShowdownTask*> showdownTasks = vector<ShowdownTask*>(48);

		int index = 0;
		for (uint8_t river = 0; river < 52; river++)
		{
			if (overlap(river, board))
				continue;

			newBoard[4] = river;

			showdownTasks[index] = new (allocate_child()) ShowdownTask(rangeManager, &results[index], node, hero, villain, rangeManager->get_reach_probs(villain, newBoard, villainReachProbs), newBoard);
			index++;
		}

		for (int i = 0; i < index; i++)
			if (i == index - 1)
				spawn_and_wait_for_all(*showdownTasks[i]);
			else
				spawn(*showdownTasks[i]);

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
		set_ref_count(49 * 48 / 2 + 1);
		vector<vector<float>> results(49 * 48 / 2);
		vector<ShowdownTask*> showdownTasks = vector<ShowdownTask*>(49 * 48 / 2);

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


		for (int i = 0; i < index; i++)
			if (i == index - 1)
				spawn_and_wait_for_all(*showdownTasks[i]);
			else
				spawn(*showdownTasks[i]);

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

			newBoard[3] = 52;
			newBoard[4] = 52;
		}

		for (int i = 0; i < evs.size(); i++)
			evs[i] /= 1980;
	}

	return evs;
}

vector<float> BestResponseTask::uncontested_best_response(TerminalNode* node, int hero, int villain, vector<float>& villainReachProbs, uint8_t board[5])
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
		villainSum += villainReachProbs[i];
		villainCardSum[villainHands[i].card1] += villainReachProbs[i];
		villainCardSum[villainHands[i].card2] += villainReachProbs[i];
	}

	float value = (hero == node->lastToAct) ? -node->value : node->value;

	vector<float> evs = vector<float>(numHeroHands);

	for (int i = 0; i < numHeroHands; i++)
	{
		evs[i] = (villainSum
			- villainCardSum[heroHands[i].card1]
			- villainCardSum[heroHands[i].card2]
			+ villainReachProbs[i])
			* value;
	}

	return evs;
}