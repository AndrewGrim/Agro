#include <chrono>

#include "../app.hpp"
#include "widget.hpp"

Widget::Widget() {}

Widget::~Widget() {}

const char* Widget::name() {
    return this->m_name;
}

void Widget::draw(DrawingContext *dc, Rect<float> rect) {}

Widget* Widget::append(Widget* widget, Fill fill_policy) {
    widget->set_fill_policy(fill_policy);
    this->children.push_back(widget);
    if (this->m_app) widget->m_app = this->m_app;

    return this;
}

Size<float> Widget::size_hint(DrawingContext *dc) {
    return Size<float>(0, 0); 
}

Color Widget::background() {
    return this->bg;
}

Widget* Widget::set_background(Color background) {
    this->bg = background;
    this->update();

    return this;
}

Color Widget::hover_background() {
    return this->hover_bg;
}

Widget* Widget::set_hover_background(Color background) {
    this->hover_bg = background;
    this->update();

    return this;
}

Color Widget::pressed_background() {
    return this->pressed_bg;
}

Widget* Widget::set_pressed_background(Color background) {
    this->pressed_bg = background;
    this->update();

    return this;
}

Widget* Widget::set_fill_policy(Fill fill_policy) {
    this->m_fill_policy = fill_policy;

    return this;
}

Fill Widget::fill_policy() {
    return this->m_fill_policy;
}

void Widget::show() {
    this->m_is_visible = true;
}

void Widget::hide() {
    this->m_is_visible = false;
}

bool Widget::is_visible() {
    return this->m_is_visible;
}

bool Widget::is_layout() {
    return false;
}

bool Widget::is_scrollable() {
    return false;
}

bool Widget::is_hovered() {
    return this->m_is_hovered;
}

void Widget::set_hovered(bool hover) {
    if (hover != this->m_is_hovered) {
        this->m_is_hovered = hover;
        this->update();
    }
}

bool Widget::is_pressed() {
    return this->m_is_pressed;
}

void Widget::set_pressed(bool pressed) {
    if (pressed != this->m_is_pressed) {
        this->m_is_pressed = pressed;
        this->update();
    }
}

void Widget::update() {
    if (this->m_app) ((Application*)this->m_app)->m_needs_update = true;
}

void* Widget::propagate_mouse_event(State *state, MouseEvent event) {
    // TODO make a widget focused if its clicked on, focus should call update
    if (this->is_scrollable()) {
        ScrolledBox *self = (ScrolledBox*)this;
        if (self->m_vertical_scrollbar) {
            if ((event.x >= self->m_vertical_scrollbar->rect.x && event.x <= self->m_vertical_scrollbar->rect.x + self->m_vertical_scrollbar->rect.w) &&
                (event.y >= self->m_vertical_scrollbar->rect.y && event.y <= self->m_vertical_scrollbar->rect.y + self->m_vertical_scrollbar->rect.h)) {
                return (void*)self->m_vertical_scrollbar->propagate_mouse_event(state, event);
            }
        }
        if (self->m_horizontal_scrollbar) {
            if ((event.x >= self->m_horizontal_scrollbar->rect.x && event.x <= self->m_horizontal_scrollbar->rect.x + self->m_horizontal_scrollbar->rect.w) &&
                (event.y >= self->m_horizontal_scrollbar->rect.y && event.y <= self->m_horizontal_scrollbar->rect.y + self->m_horizontal_scrollbar->rect.h)) {
                return (void*)self->m_horizontal_scrollbar->propagate_mouse_event(state, event);
            }
        }
        goto CHILDREN;
    } else { // TODO this probably doesnt need a goto leave the if but remove the else
        CHILDREN:
            for (Widget *child : this->children) {
                if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
                    (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
                    void *last = nullptr;
                    if (child->is_layout()) {
                        last = (void*)child->propagate_mouse_event(state, event);
                    } else {
                        child->mouse_event(state, event);
                        last = (void*)child;
                    }
                    return last;
                }
            }
    }

    if (event.type == MouseEvent::Type::Up && state->pressed) {
        ((Widget*)state->pressed)->set_pressed(false);
        ((Widget*)state->pressed)->set_hovered(false);
        state->hovered = nullptr;
        state->pressed = nullptr;
    }
    if (event.type == MouseEvent::Type::Motion && state->hovered) {
        ((Widget*)state->hovered)->set_hovered(false);
        if (((Widget*)state->hovered)->mouse_left_callback) {
            ((Widget*)state->hovered)->mouse_left_callback((Widget*)state->hovered, event);
        }
        state->hovered = nullptr;
    }
    if (event.type == MouseEvent::Type::Motion && state->pressed) {
        if (((Widget*)state->pressed)->mouse_motion_callback) {
            ((Widget*)state->pressed)->mouse_motion_callback(((Widget*)state->pressed), event);
        }
    }
    ((Application*)this->m_app)->m_last_event = std::make_pair<Application::Event, Application::EventHandler>(Application::Event::None, Application::EventHandler::Accepted);
    return nullptr;
}

