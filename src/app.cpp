#include <iostream>

#include <glad/glad.h>

#include "app.hpp"

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
    // TODO dont capture the mouse all the time only when pressed
    // and capture long enough to reset hover when exiting window
    // NOTE: Doesnt seem to work on X11
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
}

Application::~Application() {
    delete m_main_widget;
    delete m_state;
    delete dc;
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
                        case SDL_WINDOWEVENT_RESIZED:
                            // TODO test SDL_WINDOWEVENT_SIZE_CHANGED instead, to see if on windows the window refreshes while being resized
                            // also the above seems to be cover cases where the window is resized by the system better
                            this->m_size = Size(event.window.data1, event.window.data2);
                            int w, h;
                            SDL_GL_GetDrawableSize(this->m_win, &w, &h);
                            glViewport(0, 0, w, h);
                            this->m_needs_update = true;
                            this->m_layout_changed = true;
                            break;
                    }
                    break;
                case SDL_KEYDOWN: {
                        SDL_Keycode key = event.key.keysym.sym;
                        Uint16 mod = event.key.keysym.mod;
                        bool matched = false;
                        // TODO add matching for hotkeys and the widget itself
                        for (auto hotkey : m_keyboard_shortcuts) {
                            if (hotkey.key.code == key && hotkey.key.mod & mod) {
                                hotkey.callback();
                                SDL_FlushEvent(SDL_TEXTINPUT);
                                matched = true;
                            }
                        }
                        // if (mod & KMOD_CTRL) {
                        //     println("ANY CONTROL");
                        // }
                        // if (mod & KMOD_LCTRL) {
                        //     println("LEFT CONTROL");
                        // } else if (mod & KMOD_RCTRL) {
                        //     println("RIGHT CONTROL");
                        // }
                        // if (mod & KMOD_SHIFT) {
                        //     println("ANY SHIFT");
                        // }
                        // if (mod & KMOD_LSHIFT) {
                        //     println("LEFT SHIFT");
                        // } else if (mod & KMOD_RSHIFT) {
                        //     println("RIGHT SHIFT");
                        // }
                        // if (mod & KMOD_ALT) {
                        //     println("ANY ALT");
                        // }
                        // if (mod & KMOD_LALT) {
                        //     println("LEFT ALT");
                        // } else if (mod & KMOD_RALT) {
                        //     println("RIGHT ALT");
                        // }
                        // if (mod & KMOD_GUI) {
                        //     println("ANY GUI");
                        // }
                        // if (mod & KMOD_LGUI) {
                        //     println("LEFT GUI");
                        // } else if (mod & KMOD_RGUI) {
                        //     println("RIGHT GUI");
                        // }
                        // if (mod & KMOD_NUM) {
                        //     println("NUM");
                        // }
                        // if (mod & KMOD_CAPS) {
                        //     println("CAPS");
                        // }
                        // if (mod & KMOD_MODE) {
                        //     println("MODE");
                        // }
                        // if (key == SDLK_q && (mod == KMOD_LCTRL + KMOD_LSHIFT)) {
                        //     println("RUNS!");
                        //     // TODO should add some quit() function
                        //     goto EXIT;
                        // }
                    }
                    break;
                case SDL_TEXTINPUT:
                    // println("TEXT");
                    // println(event.text.text);
                    // TODO widget is going to need a onTextInput callback handler
                    // so now is probably a good time to change it so that each event
                    // has a default handler which can optionally call callbacks??
                    // does that even make sense?
                    if (m_state->focused) {
                        // TODO lineedit probably needs to have a hardcoded sizehint to some extent
                        // we dont want the text size to force it to be wider
                        // we also need to be able to scroll it somehow 
                        // ((LineEdit*)m_state->focused)->setText(((LineEdit*)m_state->focused)->text() += event.text.text);
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

Widget* Application::append(Widget* widget, Fill fill_policy, uint proportion) {
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

size_t Application::bind(Key key, std::function<void()> callback) {
    this->m_keyboard_shortcuts.push_back(KeyboardShortcut(key, callback));
    return this->m_keyboard_shortcuts.size() - 1;
}

// TODO this would break because we dont update the indices
// use a hashmap or pointers instead
void Application::unbind(size_t index) {
    this->m_keyboard_shortcuts.erase(this->m_keyboard_shortcuts.cbegin() + index);
}

void Application::quit() {
    if (this->onQuit) {
        if (!this->onQuit(this)) {
            return;
        }
    }
    m_running = false;
}