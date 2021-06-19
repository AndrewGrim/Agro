#include "color_picker.hpp"
#include "../application.hpp"

ColorPicker::ColorPicker() {
    glBindTexture(GL_TEXTURE_2D, Application::get()->icons["color_picker_gradient"]->ID);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, m_texture_data);
    m_color_edit = new LineEdit(COLOR_NONE.toString());
    append(m_color_edit, Fill::Horizontal);
    m_color_label = new Label("    ");
    append(m_color_label, Fill::Vertical);
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        onMouseMotion.notify(widget, event);
    });
    onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        if (isPressed()) {
            m_position = Point(round(event.x - rect.x), round(event.y - rect.y));
            // Note that the reason we round here is because if we dont
            // and get a Point that contains a non whole number on either x and y
            // then our access into the texture will be incorrect.
            // Because 124 * 260 and 124.5 * 260 are not the same!
            // And we only convert to int at the end and not when multiplying.
            // Took me way too long to find this issue :(
            if ((m_position.x >= 0 && m_position.x < COLOR_PICKER_LENGTH) &&
                (m_position.y >= 0 && m_position.y < COLOR_PICKER_LENGTH)) {
                m_color = Color(
                    m_texture_data[(int)(((m_position.y * COLOR_PICKER_LENGTH) * 4) + (m_position.x * 4) + 0)],
                    m_texture_data[(int)(((m_position.y * COLOR_PICKER_LENGTH) * 4) + (m_position.x * 4) + 1)],
                    m_texture_data[(int)(((m_position.y * COLOR_PICKER_LENGTH) * 4) + (m_position.x * 4) + 2)],
                    m_texture_data[(int)(((m_position.y * COLOR_PICKER_LENGTH) * 4) + (m_position.x * 4) + 3)]
                );
            } else {
                m_color = COLOR_NONE;
            }
            onColorChanged.notify(this, m_color);
        }
    });
    onColorChanged.addEventListener([&](Widget *widget, Color color) {
        m_color_edit->setText(color.toString());
        m_color_label->style.widget_background = color;
    });
}

ColorPicker::~ColorPicker() {
    delete[] m_texture_data;
}

const char* ColorPicker::name() {
    return "ColorPicker";
}

void ColorPicker::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;
    Rect old_clip = dc.clip();
    dc.setClip(rect.clipTo(old_clip));
    dc.drawTexture(
        Point(round(rect.x), round(rect.y)),
        Size(COLOR_PICKER_LENGTH, COLOR_PICKER_LENGTH),
        Application::get()->icons["color_picker_gradient"].get(),
        &m_coords,
        COLOR_WHITE
    );

    Color cur_color = COLOR_BLACK;
    if (m_position.y >= COLOR_PICKER_LENGTH / 2) { cur_color = COLOR_WHITE; }
    dc.fillRect(Rect(rect.x + m_position.x - m_cursor_width / 2, rect.y + m_position.y - m_cursor_width / 2, m_cursor_width, 1), cur_color);
    dc.fillRect(Rect(rect.x + m_position.x - m_cursor_width / 2, rect.y + m_position.y + m_cursor_width / 2, m_cursor_width, 1), cur_color);
    dc.fillRect(Rect(rect.x + m_position.x - m_cursor_width / 2, rect.y + m_position.y - m_cursor_width / 2, 1, m_cursor_width), cur_color);
    dc.fillRect(Rect(rect.x + m_position.x + m_cursor_width / 2 - 1, rect.y + m_position.y - m_cursor_width / 2, 1, m_cursor_width), cur_color);

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
