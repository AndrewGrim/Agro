#include "application.hpp"
#include "window.hpp"
#include "resources.hpp"

uint32_t tooltipCallback(uint32_t interval, void *window) {
    Window *win = (Window*)window;
    win->draw_tooltip = true;
    win->update();
    win->pulse();

    return 0;
}

// This is needed on Windows and on MacOS to
// redraw the window while the user is resizing it.
int forcePaintWhileResizing(void *data, SDL_Event *event) {
    switch (event->type) {
        case SDL_WINDOWEVENT:
            // We are using the below instead of `SDL_WINDOWEVENT_RESIZED` because this one
            // supposedly also triggers when the system changes the window size.
            if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                Window *win = (Window*)data;
                win->handleResizeEvent(event->window.data1, event->window.data2);
                if (win->onResize) {
                    win->onResize(win);
                }
                return 0;
            }
            break;
    }
    return 1;
}

Window::Window(const char* title, Size size) {
    this->m_title = title;
    this->size = size;

    m_win = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        static_cast<int>(size.w), static_cast<int>(size.h),
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    SDL_SetEventFilter(forcePaintWhileResizing, this);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    m_sdl_context = SDL_GL_CreateContext(m_win);
    SDL_GL_MakeCurrent(m_win, m_sdl_context); // TODO this will need to be called when switching between windows
    SDL_GL_SetSwapInterval(0);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        error("FAILED_TO_INITIALIZE_GLAD");
    }

    dc = new DrawingContext();
}

Window::~Window() {
    delete m_main_widget;
    delete m_state;
    delete dc;
    SDL_GL_DeleteContext(m_sdl_context);
    SDL_DestroyWindow(m_win);
}

