#ifndef COLOR_HPP
    #define COLOR_HPP

    #include <cinttypes>
    #include <cassert>

    #define COLOR_DEFAULT Color(Color::IsDefault::Yes)
    #define COLOR_NONE Color(0.0f, 0.0f, 0.0f, 0.0f)
	#define COLOR_BLACK Color(0.0f, 0.0f, 0.0f, 1.0f)
	#define COLOR_WHITE Color(1.0f, 1.0f, 1.0f, 1.0f)

    struct Color {
        enum class IsDefault {
            Yes, No
        };

        IsDefault is_default = IsDefault::No;

        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        float a = 1.0f;


        Color(IsDefault is_default) : is_default{is_default} {

        }

        Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f) :
            r{r}, g{g}, b{b}, a{a} {
        
        }

        Color(const char *string) {
            if (*string == '#') ++string;

            r = hexStringColorToInt(string) / 255.0f;
            string += 2;

            if (*string == '\0') return;
            g = hexStringColorToInt(string) / 255.0f;
            string += 2;

            if (*string == '\0') return;
            b = hexStringColorToInt(string) / 255.0f;
            string += 2;

            if (*string == '\0') return;
            a = hexStringColorToInt(string) / 255.0f;
        }

        uint8_t hexStringColorToInt(const char *s) {
            uint8_t c = 0;
                c += hexCharToInt(*s) * 16;
                c += hexCharToInt(*(++s));
            return c;
        }

        uint8_t hexCharToInt(char c) {
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
                default: assert(true && "Invalid hex number.");
            }
            return -1;
        }

        static Color fromInt(uint8_t r, uint8_t g = 0, uint8_t b = 0, uint8_t a = 255) {
            return Color(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }
    };
#endif