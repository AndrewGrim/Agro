#include "../application.hpp"
#include "widget.hpp"
#include "slider.hpp"

SliderButton::SliderButton(Size min_size) : Widget() {
    Widget::m_bg = Color(0.9f, 0.9f, 0.9f);
    setMinSize(min_size);
}

SliderButton::~SliderButton() {

}

const char* SliderButton::name() {
    return "SliderButton";
}

void SliderButton::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    Color color = COLOR_BLACK; 
    if (this->isPressed()) {
        color = this->m_pressed_bg; 
    } else if (this->isHovered()) {
        color = this->m_hovered_bg;
    } else {
        color = this->background();
    }
    dc->drawBorder(rect, style);
    dc->fillRect(rect, color);
}

Size SliderButton::sizeHint(DrawingContext *dc) {
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

Slider::Slider(Align alignment, float value) : Box(alignment), m_value{value} {
    Widget::m_bg = COLOR_BLACK;
    m_slider_button = new SliderButton();
    append(m_slider_button, Fill::Both);
    m_slider_button->parent = this;

    m_slider_button->onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        SliderButton *self = m_slider_button;
        if (self->isPressed()) {
            Rect rect = this->rect;
            float size = m_slider_button_size;

            int event_pos;
            int event_pos_rel;
            float position;
            float length;
            if (m_align_policy == Align::Horizontal) {
                event_pos = event.x;
                event_pos_rel = event.xrel;
                position = rect.x;
                length = rect.w;
            } else {
                event_pos = event.y;
                event_pos_rel = event.yrel;
                position = rect.y;
                length = rect.h;
            }

            // When the mouse position is outside the value axis use
            // the mouse position from the event to calculate the value.
            if ((event_pos < position + (size / 2)) || (event_pos > (position + (size / 2)) + (length - size))) {
                m_value = (event_pos - (position + (size / 2))) / (length - size);
            // When the mouse position is inside the value axis use
            // the relative mouse movement from the event to calculate the value.
            } else {
                m_value = m_value + (event_pos_rel / (length - size));
            }
            // Normalize the value.
            m_value = m_value < m_min ? m_min : m_value > m_max ? m_max : m_value;
        
            if (onValueChanged) {
                onValueChanged();
            }
            update();
        }
    });

    this->onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        Rect rect = this->rect;
        float size = m_slider_button_size;
        if (m_align_policy == Align::Horizontal) {
            m_value = (event.x - (rect.x + size / 2)) / (rect.w - size);
        } else {
            m_value = (event.y - (rect.y + size / 2)) / (rect.h - size);
        }
        m_value = m_value < m_min ? m_min : m_value > m_max ? m_max : m_value;
    });
}

Slider::~Slider() {
    // No need to delete slider button because it'll get freed
    // as part of the slider's children.
}

const char* Slider::name() {
    return "Slider";
}

void Slider::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;

    // Get the size of the slider button.
    float size;
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
        dc->fillRect(Rect(rect.x + (size / 2), rect.y + rect.h / 2, rect.w - size, 2), background());
    } else {
        dc->fillRect(Rect(rect.x + rect.w / 2, rect.y + (size / 2), 2, rect.h - size), background());
    }

    // Determine and draw the location of the slider button.
    if (m_align_policy == Align::Horizontal) {
        float result = ((rect.w - size) * m_value);
        if (result > 0) {
            rect.x += result;
        }
        m_slider_button->draw(dc, Rect(rect.x, rect.y, size, rect.h));
    } else {
        float result = ((rect.h - size) * m_value);
        if (result > 0) {
            rect.y += result;
        }
        m_slider_button->draw(dc, Rect(rect.x, rect.y, rect.w, size));
    }
}

Size Slider::sizeHint(DrawingContext *dc) {
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

#define SCROLL_AMOUNT 50

bool Slider::handleScrollEvent(ScrollEvent event) {
    // TODO should we do this automatically in ScrollEvent() ctor?
    event.y *= -1;
    if (this->m_align_policy == Align::Horizontal) {
        // TODO the right calculation but on the wrong values
        // instead of rect.w - button_size
        // it needs to be on the total length of the scrollable area
        float value = this->m_value + ((SCROLL_AMOUNT * event.y) / (rect.w - m_slider_button_size)); //  + (event.y / 100.0);//(event.y / (rect.w - this->m_slider_button_size));
        if (value > this->m_max) value = this->m_max;
        else if (value < this->m_min) value = this->m_min;
        this->m_value = value;
    } else {
        float value = this->m_value + ((SCROLL_AMOUNT * event.y) / (rect.h - m_slider_button_size)); // + (event.y / 100.0);// (event.y / (rect.h - this->m_slider_button_size));
        if (value > this->m_max) value = this->m_max;
        else if (value < this->m_min) value = this->m_min;
        this->m_value = value;
    }
    if (this->onValueChanged) {
        this->onValueChanged();
    }
    this->update();
    return true;
}