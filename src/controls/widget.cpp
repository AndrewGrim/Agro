#include "widget.hpp"
#include "../app.hpp"

Widget::Widget() {

}

Widget::~Widget() {

}

Widget* Widget::append(Widget* widget, Fill fill_policy) {
    widget->setFillPolicy(fill_policy);
    this->children.push_back(widget);
    if (this->app) widget->app = this->app;
    this->m_size_changed = true;

    return this;
}


Color Widget::background() {
    return this->m_bg;
}

Widget* Widget::setBackground(Color background) {
    if (this->m_bg != background) {
        this->m_bg = background;
        this->update();
    }

    return this;
}

Color Widget::foreground() {
    return this->m_fg;
}

Widget* Widget::setForeground(Color foreground) {
    if (this->m_fg != foreground) {
        this->m_fg = foreground;
        this->update();
    }

    return this;
}

Widget* Widget::setFillPolicy(Fill fill_policy) {
    if (this->m_fill_policy != fill_policy) {
        this->m_fill_policy = fill_policy;
        this->update();
    }

    return this;
}

Fill Widget::fillPolicy() {
    return this->m_fill_policy;
}

void Widget::show() {
    if (!this->m_is_visible) {
        this->m_is_visible = true;
        this->update();
    }
}

void Widget::hide() {
    if (this->m_is_visible) {
        this->m_is_visible = false;
        this->update();
    }
}

bool Widget::isVisible() {
    return this->m_is_visible;
}

bool Widget::isHovered() {
    return this->m_is_hovered;
}

void Widget::setHovered(bool hover) {
    if (this->m_is_hovered != hover) {
        this->m_is_hovered = hover;
        this->update();
    }
}

bool Widget::isPressed() {
    return this->m_is_pressed;
}

void Widget::setPressed(bool pressed) {
    if (this->m_is_pressed != pressed) {
        this->m_is_pressed = pressed;
        this->update();
    }
}

bool Widget::isFocused() {
    return this->m_is_focused;
}

void Widget::setFocused(bool focused) {
    if (this->m_is_focused != focused) {
        this->m_is_focused = focused;
        this->update();
    }
}

void Widget::update() {
    if (this->app) {
        ((Application*)this->app)->update();
    }
}

void* Widget::propagateMouseEvent(State *state, MouseEvent event) {
    if (this->isScrollable()) {
        // ScrolledBox *self = (ScrolledBox*)this;
        // if (self->m_vertical_scrollbar) {
        //     if ((event.x >= self->m_vertical_scrollbar->rect.x && event.x <= self->m_vertical_scrollbar->rect.x + self->m_vertical_scrollbar->rect.w) &&
        //         (event.y >= self->m_vertical_scrollbar->rect.y && event.y <= self->m_vertical_scrollbar->rect.y + self->m_vertical_scrollbar->rect.h)) {
        //         return (void*)self->m_vertical_scrollbar->propagate_mouse_event(state, event);
        //     }
        // }
        // if (self->m_horizontal_scrollbar) {
        //     if ((event.x >= self->m_horizontal_scrollbar->rect.x && event.x <= self->m_horizontal_scrollbar->rect.x + self->m_horizontal_scrollbar->rect.w) &&
        //         (event.y >= self->m_horizontal_scrollbar->rect.y && event.y <= self->m_horizontal_scrollbar->rect.y + self->m_horizontal_scrollbar->rect.h)) {
        //         return (void*)self->m_horizontal_scrollbar->propagate_mouse_event(state, event);
        //     }
        // }
    }
    for (Widget *child : this->children) {
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

    if (event.type == MouseEvent::Type::Up && state->pressed) {
        ((Widget*)state->pressed)->setPressed(false);
        ((Widget*)state->pressed)->setHovered(false);
        state->hovered = nullptr;
        state->pressed = nullptr;
    }
    if (event.type == MouseEvent::Type::Motion && state->hovered) {
        ((Widget*)state->hovered)->setHovered(false);
        if (((Widget*)state->hovered)->onMouseLeft) {
            ((Widget*)state->hovered)->onMouseLeft(event);
        }
        state->hovered = nullptr;
    }
    if (event.type == MouseEvent::Type::Motion && state->pressed) {
        if (((Widget*)state->pressed)->onMouseMotion) {
            ((Widget*)state->pressed)->onMouseMotion(event);
        }
    }
    ((Application*)this->app)->setLastEvent(std::make_pair<Application::Event, Application::EventHandler>(Application::Event::None, Application::EventHandler::Accepted));
    return nullptr;
}

void Widget::handleMouseEvent(State *state, MouseEvent event) {
    // TODO when the mouse moves outside the window
    // hovered and pressed should be reset
    Application *app = (Application*)this->app;
    switch (event.type) {
        case MouseEvent::Type::Down:
            if (state->pressed) {
                ((Widget*)state->pressed)->setPressed(false);
            }
            this->setPressed(true);
            if (state->focused) {
                ((Widget*)state->focused)->setFocused(false);
            }
            this->setFocused(true);
            state->focused = (void*)this;
            // TODO maybe add an on_focus callback?
            if (this->onMouseDown) this->onMouseDown(event);
            app->setLastEvent(std::make_pair<Application::Event, Application::EventHandler>(Application::Event::MouseDown, Application::EventHandler::Accepted));
            break;
        case MouseEvent::Type::Up:
            if (state->pressed) {
                ((Widget*)state->pressed)->setPressed(false);
                ((Widget*)state->pressed)->setHovered(false);
            }
            this->setHovered(true);
            state->hovered = this;
            if (this->onMouseUp) this->onMouseUp(event);
            app->setLastEvent(std::make_pair<Application::Event, Application::EventHandler>(Application::Event::MouseUp, Application::EventHandler::Accepted));
            if (this == state->pressed) {
                if (this->onMouseClick) this->onMouseClick(event);
                app->setLastEvent(std::make_pair<Application::Event, Application::EventHandler>(Application::Event::MouseClick, Application::EventHandler::Accepted));
            }
            state->pressed = nullptr;
            break;
        case MouseEvent::Type::Motion:
            if (!state->pressed) {
                if (state->hovered) {
                    if (this != state->hovered) {
                        ((Widget*)state->hovered)->setHovered(false);
                        if (((Widget*)state->hovered)->onMouseLeft) {
                            ((Widget*)state->hovered)->onMouseLeft(event);
                        }
                        this->setHovered(true);
                        if (this->onMouseEntered) {
                            this->onMouseEntered(event);
                        }
                    }
                } else {
                    this->setHovered(true);
                    if (this->onMouseEntered) {
                        this->onMouseEntered(event);
                    }
                }
                if (this->onMouseMotion) this->onMouseMotion(event);
            } else {
                if (state->pressed == state->hovered) {
                    ((Widget*)state->pressed)->setHovered(true);
                } else {
                    ((Widget*)state->pressed)->setHovered(false);
                }
                if (((Widget*)state->pressed)->onMouseMotion) {
                    ((Widget*)state->pressed)->onMouseMotion(event);
                }
            }
            app->setLastEvent(std::make_pair<Application::Event, Application::EventHandler>(Application::Event::MouseMotion, Application::EventHandler::Accepted));
            break;
    }
}

void Widget::attachApp(void *app) {
    for (Widget *child : this->children) {
        child->app = app;
        child->attachApp(app);
    }
}
