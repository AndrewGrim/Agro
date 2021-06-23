#ifndef POINT_HPP
    #define POINT_HPP

    struct Point {
        int x;
        int y;

        Point(int x = 0, int y = 0) {
            this->x = x;
            this->y = y;
        }
    };
#endif
