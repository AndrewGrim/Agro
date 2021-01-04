#ifndef SIZE_HPP
    #define SIZE_HPP

    #include <ostream>

    struct Size {
        float w;
        float h;

        Size(float w = 0.0f, float h = 0.0f) {
            this->w = w;
            this->h = h;
        }

        friend bool operator==(const Size &lhs, const Size &rhs) {
            if (lhs.w == rhs.w && lhs.h == rhs.h) {
                return true;
            }

            return false;
        }

        friend std::ostream& operator<<(std::ostream &os, const Size &size) {
            return os << "Size { w: " << size.w << ", h: " << size.h << " }";
        }
    };
#endif