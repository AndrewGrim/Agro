#include "icon_button.hpp"

IconButton::IconButton(Image *image) : Button(image) {}

IconButton::~IconButton() {}

const char* IconButton::name() {
    return "IconButton";
}

void IconButton::draw(DrawingContext &dc, Rect rect, int state) {
    Color color;
    if (isPressed() && isHovered()) {
        color = dc.pressedBackground(style);
    } else if (isHovered()) {
        color = dc.hoveredBackground(style);
    } else {
        color = dc.widgetBackground(style);
    }

    dc.margin(rect, style);
    this->rect = rect;
    dc.drawBorder(rect, style);
    dc.fillRect(rect, color);
    dc.padding(rect, style);

    dc.drawTextureAligned(
        rect, m_image->size(),
        m_image->_texture(), m_image->coords(),
        HorizontalAlignment::Center, VerticalAlignment::Center,
        dc.iconForeground(style)
    );
}

Size IconButton::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size size = m_image->sizeHint(dc);

        dc.sizeHintMargin(size, style);
        dc.sizeHintBorder(size, style);
        dc.sizeHintPadding(size, style);

        m_size = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}
