#include "cachedmove.h"

CachedMove::CachedMove()
{


}

CachedMove::CachedMove(Move move, int16_t scoreEstimate):
    move_(move)
{
    setScore(scoreEstimate);
}

void CachedMove::setScore(int16_t scoreEstimate)
{
   //scoreEstimate_ = (scoreEstimate/20)*20;
   scoreEstimate_ = scoreEstimate;
}

int16_t CachedMove::getScore() const
{
    return scoreEstimate_;
}

bool CachedMove::operator<(const CachedMove &a) const
{
    if(this->scoreEstimate_ == a.scoreEstimate_){
        return this->randomness_ < a.randomness_;
    }
    return this->scoreEstimate_ < a.scoreEstimate_;
}
