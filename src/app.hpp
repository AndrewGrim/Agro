#ifndef APP_HPP
    #define APP_HPP

    #include <iostream>

    #include <glad/glad.h>
    #include <SDL2/SDL.h>

    #include "event.hpp"
    #include "state.hpp"
    #include "controls/widget.hpp"
    #include "controls/box.hpp"
    #include "renderer/drawing_context.hpp"
    #include "renderer/text_renderer.h"
    
    class Application {
        public:
            int id_counter = 0;
            SDL_Window *win;
            SDL_GLContext sdl_context;
            DrawingContext *dc;
            Color bg = Color(1.0f, 1.0f, 1.0f);
            Widget *main_widget = new Box(Align::Vertical);
            Size<int> m_size;
            State *state = new State();
            bool m_needs_update = false;
            void (*ready_callback)(Application*) = nullptr;

            Application(const char* title = "Application", Size<int> size = Size<int>(400, 400));
            ~Application();
            void draw();
            void set_main_widget(Widget *widget);
            Widget* append(Widget *widget, Fill fill_policy);
            void show();
            int next_id();
            void run();
    };
#endif