void Window::draw() {
    dc->renderer->shader.use();
    float projection[16] = {
         2.0f / size.w,  0.0f         ,  0.0f, 0.0f,
         0.0f         , -2.0f / size.h,  0.0f, 0.0f,
         0.0f         ,  0.0f         , -1.0f, 0.0f,
        -1.0f         ,  1.0f         , -0.0f, 1.0f
    };
    dc->renderer->shader.setMatrix4("u_projection", projection);
    dc->clear();
    dc->setClip(Rect(0, 0, size.w, size.h));
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

void Window::run() {
    dc->default_font = new Font(DejaVuSans_ttf, DejaVuSans_ttf_length, 14, Font::Type::Sans);
    setMainWidget(m_main_widget);
    show();
    if (onReady) {
        onReady(this);
    }
    m_state->hovered = m_main_widget;
    uint32_t fps = 60; // TODO we should query the refresh rate of the monitor the window is on, also allow for overrides
    uint32_t frame_time = 1000 / fps;
    SDL_StartTextInput();
    while (m_running) {
        DELAY:;
        uint32_t frame_start = SDL_GetTicks();
        SDL_Event event;
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    m_is_mouse_captured = true;
                    SDL_CaptureMouse(SDL_TRUE);
                    if (propagateMouseEvent(MouseEvent(event.button)) == Window::ContextEvent::False) {
                        if (MouseEvent(event.button).button == MouseEvent::Button::Right && !active_context_menu && ((Widget*)m_state->focused)->context_menu) {
                            active_context_menu = ((Widget*)m_state->focused)->context_menu;
                            context_menu_position_start = Point(event.button.x, event.button.y);
                        } else {
                            active_context_menu = nullptr;
                        }
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (m_mouse_inside) {
                        if (m_is_mouse_captured) {
                            m_is_mouse_captured = false;
                            SDL_CaptureMouse(SDL_FALSE);
                            if ((event.button.x < 0 || event.button.x > size.w) ||
                                (event.button.y < 0 || event.button.y > size.h)) {
                                if (m_state->pressed) {
                                    SDL_MouseMotionEvent event = { SDL_MOUSEMOTION, SDL_GetTicks(), 0, 0, SDL_RELEASED, -1, -1, 0, 0 };
                                    ((Widget*)m_state->pressed)->onMouseLeft.notify(((Widget*)m_state->pressed), MouseEvent(event));
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
                        Widget *widget = (Widget*)m_state->hovered;
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
                                ((Widget*)m_state->hovered)->onMouseLeft.notify(((Widget*)m_state->hovered), MouseEvent(event));
                                m_state->hovered = nullptr;
                                SDL_RemoveTimer(m_tooltip_callback);
                                update();
                                delay_till = 0;
                            }
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
                        if (m_state->focused && key == SDLK_TAB && mods[0] == Mod::Ctrl && mods[1] == Mod::Shift) {
                            propagateFocusEvent(FocusEvent::Reverse, (Widget*)m_state->focused);
                        } else if (m_state->focused && key == SDLK_TAB && mods[0] == Mod::Ctrl) {
                            propagateFocusEvent(FocusEvent::Forward, (Widget*)m_state->focused);
                        } else {
                            matchKeybind(matched, mods, key, m_keyboard_shortcuts);
                            if (!matched && m_state->focused) {
                                matchKeybind(matched, mods, key, ((Widget*)(m_state->focused))->keyboardShortcuts());
                                if (!matched) {
                                    if (!matched && key == SDLK_TAB && mods[1] == Mod::Shift) {
                                        propagateFocusEvent(FocusEvent::Reverse, (Widget*)m_state->focused);
                                    } else if (!matched && key == SDLK_TAB) {
                                        propagateFocusEvent(FocusEvent::Forward, (Widget*)m_state->focused);
                                    }
                                }
                            }
                        }
                    }
                    break;
                case SDL_TEXTINPUT:
                    if (m_state->focused) {
                        ((Widget*)m_state->focused)->handleTextEvent(*dc, event.text.text);
                    }
                    break;
                case SDL_QUIT:
                    quit();
            }
        }
        if (delay_till) {
            if (SDL_GetTicks() < delay_till) {
                goto DELAY;
            } else {
                delay_till = 0;
            }
        }
        if (m_needs_update) {
            show();
            m_needs_update = false;
        }
        uint32_t frame_end = SDL_GetTicks() - frame_start;
        if (frame_time > frame_end) {
            delay_till = SDL_GetTicks() + (frame_time - frame_end);
        } else {
            delay_till = 0;
        }
    }
    SDL_StopTextInput();

    delete this;
}

Widget* Window::append(Widget* widget, Fill fill_policy, unsigned int proportion) {
    m_main_widget->append(widget, fill_policy, proportion);
    return m_main_widget;
}

void Window::update() {
    m_needs_update = true;
}

void Window::removeFromState(void *widget) {
    if (widget) {
        if (m_state->focused == widget) {
            m_state->focused = nullptr;
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

int Window::bind(int key, int modifiers, std::function<void()> callback) {
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

int Window::bind(int key, Mod modifier, std::function<void()> callback) {
    return bind(key, (int)modifier, callback);
}

void Window::unbind(int key) {
    m_keyboard_shortcuts.erase(key);
}

void Window::quit() {
    if (onQuit) {
        if (!onQuit(this)) {
            return;
        }
    }
    m_running = false;
}

void Window::handleResizeEvent(int width, int height) {
    active_context_menu = nullptr;
    size = Size(width, height);
    int w, h;
    SDL_GL_GetDrawableSize(m_win, &w, &h);
    glViewport(0, 0, w, h);
    show();
}

void Window::resize(int width, int height) {
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
    Widget *w = (Widget*)m_state->tooltip;
    if (!w->tooltip) {
        return;
    }
    dc->setClip(Rect(0, 0, size.w, size.h));
    Size s = w->tooltip->sizeHint(*dc);
    int x, y;
    SDL_GetMouseState(&x, &y);

    // From what i remember the 12 and 16 were based on the mouse size
    Rect r = Rect(x + 12, y + 16, s.w, s.h);
    if (r.x + s.w > size.w) {
        r.x -= 12 + r.w;
    }
    if (r.y + s.h > size.h) {
        r.y -= 16 + r.h;
    }
    w->tooltip->draw(*dc, r, w->tooltip->state());
}

void Window::move(int x, int y) {
    SDL_SetWindowPosition(m_win, x, y);
}

void Window::center() {
    SDL_SetWindowPosition(m_win, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Window::pulse() {
    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);
    delay_till = 0;
}

Window::ContextEvent Window::propagateMouseEvent(MouseEvent event) {
    if (active_context_menu) {
        Widget *widget = active_context_menu;
        if (widget->isVisible()) {
            if ((event.x >= widget->rect.x && event.x <= widget->rect.x + widget->rect.w) &&
                (event.y >= widget->rect.y && event.y <= widget->rect.y + widget->rect.h)) {
                if (widget->isLayout()) {
                    widget->propagateMouseEvent(this, m_state, event);
                } else {
                    widget->handleMouseEvent(this, m_state, event);
                }
                return ContextEvent::True;
            }
        }
        m_main_widget->propagateMouseEvent(this, m_state, MouseEvent(event));
    } else {
        m_main_widget->propagateMouseEvent(this, m_state, MouseEvent(event));
    }
    return ContextEvent::False;
}

void Window::layout() {
    // TODO this doesnt update widgets like scrollbars which are part of scrollable
    m_main_widget->forEachWidget([](Widget *widget) {
        widget->m_size_changed = true;
    });
    m_main_widget->sizeHint(*Application::get()->dc);
}

void Window::propagateFocusEvent(FocusEvent event, Widget *focused) {
    // TODO remove
    if (event == FocusEvent::Forward) {
        focused->style.text_background = Color("#00ff0055");
        focused->style.widget_background = Color("#00ff0055");
        focused->style.window_background = Color("#00ff0055");
        // focused->style.accent_widget_background = Color("#00ff0055");
    } else {
        focused->style.text_background = Color("#ff00ff55");
        focused->style.widget_background = Color("#ff00ff55");
        focused->style.window_background = Color("#ff00ff55");
        // focused->style.accent_widget_background = Color("#ff00ff55");
    }

    Widget *root = focused->propagateFocusEvent(event, m_state, Option<int>());
    if (root) {
        // also i think for layouts and capturing widgets like lineedit
        // or for compund widgets we could implement soft focus
        // which would allow the user to select where the focus will go next?
        // should we always soft focus by default??
        // and when we press space or something it activates the widget giving it hardfocus
        // this would be akin to clicking on the widget with your mouse
        // soft focus and hard focus would be independant
        // also this could be quite important because the notebook widget
        // you have to click it to activate it (atm anyway) and that always steals focus
        // same with interacting with a scrollbar using a mouse
        if (!root->propagateFocusEvent(event, m_state, Option<int>())) {
            return;
        }
        warn("focus not handled!");
        warn(root->name());
        root->style.text_background = Color("#ff00ff");
        root->style.widget_background = Color("#ff00ff");
        root->style.window_background = Color("#ff00ff");
        root->style.accent_widget_background = Color("#ff00ff");
        // if we hit this point do we just try to propagate focus
        // from mainWidget ?? that doesnt sound too bad
        // in fact when we get to this point we are hitting mainWidget
        // barring some exceptions
        // perhaps we check whether root is mainWidget and if not then
        // we propagate through mainWidget as well
        // but maybe do that at then end because we want to catch all widgets
        // that cannot propagate correctly
        if (root != m_main_widget) {
            warn("not main widget");
            m_main_widget->propagateFocusEvent(event, m_state, Option<int>());
        }
    }
    update();
}

void Window::matchKeybind(bool &matched, Mod mods[4], SDL_Keycode key, std::unordered_map<int, KeyboardShortcut> keybinds) {
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
