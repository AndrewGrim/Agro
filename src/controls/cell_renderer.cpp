#include "cell_renderer.hpp"

CellRenderer::CellRenderer() {
    setWidgetBackgroundColor(COLOR_NONE);
}

CellRenderer::~CellRenderer() {}

EmptyCell::EmptyCell() {}
EmptyCell::~EmptyCell() {}

void EmptyCell::draw(DrawingContext &dc, Rect rect, i32 state) {
    Color bg = dc.accentWidgetBackground(style());
    if (state & STATE_HARD_FOCUSED) {
        dc.fillRect(rect, bg);
    }
    if (state & STATE_HOVERED) {
        dc.fillRect(rect, Color(bg.r, bg.g, bg.b, 0.2f));
    }
}

Size EmptyCell::sizeHint(DrawingContext &dc) {
    return Size();
}

TextCellRenderer::TextCellRenderer(String text) : text{text} {}

TextCellRenderer::~TextCellRenderer() {}

void TextCellRenderer::draw(DrawingContext &dc, Rect rect, i32 state) {
    Color fg = dc.textForeground(style());
    Color bg = dc.widgetBackground(style()); // TODO should be text_background most likely
    if (state & STATE_HARD_FOCUSED) {
        fg = dc.textSelected(style());
        bg = dc.accentWidgetBackground(style());
    }

    dc.fillRect(rect, bg);
    if (state & STATE_HOVERED) {
        bg = dc.accentWidgetBackground(style());
        dc.fillRect(rect, Color(bg.r, bg.g, bg.b, 0.2f));
    }
    dc.padding(rect, style());
    dc.fillTextMultilineAligned(
        font(),
        text.slice(),
        h_align,
        v_align,
        rect,
        0,
        fg
    );
}

Size TextCellRenderer::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size s = dc.measureTextMultiline(font(), text.slice());
        dc.sizeHintPadding(s, style());
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

void ImageCellRenderer::draw(DrawingContext &dc, Rect rect, i32 state) {
    Color bg = dc.widgetBackground(image->style());
    if (state & STATE_HARD_FOCUSED) {
        bg = dc.accentWidgetBackground(style());
    }
    dc.fillRect(rect, bg);
    if (state & STATE_HOVERED) {
        bg = dc.accentWidgetBackground(style());
        dc.fillRect(rect, Color(bg.r, bg.g, bg.b, 0.2f));
    }
    dc.padding(rect, style());
    dc.drawTextureAligned(
        rect,
        image->size(),
        image->_texture(),
        image->coords(),
        HorizontalAlignment::Center,
        VerticalAlignment::Center,
        image->foreground()
    );
}

Size ImageCellRenderer::sizeHint(DrawingContext &dc) {
    Size s = image->size();
    dc.sizeHintPadding(s, style());
    return s;
}

MultipleImagesCellRenderer::MultipleImagesCellRenderer(std::vector<Image> &&images) : images{images} {}
MultipleImagesCellRenderer::~MultipleImagesCellRenderer() {}

void MultipleImagesCellRenderer::draw(DrawingContext &dc, Rect rect, i32 state) {
    Color bg = dc.widgetBackground(style());
    if (state & STATE_HARD_FOCUSED) {
        bg = dc.accentWidgetBackground(style());
    }
    dc.fillRect(rect, bg);
    if (state & STATE_HOVERED) {
        bg = dc.accentWidgetBackground(style());
        dc.fillRect(rect, Color(bg.r, bg.g, bg.b, 0.2f));
    }
    dc.padding(rect, style());
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
}

Size MultipleImagesCellRenderer::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size size = Size();
        for (auto img : images) {
            Size s = img.sizeHint(dc);
            size.w += s.w;
            if (s.h > size.h) { size.h = s.h; }
        }
        dc.sizeHintPadding(size, style());
        m_size = size;
    }
    return m_size;
}

ImageTextCellRenderer::ImageTextCellRenderer(
    Image *image,
    String text,
    HorizontalAlignment h_align
) : image{image}, text{text}, h_align{h_align} {}

ImageTextCellRenderer::~ImageTextCellRenderer() {
    delete image;
}

void ImageTextCellRenderer::draw(DrawingContext &dc, Rect rect, i32 state) {
    Color fg = dc.textForeground(style());
    Color bg = dc.widgetBackground(style());
    if (state & STATE_HARD_FOCUSED) {
        fg = dc.textSelected(style());
        bg = dc.accentWidgetBackground(style());
    }

    dc.fillRect(rect, bg);
    if (state & STATE_HOVERED) {
        bg = dc.accentWidgetBackground(style());
        dc.fillRect(rect, Color(bg.r, bg.g, bg.b, 0.2f));
    }
    dc.padding(rect, style());
    Size text_size = dc.measureTextMultiline(font(), text.slice());
    Rect local_rect = rect;
    Size image_size = image->size();
    i32 x = rect.x;
    switch (h_align) {
        case HorizontalAlignment::Right: x = rect.x + rect.w - (image_size.w + text_size.w); break;
        case HorizontalAlignment::Center: x = rect.x + (rect.w / 2) - ((image_size.w + text_size.w) / 2); break;
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
        font(),
        text.slice(),
        h_align,
        VerticalAlignment::Center,
        local_rect,
        0,
        fg
    );
}

Size ImageTextCellRenderer::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size s = dc.measureTextMultiline(font(), text.slice());
            s.w += image->size().w;
            if (image->size().h > s.h) {
                s.h = image->size().h;
            }
            dc.sizeHintPadding(s, style());
        m_size = s;
        return s;
    } else {
        return m_size;
    }
}
