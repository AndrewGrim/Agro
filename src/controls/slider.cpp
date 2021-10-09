#include "../application.hpp"
#include "widget.hpp"
#include "slider.hpp"

#define NORMALIZE(min, max, value) (value < min ? min : value > max ? max : value)

SliderButton::SliderButton(Size min_size) : Widget() {
    setMinSize(min_size);
}

SliderButton::~SliderButton() {

}

const char* SliderButton::name() {
    return "SliderButton";
}

void SliderButton::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;
    Color color;
    if (isPressed()) {
        color = dc.accentPressedBackground(style);
    } else if (isHovered()) {
        color = dc.accentHoveredBackground(style);
    } else {
        color = dc.accentWidgetBackground(style);
    }
    dc.drawBorder(rect, style);
    dc.fillRect(rect, color);
}

Size SliderButton::sizeHint(DrawingContext &dc) {
    return m_size;
}

SliderButton* SliderButton::setMinSize(Size size) {
    m_size = size;
    Application::get()->dc->sizeHintBorder(m_size, style);
    layout();
    return this;
}

Size SliderButton::minSize() {
    return m_size;
}

Slider::Slider(Align alignment, double value) : Box(alignment), m_value{value} {
    m_slider_button = new SliderButton();
    append(m_slider_button, Fill::Both);
    m_slider_button->parent = this;

    m_slider_button->onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        // The origin point is the position from where
        // the value calculations will begin.
        // It determines the start and end points for the value axis.
        if (m_align_policy == Align::Horizontal) {
            m_origin = m_slider_button->rect.x + m_slider_button->rect.w - event.x;
        } else {
            m_origin = m_slider_button->rect.y + m_slider_button->rect.h - event.y;
        }
    });

    m_slider_button->onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        SliderButton *self = m_slider_button;
        if (self->isPressed()) {
            Rect rect = this->rect;
            int size = m_slider_button_size;

            int event_pos;
            int position;
            int length;
            if (m_align_policy == Align::Horizontal) {
                event_pos = event.x;
                position = rect.x;
                length = rect.w;
            } else {
                event_pos = event.y;
                position = rect.y;
                length = rect.h;
            }

            int start = size - m_origin;
            double value = (event_pos - (position + start)) / (double)(length - start - m_origin);
            m_value = NORMALIZE(m_min, m_max, value);

            onValueChanged.notify();
            update();
        }
    });

    this->onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        Rect rect = this->rect;
        int size = m_slider_button_size;
        if (m_align_policy == Align::Horizontal) {
            m_value = (event.x - (rect.x + size / 2.0)) / (double)(rect.w - size);
        } else {
            m_value = (event.y - (rect.y + size / 2.0)) / (double)(rect.h - size);
        }
        m_value = NORMALIZE(m_min, m_max, m_value);
    });
}

Slider::~Slider() {
    // No need to delete slider button because it'll get freed
    // as part of the slider's children.
}

const char* Slider::name() {
    return "Slider";
}

void Slider::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;

    // Get the size of the slider button.
    int size;
    Size sizehint = m_slider_button->sizeHint(dc);
    if (!m_slider_button_size) {
        // Button size was not set. Default.
        if (m_align_policy == Align::Horizontal) {
            size = sizehint.w;
        } else {
            size = sizehint.h;
        }
    } else {
        // Button size was set externally, usually by a scrollable widget.
        size = m_slider_button_size;
    }

    // Draw the "rail" for the slider button.
    if (m_align_policy == Align::Horizontal) {
        dc.fillRect(Rect(rect.x + (size / 2), rect.y + rect.h / 2, rect.w - size, 2), dc.textForeground(style));
    } else {
        dc.fillRect(Rect(rect.x + rect.w / 2, rect.y + (size / 2), 2, rect.h - size), dc.textForeground(style));
    }

    // Determine and draw the location of the slider button.
    int start = size - m_origin;
    if (m_align_policy == Align::Horizontal) {
        rect.x += (rect.w - start - m_origin) * m_value;
        m_slider_button->draw(dc, Rect(rect.x, rect.y, size, rect.h), m_slider_button->state());
    } else {
        rect.y += (rect.h - start - m_origin) * m_value;
        m_slider_button->draw(dc, Rect(rect.x, rect.y, rect.w, size), m_slider_button->state());
    }
}

Size Slider::sizeHint(DrawingContext &dc) {
    Size size = this->m_slider_button->sizeHint(dc);
    if (this->m_align_policy == Align::Horizontal) {
        // The 3 is because we want the slider to be
        // at least 3 buttons wide
        // 2 so that the button can move on its axis by its full length
        // and 1 because the value comes from the center of the button
        // so we need half the width of the button on either side
        // so that the min and max values are where theyre supposed to be.
        // ++--------++
        // ====------++
        size.w *= 3;
    } else {
        size.h *= 3;
    }

    return size;
}

bool Slider::handleScrollEvent(ScrollEvent event) {
    m_value = NORMALIZE(m_min, m_max, m_value + m_step * event.y);
    onValueChanged.notify();
    update();
    return true;
}
