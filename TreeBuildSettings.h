#ifndef TREE_BUILD_SETTINGS_H
#define TREE_BUILD_SETTINGS_H

#include "BetSettings.h"
#include <stdint.h>
#include "StreetEnum.h"
#include <memory>
#include "RangeManager.h"
using std::shared_ptr;
using std::unique_ptr;

class TreeBuildSettings
{
    public:
		shared_ptr<RangeManager> rangeManager;
        int inPositionPlayerId;

        Street initialStreet;
        uint8_t initialBoard[5];
        int initialPotSize;
        int startingStackSize;

        unique_ptr<BetSettings> p1BetSettings;
        unique_ptr<BetSettings> p2BetSettings;

        int minimumBetSize;
        float allinThreshold;

        TreeBuildSettings(
			shared_ptr<RangeManager> rangeManager,
			int inPositionPlayerId,
            Street initialStreet,
			uint8_t initialBoard[5],
			int initialPotSize,
			int startingStackSize,
			unique_ptr<BetSettings> p1BetSettings,
			unique_ptr<BetSettings> p2BetSettings,
			int minimumBetSize,
			float allinThreshold
        );
};

#endif