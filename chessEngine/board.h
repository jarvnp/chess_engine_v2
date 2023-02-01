#ifndef BOARD_H
#define BOARD_H


#include "piece.h"
#include "boardpoint.h"
#include <string>
#include <vector>
#include <iostream>
using namespace std;


#define WORST_SCORE_FOR_WHITE -30000
#define CHECKMATE_SCORE_FOR_WHITE (WORST_SCORE_FOR_WHITE+100)

struct moveData{
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






const int16_t PIECE_VALUES[6] = {100,320,330,500,900,0}; //pawn,knight,bishop,rook,queen,king

const int16_t PIECE_SQUARE_VALUES[6][64] = {
  {
      // pawn
   0,  0,  0,  0,  0,  0,  0,  0,
  50, 50, 50, 50, 50, 50, 50, 50,
  10, 10, 20, 30, 30, 20, 10, 10,
   5,  5, 10, 25, 25, 10,  5,  5,
   0,  0,  0, 20, 20,  0,  0,  0,
   5, -5,-10,  0,  0,-10, -5,  5,
   5, 10, 10,-20,-20, 10, 10,  5,
   0,  0,  0,  0,  0,  0,  0,  0
 },
 {
   // knight
-50,-40,-30,-30,-30,-30,-40,-50,
-40,-20,  0,  0,  0,  0,-20,-40,
-30,  0, 10, 15, 15, 10,  0,-30,
-30,  5, 15, 20, 20, 15,  5,-30,
-30,  0, 15, 20, 20, 15,  0,-30,
-30,  5, 10, 15, 15, 10,  5,-30,
-40,-20,  0,  5,  5,  0,-20,-40,
-50,-40,-30,-30,-30,-30,-40,-50
},
 {
   // bishop
-20,-10,-10,-10,-10,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5, 10, 10,  5,  0,-10,
-10,  5,  5, 10, 10,  5,  5,-10,
-10,  0, 10, 10, 10, 10,  0,-10,
-10, 10, 10, 10, 10, 10, 10,-10,
-10,  5,  0,  0,  0,  0,  5,-10,
-20,-10,-10,-10,-10,-10,-10,-20,
},
{
  //rook
  0,  0,  0,  0,  0,  0,  0,  0,
  5, 10, 10, 10, 10, 10, 10,  5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
 -5,  0,  0,  0,  0,  0,  0, -5,
  0,  0,  0,  5,  5,  0,  0,  0
},
{
  //queen
-20,-10,-10, -5, -5,-10,-10,-20,
-10,  0,  0,  0,  0,  0,  0,-10,
-10,  0,  5,  5,  5,  5,  0,-10,
 -5,  0,  5,  5,  5,  5,  0, -5,
  0,  0,  5,  5,  5,  5,  0, -5,
-10,  5,  5,  5,  5,  5,  0,-10,
-10,  0,  5,  0,  0,  0,  0,-10,
-20,-10,-10, -5, -5,-10,-10,-20
},
{
//  king middle game
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-30,-40,-40,-50,-50,-40,-40,-30,
-20,-30,-30,-40,-40,-30,-30,-20,
-10,-20,-20,-20,-20,-20,-20,-10,
 20, 20,  0,  0,  0,  0, 20, 20,
 20, 30, 10,  0,  0, 10, 30, 20
}
};



#define KINGSIDE_CASTLING 1
#define QUEENSIDE_CASTLING 0




const string START_POSITION_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";




class Board
{
public:
    Board(string fen = START_POSITION_FEN);
    void printBoard(ostream& stream);
    bool makeMoveIfAllowed(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo = EMPTY);
    bool userMakeMoveIfAllowed(string move);

    bool isAllowedMove(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo = EMPTY);



    int16_t searchForMove(int8_t depth,int32_t alpha, int32_t beta, bool isInitialCall = false);
    int16_t searchForMove(int8_t depth,bool isInitialCall = false);


    string boardFen();

    //for debugging purposes
    uint32_t nodes = 0;
    vector<moveData> movesMade;




private:
    Piece board_[8][8]; //8x8 array,   x,y, where x is vertical (1...8) and y is horizontal (a...h)  :)

     //an array with information about every pieces' location. For example pieceLocations_[0][0] has information about where one white player's pawn is
    BoardPoint pieceLocations_[2][17];


    bool castlingIsPossible_[2][2];         // {white queenside, white kingside}{black queenside, black kingside}

    int16_t boardscore_;
    int8_t turn_; //whose turn it is
    moveData bestMove_;


    void fenToBoard(string fen);
    void initPieceArray();

    bool isOwn(int8_t x, int8_t y, int8_t color);

    //no check test in these
    bool isAllowedMoveForRook(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isAllowedMoveForBishop(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isAllowedMoveForKnight(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isAllowedMoveForPawn(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isAllowedMoveForQueen(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isAllowedMoveForKing(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);

    bool isAllowedMoveNoCheckTest(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);
    bool isChecked(int8_t x, int8_t y, int8_t color);

    //assumes that the move is legal (but checktest is done after move)
    moveBackupData makeAMove(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY,int8_t promotionTo = EMPTY);

    void reverseAMove(moveBackupData& move);

    bool updateCastlingInfo(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY);

    //return score change for move
    int16_t calculateScoreChange(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, bool enPassantCaptured, bool castling, int8_t promotionTo);

    //return board score
    int16_t evaluateBoard();

    //finds all legal moves from a position
    void findLegalMoves(vector<moveData>& moves);

    void findLegalMovesForPiece(vector<moveData>& moves, int8_t x, int8_t y);

    void findLegalMovesPawn(vector<moveData>& moves, int8_t x, int8_t y);
    void findLegalMovesRook(vector<moveData>& moves, int8_t x, int8_t y);
    void findLegalMovesBishop(vector<moveData>& moves, int8_t x, int8_t y);
    void findLegalMovesKnight(vector<moveData>& moves, int8_t x, int8_t y);
    void findLegalMovesQueen(vector<moveData>& moves, int8_t x, int8_t y);
    void findLegalMovesKing(vector<moveData>& moves, int8_t x, int8_t y);



    //returns true if a is better than b, from perspective of player's color
    bool isBetterScore(int16_t a, int16_t b, int8_t color);

};

#endif // BOARD_H
