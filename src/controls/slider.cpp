#include "../app.hpp"
#include "widget.hpp"
#include "slider.hpp"

// TODO at some point it would be better if slider and scrollbar were entirely separate
void _onMouseMove(Widget *slider_button, MouseEvent event) {
    Slider *parent = (Slider*)slider_button->m_parent;
    Rect<float> rect = parent->rect;
    if (slider_button->is_pressed()) {
        if (parent->m_align_policy == Align::Horizontal) {
            float value = parent->m_value + (event.xrel / (rect.w - parent->m_slider_button_size));
            if (value > parent->m_max) value = parent->m_max;
            else if (value < parent->m_min) value = parent->m_min;
            parent->m_value = value;
        } else {
            float value = parent->m_value + (event.yrel / (rect.h - parent->m_slider_button_size));
            if (value > parent->m_max) value = parent->m_max;
            else if (value < parent->m_min) value = parent->m_min;
            parent->m_value = value;
        }
        if (event.time_since_last_event >= 10) {
            if (parent->value_changed_callback) parent->value_changed_callback(parent);
            if (slider_button->m_app) ((Application*)slider_button->m_app)->m_needs_update = true;
            ((Application*)slider_button->m_app)->m_last_event = std::make_pair<Application::Event, Application::EventHandler>(Application::Event::Scroll, Application::EventHandler::Accepted);
        } else {
            ((Application*)slider_button->m_app)->m_last_event = std::make_pair<Application::Event, Application::EventHandler>(Application::Event::Scroll, Application::EventHandler::Ignored);
        }
    }
}