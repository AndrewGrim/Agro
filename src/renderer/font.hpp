#ifndef FONT_HPP
    #define FONT_HPP

    #include <unordered_map>
    #include <string>

    #include <ft2build.h>
    #include FT_FREETYPE_H

    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"
    #include "../common/point.hpp"
    #include "../common/number_types.h"

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
            i32 advance = 0;
            f32 texture_x = 0.0f;

            Character() {}
            Character(Point bearing, Size size, i32 advance, f32 texture_x) :
            bearing{bearing}, size{size}, advance{advance}, texture_x{texture_x} {}
        };

        std::string file_path;
        u32 pixel_size = 0;
        Type type;

        u32 next_slot = 0;
        u32 atlas_width = 0;
        u32 atlas_height = 0;
        u32 atlas_ID; // Set by opengl when creating font atlas texture.
        i32 max_bearing = 0;

        Font(FT_Library ft, std::string file_path, u32 pixel_size, Type type);
        Font(FT_Library ft, const unsigned char *data, i64 length, u32 pixel_size, Type type);
        ~Font();
        u32 maxHeight();
        Character get(u32 codepoint);

        private:
            FT_Face face = NULL;
            std::unordered_map<u32, Font::Character> characters;

            void load(FT_Face face);
            void grow(u32 width, u32 height);
            void loadGlyph(u32 codepoint, bool bind_texture = true);
    };
#endif
