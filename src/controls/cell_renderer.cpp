#include "cell_renderer.hpp"

CellRenderer::CellRenderer() {}
CellRenderer::~CellRenderer() {}

bool CellRenderer::isWidget() {
    return false;
}

EmptyCell::EmptyCell() {}
EmptyCell::~EmptyCell() {}

void EmptyCell::draw(DrawingContext &dc, Rect rect, int state) {
    if (state & STATE_FOCUSED) {
        dc.fillRect(rect, Color(0.2f, 0.5f, 1.0f));
    }
    if (state & STATE_HOVERED) {
        dc.fillRect(rect, Color(0.4f, 0.4f, 0.4f, 0.1f));
    }
}

Size EmptyCell::sizeHint(DrawingContext &dc) {
    return Size();
}

TextCellRenderer::TextCellRenderer(
    std::string text, Color foreground, Color background, int padding
) :
text{text}, foreground{foreground},
background{background}, padding{padding} {}

TextCellRenderer::~TextCellRenderer() {

}

void TextCellRenderer::draw(DrawingContext &dc, Rect rect, int state) {
    Color fg = foreground;
    Color bg = background;
    if (state & STATE_FOCUSED) {
        fg = COLOR_WHITE;
        bg = Color(0.2f, 0.5f, 1.0f);
    }

    dc.fillRect(rect, bg);
    dc.fillTextMultilineAligned(
        font,
        text,
        h_align,
        v_align,
        rect,
        padding,
        fg
    );

    if (state & STATE_HOVERED) {
        dc.fillRect(rect, Color(0.4f, 0.4f, 0.4f, 0.1f));
    }
}

Size TextCellRenderer::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size s = dc.measureTextMultiline(font, text);
            s.w += padding * 2;
            s.h += padding * 2;
        m_size = s;
        return s;
    } else {
        return m_size;
    }
}

ImageCellRenderer::ImageCellRenderer(Image *image) : image{image} {}

ImageCellRenderer::~ImageCellRenderer() {
    delete image;
}

void ImageCellRenderer::draw(DrawingContext &dc, Rect rect, int state) {
    Color bg = dc.widgetBackground(image->style);
    if (state & STATE_FOCUSED) {
        bg = Color(0.2f, 0.5f, 1.0f);
    }
    dc.fillRect(
        rect,
        bg
    );
    dc.drawTextureAligned(
        rect,
        image->size(),
        image->_texture(),
        image->coords(),
        HorizontalAlignment::Center,
        VerticalAlignment::Center,
        image->foreground()
    );
    if (state & STATE_HOVERED) {
        dc.fillRect(rect, Color(0.4f, 0.4f, 0.4f, 0.1f));
    }

}

Size ImageCellRenderer::sizeHint(DrawingContext &dc) {
    return image->size();
}

MultipleImagesCellRenderer::MultipleImagesCellRenderer(std::vector<Image> &&images) : images{images} {}
MultipleImagesCellRenderer::~MultipleImagesCellRenderer() {}

void MultipleImagesCellRenderer::draw(DrawingContext &dc, Rect rect, int state) {
    Color bg = COLOR_NONE;
    if (state & STATE_FOCUSED) {
        bg = Color(0.2f, 0.5f, 1.0f);
    }
    dc.fillRect(
        rect,
        bg
    );
    Rect drawing_rect = rect;
    Size size = sizeHint(dc);
    drawing_rect.x = drawing_rect.x + (drawing_rect.w / 2) - (size.w / 2);
    for (auto img : images) {
        dc.drawTextureAligned(
            drawing_rect,
            img.size(),
            img._texture(),
            img.coords(),
            HorizontalAlignment::Left,
            VerticalAlignment::Center,
            img.foreground()
        );
        drawing_rect.x += img.size().w;
    }
    if (state & STATE_HOVERED) {
        dc.fillRect(rect, Color(0.4f, 0.4f, 0.4f, 0.1f));
    }
}

Size MultipleImagesCellRenderer::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size size = Size();
        for (auto img : images) {
            Size s = img.sizeHint(dc);
            size.w += s.w;
            if (s.h > size.h) { size.h = s.h; }
        }
        m_size = size;
    }
    return m_size;
}

ImageTextCellRenderer::ImageTextCellRenderer(
    Image *image,
    std::string text,
    Color foreground,
    Color background,
    HorizontalAlignment h_align,
    int padding
) :
image{image}, text{text}, foreground{foreground},
background{background}, h_align{h_align}, padding{padding} {
}

ImageTextCellRenderer::~ImageTextCellRenderer() {
    delete image;
}

void ImageTextCellRenderer::draw(DrawingContext &dc, Rect rect, int state) {
    Color fg = foreground;
    Color bg = background;
    if (state & STATE_FOCUSED) {
        fg = COLOR_WHITE;
        bg = Color(0.2f, 0.5f, 1.0f);
    }

    dc.fillRect(rect, bg);
    Size text_size = dc.measureTextMultiline(font, text);
    Rect local_rect = rect;
    Size image_size = image->size();
    float x = rect.x;
    switch (h_align) {
        case HorizontalAlignment::Right: x = rect.x + rect.w - (image_size.w + text_size.w + (padding * 2)); break;
        case HorizontalAlignment::Center: x = rect.x + (rect.w / 2) - ((image_size.w + text_size.w + (padding * 2)) / 2); break;
        default: break;
    }
    dc.drawTexture(
        Point(
            x,
            local_rect.y + (local_rect.h * 0.5) - (image_size.h * 0.5)
        ),
        image_size,
        image->_texture(),
        image->coords(),
        image->foreground()
    );
    // Resize local_rect to account for image before the label is drawn.
    local_rect.x += image_size.w;
    local_rect.w -= image_size.w;
    dc.fillTextMultilineAligned(
        font,
        text,
        h_align,
        VerticalAlignment::Center,
        local_rect,
        padding,
        fg
    );


    if (state & STATE_HOVERED) {
        dc.fillRect(rect, Color(0.4f, 0.4f, 0.4f, 0.1f));
    }
}

Size ImageTextCellRenderer::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size s = dc.measureTextMultiline(font, text);
            s.w += image->size().w;
            if (image->size().h > s.h) {
                s.h = image->size().h;
            }
            s.w += padding * 2;
            s.h += padding * 2;
        m_size = s;
        return s;
    } else {
        return m_size;
    }
}
