#include "board.h"
#include <iostream>





Board::Board(string fen)
{
    fenToBoard(fen);
    boardscore_ = evaluateBoard();
}

void Board::printBoard(ostream &stream)
{
    stream << castlingIsPossible_[0][0] << " " << castlingIsPossible_[0][1] << " " << castlingIsPossible_[1][0] << " " << castlingIsPossible_[1][1] << "\n";
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
}

bool Board::makeMoveIfAllowed(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo)
{
    int8_t color = board_[fromX][fromY].color();
    if(!isAllowedMoveNoCheckTest(fromX, fromY, toX, toY)){
        return false;
    }
    moveBackupData* moveBackup = makeAMove(fromX, fromY, toX, toY, promotionTo);
    if(isChecked(pieceLocations_[color][KING_INDEX].x(), pieceLocations_[color][KING_INDEX].y(),color)){
        reverseAMove(moveBackup);
        delete moveBackup;
        return false;
    }
    delete moveBackup;
    return true;
}

bool Board::userMakeMoveIfAllowed(string move)
{
    if(move.size() < 4){
        return false;
    }
    if(move.size() > 5){
        return false;
    }
    int8_t fromX = move[1];
    int8_t fromY = move[0];
    int8_t toX = move[3];
    int8_t toY = move[2];
    int8_t promotion = EMPTY;
    if(move.size() > 4){
        promotion = move[4];
    }

    fromX = fromX - '1';     //  tranform '1' - '8' to 0-7
    fromX = 7-fromX;        //mirror
    toX = toX - '1';
    toX = 7-toX;

    fromY = fromY - 'a';
    toY = toY - 'a';

    if(fromX < 0 || fromX > 7){
        return false;
    }
    if(fromY < 0 || fromY > 7){
        return false;
    }
    if(toX < 0 || toX > 7){
        return false;
    }
    if(toY < 0 || toY > 7){
        return false;
    }

    if(promotion != 0){
        if(promotion == 'q'){
            promotion = QUEEN;
        }
        else if(promotion == 'r'){
            promotion = ROOK;
        }
        else if(promotion == 'b'){
            promotion = BISHOP;
        }
        else if(promotion == 'k'){
            promotion = KNIGHT;
        }
        else{
            return false;
        }
    }

    return makeMoveIfAllowed(fromX, fromY, toX, toY,promotion);

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
      index += 3; //skipping the part telling who's turn it is

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

//checks if a move is legal for rook
bool Board::isAllowedMoveForRook(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
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
     if(isOwn(toX, toY, board_[fromX][fromY].color())){
       return false;
     }
     return true;
}

bool Board::isAllowedMoveForBishop(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
    if( abs(fromX-toX) != abs(fromY-toY)){
        return false;
    }
    int8_t xDir = ((fromX < toX)*2)-1;
    int8_t yDir = ((fromY < toY)*2)-1;
    for(int8_t i=1; fromX+xDir*i != toX; i++){
        if((board_[fromX+xDir*i][fromY+yDir*i].getPieceType() != EMPTY) && (board_[fromX+xDir*i][fromY+yDir*i].getPieceType() != (EN_PASSANT_PAWN))){
          return false;
        }
    }
    if(isOwn(toX, toY, board_[fromX][fromY].color())){
        return false;
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
        //  (the end position also can't be checked but that is maked sure in checkTest-function)
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
      return true;
    }


    //no castling:
    if(!isOwn(toX,toY,color) && (max(abs(toX-fromX),abs(toY-fromY)) == 1) ){
        return true;
    }
    return false;
}




//Checks whether a point on board is checked
//Does this by going through every opponent piece and checking if they can move to this position
//Explanation for parameter "color": if color = WHITE, then we check if any black piece can move to this position
bool Board::isChecked(int8_t x, int8_t y, int8_t color)
{
    int8_t backUp = board_[x][y].getPiece();

    //we need to make sure the isAllowedMoveNoCheckTest -function thinks there is some piece in the square we are checking here. That is needed for the PAWN-test.
    //(Pawns move differently when they capture pieces)
    board_[x][y].setPiece(PAWN,color);

    for(int8_t i=0; i<16; i++){
        if(pieceLocations_[!color][i].isNotOnBoard() == false){
            if(isAllowedMoveNoCheckTest(pieceLocations_[!color][i].x(),pieceLocations_[!color][i].y(),x,y)){
                board_[x][y].setPiece(backUp);
                return true;
            }
        }
    }

    board_[x][y].setPiece(backUp);
    return false;
}



bool Board::isAllowedMoveNoCheckTest(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY)
{
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


moveBackupData* Board::makeAMove(int8_t fromX, int8_t fromY, int8_t toX, int8_t toY, int8_t promotionTo)
{
    moveBackupData* moveBackup = new moveBackupData;
    moveBackup ->fromX = fromX;
    moveBackup ->fromY = fromY;
    moveBackup ->toX = toX;
    moveBackup ->toY = toY;
    moveBackup->isPromoted = (promotionTo != EMPTY);


    int8_t color = board_[fromX][fromY].color();




    moveBackup->castlingInfo = 0;
    moveBackup->castlingInfo |= castlingIsPossible_[0][0];
    moveBackup->castlingInfo |= castlingIsPossible_[0][1]<<1;
    moveBackup->castlingInfo |= castlingIsPossible_[1][0]<<2;
    moveBackup->castlingInfo |= castlingIsPossible_[1][1]<<3;
    //update castling information
    bool castling = updateCastlingInfo(fromX,fromY,toX,toY);

    //check for enpassant capture
    bool enPassantCaptured = false;
    if((board_[toX][toY].getPieceType() == EN_PASSANT_PAWN) && (board_[fromX][fromY].getPieceType() == PAWN)){
        enPassantCaptured = true;
    }


    //update board score
    int16_t scoreChange = calculateScoreChange(fromX,fromY, toX, toY, enPassantCaptured, castling, promotionTo);
    moveBackup->scoreChange = scoreChange;
    boardscore_ -= scoreChange;


    //make the move
    moveBackup->capturedPiece = board_[toX][toY];
    board_[toX][toY] = board_[fromX][fromY];
    board_[fromX][fromY].setPiece(EMPTY);
    pieceLocations_[color][board_[toX][toY].getPieceIndex()] = {toX,toY};
    if(moveBackup->capturedPiece.getPieceType() != EMPTY){
        pieceLocations_[!color][moveBackup->capturedPiece.getPieceIndex()].setNotOnBoard();    //piece was captured
    }

    //remove pawn if enpassant captured
    if(enPassantCaptured){
        board_[fromX][toY].setPiece(EMPTY);  //enemy's pawn eaten at this address ("en passant" -rule)
        pieceLocations_[!color][board_[fromX][toY].getPieceIndex()].setNotOnBoard();
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
        moveBackup->removedEnPassant = pieceLocations_[!color][EN_PASSANT_INDEX];

        board_[pieceLocations_[!color][EN_PASSANT_INDEX].x()][pieceLocations_[!color][16].y()].setPiece(EMPTY);
        pieceLocations_[!color][EN_PASSANT_INDEX].setNotOnBoard();
    }

    //promotion
    if(promotionTo != EMPTY){
        board_[toX][toY].setPiece(promotionTo,color);
    }


    return moveBackup;
}

void Board::reverseAMove(moveBackupData *move)
{
    int8_t color = board_[move->toX][move->toY].color();

    //restore promotion
    if(move->isPromoted){
        board_[move->toX][move->toY].setPiece(PAWN,color);
    }

    //remove spawned en_passant pawn
    if((board_[move->fromX][move->fromY].getPieceType() == PAWN) && (abs(move->toX - move->fromX) == 2)){
        int8_t dir = color*2-1;     //-1 or 1, depending on color
        board_[move->toX-dir][move->toY].setPiece(EMPTY);   //remove en passant
        pieceLocations_[color][EN_PASSANT_INDEX].setNotOnBoard();
    }


    //restore removed (old) enpassant
    if(move->removedEnPassant.isNotOnBoard() == false){
        board_[move->removedEnPassant.x()][move->removedEnPassant.y()].setPiece(EN_PASSANT_PAWN,!color);
        board_[move->removedEnPassant.x()][move->removedEnPassant.y()].setPieceIndex(EN_PASSANT_INDEX);
        pieceLocations_[!color][EN_PASSANT_INDEX] = move->removedEnPassant;
    }


    //restore castling info
    castlingIsPossible_[0][0] = move->castlingInfo&1;
    castlingIsPossible_[0][1] = (move->castlingInfo>>1)&1;
    castlingIsPossible_[1][0] = (move->castlingInfo>>2)&1;
    castlingIsPossible_[1][1] = (move->castlingInfo>>3)&1;


    //restore board score
    boardscore_ += move->scoreChange;


    //restore removed pawn in case of en_passant capture
    if( (board_[move->toX][move->toY].getPieceType() == PAWN) && (move->capturedPiece.getPieceType() == EN_PASSANT_PAWN)){
        board_[move->fromX][move->toY].setPiece(PAWN,!color);
        pieceLocations_[!color][board_[move->fromX][move->toY].getPieceIndex()] = {move->fromX,move->toY};  //pieceIndex informations isn't removed in makeAMove-function
    }

    //restore rook position if castling was done
    if(board_[move->toX][move->toY].getPieceType() == KING && (abs(move->fromY - move->toY) > 1)){
        int8_t rookFromY;
        int8_t rookToY;
        if(move->toY > move->fromY){
            //kingside castling
            rookFromY = 7;
            rookToY = 5;
        }
        else{
            //queenside
            rookFromY = 0;
            rookToY = 3;
        }
        board_[move->fromX][rookFromY] = board_[move->fromX][rookToY];
        board_[move->fromX][rookToY].setPiece(EMPTY);
        pieceLocations_[color][board_[move->fromX][rookFromY].getPieceIndex()] = {move->fromX,rookFromY};
    }

    //undo move
    board_[move->fromX][move->fromY] = board_[move->toX][move->toY];
    board_[move->toX][move->toY].setPiece(EMPTY);
    pieceLocations_[color][board_[move->fromX][move->fromY].getPieceIndex()] = {move->fromX,move->fromY};
    if(move->capturedPiece.getPieceType() != EMPTY){
        board_[move->toX][move->toY] = move->capturedPiece;
        pieceLocations_[!color][move->capturedPiece.getPieceIndex()] = {move->toX,move->toY};
    }


}

//return true if castling is done by this move
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
