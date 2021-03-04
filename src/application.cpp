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

Application& Application::get() {
    static Application app = Application("Application", Size(400, 400));
    return app;
}

Application::Application(const char *title, Size size) : Window(title, size) {
    assert(init == 0 && "Failed initializing SDL video!");
}

Application::~Application() {
    SDL_Quit();
}