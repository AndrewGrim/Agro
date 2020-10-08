#include "../app.hpp"
#include "widget.hpp"
#include "slider.hpp"

void _onMouseMove(Widget *slider_button, MouseEvent event) {
    if (slider_button && slider_button->is_pressed()) {
        if (slider_button->m_parent) {
            Slider *parent = (Slider*)slider_button->m_parent;
            Rect<float> rect = parent->rect;
            if (parent->m_align_policy == Align::Horizontal) {
                float value = parent->m_max - ((rect.x + rect.w - event.x) / (rect.w));
                if (value > parent->m_max) value = parent->m_max;
                else if (value < parent->m_min) value = parent->m_min;
                parent->m_value = value;
            } else {
                float value = parent->m_max - ((rect.y + rect.h - event.y) / (rect.h));
                if (value > parent->m_max) value = parent->m_max;
                else if (value < parent->m_min) value = parent->m_min;
                parent->m_value = value;
            }
            if (parent->value_changed_callback) parent->value_changed_callback(parent);
            if (slider_button->m_app) ((Application*)slider_button->m_app)->m_needs_update = true;
        }
    }
}