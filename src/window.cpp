#include "application.hpp"
#include "window.hpp"
#include "resources.hpp"

u32 tooltipCallback(u32 interval, void *window) {
    Window *win = (Window*)window;
    win->draw_tooltip = true;
    win->update();
    win->pulse();

    return 0;
}

Window::Window(const char* title, Size size, Point point, i32 flags) {
    this->m_title = title;
    size.w = size.w < 120 ? 120 : size.w;
    this->size = size;

    m_win = SDL_CreateWindow(
        title,
        point.x,
        point.y,
        size.w, size.h,
        flags
    );
    if (!m_win) { fail("Failed to create SDL_Window"); }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    m_sdl_context = SDL_GL_CreateContext(m_win);
    SDL_GL_MakeCurrent(m_win, m_sdl_context);
    SDL_GL_SetSwapInterval(0);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        fail("FAILED_TO_INITIALIZE_GLAD");
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    dc = new DrawingContext(this);
}

Window::~Window() {
    Application &app = *Application::get();
    i32 index = 0;
    for (Window *window : app.m_windows) {
        if (window == this) {
            app.m_windows.erase(app.m_windows.begin() + index);
            if (app.current_window == this) { app.current_window = app.mainWindow(); }
            break;
        }
        index++;
    }
    delete m_main_widget;
    delete m_state;
    delete dc;
    SDL_GL_DeleteContext(m_sdl_context);
    SDL_DestroyWindow(m_win);
}

void Window::run() {
    Application::get()->m_windows.push_back(this);
    dc->default_font = Application::get()->mainWindow()->dc->default_font;
    show();
    if (onReady) {
        onReady(this);
    }
    m_state->hovered = m_main_widget;
}

