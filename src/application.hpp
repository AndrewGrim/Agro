#ifndef APPLICATION_HPP
    #define APPLICATION_HPP

    #include "window.hpp"
    #include "resources.hpp"
    #include "timer.hpp"

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

    struct Application {
        i32 scroll_amount = 50;

        void setMouseCursor(Cursor cursor);
        static Application* get();

        /// Starts the Application, calls `onReady` and enters the event loop.
        void run();

        void setCurrentWindow(Window *window);
        Window* currentWindow();
        Window* mainWindow();
        Timer addTimer(u32 after, std::function<u32(u32 interval)> callback);
        void removeTimer(Timer timer);

        Application(const char *title, Size size);
        ~Application();

        Window *current_window = nullptr;
        std::vector<Window*> m_windows;
        std::vector<Timer> m_timers;
        Cursors *m_cursors = nullptr;
        FT_Library ft = NULL;

        std::unordered_map<const char*, std::shared_ptr<Texture>> icons;
    };
#endif
