#include "drawing_context.hpp"
#include "../application.hpp"
#include "../slice.hpp"

DrawingContext::DrawingContext() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned int offset = 0;
    unsigned int indexCount = MAX_BATCH_SIZE * QUAD_INDEX_COUNT;
    for (unsigned int i = 0; i < indexCount; i += QUAD_INDEX_COUNT) {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;
        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;
        offset += 4;
    }

    renderer = new Renderer(indices);

    default_light_style = {
        Style::Margin{
            STYLE_ALL,
            1,
            1,
            1,
            1
        },

        Style::Border{
            STYLE_ALL,
            1,
            1,
            1,
            1,
            Color("#777777"),
            Color("#777777"),
            Color("#777777"),
            Color("#777777")
        },

        Style::Padding{
            STYLE_ALL,
            5,
            5,
            5,
            5
        },

        Color("#f0f0f0"),
        Color("#e1e1e1"),
        Color("#337fff"),

        Color("#000000"),
        Color("#ffffff"),
        Color("#ffffff"),
        Color("#aaaaaa"),

        Color("#d0d0d0"),
        Color("#c0c0c0"),
        Color("#669eff"),
        Color("#005dff"),

        Color("#000000"),
        Color("#777777")
    };
    default_dark_style = {
        Style::Margin{
            STYLE_ALL,
            1,
            1,
            1,
            1
        },

        Style::Border{
            STYLE_ALL,
            1,
            1,
            1,
            1,
            Color("#dddddd"),
            Color("#dddddd"),
            Color("#dddddd"),
            Color("#dddddd")
        },

        Style::Padding{
            STYLE_ALL,
            5,
            5,
            5,
            5
        },

        Color("#282828"),
        Color("#757575"),
        Color("#fcb737"),

        Color("#ffffff"),
        Color("#555555"),
        Color("#000000"),
        Color("#909090"),

        Color("#8e8e8e"),
        Color("#a8a8a8"),
        Color("#fcc96a"),
        Color("#fca400"),

        Color("#ffffff"),
        Color("#dddddd")
    };
    default_style = default_light_style;
}

DrawingContext::~DrawingContext() {
    delete renderer;
    delete default_font;
}

void DrawingContext::fillRect(Rect rect, Color color) {
    renderer->fillRect(rect, color);
}

void DrawingContext::fillRectWithGradient(Rect rect, Color fromColor, Color toColor, Gradient orientation) {
    renderer->fillRectWithGradient(rect, fromColor, toColor, orientation);
}

void DrawingContext::drawDashedRect(Rect rect, Color color) {
    renderer->drawDashedRect(rect, color);
}

void DrawingContext::render() {
    renderer->render();
}

void DrawingContext::fillText(Font *font, std::string text, Point point, Color color, int tab_width, Renderer::Selection selection, Color selection_color) {
    renderer->fillText(font ? font : default_font, Slice<const char>(text.c_str(), text.length()), point, color, tab_width, false, 0, selection, selection_color);
}

void DrawingContext::fillTextMultiline(Font *font, std::string text, Point point, Color color, int tab_width, int line_spacing, Renderer::Selection selection, Color selection_color) {
    renderer->fillText(font ? font : default_font, Slice<const char>(text.c_str(), text.length()), point, color, tab_width, true, line_spacing, selection, selection_color);
}

void DrawingContext::fillTextAligned(Font *font, std::string text, HorizontalAlignment h_align, VerticalAlignment v_align, Rect rect, int padding, Color color, int tab_width, Renderer::Selection selection, Color selection_color) {
    Point pos = Point();
    Size text_size = measureText(font, text, tab_width);
    switch (h_align) {
        case HorizontalAlignment::Left:
            pos.x = rect.x + padding;
            break;
        case HorizontalAlignment::Right:
            pos.x = (rect.x + rect.w) - (text_size.w + padding);
            break;
        case HorizontalAlignment::Center:
            pos.x = rect.x + (rect.w * 0.5) - (text_size.w * 0.5);
            break;
    }
    switch (v_align) {
        case VerticalAlignment::Top:
            pos.y = rect.y + padding;
            break;
        case VerticalAlignment::Bottom:
            pos.y = (rect.y + rect.h) - (text_size.h + padding);
            break;
        case VerticalAlignment::Center:
            pos.y = rect.y + (rect.h * 0.5) - (text_size.h * 0.5);
            break;
    }
    fillText(
        font,
        text,
        pos,
        color,
        tab_width,
        selection,
        selection_color
    );
}

