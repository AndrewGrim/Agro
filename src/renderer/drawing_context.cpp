#include <cmath>

#include "drawing_context.hpp"

DrawingContext::DrawingContext(void *app) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned int offset = 0;
    unsigned int indexCount = MAX_BATCH_SIZE * QUAD_INDEX_COUNT;
    for (int i = 0; i < indexCount; i += QUAD_INDEX_COUNT) {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;
        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;
        offset += 4;
    }

    renderer = new Renderer(indices, app);

    default_style = new Style();
    {
        default_style->margin = STYLE_ALL;
        default_style->margin_top = 1;
        default_style->margin_bottom = 1;
        default_style->margin_left = 1;
        default_style->margin_right = 1;

        default_style->border = STYLE_ALL;
        default_style->border_top = 1;
        default_style->border_top_color = COLOR_BLACK;
        default_style->border_bottom = 1;
        default_style->border_bottom_color = COLOR_BLACK;
        default_style->border_left = 1;
        default_style->border_left_color = COLOR_BLACK;
        default_style->border_right = 1;
        default_style->border_right_color = COLOR_BLACK;

        default_style->padding = STYLE_ALL;
        default_style->padding_top = 5;
        default_style->padding_bottom = 5;
        default_style->padding_left = 5;
        default_style->padding_right = 5;

        default_style->text_color = COLOR_BLACK;
        default_style->background_color = Color(0.9, 0.9, 0.9);
        default_style->window_color = Color(0.5, 0.5, 0.5);
        default_style->selection_color = Color(0.2, 0.5, 1.0);
        default_style->hover_color = Color(0.5, 0.5, 0.5, 0.1);
    }
}

DrawingContext::~DrawingContext() {
    delete renderer;
    delete default_font;
    delete default_style;
}

void DrawingContext::fillRect(Rect rect, Color color) {
    renderer->fillRect(rect, color);
}

void DrawingContext::fillRectWithGradient(Rect rect, Color fromColor, Color toColor, Gradient orientation) {
    renderer->fillRectWithGradient(rect, fromColor, toColor, orientation);
}

void DrawingContext::render() {
    renderer->render();
}

void DrawingContext::fillText(Font *font, std::string text, Point point, Color color, float scale) {
    Point rounded = Point(round(point.x), round(point.y));
    renderer->fillText(font ? font : default_font, text, rounded, color, scale);
}

Size DrawingContext::measureText(Font *font, std::string text, float scale) {
    return renderer->measureText(font ? font : default_font, text, scale);
}

Size DrawingContext::measureText(Font *font, char c, float scale) {
    return renderer->measureText(font ? font : default_font, c, scale);
}

