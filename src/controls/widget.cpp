#include "widget.hpp"
#include "../application.hpp"

Widget::Widget() {

}

Widget::~Widget() {
    Application::get()->removeFromState(this);
    for (Widget *child : this->children) {
        delete child;
    }
    delete this->context_menu;
    delete this->tooltip;
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

Widget* Widget::setFillPolicy(Fill fill_policy) {
    if (this->m_fill_policy != fill_policy) {
        this->m_fill_policy = fill_policy;
        this->layout();
        // TODO maybe we could manually change only the exapndable/non-expandable widgets
        // so we dont have to redo the whole sizehint calculation
        // but this would probably require a base class for all layouts
        // which wouldnt necessarily be bad
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
            // TODO maybe add an on_focus callback? yes, once we have keyboard navigation, also onFocusLost
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

Widget* Widget::propagateFocusEvent(FocusEvent event, State *state, Option<int> child_index) {
    if (event == FocusEvent::Forward) {
        int child_index_unwrapped = child_index ? child_index.unwrap() + 1 : 0;
        for (; child_index_unwrapped < (int)children.size(); child_index_unwrapped++) {
            Widget *child = children[child_index_unwrapped];
            if (child->isFocusable() && child->isVisible()) {
                if (state->focused) {
                    ((Widget*)state->focused)->onFocusLost.notify((Widget*)state->focused);
                }
                state->focused = child;
                child->onFocusGained.notify(child); // TODO maybe focus type, forward/reverse, keyboard/mouse
                return nullptr;
            }
            if (!child->propagateFocusEvent(event, state, Option<int>())) {
                return nullptr;
            }
        }
        if (parent) {
            return parent->propagateFocusEvent(event, state, Option<int>(parent_index));
        }

        return this;
    } else {
        int child_index_unwrapped = child_index ? child_index.unwrap() - 1 : (int)children.size() - 1;
        for (; child_index_unwrapped > -1; child_index_unwrapped--) {
            Widget *child = children[child_index_unwrapped];
            if (child->isFocusable() && child->isVisible()) {
                if (state->focused) {
                    ((Widget*)state->focused)->onFocusLost.notify((Widget*)state->focused);
                }
                state->focused = child;
                child->onFocusGained.notify(child);
                return nullptr;
            }
            if (!child->propagateFocusEvent(event, state, Option<int>())) {
                return nullptr;
            }
        }
        if (parent) {
            return parent->propagateFocusEvent(event, state, Option<int>(parent_index));
        }

        return this;
    }
}

bool Widget::isFocusable() {
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

int Widget::bind(int key, int modifiers, std::function<void()> callback) {
    Mod mods[4] = {Mod::None, Mod::None, Mod::None, Mod::None};

    if (modifiers & KMOD_CTRL) {
        mods[0] = Mod::Ctrl;
    }
    if (modifiers & KMOD_SHIFT) {
        mods[1] = Mod::Shift;
    }
    if (modifiers & KMOD_ALT) {
        mods[2] = Mod::Alt;
    }
    if (modifiers & KMOD_GUI) {
        mods[3] = Mod::Gui;
    }

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

void Widget::forEachWidget(std::function<void(Widget *widget)> action) {
    action(this);
    for (Widget *child : children) {
        child->forEachWidget(action);
    }
}
