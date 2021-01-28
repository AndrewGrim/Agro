#include <iostream>

#include <glad/glad.h>

#include "app.hpp"

Cursors::Cursors() {

}

Cursors::~Cursors() {
    SDL_FreeCursor(arrow);
    SDL_FreeCursor(i_beam);
    SDL_FreeCursor(wait);
    SDL_FreeCursor(crosshair);
    SDL_FreeCursor(wait_arrow);
    SDL_FreeCursor(size_nwse);
    SDL_FreeCursor(size_nesw);
    SDL_FreeCursor(size_we);
    SDL_FreeCursor(size_ns);
    SDL_FreeCursor(size_all);
    SDL_FreeCursor(no);
    SDL_FreeCursor(hand);
}

// This is needed on Windows and supposedly on MacOS (not tested) to
// redraw the window while the user is resizing it.
int forcePaintWhileResizing(void *data, SDL_Event *event) {
    switch (event->type) {
        case SDL_WINDOWEVENT:
            if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                Application *app = (Application*)data;
                app->handleResizeEvent(event->window.data1, event->window.data2);
                return 0;
            }
            break;
    }
    return 1;
}

Application::Application(const char* title, Size size) {
    this->m_size = size;

    SDL_Init(SDL_INIT_VIDEO);
    this->m_win = SDL_CreateWindow(
        title, 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        static_cast<int>(size.w), static_cast<int>(size.h),
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    SDL_SetEventFilter(forcePaintWhileResizing, this);
    // TODO dont capture the mouse all the time only when pressed
    // and capture long enough to reset hover when exiting window
    // NOTE: Does not work on X11.
    // UPDATE: Does not work until window has focus, but on X11
    // its super important to release the mouse because you cant
    // even click the close window decorator.
    // NOTE: mouse left and entered dont work when capturing on X11
    SDL_CaptureMouse(SDL_TRUE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    m_sdl_context = SDL_GL_CreateContext(this->m_win);
    SDL_GL_MakeCurrent(this->m_win, this->m_sdl_context);
    SDL_GL_SetSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        println("Failed to initialize GLAD");
    }

    dc = new DrawingContext(this);
    m_cursors = new Cursors();
}

Application::~Application() {
    delete m_main_widget;
    delete m_state;
    delete dc;
    delete m_cursors;
    SDL_GL_DeleteContext(this->m_sdl_context);
    SDL_DestroyWindow(this->m_win);
    SDL_Quit();
}

void Application::draw() {
    dc->renderer->shader.use();
    dc->renderer->shader.setMatrix4(
        "u_projection", 
        glm::ortho(
            0.0f, static_cast<float>(this->m_size.w),
            static_cast<float>(this->m_size.h), 0.0f,
            -1.0f, 1.0f
        ) 
    );
    this->dc->clear();
    this->dc->setClip(Rect(0, 0, this->m_size.w, this->m_size.h));
    this->m_main_widget->draw(this->dc, Rect(0, 0, this->m_size.w, this->m_size.h));
    this->dc->render();
}

Widget* Application::mainWidget() {
    return this->m_main_widget;
}

void Application::setMainWidget(Widget *widget) {
    this->m_main_widget = widget;
    widget->app = (void*)this;
    for (Widget *child : widget->children) {
        child->app = this;
        child->attachApp(this);
    }
}

void Application::show() {
    this->draw();
    this->dc->swap_buffer(this->m_win);
}

void Application::run() {
    this->dc->default_font = new Font("fonts/DejaVu/DejaVuSans.ttf", 14, Font::Type::Sans);
    this->setMainWidget(this->m_main_widget);
    this->show();
    if (this->onReady) {
        this->onReady(this);
    }
    int32_t mouse_movement_x = 0;
    int32_t mouse_movement_y = 0;
    SDL_StartTextInput();
    while (m_running) {
        uint32_t frame_start = SDL_GetTicks();
        SDL_Event event;
        if (SDL_WaitEvent(&event)) {
            int64_t time_since_last_event = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->m_last_event_time).count();
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    this->m_state->pressed = this->m_main_widget->propagateMouseEvent(this->m_state, MouseEvent(event.button, time_since_last_event));
                    break;
                case SDL_MOUSEBUTTONUP:
                    this->m_main_widget->propagateMouseEvent(this->m_state, MouseEvent(event.button, time_since_last_event));
                    break;
                case SDL_MOUSEMOTION:
                    if (event.motion.timestamp >= frame_start) {
                        event.motion.xrel += mouse_movement_x;
                        event.motion.yrel += mouse_movement_y;
                        this->m_state->hovered = this->m_main_widget->propagateMouseEvent(this->m_state, MouseEvent(event.motion, time_since_last_event));
                        mouse_movement_x = 0;
                        mouse_movement_y = 0;
                    } else {
                        mouse_movement_x += event.motion.xrel;
                        mouse_movement_y += event.motion.yrel;
                    }
                    break;
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        // We are using the below instead of `SDL_WINDOWEVENT_RESIZED` because this one
                        // supposedly also triggers when the system changes the window size.
                        case SDL_WINDOWEVENT_SIZE_CHANGED:
                            // TODO Gets handled in `forcePaintWhileResizing()` so this is probably not needed anymore.
                            this->handleResizeEvent(event.window.data1, event.window.data2);
                            break;
                        case SDL_WINDOWEVENT_LEAVE:
                            // TODO handle mouse up
                            if (m_state->hovered) {
                                ((Widget*)m_state->hovered)->setHovered(false);
                                if (((Widget*)m_state->hovered)->onMouseLeft) {
                                    SDL_MouseMotionEvent event = {
                                        SDL_MOUSEMOTION,
                                        SDL_GetTicks(),
                                        0,
                                        0,
                                        SDL_RELEASED,
                                        -1,
                                        -1,
                                        0,
                                        0,
                                    };
                                    ((Widget*)m_state->hovered)->onMouseLeft(MouseEvent(event, time_since_last_event));
                                }
                                m_state->hovered = nullptr;
                            }
                            println("MOUSE LEFT");
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
            if (this->m_last_event.second == EventHandler::Accepted) {
                this->m_last_event_time = std::chrono::steady_clock::now();
            }
        }
        if (this->m_needs_update) {
            this->show();
            this->m_needs_update = false;
            this->m_layout_changed = false;
        }
    }
    SDL_StopTextInput();

    delete this;
}

