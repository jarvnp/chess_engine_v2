#ifndef BOARD_H
#define BOARD_H


#include "piece.h"
#include "boardpoint.h"
#include "cachedposition.h"
#include "move.h"
#include "constants.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;



class Board
{
public:
    Board(string fen = START_POSITION_FEN);

    //prints the content of the board
    void printBoard(ostream& stream);

    //makes a move if it is legal
    bool makeMoveIfAllowed(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo = EMPTY);

    //convert string move to movedata and make move if allowed
    bool userMakeMoveIfAllowed(string move);

    //check if a move is allowed
    bool isAllowedMove(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo = EMPTY);

    //search for best move from this board position
    void searchForMove(int8_t depth);

    string boardFen();

    //for debugging purposes
    uint32_t nodes = 0;
    vector<Move> movesMade;
    uint64_t time1 = 0;
    uint64_t time2 = 0;


private:
    Piece board_[8][8]; //8x8 array,   x,y, where x is vertical (1...8) and y is horizontal (a...h)  :)

     //an array with information about every pieces' location. For example pieceLocations_[0][0] has information about where one white player's pawn is
    //2 players each with 16 pieces + space for en passant pawn for both players
    BoardPoint pieceLocations_[2][17];

    bool castlingIsPossible_[2][2];         // {white queenside, white kingside}{black queenside, black kingside}

    int16_t boardscore_;
    int8_t turn_; //whose turn it is

    //search for a move with alpha-beta pruning and a cache to help with move ordering (use with iterative deepening)
    int16_t searchForMove(int8_t depth,int16_t alpha, int16_t beta, CachedPosition* cache);

    //init board from fen
    void fenToBoard(string fen);

    //initialize pieceLocations array to match with current board_ array
    void initPieceArray();

    //cheks whether a piece is same color as parameter "color"
    bool isOwn(int8_t x, int8_t y, int8_t color);

    //no check test in these
    bool isAllowedMoveForRook(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isAllowedMoveForBishop(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isAllowedMoveForKnight(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isAllowedMoveForPawn(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isAllowedMoveForQueen(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isAllowedMoveForKing(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);

    bool isAllowedMoveNoCheckTest(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);

    //Checks whether a point on board is checked
    //Explanation for parameter "color": if color = WHITE, then we check if any black piece can move to this position
    bool isChecked(int8_t x, int8_t y, int8_t color);

    //assumes that the move is legal, however checktest can be done after the move has been done
    //For example the program does this: makeAMove(...) --> check if king is under threat --> true --> move was illegal --> reverseAMove(...)
    moveBackupData makeAMove(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY,int8_t promotionTo = EMPTY);

    //reverses a made move
    void reverseAMove(moveBackupData& move);

    //updates castling info based on a move
    //return true if castling is done by this move
    bool updateCastlingInfo(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);

    //return score change for move
    int16_t calculateScoreChange(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, bool enPassantCaptured, bool castling, int8_t promotionTo);

    //return board score
    int16_t evaluateBoard();

    //finds all legal moves for a piece that's index in pieceLocation-array is "index"
    void findLegalMovesForIndex(vector<Move>& moves, uint8_t index);

    void findLegalMovesForPiece(vector<Move>& moves, int8_t x, int8_t y);

    void findLegalMovesPawn(vector<Move>& moves, int8_t x, int8_t y);
    void findLegalMovesRook(vector<Move>& moves, int8_t x, int8_t y);
    void findLegalMovesBishop(vector<Move>& moves, int8_t x, int8_t y);
    void findLegalMovesKnight(vector<Move>& moves, int8_t x, int8_t y);
    void findLegalMovesQueen(vector<Move>& moves, int8_t x, int8_t y);
    void findLegalMovesKing(vector<Move>& moves, int8_t x, int8_t y);



    //returns true if a is better or equal compared to b, from perspective of player's color
    bool isBetterOrEqScore(int16_t a, int16_t b, int8_t color);

};

#endif // BOARD_H
