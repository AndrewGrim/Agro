#include "button.hpp"

Button::Button(std::string text) {
    this->setText(text);
    Widget::m_bg = Color(0.9, 0.9, 0.9);
}

Button::~Button() {

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
    
    // draw border and shrink rectangle to prevent drawing over the border
    rect = dc->drawBorder(rect, this->m_border_width, color);
    dc->fillRect(rect, color);
    dc->fillTextAligned(this->m_text, this->m_text_align, rect, this->m_padding);
}

Size Button::sizeHint(DrawingContext *dc) {
    if (this->m_size_changed) {
        Size size = dc->measureText(text());
        size.w += this->m_padding * 2;
        size.h += this->m_padding * 2;

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
    this->m_size_changed = true;
    this->update();
    this->layout();

    return this;
}

TextAlignment Button::textAlignment() {
    return this->m_text_align;
}

void Button::setTextAlignment(TextAlignment text_align) {
    if (this->m_text_align == text_align) {
        this->m_text_align = text_align;
        this->update();
    }
}
