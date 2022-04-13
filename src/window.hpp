#ifndef WINDOW_HPP
    #define WINDOW_HPP

    #include <string>
    #include <vector>
    #include <utility>
    #include <functional>
    #include <unordered_map>

    #include <SDL.h>

    #include "event.hpp"
    #include "state.hpp"
    #include "keyboard.hpp"
    #include "controls/widget.hpp"
    #include "controls/scrolled_box.hpp"

    class Window {
        public:
            enum class ContextEvent {
                True,
                False
            };

            struct ContextEventResult {
                ContextEvent event;
                Widget *widget;
            };

            Size size;
            Point position;

            /// The dc is used to draw various primitives.
            /// It can also draw as well as measure text.
            /// It is passed to the draw() and sizeHint() methods of each Widget.
            /// This pointer should **NEVER** be null as it gets assigned in the
            /// constructor of the Application.
            DrawingContext *dc = nullptr;

            bool draw_tooltip = false;
            bool tooltip_did_draw = false;

            bool is_owned = false;

            Widget *active_context_menu = nullptr;
            Point context_menu_position_start;

            /// `onReady` gets called when the Application has finished its first draw()
            /// but before entering the event loop.
            std::function<void(Window*)> onReady = nullptr;

            /// `onQuit` gets called when the Window about to exit but
            /// before anything is freed.
            std::function<bool(Window*)> onQuit = nullptr;

            std::function<void(Window*)> onResize = nullptr;
            std::function<void(Window*)> onFocusLost = nullptr;

            Window(const char* title, Size size, Point point, i32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
            virtual ~Window();

            /// This method is used to add a Widget to the children
            /// of the Widget in question. It adds the Widget to the
            /// end of the children dynamic array. It is exactly
            /// the same as the append() method in Widget and is
            /// provided for conveniance.
            /// It simply calls the append() method on the main Widget.
            Widget* append(Widget *widget, Fill fill_policy = Fill::None, u32 proportion = 1);

            /// Returns the pointer to the main Widget of the Application.
            Widget* mainWidget();

            /// Sets the main Widget pointer.
            void setMainWidget(Widget *widget);

            /// Tells the Application to update which causes a redraw.
            void update();

            void run();

            void handleSDLEvent(SDL_Event &event);

            /// Redraws the Application and swaps the front buffer.
            void show();

            void removeFromState(Widget *widget);

            i32 bind(i32 key, i32 modifiers, std::function<void()> callback);
            i32 bind(i32 key, Mod modifier, std::function<void()> callback);
            void unbind(i32 map_key);

            void quit();

            void handleResizeEvent(i32 width, i32 height);
            void resize(i32 width, i32 height);

            std::string title();
            void setTitle(std::string title);

            void setTooltip(Widget *widget);

            void move(i32 x, i32 y);
            void center();
            void pulse();
            ContextEventResult propagateMouseEvent(MouseEvent event);
            void propagateFocusEvent(FocusEvent event, Widget *widget);

            void layout();

            SDL_Window *m_win = nullptr;
            SDL_GLContext m_sdl_context = nullptr;
            Widget *m_main_widget = new ScrolledBox(Align::Vertical);
            State *m_state = new State();
            bool m_needs_update = false;
            // TODO have another map for hotkeys ie menu shortcut keys
            std::unordered_map<i32, KeyboardShortcut> m_keyboard_shortcuts;
            i32 m_binding_id = 0; // This is used to give out the next id to a binding. // TODO look in notes for a better solution
            bool m_running = true;
            bool m_mouse_inside = true;
            bool m_is_mouse_captured = false;
            std::string m_title;

            u32 m_tooltip_time = 500;
            SDL_TimerID m_tooltip_callback = -1;
            u32 delay_till = 0;

            /// Updates the projection matrix, clears the context and
            /// renders any state that was stored in the renderer from
            /// calls to the DrawingContext.
            /// Used internally by show().
            void draw();
            void drawTooltip();
            void matchKeybind(bool &matched, Mod mods[4], SDL_Keycode key, std::unordered_map<i32, KeyboardShortcut> keybinds);
    };
#endif
