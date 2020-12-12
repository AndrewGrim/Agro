#include "drawing_context.hpp"

DrawingContext::DrawingContext(void *app) {
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
    textRenderer = new TextRenderer(indices, app);
}

DrawingContext::~DrawingContext() {
    delete quadRenderer;
    delete textRenderer;
}

void DrawingContext::fillRect(Rect rect, Color color) {
    if (lastRenderer != (void*)quadRenderer) textRenderer->render();
    lastRenderer = (void*)quadRenderer;
    quadRenderer->fillRect(rect, color);
}

void DrawingContext::fillGradientRect(Rect rect, Color fromColor, Color toColor, QuadRenderer::Gradient orientation) {
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

Size DrawingContext::measureText(std::string text, float scale) {
    return textRenderer->measureText(text, scale);
}

Size DrawingContext::measureText(char c, float scale) {
    return textRenderer->measureText(c, scale);
}

void DrawingContext::fillTextAligned(std::string text, TextAlignment alignment, Rect rect, int padding, Color color) {
    // The reason for the additional calculations here is because in order
    // to avoid horrible texture wrapping issues on text we need to give it a
    // nice even number to start from.
    //
    // Before: "rect.x + (rect.w * 0.5) - (this->measureText(text).w * 0.5)".
    // After: "std::nearbyint((rect.x + (rect.w * 0.5) - (this->measureText(text).w * 0.5)) * 0.5) * 2.0".
    // 
    // This causes something that looks like dithering which is caused
    // by the rounding of x and or y.
    // This is only really noticeable when scrolling or resizing a window
    // and doing it slowely, if you just resize the window as you would
    // normally this isn't really perceptible.

    auto normalize = [](float coordinate) {
        int rounded = std::nearbyint(coordinate);
        return !(rounded % 2) ? (rounded * 0.5) * 2.0 : rounded;
    }; 
    
    switch (alignment) {
        case TextAlignment::Center:
            this->fillText(
                text,
                normalize(rect.x + (rect.w * 0.5) - (this->measureText(text).w * 0.5)),
                normalize(rect.y + (rect.h * 0.5) - (this->measureText(text).h * 0.5)),
                color
            );
            break;
        case TextAlignment::Right:
            this->fillText(
                text, 
                normalize((rect.x + rect.w) - (this->measureText(text).w + padding)),
                normalize(rect.y + (rect.h * 0.5) - (this->measureText(text).h * 0.5)),
                color
            );
            break;
        default:
            this->fillText(
                text, 
                normalize(rect.x + padding),
                normalize(rect.y + (rect.h * 0.5) - (this->measureText(text).h * 0.5)),
                color
            );
    }
}

Rect DrawingContext::drawBorder(Rect rect, int border_width, Color rect_color) {
    // TODO this is slow as shit
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

void DrawingContext::clear() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void DrawingContext::swap_buffer(SDL_Window *win) {
    SDL_GL_SwapWindow(win);
}