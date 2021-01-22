#include "../app.hpp"
#include "widget.hpp"
#include "scrollbar.hpp"
#include "image.hpp"

ScrollBarArrowButton::ScrollBarArrowButton(Image *image) : Button(image) {

}

ScrollBarArrowButton::~ScrollBarArrowButton() {

}

const char* ScrollBarArrowButton::name() {
    return "ScrollBarArrowButton";
}

void ScrollBarArrowButton::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    Color color; 
    if (this->isPressed() && this->isHovered()) {
        color = this->m_pressed_bg; 
    } else if (this->isHovered()) {
        color = this->m_hovered_bg;
    } else {
        color = this->background();
    }
    
    // Draw border and shrink rectangle to prevent drawing over the border
    rect = dc->drawBorder(rect, this->m_border_width, color);
    dc->fillRect(rect, color);
    // Pad the rectangle with some empty space.
    rect.shrink(m_padding);
    if (this->m_image) {
        Size image_size = Size(12, 12);
        dc->drawImageAtSize(
            Point(
                (rect.x + (rect.w * 0.5) - (image_size.w * 0.5)), 
                (rect.y + (rect.h * 0.5) - (image_size.h * 0.5))
            ),
            image_size,
            m_image,
            m_image->foreground()
        );
    }
}

Size ScrollBarArrowButton::sizeHint(DrawingContext *dc) {
    Size size = Size(12, 12);
    size.w += this->m_padding * 2 + this->m_border_width * 2;
    size.h += this->m_padding * 2 + this->m_border_width * 2;

    return size;
}

ScrollBarSlider::ScrollBarSlider(Align alignment) : Slider(alignment) {
    Widget::m_bg = Color(0.7, 0.7, 0.7);
}

ScrollBarSlider::~ScrollBarSlider() {

}

const char* ScrollBarSlider::name() {
    return "ScrollBarSlider";
}

void ScrollBarSlider::draw(DrawingContext *dc, Rect rect) {
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

ScrollBar::ScrollBar(Align alignment) : Widget() {
    this->m_align_policy = alignment;

    m_begin_button = new ScrollBarArrowButton((new Image("scrollbar_arrow.png"))->setForeground(Color()));
    m_begin_button->setPadding(1);
    m_begin_button->onMouseClick = [&](MouseEvent event) {
        this->m_slider->m_value -= 0.05f; // TODO should be a customizable step
        if (this->m_slider->m_value < 0.0f) {
            this->m_slider->m_value = 0.0f;
        }
        this->update();
    };
    this->append(m_begin_button, Fill::None);

    m_slider = new ScrollBarSlider(alignment);
    this->append(m_slider, Fill::Both);
    // TODO this is a nice idea but for the scrollbar would need
    // to inherit its parents color for the line below to do anything
    // m_slider->setBackground(m_slider->parent->background());

    m_end_button = new ScrollBarArrowButton((new Image("scrollbar_arrow.png"))->setForeground(Color()));
    m_end_button->setPadding(1);
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
    return "ScrollBar";
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
            size.w *= 2 * 2; // ?
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