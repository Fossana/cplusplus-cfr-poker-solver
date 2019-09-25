#ifndef BET_SETTINGS_H
#define BET_SETTINGS_H

#include <vector>
using std::vector;

class BetSettings
{
    public:
        vector<float> flopBetSizes;
        vector<float> turnBetSizes;
        vector<float> riverBetSizes;

        vector<float> flopRaiseSizes;
        vector<float> turnRaiseSizes;
        vector<float> riverRaiseSizes;
};

#endif