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
    return nullptr;
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
                return 0;
            }
            break;
    }
    return 1;
}

Application::Application(const char *title, Size size) {
    if (FT_Init_FreeType(&ft)) {
        fail("FAILED_TO_INITIALISE_FREETYPE");
    }
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
        fail("FAILED_TO_INITIALIZE_SDL");
    }
    SDL_SetEventFilter(forcePaintWhileResizing, this);
    Window *win = new Window(title, size, Point());
    icons = {
        {"close", std::make_shared<Texture>(close_png, close_png_length)},
        {"close_thin", std::make_shared<Texture>(close_thin_png, close_thin_png_length)},
        {"up_arrow", std::make_shared<Texture>(up_arrow_png, up_arrow_png_length)},
        {"tree_layout", std::make_shared<Texture>(tree_layout_png, tree_layout_png_length)},
        {"table_layout", std::make_shared<Texture>(table_layout_png, table_layout_png_length)},
        {"check_button_unchecked", std::make_shared<Texture>(check_button_unchecked_png, check_button_unchecked_png_length)},
        {"check_button_checked", std::make_shared<Texture>(check_button_checked_png, check_button_checked_png_length)},
        {"radio_button_unchecked", std::make_shared<Texture>(radio_button_unchecked_png, radio_button_unchecked_png_length)},
        {"radio_button_checked", std::make_shared<Texture>(radio_button_checked_png, radio_button_checked_png_length)},
        {"radio_button_background", std::make_shared<Texture>(radio_button_background_png, radio_button_background_png_length)},
        {"color_picker_gradient", std::make_shared<Texture>(color_picker_gradient_png, color_picker_gradient_png_length)},
    };
    m_cursors = new Cursors();
    current_window = win;
    m_windows.push_back(win);
    win->dc->default_font = new Font(ft, DejaVuSans_ttf, DejaVuSans_ttf_length, 14, Font::Type::Sans);
    win->is_owned = true;
}

Application::~Application() {
    for (Timer t : m_timers) {
        SDL_RemoveTimer(t.id);
    }
    for (Window *win : m_windows) {
        // This is to prevent deleting Windows which are owned by something else
        // like dropdown list and in the future tooltips etc.
        // This includes the mainWindow that is part of Application due to the
        // design changes. However we still want to free it just at the very end.
        if (!win->is_owned) { delete win; }
    }
    delete mainWindow();
    delete m_cursors;
    SDL_Quit();
    FT_Done_FreeType(ft);
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

Application* Application::get() {
    static Application *app = new Application("Application", Size(400, 400));
    return app;
}

void Application::run() {
    mainWindow()->show();
    if (mainWindow()->onReady) {
        mainWindow()->onReady(mainWindow());
    }
    // TODO not sure why we have this here, if anything we should handle the nullptr
    // since its not guaranteed that the main widget will be hovered
    mainWindow()->m_state->hovered = mainWindow()->m_main_widget;
    uint32_t fps = 60; // TODO we should query the refresh rate of the monitor the window is on, also allow for overrides
    uint32_t frame_time = 1000 / fps;
    SDL_StartTextInput();
    while (mainWindow()->m_running) {
        DELAY:;
        uint32_t frame_start;
        SDL_Event event;
        int status;
        if ((status = SDL_WaitEventTimeout(&event, frame_time))) {
            frame_start = SDL_GetTicks();
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN: {
                    Window *event_window = findEventWindow(m_windows, event.button.windowID);
                    if (event_window) { event_window->handleSDLEvent(event); }
                    else { info("Couldn't find Window for event, 'SDL_MOUSEBUTTONDOWN' perhaps it was deleted?"); }
                    break;
                }
                case SDL_MOUSEBUTTONUP: {
                    Window *event_window = findEventWindow(m_windows, event.button.windowID);
                    if (event_window) { event_window->handleSDLEvent(event); }
                    else { info("Couldn't find Window for event 'SDL_MOUSEBUTTONUP', perhaps it was deleted?"); }
                    break;
                }
                case SDL_MOUSEMOTION: {
                    Window *event_window = findEventWindow(m_windows, event.motion.windowID);
                    if (event_window) { event_window->handleSDLEvent(event); }
                    else { info("Couldn't find Window for event 'SDL_MOUSEMOTION', perhaps it was deleted?"); }
                    break;
                }
                case SDL_MOUSEWHEEL: {
                    Window *event_window = findEventWindow(m_windows, event.wheel.windowID);
                    if (event_window) { event_window->handleSDLEvent(event); }
                    else { info("Couldn't find Window for event 'SDL_MOUSEWHEEL', perhaps it was deleted?"); }
                    break;
                }
                case SDL_WINDOWEVENT: {
                    Window *event_window = findEventWindow(m_windows, event.window.windowID);
                    if (event_window) { event_window->handleSDLEvent(event); }
                    else { info("Couldn't find Window for event 'SDL_WINDOWEVENT', perhaps it was deleted?"); }
                    break;
                }
                case SDL_KEYDOWN: {
                    Window *event_window = findEventWindow(m_windows, event.key.windowID);
                    if (event_window) { event_window->handleSDLEvent(event); }
                    else { info("Couldn't find Window for event 'SDL_KEYDOWN', perhaps it was deleted?"); }
                    break;
                }
                case SDL_TEXTINPUT: {
                    Window *event_window = findEventWindow(m_windows, event.text.windowID);
                    if (event_window) { event_window->handleSDLEvent(event); }
                    else { info("Couldn't find Window for event 'SDL_TEXTINPUT', perhaps it was deleted?"); }
                    break;
                }
                case SDL_QUIT:
                    mainWindow()->quit();
                    break;
            }
        }
        if (!status) { frame_start = SDL_GetTicks(); }
        if (mainWindow()->delay_till) {
            if (SDL_GetTicks() < mainWindow()->delay_till) {
                goto DELAY;
            } else {
                mainWindow()->delay_till = 0;
            }
        }
        for (Window *window : m_windows) {
            if (window->m_needs_update) {
                window->show();
                window->m_needs_update = false;
            }
        }
        uint32_t frame_end = SDL_GetTicks() - frame_start;
        if (frame_time > frame_end) {
            mainWindow()->delay_till = SDL_GetTicks() + (frame_time - frame_end);
        } else {
            mainWindow()->delay_till = 0;
        }
    }
    SDL_StopTextInput();

    delete this;
}

void Application::setCurrentWindow(Window *window) {
    current_window = window;
    SDL_GL_MakeCurrent(window->m_win, window->m_sdl_context);
}

Window* Application::currentWindow() {
    assert(current_window && "Current window found null!");
    return current_window;
}

Window* Application::mainWindow() {
    return m_windows[0];
}

Timer Application::addTimer(uint32_t after, std::function<uint32_t(uint32_t interval)> callback) {
    Timer t = Timer(after, callback);
    m_timers.push_back(t);
    t.index = m_timers.size() - 1;
    return t;
}

void Application::removeTimer(Timer timer) {
    SDL_RemoveTimer(timer.id);
    m_timers.erase(m_timers.begin() + timer.index);
}
