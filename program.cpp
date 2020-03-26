#include "BetSettings.h"
#include "TreeBuildSettings.h"
#include "GameTree.h"
#include "card_utility.h"
#include "StreetEnum.h"
#include <vector>
#include <memory>
#include "RangeManager.h"
#include <iostream>
#include "Trainer.h"
using std::cout;
using std::move;
using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;
using std::make_shared;
using std::vector;

void testTurn2()
{
	string p1StartingHands = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AK,AQ,AJ,AT,A9,A8,A7,A6,A5,A4,A3,A2,KQ,KJ,KT,K9,K8,K7,K6,K5,K4,K3,K2,QJ,QT,Q9,Q8,Q7,Q6,Q5,Q4,Q3,Q2,JT,J9,J8,J7,J6,J5,J4,J3,J2,T9,T8,T7,T6,T5,T4,T3,T2,98,97,96,95,94,93,92,87,86,85,84,83,82,76,75,74,73,72,65,64,63,62,54,53,52,43,42,32";
	string p2StartingHands = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AK,AQ,AJ,AT,A9,A8,A7,A6,A5,A4,A3,A2,KQ,KJ,KT,K9,K8,K7,K6,K5,K4,K3,K2,QJ,QT,Q9,Q8,Q7,Q6,Q5,Q4,Q3,Q2,JT,J9,J8,J7,J6,J5,J4,J3,J2,T9,T8,T7,T6,T5,T4,T3,T2,98,97,96,95,94,93,92,87,86,85,84,83,82,76,75,74,73,72,65,64,63,62,54,53,52,43,42,32";

	int inPositionPlayerId = 2;

	Street initialStreet = Street::TURN;
	uint8_t initialBoard[5] = { card_from_string("Kd"), card_from_string("Jd"), card_from_string("Td"), card_from_string("5s"), 52 };

	int initialPotSize = 100;
	int startingStackSize = 1000;

	unique_ptr<BetSettings> p1BetSettings = make_unique<BetSettings>();
	unique_ptr<BetSettings> p2BetSettings = make_unique<BetSettings>();

	p1BetSettings->turnBetSizes.push_back(1.0f);
	p1BetSettings->riverBetSizes.push_back(1.0f);

	p1BetSettings->turnRaiseSizes.push_back(1.0f);
	p1BetSettings->riverRaiseSizes.push_back(1.0f);

	p2BetSettings->turnBetSizes.push_back(1.0f);
	p2BetSettings->riverBetSizes.push_back(1.0f);

	p2BetSettings->turnRaiseSizes.push_back(1.0f);
	p2BetSettings->riverRaiseSizes.push_back(1.0f);

	int minimumBetSize = 10;
	float allinThreshold = 0.67f;

	shared_ptr<RangeManager> rangeManager = make_shared<RangeManager>(p1StartingHands, p2StartingHands, initialBoard);

	unique_ptr<TreeBuildSettings> treeBuildSettings = make_unique<TreeBuildSettings>(
		rangeManager,
		inPositionPlayerId,
		initialStreet,
		initialBoard,
		initialPotSize,
		startingStackSize,
		move(p1BetSettings),
		move(p2BetSettings),
		minimumBetSize,
		allinThreshold);

	unique_ptr<GameTree> gameTree = make_unique<GameTree>(move(treeBuildSettings));
	unique_ptr<Node> root = gameTree->build();
	gameTree->print_tree(root.get(), 0);
	cout << '\n';

	unique_ptr<Trainer> trainer = make_unique<Trainer>(rangeManager, initialBoard, initialPotSize, inPositionPlayerId);
	trainer->train(root.get(), 200);
}

