#ifndef WINDWOW_HPP
    #define WINDWOW_HPP

    #include <string>
    #include <vector>
    #include <utility>
    #include <functional>
    #include <unordered_map>

    #include <SDL2/SDL.h>

    #include "event.hpp"
    #include "state.hpp"
    #include "keyboard.hpp"
    #include "controls/widget.hpp"
    #include "controls/scrolledbox.hpp"

    class Window {
        public:
            Size size;
            Point position;
            std::string title; // TODO getter setter (no update!)

            /// The dc is used to draw various primitives.
            /// It can also draw as well as measure text.
            /// It is passed to the draw() and sizeHint() methods of each Widget.
            /// This pointer should **NEVER** be null as it gets assigned in the
            /// constructor of the Application.
            DrawingContext *dc = nullptr;

            /// `onReady` gets called when the Application has finished its first draw()
            /// but before entering the event loop.
            std::function<void(Window*)> onReady = nullptr;

            /// `onQuit` gets called when the Window about to exit but
            /// before anything is freed.
            std::function<bool(Window*)> onQuit = nullptr;

            std::function<void(Window*)> onResize = nullptr;

            Window(const char* title, Size size);
            ~Window();

            /// This method is used to add a Widget to the children
            /// of the Widget in question. It adds the Widget to the
            /// end of the children dynamic array. It is exactly
            /// the same as the append() method in Widget and is
            /// provided for conveniance.
            /// It simply calls the append() method on the main Widget.
            Widget* append(Widget *widget, Fill fill_policy = Fill::None, unsigned int proportion = 1);

            /// Returns the pointer to the main Widget of the Application.
            Widget* mainWidget();

            /// Sets the main Widget pointer.
            void setMainWidget(Widget *widget);

            /// Tells the Application to update which causes a redraw.
            void update();

            /// Starts the Application, calls `onReady` and enters the event loop.
            void run();

            /// Redraws the Application and swaps the front buffer.
            void show();

            void removeFromState(void *widget);

            int bind(int key, int modifiers, std::function<void()> callback);
            int bind(int key, Mod modifier, std::function<void()> callback);
            void unbind(int map_key);

            void quit();

            void handleResizeEvent(int width, int height);
            void resize(int width, int height);

        protected:
            SDL_Window *m_win = nullptr;
            SDL_GLContext m_sdl_context = nullptr;
            Widget *m_main_widget = new ScrolledBox(Align::Vertical);
            State *m_state = new State();
            bool m_needs_update = false;
            // TODO have another map for hotkeys ie menu shortcut keys
            std::unordered_map<int, KeyboardShortcut> m_keyboard_shortcuts;
            int m_binding_id = 0; // This is used to give out the next id to a binding. // TODO look in notes for a better solution
            bool m_running = true;
            bool m_mouse_inside = true;
            bool m_is_mouse_captured = false;

            /// Updates the projection matrix, clears the context and
            /// renders any state that was stored in the renderer from
            /// calls to the DrawingContext.
            /// Used internally by show().
            void draw();
    };
#endif