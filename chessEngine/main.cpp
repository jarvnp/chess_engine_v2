#include "board.h"
#include <iostream>


using namespace std;

int main()
{
    string fen;
    getline(cin,fen);
    Board a(fen);
    a.printBoard(cout);
    while(1){

        a.searchForMove(4,true);
        string move;
        cin >> move;
        if(move == "q"){
            break;
        }
        if(!a.userMakeMoveIfAllowed(move)){
            cout << "illegal move" << endl;
        }
        else{
            a.printBoard(cout);
        }
    }
    return 0;
}
