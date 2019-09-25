#ifndef HAND_H
#define HAND_H

#include <stdint.h>

class Hand
{
    public:
		uint8_t card1;
		uint8_t card2;
		int rank;
		float probability;
        
        Hand(uint8_t card1, uint8_t card2);
};

#endif