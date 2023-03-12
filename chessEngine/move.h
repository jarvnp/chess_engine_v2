#ifndef MOVE_H
#define MOVE_H

#include <stdint.h>

struct Move{
    int8_t fromX;
    int8_t fromY;
    int8_t toX;
    int8_t toY;
    int8_t promotionTo;
};

#endif // MOVE_H
