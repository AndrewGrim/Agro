#include "application.hpp"

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

Window* findEventWindow(std::vector<Window*> &windows, Uint32 id) {
    for (Window *window : windows) {
        if (SDL_GetWindowID(window->m_win) == id) {
            return window;
        }
    }
    // TODO if we delete a window a mouseup event will still get sent
    // and we will hit the assert here
    assert(false && "Couldnt find the window for which the event was meant for, unreachable!");
}

// This is needed on Windows and on MacOS to
// redraw the window while the user is resizing it.
int forcePaintWhileResizing(void *data, SDL_Event *event) {
    switch (event->type) {
        case SDL_WINDOWEVENT:
            // We are using the below instead of `SDL_WINDOWEVENT_RESIZED` because this one
            // supposedly also triggers when the system changes the window size.
            if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event->window.event == SDL_WINDOWEVENT_RESIZED) {
                Application *app = (Application*)data;
                Window *win = findEventWindow(app->m_windows, event->window.windowID);
                win->handleResizeEvent(event->window.data1, event->window.data2);
                // TODO this should go into handleResizeEvent most likely
                if (win->onResize) {
                    win->onResize(win);
                }
                return 0;
            }
            break;
    }
    return 1;
}

Application::Application(const char *title, Size size) : Window(title, size) {
    assert(init == 0 && "Failed initializing SDL video!");
    current_window = this;
    SDL_SetEventFilter(forcePaintWhileResizing, this);
}

Application::~Application() {
    for (size_t i = 0; i < m_windows.size(); i++) {
        // This condition is to prevent the Application from deleting
        // itself again since it is also a Window.
        if (i) {
            Window *window = m_windows[i];
            delete window;
        }
    }
    SDL_Quit();
}

void Application::setMouseCursor(Cursor cursor) {
    switch (cursor) {
        case Cursor::Arrow: SDL_SetCursor(m_cursors.arrow); break;
        case Cursor::IBeam: SDL_SetCursor(m_cursors.i_beam); break;
        case Cursor::Wait: SDL_SetCursor(m_cursors.wait); break;
        case Cursor::Crosshair: SDL_SetCursor(m_cursors.crosshair); break;
        case Cursor::WaitArrow: SDL_SetCursor(m_cursors.wait_arrow); break;
        case Cursor::SizeNWSE: SDL_SetCursor(m_cursors.size_nwse); break;
        case Cursor::SizeNESW: SDL_SetCursor(m_cursors.size_nesw); break;
        case Cursor::SizeWE: SDL_SetCursor(m_cursors.size_we); break;
        case Cursor::SizeNS: SDL_SetCursor(m_cursors.size_ns); break;
        case Cursor::SizeAll: SDL_SetCursor(m_cursors.size_all); break;
        case Cursor::No: SDL_SetCursor(m_cursors.no); break;
        case Cursor::Hand: SDL_SetCursor(m_cursors.hand); break;
        default: SDL_SetCursor(SDL_GetDefaultCursor()); break;
    }
}

Application* Application::get() {
    static Application *app = new Application("Application", Size(400, 400));
    return app;
}

void Application::run() {
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
        uint32_t frame_start;
        SDL_Event event;
        int status;
        if ((status = SDL_WaitEventTimeout(&event, frame_time))) {
            frame_start = SDL_GetTicks();
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN: findEventWindow(m_windows, event.button.windowID)->handleSDLEvent(event); break;
                case SDL_MOUSEBUTTONUP: findEventWindow(m_windows, event.button.windowID)->handleSDLEvent(event); break;
                case SDL_MOUSEMOTION: findEventWindow(m_windows, event.motion.windowID)->handleSDLEvent(event); break;
                case SDL_MOUSEWHEEL: findEventWindow(m_windows, event.wheel.windowID)->handleSDLEvent(event); break;
                case SDL_WINDOWEVENT: findEventWindow(m_windows, event.window.windowID)->handleSDLEvent(event); break;
                case SDL_KEYDOWN: findEventWindow(m_windows, event.key.windowID)->handleSDLEvent(event); break;
                case SDL_TEXTINPUT: findEventWindow(m_windows, event.text.windowID)->handleSDLEvent(event); break;
                case SDL_QUIT:
                    quit();
            }
        }
        if (!status) { frame_start = SDL_GetTicks(); }
        if (delay_till) {
            if (SDL_GetTicks() < delay_till) {
                goto DELAY;
            } else {
                delay_till = 0;
            }
        }
        if (m_needs_update) {
            // TODO temporary to be removed
            for (Window *window : m_windows) {
                window->show();
            }
            // show();
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

void Application::setCurrentWindow(Window *window) {
    current_window = window;
    SDL_GL_MakeCurrent(window->m_win, window->m_sdl_context);
}

Window* Application::getCurrentWindow() {
    assert(current_window && "Current window found null!");
    return current_window;
}
