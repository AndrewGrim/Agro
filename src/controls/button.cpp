#include "button.hpp"

Button::Button(std::string text) {
    this->setText(text);
    Widget::m_bg = Color(0.9, 0.9, 0.9);
}

Button::Button(Image *image) {
    this->setImage(image);
    Widget::m_bg = Color(0.9, 0.9, 0.9);
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
    if (this->isPressed() && this->isHovered()) {
        color = this->m_pressed_bg; 
    } else if (this->isHovered()) {
        color = this->m_hovered_bg;
    } else {
        color = this->background();
    }
    
    // Draw border and shrink rectangle to prevent drawing over the border
    rect = dc->drawBorder(rect, this->m_border_width, color);
    dc->fillRect(rect, color);
    // Pad the rectangle with some empty space.
    rect.shrink(m_padding);
    Size text_size = dc->measureText(this->font() ? this->font() : dc->default_font, text());
    if (this->m_image) {
        Size image_size = m_image->sizeHint(dc);
        dc->drawImageAtSize(
            Point(
                round(rect.x + (rect.w / 2 - text_size.w / 2) - image_size.w / 2), 
                round(rect.y + (rect.h * 0.5) - (image_size.h * 0.5))
            ),
            image_size,
            m_image,
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
    if (this->m_text.length()) {
        dc->fillTextAligned(
            this->font() ? this->font() : dc->default_font,
            this->m_text,
            h_text_align,
            v_text_align,
            rect,
            0,
            this->m_fg
        );
    }
}

Size Button::sizeHint(DrawingContext *dc) {
    if (this->m_size_changed) {
        Size size = dc->measureText(this->font() ? this->font() : dc->default_font, text());
        if (m_image) {
            Size i = m_image->sizeHint(dc);
            size.w += i.w;
            if (i.h > size.h) {
                size.h = i.h;
            }
        }
        // Note: full padding is applied on both sides but
        // border width is divided in half for each side
        size.w += this->m_padding * 2 + this->m_border_width;
        size.h += this->m_padding * 2 + this->m_border_width;

        this->m_size = size;
        this->m_size_changed = false;

        return size;
    } else {
        return this->m_size;
    }
}

Button* Button::setForeground(Color foreground) {
    if (Widget::m_fg != foreground) {
        Widget::m_fg = foreground;
        this->update();
    }

    return this;
}

Button* Button::setBackground(Color background) {
    if (Widget::m_bg != background) {
        Widget::m_bg = background;
        this->update();
    }

    return this;
}

std::string Button::text() {
    return this->m_text;
}

Button* Button::setText(std::string text) {
    // Unlike everywhere else I don't think we
    // should compare the m_text to text since
    // that can be pretty expensive and we can
    // assume that if someone is setting the text
    // its going to be different.
    this->m_text = text;
    this->layout();

    return this;
}

HorizontalAlignment Button::horizontalAlignment() {
    return this->m_horizontal_align;
}

Button* Button::setHorizontalAlignment(HorizontalAlignment text_align) {
    if (m_horizontal_align != text_align) {
        m_horizontal_align = text_align;
        this->update();
    }

    return this;
}

VerticalAlignment Button::verticalAlignment() {
    return this->m_vertical_align;
}

Button* Button::setVerticalAlignment(VerticalAlignment text_align) {
    if (m_vertical_align != text_align) {
        m_vertical_align = text_align;
        this->update();
    }

    return this;
}

Image* Button::image() {
    return this->m_image;
}

Button* Button::setImage(Image *image) {
    if (m_image) {
        delete m_image;
    }
    this->m_image = image;
    this->layout();

    return this;
}

unsigned int Button::padding() {
    return this->m_padding;
}

Button* Button::setPadding(unsigned int padding) {
    if (this->m_padding != padding) {
        this->m_padding = padding;
        this->layout();
    }

    return this;
}

unsigned int Button::borderWidth() {
    return this->m_border_width;
}

Button* Button::setBorderWidth(unsigned int border_width) {
    if (this->m_border_width != border_width) {
        this->m_border_width = border_width;
        this->layout();
    }

    return this;
}
