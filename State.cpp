#include "State.h"
#include <iostream>
using std::make_unique;
using std::move;
using std::cout;

State::State()
{

}

State::State(State& state)
{
    street = state.street;
    potSize = state.potSize;

    for (int i = 0; i < 5; i++)
        board[i] = state.board[i];

    p1 = make_unique<PlayerState>(*state.p1);
    p2 = make_unique<PlayerState>(*state.p2);

    if (state.current->id == 1)
        current = p1.get();
    else
        current = p2.get();

    if (state.lastToAct->id == 1)
        lastToAct = p1.get();
    else
        lastToAct = p2.get();

    minimumRaiseSize = state.minimumRaiseSize;
    minimumBetSize = state.minimumBetSize;
}

int State::get_highest_wager()
{
    if (p1->wager > p2->wager)
        return p1->wager;
    return p2->wager;
}

int State::get_call_amount()
{
    return get_highest_wager() - current->wager;
}

bool State::is_uncontested()
{
    return p1->hasFolded || p2->hasFolded;
}

bool State::both_players_are_allin()
{
    return p1->is_allin() && p2->is_allin();
}

bool State::apply_player_action(Action& action)
{
    if (action.type == ActionType::FOLD)
    {
        current->hasFolded = true;
        potSize -= get_call_amount();
        return true;
    }
    else if (action.type == ActionType::CHECK)
    {
        if (current == lastToAct)
            return true;
    }
    else if (action.type == ActionType::CALL)
    {
        current->commit_chips(action.amount);
        potSize += action.amount;
        return true;
    }
    else if (action.type == ActionType::BET)
    {
        current->commit_chips(action.amount);
        potSize += action.amount;
        minimumRaiseSize = action.amount;
        reset_lastToAct();
    }
    else if (action.type == ActionType::RAISE)
    {
        int chipsToCommit = action.amount - current->wager;
        current->commit_chips(chipsToCommit);
        potSize += chipsToCommit;
        int raiseSize = action.amount - get_highest_wager();
        if (raiseSize > minimumRaiseSize)
            minimumRaiseSize = raiseSize;
        reset_lastToAct();
    }
    update_current();
    return false;
}

void State::go_to_next_street()
{
    if (street == Street::FLOP)
        street = Street::TURN;
    else if (street == Street::TURN)
        street = Street::RIVER;

    p1->reset_wager();
    p2->reset_wager();

    initialize_current();
    initialize_lastToAct();

    minimumRaiseSize = minimumBetSize;
}

void State::initialize_current()
{
    if (!p1->hasPosition)
        current = p1.get();
    else
        current = p2.get();
}

void State::initialize_lastToAct()
{
    if (p1->hasPosition)
        lastToAct = p1.get();
    else
        lastToAct = p2.get();
}

void State::update_current()
{
    if (current == p1.get())
        current = p2.get();
    else
        current = p1.get();
}

void State::reset_lastToAct()
{
    if (lastToAct == p1.get())
        lastToAct = p2.get();
    else
        lastToAct = p1.get();
}

int State::get_current_wager()
{
    return current->wager;
}

int State::get_current_stack()
{
    return current->stackSize;
}

int State::get_current_id()
{
    return current->id;
}