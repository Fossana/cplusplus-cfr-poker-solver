#include "ShowdownTask.h"

task* ShowdownTask::execute()
{
	vector<Hand>& heroHands = rangeManager->get_hands(hero, board);
	vector<Hand>& villainHands = rangeManager->get_hands(villain, board);

	int numHeroHands = heroHands.size();
	*result = vector<float>(numHeroHands);
	int numVillainHands = villainHands.size();

	vector<float>& utilities = *result;

	float value = node->value;

	float sum = 0;
	float cardSum[52];
	memset(cardSum, 0, sizeof(cardSum));

	for (int i = 0; i < numVillainHands; i++)
	{
		sum -= villainReachProbs[i];
		cardSum[villainHands[i].card1] -= villainReachProbs[i];
		cardSum[villainHands[i].card2] -= villainReachProbs[i];
	}

	int k, m;
	int j = 0;
	int i = 0;

	while (i < numHeroHands)
	{
		while (j < numVillainHands && heroHands[i].rank > villainHands[j].rank)
		{
			sum += villainReachProbs[j];
			cardSum[villainHands[j].card1] += villainReachProbs[j];
			cardSum[villainHands[j].card2] += villainReachProbs[j];

			++j;
		}
		k = j;

		while (j < numVillainHands && heroHands[i].rank == villainHands[j].rank)
		{
			sum += villainReachProbs[j];
			cardSum[villainHands[j].card1] += villainReachProbs[j];
			cardSum[villainHands[j].card2] += villainReachProbs[j];

			++j;
		}

		utilities[i] = (sum
			- cardSum[heroHands[i].card1]
			- cardSum[heroHands[i].card2])
			* value;

		for (m = i + 1; m < numHeroHands && heroHands[m].rank == heroHands[i].rank; ++m)
			utilities[m] = (sum
				- cardSum[heroHands[m].card1]
				- cardSum[heroHands[m].card2])
			* value;

		while (k < j)
		{
			sum += villainReachProbs[k];
			cardSum[villainHands[k].card1] += villainReachProbs[k];
			cardSum[villainHands[k].card2] += villainReachProbs[k];

			++k;
		}

		i = m;
	}

	return NULL;
}

ShowdownTask::ShowdownTask(shared_ptr<RangeManager> rangeManager, vector<float>* result, TerminalNode* node, int hero, int villain, vector<float> villainReachProbs, uint8_t board[5])
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