#include "splitter.hpp"
#include "../application.hpp"

Splitter::Splitter(Align alignment, Size min_size) : Widget(), m_align_policy{alignment}, m_viewport{min_size} {
    this->onMouseDown.addEventListener([&](Widget *splitter, MouseEvent event) {
        if (m_align_policy == Align::Horizontal) {
            Rect sash = Rect(rect.x + ((rect.w - m_sash_size) * m_split), rect.y, m_sash_size, rect.h);
            if (event.x >= sash.x && event.x <= sash.x + sash.w) {
                m_dragging = true;
                Application::get()->setMouseCursor(Cursor::SizeWE);
            }
        } else {
            Rect sash = Rect(rect.x, rect.y + ((rect.h - m_sash_size) * m_split), rect.w, m_sash_size);
            if (event.y >= sash.y && event.y <= sash.y + sash.h) {
                m_dragging = true;
                Application::get()->setMouseCursor(Cursor::SizeNS);
            }
        }
    });

    this->onMouseUp.addEventListener([&](Widget *splitter, MouseEvent event) {
        m_dragging = false;
    });

    this->onMouseMotion.addEventListener([&](Widget *splitter, MouseEvent event) {
        if (isPressed() && m_dragging) {
            if (m_align_policy == Align::Horizontal) {
                m_split = (event.x - rect.x) / (double)rect.w;
            } else {
                m_split = (event.y - rect.y) / (double)rect.h;
            }
            m_split = m_split < 0.0 ? 0.0 : m_split > 1.0 ? 1.0 : m_split;
            update();
        } else {
            if (m_align_policy == Align::Horizontal) {
                Rect sash = Rect(rect.x + (rect.w - m_sash_size) * m_split, rect.y, m_sash_size, rect.h);
                if (event.x >= sash.x && event.x <= sash.x + sash.w) {
                    Application::get()->setMouseCursor(Cursor::SizeWE);
                } else {
                    Application::get()->setMouseCursor(Cursor::Default);
                }
            } else {
                Rect sash = Rect(rect.x, rect.y + (rect.h - m_sash_size) * m_split, rect.w, m_sash_size);
                if (event.y >= sash.y && event.y <= sash.y + sash.h) {
                    Application::get()->setMouseCursor(Cursor::SizeNS);
                } else {
                    Application::get()->setMouseCursor(Cursor::Default);
                }
            }
        }
    });

    this->onMouseLeft.addEventListener([&](Widget *splitter, MouseEvent event) {
        m_dragging = false;
        Application::get()->setMouseCursor(Cursor::Default);
    });
}

Splitter::~Splitter() {
    delete m_first;
    delete m_second;
}

const char* Splitter::name() {
    return "Splitter";
}

void Splitter::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;

    Rect old_clip = dc.clip();
    if (m_align_policy == Align::Horizontal) {
        Rect local_rect = Rect(rect.x, rect.y, (rect.w - m_sash_size) * m_split, rect.h);
        if (m_first) {
            dc.setClip(local_rect);
            m_first->draw(dc, local_rect, m_first->state());
        }
        local_rect = Rect(local_rect.x + local_rect.w, rect.y, m_sash_size, rect.h);
        dc.setClip(local_rect);
        dc.fillRect(local_rect, dc.textForeground(style));
        if (m_second) {
            local_rect = Rect(local_rect.x + m_sash_size, rect.y, (rect.w - m_sash_size) * (1.0 - m_split), rect.h);
            dc.setClip(local_rect);
            m_second->draw(dc, local_rect, m_second->state());
        }
    } else {
        Rect local_rect = Rect(rect.x, rect.y, rect.w, (rect.h - m_sash_size) * m_split);
        if (m_first) {
            dc.setClip(local_rect);
            m_first->draw(dc, local_rect, m_first->state());
        }
        local_rect = Rect(rect.x, local_rect.y + local_rect.h, rect.w, m_sash_size);
        dc.setClip(local_rect);
        dc.fillRect(local_rect, dc.textForeground(style));
        if (m_second) {
            local_rect = Rect(rect.x, local_rect.y + m_sash_size, rect.w, (rect.h - m_sash_size) * (1.0 - m_split));
            dc.setClip(local_rect);
            m_second->draw(dc, local_rect, m_second->state());
        }
    }
    dc.setClip(old_clip);
}

Size Splitter::sizeHint(DrawingContext &dc) {
    Size s = m_viewport;
    m_first->sizeHint(dc);
    m_second->sizeHint(dc);
    if (m_align_policy == Align::Horizontal) {
        s.w += m_sash_size;
    } else {
        s.h += m_sash_size;
    }
    return s;
}

Widget* Splitter::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
    if (m_first) {
        if (m_first->isVisible()) {
            if ((event.x >= m_first->rect.x && event.x <= m_first->rect.x + m_first->rect.w) &&
                (event.y >= m_first->rect.y && event.y <= m_first->rect.y + m_first->rect.h)) {
                Widget *last = nullptr;
                if (m_first->isLayout()) {
                    last = m_first->propagateMouseEvent(window, state, event);
                } else {
                    m_first->handleMouseEvent(window, state, event);
                    last = m_first;
                }
                return last;
            }
        }
    }
    if (m_second) {
        if (m_second->isVisible()) {
            if ((event.x >= m_second->rect.x && event.x <= m_second->rect.x + m_second->rect.w) &&
                (event.y >= m_second->rect.y && event.y <= m_second->rect.y + m_second->rect.h)) {
                Widget *last = nullptr;
                if (m_second->isLayout()) {
                    last = m_second->propagateMouseEvent(window, state, event);
                } else {
                    m_second->handleMouseEvent(window, state, event);
                    last = m_second;
                }
                return last;
            }
        }
    }

    handleMouseEvent(window, state, event);
    return this;
}

bool Splitter::isLayout() {
    return true;
}

void Splitter::append() {
    if (m_align_policy == Align::Horizontal) {
        assert(false && "append() should not be used for Splitter! Use left() and right() instead.");
    } else {
        assert(false && "append() should not be used for Splitter! Use top() and bottom() instead.");
    }
}

void Splitter::top(Widget *widget) {
    m_first = widget;
    widget->parent = this;
}

void Splitter::bottom(Widget *widget) {
    m_second = widget;
    widget->parent = this;
}

void Splitter::left(Widget *widget) {
    top(widget);
}

void Splitter::right(Widget *widget) {
    bottom(widget);
}

// Note the split value goes from 0.0 to 1.0.
// The split value determines the size available
// to the left or top side with the remaining
// ,if any, size going to the other side (right, down).
void Splitter::setSplit(double new_split) {
    m_split = new_split;
    m_split = m_split < 0.0 ? 0.0 : m_split > 1.0 ? 1.0 : m_split;
    update();
}

double Splitter::split() {
    return m_split;
}