void DrawingContext::fillTextAligned(Font *font, std::string text, HorizontalAlignment h_align, VerticalAlignment v_align, Rect rect, int padding, Color color) {
    // The reason for the rounding here is because in order
    // to avoid horrible texture wrapping issues on text we need to give it a
    // nice whole number to start from.
    //
    // This causes something that looks like dithering which is caused
    // by the rounding of x and or y.
    // This is only really noticeable when scrolling or resizing a window
    // and doing it slowely, if you just resize the window as you would
    // normally this isn't really perceptible.
    Point pos = Point();
    switch (h_align) {
        case HorizontalAlignment::Left:
            pos.x = rect.x + padding;
            break;
        case HorizontalAlignment::Right:
            pos.x = (rect.x + rect.w) - (measureText(font, text).w + padding);
            break;
        case HorizontalAlignment::Center:
            pos.x = rect.x + (rect.w * 0.5) - (measureText(font, text).w * 0.5);
            break;
    }
    switch (v_align) {
        case VerticalAlignment::Top:
            pos.y = rect.y + padding;
            break;
        case VerticalAlignment::Bottom:
            pos.y = (rect.y + rect.h) - (measureText(font, text).h + padding);
            break;
        case VerticalAlignment::Center:
            pos.y = rect.y + (rect.h * 0.5) - (measureText(font, text).h * 0.5);
            break;
    }
    fillText(
        font,
        text,
        pos,
        color
    );
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

void DrawingContext::drawBorder(Rect &rect, Style *style) {
    const Style *_style = style ? style : default_style;
    // TODO lets table color for now
    // const Color border_color = 
    const Color border_color = default_style->border_top_color;

    if (_style->border != STYLE_NONE) {
        const int border = _style->border == STYLE_DEFAULT ? default_style->border : _style->border;
        if (border & STYLE_TOP) {
            const int size = _style->border_top < 0 ? default_style->border_top : _style->border_top;
            fillRect(Rect(rect.x, rect.y, rect.w, size), border_color);
            rect.y += size;
            rect.h -= size;
        }
        if (border & STYLE_BOTTOM) {
            const int size = _style->border_bottom < 0 ? default_style->border_bottom : _style->border_bottom;
            rect.h -= size;
            fillRect(Rect(rect.x, rect.y + rect.h, rect.w, size), border_color);
        }
        if (border & STYLE_LEFT) {
            const int size = _style->border_left < 0 ? default_style->border_left : _style->border_left;
            fillRect(Rect(rect.x, rect.y, size, rect.h), border_color);
            rect.x += size;
            rect.w -= size;
        }
        if (border & STYLE_RIGHT) {
            const int size = _style->border_right < 0 ? default_style->border_right : _style->border_right;
            rect.w -= size;
            fillRect(Rect(rect.x + rect.w, rect.y, size, rect.h), border_color);
        }
    }
}

void DrawingContext::margin(Rect &rect, Style *style) {
    const Style *_style = style ? style : default_style;

    if (_style->margin != STYLE_NONE) {
        const int margin = _style->margin == STYLE_DEFAULT ? default_style->margin : _style->margin;
        if (margin & STYLE_TOP) {
            const int size = _style->margin_top < 0 ? default_style->margin_top : _style->margin_top;
            rect.y += size;
            rect.h -= size;
        }
        if (margin & STYLE_BOTTOM) {
            const int size = _style->margin_bottom < 0 ? default_style->margin_bottom : _style->margin_bottom;
            rect.h -= size;
        }
        if (margin & STYLE_LEFT) {
            const int size = _style->margin_left < 0 ? default_style->margin_left : _style->margin_left;
            rect.x += size;
            rect.w -= size;
        }
        if (margin & STYLE_RIGHT) {
            const int size = _style->margin_right < 0 ? default_style->margin_right : _style->margin_right;
            rect.w -= size;
        }
    }
}

void DrawingContext::padding(Rect &rect, Style *style) {
    const Style *_style = style ? style : default_style;

    if (_style->padding != STYLE_NONE) {
        const int padding = _style->padding == STYLE_DEFAULT ? default_style->padding : _style->padding;
        if (padding & STYLE_TOP) {
            const int size = _style->padding_top < 0 ? default_style->padding_top : _style->padding_top;
            rect.y += size;
            rect.h -= size;
        }
        if (padding & STYLE_BOTTOM) {
            const int size = _style->padding_bottom < 0 ? default_style->padding_bottom : _style->padding_bottom;
            rect.w -= size;
        }
        if (padding & STYLE_LEFT) {
            const int size = _style->padding_left < 0 ? default_style->padding_left : _style->padding_left;
            rect.x += size;
            rect.w -= size;
        }
        if (padding & STYLE_RIGHT) {
            const int size = _style->padding_right < 0 ? default_style->padding_right : _style->padding_right;
            rect.w -= size;
        }
    }
}

void DrawingContext::sizeHintMargin(Size &size, Style *style) {
    const Style *_style = style ? style : default_style;
    
    if (_style->margin != STYLE_NONE) {
        const int margin = _style->margin == STYLE_DEFAULT ? default_style->margin : _style->margin;
        if (margin & STYLE_TOP) {
            size.h += _style->margin_top < 0 ? default_style->margin_top : _style->margin_top;
        }
        if (margin & STYLE_BOTTOM) {
            size.h += _style->margin_bottom < 0 ? default_style->margin_bottom : _style->margin_bottom;
        }
        if (margin & STYLE_LEFT) {
            size.w += _style->margin_left < 0 ? default_style->margin_left : _style->margin_left;
        }
        if (margin & STYLE_RIGHT) {
            size.w += _style->margin_right < 0 ? default_style->margin_right : _style->margin_right;
        }
    }
}

void DrawingContext::sizeHintBorder(Size &size, Style *style) {
    const Style *_style = style ? style : default_style;
    
    if (_style->border != STYLE_NONE) {
        const int border = _style->border == STYLE_DEFAULT ? default_style->border : _style->border;
        if (border & STYLE_TOP) {
            size.h += _style->border_top < 0 ? default_style->border_top : _style->border_top;
        }
        if (border & STYLE_BOTTOM) {
            size.h += _style->border_bottom < 0 ? default_style->border_bottom : _style->border_bottom;
        }
        if (border & STYLE_LEFT) {
            size.w += _style->border_left < 0 ? default_style->border_left : _style->border_left;
        }
        if (border & STYLE_RIGHT) {
            size.w += _style->border_right < 0 ? default_style->border_right : _style->border_right;
        }
    }
}

void DrawingContext::sizeHintPadding(Size &size, Style *style) {
    const Style *_style = style ? style : default_style;
    
    if (_style->padding != STYLE_NONE) {
        const int padding = _style->padding == STYLE_DEFAULT ? default_style->padding : _style->padding;
        if (padding & STYLE_TOP) {
            size.h += _style->padding_top < 0 ? default_style->padding_top : _style->padding_top;
        }
        if (padding & STYLE_BOTTOM) {
            size.h += _style->padding_bottom < 0 ? default_style->padding_bottom : _style->padding_bottom;
        }
        if (padding & STYLE_LEFT) {
            size.w += _style->padding_left < 0 ? default_style->padding_left : _style->padding_left;
        }
        if (padding & STYLE_RIGHT) {
            size.w += _style->padding_right < 0 ? default_style->padding_right : _style->padding_right;
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
    this->drawTexture(
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