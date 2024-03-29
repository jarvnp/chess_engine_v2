#include "board.h"
#include <iostream>
#include <time.h>


using namespace std;

int main(int argc, char* argv[])
{
    srand(time(NULL));
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
            cin >> move;
        }
        a.printBoard(cout);
    }
    return 0;
}