void testTurn3() {
	string p1StartingHands = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AK,AQ,AJ,AT,A9,A8,A7,A6,A5,A4,A3,A2,KQ,KJ,KT,K9,K8,K7,K6,K5,K4,K3,K2,QJ,QT,Q9,Q8,Q7,Q6,Q5,Q4,Q3,Q2,JT,J9,J8,J7,J6,J5,J4,J3,J2,T9,T8,T7,T6,T5,T4,T3,T2,98,97,96,95,94,93,92,87,86,85,84,83,82,76,75,74,73,72,65,64,63,62,54,53,52,43,42,32";
	string p2StartingHands = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AK,AQ,AJ,AT,A9,A8,A7,A6,A5,A4,A3,A2,KQ,KJ,KT,K9,K8,K7,K6,K5,K4,K3,K2,QJ,QT,Q9,Q8,Q7,Q6,Q5,Q4,Q3,Q2,JT,J9,J8,J7,J6,J5,J4,J3,J2,T9,T8,T7,T6,T5,T4,T3,T2,98,97,96,95,94,93,92,87,86,85,84,83,82,76,75,74,73,72,65,64,63,62,54,53,52,43,42,32";

	int inPositionPlayerId = 2;

	Street initialStreet = Street::TURN;
	uint8_t initialBoard[5] = { card_from_string("Kd"), card_from_string("Jd"), card_from_string("Td"), card_from_string("5s"), 52};

	int initialPotSize = 800;
	int startingStackSize = 800;

	unique_ptr<BetSettings> p1BetSettings = make_unique<BetSettings>();
	unique_ptr<BetSettings> p2BetSettings = make_unique<BetSettings>();

	p1BetSettings->turnBetSizes.push_back(1.0f);
	p1BetSettings->riverBetSizes.push_back(1.0f);

	p1BetSettings->turnRaiseSizes.push_back(1.0f);
	p1BetSettings->riverRaiseSizes.push_back(1.0f);

	p2BetSettings->turnBetSizes.push_back(1.0f);
	p2BetSettings->riverBetSizes.push_back(1.0f);

	p2BetSettings->turnRaiseSizes.push_back(1.0f);
	p2BetSettings->riverRaiseSizes.push_back(1.0f);

	int minimumBetSize = 10;
	float allinThreshold = 0.67f;

	shared_ptr<RangeManager> rangeManager = make_shared<RangeManager>(p1StartingHands, p2StartingHands, initialBoard);

	unique_ptr<TreeBuildSettings> treeBuildSettings = make_unique<TreeBuildSettings>(
		rangeManager,
		inPositionPlayerId,
		initialStreet,
		initialBoard,
		initialPotSize,
		startingStackSize,
		move(p1BetSettings),
		move(p2BetSettings),
		minimumBetSize,
		allinThreshold);

	unique_ptr<GameTree> gameTree = make_unique<GameTree>(move(treeBuildSettings));
	unique_ptr<Node> root = gameTree->build();
	gameTree->print_tree(root.get(), 0);
	cout << '\n';

	unique_ptr<Trainer> trainer = make_unique<Trainer>(rangeManager, initialBoard, initialPotSize, inPositionPlayerId);
	trainer->train(root.get(), 200);
    cout << "----"<< std::endl;
    gameTree->print_final_tree(root.get(),0);
}

void testRiver()
{
	string p1StartingHands = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AK,AQ,AJ,AT,A9,A8,A7,A6,A5,A4,A3,A2,KQ,KJ,KT,K9,K8,K7,K6,K5,K4,K3,K2,QJ,QT,Q9,Q8,Q7,Q6,Q5,Q4,Q3,Q2,JT,J9,J8,J7,J6,J5,J4,J3,J2,T9,T8,T7,T6,T5,T4,T3,T2,98,97,96,95,94,93,92,87,86,85,84,83,82,76,75,74,73,72,65,64,63,62,54,53,52,43,42,32";
	string p2StartingHands = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AK,AQ,AJ,AT,A9,A8,A7,A6,A5,A4,A3,A2,KQ,KJ,KT,K9,K8,K7,K6,K5,K4,K3,K2,QJ,QT,Q9,Q8,Q7,Q6,Q5,Q4,Q3,Q2,JT,J9,J8,J7,J6,J5,J4,J3,J2,T9,T8,T7,T6,T5,T4,T3,T2,98,97,96,95,94,93,92,87,86,85,84,83,82,76,75,74,73,72,65,64,63,62,54,53,52,43,42,32";

	int inPositionPlayerId = 2;

	Street initialStreet = Street::RIVER;
	uint8_t initialBoard[5] = { card_from_string("Kd"), card_from_string("Jd"), card_from_string("Td"), card_from_string("5s"), card_from_string("8s") };

	int initialPotSize = 400;
	int startingStackSize = 800;

	unique_ptr<BetSettings> p1BetSettings = make_unique<BetSettings>();
	unique_ptr<BetSettings> p2BetSettings = make_unique<BetSettings>();

	p1BetSettings->turnBetSizes.push_back(1.0f);
	p1BetSettings->riverBetSizes.push_back(1.0f);

	p1BetSettings->turnRaiseSizes.push_back(1.0f);
	p1BetSettings->riverRaiseSizes.push_back(1.0f);

	p2BetSettings->turnBetSizes.push_back(1.0f);
	p2BetSettings->riverBetSizes.push_back(1.0f);

	p2BetSettings->turnRaiseSizes.push_back(1.0f);
	p2BetSettings->riverRaiseSizes.push_back(1.0f);

	int minimumBetSize = 10;
	float allinThreshold = 0.67f;

	shared_ptr<RangeManager> rangeManager = make_shared<RangeManager>(p1StartingHands, p2StartingHands, initialBoard);

	unique_ptr<TreeBuildSettings> treeBuildSettings = make_unique<TreeBuildSettings>(
		rangeManager,
		inPositionPlayerId,
		initialStreet,
		initialBoard,
		initialPotSize,
		startingStackSize,
		move(p1BetSettings),
		move(p2BetSettings),
		minimumBetSize,
		allinThreshold);

	unique_ptr<GameTree> gameTree = make_unique<GameTree>(move(treeBuildSettings));
	unique_ptr<Node> root = gameTree->build();
	gameTree->print_tree(root.get(), 0);
	cout << '\n';

	unique_ptr<Trainer> trainer = make_unique<Trainer>(rangeManager, initialBoard, initialPotSize, inPositionPlayerId);
	trainer->train(root.get(), 1000);
}

