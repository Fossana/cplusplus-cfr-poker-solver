#ifndef STATE_H
#define STATE_H

#include "StreetEnum.h"
#include "PlayerState.h"
#include "Action.h"
#include <memory>
using std::unique_ptr;

class State
{
    public:
        Street street;
        int potSize;
		uint8_t board[5];
        unique_ptr<PlayerState> p1;
        unique_ptr<PlayerState> p2;
        PlayerState* current;
        PlayerState* lastToAct;
        int minimumRaiseSize;
        int minimumBetSize;
        
        State();
        State(State& state);
        int get_highest_wager();
        int get_call_amount();
        bool is_uncontested();
        bool both_players_are_allin();
        bool apply_player_action(Action& action);
        void go_to_next_street();
        void initialize_current();
        void initialize_lastToAct();
        void update_current();
        void reset_lastToAct();
        int get_current_wager();
        int get_current_stack();
        int get_current_id();
};

#endif