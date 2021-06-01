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

    dc->fillRect(rect, Widget::m_bg);
    dc->padding(rect, style);
    dc->fillTextMultilineAligned(
        font(),
        m_text,
        m_horizontal_align,
        m_vertical_align,
        rect,
        0,
        m_fg,
        m_line_spacing
    );
}

Size Label::sizeHint(DrawingContext *dc) {
    if (m_size_changed) {
        Size size = dc->measureTextMultiline(font(), text(), m_line_spacing);
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
