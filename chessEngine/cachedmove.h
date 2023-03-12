#ifndef CACHEDMOVE_H
#define CACHEDMOVE_H

#include "move.h"
#include <stdint.h>
#include <set>

class CachedPosition;



class CachedMove
{
public:
    CachedMove();
    CachedMove(Move move, int16_t scoreEstimate);

    //set scoreEstimate for this move
    void setScore(int16_t scoreEstimate);

    //get scoreEstimage for this move
    int16_t getScore()const;

    Move move_;

    mutable CachedPosition* nextCache_ = nullptr;

    bool operator<(const CachedMove& a)const;

private:
    int16_t scoreEstimate_;

    //this is used to decide compare-function's output when the score of two moves is equal
    uint8_t randomness_ = rand()%256;

};



#endif // CACHEDMOVE_H
