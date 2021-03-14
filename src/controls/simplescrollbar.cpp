#include "simplescrollbar.hpp"
#include "image.hpp"
#include "../application.hpp"

SimpleScrollBarSlider::SimpleScrollBarSlider(Align alignment) : Slider(alignment) {
    Widget::m_bg = Color(0.7f, 0.7f, 0.7f);
}

SimpleScrollBarSlider::~SimpleScrollBarSlider() {

}

const char* SimpleScrollBarSlider::name() {
    return "SimpleScrollBarSlider";
}

void SimpleScrollBarSlider::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    if (this->m_align_policy == Align::Horizontal) {
        dc->fillRect(rect, this->background());
    } else {
        dc->fillRect(rect, this->background());
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

SimpleScrollBar::SimpleScrollBar(Align alignment) : Widget() {
    this->m_align_policy = alignment;
    m_slider = new SimpleScrollBarSlider(alignment);
    m_slider->m_slider_button->setMinSize(Size(10, 10));
    this->append(m_slider, Fill::Both);
}

SimpleScrollBar::~SimpleScrollBar() {

}

const char* SimpleScrollBar::name() {
    return "SimpleScrollBar";
}

void SimpleScrollBar::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    this->m_slider->draw(dc, rect);
}

Size SimpleScrollBar::sizeHint(DrawingContext *dc) {
    Size size = this->m_slider->m_slider_button->sizeHint(dc);
    if (this->m_align_policy == Align::Horizontal) {
        size.w *= 2;
    } else {
        size.h *= 2;
    }
    return size;
}

bool SimpleScrollBar::isLayout() {
    return true;
}