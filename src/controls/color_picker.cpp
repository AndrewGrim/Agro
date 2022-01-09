#include "color_picker.hpp"
#include "../application.hpp"

void updatePosition(Point &m_position, int update_x, int update_y) {
    if (m_position.x < 0 || m_position.x >= COLOR_PICKER_WIDTH) { m_position.x = 0; }
    if (m_position.y < 0 || m_position.y >= COLOR_PICKER_HEIGHT) { m_position.y = 0; }
    if (update_x >= 0 && update_x < COLOR_PICKER_WIDTH) { m_position.x = update_x; }
    if (update_y >= 0 && update_y < COLOR_PICKER_HEIGHT) { m_position.y = update_y; }
}

void updateColor(ColorPicker *color_picker) {
    color_picker->m_color = Color(
        color_picker->m_texture_data[((color_picker->m_position.y * COLOR_PICKER_WIDTH) * 4) + (color_picker->m_position.x * 4) + 0],
        color_picker->m_texture_data[((color_picker->m_position.y * COLOR_PICKER_WIDTH) * 4) + (color_picker->m_position.x * 4) + 1],
        color_picker->m_texture_data[((color_picker->m_position.y * COLOR_PICKER_WIDTH) * 4) + (color_picker->m_position.x * 4) + 2],
        color_picker->m_texture_data[((color_picker->m_position.y * COLOR_PICKER_WIDTH) * 4) + (color_picker->m_position.x * 4) + 3]
    );
    color_picker->onColorChanged.notify(color_picker, color_picker->m_color);
}

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
            updatePosition(m_position, event.x - rect.x, event.y - rect.y);
            updateColor(this);
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

    dc.drawBorder(rect, style, state);
    Rect focus_rect = rect;

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

    dc.drawKeyboardFocus(focus_rect, style, state);
    dc.setClip(old_clip);
}

Size ColorPicker::sizeHint(DrawingContext &dc) {
    Size s = Size(COLOR_PICKER_WIDTH, COLOR_PICKER_HEIGHT);
    dc.sizeHintBorder(s, style);
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

int ColorPicker::isFocusable() {
    return (int)FocusType::Focusable;
}

Widget* ColorPicker::handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) {
    if (data.origin == m_color_edit) {
        if (event == FocusEvent::Reverse) {
            return setSoftFocus(event, state);
        }
    } else if (data.origin == this) {
        if (event == FocusEvent::Forward) {
            return m_color_edit->handleFocusEvent(event, state, data);
        }
    } else {
        if (event == FocusEvent::Reverse) {
            return m_color_edit->handleFocusEvent(event, state, data);
        }
        return setSoftFocus(event, state);
    }
    if (parent) {
        return parent->handleFocusEvent(event, state, FocusPropagationData(this, parent_index));
    }

    return nullptr;
}
