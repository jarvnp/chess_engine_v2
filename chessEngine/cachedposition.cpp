#include "cachedposition.h"
#include "constants.h"

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

Cache::iterator CachedPosition::getBestMoveIt() const
{
    if(moves_.size() == 0){
        return moves_.end();
    }
    if(color_ == BLACK){
        return moves_.begin();
    }
    return --(moves_.end());
}

void CachedPosition::advanceIt(Cache::iterator& it)const
{
    if(color_ == BLACK){
        it++;
        return;
    }
    if(it == moves_.begin()){
        it = moves_.end();
    }
    else{
        it--;
    }
}

bool CachedPosition::isEndIt(const Cache::iterator& it)const
{
    if(it == moves_.end()){
        return true;
    }

    return false;
}

void CachedPosition::initWithMoves(std::vector<Move>& moves, bool color)
{
    isInitialized_ = true;
    color_ = color;
    for(Move move : moves){
        int16_t scoreEstimate = WORST_SCORE_FOR_WHITE;
        if(color == BLACK){
            scoreEstimate *= -1;
        }
        moves_.insert(CachedMove(move,scoreEstimate));
    }
}

void CachedPosition::updateScoreEstimates(std::vector<int16_t> &scores, std::vector<Cache::iterator> &iterators)
{
    std::vector<CachedMove> updatedMoves;
    for(std::vector<int16_t>::size_type i = 0; i<scores.size(); i++){
        CachedMove updated = *iterators[i];
        updated.setScore(scores[i]);
        moves_.erase(iterators[i]);
        updatedMoves.push_back(updated);
    }

    int16_t worstScore = WORST_SCORE_FOR_WHITE;
    if(color_ == BLACK){
        worstScore *= -1;
    }
    for(Cache::iterator it = moves_.begin(); it != moves_.end(); it++){
        CachedMove updated = *it;
        updated.setScore(worstScore);
        updated.randomness_ = rand()&256;
        /*int16_t score = updated.getScore();
        if(color_ == WHITE){
            score -= 50;
        }
        else{
            score += 50;
        }
        updated.setScore(score);*/
        updatedMoves.push_back(updated);
    }
    moves_.clear();
    for(std::vector<CachedMove>::size_type i = 0; i<updatedMoves.size(); i++){
        moves_.insert(updatedMoves[i]);
    }
}


bool CachedPosition::isInitialized()const
{
    return isInitialized_;
}

bool CachedPosition::isEmpty()
{
    return moves_.empty();
}
