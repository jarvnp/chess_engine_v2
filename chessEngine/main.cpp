#include "board.h"
#include <iostream>
#include <time.h>


using namespace std;

int main(int argc, char* argv[])
{
    //srand(time(NULL));
    srand(0);
    //string fen;
    //getline(cin,fen);
    Board a;
    while(1){
        a.searchForMove(5);
        a.printBoard(cout);
        string move;
        cin >> move;
        if(move == "q"){
            break;
        }
        while(!a.userMakeMoveIfAllowed(move)){
            cout << "illegal move" << endl;
            cin >> move;
        }
    }
    return 0;
}
