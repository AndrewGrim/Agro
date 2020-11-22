#include "../app.hpp"
#include "widget.hpp"
#include "slider.hpp"

SliderButton::SliderButton(std::string text) : Button(text) {
    this->set_background(Color(0.7f, 0.7f, 0.7f));
}

const char* SliderButton::name() {
    return this->m_name;
}

void SliderButton::draw(DrawingContext *dc, Rect<float> rect) {
    this->rect = rect;
    Color color = Color(0, 0, 0, 0); 
    if (this->is_pressed()) {
        color = this->pressed_background(); 
    } else if (this->is_hovered()) {
        color = this->hover_background();
    } else {
        color = this->background();
    }
    
    dc->fillRect(rect, color);
    dc->fillTextAligned(this->m_text, this->m_text_align, rect, this->m_padding + this->m_border_width / 2);
}

Slider::Slider(Align alignment, std::string text, float value) : Box(alignment) {
    this->m_value = value;
    this->m_slider_button = new SliderButton(text);
    if (alignment == Align::Horizontal) this->append(this->m_slider_button, Fill::Horizontal);
    else this->append(this->m_slider_button, Fill::Vertical);
    this->m_slider_button->m_parent = this;
    this->m_slider_button->mouse_motion_callback = [&](MouseEvent event) {
        SliderButton *self = this->m_slider_button;
        Rect<float> rect = this->rect;
        if (self->is_pressed()) {
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
            if (event.time_since_last_event >= 10) {
                if (this->value_changed_callback) this->value_changed_callback(this);
                if (this->m_app) ((Application*)this->m_app)->m_needs_update = true;
                ((Application*)this->m_app)->m_last_event = std::make_pair<Application::Event, Application::EventHandler>(Application::Event::Scroll, Application::EventHandler::Accepted);
            } else {
                ((Application*)this->m_app)->m_last_event = std::make_pair<Application::Event, Application::EventHandler>(Application::Event::Scroll, Application::EventHandler::Ignored);
            }
        }
    };
}

Slider::~Slider() {}

const char* Slider::name() {
    return this->m_name;
}

void Slider::draw(DrawingContext *dc, Rect<float> rect) {
    this->rect = rect;
    Color color = Color(0, 0, 0, 0); 
    if (this->is_pressed() && this->is_hovered()) {
        color = this->pressed_background(); 
    } else if (this->is_hovered()) {
        color = this->hover_background();
    } else {
        color = this->background();
    }
    
    dc->fillRect(rect, color);
    Size<float> sizehint = this->m_slider_button->size_hint(dc);
    float size;
    if (!this->m_slider_button_size) {
        if (this->m_align_policy == Align::Horizontal) size = sizehint.w;
        else size = sizehint.h;
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
    if (result < 0) {
    } else if (result > (*align_rect[1] - size)) {
        *align_rect[0] += (*align_rect[1] - size);
    } else {
        *align_rect[0] += result;
    }
    // TODO * 2 is a quick workaround, basically we want the slider button take up the same amount of space as the < > buttons
    if (this->m_align_policy == Align::Horizontal) {
        this->m_slider_button->draw(dc, Rect<float>(rect.x, rect.y, this->m_slider_button_size, sizehint.h * 2));
    } else {
        this->m_slider_button->draw(dc, Rect<float>(rect.x, rect.y, sizehint.w * 2, this->m_slider_button_size));
    }
}

Size<float> Slider::size_hint(DrawingContext *dc) {
    Size<float> size = this->m_slider_button->size_hint(dc);
        if (this->m_align_policy == Align::Horizontal) size.w *= 2;
        else size.h *= 2;
    return size;
}

Slider* Slider::set_foreground(Color foreground) {
    this->bg = foreground;
    this->update();

    return this;
}

Slider* Slider::set_background(Color background) {
    this->bg = background;
    this->update();

    return this;
}