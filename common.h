#ifndef __COMMON_H__
#define __COMMON_H__

enum Side { 
    WHITE, BLACK
};

class Move {
   
public:
    int x, y, score;
    Move(int x, int y) {
        this->x = x;
        this->y = y;        
        this->score = 65; // higher than any score possible (33?)       
    }
    ~Move() {}

    int getX() { return x; }
    int getY() { return y; }
    int getScore() { return score; }

    void setX(int x) { this->x = x; }
    void setY(int y) { this->y = y; }
    void setScore(int score) { this->score = score; }
};

#endif