void Window::handleSDLEvent(SDL_Event &event) {
    Application::get()->setCurrentWindow(this);
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            m_is_mouse_captured = true;
            SDL_CaptureMouse(SDL_TRUE);
            ContextEventResult result = propagateMouseEvent(MouseEvent(event.button));
            assert(result.widget && "result.widget should never be null because the mouse event should come from within the window and we should have at least the m_main_widget!");
            if (result.event == Window::ContextEvent::False) {
                if (MouseEvent(event.button).button == MouseEvent::Button::Right && !active_context_menu && result.widget->context_menu) {
                    active_context_menu = result.widget->context_menu;
                    context_menu_position_start = Point(event.button.x, event.button.y);
                } else {
                    active_context_menu = nullptr;
                }
            }
            break;
        }
        case SDL_MOUSEBUTTONUP:
            if (m_mouse_inside) {
                if (m_is_mouse_captured) {
                    m_is_mouse_captured = false;
                    SDL_CaptureMouse(SDL_FALSE);
                    if ((event.button.x < 0 || event.button.x > size.w) ||
                        (event.button.y < 0 || event.button.y > size.h)) {
                        if (m_state->pressed) {
                            SDL_MouseMotionEvent event = { SDL_MOUSEMOTION, SDL_GetTicks(), 0, 0, SDL_RELEASED, -1, -1, 0, 0 };
                            m_state->pressed->onMouseLeft.notify(m_state->pressed, MouseEvent(event));
                            m_state->pressed = nullptr;
                            update();
                        }
                        if (m_state->hovered) {
                            m_state->hovered = nullptr;
                            update();
                        }
                        break;
                    }
                }
                propagateMouseEvent(MouseEvent(event.button));
            } else {
                if (m_state->pressed) {
                    m_state->pressed = nullptr;
                    update();
                }
            }
            break;
        case SDL_MOUSEMOTION:
            if (tooltip_did_draw) {
                tooltip_did_draw = false;
                draw_tooltip = false;
            }
            propagateMouseEvent(MouseEvent(event.motion));
            break;
        case SDL_MOUSEWHEEL:
            if (m_state->hovered) {
                Widget *widget = m_state->hovered;
                bool handled = false;
                while (widget->parent) {
                    if (!widget->handleScrollEvent(ScrollEvent(event.wheel))) {
                        widget = widget->parent;
                    } else {
                        handled = true;
                        break;
                    }
                }
                if (!handled) {
                    widget->handleScrollEvent(ScrollEvent(event.wheel));
                }
            }
            break;
        case SDL_WINDOWEVENT:
            switch (event.window.event) {
                case SDL_WINDOWEVENT_ENTER:
                    m_mouse_inside = true;
                    break;
                case SDL_WINDOWEVENT_LEAVE:
                    m_mouse_inside = false;
                    if (m_state->hovered && !m_state->pressed) {
                        SDL_MouseMotionEvent event = { SDL_MOUSEMOTION, SDL_GetTicks(), 0, 0, SDL_RELEASED, -1, -1, 0, 0 };
                        m_state->hovered->onMouseLeft.notify(m_state->hovered, MouseEvent(event));
                        m_state->hovered = nullptr;
                        SDL_RemoveTimer(m_tooltip_callback);
                        update();
                    }
                    break;
                case SDL_WINDOWEVENT_CLOSE:
                    quit();
                    break;
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    if (onFocusLost) { onFocusLost(this); }
                    break;
            }
            break;
        case SDL_KEYDOWN: {
                SDL_Keycode key = event.key.keysym.sym;
                Uint16 mod = event.key.keysym.mod;
                Mod mods[4] = {Mod::None, Mod::None, Mod::None, Mod::None};
                if (mod & KMOD_CTRL) {
                    mods[0] = Mod::Ctrl;
                }
                if (mod & KMOD_SHIFT) {
                    mods[1] = Mod::Shift;
                }
                if (mod & KMOD_ALT) {
                    mods[2] = Mod::Alt;
                }
                if (mod & KMOD_GUI) {
                    mods[3] = Mod::Gui;
                }
                bool matched = false;
                Widget *focus_widget = m_state->soft_focused ? m_state->soft_focused : m_state->hard_focused;
                assert(m_main_widget && "The main widget should never be null!");
                if (key == SDLK_TAB && mods[0] == Mod::Ctrl && mods[1] == Mod::Shift) {
                    propagateFocusEvent(FocusEvent::Reverse, focus_widget ? focus_widget : m_main_widget);
                } else if (key == SDLK_TAB && mods[0] == Mod::Ctrl) {
                    propagateFocusEvent(FocusEvent::Forward, focus_widget ? focus_widget : m_main_widget);
                } else if (m_state->soft_focused && m_state->soft_focused != m_state->hard_focused && key == SDLK_SPACE) {
                    m_state->soft_focused->activate();
                    SDL_FlushEvent(SDL_TEXTINPUT);
                } else {
                    matchKeybind(matched, mods, key, m_keyboard_shortcuts);
                    if (!matched) {
                        if (m_state->hard_focused) {
                            matchKeybind(matched, mods, key, m_state->hard_focused->keyboardShortcuts());
                        }
                        if (!matched) {
                            if (key == SDLK_TAB && mods[1] == Mod::Shift) {
                                propagateFocusEvent(FocusEvent::Reverse, focus_widget ? focus_widget : m_main_widget);
                            } else if (key == SDLK_TAB) {
                                propagateFocusEvent(FocusEvent::Forward, focus_widget ? focus_widget : m_main_widget);
                            }
                        }
                    }
                }
            }
            break;
        case SDL_TEXTINPUT:
            if (m_state->hard_focused) {
                m_state->hard_focused->handleTextEvent(*dc, event.text.text);
            }
            break;
    }
}

