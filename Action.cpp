#include "Action.h"

Action::Action(ActionType type, int amount)
{
    this->type = type;
    this->amount = amount;
}

bool Action::is_valid_action(Action& action, int stackSize, int wager, int callAmount, int minimumRaiseSize)
{
    switch(action.type)
		{
			case FOLD:
				return callAmount > 0; // don't allow folding if checking is an option
			case CHECK:
				return callAmount == 0;
			case CALL:
				return callAmount > 0
					&& ((action.amount == callAmount && action.amount <= stackSize)
					|| action.amount == stackSize);
			case BET:
				return callAmount == 0
					&& ((action.amount > minimumRaiseSize && action.amount <= stackSize)
					|| (action.amount > 0 && action.amount == stackSize));
			case RAISE:
            {
                int raiseSize = action.amount - callAmount - wager;
				return callAmount > 0
					&& ((raiseSize >= minimumRaiseSize && action.amount <= (stackSize + wager))
					|| (raiseSize > 0 && action.amount == (stackSize + wager)));
            }
			default:
				return false;
		}
}