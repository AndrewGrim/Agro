#include "treeview.hpp"

TestTree::TestTree(Size min_size) {
    m_viewport = min_size;
}

TestTree::~TestTree() {
    if (m_horizontal_scrollbar) {
        delete m_horizontal_scrollbar;
    }
    if (m_vertical_scrollbar) {
        delete m_vertical_scrollbar;
    }
}

const char* TestTree::name() {
    return "TestTree";
}

void TestTree::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    Rect old_clip = dc->clip();
    dc->setClip(rect);
    int records[270] = {};
    int total_height = 270 * 28;
    int total_width = 1000;
    dc->fillRect(rect, Color(0.6, 1.0, 0.2));
    float content_x = rect.x;
    float content_y = rect.y;
    bool vert = false;
    if (rect.h < total_height) {
        vert = true;
        if (!m_vertical_scrollbar) {
            m_vertical_scrollbar = new SimpleScrollBar(Align::Vertical);
            this->attachApp(app);
        }
        rect.w -= m_vertical_scrollbar->sizeHint(dc).w;
    }
    if (rect.w < total_width) {
        if (!m_horizontal_scrollbar) {
            m_horizontal_scrollbar = new SimpleScrollBar(Align::Horizontal);
            this->attachApp(app);
        }
        rect.h -= m_horizontal_scrollbar->sizeHint(dc).h;
        if (rect.h < total_height) {
            if (!m_vertical_scrollbar) {
                m_vertical_scrollbar = new SimpleScrollBar(Align::Vertical);
                this->attachApp(app);
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
    if (!(rect.h < total_height)) {
        if (m_horizontal_scrollbar) {
            delete m_vertical_scrollbar;
            m_vertical_scrollbar = nullptr;
        }
    }
    if (m_vertical_scrollbar) {
        content_y -= m_vertical_scrollbar->m_slider->m_value * ((total_height) - rect.h);
    }
    if (m_horizontal_scrollbar) {
        content_x -= m_horizontal_scrollbar->m_slider->m_value * ((total_width) - rect.w);
    }
    Point pos = Point(content_x, content_y);
    dc->fillRect(rect, Color(0.6, 0.0, 0.2));
    for (int i = 0; i < 270; i++) {
        dc->fillTextAligned(
            dc->default_font,
            std::to_string(i) + ". The quick brown fox jumps over the lazy dog!",
            HorizontalAlignment::Left,
            VerticalAlignment::Center,
            Rect(pos.x, pos.y, total_width, 28),
            10,
            Color()
        );
        pos.y += 28;
        // TODO only paints whats currently visible
    }
    if (m_vertical_scrollbar) {
        Size size = m_vertical_scrollbar->sizeHint(dc);
        float slider_size = rect.h * ((rect.h - size.h / 2) / total_height);
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
        float slider_size = rect.w * ((rect.w - size.w / 2) / total_width);
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
    dc->setClip(old_clip);
}

Size TestTree::sizeHint(DrawingContext *dc) {
    return m_viewport;
}

bool TestTree::isLayout() {
    return true;
}

void* TestTree::propagateMouseEvent(State *state, MouseEvent event) {
    if (m_vertical_scrollbar) {
        if ((event.x >= m_vertical_scrollbar->rect.x && event.x <= m_vertical_scrollbar->rect.x + m_vertical_scrollbar->rect.w) &&
            (event.y >= m_vertical_scrollbar->rect.y && event.y <= m_vertical_scrollbar->rect.y + m_vertical_scrollbar->rect.h)) {
            return (void*)m_vertical_scrollbar->propagateMouseEvent(state, event);
        }
    }
    if (m_horizontal_scrollbar) {
        if ((event.x >= m_horizontal_scrollbar->rect.x && event.x <= m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
            (event.y >= m_horizontal_scrollbar->rect.y && event.y <= m_horizontal_scrollbar->rect.y + m_horizontal_scrollbar->rect.h)) {
            return (void*)m_horizontal_scrollbar->propagateMouseEvent(state, event);
        }
    }
    for (Widget *child : children) {
        if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
            (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
            void *last = nullptr;
            if (child->isLayout()) {
                last = (void*)child->propagateMouseEvent(state, event);
            } else {
                child->handleMouseEvent(state, event);
                last = (void*)child;
            }
            return last;
        }
    }

    this->handleMouseEvent(state, event);
    return this;
}

Widget* TestTree::attachApp(void *app) {
    if (m_vertical_scrollbar) {
        m_vertical_scrollbar->app = app;
        m_vertical_scrollbar->attachApp(app);
    }
    if (m_horizontal_scrollbar) {
        m_horizontal_scrollbar->app = app;
        m_horizontal_scrollbar->attachApp(app);
    }
    return this;
}

bool TestTree::handleScrollEvent(ScrollEvent event) {
    SDL_Keymod mod = SDL_GetModState();
    if (mod & Mod::Shift) {
        if (this->m_horizontal_scrollbar) {
            return this->m_horizontal_scrollbar->m_slider->handleScrollEvent(event);
        }
    } else {
        if (this->m_vertical_scrollbar) {
            return this->m_vertical_scrollbar->m_slider->handleScrollEvent(event);
        }
    }
    return false;
}