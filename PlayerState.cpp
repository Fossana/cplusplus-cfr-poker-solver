#include "PlayerState.h"

PlayerState::PlayerState(int id, bool hasPosition, int stackSize)
{
    this->id = id;
    this->hasPosition = hasPosition;
    this->stackSize = stackSize;
    hasFolded = false;
    wager = 0;
    chipsCommitted = 0;
}

PlayerState::PlayerState(PlayerState& playerState)
{
    id = playerState.id;
	hasPosition = playerState.hasPosition;
	hasFolded = playerState.hasFolded;
	stackSize = playerState.stackSize;
	wager = playerState.wager;
	chipsCommitted = playerState.chipsCommitted;
	hasPosition = playerState.hasPosition;
}

bool PlayerState::is_allin()
{
    return stackSize == 0;
}

void PlayerState::commit_chips(int amount)
{
    wager += amount;
    chipsCommitted += amount;
    stackSize -= amount;
}

void PlayerState::uncommit_chips(int amount)
{
    wager -= amount;
    chipsCommitted -= amount;
    stackSize += amount;
}

void PlayerState::reset_wager()
{
    wager = 0;
}