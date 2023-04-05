#ifndef MOVE_H
#define MOVE_H

#include "piece.h"
#include "boardpoint.h"
#include <stdint.h>
#include <string>

class Move
{
public:
    Move();
    Move(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo);
    Move(const Move& move);
    Move(std::string moveStr);

    bool isOkInit();

    int8_t fromX_;
    int8_t fromY_;
    int8_t toX_;
    int8_t toY_;
    int8_t promotionTo_;
private:
    bool isOkInit_ = true;
};


struct moveBackupData{  //information needed to reverse a move
    int8_t fromX;
    int8_t fromY;
    int8_t toX;
    int8_t toY;
    bool isPromoted;
    Piece capturedPiece;
    BoardPoint removedEnPassant;
    int16_t scoreChange;
    int8_t castlingInfo;
};

#endif // MOVE_H
