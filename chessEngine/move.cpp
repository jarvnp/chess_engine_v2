#include "move.h"

Move::Move(){}

Move::Move(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo):promotionTo_(promotionTo){
    from = {fromX,fromY};
    to = {toX,toY};
}





Move::Move(std::string moveStr){
    if(moveStr.size() < 4){
        isOkInit_ = false;
        return;
    }
    if(moveStr.size() > 5){
        isOkInit_ = false;
        return;
    }
    int8_t fromX, fromY,toX,toY;
    fromX = moveStr[1];
    fromY = moveStr[0];
    toX = moveStr[3];
    toY = moveStr[2];
    promotionTo_ = EMPTY;
    if(moveStr.size() > 4){
        promotionTo_ = moveStr[4];
    }

    fromX = fromX - '1';     //  tranform '1' - '8' to 0-7
    fromX = 7-fromX;        //mirror
    toX = toX - '1';
    toX = 7-toX;

    fromY = fromY - 'a';
    toY = toY - 'a';

    if(fromX < 0 || fromX > 7){
        isOkInit_ = false;
        return;
    }
    if(fromY < 0 || fromY > 7){
        isOkInit_ = false;
        return;
    }
    if(toX < 0 || toX > 7){
        isOkInit_ = false;
        return;
    }
    if(toY < 0 || toY > 7){
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

    from = {fromX,fromY};
    to = {toX,toY};
}

bool Move::isOkInit(){
  return isOkInit_;
}
