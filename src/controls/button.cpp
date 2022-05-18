#include "button.hpp"

Button::Button(String text) {
    setText(text);
    bind(SDLK_SPACE, Mod::None, [&](){
        activate();
    });
}

Button::Button(Image *image) {
    setImage(image);
}

Button::~Button() {
    if (m_image) {
        delete m_image;
    }
}

const char* Button::name() {
    return "Button";
}

void Button::draw(DrawingContext &dc, Rect rect, i32 state) {
    Color color;
    if (state & STATE_PRESSED && state & STATE_HOVERED) {
        color = dc.pressedBackground(style());
    } else if (state & STATE_HOVERED) {
        color = dc.hoveredBackground(style());
    } else {
        color = dc.widgetBackground(style());
    }

    dc.margin(rect, style());
    this->rect = rect;
    dc.drawBorder(rect, style(), state);
    Rect focus_rect = rect;
    dc.fillRect(rect, color);
    dc.padding(rect, style());

    Size text_size = dc.measureText(font(), text());
    if (text().size()) { text_size.w += 5; } // Padding between image and text
    if (m_image) {
        Size image_size = m_image->sizeHint(dc);
        dc.drawTexture(
            Point(
                rect.x + (rect.w / 2 - text_size.w / 2) - image_size.w / 2,
                rect.y + (rect.h * 0.5) - (image_size.h * 0.5)
            ),
            image_size, // TODO we dont support expandable images here
            m_image->_texture(),
            m_image->coords(),
            m_image->foreground()
        );
        // Resize rect to account for image before the label is drawn.
        rect.x += image_size.w;
        rect.w -= image_size.w;
    }
    HorizontalAlignment h_text_align = m_horizontal_align;
    VerticalAlignment v_text_align = m_vertical_align;
    if (m_image) {
        h_text_align = HorizontalAlignment::Center;
        v_text_align = VerticalAlignment::Center;
    }
    if (m_text.size()) {
        dc.fillTextAligned(
            font(),
            m_text,
            h_text_align,
            v_text_align,
            rect,
            0,
            dc.textForeground(style())
        );
    }
    dc.drawKeyboardFocus(focus_rect, style(), state);
}

Size Button::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size size = dc.measureText(font(), text());
        if (m_image) {
            Size i = m_image->sizeHint(dc);
            size.w += i.w;
            if (text().size()) { size.w += 5; } // Padding between image and text
            if (i.h > size.h) {
                size.h = i.h;
            }
        }

        dc.sizeHintMargin(size, style());
        dc.sizeHintBorder(size, style());
        dc.sizeHintPadding(size, style());

        m_size = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}

String Button::text() {
    return m_text;
}

Button* Button::setText(String text) {
    m_text = text;
    layout(LAYOUT_STYLE);

    return this;
}

HorizontalAlignment Button::horizontalAlignment() {
    return m_horizontal_align;
}

Button* Button::setHorizontalAlignment(HorizontalAlignment text_align) {
    if (m_horizontal_align != text_align) {
        m_horizontal_align = text_align;
        update();
    }

    return this;
}

VerticalAlignment Button::verticalAlignment() {
    return m_vertical_align;
}

Button* Button::setVerticalAlignment(VerticalAlignment text_align) {
    if (m_vertical_align != text_align) {
        m_vertical_align = text_align;
        update();
    }

    return this;
}

Image* Button::image() {
    return m_image;
}

Button* Button::setImage(Image *image) {
    if (m_image) {
        delete m_image;
    }
    m_image = image;
    layout(LAYOUT_STYLE);

    return this;
}

i32 Button::isFocusable() {
    return (i32)FocusType::Focusable;
}