void DrawingContext::fillTextMultilineAligned(Font *font, std::string text, HorizontalAlignment h_align, VerticalAlignment v_align, Rect rect, int padding, Color color, int tab_width, int line_spacing, Renderer::Selection selection, Color selection_color) {
    font = font ? font : default_font;
    Point pos = Point(rect.x, rect.y);
    Size text_size = measureTextMultiline(font, text, tab_width, line_spacing);
    switch (v_align) {
        case VerticalAlignment::Top:
            pos.y = rect.y + padding;
            break;
        case VerticalAlignment::Bottom:
            pos.y = (rect.y + rect.h) - (text_size.h + padding);
            break;
        case VerticalAlignment::Center:
            pos.y = rect.y + (rect.h * 0.5) - (text_size.h * 0.5);
            break;
    }

    int line_width = 0;
    const char *start = text.data();
    size_t count = 0;
    for (char c : text) {
        line_width += measureText(font, c, tab_width).w;
        count++;
        if (c == '\n') {
            switch (h_align) {
                case HorizontalAlignment::Left:
                    pos.x = rect.x + padding;
                    break;
                case HorizontalAlignment::Right:
                    pos.x = (rect.x + rect.w) - (line_width + padding);
                    break;
                case HorizontalAlignment::Center:
                    pos.x = rect.x + (rect.w * 0.5) - (line_width * 0.5);
                    break;
            }
            renderer->fillText(font, Slice<const char>(start, count), pos, color, tab_width, false, 0, selection, selection_color);
            start += count;
            pos.y += font->max_height + line_spacing;
            pos.x = rect.x;
            line_width = 0;
            count = 0;
        }
    }
    switch (h_align) {
        case HorizontalAlignment::Left:
            pos.x = rect.x + padding;
            break;
        case HorizontalAlignment::Right:
            pos.x = (rect.x + rect.w) - (line_width + padding);
            break;
        case HorizontalAlignment::Center:
            pos.x = rect.x + (rect.w * 0.5) - (line_width * 0.5);
            break;
    }
    renderer->fillText(font, Slice<const char>(start, count), pos, color, tab_width, false, 0, selection, selection_color);
}

Size DrawingContext::measureText(Font *font, std::string text, int tab_width) {
    return renderer->measureText(font ? font : default_font, text, tab_width);
}

Size DrawingContext::measureText(Font *font, char c, int tab_width) {
    return renderer->measureText(font ? font : default_font, std::string(1, c), tab_width);
}

Size DrawingContext::measureTextMultiline(Font *font, std::string text, int tab_width, int line_spacing) {
    return renderer->measureText(font ? font : default_font, text, tab_width, true, line_spacing);
}

