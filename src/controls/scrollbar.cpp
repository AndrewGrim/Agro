#include "../app.hpp"
#include "widget.hpp"
#include "scrollbar.hpp"

ScrollBar::ScrollBar(Align alignment, std::string text) {
    this->m_align_policy = alignment;
    this->m_text = text;

    m_begin_button = new Button("<");
    m_begin_button->m_parent = this;
    m_begin_button->mouse_click_callback = [&](MouseEvent event) {
        this->m_slider->m_value -= 0.05f; // TODO should be a customizable step
        if (this->m_slider->m_value < 0.0f) this->m_slider->m_value = 0.0f;
        if (this->m_app) ((Application*)this->m_app)->m_needs_update = true;
    };
    this->append(m_begin_button, Fill::None);

    m_slider = new Slider(alignment, text);
    if (alignment == Align::Horizontal) this->append(m_slider, Fill::Horizontal);
    else this->append(m_slider, Fill::Vertical);

    m_end_button = new Button(">");
    m_end_button->m_parent = this;
    m_end_button->mouse_click_callback = [&](MouseEvent event) {
        this->m_slider->m_value += 0.05f; 
        if (this->m_slider->m_value > 1.0f) this->m_slider->m_value = 1.0f;
        if (this->m_app) ((Application*)this->m_app)->m_needs_update = true;
    };
    this->append(m_end_button, Fill::None);
}

ScrollBar::~ScrollBar() {

}

const char* ScrollBar::name() {
    return this->m_name;
}

void ScrollBar::draw(DrawingContext *dc, Rect<float> rect) {
    this->rect = rect;
    Size<float> button_size = this->m_begin_button->size_hint(dc);
    this->m_begin_button->draw(dc, Rect<float>(rect.x, rect.y, button_size.w, button_size.h));
    if (this->m_align_policy == Align::Horizontal) {
        rect.x += button_size.w;
        rect.w -= button_size.w * 2;
        this->m_slider->draw(dc, rect);
        this->m_end_button->draw(dc, Rect<float>(rect.x + rect.w, rect.y, button_size.w, button_size.h));
    } else {
        rect.y += button_size.h;
        rect.h -= button_size.h * 2;
        this->m_slider->draw(dc, rect);
        this->m_end_button->draw(dc, Rect<float>(rect.x, rect.y + rect.h, button_size.w, button_size.h));
    }
}

Size<float> ScrollBar::size_hint(DrawingContext *dc) {
    Size<float> size = this->m_slider->m_slider_button->size_hint(dc);
    Size<float> button_size = this->m_begin_button->size_hint(dc);
        if (this->m_align_policy == Align::Horizontal) {
            size.w *= 2 * 2;
            size.w += button_size.w;
            if (button_size.h > size.h) size.h = button_size.h;
        }
        else {
            size.h *= 2;
            size.h += button_size.h * 2;
            if (button_size.w > size.w) size.w = button_size.w;
        }
    return size;
}

bool ScrollBar::is_layout() {
    return true;
}