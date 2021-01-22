#include "../app.hpp"
#include "widget.hpp"
#include "slider.hpp"

SliderButton::SliderButton() : Widget() {
    Widget::m_bg = Color(0.9, 0.9, 0.9);
}

SliderButton::~SliderButton() {

}

const char* SliderButton::name() {
    return "SliderButton";
}

void SliderButton::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    Color color = Color(0, 0, 0, 1); 
    if (this->isPressed()) {
        color = this->m_pressed_bg; 
    } else if (this->isHovered()) {
        color = this->m_hovered_bg;
    } else {
        color = this->background();
    }
    rect = dc->drawBorder(rect, 4, color);
    dc->fillRect(rect, color);
}

Size SliderButton::sizeHint(DrawingContext *dc) {
    return m_size;
} 

SliderButton* SliderButton::setMinSize(Size size) {
    m_size = size;
    update();
    layout();
    return this;
}

Size SliderButton::minSize() {
    return m_size;
}

Slider::Slider(Align alignment, float value) : Box(alignment) {
    Widget::m_bg = Color();
    this->m_value = value;
    this->m_slider_button = new SliderButton();
    if (alignment == Align::Horizontal) {
        // TODO investigate, because scrollbar does something different
        this->append(this->m_slider_button, Fill::Horizontal);
    } else {
        this->append(this->m_slider_button, Fill::Vertical);
    }
    this->m_slider_button->parent = this;
    this->m_slider_button->onMouseMotion = [&](MouseEvent event) {
        SliderButton *self = this->m_slider_button;
        Rect rect = this->rect;
        if (self->isPressed()) {
            // TODO probably should still switch to mouse x and y
            // just make it so it only moves from the center of the slider
            // then we might be able to go back to the old limiter in the main loop
            // which would be good for controlling the framerate
            if (this->m_align_policy == Align::Horizontal) {
                float value = this->m_value + (event.xrel / (rect.w - this->m_slider_button_size));
                if (value > this->m_max) value = this->m_max;
                else if (value < this->m_min) value = this->m_min;
                this->m_value = value;
            } else {
                float value = this->m_value + (event.yrel / (rect.h - this->m_slider_button_size));
                if (value > this->m_max) value = this->m_max;
                else if (value < this->m_min) value = this->m_min;
                this->m_value = value;
            }
            if (this->onValueChanged) this->onValueChanged();
            this->update();
            ((Application*)this->app)->setLastEvent(std::make_pair<Application::Event, Application::EventHandler>(Application::Event::Scroll, Application::EventHandler::Accepted));
        }
    };
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
    if (this->m_align_policy == Align::Horizontal) {
        dc->fillRect(Rect(rect.x, rect.y + rect.h / 2, rect.w, 2), this->background());
    } else {
        dc->fillRect(Rect(rect.x + rect.w / 2, rect.y, 2, rect.h), this->background());
    }
    Size sizehint = this->m_slider_button->sizeHint(dc);
    float size;
    if (!this->m_slider_button_size) {
        if (this->m_align_policy == Align::Horizontal) {
            size = sizehint.w;
        } else {
            size = sizehint.h;
        }
    } else {
        size = this->m_slider_button_size;
    }
    float *align_rect[2] = {};
    if (this->m_align_policy == Align::Horizontal) {
        align_rect[0] = &rect.x;
        align_rect[1] = &rect.w;
    } else {
        align_rect[0] = &rect.y;
        align_rect[1] = &rect.h;
    }

    float result = ((*align_rect[1] - size) * this->m_value);
    if (result <= 0) {
        // NO OP
    } else if (result > (*align_rect[1] - size)) {
        *align_rect[0] += (*align_rect[1] - size);
    } else {
        *align_rect[0] += result;
    }
    if (this->m_align_policy == Align::Horizontal) {
        this->m_slider_button->draw(dc, Rect(rect.x, rect.y, size, rect.h));
    } else {
        this->m_slider_button->draw(dc, Rect(rect.x, rect.y, rect.w, size));
    }
}

Size Slider::sizeHint(DrawingContext *dc) {
    Size size = this->m_slider_button->sizeHint(dc);
    if (this->m_align_policy == Align::Horizontal) {
        size.w *= 2;
    } else {
        size.h *= 2;
    }

    return size;
}

Slider* Slider::setForeground(Color foreground) {
    // The foreground color for this widget acutally affects
    // the slider button.
    if (m_slider_button->background() != foreground) {
        m_slider_button->setBackground(foreground);
        this->update();
    }

    return this;
}

Slider* Slider::setBackground(Color background) {
    // The background color for this widget only affects
    // the line on which the slider sits on.
    if (Widget::m_bg != background) {
        Widget::m_bg = background;
        this->update();
    }

    return this;
}