#ifndef POINT_HPP
    #define POINT_HPP

    struct Point {
        int x;
        int y;

        Point(int x = 0.0f, int y = 0.0f) {
            this->x = x;
            this->y = y;
        }
    };
#endif
