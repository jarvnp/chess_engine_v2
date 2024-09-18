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
    Move(std::string moveStr);

    bool isOkInit();

    BoardPoint from;
    BoardPoint to;
    int8_t promotionTo_; // TODO is not clear if this is the piece type or does this include the color? (currently I think this does not include the color)
private:
    bool isOkInit_ = true;
};


struct moveBackupData{  //information needed to reverse a move
    Move move;
    Piece capturedPiece;
    Piece movedPiece; // The original moved piece (not promoted)
    BoardPoint removedEnPassant;
    int16_t scoreChange;
    int8_t oldCastlingInfo;
    int8_t newCastlingInfo; // TODO these must be formatted correctly (using 4 bits)
    uint64_t hashChange;
};

#endif // MOVE_H
