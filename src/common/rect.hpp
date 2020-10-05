#ifndef RECT_HPP
    #define RECT_HPP
    
    #include <iostream>
    #include <stdio.h>

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

            friend bool operator==(const Rect<T> &lhs, const Rect<T> &rhs) {
                if (lhs.x == rhs.x &&
                    lhs.y == rhs.y &&
                    lhs.w == rhs.w &&
                    lhs.h == rhs.h
                ) {
                    return true;
                }

                return false;
            }

            friend std::ostream& operator<<(std::ostream &os, const Rect<T> &rect) {
                printf("Rect { x: %f, y: %f, w: %f, h: %f }", rect.x, rect.y, rect.w, rect.h);
                return os;
            }
    };
#endif