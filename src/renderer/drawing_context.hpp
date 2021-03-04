#ifndef DRAWING_CONTEXT_HPP
    #define DRAWING_CONTEXT_HPP

    #include <glad/glad.h>
    #include <SDL2/SDL.h>

    #include "../common/enums.hpp"
    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"
    #include "../common/point.hpp"
    #include "../common/style.hpp"

    #include "shader.hpp"
    #include "batch.hpp"
    #include "renderer.hpp"
    #include "font.hpp"

    struct DrawingContext {
        // TODO dc will need to be modified so that
        // dc is window agnostic and only switches between the
        // renderer instances that refer to different windows/glcontexts
        // finally it will need to be made static and initialized before
        // all other imports so we could access it anywhere
        unsigned int indices[MAX_BATCH_SIZE * QUAD_INDEX_COUNT];
        Renderer *renderer = nullptr;
        Font *default_font = nullptr;
        Style default_style;

        DrawingContext();
        ~DrawingContext();
        void fillRect(Rect rect, Color color);
        void fillRectWithGradient(Rect rect, Color fromColor, Color toColor, Gradient orientation);
        void fillText(Font *font, std::string text, Point point, Color color = Color(), float scale = 1.0f);
        Size measureText(Font *font, std::string text, float scale = 1.0f);
        Size measureText(Font *font, char c, float scale = 1.0f);
        void render();
        void fillTextAligned(Font *font, std::string text, HorizontalAlignment h_align, VerticalAlignment v_align, Rect rect, int padding, Color color = Color());
        Rect drawBorder3D(Rect rect, int border_width, Color rect_color);
        void drawBorder(Rect &rect, Style &style);
        void margin(Rect &rect, Style &style);
        void padding(Rect &rect, Style &style);
        void sizeHintMargin(Size &size, Style &style);
        void sizeHintBorder(Size &size, Style &style);
        void sizeHintPadding(Size &size, Style &style);
        void drawTexture(Point point, Size size, Texture *texture, TextureCoordinates *coords, Color color = COLOR_WHITE);
        void drawTextureAligned(Rect rect, Size size, Texture *texture, TextureCoordinates *coords, HorizontalAlignment h_align, VerticalAlignment v_align, Color color = Color(1, 1, 1));
        void drawPoint(Point point, Color color);
        void clear();
        void swap_buffer(SDL_Window *win);
        void setClip(Rect rect);
        Rect clip();
    };
#endif