#ifndef BOARDPOINT_H
#define BOARDPOINT_H

//the value that is stored in point_ if this point does not point to any piece on the board
#define NOT_ON_BOARD_PIECE_LOCATION 100


#include <stdint.h>
#include <utility>
#include <functional>

class BoardPoint
{
public:
    BoardPoint();

    int8_t x()const;
    int8_t y()const;

    bool isNotOnBoard();  //if this point is not on board returns false
    void setNotOnBoard();        //sets point to be not on board. To set point on board, use assignment operator

    //assign boardPoint from a pair of coordinates
    BoardPoint& operator= (const std::pair<int8_t,int8_t> coordinates);

    bool operator== (const BoardPoint& a)const;

    int8_t getRaw();

    friend std::hash<BoardPoint>;
private:
    int8_t point_;
};


namespace std {
    template <> struct hash<BoardPoint> {
        std::size_t operator()(const BoardPoint& cp) const;
    };
}

#endif // BOARDPOINT_H
