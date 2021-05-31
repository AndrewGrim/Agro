#ifndef RENDERER_HPP
    #define RENDERER_HPP

    #include <map>
    #include <string>
    #include <vector>

    #include <ft2build.h>
    #include FT_FREETYPE_H

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

    struct Renderer {
        enum class Sampler {
            Color,
            Texture,
            Text
        };

        struct Vertex {
            float position[2];
            float texture_uv[2];
            float color[4];
            float texture_index;
            float is_text;
            float rect[4];
            float clip_rect[4];
        };

        int max_texture_slots;
        int current_texture_slot = 2;
        unsigned int gl_texture_begin = GL_TEXTURE0;
        Shader shader;
        unsigned int index = 0;
        unsigned int quad_count = 0;
        Vertex *vertices = new Vertex[MAX_BATCH_SIZE * QUAD_VERTEX_COUNT];
        unsigned int VAO, VBO, EBO;
        Rect clip_rect; // Gets set before each draw() in Application.

        Renderer(unsigned int *indices);
        ~Renderer();
        void fillText(Font *font, const char *text, size_t text_length, Point point, Color color = COLOR_BLACK, float scale = 1.0f);
        void fillTextMultiline(Font *font, std::string text, Point point, Color color = COLOR_BLACK, float scale = 1.0f, float line_spacing = 5.0f);
        Size measureText(Font *font, std::string text, float scale = 1.0f);
        Size measureText(Font *font, char c, float scale = 1.0f);
        Size measureTextMultiline(Font *font, std::string text, float scale = 1.0f, float line_spacing = 5.0f);
        void drawTexture(Point point, Size size, Texture *texture, TextureCoordinates *coords, Color color = COLOR_WHITE);
        void fillRect(Rect rect, Color color);
        void fillRectWithGradient(Rect rect, Color fromColor, Color toColor, Gradient orientation);
        void check();
        void textCheck(Font *font);
        void render();

        private:
            void reset();
    };
#endif
