#ifndef APPLICATION_HPP
    #define APPLICATION_HPP

    #include "window.hpp"
    #include "resources.hpp"

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
            std::unordered_map<std::string, std::shared_ptr<Texture>> icons = {
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

        private:
            Application(const char *title, Size size);
            ~Application();
            std::vector<SDL_Window*> m_windows;
            Cursors m_cursors;
    };
#endif
