#include "widget.hpp"
#include "../app.hpp"

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

bool Widget::is_hovered() {
    return this->m_is_hovered;
}

void Widget::set_hovered(bool hover) {
    this->m_is_hovered = hover;

    this->update();
}

bool Widget::is_pressed() {
    return this->m_is_pressed;
}

void Widget::set_pressed(bool pressed) {
    this->m_is_pressed = pressed;

    this->update();
}

void Widget::update() {
    if (this->m_app) ((Application*)this->m_app)->m_needs_update = true;
}

void* Widget::propagate_mouse_event(State *state, MouseEvent event) {
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
    return nullptr;
}

void Widget::mouse_event(State *state, MouseEvent event) {
    // TODO when the user clicks outside the window
    // hovered and pressed should be reset
    switch (event.type) {
        case MouseEvent::Type::Down:
            if (state->pressed) {
                ((Widget*)state->pressed)->set_pressed(false);
            }
            this->set_pressed(true);
            if (this->mouse_down_callback) this->mouse_down_callback(this, event);
            break;
        case MouseEvent::Type::Up:
            if (state->pressed) {
                ((Widget*)state->pressed)->set_pressed(false);
                ((Widget*)state->pressed)->set_hovered(false);
            }
            this->set_hovered(true);
            state->hovered = this;
            if (this->mouse_up_callback) this->mouse_up_callback(this, event);
            if (this == state->pressed) {
                if (this->mouse_click_callback) this->mouse_click_callback(this, event);
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
            break;
    }
}

void Widget::attach_app(void *app) {
    for (Widget *child : this->children) {
        child->m_app = app;
        child->attach_app(app);
    }
}