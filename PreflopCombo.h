#ifndef PREFLOP_COMBO_H
#define PREFLOP_COMBO_H

class PreflopCombo
{
    public:
        float probability;
        float relativeProbability;
        int card1;
        int card2;
        
        PreflopCombo(int card1, int card2, float probability);
};

#endif