void Widget::mouse_event(State *state, MouseEvent event) {
    // TODO when the user clicks outside the window
    // hovered and pressed should be reset
    Application *app = (Application*)this->m_app;
    switch (event.type) {
        case MouseEvent::Type::Down:
            if (state->pressed) {
                ((Widget*)state->pressed)->set_pressed(false);
            }
            this->set_pressed(true);
            if (this->mouse_down_callback) this->mouse_down_callback(this, event);
            app->m_last_event = std::make_pair<Application::Event, Application::EventHandler>(Application::Event::MouseDown, Application::EventHandler::Accepted);;
            break;
        case MouseEvent::Type::Up:
            if (state->pressed) {
                ((Widget*)state->pressed)->set_pressed(false);
                ((Widget*)state->pressed)->set_hovered(false);
            }
            this->set_hovered(true);
            state->hovered = this;
            if (this->mouse_up_callback) this->mouse_up_callback(this, event);
            app->m_last_event = std::make_pair<Application::Event, Application::EventHandler>(Application::Event::MouseUp, Application::EventHandler::Accepted);
            if (this == state->pressed) {
                if (this->mouse_click_callback) this->mouse_click_callback(this, event);
                app->m_last_event = std::make_pair<Application::Event, Application::EventHandler>(Application::Event::MouseClick, Application::EventHandler::Accepted);
            }
            state->pressed = nullptr;
            break;
        case MouseEvent::Type::Motion:
            if (!state->pressed) {
                if (state->hovered) {
                    if (this != state->hovered) {
                        ((Widget*)state->hovered)->set_hovered(false);
                        if (((Widget*)state->hovered)->mouse_left_callback) {
                            ((Widget*)state->hovered)->mouse_left_callback((Widget*)state->hovered, event);
                        }
                        this->set_hovered(true);
                        if (this->mouse_entered_callback) {
                            this->mouse_entered_callback(this, event);
                        }
                    }
                } else {
                    this->set_hovered(true);
                    if (this->mouse_entered_callback) {
                        this->mouse_entered_callback(this, event);
                    }
                }
                if (this->mouse_motion_callback) this->mouse_motion_callback(this, event);
            } else {
                if (state->pressed == state->hovered) {
                    ((Widget*)state->pressed)->set_hovered(true);
                } else {
                    ((Widget*)state->pressed)->set_hovered(false);
                }
                if (((Widget*)state->pressed)->mouse_motion_callback) {
                    ((Widget*)state->pressed)->mouse_motion_callback(((Widget*)state->pressed), event);
                }
            }
            app->m_last_event = std::make_pair<Application::Event, Application::EventHandler>(Application::Event::MouseMotion, Application::EventHandler::Accepted);;
            break;
    }
}

void Widget::attach_app(void *app) {
    for (Widget *child : this->children) {
        child->m_app = app;
        child->attach_app(app);
    }
}