#ifndef ACTION_H
#define ACTION_H

#include "ActionTypeEnum.h"

class Action
{
    public:
        ActionType type;
        int amount;
        Action(ActionType type, int amount);
        static bool is_valid_action(Action& action, int stackSize, int wager, int callAmount, int minimumRaiseSize);
};

#endif