Rect DrawingContext::drawBorder3D(Rect rect, int border_width, Color rect_color) {
    // light border
    {
        // bottom layer of the top & left border : white, drawn first so that the top layer will paint over some extra pixels from here
        this->fillRect(
            Rect(rect.x,
                rect.y + border_width / 2,
                rect.w,
                border_width / 2
            ), Color(1.0f, 1.0f, 1.0f)
        );
        this->fillRect(
            Rect(rect.x + border_width / 2,
                rect.y,
                border_width / 2,
                rect.h
            ), Color(1.0f, 1.0f, 1.0f)
        );
        // top layer of the top & left border : background
        this->fillRect(
            Rect(rect.x,
                rect.y,
                rect.w,
                border_width / 2
            ), rect_color
        );
        this->fillRect(
            Rect(rect.x,
                rect.y,
                border_width / 2,
                rect.h
            ), rect_color
        );
    }
    // dark border
    {
        // top layer of the bottom & right border : dark grey, drawn first so that the bottom layer will paint over some extra pixels from here
        this->fillRect(
            Rect(rect.x + border_width / 2,
                rect.y + rect.h - border_width,
                rect.w - border_width / 2,
                border_width / 2
            ), Color(0.4f, 0.4f, 0.4f)
        );
        this->fillRect(
            Rect(rect.x + rect.w - border_width,
                rect.y + border_width / 2,
                border_width / 2,
                rect.h - border_width / 2
            ), Color(0.4f, 0.4f, 0.4f)
        );
        // bottom layer of the bottom & right border : black
        this->fillRect(
            Rect(rect.x,
                rect.y + rect.h - border_width / 2,
                rect.w,
                border_width / 2
            ), Color(0.0f, 0.0f, 0.0f)
        );
        this->fillRect(
            Rect(rect.x + rect.w - border_width / 2,
                rect.y,
                border_width / 2,
                rect.h
            ), Color(0.0f, 0.0f, 0.0f)
        );
    }

    // resize rectangle to account for border
    rect = Rect(
        rect.x + border_width,
        rect.y + border_width,
        rect.w - border_width * 2,
        rect.h - border_width * 2
    );

    return rect;
}

void DrawingContext::drawBorder(Rect &rect, Style &style, int state) {
    // TODO if the widget is focused draw the border using the accent color
    // we will probably pass the whole state and check for focus
    // which means all the call sites will need to change
    if (style.border.type != STYLE_NONE) {
        const int border = style.border.type == STYLE_DEFAULT ? default_style.border.type : style.border.type;
        if (border & STYLE_TOP) {
            const int size = style.border.top < 0 ? default_style.border.top : style.border.top;
            fillRect(Rect(rect.x, rect.y, rect.w, size), state & Drawable::DrawableState::STATE_HARD_FOCUSED ? accentWidgetBackground(style) : borderTopBackground(style));
            rect.y += size;
            rect.h -= size;
        }
        if (border & STYLE_BOTTOM) {
            const int size = style.border.bottom < 0 ? default_style.border.bottom : style.border.bottom;
            rect.h -= size;
            fillRect(Rect(rect.x, rect.y + rect.h, rect.w, size), state & Drawable::DrawableState::STATE_HARD_FOCUSED ? accentWidgetBackground(style) : borderBottomBackground(style));
        }
        if (border & STYLE_LEFT) {
            const int size = style.border.left < 0 ? default_style.border.left : style.border.left;
            fillRect(Rect(rect.x, rect.y, size, rect.h), state & Drawable::DrawableState::STATE_HARD_FOCUSED ? accentWidgetBackground(style) : borderLeftBackground(style));
            rect.x += size;
            rect.w -= size;
        }
        if (border & STYLE_RIGHT) {
            const int size = style.border.right < 0 ? default_style.border.right : style.border.right;
            rect.w -= size;
            fillRect(Rect(rect.x + rect.w, rect.y, size, rect.h), state & Drawable::DrawableState::STATE_HARD_FOCUSED ? accentWidgetBackground(style) : borderRightBackground(style));
        }
    }
}

void DrawingContext::drawKeyboardFocus(Rect &rect, Style &style, int state) {
    if (state & Drawable::DrawableState::STATE_SOFT_FOCUSED) {
        drawDashedRect(rect, borderBackground(style));
    }
}

void DrawingContext::margin(Rect &rect, Style &style) {
    if (style.margin.type != STYLE_NONE) {
        const int margin = style.margin.type == STYLE_DEFAULT ? default_style.margin.type : style.margin.type;
        if (margin & STYLE_TOP) {
            const int size = style.margin.top < 0 ? default_style.margin.top : style.margin.top;
            rect.y += size;
            rect.h -= size;
        }
        if (margin & STYLE_BOTTOM) {
            const int size = style.margin.bottom < 0 ? default_style.margin.bottom : style.margin.bottom;
            rect.h -= size;
        }
        if (margin & STYLE_LEFT) {
            const int size = style.margin.left < 0 ? default_style.margin.left : style.margin.left;
            rect.x += size;
            rect.w -= size;
        }
        if (margin & STYLE_RIGHT) {
            const int size = style.margin.right < 0 ? default_style.margin.right : style.margin.right;
            rect.w -= size;
        }
    }
}

