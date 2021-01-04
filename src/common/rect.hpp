#ifndef RECT_HPP
    #define RECT_HPP
    
    #include <ostream>

    struct Rect {
        float x;
        float y;
        float w;
        float h;

        Rect(float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f) {
            this->x = x;
            this->y = y;
            this->w = w;
            this->h = h;
        }

        void shrink(float amount) {   
            this->x + amount;
            this->y + amount;
            this->w - amount * 2;
            this->h - amount * 2;
        }

        friend bool operator==(const Rect &lhs, const Rect &rhs) {
            if (lhs.x == rhs.x &&
                lhs.y == rhs.y &&
                lhs.w == rhs.w &&
                lhs.h == rhs.h
            ) {
                return true;
            }

            return false;
        }

        friend std::ostream& operator<<(std::ostream &os, const Rect &rect) {
            return os << "Rect { x: " << rect.x << ", y: " << rect.y << ", w: " << rect.w << ", h: " << rect.h << " }";
        }
    };
#endif