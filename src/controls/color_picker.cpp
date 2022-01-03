#include "color_picker.hpp"
#include "../application.hpp"

ColorPicker::ColorPicker() {
    // TODO add keybindings for moving the cursor
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
            m_position = Point(event.x - rect.x, event.y - rect.y);
            if ((m_position.x >= 0 && m_position.x < COLOR_PICKER_WIDTH) &&
                (m_position.y >= 0 && m_position.y < COLOR_PICKER_HEIGHT)) {
                m_color = Color(
                    m_texture_data[((m_position.y * COLOR_PICKER_WIDTH) * 4) + (m_position.x * 4) + 0],
                    m_texture_data[((m_position.y * COLOR_PICKER_WIDTH) * 4) + (m_position.x * 4) + 1],
                    m_texture_data[((m_position.y * COLOR_PICKER_WIDTH) * 4) + (m_position.x * 4) + 2],
                    m_texture_data[((m_position.y * COLOR_PICKER_WIDTH) * 4) + (m_position.x * 4) + 3]
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
        Point(rect.x, rect.y),
        Size(COLOR_PICKER_WIDTH, COLOR_PICKER_HEIGHT),
        Application::get()->icons["color_picker_gradient"].get(),
        &m_coords,
        COLOR_WHITE
    );

    Color cur_color = COLOR_BLACK;
    if (m_position.y >= COLOR_PICKER_HEIGHT / 2) { cur_color = COLOR_WHITE; }
    dc.fillRect(Rect(rect.x + m_position.x - m_cursor_width / 2, rect.y + m_position.y - m_cursor_width / 2, m_cursor_width, 1), cur_color);
    dc.fillRect(Rect(rect.x + m_position.x - m_cursor_width / 2, rect.y + m_position.y + m_cursor_width / 2, m_cursor_width, 1), cur_color);
    dc.fillRect(Rect(rect.x + m_position.x - m_cursor_width / 2, rect.y + m_position.y - m_cursor_width / 2, 1, m_cursor_width), cur_color);
    dc.fillRect(Rect(rect.x + m_position.x + m_cursor_width / 2 - 1, rect.y + m_position.y - m_cursor_width / 2, 1, m_cursor_width), cur_color);

    rect.y += COLOR_PICKER_HEIGHT;
    rect.h -= COLOR_PICKER_HEIGHT;
    rect.w -= m_color_label->sizeHint(dc).w;
    m_color_edit->draw(dc, rect, m_color_edit->state());

    rect.x += rect.w;
    rect.w = m_color_label->sizeHint(dc).w;
    m_color_label->draw(dc, rect, m_color_label->state());
    dc.setClip(old_clip);
}

Size ColorPicker::sizeHint(DrawingContext &dc) {
    Size s = Size(COLOR_PICKER_WIDTH, COLOR_PICKER_HEIGHT);
    int line = m_color_edit->sizeHint(dc).h;
    int label = m_color_label->sizeHint(dc).h;
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
