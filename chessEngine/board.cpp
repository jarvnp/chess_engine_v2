#include "board.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <random>
#include <time.h>

//operator for printing moves
ostream& operator<<(ostream& stream, const Move& move)
{
    char fromX = (8-move.fromX_)+'0';
    char fromY = move.fromY_+'a';
    char toX = (8-move.toX_)+'0';
    char toY = move.toY_+'a';
    char promotion = 0;
    if(move.promotionTo_ != EMPTY){
        switch(move.promotionTo_){
        case QUEEN:
            promotion = 'q';
            break;
        case ROOK:
            promotion = 'r';
            break;
        case KNIGHT:
            promotion = 'n';
            break;
        case BISHOP:
            promotion = 'b';
            break;
        }
    }
    stream << fromY << fromX << toY << toX;
    if(promotion != 0){
      stream << promotion;
    }
    return stream;
}



Board::Board(string fen)
{
    fenToBoard(fen);
    boardscore_ = evaluateBoard();
}

void Board::printBoard(ostream &stream)
{

    stream << "Moves made: ";
    for(Move move: movesMade){
        stream << move << " ";
    }
    cout << "\n";
    stream << castlingIsPossible_[0][0] << " " << castlingIsPossible_[0][1] << " " << castlingIsPossible_[1][0] << " " << castlingIsPossible_[1][1] << "\n";
    stream << "turn: " << turn_*1 << "\n";
    stream << boardFen() << "\n";

    stream << "\t";
    for(char i = 'a'; i<'i'; i++){
        stream << i << "\t";
    }
    stream << endl;
    for(int8_t i=0; i<10; i++){
        stream  << "\t";
    }
    stream << endl;
    for(int8_t i=0; i<8; i++){
        stream << 8-i << "\t";
        for(int8_t i2=0; i2<8; i2++){
            stream << board_[i][i2].getPiece()*1 << "\t";
        }
        stream << "\n";
    }
    stream << endl;
    stream << boardscore_ << " " << evaluateBoard() << "\n";

    vector<Move> moves;
}

bool Board::makeMoveIfAllowed(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo)
{
    if(!isAllowedMoveNoCheckTest(fromX, fromY, toX, toY)){
        return false;
    }
    int8_t color = board_[fromX][fromY].color();
    moveBackupData moveBackup = makeAMove(fromX, fromY, toX, toY, promotionTo);
    if(isChecked(pieceLocations_[color][KING_INDEX].x(), pieceLocations_[color][KING_INDEX].y(),color)){
        reverseAMove(moveBackup);
        return false;
    }
    return true;
}

bool Board::userMakeMoveIfAllowed(string moveStr)
{
    Move move(moveStr);
    if(!move.isOkInit()){
      return false;
    }

    return makeMoveIfAllowed(move.fromX_, move.fromY_, move.toX_, move.toY_,move.promotionTo_);

}



