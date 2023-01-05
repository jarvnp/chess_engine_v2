#ifndef PIECE_H
#define PIECE_H

#include <stdint.h>


#define EMPTY 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6
#define EN_PASSANT_PAWN 7     //I will implement en passant with a "ghost" pawn that can be eaten with a pawn and exists for one turn only


#define DIFFERENCE_BETWEEN_COLORS 10   //white pawn is 1, black is 10+1 = 11 and so on

#define KING_INDEX 15           //where king is found in pieces_-array
#define EN_PASSANT_INDEX 16


#define WHITE 0
#define BLACK 1



class Piece
{
public:
    Piece(int8_t piece, int8_t pieceIndex);
    Piece(Piece& piece);
    Piece();
    Piece& operator= (Piece& piece);


    bool color();
    void setPiece(int8_t piece, int8_t color = 0);      //if parameter color not given, then piece_ is set to piece
    void setPieceIndex(int8_t pieceIndex);
    int8_t getPiece();
    int8_t getPieceIndex();

    int8_t getPieceType();
private:
    int8_t piece_;     //What piece this is (PAWN, KNIGHT etc)
    int8_t pieceIndex_;    //index in pieces-array
};

#endif // PIECE_H
