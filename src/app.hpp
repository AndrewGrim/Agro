#ifndef APP_HPP
    #define APP_HPP

    #include <iostream>
    #include <chrono>
    #include <utility>

    #include <glad/glad.h>
    #include <SDL2/SDL.h>

    #include "event.hpp"
    #include "state.hpp"
    #include "controls/widget.hpp"
    // #include "controls/box.hpp"
    // #include "controls/scrolledbox.hpp"
    #include "renderer/drawing_context.hpp"
    
    class Application {
        public:
            enum class Event {
                None,
                MouseUp,
                MouseDown,
                MouseClick,
                MouseLeft,
                MouseEntered,
                MouseMotion,
                Scroll,
            };

            enum class EventHandler {
                Ignored,
                Accepted,
            };

            int id_counter = 0;
            SDL_Window *win;
            SDL_GLContext sdl_context;
            DrawingContext *dc;
            Color bg = Color(1.0f, 1.0f, 1.0f);
            // Widget *main_widget = new ScrolledBox(Align::Vertical);
            Widget *main_widget = new Widget();
            Size m_size;
            State *state = new State();
            bool m_needs_update = false;
            bool m_layout_changed = true;
            std::pair<Event, EventHandler> m_last_event = std::make_pair<Event, EventHandler>(Event::None, EventHandler::Accepted);
            std::chrono::time_point<std::chrono::steady_clock> m_last_event_time = std::chrono::steady_clock::now();
            void (*ready_callback)(Application*) = nullptr;

            Application(const char* title = "Application", Size size = Size(400, 400));
            ~Application();
            void draw();
            void set_main_widget(Widget *widget);
            Widget* append(Widget *widget, Fill fill_policy);
            void show();
            int next_id();
            void run();
    };
#endif