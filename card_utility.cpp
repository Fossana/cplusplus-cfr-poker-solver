#include "card_utility.h"

uint8_t card_from_rank_and_suit(char rank, uint8_t suit)
{
    return (rank_to_int(rank) - 2) * 4 + suit;
}

uint8_t rank_to_int(char rank)
{
    switch(rank)
    {
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'T': return 10;
        case 'J': return 11;
        case 'Q': return 12;
        case 'K': return 13;
        case 'A': return 14;
        default: return 2;
    }
}

uint8_t suit_to_int(char suit)
{
    switch(suit)
    {
        case 'c': return 0;
        case 'd': return 1;
        case 'h': return 2;
        case 's': return 3;
        default: return 0;
    }
}

uint8_t card_from_string(string str)
{
    char rank = str.at(0);
    char suit = str.at(1);

    return (rank_to_int(rank) - 2) * 4 + suit_to_int(suit);
}