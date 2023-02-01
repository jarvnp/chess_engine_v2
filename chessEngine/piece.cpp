#include "piece.h"

Piece::Piece(int8_t piece, int8_t pieceIndex):piece_(piece), pieceIndex_(pieceIndex)
{}

Piece::Piece(Piece &piece)
{
    Piece(piece.getPiece(), piece.getPieceIndex());
}

Piece::Piece(){}

Piece &Piece::operator=(Piece &piece)
{
    piece_ = piece.getPiece();
    pieceIndex_ = piece.getPieceIndex();
    return *this;
}

bool Piece::color()
{
    return piece_/DIFFERENCE_BETWEEN_COLORS;
}

void Piece::setPiece(int8_t piece, int8_t color)
{
    piece_ = piece+DIFFERENCE_BETWEEN_COLORS*color;
}

void Piece::setPieceIndex(int8_t pieceIndex)
{
    pieceIndex_ = pieceIndex;
}

int8_t Piece::getPiece()
{
    return piece_;
}

int8_t Piece::getPieceIndex()
{
    return pieceIndex_;
}

int8_t Piece::getPieceType()
{
    return piece_%DIFFERENCE_BETWEEN_COLORS;
}

