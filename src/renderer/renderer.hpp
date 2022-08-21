#ifndef RENDERER_HPP
    #define RENDERER_HPP

    #include <vector>
    #include <memory>

    #include <ft2build.h>
    #include FT_FREETYPE_H

    #include "../slice.hpp"

    #include "../common/enums.hpp"
    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"
    #include "../common/point.hpp"

    #include "glad.h"
    #include "shader.hpp"
    #include "batch.hpp"
    #include "texture.hpp"
    #include "font.hpp"

    #define AGRO_OPENGL_RESERVED_FOR_LOADING 0
    #define AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY 1
    #define AGRO_OPENGL_RESERVED_TEXTURE_SLOTS 2

    struct Renderer {
        enum class Sampler {
            Color,
            Texture,
            Text,
            Dashed
        };

        struct Vertex {
            f32 position[2];
            f32 texture_uv[2];
            f32 color[4];
            f32 texture_index;
            f32 texture_array_index;
            f32 is_text;
            f32 rect[4];
            f32 clip_rect[4];
        };

        struct Selection {
            u64 begin = 0;
            u64 end = 0;

            Selection(u64 begin = 0, u64 end = 0) : begin{begin}, end{end} {}
        };

        i32 max_texture_slots;
        i32 max_texture_depth;
        i32 max_texture_size;
        i32 current_texture_slot = AGRO_OPENGL_RESERVED_TEXTURE_SLOTS;
        u32 gl_texture_begin = GL_TEXTURE0;
        Shader shader;
        u32 index = 0;
        u32 quad_count = 0;
        Vertex *vertices = new Vertex[MAX_BATCH_SIZE * QUAD_VERTEX_COUNT];
        u32 VAO, VBO, EBO;
        Rect clip_rect; // Gets set before each draw() in Application.
        Font *last_font = nullptr;

        Renderer(u32 *indices);
        ~Renderer();
        void fillText(
            std::shared_ptr<Font> font,
            Slice<const char> text,
            Point point,
            Color color = COLOR_BLACK,
            i32 tab_width = 4,
            bool is_multiline = false,
            i32 line_spacing = 5,
            Selection selection = Selection(),
            Color selection_color = COLOR_BLACK
        );
        void fillTextAtScale(
            std::shared_ptr<Font> font,
            Slice<const char> text,
            Point point,
            Color color = COLOR_BLACK,
            i32 tab_width = 4,
            bool is_multiline = false,
            i32 line_spacing = 5,
            Selection selection = Selection(),
            Color selection_color = COLOR_BLACK,
            f32 scale = 1.0f
        );
        Size measureText(
            std::shared_ptr<Font> font,
            Slice<const char> text,
            i32 tab_width = 4,
            bool is_multiline = false,
            i32 line_spacing = 5
        );
        Size measureTextAtScale(
            std::shared_ptr<Font> font,
            Slice<const char> text,
            i32 tab_width = 4,
            bool is_multiline = false,
            i32 line_spacing = 5,
            f32 scale = 1.0f
        );
        void drawTexture(Point point, Size size, Texture *texture, TextureCoordinates *coords, Color color = COLOR_WHITE);
        void fillRect(Rect rect, Color color);
        void fillRectWithGradient(Rect rect, Color fromColor, Color toColor, Gradient orientation);
        void drawDashedRect(Rect rect, Color color);
        void textQuad(f32 x, f32 y, Font::Character ch, Font &font, Color color);
        void check();
        void textCheck(std::shared_ptr<Font> font);
        void render();

        private:
            void reset();
    };
#endif
