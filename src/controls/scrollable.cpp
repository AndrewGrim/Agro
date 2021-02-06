#include "scrollable.hpp"

const char* Scrollable::name() {
    return "Scrollable";
}

void Scrollable::draw(DrawingContext *dc, Rect rect) {

}

Size Scrollable::sizeHint(DrawingContext *dc) {
    return Size(0, 0);
}

bool Scrollable::isLayout() {
    return true;
}

Point Scrollable::automaticallyAddOrRemoveScrollBars(DrawingContext *dc, Rect &rect, Size virtual_size) {
    float content_x = rect.x;
    float content_y = rect.y;
    bool vert = false;
    if (rect.h < virtual_size.h) {
        vert = true;
        if (!m_vertical_scrollbar) {
            m_vertical_scrollbar = new SimpleScrollBar(Align::Vertical);
            m_vertical_scrollbar->app = app;
            m_vertical_scrollbar->attachApp(app);
        }
        rect.w -= m_vertical_scrollbar->sizeHint(dc).w;
    }
    if (rect.w < virtual_size.w) {
        if (!m_horizontal_scrollbar) {
            m_horizontal_scrollbar = new SimpleScrollBar(Align::Horizontal);
            m_horizontal_scrollbar->app = app;
            m_horizontal_scrollbar->attachApp(app);
        }
        rect.h -= m_horizontal_scrollbar->sizeHint(dc).h;
        if (rect.h < virtual_size.h) {
            if (!m_vertical_scrollbar) {
                m_vertical_scrollbar = new SimpleScrollBar(Align::Vertical);
                m_vertical_scrollbar->app = app;
                m_vertical_scrollbar->attachApp(app);
            }
            if (!vert) {
                rect.w -= m_vertical_scrollbar->sizeHint(dc).w;
            }
        }
    } else {
        if (m_horizontal_scrollbar) {
            delete m_horizontal_scrollbar;
            m_horizontal_scrollbar = nullptr;
        }
    }
    if (!(rect.h < virtual_size.h)) {
        if (m_horizontal_scrollbar) {
            delete m_vertical_scrollbar;
            m_vertical_scrollbar = nullptr;
        }
    }
    if (m_vertical_scrollbar) {
        content_y -= m_vertical_scrollbar->m_slider->m_value * ((virtual_size.h) - rect.h);
    }
    if (m_horizontal_scrollbar) {
        content_x -= m_horizontal_scrollbar->m_slider->m_value * ((virtual_size.w) - rect.w);
    }

    return Point(content_x, content_y);
}

void Scrollable::drawScrollBars(DrawingContext *dc, Rect &rect, Size virtual_size) {
    if (m_vertical_scrollbar) {
        Size size = m_vertical_scrollbar->sizeHint(dc);
        float slider_size = rect.h * ((rect.h - size.h / 2) / virtual_size.h);
        if (slider_size < 20) {
            slider_size = 20;
        } else if (slider_size > (rect.h - 10)) {
            slider_size = rect.h - 10;
        }
        m_vertical_scrollbar->m_slider->m_slider_button_size = slider_size;
        m_vertical_scrollbar->draw(dc, Rect(
            rect.x + rect.w, 
            rect.y, 
            size.w, 
            rect.h > size.h ? rect.h : size.h
        ));
    }
    if (m_horizontal_scrollbar) {
        Size size = m_horizontal_scrollbar->sizeHint(dc);
        float slider_size = rect.w * ((rect.w - size.w / 2) / virtual_size.w);
        if (slider_size < 20) {
            slider_size = 20;
        } else if (slider_size > (rect.w - 10)) {
            slider_size = rect.w - 10;
        }
        m_horizontal_scrollbar->m_slider->m_slider_button_size = slider_size;
        m_horizontal_scrollbar->draw(dc, Rect(
            rect.x, 
            rect.y + rect.h, 
            rect.w > size.w ? rect.w : size.w, 
            size.h
        ));
    }
    if (m_vertical_scrollbar && m_horizontal_scrollbar) {
        dc->fillRect(Rect(
            rect.x + rect.w, 
            rect.y + rect.h, 
            m_vertical_scrollbar->sizeHint(dc).w, 
            m_horizontal_scrollbar->sizeHint(dc).h), 
            m_vertical_scrollbar->background()
        );
    }
}
