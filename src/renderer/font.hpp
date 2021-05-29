#ifndef FONT_HPP
    #define FONT_HPP

    #include <map>
    #include <string>

    #include <ft2build.h>
    #include FT_FREETYPE_H

    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"
    #include "../common/point.hpp"

    #include "glad.h"
    #include "shader.hpp"
    #include "batch.hpp"
    #include "texture.hpp"

    struct Font {
        enum class Type {
            Sans,
            Serif,
            Mono,
        };

        struct Character {
            Point bearing;
            Size size;
            long int advance;
            float textureX;
        };

        std::map<char, Font::Character> characters;
        unsigned int max_height = 0;
        unsigned int pixel_size = 0;
        unsigned int atlas_width = 0;
        unsigned int atlas_height = 0;
        unsigned int tab_width = 4;
        Type type;
        std::string file_path;
        unsigned int atlas_ID;

        Font(std::string file_path, unsigned int pixel_size, Type type);
        Font(const unsigned char *data, signed long length, unsigned int pixel_size, Type type);
        ~Font();

        private:
            void load(FT_Face face);
    };
#endif
