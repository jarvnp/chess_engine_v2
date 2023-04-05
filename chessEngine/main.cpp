#include "board.h"
#include <iostream>
#include <time.h>


using namespace std;

int main()
{
    srand(time(NULL));
<<<<<<< Updated upstream
    string fen;
    getline(cin,fen);
    Board a(fen);
    a.printBoard(cout);
    while(1){
        a.searchForMove(5);
        string move;
        cin >> move;
        if(move == "q"){
            break;
        }
        while(!a.userMakeMoveIfAllowed(move)){
            cout << "illegal move" << endl;
        }
        a.printBoard(cout);
=======
    Board board;
    for(int i=1; i<argc; i++){
      if(!board.userMakeMoveIfAllowed(argv[i])){
        return EXIT_FAILURE;
      }
>>>>>>> Stashed changes
    }
    board.searchForMove(2);  //2 seconds


    return 0;
}