void testTurn()
{
	string p1StartingHands = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AK,AQ,AJ,AT,A9,A8,A7,A6,A5,A4,A3,A2,KQ,KJ,KT,K9,K8,K7,K6,K5,K4,K3,K2,QJ,QT,Q9,Q8,Q7,Q6,Q5,Q4,Q3,Q2,JT,J9,J8,J7,J6,J5,J4,J3,J2,T9,T8,T7,T6,T5,T4,T3,T2,98,97,96,95,94,93,92,87,86,85,84,83,82,76,75,74,73,72,65,64,63,62,54,53,52,43,42,32";
	string p2StartingHands = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AK,AQ,AJ,AT,A9,A8,A7,A6,A5,A4,A3,A2,KQ,KJ,KT,K9,K8,K7,K6,K5,K4,K3,K2,QJ,QT,Q9,Q8,Q7,Q6,Q5,Q4,Q3,Q2,JT,J9,J8,J7,J6,J5,J4,J3,J2,T9,T8,T7,T6,T5,T4,T3,T2,98,97,96,95,94,93,92,87,86,85,84,83,82,76,75,74,73,72,65,64,63,62,54,53,52,43,42,32";

	int inPositionPlayerId = 2;

	Street initialStreet = Street::TURN;
	uint8_t initialBoard[5] = { card_from_string("Kd"), card_from_string("Jd"), card_from_string("Td"), card_from_string("5s"), 52 };

	int initialPotSize = 100;
	int startingStackSize = 1000;

	unique_ptr<BetSettings> p1BetSettings = make_unique<BetSettings>();
	unique_ptr<BetSettings> p2BetSettings = make_unique<BetSettings>();

	p1BetSettings->turnBetSizes.push_back(0.5f);
	p1BetSettings->turnBetSizes.push_back(1.0f);

	p1BetSettings->riverBetSizes.push_back(0.25f);
	p1BetSettings->riverBetSizes.push_back(0.5f);
	p1BetSettings->riverBetSizes.push_back(1.0f);

	p1BetSettings->turnRaiseSizes.push_back(0.5f);
	p1BetSettings->riverRaiseSizes.push_back(0.5f);

	p2BetSettings->turnBetSizes.push_back(0.5f);
	p2BetSettings->turnBetSizes.push_back(1.0f);

	p2BetSettings->riverBetSizes.push_back(0.25f);
	p2BetSettings->riverBetSizes.push_back(0.5f);
	p2BetSettings->riverBetSizes.push_back(1.0f);

	p2BetSettings->turnRaiseSizes.push_back(0.5f);
	p2BetSettings->riverRaiseSizes.push_back(0.5f);

	int minimumBetSize = 10;
	float allinThreshold = 0.67f;

	shared_ptr<RangeManager> rangeManager = make_shared<RangeManager>(p1StartingHands, p2StartingHands, initialBoard);

	unique_ptr<TreeBuildSettings> treeBuildSettings = make_unique<TreeBuildSettings>(
		rangeManager,
		inPositionPlayerId,
		initialStreet,
		initialBoard,
		initialPotSize,
		startingStackSize,
		move(p1BetSettings),
		move(p2BetSettings),
		minimumBetSize,
		allinThreshold);

	unique_ptr<GameTree> gameTree = make_unique<GameTree>(move(treeBuildSettings));
	unique_ptr<Node> root = gameTree->build();
	gameTree->print_tree(root.get(), 0);
	cout << '\n';

	unique_ptr<Trainer> trainer = make_unique<Trainer>(rangeManager, initialBoard, initialPotSize, inPositionPlayerId);
	trainer->train(root.get(), 200);
    cout << "----" << std::endl;
	gameTree->print_final_tree(root.get(), 0);
}

