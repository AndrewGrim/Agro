#ifndef APP_HPP
    #define APP_HPP

    #include <chrono>
    #include <utility>
    #include <functional>
    
    #include <SDL2/SDL.h>

    #include "event.hpp"
    #include "state.hpp"
    #include "common/color.hpp"
    #include "common/size.hpp"
    #include "common/align.hpp"
    #include "controls/widget.hpp"
    #include "controls/box.hpp"
    // #include "controls/scrolledbox.hpp"
    #include "renderer/drawing_context.hpp"
    
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

        private:
            SDL_Window *m_win = nullptr;
            SDL_GLContext m_sdl_context = nullptr;
            // Widget *main_widget = new ScrolledBox(Align::Vertical);
            Widget *m_main_widget = new Box(Align::Vertical);
            State *m_state = new State();
            bool m_needs_update = false;
            bool m_layout_changed = true;
            std::pair<Event, EventHandler> m_last_event = std::make_pair<Event, EventHandler>(Event::None, EventHandler::Accepted);
            std::chrono::time_point<std::chrono::steady_clock> m_last_event_time = std::chrono::steady_clock::now();

            /// Updates the projection matrix, clears the context and
            /// renders any state that was stored in the renderes from
            /// calls to the DrawingContext.
            /// Used internally by show().
            void draw();

            /// Redraws the Application and swaps the front buffer.
            void show();
    };
#endif