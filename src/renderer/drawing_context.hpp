#ifndef DRAWING_CONTEXT_HPP
    #define DRAWING_CONTEXT_HPP

    #include <glad/glad.h>
    #include <SDL2/SDL.h>

    #include "../common/align.hpp"
    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"

    #include "shader.hpp"
    #include "batch.hpp"

    #include "renderer.hpp"
    #include "font.hpp"

    struct DrawingContext {
        uint indices[MAX_BATCH_SIZE * QUAD_INDEX_COUNT];
        Renderer *renderer;
        Font *default_font = nullptr;

        DrawingContext(void *app);
        ~DrawingContext();
        void fillRect(Rect rect, Color color);
        void fillGradientRect(Rect rect, Color fromColor, Color toColor, Renderer::Gradient orientation);
        void fillText(Font *font, std::string text, float x, float y, Color color = Color(), float scale = 1.0f);
        Size measureText(Font *font, std::string text, float scale = 1.0f);
        Size measureText(Font *font, char c, float scale = 1.0f);
        void render();
        void fillTextAligned(Font *font, std::string text, TextAlignment alignment, Rect rect, int padding, Color color = Color());
        Rect drawBorder(Rect rect, int border_width, Color rect_color);
        void drawImage(float x, float y, Texture *texture, Color color = Color(1, 1, 1));
        void drawImageAtSize(float x, float y, Size size, Texture *texture, Color color = Color(1, 1, 1));
        void drawImageAligned(Rect rect, HorizontalAlignment h_align, VerticalAlignment v_align, Texture *texture, Color color = Color(1, 1, 1));
        void drawImageAlignedAtSize(Rect rect, HorizontalAlignment h_align, VerticalAlignment v_align, Size size, Texture *texture, Color color = Color(1, 1, 1));
        void clear();
        void swap_buffer(SDL_Window *win);
    };
#endif