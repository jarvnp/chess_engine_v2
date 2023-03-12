#ifndef CACHEDPOSITION_H
#define CACHEDPOSITION_H

#include "cachedmove.h"
#include "move.h"
#include <set>
#include <vector>

typedef std::multiset<CachedMove> Cache;


class CachedPosition
{
public:
    CachedPosition();

    ~CachedPosition();

    //return iterator to begin or end of Cache, depenging on whose turn it is
    Cache::iterator getBestMoveIt() const;

    //either ++it or --it, depending on color
    void advanceIt(Cache::iterator& it) const;

    //checks if it is valid (points to an object of cache)
    bool isEndIt(const Cache::iterator& it) const;

    //add moves to cache. The default value for score_estimate depends on color
    void initWithMoves(std::vector<Move>& moves, bool color);

    //update elements of cache pointed by the iterators with elements in the "moves" vector
    void updateScoreEstimates(std::vector<int16_t>& scores, std::vector<Cache::iterator>& iterators);

    bool isInitialized() const;

    bool isEmpty();



private:
    Cache moves_;
    bool isInitialized_ = false;
    bool color_;

};

#endif // CACHEDPOSITION_H
