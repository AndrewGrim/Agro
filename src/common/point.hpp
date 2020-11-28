#ifndef POINT_HPP
    #define POINT_HPP

    struct Point {
        float x;
        float y;

        Point(float x = 0.0f, float y = 0.0f) {
            this->x = x;
            this->y = y;
        }
    };
#endif