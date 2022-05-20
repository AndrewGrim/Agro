#include "../application.hpp"
#include "scrollable.hpp"

Scrollable::Scrollable(Size min_size) {
    m_viewport = min_size;
    m_size = min_size;
}

Scrollable::~Scrollable() {
    delete m_horizontal_scrollbar;
    delete m_vertical_scrollbar;
}

const char* Scrollable::name() {
    return "Scrollable";
}

void Scrollable::draw(DrawingContext &dc, Rect rect, i32 state) {

}

Size Scrollable::sizeHint(DrawingContext &dc) {
    if (!m_vertical_scrollbar) { m_vertical_scrollbar = new ScrollBar(Align::Vertical); }
    if (!m_horizontal_scrollbar) { m_horizontal_scrollbar = new ScrollBar(Align::Horizontal); }
    return m_viewport;
}

Point Scrollable::automaticallyAddOrRemoveScrollBars(DrawingContext &dc, Rect &rect, const Size &virtual_size) {
    i32 content_x = rect.x;
    i32 content_y = rect.y;
    bool vert = false;
    if (rect.h < virtual_size.h) {
        vert = true;
        if (!m_vertical_scrollbar->isVisible()) {
            m_vertical_scrollbar->show();
        }
        rect.w -= m_vertical_scrollbar->sizeHint(dc).w;
    }
    if (rect.w < virtual_size.w) {
        if (!m_horizontal_scrollbar->isVisible()) {
            m_horizontal_scrollbar->show();
        }
        rect.h -= m_horizontal_scrollbar->sizeHint(dc).h;
        if (rect.h < virtual_size.h) {
            if (!m_vertical_scrollbar->isVisible()) {
                m_vertical_scrollbar->show();
            }
            if (!vert) {
                rect.w -= m_vertical_scrollbar->sizeHint(dc).w;
            }
        }
    } else {
        if (m_horizontal_scrollbar->isVisible()) {
            m_horizontal_scrollbar->hide();
        }
    }
    if (!(rect.h < virtual_size.h)) {
        if (m_vertical_scrollbar->isVisible()) {
            m_vertical_scrollbar->hide();
        }
    }
    if (m_vertical_scrollbar->isVisible()) {
        content_y -= m_vertical_scrollbar->m_slider->m_value * (virtual_size.h - rect.h);
        m_vertical_scrollbar->m_slider->m_step = Application::get()->scroll_amount / (f64)(virtual_size.h - rect.h);
    }
    if (m_horizontal_scrollbar->isVisible()) {
        content_x -= m_horizontal_scrollbar->m_slider->m_value * (virtual_size.w - rect.w);
        m_horizontal_scrollbar->m_slider->m_step = Application::get()->scroll_amount / (f64)(virtual_size.w - rect.w);
    }

    return Point(content_x, content_y);
}

void Scrollable::drawScrollBars(DrawingContext &dc, Rect &rect, const Size &virtual_size) {
    if (m_vertical_scrollbar->isVisible()) {
        Size size = m_vertical_scrollbar->sizeHint(dc);
        i32 slider_size = rect.h * ((rect.h - size.h / 2.0) / virtual_size.h);
        if (slider_size < 10) {
            slider_size = 10;
        }
        m_vertical_scrollbar->m_slider->m_slider_button_size = slider_size;
        m_vertical_scrollbar->draw(
            dc,
            Rect(
                rect.x + rect.w,
                rect.y,
                size.w,
                rect.h > size.h ? rect.h : size.h
            ),
            m_vertical_scrollbar->state()
        );
    }
    if (m_horizontal_scrollbar->isVisible()) {
        Size size = m_horizontal_scrollbar->sizeHint(dc);
        i32 slider_size = rect.w * ((rect.w - size.w / 2.0) / virtual_size.w);
        if (slider_size < 10) {
            slider_size = 10;
        }
        m_horizontal_scrollbar->m_slider->m_slider_button_size = slider_size;
        m_horizontal_scrollbar->draw(
            dc,
            Rect(
                rect.x,
                rect.y + rect.h,
                rect.w > size.w ? rect.w : size.w,
                size.h
            ),
            m_horizontal_scrollbar->state()
        );
    }
    if (m_vertical_scrollbar->isVisible() && m_horizontal_scrollbar->isVisible()) {
        dc.fillRect(Rect(
            rect.x + rect.w,
            rect.y + rect.h,
            m_vertical_scrollbar->sizeHint(dc).w,
            m_horizontal_scrollbar->sizeHint(dc).h),
            dc.widgetBackground(m_vertical_scrollbar->style())
        );
    }
}

bool Scrollable::isLayout() {
    return true;
}