void Window::draw() {
    if (SDL_GL_GetCurrentWindow() != m_win) { Application::get()->setCurrentWindow(this); }
    dc->renderer->shader.use();
    f32 projection[16] = {
         2.0f / size.w,  0.0f         ,  0.0f, 0.0f,
         0.0f         , -2.0f / size.h,  0.0f, 0.0f,
         0.0f         ,  0.0f         , -1.0f, 0.0f,
        -1.0f         ,  1.0f         , -0.0f, 1.0f
    };
    dc->renderer->shader.setMatrix4("u_projection", projection);
    dc->clear();
    dc->setClip(Rect(0, 0, size.w, size.h));
    m_main_widget->sizeHint(*dc);
    m_main_widget->draw(*dc, Rect(0, 0, size.w, size.h), m_main_widget->state());
    if (active_context_menu) {
        Size size = active_context_menu->sizeHint(*dc);
        active_context_menu->draw(*dc, Rect(context_menu_position_start.x, context_menu_position_start.y, size.w, size.h), active_context_menu->state());
    }
    if (m_state->tooltip && draw_tooltip) {
        drawTooltip();
        tooltip_did_draw = true;
    }
    dc->render();
}

Widget* Window::mainWidget() {
    return m_main_widget;
}

// TODO need to free the previous main widget but perhaps
// leave that to the user?? perhaps return the previous mainWidget here
// TODO preferrably return old main widget from method
void Window::setMainWidget(Widget *widget) {
    m_main_widget = widget;
    widget->update();
}

void Window::show() {
    draw();
    dc->swap_buffer(m_win);
}

Widget* Window::append(Widget *widget, Fill fill_policy, u32 proportion) {
    m_main_widget->append(widget, fill_policy, proportion);
    return m_main_widget;
}

void Window::update() {
    m_needs_update = true;
}

void Window::removeFromState(Widget *widget) {
    if (widget) {
        if (m_state->soft_focused == widget) {
            m_state->soft_focused = nullptr;
        }
        if (m_state->hard_focused == widget) {
            m_state->hard_focused = nullptr;
        }
        if (m_state->hard_focused == widget) {
            m_state->hard_focused = nullptr;
        }
        if (m_state->hovered == widget) {
            m_state->hovered = nullptr;
        }
        if (m_state->pressed == widget) {
            m_state->pressed = nullptr;
        }
        if (m_state->tooltip == widget) {
            m_state->tooltip = nullptr;
            draw_tooltip = false;
            tooltip_did_draw = false;
        }
        update();
    }
}

