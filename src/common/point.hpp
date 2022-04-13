#ifndef POINT_HPP
    #define POINT_HPP

    #include "number_types.h"

    struct Point {
        i32 x;
        i32 y;

        Point(i32 x = 0, i32 y = 0) {
            this->x = x;
            this->y = y;
        }
    };
#endif
