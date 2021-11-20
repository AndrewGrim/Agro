#ifndef WIDGET_HPP
    #define WIDGET_HPP

    #include <vector>
    #include <unordered_map>
    #include <functional>

    #include "../state.hpp"
    #include "../event.hpp"
    #include "../keyboard.hpp"
    #include "../drawable.hpp"
    #include "../common/enums.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"
    #include "../common/color.hpp"
    #include "../common/point.hpp"
    #include "../common/style.hpp"
    #include "../renderer/drawing_context.hpp"
    #include "../option.hpp"

    class Widget;
    class Window;

    /// Generic implementation for EventListener which provides
    /// a simple API for a Widget callback to have multiple event listeners.
    /// Because underneath these simply use std::vector and Widgets use EventListeners
    /// for their own event handling, in order to remove built-in behaviour of a Widget,
    /// especially when inheriting from one, you can simply remove a listener from the
    /// listeners std::vector.
    template <typename... Types> struct EventListener {
        std::vector<std::function<void(Types... types)>> listeners;

        /// Calls each callback function stored in the EventListener
        /// in the order they were added unless dealing with the listeners
        /// std::vector directly.
        /// This method can be safely called on EventListeners which are empty
        /// as it uses the iterators to go over the listeners.
        void notify(Types... types) {
            for (auto callback : listeners) {
                callback(types...);
            }
        }

        /// Simple wrapper for adding a new callback function.
        // TDOD we should probably think about overriding operator+
        // for more succint syntax.
        void addEventListener(std::function<void(Types... types)> fn) {
            listeners.push_back(fn);
        }
    };

    /// A generic class meant to represent most interactable
    /// graphical elements within the library.
    /// To a big extent.. everything is a Widget. Layouts are also Widgets
    /// the only distinction between them and a non-layout Widget is that
    /// it communicates that its responsible for layout and other Widgets
    /// need to account for that.
    class Widget : public Drawable {
        public:
            /// A rectangle representing the Widget position and size.
            Rect rect = Rect(0, 0, 0, 0);

            /// A rectangle representing the Widget position and size
            /// but without the margins, border and padding.
            Rect inner_rect = Rect(0, 0, 0, 0);

            /// The parent* is mostly used by compound Widgets.
            Widget *parent = nullptr;

            int parent_index = 0; // TODO this would not be representative
            // when there is a parent but the widget its not part of the children
            // like in Splitter
            // also means remove is gonna totally break for splitter and any other
            // Widget that doesnt use the children vector to store the child Widgets.

            // TODO multiple windows
            // int window_index;

            /// Stores all the children Widgets of this Widget.
            /// Its not meant to be interacted with directly but
            /// rather through methods like append().
            std::vector<Widget*> children;

            /// Describes the style according to which the Widget should be
            /// drawn. Note that for most Widget this will contain default
            /// values that specify that the drawee should use the default
            /// style from DrawingContext.
            Style style;

            Widget *tooltip = nullptr;
            Widget *context_menu = nullptr;

            /// The following functions pointers are responsible
            /// for callbacks that the Widget can execute as
            /// certain events happen. If not in use these should
            /// always be `nullptr` or `NULL`.
            /// std::function allows the assigned function pointer
            /// to be a standalone function, a member method or
            /// a lambda.
            ///
            /// Note: When a user clicks on a Widget the events
            /// will always fire in this order: Down, Up, Click.
            /// Furthermore, the Click event will **ONLY** fire if
            /// both the target of Down and Up are the same.

            /// `onMouseDown` gets called when the user presses
            /// down **ANY** mouse button over the target Widget.
            EventListener<Widget*, MouseEvent> onMouseDown = EventListener<Widget*, MouseEvent>();

            ///`onMouseUp` gets called when the user releases
            /// **ANY** mouse button over the target Widget.
            EventListener<Widget*, MouseEvent> onMouseUp = EventListener<Widget*, MouseEvent>();

            /// `onMouseClick` gets called whenver the user presses
            /// down on a Widget and the releases the button over
            /// the same Widget.
            // TODO this will likely trigger when the mouse events come from different buttons
            // i dont think that behaviour is very intuitive and so should be changed.
            EventListener<Widget*, MouseEvent> onMouseClick = EventListener<Widget*, MouseEvent>();

            /// `onMouseLeft` get called whenever the mouse leaves
            /// the area of the Widget.
            EventListener<Widget*, MouseEvent> onMouseLeft = EventListener<Widget*, MouseEvent>();

            /// `onMouseEntered` get called whenever the mouse
            /// enters the area of the Widget.
            EventListener<Widget*, MouseEvent> onMouseEntered = EventListener<Widget*, MouseEvent>();

            /// `onMouseMotion` get called whenever the mouse moves
            /// over the area of the Widget.
            EventListener<Widget*, MouseEvent> onMouseMotion = EventListener<Widget*, MouseEvent>();

            EventListener<Widget*> onFocusLost = EventListener<Widget*>();
            EventListener<Widget*> onFocusGained = EventListener<Widget*>();

            /// The constructor is empty.
            Widget();

            /// The destructor is empty.
            virtual ~Widget();

            /// This method should return the name of the Widget class
            /// in `CamelCase`.
            /// This is needed for the user to perform run-time casting.
            virtual const char* name() = 0;

            /// This method is responsible for drawing the widget
            /// to the screen whenever the application updates.
            /// It should handle drawing the widgets in different
            /// states; such as Disabled, Focused, Pressed, Hovered.
            /// When applicable it should prefer to use global
            /// color theme values.
            virtual void draw(DrawingContext &dc, Rect rect, int state) = 0;

            /// This method is used to add a Widget to the children
            /// of the Widget in question. It adds the Widget to the
            /// end of the children dynamic array.
            virtual Widget* append(Widget *widget, Fill fill_policy = Fill::None, unsigned int proportion = 1);

            /// This method is used to remove a Widget from parent's children based on the given index.
            // TODO the index should be passed in internally rather than requiring the user to pass it in.
            virtual Widget* remove(size_t parent_index);

            /// This method is used to free the Widget. It will also remove it from its parent's children when applicable.
            /// DO NOT manually delete defualt Widgets.
            virtual void destroy();

            /// This method is responsible for returning the minimum size
            /// for the widget.
            /// It is directly used by any layout widgets for computing
            /// the layout extents.
            ///
            /// A layout widget should return the total size of the layout.
            /// In the Box layout this would take the form of total children size
            /// on the alignment axis and max children size for the other axis.
            ///
            /// To avoid needlessly recalculating the sizeHint all the time
            /// especially when nothing has changed and when it can be very
            /// expensive like for layout Widgets.
            /// You should utilize the built in m_size_changed memeber.
            /// If it hasnt changed simply return the last result from m_size.
            virtual Size sizeHint(DrawingContext &dc) = 0;

            /// This method sets the Fill policy of the Widget.
            /// Fill dicates how the Widget will expand.
            /// Options are: None, Horizontal, Vertical and Both.
            Widget* setFillPolicy(Fill fill_policy);

            /// This method retrieves the Fill policy of the Widget.
            Fill fillPolicy();

            /// Changes the Widget's visibility.
            Widget* show();

            /// Changes the Widget's visibility.
            Widget* hide();

            bool isVisible();

            /// Used to check if the Widget implements a Layout interface.
            /// This affects how the events pass through the Widget.
            /// A non-layout Widget matching an event will be called
            /// with handleMouseWidget().
            /// Meanwhile, a layout Widget will get called with propagateMouseEvent().
            virtual bool isLayout();

            /// Used to check if the Widget implements a Scrollable interface.
            // TODO double check what this is about??? i think it just indicates whether a Widget utilizes ScrollBar?
            virtual bool isScrollable();

            virtual bool isFocusable();

            /// Returns whether the Widget is currently hovered or not.
            bool isHovered();

            /// Returns whether the Widget is currently pressed or not.
            bool isPressed();

            /// Returns whether the Widget is currently focused or not.
            bool isFocused();

            /// Tells the Application that it needs to redraw.
            Widget* update();

            /// Recursively tells the parent that m_size_changed so only
            /// the affected Widgets recalculate their sizeHints.
            Widget* layout();

            /// Passes the event further down the Widget tree until
            /// it finds a Widget that matches the x and y of the event.
            virtual void* propagateMouseEvent(Window *window, State *state, MouseEvent event);

            /// Handles the MouseEvent sent by the Application.
            virtual void handleMouseEvent(Window *window, State *state, MouseEvent event);

            virtual void handleTextEvent(DrawingContext &dc, const char *text);

            virtual bool handleScrollEvent(ScrollEvent event);

            virtual Widget* propagateFocusEvent(FocusEvent event, State *state, Option<int> child_index);

            unsigned int proportion();

            Widget* setProportion(unsigned int proportion);

            // TODO font should probably be moved to style
            Font* font();

            // TODO font should probably be moved to style
            Widget* setFont(Font *font);

            /// Returns the Widget state bit flag consisting of:
            /// focus, hover and pressed states.
            int state();

            int bind(int key, int modifiers, std::function<void()> callback);
            int bind(int key, Mod modifier, std::function<void()> callback);
            void unbind(int map_key);
            const std::unordered_map<int, KeyboardShortcut> keyboardShortcuts();
            Size size();
            void setStyle(Style style);

            bool isWidget();

            void forEachWidget(std::function<void(Widget *widget)> action);

            bool m_is_visible = true;
            Fill m_fill_policy = Fill::None;

            /// Determines how much the Widget will scale into expandable space
            /// compared to other Widgets.
            /// Default is 1 and it means that each Widget will get the same
            /// amount of expandable space if its available.
            /// Proportion higher than that, 5 for example, will mean that for every pixel a default
            /// Widget expands this one will expand 5 pixels.
            unsigned int m_proportion = 1;

            /// The precomputed sizeHint of the Widget.
            /// Returned when nothing changed that would
            /// invalidate the sizeHint() calculation.
            Size m_size = Size();

            /// Stores whether the sizeHint() calculation
            /// needs to be recomputed.
            bool m_size_changed = true;

            // TODO font should probably be moved to style
            Font *m_font = nullptr;

            // TODO wtf is this for?
            int m_binding_id = 0;
            std::unordered_map<int, KeyboardShortcut> m_keyboard_shortcuts;
    };
#endif