i32 Window::bind(i32 key, i32 modifiers, std::function<void()> callback) {
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

    m_keyboard_shortcuts.insert(
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

i32 Window::bind(i32 key, Mod modifier, std::function<void()> callback) {
    return bind(key, (i32)modifier, callback);
}

void Window::unbind(i32 key) {
    m_keyboard_shortcuts.erase(key);
}

void Window::quit() {
    if (onQuit) {
        if (!onQuit(this)) {
            return;
        }
    }
    m_running = false;
    if (!is_owned) { delete this; }
}

void Window::handleResizeEvent(i32 width, i32 height) {
    Application::get()->setCurrentWindow(this);
    active_context_menu = nullptr;
    size = Size(width, height);
    i32 w, h;
    SDL_GL_GetDrawableSize(m_win, &w, &h);
    glViewport(0, 0, w, h);
    show();
    if (onResize) {
        onResize(this);
    }
}

void Window::resize(i32 width, i32 height) {
    SDL_SetWindowSize(m_win, width, height);
    handleResizeEvent(width, height);
}

std::string Window::title() {
    return m_title;
}

void Window::setTitle(std::string title) {
    SDL_SetWindowTitle(m_win, title.c_str());
}

void Window::setTooltip(Widget *widget) {
    SDL_RemoveTimer(m_tooltip_callback);
    if (!widget->tooltip) {
        return;
    }
    m_state->tooltip = widget;
    m_tooltip_callback = SDL_AddTimer(m_tooltip_time, tooltipCallback, this);
    update();
}

void Window::drawTooltip() {
    if (!m_state->tooltip) {
        return;
    }
    dc->setClip(Rect(0, 0, size.w, size.h));
    Size s = m_state->tooltip->tooltip->sizeHint(*dc);
    i32 x, y;
    SDL_GetMouseState(&x, &y);

    // From what i remember the 12 and 16 were based on the mouse cursor size
    // TODO ideally this wouldnt be hardcoded
    Rect r = Rect(x + 12, y + 16, s.w, s.h);
    if (r.x + s.w > size.w) {
        r.x -= 12 + r.w;
    }
    if (r.y + s.h > size.h) {
        r.y -= 16 + r.h;
    }
    m_state->tooltip->tooltip->draw(*dc, r, m_state->tooltip->tooltip->state());
}

void Window::move(i32 x, i32 y) {
    SDL_SetWindowPosition(m_win, x, y);
}

void Window::center() {
    SDL_SetWindowPosition(m_win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Window::pulse() {
    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = AGRO_PULSE;
    userevent.data1 = NULL;
    userevent.data2 = NULL;
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);
}

Window::ContextEventResult Window::propagateMouseEvent(MouseEvent event) {
    Widget *last = nullptr;
    if (active_context_menu) {
        Widget *widget = active_context_menu;
        if (widget->isVisible()) {
            if ((event.x >= widget->rect.x && event.x <= widget->rect.x + widget->rect.w) &&
                (event.y >= widget->rect.y && event.y <= widget->rect.y + widget->rect.h)) {
                if (widget->isLayout()) {
                    last = widget->propagateMouseEvent(this, m_state, event);
                } else {
                    widget->handleMouseEvent(this, m_state, event);
                    last = widget;
                }
                return ContextEventResult{ ContextEvent::True, last };
            }
        }
        last = m_main_widget->propagateMouseEvent(this, m_state, MouseEvent(event));
    } else {
        last = m_main_widget->propagateMouseEvent(this, m_state, MouseEvent(event));
    }
    return ContextEventResult{ ContextEvent::False, last };
}

void Window::layout() {
    // TODO this doesnt update widgets like scrollbars which are part of scrollable
    m_main_widget->forEachWidget([](Widget *widget) {
        widget->m_size_changed = true;
        widget->m_text_changed = true;
    });
    m_main_widget->sizeHint(*dc);
}

void Window::propagateFocusEvent(FocusEvent event, Widget *focused) {
    assert(focused && "The passed in focused widget should never be null!");
    if (focused->isFocusable() && focused->isVisible()) {
        if (!focused->handleFocusEvent(event, m_state, FocusPropagationData(focused, Option<i32>()))) {
            if (m_main_widget->isFocusable() && m_main_widget->isVisible()) {
                info("Unhandled focus event, starting again from m_main_widget");
                m_main_widget->handleFocusEvent(event, m_state, FocusPropagationData(m_main_widget, Option<i32>()));
            }
        }
    } else {
        // TODO we might want to either put this behind debug options or remove it
        // once we go over all widgets and make sure they are keyboard navigable
        warn("propagateFocusEvent encountered a non-focusable widget");
        warn(std::string("focused->name(): ") + focused->name());
        if (focused->parent) { warn(std::string("focused->parent->name(): ") + focused->parent->name()); }
        if (m_main_widget->isFocusable() && m_main_widget->isVisible()) {
            m_main_widget->handleFocusEvent(event, m_state, FocusPropagationData(m_main_widget, Option<i32>()));
        }
    }
    update();
}

void Window::matchKeybind(bool &matched, Mod mods[4], SDL_Keycode key, std::unordered_map<i32, KeyboardShortcut> keybinds) {
    for (auto hotkey : keybinds) {
        if (hotkey.second.key == key) {
            bool mods_matched = true;
            if (hotkey.second.ctrl != mods[0]) {
                mods_matched = false;
            }
            if (hotkey.second.shift != mods[1]) {
                mods_matched = false;
            }
            if (hotkey.second.alt != mods[2]) {
                mods_matched = false;
            }
            if (hotkey.second.gui != mods[3]) {
                mods_matched = false;
            }
            if (mods_matched) {
                hotkey.second.callback();
                SDL_FlushEvent(SDL_TEXTINPUT);
                matched = true;
                break;
            }
        }
    }
}
