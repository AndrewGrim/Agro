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
    };
#endif