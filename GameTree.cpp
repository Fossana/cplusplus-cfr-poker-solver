#include "GameTree.h"
#include "BetSettings.h"
#include <algorithm>
#include <iostream>
#include "card_utility.h"
#include "NodeTypeEnum.h"

using std::make_unique;
using std::min;
using std::cout;

int flopActionNodeCount = 0;
int turnActionNodeCount = 0;
int riverActionNodeCount = 0;
int chanceNodeCount = 0;
int allinNodeCount = 0;
int showdownNodeCount = 0;
int uncontestedNodeCount = 0;

GameTree::GameTree(unique_ptr<TreeBuildSettings> treeBuildSettings)
{
    this->treeBuildSettings = move(treeBuildSettings);
}

void GameTree::print_tree(Node* node, int tabCount)
{
    if (node->type == NodeType::ACTION)
    {
        ActionNode* actionNode = static_cast<ActionNode*>(node);

        auto actionIt = begin(actionNode->actions);
        for (auto childIt = begin(actionNode->children); childIt != end(actionNode->children); childIt++)
        {
            Action* action = actionIt->get();
            Node* child = childIt->get();

            for (int i = 0; i < tabCount; i++)
            cout << "    ";

            if (action->type == ActionType::FOLD)
                cout << "p" << actionNode->player << ": FOLD\n";
            else if (action->type == ActionType::CHECK)
                cout << "p" << actionNode->player << ": CHECK\n";
            else if (action->type == ActionType::CALL)
                cout << "p" << actionNode->player << ": CALL " << action->amount << "\n";
            else if (action->type == ActionType::BET)
                cout << "p" << actionNode->player << ": BET " << action->amount << "\n";
            if (action->type == ActionType::RAISE)
                cout << "p" << actionNode->player << ": RAISE " << action->amount << "\n";

            print_tree(child, tabCount+1);
            actionIt++;
        }
    }
    else if (node->type == NodeType::CHANCE)
    {
        for (int i = 0; i < tabCount; i++)
            cout << "    ";

        ChanceNode* chanceNode = static_cast<ChanceNode*>(node);
        if (chanceNode->type == ChanceNodeType::DEAL_TURN)
            cout << "DEAL_TURN\n";
        else if (chanceNode->type == ChanceNodeType::DEAL_RIVER)
            cout << "DEAL_RIVER\n";

        print_tree(chanceNode->get_children().at(0)->node.get(), tabCount+1);
    }
    else
    {
        for (int i = 0; i < tabCount; i++)
            cout << "    ";

        TerminalNode* terminalNode = static_cast<TerminalNode*>(node);
        if (terminalNode->type == TerminalNodeType::ALLIN)
            cout << "ALLIN: ";
        else if (terminalNode->type == TerminalNodeType::UNCONTESTED)
            cout << "UNCONTESTED: ";
        else if (terminalNode->type == TerminalNodeType::SHOWDOWN)
            cout << "SHOWDOWN: ";
        cout << "POT: " << terminalNode->value * 2 << " LAST_TO_ACT: p" << terminalNode->lastToAct << "\n";
    }
}

unique_ptr<Node> GameTree::build()
{
    unique_ptr<State> initialState = get_initial_state();
    unique_ptr<Node> root = build_action_nodes(NULL, *initialState);
	root->parent = nullptr;

    initialState.reset();

    cout << "Flop action node count: " << flopActionNodeCount << "\n";
    cout << "Turn action node count: " << turnActionNodeCount << "\n";
    cout << "River action node count: " << riverActionNodeCount << "\n";
    cout << "Chance node count: " << chanceNodeCount << "\n";
    cout << "Uncontested node count: " << uncontestedNodeCount << "\n";
	cout << "Allin node count: " << allinNodeCount << "\n";
	cout << "Showdown node count: " << showdownNodeCount << "\n";

    return root;
}

unique_ptr<State> GameTree::get_initial_state()
{
    unique_ptr<State> state = make_unique<State>();

    state->street = treeBuildSettings->initialStreet;
		
    state->potSize = treeBuildSettings->initialPotSize;
    
    state->minimumBetSize = treeBuildSettings->minimumBetSize;
    state->minimumRaiseSize = treeBuildSettings->minimumBetSize;
    
    unique_ptr<PlayerState> p1 = make_unique<PlayerState>(1, treeBuildSettings->inPositionPlayerId == 1, treeBuildSettings->startingStackSize);
    unique_ptr<PlayerState> p2 = make_unique<PlayerState>(2, treeBuildSettings->inPositionPlayerId == 2, treeBuildSettings->startingStackSize);
    
    state->p1 = move(p1);
    state->p2 = move(p2);
    
    for (int i = 0; i < 5; i++)
        state->board[i] = treeBuildSettings->initialBoard[i];
    
    state->initialize_current();
    state->initialize_lastToAct();
    
    return state;
}

