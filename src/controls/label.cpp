#include "label.hpp"

Label::Label(std::string text) {
    this->setText(text);
    Widget::m_bg = COLOR_NONE;
}

Label::~Label() {
}

const char* Label::name() {
    return "Label";
}

void Label::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;

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

    dc->fillRect(drawing_rect, Widget::m_bg);
    dc->padding(drawing_rect, style);
    dc->fillTextAligned(
        this->font() ? this->font() : dc->default_font,
        this->m_text,
        this->m_horizontal_align,
        this->m_vertical_align,
        drawing_rect,
        0,
        this->m_fg
    );
}

Size Label::sizeHint(DrawingContext *dc) {
    if (this->m_size_changed) {
        Size size = dc->measureText(this->font() ? this->font() : dc->default_font, this->text());
        dc->sizeHintPadding(size, style);
        this->m_size = size;
        this->m_size_changed = false;

        return size;
    } else {
        return this->m_size;
    }
}

Label* Label::setForeground(Color foreground) {
    Widget::m_fg = foreground;
    this->update();

    return this;
}

Label* Label::setBackground(Color background) {
    Widget::m_bg = background;
    this->update();

    return this;
}

std::string Label::text() {
    return this->m_text;
}

Label* Label::setText(std::string text) {
    // Unlike everywhere else I don't think we
    // should compare the m_text to text since
    // that can be pretty expensive and we can
    // assume that if someone is setting the text
    // its going to be different.
    this->m_text = text;
    this->layout();

    return this;
}

HorizontalAlignment Label::horizontalAlignment() {
    return this->m_horizontal_align;
}

Label* Label::setHorizontalAlignment(HorizontalAlignment text_align) {
    if (m_horizontal_align != text_align) {
        m_horizontal_align = text_align;
        this->update();
    }

    return this;
}

VerticalAlignment Label::verticalAlignment() {
    return this->m_vertical_align;
}

Label* Label::setVerticalAlignment(VerticalAlignment text_align) {
    if (m_vertical_align != text_align) {
        m_vertical_align = text_align;
        this->update();
    }

    return this;
}
