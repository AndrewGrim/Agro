#ifndef COLOR_HPP
    #define COLOR_HPP

    struct Color {
        float r;
        float g;
        float b;
        float a;

        Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
    };
#endif