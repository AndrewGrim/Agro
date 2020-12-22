#include "label.hpp"

Label::Label(std::string text) {
    this->setText(text);
    Widget::m_bg = Color(0, 0, 0, 0);
}

Label::~Label() {
}

const char* Label::name() {
    return "Label";
}

void Label::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    dc->fillRect(rect, Widget::m_bg);
    dc->fillTextAligned(
        this->font() ? this->font() : dc->default_font,
        this->m_text,
        this->m_horizontal_align,
        this->m_vertical_align,
        rect,
        0,
        this->m_fg
    );
}

Size Label::sizeHint(DrawingContext *dc) {
    if (this->m_size_changed) {
        Size size = dc->measureText(this->font() ? this->font() : dc->default_font, this->text());
        this->m_size = size;
        this->m_size_changed = false;

        return size;
    } else {
        return this->m_size;
    }
}

Label* Label::setForeground(Color foreground) {
    if (Widget::m_fg != foreground) {
        Widget::m_fg = foreground;
        this->update();
    }

    return this;
}

Label* Label::setBackground(Color background) {
    if (Widget::m_bg != background) {
        Widget::m_bg = background;
        this->update();
    }

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
    this->m_size_changed = true;
    this->update();
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
