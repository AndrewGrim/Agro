#ifndef RENDERER_HPP
    #define RENDERER_HPP

    #include <map>
    #include <string>
    #include <vector>

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

    class Window;

    struct Renderer {
        enum class Sampler {
            Color,
            Texture,
            Text,
            Dashed
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

        struct Selection {
            size_t begin = 0;
            size_t end = 0;

            Selection(size_t begin = 0, size_t end = 0) : begin{begin}, end{end} {}
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
        Window *window = nullptr;

        Renderer(Window *window, unsigned int *indices);
        ~Renderer();
        void fillText(Font *font, Slice<const char> text, Point point, Color color = COLOR_BLACK, int tab_width = 4, bool is_multiline = false, int line_spacing = 5, Selection selection = Selection(), Color selection_color = COLOR_BLACK);
        Size measureText(Font *font, std::string text, int tab_width = 4, bool is_multiline = false, int line_spacing = 5);
        void drawTexture(Point point, Size size, Texture *texture, TextureCoordinates *coords, Color color = COLOR_WHITE);
        void fillRect(Rect rect, Color color);
        void fillRectWithGradient(Rect rect, Color fromColor, Color toColor, Gradient orientation);
        void drawDashedRect(Rect rect, Color color);
        void check();
        void textCheck(Font *font);
        void render();

        private:
            void reset();
    };
#endif
