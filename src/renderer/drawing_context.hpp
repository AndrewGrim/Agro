#ifndef DRAWING_CONTEXT_HPP
    #define DRAWING_CONTEXT_HPP

    #include <cmath>

    #include <SDL.h>

    #include "../common/enums.hpp"
    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"
    #include "../common/point.hpp"
    #include "../common/style.hpp"

    #include "glad.h"
    #include "shader.hpp"
    #include "batch.hpp"
    #include "renderer.hpp"
    #include "font.hpp"

    class Window;

    struct DrawingContext {
        u32 indices[MAX_BATCH_SIZE * QUAD_INDEX_COUNT];
        Renderer *renderer = nullptr;
        Style default_light_style;
        Style default_dark_style;
        Style default_style;
        Window *window = nullptr;

        DrawingContext(Window *window);
        ~DrawingContext();
        void fillRect(Rect rect, Color color);
        void fillRectWithGradient(Rect rect, Color fromColor, Color toColor, Gradient orientation);
        void drawDashedRect(Rect rect, Color color);
        void fillText(std::shared_ptr<Font> font, Slice<const char> text, Point point, Color color = COLOR_BLACK, i32 tab_width = 4, Renderer::Selection selection = Renderer::Selection(), Color selection_color = COLOR_BLACK);
        void fillTextMultiline(std::shared_ptr<Font> font, Slice<const char> text, Point point, Color color = COLOR_BLACK, i32 tab_width = 4, i32 line_spacing = 5, Renderer::Selection selection = Renderer::Selection(), Color selection_color = COLOR_BLACK);
        void fillTextAligned(std::shared_ptr<Font> font, Slice<const char> text, HorizontalAlignment h_align, VerticalAlignment v_align, Rect rect, i32 padding, Color color = COLOR_BLACK, i32 tab_width = 4, Renderer::Selection selection = Renderer::Selection(), Color selection_color = COLOR_BLACK);
        void fillTextMultilineAligned(std::shared_ptr<Font> font, Slice<const char> text, HorizontalAlignment h_align, VerticalAlignment v_align, Rect rect, i32 padding, Color color = COLOR_BLACK, i32 tab_width = 4, i32 line_spacing = 5, Renderer::Selection selection = Renderer::Selection(), Color selection_color = COLOR_BLACK);
        Size measureText(std::shared_ptr<Font> font, Slice<const char> text, i32 tab_width = 4);
        Size measureTextMultiline(std::shared_ptr<Font> font, Slice<const char> text, i32 tab_width = 4, i32 line_spacing = 5);
        void render();
        Rect drawBorder3D(Rect rect, i32 border_width, Color rect_color);
        void drawBorder(Rect &rect, Style &style, i32 state);
        void drawKeyboardFocus(Rect &rect, Style &style, i32 state);
        void margin(Rect &rect, Style &style);
        void padding(Rect &rect, Style &style);
        void sizeHintMargin(Size &size, Style &style);
        void sizeHintBorder(Size &size, Style &style);
        void sizeHintPadding(Size &size, Style &style);
        void drawTexture(Point point, Size size, Texture *texture, TextureCoordinates *coords, Color color = COLOR_WHITE);
        void drawTextureAligned(Rect rect, Size size, Texture *texture, TextureCoordinates *coords, HorizontalAlignment h_align, VerticalAlignment v_align, Color color = COLOR_WHITE);
        void drawPoint(Point point, Color color);
        void clear();
        void swap_buffer(SDL_Window *win);
        void setClip(Rect rect);
        Rect clip();

        // TODO these should probably move to widget
        // this way the user could easily query the color of the widget
        // without having to access the style directly and possibly
        // also checking the default style???? eh im not sure
        // UPDATE: We added accessors for all style members
        // into drawable so in theory they could fullfil this purpose
        // of querying the colors but i do like having them in drawingcontext
        // since thats what the user will be using for drawing anyway.
        Color windowBackground(Style &style);
        Color widgetBackground(Style &style);
        Color accentWidgetBackground(Style &style);
        Color hoveredBackground(Style &style);
        Color pressedBackground(Style &style);
        Color accentHoveredBackground(Style &style);
        Color accentPressedBackground(Style &style);
        Color textForeground(Style &style);
        Color textBackground(Style &style);
        Color textSelected(Style &style);
        Color textDisabled(Style &style);
        Color iconForeground(Style &style);
        Color borderBackground(Style &style);
        Color borderTopBackground(Style &style);
        Color borderBottomBackground(Style &style);
        Color borderLeftBackground(Style &style);
        Color borderRightBackground(Style &style);

        Color getColor(Point point);
    };
#endif
