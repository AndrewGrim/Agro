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

        friend bool operator!=(const Color &lhs, const Color &rhs) {
            if (lhs.r != rhs.r ||
                lhs.g != rhs.g ||
                lhs.b != rhs.b ||
                lhs.a != rhs.a
            ) {
                return true;
            }

            return false;
        }
    };
#endif