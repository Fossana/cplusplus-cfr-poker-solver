#ifndef CARD_UTILITY_H
#define CARD_UTILITY_H

#include "Hand.h"
#include <string>
#include <vector>
using std::vector;
using std::string;

uint8_t card_from_rank_and_suit(char rank, uint8_t suit);
uint8_t rank_to_int(char rank);
uint8_t suit_to_int(char suit);
uint8_t card_from_string(string str);

inline uint8_t card_from_rank_and_suit2(uint8_t rank, uint8_t suit)
{
	return (rank - 2) * 4 + suit;
}

inline bool board_has_turn(uint8_t board[5])
{
	return board[3] != 52;
}

inline bool board_has_river(uint8_t board[5])
{
	return board[4] != 52;
}

inline uint8_t get_turn(uint8_t board[5])
{
	return board[3];
}

inline bool equals(Hand h1, Hand h2)
{
	return h1.card1 == h2.card1 && h1.card2 == h2.card2;
}

inline void set_turn(uint8_t board[5], uint8_t turn)
{
	board[3] = turn;
}

inline uint8_t get_river(uint8_t board[5])
{
	return board[4];
}

inline void set_river(uint8_t board[5], uint8_t river)
{
	board[4] = river;
}

inline uint8_t suit_from_card(uint8_t card)
{
	uint8_t rank = card / 4 + 2;
	return card - (rank - 2) * 4;
}

inline uint8_t get_flush_suit(uint8_t board[5])
{
	int clubCount = 0;
	int diamondCount = 0;
	int heartCount = 0;
	int spadeCount = 0;

	for (int i = 0; i < 5; i++)
		if (board[i] != 52)
		{
			uint8_t suit = suit_from_card(board[i]);
			if (suit == 0)
				clubCount++;
			else if (suit == 1)
				diamondCount++;
			else if (suit == 2)
				heartCount++;
			else
				spadeCount++;
		}

	if (clubCount == 2)
		return 0;
	else if (diamondCount == 2)
		return 1;
	else if (heartCount == 2)
		return 2;
	else if (spadeCount == 2)
		return 3;
	else
		return -1;
}

inline bool overlap(uint8_t holeCard1, uint8_t holeCard2, uint8_t board[5])
{
	for (int i = 0; i < 5; i++)
		if (holeCard1 == board[i] || holeCard2 == board[i])
			return true;
	return false;
}

inline bool overlap(Hand& h1, Hand& h2)
{
	if (h1.card1 == h2.card1 || h1.card1 == h2.card2)
		return true;
	if (h1.card2 == h2.card1 || h1.card2 == h2.card2)
		return true;
	return false;
}

inline bool overlap(Hand& hand, uint8_t card)
{
	if (hand.card1 == card || hand.card2 == card)
		return true;
	return false;
}

inline bool overlap(Hand& hand, uint8_t board[5])
{
	for (int i = 0; i < 5; i++)
		if (hand.card1 == board[i] || hand.card2 == board[i])
			return true;

	return false;
}

inline bool overlap(uint8_t card, uint8_t board[5])
{
	for (int i = 0; i < 5; i++)
		if (card == board[i])
			return true;

	return false;
}

#endif

