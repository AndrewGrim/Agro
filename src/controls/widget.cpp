#include "widget.hpp"
#include "../application.hpp"

Widget::Widget() {}

Widget::~Widget() {
    Application::get()->currentWindow()->removeFromState(this);
    for (Widget *child : this->children) {
        delete child;
    }
    delete this->context_menu;
    delete this->tooltip;
}

Widget* Widget::append(Widget* widget, Fill fill_policy, u32 proportion) {
    if (widget->parent) {
        widget->parent->remove(widget->parent_index);
    }
    widget->parent = this;
    widget->setFillPolicy(fill_policy);
    widget->setProportion(proportion);
    this->children.push_back(widget);
    widget->parent_index = this->children.size() - 1;
    this->layout(LAYOUT_CHILD);

    return this;
}

Widget* Widget::remove(u64 parent_index) {
    Widget *child = this->children[parent_index];
    Application::get()->currentWindow()->removeFromState(child);
    this->children.erase(this->children.begin() + parent_index);
    u64 i = 0;
    for (Widget *child : this->children) {
        child->parent_index = i;
        i++;
    }
    this->layout(LAYOUT_CHILD);

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
        this->layout(LAYOUT_CHILD);
        // TODO maybe we could manually change only the exapndable/non-expandable widgets
        // so we dont have to redo the whole sizehint calculation
        // but this would probably require a base class for all layouts
        // which wouldnt necessarily be bad
        // update: we could submit more granular layout events such as
        // child added, child removed, child fill policy changed etc and ideally a widget*
    }

    return this;
}

Fill Widget::fillPolicy() {
    return this->m_fill_policy;
}

Widget* Widget::show() {
    if (!this->m_is_visible) {
        this->m_is_visible = true;
        this->layout(LAYOUT_CHILD);
    }

    return this;
}

Widget* Widget::hide() {
    if (this->m_is_visible) {
        this->m_is_visible = false;
        this->layout(LAYOUT_CHILD);
    }

    return this;
}

bool Widget::isVisible() {
    return this->m_is_visible;
}

bool Widget::isHovered() {
    return Application::get()->currentWindow()->m_state->hovered == this;
}

bool Widget::isPressed() {
    return Application::get()->currentWindow()->m_state->pressed == this;
}

bool Widget::isSoftFocused() {
    return Application::get()->currentWindow()->m_state->soft_focused == this;
}

bool Widget::isHardFocused() {
    return Application::get()->currentWindow()->m_state->hard_focused == this;
}

i32 Widget::state() {
    i32 widget_state = STATE_DEFAULT;
    State *app_state = Application::get()->currentWindow()->m_state;
    if (app_state->hovered == this) { widget_state |= STATE_HOVERED; }
    if (app_state->pressed == this) { widget_state |= STATE_PRESSED; }
    if (app_state->soft_focused == this) { widget_state |= STATE_SOFT_FOCUSED; }
    if (app_state->hard_focused == this) { widget_state |= STATE_HARD_FOCUSED; }

    return widget_state;
}

Widget* Widget::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
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