bool Scrollable::isScrollable() {
    return true;
}

i32 Scrollable::isFocusable() {
    return (i32)FocusType::Passthrough;
}

Widget* Scrollable::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
    if (m_vertical_scrollbar->isVisible()) {
        if ((event.x >= m_vertical_scrollbar->rect.x && event.x <= m_vertical_scrollbar->rect.x + m_vertical_scrollbar->rect.w) &&
            (event.y >= m_vertical_scrollbar->rect.y && event.y <= m_vertical_scrollbar->rect.y + m_vertical_scrollbar->rect.h)) {
            return m_vertical_scrollbar->propagateMouseEvent(window, state, event);
        }
    }
    if (m_horizontal_scrollbar->isVisible()) {
        if ((event.x >= m_horizontal_scrollbar->rect.x && event.x <= m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
            (event.y >= m_horizontal_scrollbar->rect.y && event.y <= m_horizontal_scrollbar->rect.y + m_horizontal_scrollbar->rect.h)) {
            return m_horizontal_scrollbar->propagateMouseEvent(window, state, event);
        }
    }
    if (m_vertical_scrollbar->isVisible() && m_horizontal_scrollbar->isVisible()) {
        if ((event.x > m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
            (event.y > m_vertical_scrollbar->rect.y + m_vertical_scrollbar->rect.h)) {
            if (state->hovered) {
                state->hovered->onMouseLeft.notify(this, event);
            }
            state->hovered = nullptr;
            update();
            return nullptr;
        }
    }

    for (Widget *child : children) {
        if (child->isVisible()) {
            if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
                (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
                Widget *last = nullptr;
                if (child->isLayout()) {
                    last = child->propagateMouseEvent(window, state, event);
                } else {
                    child->handleMouseEvent(window, state, event);
                    last = child;
                }
                return last;
            }
        }
    }

    handleMouseEvent(window, state, event);
    return this;
}

bool Scrollable::handleScrollEvent(ScrollEvent event) {
    SDL_Keymod mod = SDL_GetModState();
    if (mod & Mod::Shift) {
        if (m_horizontal_scrollbar->isVisible()) {
            return m_horizontal_scrollbar->m_slider->handleScrollEvent(event);
        }
    } else {
        if (m_vertical_scrollbar->isVisible()) {
            return m_vertical_scrollbar->m_slider->handleScrollEvent(event);
        }
    }
    return false;
}

Size Scrollable::minSize() {
    return m_viewport;
}

void Scrollable::setMinSize(Size min_size) {
    m_viewport = min_size;
    layout(LAYOUT_STYLE);
}

Rect Scrollable::clip() {
    DrawingContext &dc = *Application::get()->currentWindow()->dc;

    if (parent) {
        if (parent->isScrollable()) {
            dc.setClip(rect.clipTo(parent->rect));
        } else {
            // Non-inception Scrollable
            dc.setClip(rect);
        }
    } else {
        // No parent
        dc.setClip(rect);
    }

    return dc.clip();
}

Widget* Scrollable::handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) {
    assert(event != FocusEvent::Activate && "handleFocusEvent should only be called with Forward and Reverse focus events!");
    if (event == FocusEvent::Forward) {
        i32 child_index_unwrapped = data.origin_index ? data.origin_index.unwrap() + 1 : 0;
        for (; child_index_unwrapped < (i32)children.size(); child_index_unwrapped++) {
            Widget *child = children[child_index_unwrapped];
            if (child->isFocusable() && child->isVisible()) {
                return child->handleFocusEvent(event, state, FocusPropagationData());
            }
        }
        if (parent) {
            return parent->handleFocusEvent(event, state, FocusPropagationData(this, parent_index));
        }
        return nullptr;
    } else {
        i32 child_index_unwrapped = data.origin_index ? data.origin_index.unwrap() - 1 : (i32)children.size() - 1;
        for (; child_index_unwrapped > -1; child_index_unwrapped--) {
            Widget *child = children[child_index_unwrapped];
            if (child->isFocusable() && child->isVisible()) {
                return child->handleFocusEvent(event, state, FocusPropagationData());
            }
        }
        if (parent) {
            return parent->handleFocusEvent(event, state, FocusPropagationData(this, parent_index));
        }
        return nullptr;
    }
}

void Scrollable::forEachDrawable(std::function<void(Drawable *drawable)> action) {
    action(this);
    for (Widget *child : children) {
        child->forEachDrawable(action);
    }
    if (m_horizontal_scrollbar && m_vertical_scrollbar) {
        m_horizontal_scrollbar->forEachDrawable(action);
        m_vertical_scrollbar->forEachDrawable(action);
    }
}
