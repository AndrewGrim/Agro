#include "color_picker.hpp"
#include "../application.hpp"

ColorPicker::ColorPicker() {
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        onMouseMotion.notify(widget, event);
    });
    onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        if (isPressed()) {
            m_position = Point(event.x, event.y);
            m_color = Application::get()->dc->getColor(m_position);
            if (onColorChanged) { onColorChanged(this, m_color); }
        }
    });
}

ColorPicker::~ColorPicker() {}

const char* ColorPicker::name() {
    return "ColorPicker";
}

void ColorPicker::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;
    Rect old_clip = dc.clip();
    dc.setClip(rect.clipTo(old_clip));
    dc.fillRectWithGradient(Rect(rect.x, rect.y, rect.w / 5, rect.h), Color("#ff0000ff"), Color("#ffff00ff"), Gradient::LeftToRight);
    dc.fillRectWithGradient(Rect(rect.x + rect.w / 5, rect.y, rect.w / 5, rect.h), Color("#ffff00ff"), Color("#00ff00ff"), Gradient::LeftToRight);
    dc.fillRectWithGradient(Rect(rect.x + rect.w / 5 * 2, rect.y, rect.w / 5, rect.h), Color("#00ff00ff"), Color("#00ffffff"), Gradient::LeftToRight);
    dc.fillRectWithGradient(Rect(rect.x + rect.w / 5 * 3, rect.y, rect.w / 5, rect.h), Color("#00ffffff"), Color("#0000ffff"), Gradient::LeftToRight);
    dc.fillRectWithGradient(Rect(rect.x + rect.w / 5 * 4, rect.y, rect.w / 5, rect.h), Color("#0000ffff"), Color("#ff00ffff"), Gradient::LeftToRight);

    dc.fillRectWithGradient(Rect(rect.x, rect.y, rect.w, rect.h / 2), Color("#ffffffff"), Color("#ffffff00"), Gradient::TopToBottom);
    dc.fillRectWithGradient(Rect(rect.x, rect.y + rect.h - rect.h / 2, rect.w, rect.h / 2), Color("#00000000"), Color("#000000ff"), Gradient::TopToBottom);

    Color cur_color = COLOR_BLACK;
    if (m_position.y >= rect.y + rect.h / 2) { cur_color = COLOR_WHITE; }
    dc.fillRect(Rect(m_position.x - m_cursor_width / 2, m_position.y - m_cursor_width / 2, m_cursor_width, 1), cur_color);
    dc.fillRect(Rect(m_position.x - m_cursor_width / 2, m_position.y + m_cursor_width / 2, m_cursor_width, 1), cur_color);
    dc.fillRect(Rect(m_position.x - m_cursor_width / 2, m_position.y - m_cursor_width / 2, 1, m_cursor_width), cur_color);
    dc.fillRect(Rect(m_position.x + m_cursor_width / 2 - 1, m_position.y - m_cursor_width / 2, 1, m_cursor_width), cur_color);
    dc.setClip(old_clip);
}

Size ColorPicker::sizeHint(DrawingContext &dc) {
    return Size(260, 260);
}

Color ColorPicker::color() {
    return m_color;
}
