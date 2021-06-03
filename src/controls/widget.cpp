#include "widget.hpp"
#include "../application.hpp"

Widget::Widget() {

}

Widget::~Widget() {
    Application::get()->removeFromState(this);
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
    this->layout();

    return this;
}

Widget* Widget::remove(size_t parent_index) {
    Widget *child = this->children[parent_index];
    Application::get()->removeFromState(child);
    this->children.erase(this->children.begin() + parent_index);
    size_t i = 0;
    for (Widget *child : this->children) {
        child->parent_index = i;
        i++;
    }
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
    this->m_bg = background;
    this->update();

    return this;
}

Color Widget::foreground() {
    return this->m_fg;
}

Widget* Widget::setForeground(Color foreground) {
    this->m_fg = foreground;
    this->update();

    return this;
}

Widget* Widget::setFillPolicy(Fill fill_policy) {
    if (this->m_fill_policy != fill_policy) {
        this->m_fill_policy = fill_policy;
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
        this->layout();
    }

    return this;
}

Widget* Widget::hide() {
    if (this->m_is_visible) {
        this->m_is_visible = false;
        this->layout();
    }

    return this;
}

bool Widget::isVisible() {
    return this->m_is_visible;
}

bool Widget::isHovered() {
    return Application::get()->m_state->hovered == this;
}

bool Widget::isPressed() {
    return Application::get()->m_state->pressed == this;
}

bool Widget::isFocused() {
    return Application::get()->m_state->focused == this;
}

int Widget::state() {
    int _state = STATE_DEFAULT;
    auto app = Application::get();
    if (app->m_state->hovered == this) { _state |= STATE_HOVERED; }
    if (app->m_state->pressed == this) { _state |= STATE_PRESSED; }
    if (app->m_state->focused == this) { _state |= STATE_FOCUSED; }

    return _state;
}

Widget* Widget::update() {
    Application::get()->update();

    return this;
}

Widget* Widget::layout() {
    m_size_changed = true;
    Widget *parent = this->parent;
    while (parent) {
        if (parent->m_size_changed) {
            break;
        }
        parent->m_size_changed = true;
        parent = parent->parent;
    }
    update();

    return this;
}

void* Widget::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
    for (Widget *child : children) {
        if (child->isVisible()) {
            if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
                (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
                void *last = nullptr;
                if (child->isLayout()) {
                    last = (void*)child->propagateMouseEvent(window, state, event);
                } else {
                    child->handleMouseEvent(window, state, event);
                    last = (void*)child;
                }
                return last;
            }
        }
    }

    handleMouseEvent(window, state, event);
    return this;
}

void Widget::handleMouseEvent(Window *window, State *state, MouseEvent event) {
    switch (event.type) {
        case MouseEvent::Type::Down:
            state->pressed = this;
            state->focused = this;
            // TODO maybe add an on_focus callback?
            onMouseDown.notify(this, event);
            break;
        case MouseEvent::Type::Up:
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
            window->setTooltip(this);
            if (!state->pressed) {
                if (state->hovered) {
                    if (this != state->hovered) {
                        ((Widget*)state->hovered)->onMouseLeft.notify(this, event);
                    }
                }
                state->hovered = this;
                onMouseEntered.notify(this, event);
                onMouseMotion.notify(this, event);
            } else {
                if (state->pressed == this) { state->hovered = this; }
                else { state->hovered = nullptr; }
                ((Widget*)state->pressed)->onMouseMotion.notify(this, event);
            }
            break;
    }
    update();
}

void Widget::handleTextEvent(DrawingContext &dc, const char *text) {
    // Up to the widget to implement!
}

bool Widget::handleScrollEvent(ScrollEvent event) {
    // Up to the widget to implement!
    // Returns false to let the application know that
    // this widget doesnt handle scroll events.
    // Otherwise return true.
    return false;
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

Size Widget::size() {
    return m_size;
}

void Widget::setStyle(Style style) {
    this->style = style;
    layout();
}

bool Widget::isWidget() {
    return true;
}