unique_ptr<Node> GameTree::build_action_nodes(Node* parent, State& state)
{
    int numHands = 0;
	int p1NumHands = treeBuildSettings->rangeManager->get_num_hands(1, state.board);
	int p2NumHands = treeBuildSettings->rangeManager->get_num_hands(2, state.board);

	if (state.get_current_id() == 1)
		numHands = p1NumHands;
	else if (state.get_current_id() == 2)
		numHands = p2NumHands;

    unique_ptr<Node> actionNode = make_unique<ActionNode>(parent, state.get_current_id(), numHands);
	actionNode->type = NodeType::ACTION;

    if (state.street == Street::FLOP)
        flopActionNodeCount++;
    else if (state.street == Street::TURN)
        turnActionNodeCount++;
    else if (state.street == Street::RIVER)
        riverActionNodeCount++;
    
    vector<float> betSizes;
    vector<float> raiseSizes;
    
    unique_ptr<BetSettings>* p1BetSettings = &treeBuildSettings->p1BetSettings;
    unique_ptr<BetSettings>* p2BetSettings = &treeBuildSettings->p2BetSettings;
    
    if (state.street == Street::FLOP)
    {
        if (state.get_current_id() == 1)
        {
            betSizes = (*p1BetSettings)->flopBetSizes;
            raiseSizes = (*p1BetSettings)->flopRaiseSizes;
        }
        else
        {
            betSizes = (*p2BetSettings)->flopBetSizes;
            raiseSizes = (*p2BetSettings)->flopRaiseSizes;
        }
    }
    else if (state.street == Street::TURN)
    {
        if (state.get_current_id() == 1)
        {
            betSizes = (*p1BetSettings)->turnBetSizes;
            raiseSizes = (*p1BetSettings)->turnRaiseSizes;
        }
        else
        {
            betSizes = (*p2BetSettings)->turnBetSizes;
            raiseSizes = (*p2BetSettings)->turnRaiseSizes;
        }
    }
    else if (state.street == Street::RIVER)
    {
        if (state.get_current_id() == 1)
        {
            betSizes = (*p1BetSettings)->riverBetSizes;
            raiseSizes = (*p1BetSettings)->riverRaiseSizes;
        }
        else
        {
            betSizes = (*p2BetSettings)->riverBetSizes;
            raiseSizes = (*p2BetSettings)->riverRaiseSizes;
        }
    }
    
    for (int actionType = ActionType::FOLD; actionType <= ActionType::RAISE; actionType++)
    {
        if (actionType == ActionType::FOLD)
        {
            unique_ptr<Action> action = make_unique<Action>(ActionType::FOLD, 0);
            build_action(actionNode.get(), state, p1NumHands, p2NumHands, move(action));
        }
        else if (actionType == ActionType::CHECK)
        {
            unique_ptr<Action> action = make_unique<Action>(ActionType::CHECK, 0);
            build_action(actionNode.get(), state, p1NumHands, p2NumHands, move(action));
        }	
        else if (actionType == ActionType::CALL)
        {
            unique_ptr<Action> action = make_unique<Action>(ActionType::CALL, state.get_call_amount());
            build_action(actionNode.get(), state, p1NumHands, p2NumHands, move(action));
        }
        else if (actionType == ActionType::BET)
        {
            for (auto it = begin(betSizes); it != end(betSizes); it++)
            {
                float betSize = *it;

                int betAmount = (int) (betSize * state.potSize);
                betAmount = min(betAmount, state.get_current_stack());
                if (((float) betAmount + state.get_current_wager()) / (state.get_current_stack() + state.get_current_wager()) >= treeBuildSettings->allinThreshold)
                {
                    betAmount = state.get_current_stack();
                    unique_ptr<Action> action = make_unique<Action>(ActionType::BET, betAmount);
                    build_action(actionNode.get(), state, p1NumHands, p2NumHands, move(action));
                    break;
                }
                else
                {
                    unique_ptr<Action> action = make_unique<Action>(ActionType::BET, betAmount);
                    build_action(actionNode.get(), state, p1NumHands, p2NumHands, move(action));
                }	
            }
        }
        else if (actionType == ActionType::RAISE)
        {
            for (auto it = begin(raiseSizes); it != end(raiseSizes); it++)
            {
                float raiseSize = *it;

                int raiseAmount = (int) (state.get_current_wager() + state.get_call_amount() + raiseSize * (state.potSize + state.get_call_amount()));
                raiseAmount = min(raiseAmount, state.get_current_wager() + state.get_current_stack());
                if ((float) raiseAmount / (state.get_current_stack() + state.get_current_wager()) >= treeBuildSettings->allinThreshold)
                {
                    raiseAmount = state.get_current_stack() + state.get_current_wager();
                    unique_ptr<Action> action = make_unique<Action>(ActionType::RAISE, raiseAmount);
                    build_action(actionNode.get(), state, p1NumHands, p2NumHands, move(action));
                    break;
                }
                else
                {
                    unique_ptr<Action> action = make_unique<Action>(ActionType::RAISE, raiseAmount);
                    build_action(actionNode.get(), state, p1NumHands, p2NumHands, move(action));
                }
            }
        }
    }

    static_cast<ActionNode*>(actionNode.get())->initialize_strategySum();
	static_cast<ActionNode*>(actionNode.get())->initialize_regretSum();

    return actionNode;
}

