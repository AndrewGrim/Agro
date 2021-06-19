#include "color_picker.hpp"
#include "../application.hpp"

ColorPicker::ColorPicker() {
    m_color_edit = new LineEdit(COLOR_NONE.toString());
    append(m_color_edit, Fill::Horizontal);
    m_color_label = new Label("    ");
    append(m_color_label, Fill::Vertical);
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        onMouseMotion.notify(widget, event);
    });
    onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        if (isPressed()) {
            m_position = Point(event.x, event.y);
            float texture_data[(int)(COLOR_PICKER_LENGTH * COLOR_PICKER_LENGTH * 4)] = {};
            glBindTexture(GL_TEXTURE_2D, Application::get()->icons["color_picker_gradient"]->ID);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, texture_data);
            Point pos = Point(m_position.x - rect.x, m_position.y - rect.y);
            if ((pos.x >= 0 && pos.x < COLOR_PICKER_LENGTH) &&
                (pos.y >= 0 && pos.y < COLOR_PICKER_LENGTH)) {
                m_color = Color(
                    texture_data[(int)(pos.y * COLOR_PICKER_LENGTH * 4 + pos.x * 4 + 0)],
                    texture_data[(int)(pos.y * COLOR_PICKER_LENGTH * 4 + pos.x * 4 + 1)],
                    texture_data[(int)(pos.y * COLOR_PICKER_LENGTH * 4 + pos.x * 4 + 2)],
                    texture_data[(int)(pos.y * COLOR_PICKER_LENGTH * 4 + pos.x * 4 + 3)]
                );
            } else {
                m_color = COLOR_NONE;
            }
            if (onColorChanged) { onColorChanged(this, m_color); }
        }
    });
    onColorChanged = [&](Widget *widget, Color color) {
        m_color_edit->setText(color.toString());
        m_color_label->style.widget_background = color;
    };
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
        Size(COLOR_PICKER_LENGTH, COLOR_PICKER_LENGTH),
        Application::get()->icons["color_picker_gradient"].get(),
        &m_coords,
        COLOR_WHITE
    );

    Color cur_color = COLOR_BLACK;
    if (m_position.y >= rect.y + COLOR_PICKER_LENGTH / 2) { cur_color = COLOR_WHITE; }
    dc.fillRect(Rect(m_position.x - m_cursor_width / 2, m_position.y - m_cursor_width / 2, m_cursor_width, 1), cur_color);
    dc.fillRect(Rect(m_position.x - m_cursor_width / 2, m_position.y + m_cursor_width / 2, m_cursor_width, 1), cur_color);
    dc.fillRect(Rect(m_position.x - m_cursor_width / 2, m_position.y - m_cursor_width / 2, 1, m_cursor_width), cur_color);
    dc.fillRect(Rect(m_position.x + m_cursor_width / 2 - 1, m_position.y - m_cursor_width / 2, 1, m_cursor_width), cur_color);

    // Step over color picker
    rect.y += COLOR_PICKER_LENGTH;
    rect.h -= COLOR_PICKER_LENGTH;
    rect.w -= m_color_label->sizeHint(dc).w;
    m_color_edit->draw(dc, rect, m_color_edit->state());

    rect.x += rect.w;
    rect.w = m_color_label->sizeHint(dc).w;
    m_color_label->draw(dc, rect, m_color_label->state());
    dc.setClip(old_clip);
}

Size ColorPicker::sizeHint(DrawingContext &dc) {
    Size s = Size(COLOR_PICKER_LENGTH, COLOR_PICKER_LENGTH);
    float line = m_color_edit->sizeHint(dc).h;
    float label = m_color_label->sizeHint(dc).h;
    if (label > line) { s.h += label; }
    else { s.h += line; }
    return s;
}

Color ColorPicker::color() {
    return m_color;
}

bool ColorPicker::isLayout() {
    return true;
}
