#ifndef COLOR_HPP
    #define COLOR_HPP

    #include <cassert>
    #include "../core/string.hpp"

    #include "number_types.h"

    #define COLOR_DEFAULT Color(0, 0, 0, 1)
    #define COLOR_NONE Color(0, 0, 0, 0)
    #define COLOR_BLACK Color(0, 0, 0, 255)
    #define COLOR_WHITE Color(255, 255, 255, 255)

    struct Color {
        u8 r = 0;
        u8 g = 0;
        u8 b = 0;
        u8 a = 255;

        Color(u8 r = 0, u8 g = 0, u8 b = 0, u8 a = 255) : r{r}, g{g}, b{b}, a{a} {}

        Color(const char *string) {
            if (!string || *string == '\0') return;
            else if (*string == '#') string++;

            u8 *color_attribute = &r;
            for (i32 i = 0; i < 4; i++) {
                if (*string == '\0') { return; }
                *color_attribute = (matchHexFromChar(*string) * 16);
                string++;
                if (*string == '\0') { return; }
                *color_attribute += matchHexFromChar(*string);
                string++;
                color_attribute++;
            }
        }

        u8 matchHexFromChar(u8 c) {
            switch (c) {
                case '0': return 0;
                case '1': return 1;
                case '2': return 2;
                case '3': return 3;
                case '4': return 4;
                case '5': return 5;
                case '6': return 6;
                case '7': return 7;
                case '8': return 8;
                case '9': return 9;
                case 'a': case 'A': return 10;
                case 'b': case 'B': return 11;
                case 'c': case 'C': return 12;
                case 'd': case 'D': return 13;
                case 'e': case 'E': return 14;
                case 'f': case 'F': return 15;
                default: return 0;
            }
        }

        String toString() {
            char buffer[10] = {};
            sprintf(buffer, "#%.02X%.02X%.02X%.02X", r, g, b, a);
            return String(buffer);
        }

        operator bool() {
            return !(*this == COLOR_DEFAULT);
        }

        bool operator==(Color &&rhs) {
            return this->r == rhs.r &&
                   this->g == rhs.g &&
                   this->b == rhs.b &&
                   this->a == rhs.a;
        }

        Color& operator+=(const Color &rhs) {
            this->r += rhs.r;
            this->g += rhs.g;
            this->b += rhs.b;
            this->a += rhs.a;
            return *this;
        }

        static Color interpolate(Color from, Color to, f32 step) {
            f32 r = (to.r - from.r) * step;
            f32 g = (to.g - from.g) * step;
            f32 b = (to.b - from.b) * step;
            f32 a = (to.a - from.a) * step;

            return from += Color(r * 255, g * 255, b * 255, a * 255);
        }
    };
#endif
