#include "move.h"

Move::Move(){}

Move::Move(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo):
fromX_(fromX), fromY_(fromY), toX_(toX), toY_(toY), promotionTo_(promotionTo)
{}


Move::Move(const Move& move):
fromX_(move.fromX_), fromY_(move.fromY_), toX_(move.toX_), toY_(move.toY_), promotionTo_(move.promotionTo_)
{}


Move::Move(std::string moveStr){
  if(moveStr.size() < 4){
      isOkInit_ = false;
      return;
  }
  if(moveStr.size() > 5){
      isOkInit_ = false;
      return;
  }
  fromX_ = moveStr[1];
  fromY_ = moveStr[0];
  toX_ = moveStr[3];
  toY_ = moveStr[2];
  promotionTo_ = EMPTY;
  if(moveStr.size() > 4){
      promotionTo_ = moveStr[4];
  }

  fromX_ = fromX_ - '1';     //  tranform '1' - '8' to 0-7
  fromX_ = 7-fromX_;        //mirror
  toX_ = toX_ - '1';
  toX_ = 7-toX_;

  fromY_ = fromY_ - 'a';
  toY_ = toY_ - 'a';

  if(fromX_ < 0 || fromX_ > 7){
      isOkInit_ = false;
      return;
  }
  if(fromY_ < 0 || fromY_ > 7){
      isOkInit_ = false;
      return;
  }
  if(toX_ < 0 || toX_ > 7){
      isOkInit_ = false;
      return;
  }
  if(toY_ < 0 || toY_ > 7){
      isOkInit_ = false;
      return;
  }

  if(promotionTo_ != 0){
      if(promotionTo_ == 'q'){
          promotionTo_ = QUEEN;
      }
      else if(promotionTo_ == 'r'){
          promotionTo_ = ROOK;
      }
      else if(promotionTo_ == 'b'){
          promotionTo_ = BISHOP;
      }
      else if(promotionTo_ == 'k'){
          promotionTo_ = KNIGHT;
      }
      else{
        isOkInit_ = false;
      }
  }
}

bool Move::isOkInit(){
  return isOkInit_;
}