int16_t Board::searchForMove(int8_t depth,int16_t alpha, int16_t beta, CachedPosition* cache, time_t& endTime)
{
    //no need to check time every time
    if(nodes%1000 == 0){
        if(difftime(time(nullptr), endTime) > 0 ){
            return 0;       //return value doesn't matter, we will discard everything from an unfinished search
        }
    }
    nodes++;

    if(depth == 0){
        return boardscore_;
    }

    if(!cache->isInitialized()){
        cache->initColor(turn_);
    }



    //for debugging
    /*if(boardscore_ != evaluateBoard()){
        cout << "internal error\n";
        printBoard(cout);
        while(1);
    }*/


    int16_t bestScore = WORST_SCORE_FOR_WHITE;
    if(turn_ == BLACK){
        bestScore *= -1;
    }


    Cache::size_type index = 0;
    uint8_t indeces[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
    shuffle(indeces, indeces+16,std::default_random_engine(cache->seed_));

    do{

        if( (!cache->isAllMovesFetched()) && (index >= cache->moves_.size())){
            vector<Move> moves;
            findLegalMovesForIndex(moves, indeces[cache->fetchedLegalMovesIndex_]);
            cache->fetchedLegalMovesIndex_++;
            for(auto move : moves){
                int16_t scoreEstimate = WORST_SCORE_FOR_WHITE;
                if(turn_ == BLACK){
                    scoreEstimate *= -1;
                }
                cache->moves_.push_back(CachedMove(move,scoreEstimate));
            }
        }

        for(; index < cache->moves_.size(); index++){



            moveBackupData backup = makeAMove(
                        cache->moves_[index].move_.fromX_, cache->moves_[index].move_.fromY_,
                        cache->moves_[index].move_.toX_,cache->moves_[index].move_.toY_,cache->moves_[index].move_.promotionTo_);
            if(cache->moves_[index].nextCache_ == nullptr){
                cache->moves_[index].nextCache_ = new CachedPosition();
            }
            int16_t temp = searchForMove(depth-1, beta, alpha, cache->moves_[index].nextCache_, endTime);
            reverseAMove(backup);

           cache->moves_[index].setScore(temp);

            if(isBetterOrEqScore(temp,bestScore,turn_)){
                bestScore = temp;
            }
            //Alpha is always the max score that the opponent player is for sure able to achieve.
            //The opponent player won't allow this position, if better score (worse for the opponent) is achievable.
            if(isBetterOrEqScore(temp,alpha,turn_)){
                cache->refreshOrder();

                //We have found a move whose score is equal to the score that the opponent player is guaranteed to get.
                //We don't need to continue searching for moves, since the score can get worse, (for opponent), in which case the opponent won't allow this position to happen.
                //However we cannot return this value, because then earlier stages of the search tree won't be able to differentiate between this position and
                // the actual position, where alpha is guaranteed for the opponent.
                //We add or substract 1 from the score, to make it a bit worse (for the opponent)
                if(temp == alpha){
                    if(turn_ == WHITE){
                        return temp+1;
                    }
                    return temp-1;
                }
                return temp;
            }


            if(!isBetterOrEqScore(beta,temp, turn_)){
                beta = temp;
            }


        }



    }while(cache->fetchedLegalMovesIndex_ < 16);

    //if no legal moves were found
    if(cache->isEmpty()){

        //checkmate
        if(isChecked(pieceLocations_[turn_][KING_INDEX].x(), pieceLocations_[turn_][KING_INDEX].y(), turn_)){
            //checkmatescore must be different compared to WORST_SCORE_FOR_WHITE. If it's equal,
            //checkmate is not recognized properly in situations where checkmate is inevitable, but there is still some legal move to be made
            //depth is substacted from checkmate score to ensure that if checkmate is possible in for example 2 moves and 3 moves, the program aims for 2 move mate
            return (turn_ == WHITE) ? (CHECKMATE_SCORE_FOR_WHITE-depth) : -1*(CHECKMATE_SCORE_FOR_WHITE-depth);
        }

        //if king isn't checked, it's stalemate
        return 0;
    }
    cache->refreshOrder();
    return bestScore;
}

void Board::searchForMove(uint32_t maxTimeSeconds)
{
    auto start = std::chrono::high_resolution_clock::now();

    std::time_t startTime = std::time(nullptr);
    std::time_t endTime = startTime+ maxTimeSeconds;

    nodes = 0;
    int16_t rootValue = WORST_SCORE_FOR_WHITE;
    if(turn_ == WHITE){
        rootValue *= -1;
    }
    CachedPosition root = CachedPosition();
    Move bestMove;
    for(int8_t i=1; std::difftime(time(nullptr),endTime) <= 0; i++){
        if(root.getBestMovePtr() != nullptr){
           bestMove = root.getBestMovePtr()->move_;
        }
        searchForMove(i, rootValue,-rootValue, &root, endTime);



    }
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    uint32_t micros = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

    cout << bestMove;


}

string Board::boardFen(){
  string fen = "";
  char betweenRanks = '/';
  for(int8_t i=0; i<8; i++){
    int8_t emptyCounter = 0;
    if(i != 0){
        fen += (char)betweenRanks;
    }
    for(int8_t i2=0; i2<8; i2++){
      if(board_[i][i2].getPieceType() != EMPTY && board_[i][i2].getPieceType()  != EN_PASSANT_PAWN && emptyCounter != 0){
        fen += '0'+emptyCounter;
        emptyCounter = 0;
      }

      if(board_[i][i2].getPieceType() == PAWN){
     //32 difference between small and capital letters in ascii
        fen += (char)('p' - 32*(!board_[i][i2].color() ));
      }
      else if(board_[i][i2].getPieceType() == ROOK){
        fen += (char)('r' - 32*(!board_[i][i2].color() ));
      }
      else if(board_[i][i2].getPieceType() == BISHOP){
        fen += (char)('b' - 32*(!board_[i][i2].color() ));
      }
      else if(board_[i][i2].getPieceType() == KNIGHT){
        fen += (char)('n' - 32*(!board_[i][i2].color() ));
      }
      else if(board_[i][i2].getPieceType() == QUEEN){
        fen += (char)('q' - 32*(!board_[i][i2].color() ));
      }
      else if(board_[i][i2].getPieceType() == KING){
        fen += (char)('k' - 32*(!board_[i][i2].color() ));
      }
      else{
        emptyCounter++;
      }
    }
    if(emptyCounter != 0){
      fen += '0'+emptyCounter;
    }
  }
  if(turn_){
    fen += " b";
  }
  else{
    fen += " w";
  }
  bool castling = 0;
  fen += " ";
  if(castlingIsPossible_[0][1]){
    castling = 1;
    fen += "K";
  }
  if(castlingIsPossible_[0][0]){
    castling = 1;
    fen += "Q";

  }
  if(castlingIsPossible_[1][1]){
    castling = 1;
    fen += "k";
  }
  if(castlingIsPossible_[1][0]){
    castling = 1;
    fen += "q";
  }
  if(!castling){
    fen += "-";
  }
  if(pieceLocations_[!turn_][EN_PASSANT_INDEX].isNotOnBoard() == false){
    fen += " ";
    fen += (char)('a' + pieceLocations_[!turn_][EN_PASSANT_INDEX].y());
    fen += '0'+(8-pieceLocations_[!turn_][EN_PASSANT_INDEX].x());
  }
  else{
    fen += " -";
  }
  fen += " 0 0";
  return fen;
}

bool Board::isAllowedMove(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo)
{


    if(board_[fromX][fromY].color() != turn_){
        return false;
    }
    if(!isAllowedMoveNoCheckTest(fromX, fromY, toX, toY)){
        return false;
    }
    int8_t color = board_[fromX][fromY].color();

    moveBackupData moveBackup = makeAMove(fromX, fromY, toX, toY, promotionTo);
    if(isChecked(pieceLocations_[color][KING_INDEX].x(), pieceLocations_[color][KING_INDEX].y(),color)){
        reverseAMove(moveBackup);
        return false;
    }
    reverseAMove(moveBackup);
    return true;
}



void Board::fenToBoard(string fen)
{
    int8_t index = 0;

    //Firstly FEN has information about piece locations
    for(int8_t i=0; i<8; i++){
        for(int8_t i2=0; i2<8; i2++){
          if(fen[index] > '9'){
            if(fen[index] == 'p' || fen[index] == 'P'){
              board_[i][i2].setPiece(PAWN, fen[index] == 'p');
            }
            else if(fen[index] == 'n' || fen[index] == 'N'){
              board_[i][i2].setPiece(KNIGHT,fen[index] == 'n');
            }
            else if(fen[index] == 'b' || fen[index] == 'B'){
              board_[i][i2].setPiece(BISHOP, fen[index] == 'b');
            }
            else if(fen[index] == 'r' || fen[index] == 'R'){
              board_[i][i2].setPiece(ROOK, fen[index] == 'r');
            }
            else if(fen[index] == 'q' || fen[index] == 'Q'){
              board_[i][i2].setPiece(QUEEN, fen[index] == 'q');
            }
            else if(fen[index] == 'k' || fen[index] == 'K'){
              board_[i][i2].setPiece(KING, fen[index] == 'k');
            }
            index++;
            if(fen[index] == '/'){
              index++;
            }
          }

          else{
            board_[i][i2].setPiece(EMPTY);
            fen[index]--;
            if(fen[index] == '0'){
              index++;
              if(fen[index] == '/'){
                index++;
              }
            }
          }
        }
      }

      //whose turn it is
      index += 1;
      turn_ = (fen[index] == 'b');
      index += 2;

      //Castling information

      castlingIsPossible_[0][0] = 0;
      castlingIsPossible_[0][1] = 0;
      castlingIsPossible_[1][0] = 0;
      castlingIsPossible_[1][1] = 0;
      for(; fen[index] != ' '; index++){
        if(fen[index] == 'K'){
          castlingIsPossible_[WHITE][KINGSIDE_CASTLING] = 1;
        }
        if(fen[index] == 'Q'){
          castlingIsPossible_[WHITE][QUEENSIDE_CASTLING] = 1;
        }
        if(fen[index] == 'k'){
          castlingIsPossible_[BLACK][KINGSIDE_CASTLING] = 1;
        }
        if(fen[index] == 'q'){
          castlingIsPossible_[BLACK][QUEENSIDE_CASTLING] = 1;
        }
      }

      //en passant pawn information
      index++;
      if(fen[index] != '-'){
        int8_t y = fen[index];
        index++;
        int8_t x = fen[index];
        x-= 48;
        x = 8-x;
        y -= 'a';
        board_[x][y].setPiece(EN_PASSANT_PAWN, x == 2);
      }
      index++;



      initPieceArray();
}

void Board::initPieceArray()
{
    for(int8_t i=0; i<2; i++){
       for(int8_t i2=0; i2<16; i2++){
         pieceLocations_[i][i2].setNotOnBoard();
       }
     }
     int8_t pawns[] = {0,0};
     int8_t rooks[] = {0,0};
     int8_t bishops[] = {0,0};
     int8_t knights[] = {0,0};
     for(int8_t i=0; i<8; i++){
       for(int8_t i2=0; i2<8; i2++){
         if(board_[i][i2].getPiece() != EMPTY){
           if(board_[i][i2].getPieceType() == PAWN){
             pieceLocations_[board_[i][i2].color()][0+pawns[board_[i][i2].color()]] = {i,i2};
             pawns[board_[i][i2].color()]++;
           }
           if(board_[i][i2].getPieceType() == ROOK){
             pieceLocations_[board_[i][i2].color()][8+rooks[board_[i][i2].color()]] = {i,i2};
             rooks[board_[i][i2].color()]++;
           }
           if(board_[i][i2].getPieceType() == KNIGHT){
             pieceLocations_[board_[i][i2].color()][10+knights[board_[i][i2].color()]] = {i,i2};
             knights[board_[i][i2].color()]++;
           }
           if(board_[i][i2].getPieceType() == BISHOP){
             pieceLocations_[board_[i][i2].color()][12+bishops[board_[i][i2].color()]] = {i,i2};
             bishops[board_[i][i2].color()]++;
           }
           if(board_[i][i2].getPieceType() == QUEEN){
             pieceLocations_[board_[i][i2].color()][14] = {i,i2};
           }
           if(board_[i][i2].getPieceType() == KING){
             pieceLocations_[board_[i][i2].color()][15] = {i,i2};
           }
           if(board_[i][i2].getPieceType() == EN_PASSANT_PAWN){
             pieceLocations_[board_[i][i2].color()][16] = {i,i2};
           }
         }
       }
     }
     for(int8_t i=0; i<2; i++){
       for(int8_t i2=0; i2<17; i2++){
         if(!pieceLocations_[i][i2].isNotOnBoard()){
           board_[pieceLocations_[i][i2].x()][pieceLocations_[i][i2].y()].setPieceIndex(i2);
         }
       }
     }
}

bool Board::isOwn(int8_t x, int8_t y, int8_t color)
{
    if(board_[x][y].getPieceType() == EMPTY){
        return false;
    }
    return board_[x][y].color() == color;
}


bool Board::isAllowedMoveForRook(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
    if(isOwn(toX, toY, board_[fromX][fromY].color())){
      return false;
    }
    if(toX == fromX){
       int8_t dir = ((fromY < toY)*2)-1;
       for(int8_t i=fromY+dir; i != toY; i += dir){
         if( (board_[toX][i].getPieceType() != EMPTY) && (board_[toX][i].getPieceType() != EN_PASSANT_PAWN )){
           return false;
         }
       }
     }
     else if(toY == fromY){
       int8_t dir = ((fromX < toX)*2)-1;
       for(int8_t i=fromX+dir; i != toX; i += dir){
         if((board_[i][toY].getPieceType() != EMPTY) && (board_[i][toY].getPieceType() != EN_PASSANT_PAWN )){
           return false;
         }
       }
     }
     else{
       return false;
     }
     return true;
}

bool Board::isAllowedMoveForBishop(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
    if( abs(fromX-toX) != abs(fromY-toY)){
        return false;
    }
    if(isOwn(toX, toY, board_[fromX][fromY].color())){
        return false;
    }
    int8_t xDir = ((fromX < toX)*2)-1;
    int8_t yDir = ((fromY < toY)*2)-1;
    for(int8_t i=1; fromX+xDir*i != toX; i++){
        if((board_[fromX+xDir*i][fromY+yDir*i].getPieceType() != EMPTY) && (board_[fromX+xDir*i][fromY+yDir*i].getPieceType() != (EN_PASSANT_PAWN))){
          return false;
        }
    }
    return true;
}

bool Board::isAllowedMoveForKnight(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
    if(!(
                ((abs(fromX-toX) == 2) && (abs(fromY-toY) == 1)) ||
                ((abs(fromY-toY) == 2) && (abs(fromX-toX) == 1))) ){
          return false;
    }
    if(isOwn(toX, toY, board_[fromX][fromY].color())){
        return false;
    }
    return true;
}

bool Board::isAllowedMoveForPawn(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
    int8_t dir = ((int8_t)(board_[fromX][fromY].color()*2))-1;         //-1 or 1 depending on color
    int8_t pawnInitialPosition = 6-board_[fromX][fromY].color()*5;      //6 or 1 depending on color

    if(!(   ((toY == fromY)&& (toX == fromX+dir) && (board_[toX][toY].getPieceType() == EMPTY)) ||    //1 step forward
            ((toY == fromY)&& (toX == fromX+2*dir) && (board_[toX][toY].getPieceType() == EMPTY) && (board_[toX-dir][toY].getPieceType() == EMPTY) && (fromX == pawnInitialPosition)) ||    //2 steps forward
            ((toX == fromX+dir) && ( (toY == fromY+1) || (toY == fromY-1) ) && (board_[toX][toY].getPieceType() != EMPTY)&& (!isOwn(toX, toY,board_[fromX][fromY].color())) ) )){ //pawn capturing another piece

          return false;
    }

    return true;
}

bool Board::isAllowedMoveForQueen(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
    return isAllowedMoveForBishop(fromX, fromY, toX, toY) || isAllowedMoveForRook(fromX, fromY, toX, toY);
}

bool Board::isAllowedMoveForKing(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
    bool castling = false;
    int8_t color = board_[fromX][fromY].color();
    if(abs(fromY-toY)>1){
      if(castlingIsPossible_[color][0] || castlingIsPossible_[color][1]){
        castling = true;
      }
    }


    if(castling){
      if( !((abs(fromY-toY) == 2) && (toX == fromX) && (fromX == 7-color*7)) ) {
        return false;       //king must move 2 steps to x direction and kings x-coordinate must be 7 or 0 (depending on color)
      }



      if(toY > fromY){  //Kingside castling
        if(!castlingIsPossible_[color][KINGSIDE_CASTLING]){
            return false;
        }

        //the positions between king and rook must be empty
        if(! ( (board_[fromX][5].getPieceType() == EMPTY) && (board_[fromX][6].getPieceType() == EMPTY) ) ){
          return false;
        }

        //the positions between king and rook can't be checked
        //  (the end position also can't be checked but that is maked sure later)
        //  (This is because generally we can do checktest only after the move has actually been made)
        if(isChecked(fromX,5,color) || isChecked(fromX,6,color)){
          return false;
        }

      }
      else{ //queenside castling
        if(!castlingIsPossible_[color][QUEENSIDE_CASTLING]){
          return false;
        }

        //the positions between king and rook must be empty
        if(! ( (board_[fromX][1].getPieceType() == EMPTY) && (board_[fromX][2].getPieceType() == EMPTY) && (board_[fromX][3].getPieceType() == EMPTY) ) ){
          return false;
        }

        //the positions between king and rook can't be checked
        //  (the end position also can't be checked but that is maked sure in checkTest-function)
        //  (This is because generally we can do checktest only after the move has actually been made)
        if(isChecked(fromX,1,color) || isChecked(fromX,2,color) || isChecked(fromX,3,color)){
          return false;
        }
      }

      //king can't be in check when castling
      if(isChecked(fromX,fromY,color)){
          return false;
      }
      return true;
    }


    //no castling:
    if(!isOwn(toX,toY,color) && (max(abs(toX-fromX),abs(toY-fromY)) == 1) ){
        return true;
    }
    return false;
}



bool Board::isChecked(int8_t x, int8_t y, int8_t color){
    int8_t targetX, targetY;
    for(int8_t i=0; i<4; i++){
        int8_t* movingAxis = &targetX;
        if(i >= 2){
            movingAxis  = &targetY;
        }
        int8_t dir = (i%2)*2-1; //-1 or 1
        targetX = x;
        targetY = y;
        for(*movingAxis +=dir; max(targetX,targetY)<8 && min(targetX,targetY) >= 0; *movingAxis += dir){
            if( (board_[targetX][targetY].getPieceType() != EMPTY) && (board_[targetX][targetY].getPieceType() != EN_PASSANT_PAWN)){
                if(!isOwn(targetX, targetY,color)){
                    if(board_[targetX][targetY].getPieceType() == QUEEN || board_[targetX][targetY].getPieceType() == ROOK){
                        return true;
                    }
                    if(board_[targetX][targetY].getPieceType() == KING && max(abs(targetX-x), abs(targetY-y)) == 1){
                        return true;
                    }
                }
                break;
            }
        }
    }

    for(int8_t i=0; i<4; i++){

        int8_t xDir = (i%2)*2-1; //-1 1 -1 1
        int8_t yDir = (i<2)*2-1; //1 1 -1 -1
        targetX = x+xDir;
        targetY = y+yDir;
        while(max(targetX,targetY)<8 && min(targetX,targetY) >= 0){
            if( (board_[targetX][targetY].getPieceType() != EMPTY) && (board_[targetX][targetY].getPieceType() != EN_PASSANT_PAWN)){
                if(!isOwn(targetX, targetY,color)){
                    if(board_[targetX][targetY].getPieceType() == QUEEN || board_[targetX][targetY].getPieceType() == BISHOP){
                        return true;
                    }
                    if(board_[targetX][targetY].getPieceType() == KING && max(abs(targetX-x), abs(targetY-y)) == 1){
                        return true;
                    }
                    int8_t dir = ((int8_t)((!color)*2))-1;         //-1 or 1 depending on color. The direction of opponent pawn movement
                    if(board_[targetX][targetY].getPieceType() == PAWN && (targetX+dir == x)){
                        return true;
                    }
                }
                break;
            }
            targetX+=xDir;
            targetY+=yDir;
        }
    }


    int8_t dX = 2;
    int8_t dY = 1;
    for(int8_t i=0; i<8; i++){
        if(min(x+dX,y+dY) >= 0 && max(x+dX, y+dY) < 8){
            if(board_[x+dX][y+dY].getPieceType() == KNIGHT){
                if(!isOwn(x+dX,y+dY,color)){
                    return true;
                }
            }
        }

        dY *= -1;
        if(i%2){
            dX *= -1;
        }
        if(i == 3){
            dX = 1;
            dY = 2;
        }
    }

    return false;
}




bool Board::isAllowedMoveNoCheckTest(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
    if(max(fromX,fromY)>7 || min(fromX,fromY)<0){
        return false;
    }
    if(max(toX,toY)>7 || min(toX,toY)<0){
        return false;
    }
    switch(board_[fromX][fromY].getPieceType()){
        case PAWN:
            return isAllowedMoveForPawn(fromX,fromY,toX,toY);
        case ROOK:
            return isAllowedMoveForRook(fromX,fromY,toX,toY);
        case KNIGHT:
            return isAllowedMoveForKnight(fromX,fromY,toX,toY);
        case BISHOP:
            return  isAllowedMoveForBishop(fromX,fromY,toX,toY);
        case QUEEN:
            return  isAllowedMoveForQueen(fromX,fromY,toX,toY);
        case KING:
            return  isAllowedMoveForKing(fromX,fromY,toX,toY);
        default:
            return false;
    }

}

moveBackupData Board::makeAMove(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo)
{
    movesMade.push_back(Move(fromX,fromY,toX,toY,promotionTo));

    moveBackupData moveBackup;
    moveBackup.fromX = fromX;
    moveBackup.fromY = fromY;
    moveBackup.toX = toX;
    moveBackup.toY = toY;
    moveBackup.isPromoted = (promotionTo != EMPTY);


    int8_t color = board_[fromX][fromY].color();




    moveBackup.castlingInfo = 0;
    moveBackup.castlingInfo |= castlingIsPossible_[0][0];
    moveBackup.castlingInfo |= castlingIsPossible_[0][1]<<1;
    moveBackup.castlingInfo |= castlingIsPossible_[1][0]<<2;
    moveBackup.castlingInfo |= castlingIsPossible_[1][1]<<3;
    //update castling information
    bool castling = updateCastlingInfo(fromX,fromY,toX,toY);

    //check for enpassant capture
    bool enPassantCaptured = false;
    if((board_[toX][toY].getPieceType() == EN_PASSANT_PAWN) && (board_[fromX][fromY].getPieceType() == PAWN)){
        enPassantCaptured = true;
    }


    //update board score
    int16_t scoreChange = calculateScoreChange(fromX,fromY, toX, toY, enPassantCaptured, castling, promotionTo);
    moveBackup.scoreChange = scoreChange;
    boardscore_ -= scoreChange;


    //make the move
    moveBackup.capturedPiece = board_[toX][toY];
    board_[toX][toY] = board_[fromX][fromY];
    board_[fromX][fromY].setPiece(EMPTY);
    pieceLocations_[color][board_[toX][toY].getPieceIndex()] = {toX,toY};
    if(moveBackup.capturedPiece.getPieceType() != EMPTY){
        pieceLocations_[!color][moveBackup.capturedPiece.getPieceIndex()].setNotOnBoard();    //piece was captured
    }





    //remove pawn if enpassant captured
    if(enPassantCaptured){
        board_[fromX][toY].setPiece(EMPTY);  //enemy's pawn eaten at this address ("en passant" -rule)
        pieceLocations_[!color][board_[fromX][toY].getPieceIndex()].setNotOnBoard();
        moveBackup.capturedPiece.setPieceIndex(board_[fromX][toY].getPieceIndex());     //save the pieceindex of removed pawn
    }

    //move rook if castling
    if(castling){
        int8_t rookFromY;
        int8_t rookToY;
        if(toY > fromY){
            //kingside castling
            rookFromY = 7;
            rookToY = 5;
        }
        else{
            //queenside
            rookFromY = 0;
            rookToY = 3;
        }
        board_[fromX][rookToY] = board_[fromX][rookFromY];
        board_[fromX][rookFromY].setPiece(EMPTY);
        pieceLocations_[color][board_[fromX][rookToY].getPieceIndex()] = {fromX,rookToY};
    }


    //spawn enpassant pawn
    if((board_[toX][toY].getPieceType() == PAWN) && (abs(toX-fromX) == 2)){
        int8_t dir = color*2-1;     //-1 or 1, depending on color
        board_[toX-dir][toY].setPiece(EN_PASSANT_PAWN, color);
        board_[toX-dir][toY].setPieceIndex(EN_PASSANT_INDEX);
        pieceLocations_[color][EN_PASSANT_INDEX] = {toX-dir, toY};
    }


    //remove opponents old en passant
    if(pieceLocations_[!color][EN_PASSANT_INDEX].isNotOnBoard() == false){
        moveBackup.removedEnPassant = pieceLocations_[!color][EN_PASSANT_INDEX];

        board_[pieceLocations_[!color][EN_PASSANT_INDEX].x()][pieceLocations_[!color][EN_PASSANT_INDEX].y()].setPiece(EMPTY);
        pieceLocations_[!color][EN_PASSANT_INDEX].setNotOnBoard();
    }
    else{
        moveBackup.removedEnPassant.setNotOnBoard();
    }

    //promotion
    if(promotionTo != EMPTY){
        board_[toX][toY].setPiece(promotionTo,color);
    }

    //opponents turn
    turn_ = !turn_;


    return moveBackup;
}

void Board::reverseAMove(moveBackupData& move)
{
    movesMade.pop_back();

    int8_t color = board_[move.toX][move.toY].color();

    //restore promotion
    if(move.isPromoted){
        board_[move.toX][move.toY].setPiece(PAWN,color);
    }

    //remove spawned en_passant pawn
    if((board_[move.toX][move.toY].getPieceType() == PAWN) && (abs(move.toX - move.fromX) == 2)){
        int8_t dir = color*2-1;     //-1 or 1, depending on color
        board_[move.toX-dir][move.toY].setPiece(EMPTY);   //remove en passant
        pieceLocations_[color][EN_PASSANT_INDEX].setNotOnBoard();
    }


    //restore removed (old) enpassant
    if(move.removedEnPassant.isNotOnBoard() == false){
        board_[move.removedEnPassant.x()][move.removedEnPassant.y()].setPiece(EN_PASSANT_PAWN,!color);
        board_[move.removedEnPassant.x()][move.removedEnPassant.y()].setPieceIndex(EN_PASSANT_INDEX);
        pieceLocations_[!color][EN_PASSANT_INDEX] = move.removedEnPassant;
    }


    //restore castling info
    castlingIsPossible_[0][0] = move.castlingInfo&1;
    castlingIsPossible_[0][1] = (move.castlingInfo>>1)&1;
    castlingIsPossible_[1][0] = (move.castlingInfo>>2)&1;
    castlingIsPossible_[1][1] = (move.castlingInfo>>3)&1;


    //restore board score
    boardscore_ += move.scoreChange;




    bool enPassantCaptured = false;
    //restore removed pawn in case of en_passant capture
    if( (board_[move.toX][move.toY].getPieceType() == PAWN) && (move.capturedPiece.getPieceType() == EN_PASSANT_PAWN)){
        enPassantCaptured = true;
        board_[move.fromX][move.toY].setPiece(PAWN,!color);
        board_[move.fromX][move.toY].setPieceIndex(move.capturedPiece.getPieceIndex());
        pieceLocations_[!color][move.capturedPiece.getPieceIndex()] = {move.fromX,move.toY};  //in this case the piece index of the removed pawn is stored here
    }

    //restore rook position if castling was done
    if(board_[move.toX][move.toY].getPieceType() == KING && (abs(move.fromY - move.toY) > 1)){
        int8_t rookFromY;
        int8_t rookToY;
        if(move.toY > move.fromY){
            //kingside castling
            rookFromY = 7;
            rookToY = 5;
        }
        else{
            //queenside
            rookFromY = 0;
            rookToY = 3;
        }
        board_[move.fromX][rookFromY] = board_[move.fromX][rookToY];
        board_[move.fromX][rookToY].setPiece(EMPTY);
        pieceLocations_[color][board_[move.fromX][rookFromY].getPieceIndex()] = {move.fromX,rookFromY};
    }


    //undo move
    board_[move.fromX][move.fromY] = board_[move.toX][move.toY];
    board_[move.toX][move.toY].setPiece(EMPTY);
    pieceLocations_[color][board_[move.fromX][move.fromY].getPieceIndex()] = {move.fromX,move.fromY};
    if(move.capturedPiece.getPieceType() != EMPTY){
        board_[move.toX][move.toY] = move.capturedPiece;
        if(enPassantCaptured){
            pieceLocations_[!color][EN_PASSANT_INDEX] = {move.toX,move.toY};
            board_[move.toX][move.toY].setPieceIndex(EN_PASSANT_INDEX); //the index of the captured pawn is stored in move.capturedPiece
        }
        else{
            pieceLocations_[!color][move.capturedPiece.getPieceIndex()] = {move.toX,move.toY};
        }
    }

    //opponents turn
    turn_ = !turn_;

}


bool Board::updateCastlingInfo(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
    int8_t color = board_[fromX][fromY].color();
    bool castling = 0;
    if(board_[fromX][fromY].getPieceType() == KING){
        if(abs(fromY-toY)>1){
            castling = true;
        }
        else{
            castling = false;
        }
        castlingIsPossible_[color][0] = 0;
        castlingIsPossible_[color][1] = 0;
    }
    if(board_[fromX][fromY].getPieceType() == ROOK){        //rook moved
        if((fromY == 0) && (fromX == 7-color*7)){
            castlingIsPossible_[color][QUEENSIDE_CASTLING] = 0;
        }
        else if((fromY == 7) && (fromX == 7-color*7)){
            castlingIsPossible_[color][KINGSIDE_CASTLING] = 0;
        }
    }
    if(board_[toX][toY].getPieceType() == ROOK){    //opponents rook captured
        if(toY == 7 && (toX == 7-(!color)*7)){
            castlingIsPossible_[!color][KINGSIDE_CASTLING] = 0;
        }
        else if(toY == 0 && (toX == 7-(!color)*7)){
            castlingIsPossible_[!color][QUEENSIDE_CASTLING] = 0;
        }
    }

    return castling;
}

int16_t Board::calculateScoreChange(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, bool enPassantCaptured,bool castling, int8_t promotionTo)
{
    int16_t scoreChange = 0;

    //if a piece is captured
    if((board_[toX][toY].getPieceType() != EMPTY)  && ( (board_[toX][toY].getPieceType() != EN_PASSANT_PAWN) || enPassantCaptured)){
        int8_t index = board_[toX][toY].getPieceType()-1;
        int8_t pieceSquareIndex;
        if(board_[toX][toY].color() == WHITE){
          pieceSquareIndex = toX*8+toY;
        }
        else{
          pieceSquareIndex = (7-toX)*8 + toY;   //piecesquareindex mirrored if color == BLACK
        }
        if(enPassantCaptured){
          index = PAWN-1;
          if(board_[toX][toY].color() == WHITE){
            pieceSquareIndex = fromX*8+toY;       //this is where pawn was captured (en passant-rule)
          }
          else{
            pieceSquareIndex = (7-fromX)*8 + toY;
          }
        }
        scoreChange = -1*(PIECE_VALUES[index]+PIECE_SQUARE_VALUES[index][pieceSquareIndex]);
    }


    //score change for move from {fromX,fromY}, {toX,toY}
    //value is substracted from boardscore, so "own" pieces "gone" ->positive, new pieces/positions negative
    //"own" means that pieces of the player making the move that are lost -> positive
    int8_t fromIndex = board_[fromX][fromY].getPieceType()-1;
    int8_t toIndex = fromIndex;
    if(promotionTo != EMPTY){
        toIndex = promotionTo-1;
        scoreChange += PIECE_VALUES[fromIndex];
        scoreChange -= PIECE_VALUES[toIndex];
    }
    int8_t fromPieceSquareIndex;
    int8_t toPieceSquareIndex;
    if(board_[fromX][fromY].color() == WHITE){
        toPieceSquareIndex = toX*8+toY;
        fromPieceSquareIndex = fromX*8+fromY;
    }
    else{
        toPieceSquareIndex = toY+(7-toX)*8;
        fromPieceSquareIndex = fromY+(7-fromX)*8;
    }
    scoreChange += PIECE_SQUARE_VALUES[fromIndex][fromPieceSquareIndex];
    scoreChange -= PIECE_SQUARE_VALUES[toIndex][toPieceSquareIndex];

    //if castling was done then rook moves also in addition to king
    if(castling){
        int8_t rookFromY;
        int8_t rookToY;
        if(toY > fromY){    //kingside castling
            rookFromY = 7;
            rookToY = 5;
        }
        else{   //queenside
            rookFromY = 0;
            rookToY = 3;
        }
        scoreChange += PIECE_SQUARE_VALUES[ROOK-1][7*8 + rookFromY];
        scoreChange -= PIECE_SQUARE_VALUES[ROOK-1][7*8 + rookToY];
    }


    if(board_[fromX][fromY].color() == BLACK){       //negative values for black pieces
      scoreChange *= -1;
    }
    return scoreChange;
}

int16_t Board::evaluateBoard()
{
    int16_t tempScore = 0;
    for(int8_t i=0; i<2; i++){
        for(int8_t i2=0; i2<16; i2++){
            if(pieceLocations_[i][i2].isNotOnBoard() == false){
                int8_t index = (board_[pieceLocations_[i][i2].x()][pieceLocations_[i][i2].y()].getPieceType())-1;
                int8_t pieceSquareIndex;
                if(board_[pieceLocations_[i][i2].x()][pieceLocations_[i][i2].y()].color() == WHITE){
                    int8_t x = pieceLocations_[i][i2].x();
                    int8_t y = pieceLocations_[i][i2].y();
                    pieceSquareIndex = y+x*8;
                }
                else{
                    int8_t x = 7-pieceLocations_[i][i2].x();        //mirror x for black pieces for pieceSquareindex arrays
                    int8_t y = pieceLocations_[i][i2].y();
                    pieceSquareIndex = y+x*8;
                }
                int16_t value = PIECE_VALUES[index]+PIECE_SQUARE_VALUES[index][pieceSquareIndex];
                if(board_[pieceLocations_[i][i2].x()][pieceLocations_[i][i2].y()].color() == BLACK){       //negative values for black pieces
                    value *= -1;
                }
                tempScore += value;
            }
        }
    }
    return tempScore;
}

void Board::findLegalMovesForIndex(vector<Move> &moves, uint8_t index)
{
    findLegalMovesForPiece(moves, pieceLocations_[turn_][index].x(), pieceLocations_[turn_][index].y());
}

void Board::findLegalMovesForPiece(vector<Move> &moves, int8_t x, int8_t y)
{
    switch(board_[x][y].getPieceType()){
        case PAWN:
            findLegalMovesPawn(moves, x,y);
            break;
        case ROOK:
            findLegalMovesRook(moves, x,y);
            break;
        case KNIGHT:
            findLegalMovesKnight(moves, x,y);
            break;
        case BISHOP:
            findLegalMovesBishop(moves, x,y);
            break;
        case QUEEN:
            findLegalMovesQueen(moves, x,y);
            break;
        case KING:
            findLegalMovesKing(moves, x,y);
            break;
    }
}

void Board::findLegalMovesPawn(vector<Move> &moves, int8_t x, int8_t y)
{
    const int8_t dir = ((int8_t)(board_[x][y].color()*2))-1;         //-1 or 1 depending on color
    const int8_t pawnInitialPosition = 6-board_[x][y].color()*5;      //6 or 1 depending on color
    const int8_t pawnEndPosition = 0+board_[x][y].color()*7;        //0 or 7

    for(int8_t yMove = -1; yMove != 2; yMove++){
        if(isAllowedMove(x,y,x+dir,y+yMove)){
            if(x+dir == pawnEndPosition){
                moves.push_back({x,y,(int8_t)(x+dir),(int8_t)(y+yMove),QUEEN});
                moves.push_back({x,y,(int8_t)(x+dir),(int8_t)(y+yMove),ROOK});
                moves.push_back({x,y,(int8_t)(x+dir),(int8_t)(y+yMove),BISHOP});
                moves.push_back({x,y,(int8_t)(x+dir),(int8_t)(y+yMove),KNIGHT});
            }
            else{
              moves.push_back({x,y,(int8_t)(x+dir),(int8_t)(y+yMove),EMPTY});
            }
        }
    }


    if(x == pawnInitialPosition){
        if(isAllowedMove(x,y,x+2*dir,y)){
            moves.push_back({x,y,(int8_t)(x+2*dir),y,EMPTY});
        }
    }

}

void Board::findLegalMovesRook(vector<Move> &moves, int8_t x, int8_t y)
{
    int8_t toX, toY;
    for(int8_t i=0; i<4; i++){
        int8_t* movingAxis = &toX;
        if(i >= 2){
            movingAxis  = &toY;
        }
        int8_t dir = (i%2)*2-1; //-1 or 1
        toX = x;
        toY = y;
        for(*movingAxis +=dir; max(toX,toY)<8 && min(toX,toY) >= 0; *movingAxis += dir){
            if(isAllowedMove(x,y,toX,toY)){
                moves.push_back({x,y,toX,toY,EMPTY});
            }
            if( (board_[toX][toY].getPieceType() != EMPTY) && (board_[toX][toY].getPieceType() != EN_PASSANT_PAWN)){
                break;
            }
        }
    }
}

void Board::findLegalMovesBishop(vector<Move> &moves, int8_t x, int8_t y)
{
    for(int8_t i=0; i<4; i++){

        int8_t xDir = (i%2)*2-1; //-1 1 -1 1
        int8_t yDir = (i<2)*2-1; //1 1 -1 -1
        int8_t toX = x+xDir;
        int8_t toY = y+yDir;
        while(max(toX,toY)<8 && min(toX,toY) >= 0){
            if(isAllowedMove(x,y,toX,toY)){
                moves.push_back({x,y,toX,toY,EMPTY});
            }
            if( (board_[toX][toY].getPieceType() != EMPTY) && (board_[toX][toY].getPieceType() != EN_PASSANT_PAWN)){
                break;
            }
            toX+=xDir;
            toY+=yDir;
        }
    }
}

void Board::findLegalMovesKnight(vector<Move> &moves, int8_t x, int8_t y)
{
    int8_t dX = 2;
    int8_t dY = 1;
    for(int8_t i=0; i<8; i++){
        if(isAllowedMove(x,y,x+dX,y+dY)){
            moves.push_back({x,y,(int8_t)(x+dX),(int8_t)(y+dY),EMPTY});
        }
        dY *= -1;
        if(i%2){
            dX *= -1;
        }
        if(i == 3){
            dX = 1;
            dY = 2;
        }
    }
}

void Board::findLegalMovesQueen(vector<Move> &moves, int8_t x, int8_t y)
{
    findLegalMovesRook(moves,x,y);
    findLegalMovesBishop(moves,x,y);
}

void Board::findLegalMovesKing(vector<Move> &moves, int8_t x, int8_t y)
{
    for(int8_t dX=-1; dX <= 1; dX++){
        for(int8_t dY=-1; dY <= 1; dY++){
            if((dX == 0) && (dY == 0)){
                continue;
            }
            if(isAllowedMove(x,y,x+dX,y+dY)){
                moves.push_back({x,y,(int8_t)(x+dX),(int8_t)(y+dY),EMPTY});
            }
        }
    }

    //try castling
    int8_t castlingMove = 2;
    for(int8_t i=0; i<2; i++){
        if(isAllowedMove(x,y,x,y+castlingMove)){
            moves.push_back({x,y,x,(int8_t)(y+castlingMove),EMPTY});
        }
        castlingMove *= -1;
    }
}



bool Board::isBetterOrEqScore(int16_t a, int16_t b, int8_t color)
{
    if(color == WHITE){
        return a >= b;
    }
    return a <= b;
}
