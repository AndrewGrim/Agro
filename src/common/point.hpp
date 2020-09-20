#ifndef POINT_HPP
    #define POINT_HPP

    template <class T> class Point {
        public:
            T x;
            T y;

            Point(T x = 0.0f, T y = 0.0f) {
                this->x = x;
                this->y = y;
            }
    };
#endif