void Widget::handleMouseEvent(Window *window, State *state, MouseEvent event) {
    switch (event.type) {
        case MouseEvent::Type::Down:
            state->pressed = this;
            setHardFocus(state);
            onMouseDown.notify(this, event);
            break;
        case MouseEvent::Type::Up:
            state->hovered = this;
            onMouseUp.notify(this, event);
            if (this == state->pressed) {
                onMouseClick.notify(this, event);
            } else {
                if (state->pressed) {
                    state->pressed->onMouseLeft.notify(this, event);
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
                        state->hovered->onMouseLeft.notify(this, event);
                    }
                }
                state->hovered = this;
                onMouseEntered.notify(this, event);
                onMouseMotion.notify(this, event);
            } else {
                if (state->pressed == this) { state->hovered = this; }
                else { state->hovered = nullptr; }
                state->pressed->onMouseMotion.notify(this, event);
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

Widget* Widget::setSoftFocus(FocusEvent event, State *state) {
    if (state->soft_focused) {
        state->soft_focused->onFocusLost.notify(state->soft_focused, event);
    }
    state->soft_focused = this;
    this->onFocusGained.notify(this, event);
    return this;
}

void Widget::setHardFocus(State *state) {
    if ((FocusType)isFocusable() == FocusType::Focusable) {
        // We only want to send one event in the case
        // that soft and hard focus are currently the same widget.
        if (state->soft_focused && state->soft_focused != state->hard_focused) { state->soft_focused->onFocusLost.notify(state->soft_focused, FocusEvent::Activate); }
        if (state->hard_focused && state->soft_focused != state->hard_focused) { state->hard_focused->onFocusLost.notify(state->hard_focused, FocusEvent::Activate); }
        // TODO WTF? this triggers over and over for the same widget if we just click it with a mouse
        // also we dont handle focus event callback properly accross different windows
        if ((state->soft_focused && state->hard_focused) && (state->soft_focused == state->hard_focused)) { state->soft_focused->onFocusLost.notify(state->soft_focused, FocusEvent::Activate); }
        state->soft_focused = this;
        state->hard_focused = this;
        onFocusGained.notify(this, FocusEvent::Activate);
    }
    update();
}

Widget* Widget::handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) {
    if (data.origin != this) {
        return setSoftFocus(event, state);
    }
    if (parent) {
        return parent->handleFocusEvent(event, state, FocusPropagationData(this, parent_index));
    }
    return nullptr;
}

i32 Widget::isFocusable() {
    return (i32)FocusType::None;
}

bool Widget::isLayout() {
    return false;
}

bool Widget::isScrollable() {
    return false;
}

u32 Widget::proportion() {
    return m_proportion;
}

Widget* Widget::setProportion(u32 proportion) {
    if (m_proportion != proportion) {
        m_proportion = proportion ? proportion : 1;
        // TODO this one along with fill policy should be changed such that
        // they can be updated individually and only require and update rather
        // than a layout
        layout(LAYOUT_CHILD);
    }

    return this;
}

bool Widget::bind(i32 key, i32 modifiers, std::function<void()> callback) {
    return m_keyboard_shortcuts.insert(KeyboardShortcut(key, modifiers), callback);
}

bool Widget::bind(i32 key, Mod modifier, std::function<void()> callback) {
    return bind(key, (i32)modifier, callback);
}

bool Widget::unbind(i32 key, i32 modifiers) {
    return m_keyboard_shortcuts.remove(KeyboardShortcut(key, modifiers));
}

HashMap<KeyboardShortcut, std::function<void()>>& Widget::keyboardShortcuts() {
    return m_keyboard_shortcuts;
}

Size Widget::size() {
    return m_size;
}

bool Widget::isWidget() {
    return true;
}

void Widget::forEachDrawable(std::function<void(Drawable *drawable)> action) {
    action(this);
    for (Widget *child : children) {
        child->forEachDrawable(action);
    }
}

void Widget::activate() {
    // TODO maybe we should add an onActivate callback that could be invoked in a more ergonomic way
    // mouse clicks could automatically notify onActivate?
    SDL_MouseButtonEvent event = { SDL_MOUSEBUTTONDOWN, SDL_GetTicks(), 0, 0, SDL_BUTTON_LEFT, SDL_PRESSED, 1, 0, -1, -1 };
    handleMouseEvent(Application::get()->currentWindow(), Application::get()->currentWindow()->m_state, event);
    event = { SDL_MOUSEBUTTONUP, SDL_GetTicks(), 0, 0, SDL_BUTTON_LEFT, SDL_RELEASED, 1, 0, -1, -1 };
    handleMouseEvent(Application::get()->currentWindow(), Application::get()->currentWindow()->m_state, event);
    Application::get()->currentWindow()->m_state->hovered = nullptr;
    update();
}
