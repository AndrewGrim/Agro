#include "label.hpp"

Label::Label(std::string text) {
    setText(text);
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
        font(),
        m_text,
        m_horizontal_align,
        m_vertical_align,
        drawing_rect,
        0,
        m_fg
    );
}

Size Label::sizeHint(DrawingContext *dc) {
    if (m_size_changed) {
        Size size = dc->measureText(font(), text());
        dc->sizeHintPadding(size, style);
        m_size = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}

Label* Label::setForeground(Color foreground) {
    Widget::m_fg = foreground;
    update();

    return this;
}

Label* Label::setBackground(Color background) {
    Widget::m_bg = background;
    update();

    return this;
}

std::string Label::text() {
    return m_text;
}

Label* Label::setText(std::string text) {
    // Unlike everywhere else I don't think we
    // should compare the m_text to text since
    // that can be pretty expensive and we can
    // assume that if someone is setting the text
    // its going to be different.
    m_text = text;
    layout();

    return this;
}

HorizontalAlignment Label::horizontalAlignment() {
    return m_horizontal_align;
}

Label* Label::setHorizontalAlignment(HorizontalAlignment text_align) {
    if (m_horizontal_align != text_align) {
        m_horizontal_align = text_align;
        update();
    }

    return this;
}

VerticalAlignment Label::verticalAlignment() {
    return m_vertical_align;
}

Label* Label::setVerticalAlignment(VerticalAlignment text_align) {
    if (m_vertical_align != text_align) {
        m_vertical_align = text_align;
        update();
    }

    return this;
}
