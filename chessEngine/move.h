#ifndef MOVE_H
#define MOVE_H

#include "piece.h"
#include "boardpoint.h"
#include <stdint.h>

struct Move{
    int8_t fromX;
    int8_t fromY;
    int8_t toX;
    int8_t toY;
    int8_t promotionTo;
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
