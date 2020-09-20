#ifndef RECT_HPP
    #define RECT_HPP

    class Rect {
        public:
            int x;
            int y;
            int w;
            int h;

            Rect(int x, int y, int w, int h) {
                this->x = x;
                this->y = y;
                this->w = w;
                this->h = h;
            }
    };
#endif