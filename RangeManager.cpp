#include "RangeManager.h"
#include <algorithm>
#include <sstream>
#include <iostream>
using std::cout;
using std::istringstream;
using std::stof;
using std::move;
using std::sort;

RangeManager::RangeManager(string p1StartingHands, string p2StartingHands, uint8_t initialBoard[5])
{
    handEvaluator = HandEvaluator::get_instance();
	initialize_starting_range(1, p1StartingHands, initialBoard);
	initialize_starting_range(2, p2StartingHands, initialBoard);
	initialize_ranges(1, initialBoard);
	initialize_ranges(2, initialBoard);
	free(handEvaluator);
	initialize_reach_probs_mapping(1, initialBoard);
	initialize_reach_probs_mapping(2, initialBoard);
}

void RangeManager::initialize_starting_range(int player, string startingHands, uint8_t initialBoard[5])
{
	istringstream ss(startingHands);
	string token;

	vector<Hand> hands;

	while (getline(ss, token, ','))
	{
		char firstCardRank = token.at(0);
		char secondCardRank = token.at(1);
		float weight = 1.0f;

		if (token.length() == 2)
		{
			for (int suit1 = 0; suit1 < 4; suit1++)
			{
				for (int suit2 = 0; suit2 < 4; suit2++)
				{
					if (firstCardRank == secondCardRank && suit1 >= suit2)
						continue;

					int card1 = card_from_rank_and_suit(firstCardRank, suit1);
					int card2 = card_from_rank_and_suit(secondCardRank, suit2);

					if (overlap(card1, card2, initialBoard))
						continue;

					Hand hand = Hand(card1, card2);
					hand.probability = weight;

					hands.push_back(hand);
				}
			}
		}
		else if (token.at(2) == ':')
		{
			weight = stof(token.substr(3, token.length()));

			for (int suit1 = 0; suit1 < 4; suit1++)
			{
				for (int suit2 = suit1 + 1; suit2 < 4; suit2++)
				{
					int card1 = card_from_rank_and_suit(firstCardRank, suit1);
					int card2 = card_from_rank_and_suit(secondCardRank, suit2);

					if (overlap(card1, card2, initialBoard))
						continue;

					Hand hand = Hand(card1, card2);
					hand.probability = weight;
						
					hands.push_back(hand);
				}
			}
		}
		else if (token.at(2) == 's')
		{
			if (token.length() > 4)
				weight = stof(token.substr(4, token.length()));

			for (int suit = 0; suit < 4; suit++)
			{
				int card1 = card_from_rank_and_suit(firstCardRank, suit);
				int card2 = card_from_rank_and_suit(secondCardRank, suit);

				if (overlap(card1, card2, initialBoard))
					continue;

				Hand hand = Hand(card1, card2);
				hand.probability = weight;

				hands.push_back(hand);
			}
		}
		else if (token.at(2) == 'o')
		{
			if (token.length() > 4)
				weight = stof(token.substr(4, token.length()));

			for (int suit1 = 0; suit1 < 4; suit1++)
			{
				for (int suit2 = 0; suit2 < 4; suit2++)
				{
					if (suit1 == suit2)
						continue;

					int card1 = card_from_rank_and_suit(firstCardRank, suit1);
					int card2 = card_from_rank_and_suit(secondCardRank, suit2);

					if (overlap(card1, card2, initialBoard))
						continue;

					Hand hand = Hand(card1, card2);
					hand.probability = weight;

					hands.push_back(hand);
				}
			}
		}
	}

	if (player == 1)
	{
		p1StartingHands = hands;
		p1Ranges[get_key(initialBoard)] = hands;
	}
	else
	{
		p2StartingHands = hands;
		p2Ranges[get_key(initialBoard)] = hands;
	}
}

