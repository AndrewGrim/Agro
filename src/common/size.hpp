#ifndef SIZE_HPP
    #define SIZE_HPP

    #include <ostream>

    #include "number_types.h"

    struct Size {
        i32 w;
        i32 h;

        Size(i32 w = 0, i32 h = 0) {
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
