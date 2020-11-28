#ifndef SIZE_HPP
    #define SIZE_HPP

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
            printf("Size { w: %f, h: %f }", size.w, size.h);
            return os;
        }
    };
#endif