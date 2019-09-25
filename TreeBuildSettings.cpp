#include "TreeBuildSettings.h"

 TreeBuildSettings::TreeBuildSettings(
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
        )
{
	this->rangeManager = rangeManager;
	this->inPositionPlayerId = inPositionPlayerId;
	this->initialStreet = initialStreet;
	for (int i = 0; i < 5; i++)
		this->initialBoard[i] = initialBoard[i];
	this->initialPotSize = initialPotSize;
	this->startingStackSize = startingStackSize;
	this->p1BetSettings = move(p1BetSettings);
	this->p2BetSettings = move(p2BetSettings);
	this->minimumBetSize = minimumBetSize;
	this->allinThreshold = allinThreshold;
}