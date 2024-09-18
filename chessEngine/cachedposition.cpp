#include "cachedposition.h"
#include "constants.h"
#include <algorithm>
#include <unordered_set>

CachedPosition::CachedPosition()
{

}

void collectDeletePointers(std::unordered_set<CachedMove*>& pointers, CachedPosition* pos){
    for(CachedMove move : pos->moves_){
        if(move.nextCache_ != nullptr){
            pointers.insert((CachedMove*)move.nextCache_);
            collectDeletePointers(pointers,move.nextCache_);
        }
    }
}

CachedPosition::~CachedPosition()
{
    std::unordered_set<CachedMove*> pointers;
    collectDeletePointers(pointers,this);
    for(auto pointer : pointers){
        delete pointer;
    }
}



const CachedMove* CachedPosition::getBestMovePtr() const
{
    if(this->isEmpty()){
        return nullptr;
    }
    return &moves_.front();
}

bool CachedPosition::isAllMovesFetched()
{
    return fetchedLegalMovesIndex_ >= 16;
}



bool CachedPosition::isInitialized()const
{
    return isInitialized_;
}

bool CachedPosition::isEmpty()const
{
    return moves_.empty();
}

void CachedPosition::refreshOrder()
{
    if(color_ == BLACK){
        sort(moves_.begin(),moves_.end());
    }else{
        sort(moves_.rbegin(), moves_.rend());
    }
}

void CachedPosition::initColor(bool color)
{
    color_ = color;
    isInitialized_ = true;
}
