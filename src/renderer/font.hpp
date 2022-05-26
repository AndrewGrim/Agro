#ifndef FONT_HPP
    #define FONT_HPP

    #include <unordered_map>

    #include <ft2build.h>
    #include FT_FREETYPE_H

    #include "../core/string.hpp"
    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"
    #include "../common/point.hpp"
    #include "../common/number_types.h"

    #include "glad.h"
    #include "shader.hpp"
    #include "batch.hpp"
    #include "texture.hpp"

    class Application;

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
            f32 texture_array_index = 0.0f;

            Character() {}
            Character(Point bearing, Size size, i32 advance, f32 texture_x, f32 texture_array_index) :
            bearing{bearing}, size{size}, advance{advance}, texture_x{texture_x}, texture_array_index{texture_array_index} {}
        };

        String file_path;
        bool loaded_from_file = false;
        const u8 *data = nullptr;
        i64 data_size = 0;
        u32 pixel_size = 0;
        Type type;

        u32 next_slot = 0;
        u32 next_depth = 0;
        u32 atlas_width = 0;
        u32 atlas_height = 0;
        u32 atlas_depth = 0;
        u32 atlas_ID; // Set by opengl when creating font atlas texture.
        i32 max_bearing = 0;

        Font(Application *app, String file_path, u32 pixel_size, Type type);
        // Note for UI zoom / scaling we assume that we can use the same data pointer to load the font at a different size.
        Font(Application *app, const u8 *data, i64 length, u32 pixel_size, Type type);
        ~Font();
        u32 maxHeight();
        Character get(u32 codepoint);
        // Note that this returns a new font at the specified size
        // nothing happens to the original.
        Font* reload(i64 new_pixel_size);

        private:
            Application *app;

            FT_Face face = NULL;
            std::unordered_map<u32, Font::Character> characters;

            void load(FT_Face face);
            void grow(u32 width, u32 height, u32 depth);
            void loadGlyph(u32 codepoint, bool bind_texture = true);
    };
#endif
