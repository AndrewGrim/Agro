#ifndef RECT_HPP
    #define RECT_HPP

    template <class T> class Rect {
        public:
            T x;
            T y;
            T w;
            T h;

            Rect(T x = 0.0f, T y = 0.0f, T w = 0.0f, T h = 0.0f) {
                this->x = x;
                this->y = y;
                this->w = w;
                this->h = h;
            }
    };
#endif