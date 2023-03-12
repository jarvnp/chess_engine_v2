#include "cachedposition.h"
#include "constants.h"
#include <algorithm>

CachedPosition::CachedPosition()
{

}

CachedPosition::~CachedPosition()
{
    for(CachedMove move : moves_){
        if(move.nextCache_ != nullptr){
            delete move.nextCache_;
        }
        move.nextCache_ = nullptr;
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
