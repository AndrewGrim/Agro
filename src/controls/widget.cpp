#include "widget.hpp"
#include "../app.hpp"

Widget::Widget() {

}

Widget::~Widget() {
    if (this->app) {
        ((Application*)this->app)->removeFromState(this);
    }
    for (Widget *child : this->children) {
        delete child;
    }
}

Widget* Widget::append(Widget* widget, Fill fill_policy, unsigned int proportion) {
    if (widget->parent) {
        widget->parent->remove(widget->parent_index);
    }
    widget->parent = this;
    widget->setFillPolicy(fill_policy);
    widget->setProportion(proportion);
    this->children.push_back(widget);
    widget->parent_index = this->children.size() - 1;
    if (this->app) widget->app = this->app;
    this->m_size_changed = true;
    this->update();
    this->layout();

    return this;
}

Widget* Widget::remove(size_t parent_index) {
    Widget *child = this->children[parent_index];
    child->setPressed(false);
    child->setHovered(false);
    child->setFocused(false);
    ((Application*)child->app)->removeFromState(child);
    this->children.erase(this->children.begin() + parent_index);
    size_t i = 0;
    for (Widget *child : this->children) {
        child->parent_index = i;
        i++;
    }
    this->m_size_changed = true;
    this->update();
    this->layout();

    return child;
}

void Widget::destroy() {
    if (this->parent) {
        this->parent->remove(this->parent_index);
    }
    delete this;
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
        this->layout();
        // TODO maybe instead of layout manually change
        // expandable and non expandable widgets for parent?
        // it would probably be good to have an abstract layout then
    }

    return this;
}

Fill Widget::fillPolicy() {
    return this->m_fill_policy;
}

Widget* Widget::show() {
    if (!this->m_is_visible) {
        this->m_is_visible = true;
        this->m_size_changed = true;
        this->update();
        this->layout();
    }
    
    return this;
}

Widget* Widget::hide() {
    if (this->m_is_visible) {
        this->m_is_visible = false;
        this->m_size_changed = true;
        this->update();
        this->layout();
    }
    
    return this;
}

bool Widget::isVisible() {
    return this->m_is_visible;
}

bool Widget::isHovered() {
    return this->m_is_hovered;
}

Widget* Widget::setHovered(bool hover) {
    if (this->m_is_hovered != hover) {
        this->m_is_hovered = hover;
        this->update();
    }
    
    return this;
}

bool Widget::isPressed() {
    return this->m_is_pressed;
}

Widget* Widget::setPressed(bool pressed) {
    if (this->m_is_pressed != pressed) {
        this->m_is_pressed = pressed;
        this->update();
    }
    
    return this;
}

bool Widget::isFocused() {
    return this->m_is_focused;
}

Widget* Widget::setFocused(bool focused) {
    if (this->m_is_focused != focused) {
        this->m_is_focused = focused;
        this->update();
    }
    
    return this;
}

Widget* Widget::update() {
    if (this->app) {
        ((Application*)this->app)->update();
    }
    
    return this;
}

Widget* Widget::layout() {
    if (this->app) {
        ((Application*)this->app)->layout();
    }
    
    return this;
}

void* Widget::propagateMouseEvent(State *state, MouseEvent event) {
    if (this->isScrollable()) {
        // TODO this should probably be an abstract class that provides a vert
        // and a horizontal scrollbar, so in the future dont cast to scrolledbox
        // Alternatively could override the propagate method of scrolledbox
        // struct Scrollable {
        //     ScrollBar m_horizontal;
        //     ScrollBar m_vertical;
        // };
        ScrolledBox *self = (ScrolledBox*)this;
        if (self->m_vertical_scrollbar) {
            if ((event.x >= self->m_vertical_scrollbar->rect.x && event.x <= self->m_vertical_scrollbar->rect.x + self->m_vertical_scrollbar->rect.w) &&
                (event.y >= self->m_vertical_scrollbar->rect.y && event.y <= self->m_vertical_scrollbar->rect.y + self->m_vertical_scrollbar->rect.h)) {
                return (void*)self->m_vertical_scrollbar->propagateMouseEvent(state, event);
            }
        }
        if (self->m_horizontal_scrollbar) {
            if ((event.x >= self->m_horizontal_scrollbar->rect.x && event.x <= self->m_horizontal_scrollbar->rect.x + self->m_horizontal_scrollbar->rect.w) &&
                (event.y >= self->m_horizontal_scrollbar->rect.y && event.y <= self->m_horizontal_scrollbar->rect.y + self->m_horizontal_scrollbar->rect.h)) {
                return (void*)self->m_horizontal_scrollbar->propagateMouseEvent(state, event);
            }
        }
    }
    for (Widget *child : this->children) {
        if (child->isVisible()) {
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
    }

    this->handleMouseEvent(state, event);
    return this;
}

void Widget::handleMouseEvent(State *state, MouseEvent event) {
    // Note: The hovered state is not set as it is returned
    // from the propagateMouseEvent function.

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
            state->focused = this;
            // TODO maybe add an on_focus callback?
            onMouseDown.notify(this, event);
            break;
        case MouseEvent::Type::Up:
            if (state->pressed) {
                ((Widget*)state->pressed)->setPressed(false);
                ((Widget*)state->pressed)->setHovered(false);
            }
            this->setHovered(true);
            state->hovered = this;
            onMouseUp.notify(this, event);
            if (this == state->pressed) {
                onMouseClick.notify(this, event);
            } else {
                if (state->pressed) {
                    ((Widget*)state->pressed)->onMouseLeft.notify(this, event);
                }
                onMouseEntered.notify(this, event);
            }
            state->pressed = nullptr;
            break;
        case MouseEvent::Type::Motion:
            if (!state->pressed) {
                if (state->hovered) {
                    if (this != state->hovered) {
                        ((Widget*)state->hovered)->setHovered(false);
                        ((Widget*)state->hovered)->onMouseLeft.notify(this, event);
                        this->setHovered(true);
                        onMouseEntered.notify(this, event);
                    }
                } else {
                    this->setHovered(true);
                    onMouseEntered.notify(this, event);
                }
            } else {
                if (state->pressed == state->hovered) {
                    ((Widget*)state->pressed)->setHovered(true);
                } else {
                    ((Widget*)state->pressed)->setHovered(false);
                }
                ((Widget*)state->pressed)->onMouseMotion.notify(this, event);
            }
            break;
    }
}