void RangeManager::initialize_reach_probs_mapping(int player, uint8_t initialBoard[5])
{
	vector<Hand>& startingHands = get_starting_hands(player);
	unordered_map<int, vector<Hand>>& ranges = get_ranges(player);
	unordered_map<int, vector<int>>& reachProbsMapping = get_reach_probs_mappings(player);

	if (!board_has_turn(initialBoard))
	{
		for (uint8_t turn = 0; turn < 52; turn++)
		{
			if (overlap(turn, initialBoard))
				continue;
			
			uint8_t board[5] = { initialBoard[0], initialBoard[1], initialBoard[2], turn, 52 };
			int key = get_key(board);

			vector<Hand>& turnHands = ranges[key];
			vector<int> reachProbs = vector<int>(turnHands.size());

			int j = 0;
			for (int i = 0; i < turnHands.size(); i++)
			{
				while (!equals(turnHands[i], startingHands[j]))
					j++;

				reachProbs[i] = j;
			}

			reachProbsMapping[key] = reachProbs;
			
			for (uint8_t river = 0; river < 52; river++)
			{
				if (turn == river || overlap(river, initialBoard))
					continue;

				uint8_t board[5] = { initialBoard[0], initialBoard[1], initialBoard[2], turn, river };
				int key = get_key(board);

				vector<Hand>& riverHands = ranges[key];
				vector<int> reachProbs = vector<int>(riverHands.size());

				int j = 0;
				for (int i = 0; i < riverHands.size(); i++)
				{
					while (!equals(riverHands[i], turnHands[j]))
						j++;

					reachProbs[i] = j;
				}

				quickSort(riverHands, reachProbs, 0, riverHands.size()-1);

				reachProbsMapping[key] = reachProbs;
			}
		}
	}
	else if (!board_has_river(initialBoard))
	{
		for (uint8_t river = 0; river < 52; river++)
		{
			if (overlap(river, initialBoard))
				continue;

			uint8_t board[5] = { initialBoard[0], initialBoard[1], initialBoard[2], initialBoard[3], river };
			int key = get_key(board);

			vector<Hand>& riverHands = ranges[key];
			vector<int> reachProbs = vector<int>(riverHands.size());

			int j = 0;
			for (int i = 0; i < riverHands.size(); i++)
			{
				while (!equals(riverHands[i], startingHands[j]))
					j++;

				reachProbs[i] = j;
			}

			quickSort(riverHands, reachProbs, 0, riverHands.size()-1);

			reachProbsMapping[key] = reachProbs;
		}
	}
}

void RangeManager::initialize_ranges(int player, uint8_t initialBoard[5])
{
	vector<Hand>& startingHands = get_starting_hands(player);
	unordered_map<int, vector<Hand>>& ranges = get_ranges(player);

	if (!board_has_turn(initialBoard))
	{
		for (uint8_t turn = 0; turn < 52; turn++)
		{
			if (overlap(turn, initialBoard))
				continue;

			uint8_t board[5] = { initialBoard[0], initialBoard[1], initialBoard[2], turn, 52 };
			int key = get_key(board);
			vector<Hand> turnHands;

			for (Hand hand : startingHands)
			{
				if (overlap(hand, turn))
					continue;

				Hand newHand = Hand(hand.card1, hand.card2);
				newHand.probability = hand.probability;
				turnHands.push_back(newHand);
			}
			ranges[key] = turnHands;

			for (uint8_t river = turn+1; river < 52; river++)
			{
				if (overlap(river, initialBoard))
					continue;

				uint8_t board[5] = { initialBoard[0], initialBoard[1], initialBoard[2], turn, river };
				int key = get_key(board);
				vector<Hand> hands;

				for (Hand hand : turnHands)
				{
					if (overlap(hand, river))
						continue;

					Hand newHand = Hand(hand.card1, hand.card2);
					newHand.probability = hand.probability;
					newHand.rank = handEvaluator->get_hand_rank(newHand.card1, newHand.card2, board);
					hands.push_back(newHand);
				}

				ranges[key] = hands;

				uint8_t symmetricalBoard[5] = { initialBoard[0], initialBoard[1], initialBoard[2], river, turn };
				int symmetricalKey = get_key(symmetricalBoard);
				ranges[symmetricalKey] = hands;
			}
		}
	}
	else if (!board_has_river(initialBoard))
	{
		for (uint8_t river = 0; river < 52; river++)
		{
			if (overlap(river, initialBoard))
				continue;

			uint8_t board[5] = { initialBoard[0], initialBoard[1], initialBoard[2], initialBoard[3], river };
			int key = get_key(board);
			vector<Hand> hands;

			for (Hand hand : startingHands)
			{
				if (overlap(hand, river))
					continue;

				Hand newHand = Hand(hand.card1, hand.card2);
				newHand.probability = hand.probability;
				newHand.rank = handEvaluator->get_hand_rank(newHand.card1, newHand.card2, board);
				hands.push_back(newHand);
			}

			ranges[key] = hands;
		}
	}
}

