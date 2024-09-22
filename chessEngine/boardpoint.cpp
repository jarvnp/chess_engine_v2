#include "boardpoint.h"

BoardPoint::BoardPoint()
{
    setNotOnBoard();
}

int8_t BoardPoint::x()const
{
    return point_ & 0b00000111;
}

int8_t BoardPoint::y()const
{
    return (point_&0b00111000) >> 3;
}

bool BoardPoint::isNotOnBoard()
{
    return point_ == NOT_ON_BOARD_PIECE_LOCATION;
}

void BoardPoint::setNotOnBoard()
{
    point_ = NOT_ON_BOARD_PIECE_LOCATION;
}

int8_t BoardPoint::getRaw(){
    return point_;
}

BoardPoint &BoardPoint::operator=(const std::pair<int8_t, int8_t> coordinates)
{
    point_ = coordinates.first | (coordinates.second<<3);
    return *this;
}

bool BoardPoint::operator== (const BoardPoint& a)const{
    return (a.point_ == point_);
}


std::size_t std::hash<BoardPoint>::operator()(BoardPoint const& a) const {
    return a.point_;
}