void GameTree::build_action(
    Node* actionNode,
    State& state,
    int p1NumHands,
    int p2NumHands,
    unique_ptr<Action> action)
{
    if (Action::is_valid_action(*action, state.get_current_stack(), state.get_current_wager(), state.get_call_amount(), state.minimumRaiseSize))
    {
        unique_ptr<Node> child = nullptr;
        unique_ptr<State> nextState = make_unique<State>(state);
        bool betsSettled = nextState->apply_player_action(*action);
        if (betsSettled)
        {
            if (nextState->is_uncontested() || nextState->both_players_are_allin() || nextState->street == Street::RIVER)
                child = build_terminal_nodes(actionNode, *nextState);
            else
                child = build_chance_nodes(actionNode, *nextState);
        }
        else
        {
            child = build_action_nodes(actionNode, *nextState);
        }
        static_cast<ActionNode*>(actionNode)->add_child(move(child), move(action));
        nextState.reset();
    }
}

unique_ptr<Node> GameTree::build_chance_nodes(Node* parent, State& state)
{
    unique_ptr<Node> node = nullptr;
    
    if (state.street == Street::FLOP)
        node = make_unique<ChanceNode>(parent, ChanceNodeType::DEAL_TURN);
    else if (state.street == Street::TURN)
        node = make_unique<ChanceNode>(parent, ChanceNodeType::DEAL_RIVER);

	node->type = NodeType::CHANCE;

    ChanceNode *chanceNode = static_cast<ChanceNode*>(node.get());
    chanceNodeCount++;
    
    for (uint8_t card = 0; card < 52; card++)
    {
		if (overlap(card, state.board))
			continue;

        unique_ptr<State> nextState = make_unique<State>(state);
        
        if (state.street == Street::FLOP)
            nextState->board[3] = card;
        else if (state.street == Street::TURN)
            nextState->board[4] = card;
        
        nextState->go_to_next_street();
        
        unique_ptr<Node> actionNode = build_action_nodes(node.get(), *nextState);
        chanceNode->add_child(move(actionNode), card);
    }
    
    return node;
}

unique_ptr<Node> GameTree::build_terminal_nodes(Node* parent, State& state)
{
    unique_ptr<Node> node = nullptr;
    
	if (state.both_players_are_allin() && state.street != Street::RIVER)
	{
		allinNodeCount++;
		node = make_unique<TerminalNode>(parent, TerminalNodeType::ALLIN);
	}
	else if (state.is_uncontested())
	{
		uncontestedNodeCount++;
		node = make_unique<TerminalNode>(parent, TerminalNodeType::UNCONTESTED);
	}
	else
	{
		showdownNodeCount++;
		node = make_unique<TerminalNode>(parent, TerminalNodeType::SHOWDOWN);
	}
        
	node->type = NodeType::TERMINAL;

    TerminalNode *terminalNode = static_cast<TerminalNode*>(node.get());
    
    terminalNode->value = state.potSize / 2.0f;
    
    for (int i = 0; i < 5; i++)
        terminalNode->board[i] = state.board[i];
    
    Node* temp = parent;
    while (!temp->type == NodeType::ACTION)
    {
        temp = temp->parent;
    }
    
    terminalNode->lastToAct = static_cast<ActionNode*>(temp)->player;
    
    return node;
}