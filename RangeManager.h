#ifndef RANGE_MANAGER_H
#define RANGE_MANAGER_H

#include <vector>
#include <unordered_map>
#include <set>
#include "Hand.h"
#include "card_utility.h"
#include <iostream>
#include <string>
#include "HandEvaluator.h"
using std::vector;
using std::set;
using std::unordered_map;
using std::string;

class RangeManager
{
    private:
		vector<Hand> p1StartingHands;
		vector<Hand> p2StartingHands;

		unordered_map<int, vector<Hand>> p1Ranges;
		unordered_map<int, vector<Hand>> p2Ranges;

		unordered_map<int, vector<int>> p1ReachProbsMapping;
		unordered_map<int, vector<int>> p2ReachProbsMapping;

        HandEvaluator* handEvaluator;

		void quickSort(vector<Hand>& hands, vector<int>& handMap, int low, int high);
		int partition(vector<Hand>& hands, vector<int>& handMap, int low, int high);

        static bool compare_hands(Hand h1, Hand h2);
		void initialize_starting_range(int player, string startingHands, uint8_t initialBoard[5]);

		inline int get_key(uint8_t board[5])
		{
			if (!board_has_turn(board))
				return 100000000 * (board[0] + 1) +
				1000000 * (board[1] + 1) +
				10000 * (board[2] + 1);
			else if (!board_has_river(board))
				return
				100000000 * (board[0] + 1) +
				1000000 * (board[1] + 1) +
				10000 * (board[2] + 1) +
				100 * (board[3] + 1);
			return 100000000 * (board[0] + 1) +
				1000000 * (board[1] + 1) +
				10000 * (board[2] + 1) +
				100 * (board[3] + 1) +
				(board[4] + 1);
		}

    public:
		RangeManager(string p1StartingHands, string p2StartingHands, uint8_t initialBoard[5]);
		void initialize_ranges(int player, uint8_t initialBoard[5]);
		void initialize_reach_probs_mapping(int player, uint8_t initialBoard[5]);
		vector<float> get_reach_probs(int player, uint8_t board[5], vector<float>& reachProbs);
		vector<float> get_initial_reach_probs(int player);

		int get_num_hands(int player, uint8_t board[5]);
		vector<Hand>& get_starting_hands(int player);
		unordered_map<int, vector<int>>& get_reach_probs_mappings(int player);
		unordered_map<int, vector<Hand>>& get_ranges(int player);
		vector<Hand>& get_hands(int player, uint8_t board[5]);
		vector<int>& get_reach_probs_mapping(int player, uint8_t board[5]);
};

#endif
