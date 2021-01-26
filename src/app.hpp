#ifndef APP_HPP
    #define APP_HPP

    #include <chrono>
    #include <utility>
    #include <functional>
    #include <unordered_map>
    
    #include <SDL2/SDL.h>

    #include "event.hpp"
    #include "state.hpp"
    #include "keyboard.hpp"
    #include "common/color.hpp"
    #include "common/size.hpp"
    #include "common/align.hpp"
    #include "controls/widget.hpp"
    #include "controls/box.hpp"
    #include "controls/scrolledbox.hpp"
    #include "renderer/drawing_context.hpp"

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
    
    class Application {
        public:
            // TODO this might be better off in event.hpp
            enum class Event {
                None,
                MouseUp,
                MouseDown,
                MouseClick,
                MouseLeft,
                MouseEntered,
                MouseMotion,
                Scroll, // TODO implement scroll events from mouse
            };

            // TODO this might be better off in event.hpp
            enum class EventHandler {
                Ignored,
                Accepted,
            };

            /// The dc is used to draw various primitives.
            /// It can also draw as well as measure text.
            /// It is passed to the draw() and sizeHint() methods of each Widget.
            /// This pointer should **NEVER** be null as it gets assigned in the
            /// constructor of the Application.
            DrawingContext *dc = nullptr;

            Size m_size; // TODO get rid of this, change to rect?

            /// `onReady` gets called when the Application has finished its first draw()
            /// but before entering the event loop.
            std::function<void(Application*)> onReady = nullptr;

            /// `onQuit` gets called when the Application is about to exit but
            /// before anything is freed.
            std::function<bool(Application*)> onQuit = nullptr;

            Application(const char* title = "Application", Size size = Size(400, 400));

            ~Application();

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

            /// Returns the Event, EventHandler pair of the last event.
            /// Event will be one of the possible events such as MouseDown.
            /// EventHanlder will be either Accepted or Ignored, which
            /// tells you if the last event was handled by the widget it was sent to.
            // TODO actually thats a lie because if there is no callback that doesnt happend?
            // and this is not true because slider sets the last event directly
            // the last event should probably be returned by the callback?
            // but i dont really want to user to have to deal with that
            std::pair<Event, EventHandler> lastEvent();

            /// Sets the last event.
            // TODO add more docs
            void setLastEvent(std::pair<Event, EventHandler> event);

            /// Tells the Application to update which causes a redraw.
            void update();

            /// Tells the Application to recompute its layout.
            void layout();

            /// Returns whether the Application layout has been invalidated.
            bool hasLayoutChanged();

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

            void setMouseCursor(Cursor cursor);

        private:
            SDL_Window *m_win = nullptr;
            SDL_GLContext m_sdl_context = nullptr;
            Widget *m_main_widget = new ScrolledBox(Align::Vertical);
            State *m_state = new State();
            bool m_needs_update = false;
            bool m_layout_changed = true;
            std::pair<Event, EventHandler> m_last_event = std::make_pair<Event, EventHandler>(Event::None, EventHandler::Accepted);
            std::chrono::time_point<std::chrono::steady_clock> m_last_event_time = std::chrono::steady_clock::now();
            // TODO have another map for hotkeys ie menu shortcut keys
            std::unordered_map<int, KeyboardShortcut> m_keyboard_shortcuts;
            int m_binding_id = 0;
            bool m_running = true;
            Cursors *m_cursors = nullptr;

            /// Updates the projection matrix, clears the context and
            /// renders any state that was stored in the renderer from
            /// calls to the DrawingContext.
            /// Used internally by show().
            void draw();
    };
#endif