void DrawingContext::padding(Rect &rect, Style &style) {
    if (style.padding.type != STYLE_NONE) {
        const int padding = style.padding.type == STYLE_DEFAULT ? default_style.padding.type : style.padding.type;
        if (padding & STYLE_TOP) {
            const int size = style.padding.top < 0 ? default_style.padding.top : style.padding.top;
            rect.y += size;
            rect.h -= size;
        }
        if (padding & STYLE_BOTTOM) {
            const int size = style.padding.bottom < 0 ? default_style.padding.bottom : style.padding.bottom;
            rect.h -= size;
        }
        if (padding & STYLE_LEFT) {
            const int size = style.padding.left < 0 ? default_style.padding.left : style.padding.left;
            rect.x += size;
            rect.w -= size;
        }
        if (padding & STYLE_RIGHT) {
            const int size = style.padding.right < 0 ? default_style.padding.right : style.padding.right;
            rect.w -= size;
        }
    }
}

void DrawingContext::sizeHintMargin(Size &size, Style &style) {
    if (style.margin.type != STYLE_NONE) {
        const int margin = style.margin.type == STYLE_DEFAULT ? default_style.margin.type : style.margin.type;
        if (margin & STYLE_TOP) {
            size.h += style.margin.top < 0 ? default_style.margin.top : style.margin.top;
        }
        if (margin & STYLE_BOTTOM) {
            size.h += style.margin.bottom < 0 ? default_style.margin.bottom : style.margin.bottom;
        }
        if (margin & STYLE_LEFT) {
            size.w += style.margin.left < 0 ? default_style.margin.left : style.margin.left;
        }
        if (margin & STYLE_RIGHT) {
            size.w += style.margin.right < 0 ? default_style.margin.right : style.margin.right;
        }
    }
}

void DrawingContext::sizeHintBorder(Size &size, Style &style) {
    if (style.border.type != STYLE_NONE) {
        const int border = style.border.type == STYLE_DEFAULT ? default_style.border.type : style.border.type;
        if (border & STYLE_TOP) {
            size.h += style.border.top < 0 ? default_style.border.top : style.border.top;
        }
        if (border & STYLE_BOTTOM) {
            size.h += style.border.bottom < 0 ? default_style.border.bottom : style.border.bottom;
        }
        if (border & STYLE_LEFT) {
            size.w += style.border.left < 0 ? default_style.border.left : style.border.left;
        }
        if (border & STYLE_RIGHT) {
            size.w += style.border.right < 0 ? default_style.border.right : style.border.right;
        }
    }
}

void DrawingContext::sizeHintPadding(Size &size, Style &style) {
    if (style.padding.type != STYLE_NONE) {
        const int padding = style.padding.type == STYLE_DEFAULT ? default_style.padding.type : style.padding.type;
        if (padding & STYLE_TOP) {
            size.h += style.padding.top < 0 ? default_style.padding.top : style.padding.top;
        }
        if (padding & STYLE_BOTTOM) {
            size.h += style.padding.bottom < 0 ? default_style.padding.bottom : style.padding.bottom;
        }
        if (padding & STYLE_LEFT) {
            size.w += style.padding.left < 0 ? default_style.padding.left : style.padding.left;
        }
        if (padding & STYLE_RIGHT) {
            size.w += style.padding.right < 0 ? default_style.padding.right : style.padding.right;
        }
    }
}

void DrawingContext::clear() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void DrawingContext::swap_buffer(SDL_Window *win) {
    SDL_GL_SwapWindow(win);
}

void DrawingContext::drawTexture(Point point, Size size, Texture *texture, TextureCoordinates *coords, Color color) {
    renderer->drawTexture(point, size, texture, coords, color);
}

