#ifndef CACHEDPOSITION_H
#define CACHEDPOSITION_H

#include "cachedmove.h"
#include "move.h"
#include <set>
#include <vector>
#include <time.h>

typedef std::vector<CachedMove> Cache;


class CachedPosition
{
public:
    CachedPosition();

    ~CachedPosition();

    //return pointer to first element of moves_ (or nullptr if moves_ is empty)
    const CachedMove* getBestMovePtr() const;

    //returns true if all legal moves from this position are fetched
    bool isAllMovesFetched();

    bool isInitialized() const;

    bool isEmpty()const;

    void refreshOrder();

    void initColor(bool color);

    uint8_t fetchedLegalMovesIndex_ = 0;
    uint32_t seed_ = time(NULL);

    Cache moves_;

private:

    bool isInitialized_ = false;
    bool color_;

};

#endif // CACHEDPOSITION_H
