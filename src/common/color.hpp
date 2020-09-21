#ifndef COLOR_HPP
    #define COLOR_HPP

    class Color {
        public:
            float red;
            float green;
            float blue;
            float alpha;

            Color(float red = 0.0f, float green = 0.0f, float blue = 0.0f, float aplha = 1.0f) {
                this->red = red;
                this->green = green;
                this->blue = blue;
                this->alpha = alpha;
            }
    };
#endif