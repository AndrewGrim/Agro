#include "color_picker.hpp"
#include "../application.hpp"

ColorPicker::ColorPicker() {
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        onMouseMotion.notify(widget, event);
    });
    onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        if (isPressed()) {
            m_position = Point(event.x, event.y);
            float texture_data[260*260*4] = {};
            glBindTexture(GL_TEXTURE_2D, Application::get()->icons["color_picker_gradient"]->ID);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, texture_data);
            Point pos = Point(m_position.x - rect.x, m_position.y - rect.y);
            if ((pos.x >= 0 && pos.x < 260) &&
                (pos.y >= 0 && pos.y < 260)) {
                m_color = Color(
                    texture_data[(int)(pos.y * 260 * 4 + pos.x * 4 + 0)],
                    texture_data[(int)(pos.y * 260 * 4 + pos.x * 4 + 1)],
                    texture_data[(int)(pos.y * 260 * 4 + pos.x * 4 + 2)],
                    texture_data[(int)(pos.y * 260 * 4 + pos.x * 4 + 3)]
                );
            } else {
                m_color = COLOR_NONE;
            }
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
    dc.drawTexture(
        Point(rect.x, rect.y),
        sizeHint(dc),
        Application::get()->icons["color_picker_gradient"].get(),
        &m_coords,
        COLOR_WHITE
    );

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
