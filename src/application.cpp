#include "application.hpp"
#include "drawable.hpp"

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

Window* findEventWindow(std::vector<Window*> &windows, u32 id) {
    for (Window *window : windows) {
        if (SDL_GetWindowID(window->m_win) == id) {
            return window;
        }
    }
    return nullptr;
}

// This is needed on Windows and on MacOS to
// redraw the window while the user is resizing it.
i32 forcePaintWhileResizing(void *data, SDL_Event *event) {
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
    SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "1");
    SDL_SetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE , "1");
    SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
    SDL_SetEventFilter(forcePaintWhileResizing, this);
    Window *win = new Window(title, size, Point(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
    icons.insert({
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
    });
    m_cursors = new Cursors();
    current_window = win;
    m_main_window = win;
    m_windows.push_back(win);
    win->dc->default_style.font = std::shared_ptr<Font>(new Font(this, DejaVuSans_ttf, DejaVuSans_ttf_length, 14, Font::Type::Sans));
    win->dc->default_light_style.font = win->dc->default_style.font;
    win->dc->default_dark_style.font = win->dc->default_style.font;
    win->is_owned = true;

    // TODO these are not global anymore hmm...
    mainWindow()->bind(SDLK_EQUALS, Mod::Ctrl, [&]() {
        if (scale == 500) { return; }
        scale += 25;
        freeze = true;
        for (Window *win : m_windows) {
            win->dc->default_style.font = std::shared_ptr<Font>(win->dc->default_style.font->reload((i64)(default_scale_font_size * (scale / 100.0))));
            win->layout(LAYOUT_SCALE);
        }
    });
    mainWindow()->bind(SDLK_MINUS, Mod::Ctrl, [&]() {
        if (scale == 50) { return; }
        scale -= 25;
        freeze = true;
        for (Window *win : m_windows) {
            win->dc->default_style.font = std::shared_ptr<Font>(win->dc->default_style.font->reload((i64)(default_scale_font_size * (scale / 100.0))));
            win->layout(LAYOUT_SCALE);
        }
    });
}

Application::~Application() {
    for (Timer t : m_timers) {
        SDL_RemoveTimer(t.id);
    }
    for (Window *win : m_windows) {
        // This is to prevent deleting Windows which are owned by something else
        // like dropdown list and in the future tooltips etc.
        // This includes the mainWindow that is part of Application due to the
        // design changes. However we still want to free it, just at the very end.
        if (!win->is_owned) { delete win; }
    }
    delete m_main_window;
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
    // TODO Im not a fan of this solution but it does
    // prevent the UI from jumping on the initial load
    // if the font size changes while calculating sizehint.
    // however it does also cause extra work that isnt stricly necessary.
    for (Window *win : m_windows) {
        win->show();
        win->layout(LAYOUT_FONT);
        win->show();
    }
    SDL_ShowWindow(mainWindow()->m_win);
    if (mainWindow()->onReady) {
        mainWindow()->onReady(mainWindow());
    }
    // TODO not sure why we have this here, if anything we should handle the nullptr
    // since its not guaranteed that the main widget will be hovered
    mainWindow()->m_state->hovered = mainWindow()->m_main_widget;
    u32 fps = 60; // TODO we should query the refresh rate of the monitor the window is on, also allow for overrides
    u32 frame_time = 1000 / fps;
    while (mainWindow()->m_running) {
        DELAY:;
        u32 frame_start;
        SDL_Event event;
        i32 status;
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
                case SDL_USEREVENT:
                    switch (event.user.code) {
                        case LAYOUT_FONT:
                            for (Window *window : m_windows) {
                                window->layout(LAYOUT_FONT);
                                window->show();
                            }
                            break;
                        case LAYOUT_SCALE:
                            for (Window *window : m_windows) {
                                window->update();
                            }
                            break;
                        default:; // Right now the only other event is LAYOUT_NONE which we don't need to explicitly handle.
                    }
                    break;
                case SDL_QUIT:
                    mainWindow()->quit();
                    break;
            }
        }
        // TODO surely this should be application specific rather than window
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
        if (freeze) {
            freeze = false;
            Application::pulse(LAYOUT_SCALE);
        }
        u32 frame_end = SDL_GetTicks() - frame_start;
        if (frame_time > frame_end) {
            mainWindow()->delay_till = SDL_GetTicks() + (frame_time - frame_end);
        } else {
            mainWindow()->delay_till = 0;
        }
    }

    delete this;
}

void Application::setCurrentWindow(Window *window) {
    assert(window && "Tried to set current window to null!");
    current_window = window;
    SDL_GL_MakeCurrent(window->m_win, window->m_sdl_context);
}

Window* Application::currentWindow() {
    assert(current_window && "Current window found null!");
    return current_window;
}

Window* Application::mainWindow() {
    return m_main_window;
}

Timer Application::addTimer(u32 after, std::function<u32(u32 interval)> callback) {
    Timer t = Timer(after, callback);
    m_timers.push_back(t);
    return t;
}

void Application::removeTimer(Timer timer) {
    SDL_RemoveTimer(timer.id);
    for (u64 i = 0; i < m_timers.size(); i++) {
        const Timer &t = m_timers[i];
        if (t.id == timer.id) {
            m_timers.erase(m_timers.begin() + i);
            break;
        }
    }
}

void Application::pulse(LayoutEvent layout_event) {
    SDL_Event event;
    SDL_UserEvent userevent;
    userevent.type = SDL_USEREVENT;
    userevent.code = layout_event;
    userevent.data1 = NULL;
    userevent.data2 = NULL;
    event.type = SDL_USEREVENT;
    event.user = userevent;
    SDL_PushEvent(&event);
}
