#include "board.h"
#include <iostream>


using namespace std;

int main()
{
    Board a;
    a.printBoard(cout);
    while(1){
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
