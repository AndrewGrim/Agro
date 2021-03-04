#include <glad/glad.h>

# include "window.hpp"

// This is needed on Windows and supposedly on MacOS (not tested) to
// redraw the window while the user is resizing it.
// I guess alternatively we could paint in a different thread??
int forcePaintWhileResizing(void *data, SDL_Event *event) {
    switch (event->type) {
        case SDL_WINDOWEVENT:
            if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                Window *app = (Window*)data;
                app->handleResizeEvent(event->window.data1, event->window.data2);
                if (app->onResize) {
                    app->onResize(app);
                }
                return 0;
            }
            break;
    }
    return 1;
}

Window::Window(const char* title, Size size) {
    this->title = title;
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
    SDL_GL_SetSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        println("Failed to initialize GLAD");
    }

    dc = new DrawingContext(this);
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
    dc->renderer->shader.setMatrix4(
        "u_projection", 
        glm::ortho(
            0.0f, static_cast<float>(size.w),
            static_cast<float>(size.h), 0.0f,
            -1.0f, 1.0f
        ) 
    );
    dc->clear();
    dc->setClip(Rect(0, 0, size.w, size.h));
    m_main_widget->draw(dc, Rect(0, 0, size.w, size.h));
    dc->render();
}

Widget* Window::mainWidget() {
    return m_main_widget;
}

void Window::setMainWidget(Widget *widget) {
    m_main_widget = widget;
    widget->app = (void*)this;
    for (Widget *child : widget->children) {
        child->app = this;
        child->attachApp(this);
    }
}

void Window::show() {
    draw();
    dc->swap_buffer(m_win);
}

void Window::run() {
    dc->default_font = new Font("fonts/DejaVu/DejaVuSans.ttf", 14, Font::Type::Sans);
    setMainWidget(m_main_widget);
    show();
    if (onReady) {
        onReady(this);
    }
    m_state->hovered = m_main_widget;
    int32_t mouse_movement_x = 0;
    int32_t mouse_movement_y = 0;
    SDL_StartTextInput();
    while (m_running) {
        uint32_t frame_start = SDL_GetTicks();
        SDL_Event event;
        if (SDL_WaitEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    m_is_mouse_captured = true;
                    SDL_CaptureMouse(SDL_TRUE);
                    m_state->pressed = m_main_widget->propagateMouseEvent(m_state, MouseEvent(event.button));
                    break;
                case SDL_MOUSEBUTTONUP:
                    if (m_mouse_inside) {
                        if (m_is_mouse_captured) {
                            m_is_mouse_captured = false;
                            SDL_CaptureMouse(SDL_FALSE);
                            if ((event.button.x < 0 || event.button.x > size.w) ||
                                (event.button.y < 0 || event.button.y > size.w)) {
                                if (m_state->pressed) {
                                    SDL_MouseMotionEvent event = { SDL_MOUSEMOTION, SDL_GetTicks(), 0, 0, SDL_RELEASED, -1, -1, 0, 0 };
                                    ((Widget*)m_state->pressed)->onMouseLeft.notify(((Widget*)m_state->pressed), MouseEvent(event));
                                    ((Widget*)m_state->pressed)->setPressed(false);
                                    m_state->pressed = nullptr;
                                }
                                if (m_state->hovered) {
                                    ((Widget*)m_state->hovered)->setHovered(false);
                                    m_state->hovered = nullptr;
                                }
                                break;
                            }
                        }
                        m_main_widget->propagateMouseEvent(m_state, MouseEvent(event.button));
                    } else {
                        if (m_state->pressed) {
                            ((Widget*)m_state->pressed)->setPressed(false);
                            m_state->pressed = nullptr;
                        }
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (event.motion.timestamp >= frame_start) {
                        event.motion.xrel += mouse_movement_x;
                        event.motion.yrel += mouse_movement_y;
                        m_state->hovered = m_main_widget->propagateMouseEvent(m_state, MouseEvent(event.motion));
                        mouse_movement_x = 0;
                        mouse_movement_y = 0;
                    } else {
                        mouse_movement_x += event.motion.xrel;
                        mouse_movement_y += event.motion.yrel;
                    }
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
                        // We are using the below instead of `SDL_WINDOWEVENT_RESIZED` because this one
                        // supposedly also triggers when the system changes the window size.
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            // TODO Gets handled in `forcePaintWhileResizing()` so this is probably not needed anymore.
                            handleResizeEvent(event.window.data1, event.window.data2);
                            if (onResize) {
                                onResize(this);
                            }
                            break;
                        case SDL_WINDOWEVENT_ENTER:
                            m_mouse_inside = true;
                            break;
                        case SDL_WINDOWEVENT_LEAVE:
                            m_mouse_inside = false;
                            if (m_state->hovered && !m_state->pressed) {
                                ((Widget*)m_state->hovered)->setHovered(false);
                                SDL_MouseMotionEvent event = { SDL_MOUSEMOTION, SDL_GetTicks(), 0, 0, SDL_RELEASED, -1, -1, 0, 0 };
                                ((Widget*)m_state->hovered)->onMouseLeft.notify(((Widget*)m_state->hovered), MouseEvent(event));
                                m_state->hovered = nullptr;
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
                        // TODO add matching for hotkeys and the widget itself
                        for (auto hotkey : m_keyboard_shortcuts) {
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
                        if (!matched && m_state->focused) {
                            for (auto hotkey : ((Widget*)(m_state->focused))->keyboardShortcuts()) {
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
                    }
                    break;
                case SDL_TEXTINPUT:
                    if (m_state->focused) {
                        ((Widget*)m_state->focused)->handleTextEvent(dc, event.text.text);
                    }
                    break;
                case SDL_QUIT:
                    quit();
            }
        }
        if (this->m_needs_update) {
            this->show();
            this->m_needs_update = false;
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
        update();
    }
}

// TODO implement left/right specific modifiers any the other remaining ones
int Window::bind(int key, int modifiers, std::function<void()> callback) {
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
    size = Size(width, height);
    int w, h;
    SDL_GL_GetDrawableSize(m_win, &w, &h);
    glViewport(0, 0, w, h);
    m_needs_update = true;
    show();
}

void Window::resize(int width, int height) {
    SDL_SetWindowSize(m_win, width, height);
    handleResizeEvent(width, height);
}