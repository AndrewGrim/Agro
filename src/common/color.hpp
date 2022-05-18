#ifndef COLOR_HPP
    #define COLOR_HPP

    #include <cassert>
    #include "../core/string.hpp"

    #include "number_types.h"

    #define COLOR_DEFAULT Color(Color::IsDefault::Yes)
    #define COLOR_NONE Color(0.0f, 0.0f, 0.0f, 0.0f)
	#define COLOR_BLACK Color(0.0f, 0.0f, 0.0f, 1.0f)
	#define COLOR_WHITE Color(1.0f, 1.0f, 1.0f, 1.0f)

    struct Color {
        enum class IsDefault {
            Yes, No
        };

        IsDefault is_default = IsDefault::No;

        f32 r = 0.0f;
        f32 g = 0.0f;
        f32 b = 0.0f;
        f32 a = 1.0f;


        Color(IsDefault is_default) : is_default{is_default} {

        }

        Color(f32 r = 0.0f, f32 g = 0.0f, f32 b = 0.0f, f32 a = 1.0f) :
            r{r}, g{g}, b{b}, a{a} {

        }

        Color(const char *string) {
            if (!string || *string == '\0') return;
            else if (*string == '#') string++;

            f32 *color_attribute = &r;
            for (i32 i = 0; i < 4; i++) {
                if (*string == '\0') { return; }
                *color_attribute = (matchHexFromChar(*string) * 16) / 255.0f;
                string++;
                if (*string == '\0') { return; }
                *color_attribute += matchHexFromChar(*string) / 255.0f;
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
            sprintf(buffer, "#%.02X%.02X%.02X%.02X", (i32)(r * 255), (i32)(g * 255), (i32)(b * 255), (i32)(a * 255));
            return String(buffer);
        }

        static Color fromInt(u8 r, u8 g = 0, u8 b = 0, u8 a = 255) {
            return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }

        operator bool() {
            if (is_default == IsDefault::Yes) {
                return false;
            }
            return true;
        }

        bool operator==(Color &&rhs) {
            if (this->r == rhs.r &&
                this->g == rhs.g &&
                this->b == rhs.b &&
                this->a == rhs.a) {
                return true;
            }
            return false;
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

            return from += Color(r, g, b, a);
        }
    };
#endif