Widget* Application::append(Widget* widget, Fill fill_policy, unsigned int proportion) {
    this->m_main_widget->append(widget, fill_policy, proportion);
    return this->m_main_widget;
}

std::pair<Application::Event, Application::EventHandler> Application::lastEvent() {
    return this->m_last_event;
}

void Application::setLastEvent(std::pair<Application::Event, Application::EventHandler> event) {
    this->m_last_event = event;
}

void Application::update() {
    this->m_needs_update = true;
}

void Application::layout() {
    // TODO i think it would make sense if layout
    // also called update
    this->m_layout_changed = true;
}

bool Application::hasLayoutChanged() {
    return this->m_layout_changed;
}

void Application::removeFromState(void *widget) {
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
        this->update();
    }
}

// TODO implement left/right specific modifiers any the other remaining ones
int Application::bind(int key, int modifiers, std::function<void()> callback) {
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

int Application::bind(int key, Mod modifier, std::function<void()> callback) {
    return bind(key, (int)modifier, callback);
}

void Application::unbind(int key) {
    this->m_keyboard_shortcuts.erase(key);
}

void Application::quit() {
    if (this->onQuit) {
        if (!this->onQuit(this)) {
            return;
        }
    }
    m_running = false;
}

void Application::handleResizeEvent(int width, int height) {
    this->m_size = Size(width, height);
    int w, h;
    SDL_GL_GetDrawableSize(this->m_win, &w, &h);
    glViewport(0, 0, w, h);
    this->m_needs_update = true;
    this->m_layout_changed = true;
    this->show();
}

void Application::resize(int width, int height) {
    SDL_SetWindowSize(this->m_win, width, height);
    handleResizeEvent(width, height);
}

void Application::setMouseCursor(Cursor cursor) {
    switch (cursor) {
        case Cursor::Arrow: SDL_SetCursor(m_cursors->arrow); break;
        case Cursor::IBeam: SDL_SetCursor(m_cursors->i_beam); break;
        case Cursor::Wait: SDL_SetCursor(m_cursors->wait); break;
        case Cursor::Crosshair: SDL_SetCursor(m_cursors->crosshair); break;
        case Cursor::WaitArrow: SDL_SetCursor(m_cursors->wait_arrow); break;
        case Cursor::SizeNWSE: SDL_SetCursor(m_cursors->size_nwse); break;
        case Cursor::SizeNESW: SDL_SetCursor(m_cursors->size_nesw); break;
        case Cursor::SizeWE: SDL_SetCursor(m_cursors->size_we); break;
        case Cursor::SizeNS: SDL_SetCursor(m_cursors->size_ns); break;
        case Cursor::SizeAll: SDL_SetCursor(m_cursors->size_all); break;
        case Cursor::No: SDL_SetCursor(m_cursors->no); break;
        case Cursor::Hand: SDL_SetCursor(m_cursors->hand); break;
        default: SDL_SetCursor(SDL_GetDefaultCursor()); break;
    }
}