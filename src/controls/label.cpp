#include "label.hpp"

Label::Label(String text) {
    setText(text);
    setWidgetBackgroundColor(COLOR_NONE);
    setBorderType(STYLE_NONE);
}

Label::~Label() {
}

const char* Label::name() {
    return "Label";
}

void Label::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;

    dc.margin(rect, style());
    dc.drawBorder(rect, style(), state);
    dc.fillRect(rect, dc.widgetBackground(style()));
    dc.padding(rect, style());
    dc.fillTextMultilineAligned(
        font(),
        m_text,
        m_horizontal_align,
        m_vertical_align,
        rect,
        0,
        dc.textForeground(style()),
        m_line_spacing
    );
}

Size Label::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size size = dc.measureTextMultiline(font(), text(), m_line_spacing);
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

String Label::text() {
    return m_text;
}

Label* Label::setText(String text) {
    // Unlike everywhere else I don't think we
    // should compare the m_text to text since
    // that can be pretty expensive and we can
    // assume that if someone is setting the text
    // its going to be different.
    m_text = text;
    layout(LAYOUT_STYLE);

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