void DrawingContext::drawTextureAligned(Rect rect, Size size, Texture *texture, TextureCoordinates *coords, HorizontalAlignment h_align, VerticalAlignment v_align, Color color) {
    Point pos = Point();
    switch (h_align) {
        case HorizontalAlignment::Left:
            pos.x = rect.x;
            break;
        case HorizontalAlignment::Right:
            pos.x = rect.x + rect.w - size.w;
            break;
        case HorizontalAlignment::Center:
            pos.x = rect.x + (rect.w / 2) - size.w / 2;
            break;
    }
    switch (v_align) {
        case VerticalAlignment::Top:
            pos.y = rect.y;
            break;
        case VerticalAlignment::Bottom:
            pos.y = rect.y + rect.h - size.h;
            break;
        case VerticalAlignment::Center:
            pos.y = rect.y + (rect.h / 2) - size.h / 2;
            break;
    }
    drawTexture(
        pos,
        size,
        texture,
        coords,
        color
    );
}

void DrawingContext::drawPoint(Point point, Color color) {
    this->fillRect(Rect(point.x, point.y, 1, 1), color);
}

void DrawingContext::setClip(Rect rect) {
    this->renderer->clip_rect = rect;
}

Rect DrawingContext::clip() {
    return this->renderer->clip_rect;
}

Color DrawingContext::windowBackground(Style &style) {
    return !style.window_background ? this->default_style.window_background : style.window_background;
}

Color DrawingContext::widgetBackground(Style &style) {
    return !style.widget_background ? this->default_style.widget_background : style.widget_background;
}

Color DrawingContext::accentWidgetBackground(Style &style) {
    return !style.accent_widget_background ? this->default_style.accent_widget_background : style.accent_widget_background;
}

Color DrawingContext::hoveredBackground(Style &style) {
    return !style.hovered_background ? this->default_style.hovered_background : style.hovered_background;
}

Color DrawingContext::pressedBackground(Style &style) {
    return !style.pressed_background ? this->default_style.pressed_background : style.pressed_background;
}

Color DrawingContext::accentHoveredBackground(Style &style) {
    return !style.accent_hovered_background ? this->default_style.accent_hovered_background : style.accent_hovered_background;
}

Color DrawingContext::accentPressedBackground(Style &style) {
    return !style.accent_pressed_background ? this->default_style.accent_pressed_background : style.accent_pressed_background;
}

Color DrawingContext::textForeground(Style &style) {
    return !style.text_foreground ? this->default_style.text_foreground : style.text_foreground;
}

Color DrawingContext::textBackground(Style &style) {
    return !style.text_background ? this->default_style.text_background : style.text_background;
}

Color DrawingContext::textSelected(Style &style) {
    return !style.text_selected ? this->default_style.text_selected : style.text_selected;
}

Color DrawingContext::textDisabled(Style &style) {
    return !style.text_disabled ? this->default_style.text_disabled : style.text_disabled;
}

Color DrawingContext::iconForeground(Style &style) {
    return !style.icon_foreground ? this->default_style.icon_foreground : style.icon_foreground;
}

Color DrawingContext::borderBackground(Style &style) {
    return !style.border_background ? this->default_style.border_background : style.border_background;
}

Color DrawingContext::borderTopBackground(Style &style) {
    return !style.border.color_top ? this->default_style.border.color_top : style.border.color_top;
}

Color DrawingContext::borderBottomBackground(Style &style) {
    return !style.border.color_bottom ? this->default_style.border.color_bottom : style.border.color_bottom;
}

Color DrawingContext::borderLeftBackground(Style &style) {
    return !style.border.color_left ? this->default_style.border.color_left : style.border.color_left;
}

Color DrawingContext::borderRightBackground(Style &style) {
    return !style.border.color_right ? this->default_style.border.color_right : style.border.color_right;
}

Color DrawingContext::getColor(Point point) {
    float data[4] = { 0, 0, 0, 0 };
    Size win = Application::get()->size;
    if (point.x >= win.w || point.y >= win.h) { return COLOR_NONE; }
    glReadPixels(point.x, win.h - point.y, 1, 1, GL_RGBA, GL_FLOAT, data);
    return Color(data[0], data[1], data[2], data[3]);
}
