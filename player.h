#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <iostream>
#include "common.h"
#include "board.h"
#include <vector>
using namespace std;

    float max_value(Side side, Board * board, int depth, float alpha, float beta);
    float min_value(Side side, Board * board, int depth, float alpha, float beta) ;
    Move * abpruning(Side side, Board * board, long msLeft);

class Player {

public:
    Player(Side side);
    ~Player();
    
    Move *doMove(Move *opponentsMove, int msLeft);
    Board * board;

    // Flag to tell if the player is running within the test_minimax context
    bool testingMinimax;
private:
    Side playerSide;
    Side opponentSide;
};

#endif
