#include "player.h"
#include <iostream>
#include <vector>
#include <ctime> // for timing
#include <cstdlib>
#include <unistd.h>

#define BOARD_SIZE 8
#define EPSILON_TIME 15 

void boardtest(Board * testboard) {
    for (int y=0; y<BOARD_SIZE; y++) {
        string s = "";
        for (int x=0; x<BOARD_SIZE; x++) {
            if (testboard->get(BLACK, x,y)) {
                s += "b ";
            } else if (testboard->get(WHITE, x, y)) {
                    s += "w ";
            } else {
                s += "- ";
            }
        }
        std::cerr << s << std::endl;
    }
}

/*
 * Constructor for the player; initialize everything here. The side your AI is
 * on (BLACK or WHITE) is passed in as "side". The constructor must finish 
 * within 30 seconds.
 */
Player::Player(Side side) {
    // Will be set to true in test_minimax.cpp.
    testingMinimax = false;

    playerSide = side;
    opponentSide = (side == BLACK) ? WHITE : BLACK;
    board = new Board();
    srand(time(NULL));
    boardtest(board);
}



/*
 * Destructor for the player.
 */
Player::~Player() {
    delete board;
}

/*
 * Compute the next move given the opponent's last move. Your AI is
 * expected to keep track of the board on its own. If this is the first move,
 * or if the opponent passed on the last move, then opponentsMove will be NULL.
 *
 * msLeft represents the time your AI has left for the total game, in
 * milliseconds. doMove() must take no longer than msLeft, or your AI will
 * be disqualified! An msLeft value of -1 indicates no time limit.
 *
 * The move returned must be legal; if there are no valid moves for your side,
 * return NULL.
 */
Move *Player::doMove(Move *opponentsMove, int msLeft) {
    //time_t init = time(0);


    
    if (opponentsMove != NULL) {
        board->doMove(opponentsMove, opponentSide);

    }
    std::vector<Move> * moves = getPossibleMoves(playerSide, board);
    for (int i=0; i<moves->size(); i++) {
        Move * playerMove = &moves->at(i);
        std::cerr << playerMove->getX() << ", " << playerMove->getY() << endl;
    }
    if (!moves->empty()) {
        boardtest(board);
        Move * playerMove = &moves->at(moves->size() -1);
        board->doMove(playerMove, playerSide);
        boardtest(board);
        return playerMove;
    }
    return NULL;
}


/**
 * Returns a vector to get a list of possible moves, given the board
 * (different boards facilitate board trees for minimax) 
 */
std::vector<Move> * Player::getPossibleMoves(Side side, Board * testBoard) {
    std::vector<Move> * moves = new std::vector<Move>();
    for (int x=0; x<BOARD_SIZE; x++) {
        for (int y=0; y<BOARD_SIZE; y++) {
            Move temp = Move(x,y);
            if (testBoard->checkMove(&temp, side)) {
                moves->push_back(temp);
            }
        }
    }
    return moves;
}
