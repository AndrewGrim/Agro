#ifndef APPLICATION_HPP
    #define APPLICATION_HPP

    #include "window.hpp"

    static int init = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    enum class Cursor {
        Arrow,
        IBeam,
        Wait,
        Crosshair,
        WaitArrow,
        SizeNWSE,
        SizeNESW,
        SizeWE,
        SizeNS,
        SizeAll,
        No,
        Hand,
        Default
    };

    struct Cursors {
        SDL_Cursor *arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
        SDL_Cursor *i_beam = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
        SDL_Cursor *wait = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
        SDL_Cursor *crosshair = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
        SDL_Cursor *wait_arrow = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
        SDL_Cursor *size_nwse = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
        SDL_Cursor *size_nesw = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
        SDL_Cursor *size_we = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
        SDL_Cursor *size_ns = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
        SDL_Cursor *size_all = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
        SDL_Cursor *no = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
        SDL_Cursor *hand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

        Cursors();
        ~Cursors();
    };
    
    class Application : public Window {
        public:
            int scroll_amount = 50;
            void setMouseCursor(Cursor cursor);
            static Application* get();

        private:
            Application(const char *title, Size size);
            ~Application();
            std::vector<SDL_Window*> m_windows;
            Cursors m_cursors;
    };
#endif