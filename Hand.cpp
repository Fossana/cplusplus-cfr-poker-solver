#include "Hand.h"

Hand::Hand(uint8_t card1, uint8_t card2)
{
    this->card1 = card1;
    this->card2 = card2;
	probability = 0;
	rank = 0;
}