void testFlop()
{
	string p1StartingHands = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AK,AQ,AJ,AT,A9,A8,A7,A6,A5,A4,A3,A2,KQ,KJ,KT,K9,K8,K7,K6,K5,K4,K3,K2,QJ,QT,Q9,Q8,Q7,Q6,Q5,Q4,Q3,Q2,JT,J9,J8,J7,J6,J5,J4,J3,J2,T9,T8,T7,T6,T5,T4,T3,T2,98,97,96,95,94,93,92,87,86,85,84,83,82,76,75,74,73,72,65,64,63,62,54,53,52,43,42,32";
	string p2StartingHands = "AA,KK,QQ,JJ,TT,99,88,77,66,55,44,33,22,AK,AQ,AJ,AT,A9,A8,A7,A6,A5,A4,A3,A2,KQ,KJ,KT,K9,K8,K7,K6,K5,K4,K3,K2,QJ,QT,Q9,Q8,Q7,Q6,Q5,Q4,Q3,Q2,JT,J9,J8,J7,J6,J5,J4,J3,J2,T9,T8,T7,T6,T5,T4,T3,T2,98,97,96,95,94,93,92,87,86,85,84,83,82,76,75,74,73,72,65,64,63,62,54,53,52,43,42,32";

	int inPositionPlayerId = 2;

	Street initialStreet = Street::FLOP;
	uint8_t initialBoard[5] = { card_from_string("Kd"), card_from_string("Jc"), card_from_string("Ts"), 52, 52 };

	int initialPotSize = 55;
	int startingStackSize = 975;

	unique_ptr<BetSettings> p1BetSettings = make_unique<BetSettings>();
	unique_ptr<BetSettings> p2BetSettings = make_unique<BetSettings>();

	p1BetSettings->flopBetSizes.push_back(0.5f);
	p1BetSettings->flopBetSizes.push_back(1.0f);
	p1BetSettings->turnBetSizes.push_back(1.0f);
	p1BetSettings->riverBetSizes.push_back(1.0f);

	p1BetSettings->flopRaiseSizes.push_back(0.5f);
	p1BetSettings->turnRaiseSizes.push_back(0.5f);
	p1BetSettings->riverRaiseSizes.push_back(0.5f);

	p2BetSettings->flopBetSizes.push_back(0.5f);
	p2BetSettings->flopBetSizes.push_back(1.0f);
	p2BetSettings->turnBetSizes.push_back(1.0f);
	p2BetSettings->riverBetSizes.push_back(1.0f);

	p2BetSettings->flopRaiseSizes.push_back(0.5f);
	p2BetSettings->turnRaiseSizes.push_back(0.5f);
	p2BetSettings->riverRaiseSizes.push_back(0.5f);

	int minimumBetSize = 10;
	float allinThreshold = 0.67f;

	shared_ptr<RangeManager> rangeManager = make_shared<RangeManager>(p1StartingHands, p2StartingHands, initialBoard);

	unique_ptr<TreeBuildSettings> treeBuildSettings = make_unique<TreeBuildSettings>(
		rangeManager,
		inPositionPlayerId,
		initialStreet,
		initialBoard,
		initialPotSize,
		startingStackSize,
		move(p1BetSettings),
		move(p2BetSettings),
		minimumBetSize,
		allinThreshold);

	unique_ptr<GameTree> gameTree = make_unique<GameTree>(move(treeBuildSettings));
	unique_ptr<Node> root = gameTree->build();
	gameTree->print_tree(root.get(), 0);
	cout << '\n';

	unique_ptr<Trainer> trainer = make_unique<Trainer>(rangeManager, initialBoard, initialPotSize, inPositionPlayerId);
	trainer->train(root.get(), 500);
}

int main()
{
	testTurn3();
	//getchar();

    return 0;
}
