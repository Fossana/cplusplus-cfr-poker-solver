#include "BestResponse.h"
#include "card_utility.h"
#include <iostream>
#include <cstring>
#include "BestResponseTask.h"
#include "ChanceNodeTypeEnum.h"
#include "Hand.h"
#include "TerminalNodeTypeEnum.h"
using std::cout;
using std::make_unique;
using std::memset;

BestResponse::BestResponse(shared_ptr<RangeManager> rangeManager, Node* root, uint8_t initialBoard[5], int initialPot, int inPositionPlayer)
{
	this->rangeManager = rangeManager;
	this->root = root;
	for (int i = 0; i < 5; i++)
		this->initialBoard[i] = initialBoard[i];
	this->initialPot = initialPot;
	this->inPositionPlayer = inPositionPlayer;
	set_relative_probabilities(initialBoard);
}

float BestResponse::get_best_response_Ev(int hero, int villain)
{
    float totalEv = 0;

	vector<Hand>& heroHands = rangeManager->get_starting_hands(hero);
	vector<Hand>& villainHands = rangeManager->get_starting_hands(villain);

	vector<float>& relativeProbs = (hero == 1) ? p1RelativeProbs : p2RelativeProbs;
	vector<float> villainReachProbs = rangeManager->get_initial_reach_probs(villain);

	vector<float> result;
	BestResponseTask& cfrTask = *new (task::allocate_root()) BestResponseTask(rangeManager, &result, root, hero, villain, &villainReachProbs, initialBoard);
	task::spawn_root_and_wait(cfrTask);
	vector<float>& evs = result;

	for (int i = 0; i < heroHands.size(); i++)
	{
		totalEv += evs[i] / get_unblocked_combo_count(heroHands[i], villainHands) * relativeProbs[i];
		//cout << heroHands[i].card1 << heroHands[i].card2 << " ev: " << evs[i] << " unblocked combo count: " << get_unblocked_combo_count(heroHands[i], villainHands) << " relative prob: " << heroHands[i].relativeProbability << "\n";
	}
		
		
    return totalEv;
}

float BestResponse::get_unblocked_combo_count(Hand& heroHand, vector<Hand>& villainHands)
{
    float sum = 0;

    for (int i = 0; i < villainHands.size(); i++)
        if (!overlap(villainHands[i], heroHand))
            sum += villainHands[i].probability;

    return sum;
}

void BestResponse::set_relative_probabilities(uint8_t initialBoard[5])
{
	for (int player = 1; player <= 2; player++)
	{
		vector<float>& relativeProbs = (player == 1) ? p1RelativeProbs : p2RelativeProbs;
		vector<Hand>& heroStartingHands = rangeManager->get_starting_hands(player);
		vector<Hand>& villainStartingHands = rangeManager->get_starting_hands(player ^ 1 ^ 2);
		relativeProbs.resize(heroStartingHands.size());

		float relativeSum = 0;

		for (int i = 0; i < heroStartingHands.size(); i++)
		{
			Hand& heroHand = heroStartingHands[i];

			float villainSum = 0;

			for (int j = 0; j < villainStartingHands.size(); j++)
			{
				Hand& villainHand = villainStartingHands[j];

				if (overlap(heroHand, villainHand))
					continue;

				villainSum += villainHand.probability;
			}

			relativeProbs[i] = villainSum * heroHand.probability;
			relativeSum += relativeProbs[i];
		}

		for (int i = 0; i < relativeProbs.size(); i++)
			relativeProbs[i] /= relativeSum;
	}
}

void BestResponse::print_exploitability()
{
    float oopEv = 0;
    float ipEv = 0;

    if (inPositionPlayer == 1)
    {
        ipEv = get_best_response_Ev(1, 2);
        oopEv = get_best_response_Ev(2, 1);
    }
    else
    {
        ipEv = get_best_response_Ev(2, 1);
        oopEv = get_best_response_Ev(1, 2);
    }

    float exploitability = (oopEv + ipEv) / 2 / initialPot * 100;

    cout << "OOP BR EV: " << oopEv << "\n";
    cout << "IP BR EV: " << ipEv << "\n";
    cout << "Exploitability: " << exploitability << "%%\n";
}



