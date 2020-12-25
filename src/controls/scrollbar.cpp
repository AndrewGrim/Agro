#include "../app.hpp"
#include "widget.hpp"
#include "scrollbar.hpp"
#include "image.hpp"

ScrollBar::ScrollBar(Align alignment) : Widget() {
    this->m_align_policy = alignment;

    m_begin_button = new Button((new Image("scrollbar_arrow.png"))->setForeground(Color()));
    m_begin_button->setPadding(0);
    m_begin_button->onMouseClick = [&](MouseEvent event) {
        this->m_slider->m_value -= 0.05f; // TODO should be a customizable step
        if (this->m_slider->m_value < 0.0f) {
            this->m_slider->m_value = 0.0f;
        }
        this->update();
    };
    this->append(m_begin_button, Fill::None);

    m_slider = new Slider(alignment);
    if (alignment == Align::Horizontal) {
        this->append(m_slider, Fill::Both);
    } else {
        this->append(m_slider, Fill::Both);
    }

    m_end_button = new Button((new Image("scrollbar_arrow.png"))->setForeground(Color()));
    m_end_button->setPadding(0);
    m_end_button->image()->flipVertically();
    m_end_button->onMouseClick = [&](MouseEvent event) {
        this->m_slider->m_value += 0.05f; 
        if (this->m_slider->m_value > 1.0f) {
            this->m_slider->m_value = 1.0f;
        }
        this->update();
    };
    this->append(m_end_button, Fill::None);
}

ScrollBar::~ScrollBar() {
    // No need to delete the buttons and slider as they are children
    // of the ScrollBar and will get cleaned up automatically.
}

const char* ScrollBar::name() {
    return "Scrollbar";
}

void ScrollBar::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    Size button_size = this->m_begin_button->sizeHint(dc);
    this->m_begin_button->draw(dc, Rect(rect.x, rect.y, button_size.w, button_size.h));
    if (this->m_align_policy == Align::Horizontal) {
        rect.x += button_size.w;
        rect.w -= button_size.w * 2;
        this->m_slider->draw(dc, rect);
        this->m_end_button->draw(dc, Rect(rect.x + rect.w, rect.y, button_size.w, button_size.h));
    } else {
        rect.y += button_size.h;
        rect.h -= button_size.h * 2;
        this->m_slider->draw(dc, rect);
        this->m_end_button->draw(dc, Rect(rect.x, rect.y + rect.h, button_size.w, button_size.h));
    }
}

Size ScrollBar::sizeHint(DrawingContext *dc) {
    Size size = this->m_slider->m_slider_button->sizeHint(dc);
    Size button_size = this->m_begin_button->sizeHint(dc);
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

bool ScrollBar::isLayout() {
    return true;
}