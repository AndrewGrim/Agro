#ifndef SIZE_HPP
    #define SIZE_HPP

    template <class T> class Size {
        public:
            T w;
            T h;

            Size(T w = 0.0f, T h = 0.0f) {
                this->w = w;
                this->h = h;
            }

            friend bool operator==(const Size<T> &lhs, const Size<T> &rhs) {
                if (lhs.w == rhs.w && lhs.h == rhs.h) {
                    return true;
                }

                return false;
            }

            friend std::ostream& operator<<(std::ostream &os, const Size<T> &size) {
                printf("Size { w: %f, h: %f }", size.w, size.h);
                return os;
            }
    };
#endif