void Widget::handleTextEvent(DrawingContext *dc, const char *text) {
    // Up to the widget to implement!
}

bool Widget::handleScrollEvent(ScrollEvent event) {
    // Up to the widget to implement!
    // Returns false to let the application know that
    // this widget doesnt handle scroll events.
    // Otherwise return true.
    return false;
}

Widget* Widget::attachApp(void *app) {
    for (Widget *child : this->children) {
        child->app = app;
        child->attachApp(app);
    }
    
    return this;
}

bool Widget::isLayout() {
    return false;
}

bool Widget::isScrollable() {
    return false;
}

unsigned int Widget::proportion() {
    return this->m_proportion;
}

Widget* Widget::setProportion(unsigned int proportion) {
    if (this->m_proportion != proportion) {
        this->m_proportion = proportion ? proportion : 1;
        this->update();
        this->layout();
    }
    
    return this;
}

Font* Widget::font() {
    return this->m_font;
}

Widget* Widget::setFont(Font *font) {
    if (this->m_font != font) {
        this->m_font = font;
        this->m_size_changed = true;
        this->update();
        this->layout();
    }
    
    return this;
}

// TODO implement left/right specific modifiers any the other remaining ones
int Widget::bind(int key, int modifiers, std::function<void()> callback) {
    Mod mods[4] = {Mod::None, Mod::None, Mod::None, Mod::None};

    if (modifiers & KMOD_CTRL) {
        mods[0] = Mod::Ctrl;
    }
    // if (modifiers & KMOD_LCTRL) {
    //     println("LEFT CONTROL");
    // } else if (modifiers & KMOD_RCTRL) {
    //     println("RIGHT CONTROL");
    // }
    if (modifiers & KMOD_SHIFT) {
        mods[1] = Mod::Shift;
    }
    // if (modifiers & KMOD_LSHIFT) {
    //     println("LEFT SHIFT");
    // } else if (modifiers & KMOD_RSHIFT) {
    //     println("RIGHT SHIFT");
    // }
    if (modifiers & KMOD_ALT) {
        mods[2] = Mod::Alt;
    }
    // if (modifiers & KMOD_LALT) {
    //     println("LEFT ALT");
    // } else if (modifiers & KMOD_RALT) {
    //     println("RIGHT ALT");
    // }
    if (modifiers & KMOD_GUI) {
        mods[3] = Mod::Gui;
    }
    // if (modifiers & KMOD_LGUI) {
    //     println("LEFT GUI");
    // } else if (modifiers & KMOD_RGUI) {
    //     println("RIGHT GUI");
    // }
    // if (modifiers & KMOD_NUM) {
    //     println("NUM");
    // }
    // if (modifiers & KMOD_CAPS) {
    //     println("CAPS");
    // }
    // if (modifiers & KMOD_MODE) {
    //     println("MODE");
    // }
    this->m_keyboard_shortcuts.insert(
        std::make_pair(
            m_binding_id, 
            KeyboardShortcut(
                key,
                mods[0], mods[1], mods[2], mods[3],
                modifiers,
                callback
            )
        )
    );
    return m_binding_id++;
}

int Widget::bind(int key, Mod modifier, std::function<void()> callback) {
    return bind(key, (int)modifier, callback);
}

void Widget::unbind(int key) {
    this->m_keyboard_shortcuts.erase(key);
}

const std::unordered_map<int, KeyboardShortcut> Widget::keyboardShortcuts() {
    return this->m_keyboard_shortcuts;
}