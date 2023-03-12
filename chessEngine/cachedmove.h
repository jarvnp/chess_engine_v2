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
    Move move_;

    uint8_t randomness_ = rand()%256;

    void setScore(int16_t scoreEstimate);
    int16_t getScore()const;

    mutable CachedPosition* nextCache_ = nullptr;

    bool operator<(const CachedMove& a)const;

private:
    int16_t scoreEstimate_;

};



#endif // CACHEDMOVE_H
