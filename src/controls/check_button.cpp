#include "check_button.hpp"
#include "../application.hpp"

CheckButton::CheckButton(std::string text, bool is_checked) : Widget(), m_text{text}, m_is_checked{is_checked} {
    m_checked_image = new Image(Application::get()->icons["check_button_checked"]);
    m_unchecked_image = new Image(Application::get()->icons["check_button_unchecked"]);

    onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
        if (m_is_checked) { m_is_checked = false; }
        else { m_is_checked = true; }
        onValueChanged.notify(m_is_checked);
    });
}

CheckButton::~CheckButton() {
    delete m_checked_image;
    delete m_unchecked_image;
}

const char* CheckButton::name() {
    return "CheckButton";
}

void CheckButton::draw(DrawingContext &dc, Rect rect, int state) {
    dc.margin(rect, style);
    dc.padding(rect, style);
    // Set rect only to the area where we actually draw the widget.
    this->rect = Rect(rect.x, rect.y + (rect.h / 2) - (m_size.h / 2), m_size.w, m_size.h);

    Image *image = m_is_checked ? m_checked_image : m_unchecked_image;
    Color bg = m_is_checked ? dc.accentWidgetBackground(style) : dc.textBackground(style);
    Color check_image_bg = m_is_checked ? dc.textSelected(style) : dc.borderBackground(style);
    if (isHovered()) {
        bg = m_is_checked ? dc.textBackground(style) : bg;
        check_image_bg = m_is_checked ? dc.accentWidgetBackground(style) : dc.accentHoveredBackground(style);
    }
    dc.fillRect(Rect(rect.x, rect.y + (rect.h / 2) - (m_size.h / 2), m_size.h, m_size.h), bg);
    dc.drawTextureAligned(
        Rect(rect.x, rect.y, m_size.h, rect.h),
        Size(m_size.h, m_size.h),
        image->_texture(),
        image->coords(),
        HorizontalAlignment::Left,
        VerticalAlignment::Center,
        check_image_bg
    );
    rect.x += m_size.h + m_padding;
    rect.w -= m_size.h + m_padding;
    dc.fillTextAligned(
        font(),
        m_text,
        HorizontalAlignment::Left,
        VerticalAlignment::Center,
        rect,
        0,
        dc.textForeground(style),
        m_tab_width
    );
}

Size CheckButton::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size s = dc.measureText(font(), m_text, m_tab_width);
        s.w += m_padding;
        if (s.h % 2 > 0) { s.h++; }
        s.h += 4; // Make the icon slightly larger than text.
        s.w += s.h; // The image size will be the square of the height.
        m_size = s;

        dc.sizeHintPadding(s, style);
        dc.sizeHintMargin(s, style);

        return s;
    }
    return m_size;
}
