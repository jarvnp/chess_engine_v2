#include "boardpoint.h"

BoardPoint::BoardPoint()
{
    setNotOnBoard();
}

int8_t BoardPoint::x()
{
    return point_/8;
}

int8_t BoardPoint::y()
{
    return point_%8;
}

bool BoardPoint::isNotOnBoard()
{
    return point_ == NOT_ON_BOARD_PIECE_LOCATION;
}

void BoardPoint::setNotOnBoard()
{
    point_ = NOT_ON_BOARD_PIECE_LOCATION;
}



BoardPoint &BoardPoint::operator=(const std::pair<int8_t, int8_t> coordinates)
{
    point_ = coordinates.first*8+coordinates.second;
    return *this;
}
