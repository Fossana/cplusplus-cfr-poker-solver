#ifndef HAND_EVALUATOR_H
#define HAND_EVALUATOR_H

#include <stdint.h>

class HandEvaluator
{
    private:
        int HR[32487834];
        static HandEvaluator* instance;
        HandEvaluator();

    public:
        int get_hand_rank(uint8_t holeCard1, uint8_t holeCard2, uint8_t board[5]);
        void test();
        static HandEvaluator* get_instance();
};

#endif

