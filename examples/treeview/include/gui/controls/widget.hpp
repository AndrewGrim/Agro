#ifndef WIDGET_HPP
    #define WIDGET_HPP

    #include <vector>
    #include <unordered_map>
    #include <functional>

    #include "../state.hpp"
    #include "../event.hpp"
    #include "../keyboard.hpp"
    #include "../common/enums.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"
    #include "../common/color.hpp"
    #include "../common/point.hpp"
    #include "../common/style.hpp"
    #include "../renderer/drawing_context.hpp"

    class Widget;

    class EventListener {
        public:
            std::vector<std::function<void(Widget *widget, MouseEvent event)>> listeners;

            void notify(Widget *widget, MouseEvent event) {
                for (auto callback : listeners) {
                    callback(widget, event);
                }
            }

            void addEventListener(std::function<void(Widget *widget, MouseEvent event)> fn) {
                listeners.push_back(fn);
            }
    };

    class Widget {
        public:
            /// A rectangle representing the Widget position and size.
            Rect rect = Rect(0, 0, 0, 0);
            Rect inner_rect = Rect(0, 0, 0, 0);
            
            /// The parent* is mostly used by compound Widgets.
            Widget *parent = nullptr;

            size_t parent_index = 0;
            
            /// Stores all the child Widgets of this Widget.
            /// Its not meant to be interacted with directly but
            /// rather through methods like append().
            std::vector<Widget*> children;

            Style style;

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
            EventListener onMouseDown = EventListener();
            
            ///`onMouseUp` gets called when the user releases
            /// **ANY** mouse button over the target Widget.
            EventListener onMouseUp = EventListener();

            /// `onMouseClick` gets called whenver the user presses
            /// down on a Widget and the releases the button over
            /// the same Widget.
            // TODO this will likely trigger when the mouse events come from different buttons
            // i dont think that behaviour is very intuitive and so should be changed.
            EventListener onMouseClick = EventListener();

            /// `onMouseLeft` get called whenever the mouse leaves
            /// the area of the Widget.
            EventListener onMouseLeft = EventListener();

            /// `onMouseEntered` get called whenever the mouse
            /// enters the area of the Widget.
            EventListener onMouseEntered = EventListener();

            /// `onMouseMotion` get called whenever the mouse moves
            /// over the area of the Widget.
            EventListener onMouseMotion = EventListener();

            /// The constructor is empty.
            Widget();

            /// The destructor is empty.
            virtual ~Widget();

            /// This method should return the name of the widget
            /// in `CamelCase`.
            virtual const char* name() = 0;

            /// This method is responsible for drawing the widget
            /// to the screen whenever the application updates.
            /// It should handle drawing the widgets in different
            /// states; such as Disabled, Focused, Pressed, Hovered.
            /// When applicable it should prefer to use global
            /// color theme values. 
            virtual void draw(DrawingContext *dc, Rect rect) = 0;

            /// This method is used to add a Widget to the children
            /// of the Widget in question. It adds the Widget to the
            /// end of the children dynamic array.
            virtual Widget* append(Widget *widget, Fill fill_policy = Fill::None, unsigned int proportion = 1);

            /// This method is used to remove a Widget from parent's children based on the given index.
            virtual Widget* remove(size_t parent_index);

            /// This method is used to free the Widget. It will also remove it from its parent's children when applicable.
            virtual void destroy();

            /// This method is responsible for returning the minimum size
            /// for the widget.
            /// It is directly used by any layout widgets for computing
            /// the layout extents.
            ///
            /// A layout widget should return the total size of the layout.
            /// In the Box layout this would take the form of total children size
            /// on the alignment axis and max children size for the other axis.
            // TODO add example from box layout
            // TODO note about performance and avoiding recomputing the sizeHint
            // unless something has changed
            virtual Size sizeHint(DrawingContext *dc) = 0;

            /// Returns the background Color of the Widget.
            Color background();
            
            /// Sets the background Color of the Widget.
            virtual Widget* setBackground(Color background);

            /// Returns the foreground Color of the Widget.
            Color foreground();
            
            /// Sets the foreground Color of the Widget.
            virtual Widget* setForeground(Color foreground);
            
            /// This method sets the Fill policy of the Widget.
            /// Fill dicates how the Widget will expand.
            /// Options are: None, Horizontal, Vertical and Both.
            Widget* setFillPolicy(Fill fill_policy);

            /// This method retrieves the Fill policy of the Widget.
            Fill fillPolicy();

            Widget* show();
            
            Widget* hide();
            
            bool isVisible();
            
            /// Used to check if the Widget implements a Layout interface.
            virtual bool isLayout();
            
            /// Used to check if the Widget implements a Scrollable interface.
            virtual bool isScrollable();
            
            /// Returns whether the Widget is currently hovered or not.
            bool isHovered();

            /// Sets whether the Widget is currently hovered or not.
            /// To avoid unecessary redraws it first checks if the new
            /// value is the same as the one stored in the Widget
            /// and if its not it will call update().
            Widget* setHovered(bool hover);

            /// Returns whether the Widget is currently pressed or not.
            bool isPressed();

            /// Sets whether the Widget is currently pressed or not.
            /// To avoid unecessary redraws it first checks if the new
            /// value is the same as the one stored in the Widget
            /// and if its not it will call update().
            Widget* setPressed(bool pressed);

            /// Returns whether the Widget is currently focused or not.
            bool isFocused();

            /// Sets whether the Widget is currently focused or not.
            /// To avoid unecessary redraws it first checks if the new
            /// value is the same as the one stored in the Widget
            /// and if its not it will call update().
            Widget* setFocused(bool focused);

            /// Tells the Application that it needs to redraw.
            Widget* update();

            /// Tells the Application that it needs to recalculate its layout. // TODO outdated
            Widget* layout();
            void parentLayout();

            /// Passes the event further down the Widget tree until
            /// it finds a Widget that matches the x and y of the event.
            virtual void* propagateMouseEvent(State *state, MouseEvent event);

            /// Handles the MouseEvent sent by the Application.
            virtual void handleMouseEvent(State *state, MouseEvent event);

            virtual void handleTextEvent(DrawingContext *dc, const char *text);

            virtual bool handleScrollEvent(ScrollEvent event);

            unsigned int proportion();

            Widget* setProportion(unsigned int proportion);

            Font* font();

            Widget* setFont(Font *font);

            int bind(int key, int modifiers, std::function<void()> callback);
            int bind(int key, Mod modifier, std::function<void()> callback);
            void unbind(int map_key);
            const std::unordered_map<int, KeyboardShortcut> keyboardShortcuts();
            Size size();
            void setStyle(Style style);

        protected:
            bool m_is_hovered = false;
            bool m_is_pressed = false;
            bool m_is_focused = false;
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

            Color m_fg = Color();
            Color m_bg = Color(1, 1, 1);
            Color m_hovered_bg = Color(0.8f, 0.8f, 0.8f); // TODO i think these should be implemented
            // as an altered fg, bg color? dynamic i mean
            Color m_pressed_bg = Color(0.6f, 0.6f, 0.6f); // TODO i think these should be implemented
            // as an altered fg, bg color? dynamic i mean

            Font *m_font = nullptr;

            int m_binding_id = 0;
            std::unordered_map<int, KeyboardShortcut> m_keyboard_shortcuts;
    };
#endif