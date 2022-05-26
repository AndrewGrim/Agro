#include "../application.hpp"
#include "widget.hpp"
#include "scroll_bar.hpp"
#include "image.hpp"

ScrollBarSlider::ScrollBarSlider(Align alignment, f64 value) : Slider(alignment, value) {}

ScrollBarSlider::~ScrollBarSlider() {}

const char* ScrollBarSlider::name() {
    return "ScrollBarSlider";
}

void ScrollBarSlider::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;

    // Get the size of the slider button.
    i32 size;
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
    dc.fillRect(rect, dc.windowBackground(style()));

    // Determine and draw the location of the slider button.
    if (m_align_policy == Align::Horizontal) {
        i32 result = ((rect.w - size) * m_value);
        if (result > 0) {
            rect.x += result;
        }
        m_slider_button->draw(dc, Rect(rect.x, rect.y, size, rect.h), m_slider_button->state());
    } else {
        i32 result = ((rect.h - size) * m_value);
        if (result > 0) {
            rect.y += result;
        }
        m_slider_button->draw(dc, Rect(rect.x, rect.y, rect.w, size), m_slider_button->state());
    }
}

SimpleScrollBar::SimpleScrollBar(Align alignment, Size min_size) : Box(alignment) {
    m_slider = new ScrollBarSlider(alignment);
    m_slider->m_slider_button->setMinSize(min_size);
    m_slider->setWindowBackgroundColor(COLOR_NONE);
    append(m_slider, Fill::Both);
}

SimpleScrollBar::~SimpleScrollBar() {

}

const char* SimpleScrollBar::name() {
    return "SimpleScrollBar";
}

void SimpleScrollBar::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;
    m_slider->draw(dc, rect, m_slider->state());
}

Size SimpleScrollBar::sizeHint(DrawingContext &dc) {
    return m_slider->sizeHint(dc);
}

ScrollBar::ScrollBar(Align alignment) : Box(alignment) {
    m_begin_button = new IconButton((new Image(Application::get()->icons["up_arrow"]))->setMinSize(Size(8, 8)));
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

    m_end_button = new IconButton((new Image(Application::get()->icons["up_arrow"]))->setMinSize(Size(8, 8)));
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

    DrawingContext &dc = *Application::get()->currentWindow()->dc;
    Widget *widgets[4] = { m_begin_button, m_end_button, m_slider, m_slider->m_slider_button };
    for (i32 i = 0; i < 4; i++) {
        Widget *w = widgets[i];
        w->setPaddingType(STYLE_ALL);
        w->setPaddingTop(dc.default_style.padding.top / 4 > m_min_padding ? dc.default_style.padding.top / 4 : m_min_padding);
        w->setPaddingBottom(dc.default_style.padding.bottom / 4 > m_min_padding ? dc.default_style.padding.bottom / 4 : m_min_padding);
        w->setPaddingLeft(dc.default_style.padding.left / 4 > m_min_padding ? dc.default_style.padding.left / 4 : m_min_padding);
        w->setPaddingRight(dc.default_style.padding.right / 4 > m_min_padding ? dc.default_style.padding.right / 4 : m_min_padding);
        w->setMarginType(STYLE_NONE);
        w->setBorderType(STYLE_ALL);
        w->setBorderTop(dc.default_style.border.top / 4 > m_min_border_width ? dc.default_style.border.top / 4 : m_min_border_width);
        w->setBorderBottom(dc.default_style.border.bottom / 4 > m_min_border_width ? dc.default_style.border.bottom / 4 : m_min_border_width);
        w->setBorderLeft(dc.default_style.border.left / 4 > m_min_border_width ? dc.default_style.border.left / 4 : m_min_border_width);
        w->setBorderRight(dc.default_style.border.right / 4 > m_min_border_width ? dc.default_style.border.right / 4 : m_min_border_width);
    }
}

ScrollBar::~ScrollBar() {
    // No need to delete the buttons and slider as they are children
    // of the ScrollBar and will get cleaned up automatically.
}

const char* ScrollBar::name() {
    return "ScrollBar";
}

void ScrollBar::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;
    Size button_size = m_begin_button->sizeHint(dc);
    m_begin_button->draw(dc, Rect(rect.x, rect.y, button_size.w, button_size.h), m_begin_button->state());
    if (m_align_policy == Align::Horizontal) {
        rect.x += button_size.w;
        rect.w -= button_size.w * 2;
        m_slider->draw(dc, rect, m_slider->state());
        m_end_button->draw(dc, Rect(rect.x + rect.w, rect.y, button_size.w, button_size.h), m_end_button->state());
    } else {
        rect.y += button_size.h;
        rect.h -= button_size.h * 2;
        m_slider->draw(dc, rect, m_slider->state());
        m_end_button->draw(dc, Rect(rect.x, rect.y + rect.h, button_size.w, button_size.h), m_end_button->state());
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

bool ScrollBar::handleLayoutEvent(LayoutEvent event) {
    if (event) {
        if (event & LAYOUT_SCALE) {
            if (_style.font.get()) { setFont(std::shared_ptr<Font>(_style.font.get()->reload(Application::get()->currentWindow()->dc->default_style.font->pixel_size))); }
        }
        if (event & LAYOUT_BORDER) {
            DrawingContext &dc = *Application::get()->currentWindow()->dc;
            Widget *widgets[4] = { m_begin_button, m_end_button, m_slider, m_slider->m_slider_button };
            for (i32 i = 0; i < 4; i++) {
                Widget *w = widgets[i];
                w->style().padding.top = dc.default_style.padding.top / 4 > m_min_padding ? dc.default_style.padding.top / 4 : m_min_padding;
                w->style().padding.bottom = dc.default_style.padding.bottom / 4 > m_min_padding ? dc.default_style.padding.bottom / 4 : m_min_padding;
                w->style().padding.left = dc.default_style.padding.left / 4 > m_min_padding ? dc.default_style.padding.left / 4 : m_min_padding;
                w->style().padding.right = dc.default_style.padding.right / 4 > m_min_padding ? dc.default_style.padding.right / 4 : m_min_padding;
                w->style().border.top = dc.default_style.border.top / 4 > m_min_border_width ? dc.default_style.border.top / 4 : m_min_border_width;
                w->style().border.bottom = dc.default_style.border.bottom / 4 > m_min_border_width ? dc.default_style.border.bottom / 4 : m_min_border_width;
                w->style().border.left = dc.default_style.border.left / 4 > m_min_border_width ? dc.default_style.border.left / 4 : m_min_border_width;
                w->style().border.right = dc.default_style.border.right / 4 > m_min_border_width ? dc.default_style.border.right / 4 : m_min_border_width;
            }
        }
        if (m_size_changed) { return true; }
        m_size_changed = true;
    }
    return false;
}
