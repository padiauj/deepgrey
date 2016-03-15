#include "player.h"
#include <iostream>
#include <vector>
#include <ctime> // for timing
#include <cstdlib>
#include <unistd.h>
#include <limits>
#include <algorithm>

#define BOARD_SIZE 8
#define EPSILON_TIME 15 
#define MAX_DEPTH 10

/**
 * Prints current board state
 */

void printBoard(Board * testboard) {
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

/**
 * Returns a vector to get a list of possible moves, given the board
 * (different boards facilitate board trees for minimax) 
 */
std::vector<Move> * getPossibleMoves(Side side, Board * testBoard) {
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

/**
 * Gets the score (difference in number of white/black pieces on board)
 */
int getScore(Side side, Board * testboard) {
    Side opSide = (side == BLACK) ? WHITE : BLACK;
    return testboard->count(side)-testboard->count(opSide) ;
}

/**
 * Gets the heuristic: combination of respective captures, and mobility
 */
int getHeuristic(Side side, Board * testboard) {
    int score = 0;
    Side opSide = (side == BLACK) ? WHITE : BLACK;

    for (int x=0; x < BOARD_SIZE; x++) {
        for (int y=0; y <BOARD_SIZE; y++) {
            // check if this is a corner
            int temp = 0;
            if ((x == 0 || x == 7) && (y == 0 || y == 7)) {
                temp += 40;
            }
            if (x == 0 || x == 7 || y == 0 || y == 7) {
                temp += 10;
            }
            if (x == 1 || x == 6 || y == 1 || y == 6) {
                // take care of dangerous pitfalls
                temp -= 1;
                if (x == 1 && y == 1 && !testboard->get(side, 0,0)) {
                    temp -= 5;
                }
                if (x == 1 && y == 6 && !testboard->get(side, 0,7)) {
                    temp -= 5;
                }
                if (x == 6 && y == 1 && !testboard->get(side, 7,0)) {
                    temp -= 5;
                }
                if (x == 6 && y == 6 && !testboard->get(side, 7,7)) {
                    temp -= 5;
                }
            }
            if (testboard->get(side, x,y)) {
                score += temp;
            } else if(testboard->get(opSide, x,y)) {
                score += -1*temp;
            }
        }
    }
    return -1*score;

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
}

/*
 * Destructor for the player.
 */
Player::~Player() {
    delete board;
}

float max_value(Side side, Board * board, int depth, float alpha, float beta) {
    if (depth >= MAX_DEPTH) {
        return getHeuristic(side, board);
    }
    float best_score = numeric_limits<float>::infinity() * -1;
    Side opSide = (side == BLACK) ? WHITE : BLACK;
    std::vector<Move> * moves = getPossibleMoves(side, board);
    for (size_t i = 0; i < moves->size(); ++i) {
        Board * temp = board->copy();
        Move * tempMove = &moves->at(i);
        temp->doMove(tempMove, side);
        float score = min_value(opSide, temp, depth+1, alpha, beta);
        delete temp;
        best_score = max(best_score, score);
        if (best_score >= beta) {
            return best_score;
        } 
        beta = max(best_score, alpha);

    }
        return best_score;

}

float min_value(Side side, Board * board, int depth, float alpha, float beta) {
    if (depth >= MAX_DEPTH) {
        return getHeuristic(side, board);
    }
    float best_score = numeric_limits<float>::infinity();
    Side opSide = (side == BLACK) ? WHITE : BLACK;

    std::vector<Move> * moves = getPossibleMoves(side, board);
    for (size_t i = 0; i < moves->size(); ++i) {
        Board * temp = board->copy();
        Move * tempMove = &moves->at(i);
        temp->doMove(tempMove, side);
        float score = max_value(opSide, temp, depth+1, alpha, beta);
        delete temp;
        best_score = min(best_score, score);
        if (best_score <= beta) {
            return best_score;
        } 
        beta = min(best_score, beta);
    }
        return best_score;

}

Move * abpruning(Side side, Board * board, long msLeft) {
    int depth = 0;
    std::vector<Move> * moves = getPossibleMoves(side, board);
    if (moves->size() != 0) {
        Move * best = &moves->at(0);
        for (size_t i = 0; i < moves->size(); ++i) {
            Board * temp = board->copy();
            Move * tempMove = &moves->at(i);
            temp->doMove(tempMove, side);
            float score = min_value(side, temp, depth+1, -1*numeric_limits<float>::infinity(), numeric_limits<float>::infinity());
            tempMove->setScore(score);
            if (score > best->getScore()) {
                best = tempMove;
            }
        }
        return best;
    } else {
        return NULL;
    }
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

    Move * playerMove = abpruning(playerSide, board, msLeft);
    if (playerMove != NULL) {
        board->doMove(playerMove, playerSide);
    }

    return playerMove;
}

