#include "../app.hpp"
#include "widget.hpp"
#include "scrollbar.hpp"

void _onBeginClicked(Widget *scrollbar_begin_button, MouseEvent event) {
    if (scrollbar_begin_button) {
        if (scrollbar_begin_button->m_parent) {
            ScrollBar *parent = (ScrollBar*)scrollbar_begin_button->m_parent;
            parent->m_slider->m_value -= 0.05f; 
            if (parent->m_slider->m_value < 0.0f) parent->m_slider->m_value = 0.0f;
            if (scrollbar_begin_button->m_app) ((Application*)scrollbar_begin_button->m_app)->m_needs_update = true;
        }
    }
}

void _onEndClicked(Widget *scrollbar_end_button, MouseEvent event) {
    if (scrollbar_end_button) {
        if (scrollbar_end_button->m_parent) {
            ScrollBar *parent = (ScrollBar*)scrollbar_end_button->m_parent;
            parent->m_slider->m_value += 0.05f; 
            if (parent->m_slider->m_value > 1.0f) parent->m_slider->m_value = 1.0f;
            if (scrollbar_end_button->m_app) ((Application*)scrollbar_end_button->m_app)->m_needs_update = true;
        }
    }
}