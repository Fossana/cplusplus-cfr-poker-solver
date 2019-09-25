#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

#include <memory>
using std::unique_ptr;

class PlayerState
{
    public:
        bool hasPosition;
        bool hasFolded;
        int stackSize;
        int chipsCommitted;
        int wager;
        int id;

        PlayerState(int id, bool hasPosition, int stackSize);
        PlayerState(PlayerState& playerState);
        bool is_allin();
        void commit_chips(int amount);
        void uncommit_chips(int amount);
        void reset_wager();
};

#endif