#include "button.hpp"

Button::Button(std::string text) {
    this->setText(text);
    Widget::m_bg = Color(0.9f, 0.9f, 0.9f);
}

Button::Button(Image *image) {
    this->setImage(image);
    Widget::m_bg = Color(0.9f, 0.9f, 0.9f);
}

Button::~Button() {
    if (m_image) {
        delete m_image;
    }
}

const char* Button::name() {
    return "Button";
}

void Button::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;

    Color color; 
    if (isPressed() && isHovered()) {
        color = m_pressed_bg; 
    } else if (isHovered()) {
        color = m_hovered_bg;
    } else {
        color = background();
    }
    
    // We pretend that the widget always gets enough space.
    // This allows us to draw the widget exactly how its meant to look.
    // Not explicitly clipping should not be a problem because
    // as long as there is a scrollable somewhere in the tree everything will get its
    // requested size and otherwise if its boxes all the way up
    // you only wont get enough space if the widget is going to go outside the window.
    Rect drawing_rect = Rect(
        rect.x, 
        rect.y, 
        sizeHint(dc).w > rect.w ? sizeHint(dc).w : rect.w, 
        sizeHint(dc).h > rect.h ? sizeHint(dc).h : rect.h
    );

    dc->margin(drawing_rect, style);
    dc->drawBorder(drawing_rect, style);
    dc->fillRect(drawing_rect, color);
    dc->padding(drawing_rect, style);

    Size text_size = dc->measureText(font(), text());
    if (m_image) {
        Size image_size = m_image->sizeHint(dc);
        dc->drawTexture(
            Point(
                round(drawing_rect.x + (drawing_rect.w / 2 - text_size.w / 2) - image_size.w / 2), 
                round(drawing_rect.y + (drawing_rect.h * 0.5) - (image_size.h * 0.5))
            ),
            image_size,
            m_image->_texture(),
            m_image->coords(),
            m_image->foreground()
        );
        // Resize drawing_rect to account for image before the label is drawn.
        drawing_rect.x += image_size.w;
        drawing_rect.w -= image_size.w;
    }
    HorizontalAlignment h_text_align = m_horizontal_align;
    VerticalAlignment v_text_align = m_vertical_align;
    if (m_image) {
        h_text_align = HorizontalAlignment::Center;
        v_text_align = VerticalAlignment::Center;
    }
    if (m_text.length()) {
        dc->fillTextAligned(
            font(),
            m_text,
            h_text_align,
            v_text_align,
            drawing_rect,
            0,
            m_fg
        );
    }
}

Size Button::sizeHint(DrawingContext *dc) {
    if (m_size_changed) {
        Size size = dc->measureText(font(), text());
        if (m_image) {
            Size i = m_image->sizeHint(dc);
            size.w += i.w;
            if (i.h > size.h) {
                size.h = i.h;
            }
        }

        dc->sizeHintMargin(size, style);
        dc->sizeHintBorder(size, style);
        dc->sizeHintPadding(size, style);

        m_size = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}

Button* Button::setForeground(Color foreground) {
    Widget::m_fg = foreground;
    update();

    return this;
}

Button* Button::setBackground(Color background) {
    Widget::m_bg = background;
    update();

    return this;
}

std::string Button::text() {
    return m_text;
}

Button* Button::setText(std::string text) {
    m_text = text;
    layout();

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
    layout();

    return this;
}