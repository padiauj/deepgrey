#include "player.h"
#include <iostream>
#include <vector>
#include <ctime> // for timing
#include <cstdlib>
#include <unistd.h>

#define BOARD_SIZE 8
#define EPSILON_TIME 15 

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
    Side opSide = (side == BLACK) ? WHITE : BLACK;
    int difference_score = (float)getScore(side, testboard)/ 
        (float) ( testboard->count(side)+testboard->count(opSide)) ;
    
    std::vector<Move> * pmoves = getPossibleMoves(side, testboard);
    std::vector<Move> * omoves = getPossibleMoves(opSide, testboard);
    int pmoves_num = pmoves->size();
    int omoves_num = omoves->size();
    int num_moves;
    if (pmoves_num + omoves_num == 0) {
        num_moves = 0;
    }
    num_moves = (float)(pmoves_num - omoves_num)/(float)(pmoves_num + omoves_num);

    return (100*(num_moves + difference_score));

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

Move * just_valid(Side playerSide, Board * board) {
    std::vector<Move> * moves = getPossibleMoves(playerSide, board);
    if (!moves->empty()) {
        int rand_idx = rand() % (moves->size());
        Move * playerMove = &moves->at(rand_idx);
        return playerMove;
    }
    return NULL;

}

Move * just_heuristic(Side playerSide, Board * board) {
    std::vector<Move> * moves = getPossibleMoves(playerSide, board);
    if (!moves->empty()) {
        // search for move with best score
        int best_score = BOARD_SIZE * BOARD_SIZE;
        Move * best_move;

        for (int i=0; i<moves->size(); i++) {
            Board * tempBoard  = board->copy();
            Move * tempMove = &moves->at(i);

            tempBoard->doMove(tempMove, playerSide);
            int score = getHeuristic(playerSide, tempBoard);
            tempMove->setScore(score);

            if (score > best_score) {
                best_score = score;
                best_move = tempMove;
            }
            delete tempBoard;
        }

        return best_move;
    }
    return NULL;

}


int minplay(Side playerSide, Board * board) {
        std::vector<Move> * moves = getPossibleMoves(playerSide, board);
        if (!moves->empty()) {
            Move * best_move = &moves->at(0);
            for (int i=0; i<moves->size(); i++) {
                // test the move out
                Board * tempBoard  = board->copy();
                Move * tempMove = &moves->at(i);
                tempBoard->doMove(tempMove, playerSide);

                tempMove->setScore(getScore(playerSide, board));
                if (tempMove->getScore() < best_move->getScore()) {
                    best_move = tempMove;
                }
            }
        }
        return getScore(playerSide, board);
}

/**
 * The minimax algorithm (high score is better) as described in Assignment 9
 */
Move * minimax(Side playerSide, Board * board) {
        std::vector<Move> * moves = getPossibleMoves(playerSide, board);
        if (!moves->empty()) {
            Move * best_move = &moves->at(0);
            for (int i=0; i<moves->size(); i++) {
                // test the move out
                Board * tempBoard  = board->copy();
                Move * tempMove = &moves->at(i);
                tempBoard->doMove(tempMove, playerSide);

                tempMove->setScore(minplay(playerSide, board));
                if (tempMove->getScore() > best_move->getScore()) {
                    best_move = tempMove;
                }
            }
            return best_move;
        }
        return NULL;
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
    /* uncomment, recomment  for various levels of AI (valid moves, minimax,
       heuristic to beat SimplePlayer)
       */

 //     Move * playerMove = just_valid(playerSide, board);
      Move * playerMove = just_heuristic(playerSide, board);
//    Move * playerMove = minimax(playerSide, board);
    if (playerMove != NULL) {
        board->doMove(playerMove, playerSide);
    }

    return playerMove;
}

