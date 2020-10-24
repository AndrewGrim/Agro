#include "drawing_context.hpp"

DrawingContext::DrawingContext() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    quadRenderer = new QuadRenderer(indices);
    textRenderer = new TextRenderer(indices);
}

DrawingContext::~DrawingContext() {
}

void DrawingContext::fillRect(Rect<float> rect, Color color) {
    if (lastRenderer != (void*)quadRenderer) textRenderer->render();
    lastRenderer = (void*)quadRenderer;
    quadRenderer->fillRect(rect, color);
}

void DrawingContext::fillGradientRect(Rect<float> rect, Color fromColor, Color toColor, QuadRenderer::Gradient orientation) {
    if (lastRenderer != (void*)quadRenderer) textRenderer->render();
    lastRenderer = (void*)quadRenderer;
    quadRenderer->fillGradientRect(rect, fromColor, toColor, orientation);
}

void DrawingContext::render() {
    quadRenderer->render();
    textRenderer->render();
}

void DrawingContext::fillText(std::string text, float x, float y, Color color, float scale) {
    if (lastRenderer != (void*)textRenderer) quadRenderer->render();
    lastRenderer = (void*)textRenderer;
    textRenderer->fillText(text, x, y, color, scale);
}

Size<float> DrawingContext::measureText(std::string text, float scale) {
    return textRenderer->measureText(text, scale);
}

void DrawingContext::fillTextAligned(std::string text, TextAlignment alignment, Rect<float> rect, int padding, Color color) {
    switch (alignment) {
        case TextAlignment::Center:
            this->fillText(
                text,
                rect.x + (rect.w / 2) - (this->measureText(text).w / 2),
                rect.y + (rect.h / 2) - (this->measureText(text).h / 2),
                color
            );
            break;
        case TextAlignment::Right:
            this->fillText(
                text, 
                (rect.x + rect.w) - (this->measureText(text).w + padding), 
                rect.y + (rect.h / 2) - (this->measureText(text).h / 2),
                color
            );
            break;
        default:
            this->fillText(
                text, 
                rect.x + padding, 
                rect.y + (rect.h / 2) - (this->measureText(text).h / 2),
                color
            );
    }
}

Rect<float> DrawingContext::drawBorder(Rect<float> rect, int border_width, Color rect_color) {
    // TODO this is slow as shit
    // light border
    {
        // bottom layer of the top & left border : white, drawn first so that the top layer will paint over some extra pixels from here
        this->fillRect(
            Rect<float>(rect.x,
                rect.y + border_width / 2,
                rect.w,
                border_width / 2
            ), Color(1.0f, 1.0f, 1.0f)
        );
        this->fillRect(
            Rect<float>(rect.x + border_width / 2,
                rect.y,
                border_width / 2,
                rect.h
            ), Color(1.0f, 1.0f, 1.0f)
        );
        // top layer of the top & left border : background
        this->fillRect(
            Rect<float>(rect.x,
                rect.y,
                rect.w,
                border_width / 2
            ), rect_color
        );
        this->fillRect(
            Rect<float>(rect.x,
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
            Rect<float>(rect.x + border_width / 2,
                rect.y + rect.h - border_width,
                rect.w - border_width / 2,
                border_width / 2
            ), Color(0.4f, 0.4f, 0.4f)
        );
        this->fillRect(
            Rect<float>(rect.x + rect.w - border_width,
                rect.y + border_width / 2,
                border_width / 2,
                rect.h - border_width / 2
            ), Color(0.4f, 0.4f, 0.4f)
        );
        // bottom layer of the bottom & right border : black
        this->fillRect(
            Rect<float>(rect.x,
                rect.y + rect.h - border_width / 2,
                rect.w,
                border_width / 2
            ), Color(0.0f, 0.0f, 0.0f)
        );
        this->fillRect(
            Rect<float>(rect.x + rect.w - border_width / 2,
                rect.y,
                border_width / 2,
                rect.h
            ), Color(0.0f, 0.0f, 0.0f)
        );
    }

    // resize rectangle to account for border
    rect = Rect<float>(
        rect.x + border_width, 
        rect.y + border_width, 
        rect.w - border_width * 2, 
        rect.h - border_width * 2
    );

    return rect;
}

void DrawingContext::clear() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void DrawingContext::swap_buffer(SDL_Window *win) {
    SDL_GL_SwapWindow(win);
}