#include "radio_button.hpp"
#include "../application.hpp"

RadioButton::RadioButton(std::shared_ptr<RadioGroup> group, String text) : CheckButton(text, false) {
    assert(group && "RadioGroup cannot be null!");
    m_group = group;
    group->buttons.push_back(this);
    onMouseClick.listeners.clear();
    delete m_checked_image;
    delete m_unchecked_image;

    m_checked_image = new Image(Application::get()->icons["radio_button_checked"]);
    m_unchecked_image = new Image(Application::get()->icons["radio_button_unchecked"]);
    m_background_image = new Image(Application::get()->icons["radio_button_background"]);

    onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
        for (auto child : m_group->buttons) {
            child->m_is_checked = false;
        }
        m_is_checked = true;
        m_group->onValueChanged.notify(this);
    });
}

RadioButton::~RadioButton() {
    delete m_background_image;
}

const char* RadioButton::name() {
    return "RadioButton";
}

void RadioButton::draw(DrawingContext &dc, Rect rect, i32 state) {
    dc.margin(rect, style());
    dc.padding(rect, style());
    // Set rect only to the area where we actually draw the widget.
    this->rect = Rect(rect.x, rect.y + (rect.h / 2) - (m_size.h / 2), m_size.w, m_size.h);

    Image *image = m_is_checked ? m_checked_image : m_unchecked_image;
    Color check_image_bg = m_is_checked ? dc.accentWidgetBackground(style()) : dc.borderBackground(style());
    if (state & STATE_HOVERED) {
        check_image_bg = m_is_checked ? check_image_bg : dc.accentHoveredBackground(style());
    }
    dc.drawTextureAligned(
        Rect(rect.x, rect.y, m_size.h, rect.h),
        Size(m_size.h, m_size.h),
        m_background_image->_texture(),
        m_background_image->coords(),
        HorizontalAlignment::Left,
        VerticalAlignment::Center,
        dc.textBackground(style())
    );
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
        dc.textForeground(style()),
        m_tab_width
    );
    dc.drawKeyboardFocus(this->rect, style(), state);
}
