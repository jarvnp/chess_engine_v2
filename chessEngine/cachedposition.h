#ifndef CACHEDPOSITION_H
#define CACHEDPOSITION_H

#include "cachedmove.h"
#include <set>
#include <vector>
#include <time.h>

//where we store the moves for each position
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

    //Sorts the Cache-vector (ascending or decending, depending on color_)
    void refreshOrder();

    void initColor(bool color);

    //how many pieces' legal moves have been fetched for this position
    uint8_t fetchedLegalMovesIndex_ = 0;

    //Legal moves are fethed in random order, determined by this seed
    uint32_t seed_ = time(NULL);

    Cache moves_;

private:

    bool isInitialized_ = false;
    bool color_;

};

#endif // CACHEDPOSITION_H
