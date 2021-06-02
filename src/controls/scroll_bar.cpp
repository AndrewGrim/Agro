#include "../application.hpp"
#include "widget.hpp"
#include "scroll_bar.hpp"
#include "image.hpp"

ScrollBarArrowButton::ScrollBarArrowButton(Image *image) : Button(image) {
    style.padding.type = STYLE_ALL;
    style.padding.top = 2;
    style.padding.bottom = 2;
    style.padding.left = 2;
    style.padding.right = 2;
}

ScrollBarArrowButton::~ScrollBarArrowButton() {

}

const char* ScrollBarArrowButton::name() {
    return "ScrollBarArrowButton";
}

void ScrollBarArrowButton::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;
    Color color;
    if (this->isPressed() && this->isHovered()) {
        color = this->m_pressed_bg;
    } else if (this->isHovered()) {
        color = this->m_hovered_bg;
    } else {
        color = this->background();
    }

    dc.drawBorder(rect, style);
    dc.fillRect(rect, color);
    dc.padding(rect, style);

    if (this->m_image) {
        Size image_size = Size(8, 8);
        dc.drawTexture(
            Point(
                (rect.x + (rect.w * 0.5) - (image_size.w * 0.5)),
                (rect.y + (rect.h * 0.5) - (image_size.h * 0.5))
            ),
            image_size,
            m_image->_texture(),
            m_image->coords(),
            m_image->foreground()
        );
    }
}

Size ScrollBarArrowButton::sizeHint(DrawingContext &dc) {
    Size size = Size(8, 8);
    dc.sizeHintBorder(size, style);
    dc.sizeHintPadding(size, style);

    return size;
}

ScrollBarSlider::ScrollBarSlider(Align alignment, float value) : Slider(alignment, value) {
    Widget::m_bg = Color(0.7f, 0.7f, 0.7f);
}

ScrollBarSlider::~ScrollBarSlider() {

}

const char* ScrollBarSlider::name() {
    return "ScrollBarSlider";
}

void ScrollBarSlider::draw(DrawingContext &dc, Rect rect, int state) {
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

    // Draw the background.
    dc.fillRect(rect, background());

    // Determine and draw the location of the slider button.
    if (m_align_policy == Align::Horizontal) {
        float result = ((rect.w - size) * m_value);
        if (result > 0) {
            rect.x += result;
        }
        m_slider_button->draw(dc, Rect(rect.x, rect.y, size, rect.h), m_slider_button->state());
    } else {
        float result = ((rect.h - size) * m_value);
        if (result > 0) {
            rect.y += result;
        }
        m_slider_button->draw(dc, Rect(rect.x, rect.y, rect.w, size), m_slider_button->state());
    }
}

SimpleScrollBar::SimpleScrollBar(Align alignment, Size min_size) : Box(alignment) {
    m_slider = new ScrollBarSlider(alignment);
    m_slider->m_slider_button->setMinSize(min_size);
    this->append(m_slider, Fill::Both);
}

SimpleScrollBar::~SimpleScrollBar() {

}

const char* SimpleScrollBar::name() {
    return "SimpleScrollBar";
}

void SimpleScrollBar::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;
    this->m_slider->draw(dc, rect, m_slider->state());
}

Size SimpleScrollBar::sizeHint(DrawingContext &dc) {
    return m_slider->sizeHint(dc);
}

ScrollBar::ScrollBar(Align alignment) : Box(alignment) {
    m_begin_button = new ScrollBarArrowButton((new Image(Application::get()->icons["up_arrow"]))->setForeground(COLOR_BLACK));
    if (alignment == Align::Horizontal) {
        m_begin_button->image()->counterClockwise90();
    }
    m_begin_button->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
        m_slider->m_value -= m_slider->m_step;
        if (m_slider->m_value < m_slider->m_min) {
            m_slider->m_value = m_slider->m_min;
        }
        update();
    });
    append(m_begin_button, Fill::None);

    m_slider = new ScrollBarSlider(alignment);
    append(m_slider, Fill::Both);

    m_end_button = new ScrollBarArrowButton((new Image(Application::get()->icons["up_arrow"]))->setForeground(COLOR_BLACK));
    m_end_button->image()->flipVertically();
    if (alignment == Align::Horizontal) {
        m_end_button->image()->clockwise90();
    }
    m_end_button->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
        m_slider->m_value += m_slider->m_step;
        if (m_slider->m_value > m_slider->m_max) {
            m_slider->m_value = m_slider->m_max;
        }
        update();
    });
    append(m_end_button, Fill::None);
}

ScrollBar::~ScrollBar() {
    // No need to delete the buttons and slider as they are children
    // of the ScrollBar and will get cleaned up automatically.
}

const char* ScrollBar::name() {
    return "ScrollBar";
}

void ScrollBar::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;
    Size button_size = this->m_begin_button->sizeHint(dc);
    this->m_begin_button->draw(dc, Rect(rect.x, rect.y, button_size.w, button_size.h), m_begin_button->state());
    if (this->m_align_policy == Align::Horizontal) {
        rect.x += button_size.w;
        rect.w -= button_size.w * 2;
        this->m_slider->draw(dc, rect, m_slider->state());
        this->m_end_button->draw(dc, Rect(rect.x + rect.w, rect.y, button_size.w, button_size.h), m_end_button->state());
    } else {
        rect.y += button_size.h;
        rect.h -= button_size.h * 2;
        this->m_slider->draw(dc, rect, m_slider->state());
        this->m_end_button->draw(dc, Rect(rect.x, rect.y + rect.h, button_size.w, button_size.h), m_end_button->state());
    }
}

Size ScrollBar::sizeHint(DrawingContext &dc) {
    Size size = m_slider->sizeHint(dc);
    Size button_size = m_begin_button->sizeHint(dc);
    if (m_align_policy == Align::Horizontal) {
        size.w += button_size.w * 2;
        if (button_size.h > size.h) size.h = button_size.h;
    } else {
        size.h += button_size.h * 2;
        if (button_size.w > size.w) size.w = button_size.w;
    }

    return size;
}
