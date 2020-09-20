#ifndef COLOR_HPP
    #define COLOR_HPP

    class Color {
        public:
            float red;
            float green;
            float blue;
            float alpha;

            Color(float red, float green, float blue, float aplha) {
                this->red = red;
                this->green = green;
                this->blue = blue;
                this->alpha = alpha;
            }
    };
#endif