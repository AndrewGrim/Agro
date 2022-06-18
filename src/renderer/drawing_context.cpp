#include "drawing_context.hpp"
#include "../application.hpp"
#include "../slice.hpp"

DrawingContext::DrawingContext(Window *window) : window{window} {
    u32 offset = 0;
    u32 indexCount = MAX_BATCH_SIZE * QUAD_INDEX_COUNT;
    for (u32 i = 0; i < indexCount; i += QUAD_INDEX_COUNT) {
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
        Style::BorderColor {
            COLOR_DEFAULT,
            COLOR_DEFAULT,
            COLOR_DEFAULT,
            COLOR_DEFAULT
        },

        Style::Border {
            STYLE_ALL,
            1,
            1,
            1,
            1
        },

        Style::Margin {
            STYLE_ALL,
            1,
            1,
            1,
            1
        },

        Style::Padding {
            STYLE_ALL,
            5,
            5,
            5,
            5
        },

        nullptr,

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
        Style::BorderColor {
            COLOR_DEFAULT,
            COLOR_DEFAULT,
            COLOR_DEFAULT,
            COLOR_DEFAULT
        },

        Style::Border {
            STYLE_ALL,
            1,
            1,
            1,
            1
        },

        Style::Margin {
            STYLE_ALL,
            1,
            1,
            1,
            1
        },

        Style::Padding {
            STYLE_ALL,
            5,
            5,
            5,
            5
        },

        nullptr,

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

void DrawingContext::fillText(std::shared_ptr<Font> font, String text, Point point, Color color, i32 tab_width, Renderer::Selection selection, Color selection_color) {
    renderer->fillText(font, Slice<const char>(text.data(), text.size()), point, color, tab_width, false, 0, selection, selection_color);
}

void DrawingContext::fillTextMultiline(std::shared_ptr<Font> font, String text, Point point, Color color, i32 tab_width, i32 line_spacing, Renderer::Selection selection, Color selection_color) {
    renderer->fillText(font, Slice<const char>(text.data(), text.size()), point, color, tab_width, true, line_spacing, selection, selection_color);
}

void DrawingContext::fillTextAligned(std::shared_ptr<Font> font, String text, HorizontalAlignment h_align, VerticalAlignment v_align, Rect rect, i32 padding, Color color, i32 tab_width, Renderer::Selection selection, Color selection_color) {
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

void DrawingContext::fillTextAligned(std::shared_ptr<Font> font, Slice<const char> text, HorizontalAlignment h_align, VerticalAlignment v_align, Rect rect, i32 padding, Color color, i32 tab_width, Renderer::Selection selection, Color selection_color) {
    fillTextAligned(font, String(text.data), h_align, v_align, rect, padding, color, tab_width, selection, selection_color);
}

void DrawingContext::fillTextMultilineAligned(std::shared_ptr<Font> font, String text, HorizontalAlignment h_align, VerticalAlignment v_align, Rect rect, i32 padding, Color color, i32 tab_width, i32 line_spacing, Renderer::Selection selection, Color selection_color) {
    font = font;
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

    i32 line_width = 0;
    const char *start = text.data();
    u64 count = 0;
    u8 length = 0;
    for (u64 i = 0; i < text.size(); i += length) {
        length = utf8::length(text.data() + i);
        line_width += measureText(font, Slice<const char>(text.data() + i, length), tab_width).w;
        count += length;
        if (text.data()[i] == '\n') {
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
            pos.y += font->maxHeight() + line_spacing;
            if (pos.y >= renderer->clip_rect.y + renderer->clip_rect.h) { return; }
            start += count;
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

void DrawingContext::fillTextMultilineAligned(std::shared_ptr<Font> font, Slice<const char> text, HorizontalAlignment h_align, VerticalAlignment v_align, Rect rect, i32 padding, Color color, i32 tab_width, i32 line_spacing, Renderer::Selection selection, Color selection_color) {
    fillTextMultilineAligned(font, String(text.data), h_align, v_align, rect, padding, color, tab_width, line_spacing, selection, selection_color);
}

Size DrawingContext::measureText(std::shared_ptr<Font> font, Slice<const char> text, i32 tab_width) {
    return renderer->measureText(font, text, tab_width);
}

Size DrawingContext::measureText(std::shared_ptr<Font> font, String text, i32 tab_width) {
    return renderer->measureText(font, Slice<const char>(text.data(), text.size()), tab_width);
}

Size DrawingContext::measureTextMultiline(std::shared_ptr<Font> font, String text, i32 tab_width, i32 line_spacing) {
    return renderer->measureText(font, Slice<const char>(text.data(), text.size()), tab_width, true, line_spacing);
}

Size DrawingContext::measureTextMultiline(std::shared_ptr<Font> font, Slice<const char> text, i32 tab_width, i32 line_spacing) {
    return renderer->measureText(font, text, tab_width, true, line_spacing);
}

Rect DrawingContext::drawBorder3D(Rect rect, i32 border_width, Color rect_color) {
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

void DrawingContext::drawBorder(Rect &rect, Style &style, i32 state) {
    if (style.border.type != STYLE_NONE) {
        const i32 border = style.border.type == STYLE_DEFAULT ? default_style.border.type : style.border.type;
        if (border & STYLE_TOP) {
            const i32 size = style.border.top < 0 ? default_style.border.top : style.border.top;
            fillRect(Rect(rect.x, rect.y, rect.w, size), state & STATE_HARD_FOCUSED ? accentWidgetBackground(style) : borderTopBackground(style));
            rect.y += size;
            rect.h -= size;
        }
        if (border & STYLE_BOTTOM) {
            const i32 size = style.border.bottom < 0 ? default_style.border.bottom : style.border.bottom;
            rect.h -= size;
            fillRect(Rect(rect.x, rect.y + rect.h, rect.w, size), state & STATE_HARD_FOCUSED ? accentWidgetBackground(style) : borderBottomBackground(style));
        }
        if (border & STYLE_LEFT) {
            const i32 size = style.border.left < 0 ? default_style.border.left : style.border.left;
            fillRect(Rect(rect.x, rect.y, size, rect.h), state & STATE_HARD_FOCUSED ? accentWidgetBackground(style) : borderLeftBackground(style));
            rect.x += size;
            rect.w -= size;
        }
        if (border & STYLE_RIGHT) {
            const i32 size = style.border.right < 0 ? default_style.border.right : style.border.right;
            rect.w -= size;
            fillRect(Rect(rect.x + rect.w, rect.y, size, rect.h), state & STATE_HARD_FOCUSED ? accentWidgetBackground(style) : borderRightBackground(style));
        }
    }
}

void DrawingContext::drawKeyboardFocus(Rect &rect, Style &style, i32 state) {
    if (state & STATE_SOFT_FOCUSED) {
        if (state & STATE_HARD_FOCUSED) {
            drawDashedRect(rect, accentWidgetBackground(style));
        } else {
            drawDashedRect(rect, borderBackground(style));
        }
    }
}

void DrawingContext::margin(Rect &rect, Style &style) {
    if (style.margin.type != STYLE_NONE) {
        const i32 margin = style.margin.type == STYLE_DEFAULT ? default_style.margin.type : style.margin.type;
        if (margin & STYLE_TOP) {
            const i32 size = style.margin.top < 0 ? default_style.margin.top : style.margin.top;
            rect.y += size;
            rect.h -= size;
        }
        if (margin & STYLE_BOTTOM) {
            const i32 size = style.margin.bottom < 0 ? default_style.margin.bottom : style.margin.bottom;
            rect.h -= size;
        }
        if (margin & STYLE_LEFT) {
            const i32 size = style.margin.left < 0 ? default_style.margin.left : style.margin.left;
            rect.x += size;
            rect.w -= size;
        }
        if (margin & STYLE_RIGHT) {
            const i32 size = style.margin.right < 0 ? default_style.margin.right : style.margin.right;
            rect.w -= size;
        }
    }
}

void DrawingContext::padding(Rect &rect, Style &style) {
    if (style.padding.type != STYLE_NONE) {
        const i32 padding = style.padding.type == STYLE_DEFAULT ? default_style.padding.type : style.padding.type;
        if (padding & STYLE_TOP) {
            const i32 size = style.padding.top < 0 ? default_style.padding.top : style.padding.top;
            rect.y += size;
            rect.h -= size;
        }
        if (padding & STYLE_BOTTOM) {
            const i32 size = style.padding.bottom < 0 ? default_style.padding.bottom : style.padding.bottom;
            rect.h -= size;
        }
        if (padding & STYLE_LEFT) {
            const i32 size = style.padding.left < 0 ? default_style.padding.left : style.padding.left;
            rect.x += size;
            rect.w -= size;
        }
        if (padding & STYLE_RIGHT) {
            const i32 size = style.padding.right < 0 ? default_style.padding.right : style.padding.right;
            rect.w -= size;
        }
    }
}

void DrawingContext::sizeHintMargin(Size &size, Style &style) {
    if (style.margin.type != STYLE_NONE) {
        const i32 margin = style.margin.type == STYLE_DEFAULT ? default_style.margin.type : style.margin.type;
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
        const i32 border = style.border.type == STYLE_DEFAULT ? default_style.border.type : style.border.type;
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
        const i32 padding = style.padding.type == STYLE_DEFAULT ? default_style.padding.type : style.padding.type;
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
    return !style.window_background_color ? this->default_style.window_background_color : style.window_background_color;
}

Color DrawingContext::widgetBackground(Style &style) {
    return !style.widget_background_color ? this->default_style.widget_background_color : style.widget_background_color;
}

Color DrawingContext::accentWidgetBackground(Style &style) {
    return !style.accent_widget_background_color ? this->default_style.accent_widget_background_color : style.accent_widget_background_color;
}

Color DrawingContext::hoveredBackground(Style &style) {
    return !style.hovered_background_color ? this->default_style.hovered_background_color : style.hovered_background_color;
}

Color DrawingContext::pressedBackground(Style &style) {
    return !style.pressed_background_color ? this->default_style.pressed_background_color : style.pressed_background_color;
}

Color DrawingContext::accentHoveredBackground(Style &style) {
    return !style.accent_hovered_background_color ? this->default_style.accent_hovered_background_color : style.accent_hovered_background_color;
}

Color DrawingContext::accentPressedBackground(Style &style) {
    return !style.accent_pressed_background_color ? this->default_style.accent_pressed_background_color : style.accent_pressed_background_color;
}

Color DrawingContext::textForeground(Style &style) {
    return !style.text_foreground_color ? this->default_style.text_foreground_color : style.text_foreground_color;
}

Color DrawingContext::textBackground(Style &style) {
    return !style.text_background_color ? this->default_style.text_background_color : style.text_background_color;
}

Color DrawingContext::textSelected(Style &style) {
    return !style.text_selected_color ? this->default_style.text_selected_color : style.text_selected_color;
}

Color DrawingContext::textDisabled(Style &style) {
    return !style.text_disabled_color ? this->default_style.text_disabled_color : style.text_disabled_color;
}

Color DrawingContext::iconForeground(Style &style) {
    return !style.icon_foreground_color ? this->default_style.icon_foreground_color : style.icon_foreground_color;
}

Color DrawingContext::borderBackground(Style &style) {
    return !style.border_background_color ? this->default_style.border_background_color : style.border_background_color;
}

Color DrawingContext::borderTopBackground(Style &style) {
    return !style.border_color.top ?
                !default_style.border_color.top ?
                    !style.border_background_color ?
                        default_style.border_background_color
                    : style.border_background_color
                : default_style.border_color.top
            : style.border_color.top;
}

Color DrawingContext::borderBottomBackground(Style &style) {
    return !style.border_color.bottom ?
                !default_style.border_color.bottom ?
                    !style.border_background_color ?
                        default_style.border_background_color
                    : style.border_background_color
                : default_style.border_color.bottom
            : style.border_color.bottom;
}

Color DrawingContext::borderLeftBackground(Style &style) {
    return !style.border_color.left ?
                !default_style.border_color.left ?
                    !style.border_background_color ?
                        default_style.border_background_color
                    : style.border_background_color
                : default_style.border_color.left
            : style.border_color.left;
}

Color DrawingContext::borderRightBackground(Style &style) {
    return !style.border_color.right ?
                !default_style.border_color.right ?
                    !style.border_background_color ?
                        default_style.border_background_color
                    : style.border_background_color
                : default_style.border_color.right
            : style.border_color.right;
}

Color DrawingContext::getColor(Point point) {
    f32 data[4] = { 0, 0, 0, 0 };
    Size window_size = window->size;
    if (point.x >= window_size.w || point.y >= window_size.h) { return COLOR_NONE; }
    glReadPixels(point.x, window_size.h - point.y, 1, 1, GL_RGBA, GL_FLOAT, data);
    return Color(data[0], data[1], data[2], data[3]);
}