void RangeManager::quickSort(vector<Hand>& hands, vector<int>& handMap, int low, int high)
{
	if (low < high)
	{
		int pi = partition(hands, handMap, low, high);
		quickSort(hands, handMap, low, pi - 1);
		quickSort(hands, handMap, pi + 1, high);
	}
}

int RangeManager::partition(vector<Hand>& hands, vector<int>& handMap, int low, int high)
{
	int pivot = hands[high].rank;
	int i = low - 1;
	for (int j = low; j <= high - 1; j++)
	{
		if (hands[j].rank <= pivot)
		{
			i++;
			Hand handTemp = hands[i];
			hands[i] = hands[j];
			hands[j] = handTemp;

			int intTemp = handMap[i];
			handMap[i] = handMap[j];
			handMap[j] = intTemp;
		}
	}
	Hand handTemp = hands[i+1];
	hands[i+1] = hands[high];
	hands[high] = handTemp;

	int intTemp = handMap[i+1];
	handMap[i+1] = handMap[high];
	handMap[high] = intTemp;

	return i+1;
}

bool RangeManager::compare_hands(Hand h1, Hand h2)
{
    return h1.rank < h2.rank;
}

int RangeManager::get_num_hands(int player, uint8_t board[5])
{
	vector<Hand>& hands = get_hands(player, board);
	return hands.size();
}

vector<Hand>& RangeManager::get_starting_hands(int player)
{
	return (player == 1) ? p1StartingHands : p2StartingHands;
}

unordered_map<int, vector<int>>& RangeManager::get_reach_probs_mappings(int player)
{
	return (player == 1) ? p1ReachProbsMapping : p2ReachProbsMapping;
}

unordered_map<int, vector<Hand>>& RangeManager::get_ranges(int player)
{
	return (player == 1) ? p1Ranges : p2Ranges;
}

vector<Hand>& RangeManager::get_hands(int player, uint8_t board[5])
{
	unordered_map<int, vector<Hand>>& ranges = get_ranges(player);
	return ranges[get_key(board)];
}

vector<int>& RangeManager::get_reach_probs_mapping(int player, uint8_t board[5])
{
	unordered_map<int, vector<int>>& reachProbsMappings = get_reach_probs_mappings(player);
	return reachProbsMappings[get_key(board)];
}

vector<float> RangeManager::get_reach_probs(int player, uint8_t board[5], vector<float>& reachProbs)
{
	vector<int>& reachProbsMapping = get_reach_probs_mapping(player, board);

	vector<float> newReachProbs(reachProbsMapping.size());

	for (int i = 0; i < reachProbsMapping.size(); i++)
		newReachProbs[i] = reachProbs[reachProbsMapping[i]];

	return newReachProbs;
}

vector<float> RangeManager::get_initial_reach_probs(int player)
{
	vector<Hand>& startingHands = get_starting_hands(player);

	vector<float> reachProbs(startingHands.size());

	for (int hand = 0; hand < startingHands.size(); hand++)
		reachProbs[hand] = startingHands[hand].probability;

	return reachProbs;
}