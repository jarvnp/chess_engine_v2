// https://www.chessprogramming.org/Zobrist_Hashing

#include "hasher.h"
#include "piece.h"
#include "boardpoint.h"
#include "constants.h"

#include <iostream>
#include <set>

const int8_t pieces[] = {PAWN,KNIGHT,BISHOP,ROOK,QUEEN,KING,EN_PASSANT_PAWN};

Hasher::Hasher(){
    hash = 0;
    for(int8_t i=0; i<8; i++){
        for(int8_t j=0; j<8; j++){
            BoardPoint point;
            point = {i,j};
            for(int8_t piece : pieces){
                this->pieceHashes[piece][point.getRaw()] = ((uint64_t)rand()) | ((uint64_t)rand() << 32);
                this->pieceHashes[piece + DIFFERENCE_BETWEEN_COLORS][point.getRaw()] = ((uint64_t)rand()) | ((uint64_t)rand() << 32);
            } 
        }
    }
    for(int i=0; i<CASTLING_HASHES_N; i++){
        this->castlingHashes[i] = ((uint64_t)rand()) | ((uint64_t)rand() << 32);
    }
    blackMoveHash = ((uint64_t)rand()) | ((uint64_t)rand() << 32);
}

void Hasher::makeAMove(moveBackupData& move){
    uint64_t origHash = hash;
    int8_t movedColor = move.movedPiece.color();
    hash ^= blackMoveHash;

    int8_t colorOffset = movedColor == WHITE ? 0 : DIFFERENCE_BETWEEN_COLORS;
    int8_t opponentColorOffset = movedColor == BLACK ? 0 : DIFFERENCE_BETWEEN_COLORS;

    //put spawned en_passant pawn
    if((move.movedPiece.getPieceType() == PAWN) && (abs(move.move.to.x() - move.move.from.x()) == 2)){
        int8_t dir = movedColor*2-1;     //-1 or 1, depending on color
        BoardPoint point;
        point = {move.move.to.x() - dir,move.move.to.y()};
        hash ^= pieceHashes[EN_PASSANT_PAWN + colorOffset][point.getRaw()];
    }

    //remove old enpassant
    if(move.removedEnPassant.isNotOnBoard() == false){
        hash ^= pieceHashes[EN_PASSANT_PAWN + opponentColorOffset][move.removedEnPassant.getRaw()];
    }

    //udpate castling info
    hash ^= castlingHashes[move.oldCastlingInfo];
    hash ^= castlingHashes[move.newCastlingInfo];

    //update rook position if castling was done
    if(move.movedPiece.getPieceType() == KING && (abs(move.move.from.y() - move.move.to.y()) > 1)){
        BoardPoint rookFrom;
        BoardPoint rookTo;
        if(move.move.to.y() > move.move.from.y()){
            //kingside castling
            rookFrom = {move.move.from.x(),7};
            rookTo = {move.move.from.x(),5};
        }
        else{
            //queenside
            rookFrom = {move.move.from.x(),0};
            rookTo = {move.move.from.x(),3};
        }
        hash ^= pieceHashes[ROOK + colorOffset][rookFrom.getRaw()];
        hash ^= pieceHashes[ROOK + colorOffset][rookTo.getRaw()];
    }

    //put moved piece to its end location
    Piece finalPiece = Piece(move.movedPiece);
    if(move.move.promotionTo_ != EMPTY){
        finalPiece.setPiece(move.move.promotionTo_,movedColor);
    }
    hash ^= pieceHashes[finalPiece.getPiece()][move.move.to.getRaw()];

    //Remove moved piece from its start location
    hash ^= pieceHashes[move.movedPiece.getPiece()][move.move.from.getRaw()];

    bool enPassantCaptured = false;
    //removed pawn in case of en_passant capture
    if( (move.movedPiece.getPieceType() == PAWN) && (move.capturedPiece.getPieceType() == EN_PASSANT_PAWN)){
        enPassantCaptured = true;
        BoardPoint point;
        point = {move.move.from.x(),move.move.to.y()};
        hash ^= pieceHashes[PAWN + opponentColorOffset][point.getRaw()];
    }

    // Remove captured piece
    if(move.capturedPiece.getPieceType() != EMPTY){
        hash ^= pieceHashes[move.capturedPiece.getPiece()][move.move.to.getRaw()];
    }

    move.hashChange = hash ^ origHash;
}

void Hasher::reverseAMove(uint64_t hashChange){
    hash ^= hashChange;
}
