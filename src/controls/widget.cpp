#include "widget.hpp"
#include "../application.hpp"

Widget::Widget() {

}

Widget::~Widget() {
    Application::get()->getCurrentWindow()->removeFromState(this);
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
    Application::get()->getCurrentWindow()->removeFromState(child);
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
    return Application::get()->getCurrentWindow()->m_state->hovered == this;
}

bool Widget::isPressed() {
    return Application::get()->getCurrentWindow()->m_state->pressed == this;
}

bool Widget::isSoftFocused() {
    return Application::get()->getCurrentWindow()->m_state->soft_focused == this;
}

bool Widget::isHardFocused() {
    return Application::get()->getCurrentWindow()->m_state->hard_focused == this;
}

int Widget::state() {
    int widget_state = STATE_DEFAULT;
    State *app_state = Application::get()->getCurrentWindow()->m_state;
    if (app_state->hovered == this) { widget_state |= STATE_HOVERED; }
    if (app_state->pressed == this) { widget_state |= STATE_PRESSED; }
    if (app_state->soft_focused == this) { widget_state |= STATE_SOFT_FOCUSED; }
    if (app_state->hard_focused == this) { widget_state |= STATE_HARD_FOCUSED; }

    return widget_state;
}

Widget* Widget::update() {
    Application::get()->getCurrentWindow()->update();

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

int Widget::isFocusable() {
    return (int)FocusType::None;
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

void Widget::activate() {
    SDL_MouseButtonEvent event = { SDL_MOUSEBUTTONDOWN, SDL_GetTicks(), 0, 0, SDL_BUTTON_LEFT, SDL_PRESSED, 1, 0, -1, -1 };
    handleMouseEvent(Application::get()->getCurrentWindow(), Application::get()->getCurrentWindow()->m_state, event);
    event = { SDL_MOUSEBUTTONUP, SDL_GetTicks(), 0, 0, SDL_BUTTON_LEFT, SDL_RELEASED, 1, 0, -1, -1 };
    handleMouseEvent(Application::get()->getCurrentWindow(), Application::get()->getCurrentWindow()->m_state, event);
    Application::get()->getCurrentWindow()->m_state->hovered = nullptr;
    update();
}
