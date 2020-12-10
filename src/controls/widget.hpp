#ifndef WIDGET_HPP
    #define WIDGET_HPP

    #include <vector>
    #include <functional>

    #include "../state.hpp"
    #include "../event.hpp"
    #include "../common/rect.hpp"
    #include "../common/fill.hpp"
    #include "../common/size.hpp"
    #include "../common/color.hpp"
    #include "../renderer/drawing_context.hpp"

    class Widget {
        public:
            /// A rectangle representing the Widget position and size.
            Rect rect = Rect(0, 0, 0, 0);
            
            /// Pointer to the Application. Used to tell the Application to update.
            void *app = nullptr;

            /// The parent* is mostly used by compound Widgets.
            Widget *parent = nullptr;

            size_t parent_index = 0;
            
            /// Stores all the child Widgets of this Widget.
            /// Its not meant to be interacted with directly but
            /// rather through methods like append().
            std::vector<Widget*> children;

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
            std::function<void(MouseEvent)> onMouseDown = nullptr;
            
            ///`onMouseUp` gets called when the user releases
            /// **ANY** mouse button over the target Widget.
            // TODO does this also get called when we release over a layout?? dont think so but check.
            std::function<void(MouseEvent)> onMouseUp = nullptr;

            /// `onMouseClick` gets called whenver the user presses
            /// down on a Widget and the releases the button over
            /// the same Widget.
            // TODO this will likely trigger when the mouse events come from different buttons
            // i dont think that behaviour is very intuitive and so should be changed.
            std::function<void(MouseEvent)> onMouseClick = nullptr;

            /// `onMouseLeft` get called whenever the mouse leaves
            /// the area of the Widget.
            std::function<void(MouseEvent)> onMouseLeft = nullptr;

            /// `onMouseEntered` get called whenever the mouse
            /// enters the area of the Widget.
            std::function<void(MouseEvent)> onMouseEntered = nullptr;

            /// `onMouseMotion` get called whenever the mouse moves
            /// over the area of the Widget.
            std::function<void(MouseEvent)> onMouseMotion = nullptr; // TODO name it move instead?

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

            virtual Widget* remove(size_t parent_index);

            /// This method is responsible for returning the minimum size
            /// for the widget.
            /// It is directly used by any layout widgets for computing
            /// the layout extents.
            ///
            /// A layout widget should return the total size of the layout.
            /// Which most often would take the form of total children size
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

            void show(); // TODO not used at the moment, 
            // but i totally see them being useful, 
            // should probably use this information in layout 
            // and when drawing and just skip over these widget
            
            void hide(); // TODO not used at the moment, 
            // but i totally see them being useful, 
            // should probably use this information in layout 
            // and when drawing and just skip over these widget
            
            bool isVisible();
            
            /// Used to check if the Widget implements a Layout interface.
            virtual bool isLayout();
            
            /// Used to check if the Widget implements a Scrollable interface.
            virtual bool isScrollable();

            // TODO acceptsInput(); this would be used for text widgets, would it be used for shortcuts as well??
            
            /// Returns whether the Widget is currently hovered or not.
            bool isHovered();

            /// Sets whether the Widget is currently hovered or not.
            /// To avoid unecessary redraws it first checks if the new
            /// value is the same as the one stored in the Widget
            /// and if its not it will call update().
            void setHovered(bool hover);

            /// Returns whether the Widget is currently pressed or not.
            bool isPressed();

            /// Sets whether the Widget is currently pressed or not.
            /// To avoid unecessary redraws it first checks if the new
            /// value is the same as the one stored in the Widget
            /// and if its not it will call update().
            void setPressed(bool pressed);

            /// Returns whether the Widget is currently focused or not.
            bool isFocused();

            /// Sets whether the Widget is currently focused or not.
            /// To avoid unecessary redraws it first checks if the new
            /// value is the same as the one stored in the Widget
            /// and if its not it will call update().
            void setFocused(bool focused);

            /// Tells the Application that it needs to redraw.
            void update();

            /// Tells the Application that it needs to recalculate its layout.
            void layout();

            /// Passes the event further down the Widget tree until
            /// it finds a Widget that matches the x and y of the event.
            /// Should not be used directly! Let the application call it.
            // TODO im not a fan of this one and the next one being public
            // and asking the user not to call. There must be a better way!
            virtual void* propagateMouseEvent(State *state, MouseEvent event);

            /// Handles the MouseEvent sent by the Application.
            /// Should not be used directly! Let the application call it.
            virtual void handleMouseEvent(State *state, MouseEvent event);

            /// Sets the Application* of the Widget and does the same
            /// for any of its children.
            virtual void attachApp(void *app);

            unsigned int proportion();

            void setProportion(unsigned int proportion);

            // TODO hmm.. we might need to set the parent for every widget
            // then if the widget has a parent, remove its from its children
            // and do any deleting operations that are required.
            // we should be easily able to set the parent in append
            // how would we know the index in the children vector??
            // i guess just compare the pointers, so its not going to be super efficient
            // we could store the index in the widget, but it would have to be updated
            // whenever it would move around in the children vector, thankfully it doesnt atm
            // we could also do some operations when append finds that a widget already has a parent
            // to prevent double inserts and so we could easily move ownership
            // what would happen if you appended the mainWidget somewhere?? just dont do it dog
            // well actually the main widget could have an application pointer as its parent? hmm
            // i think it would be better if the parent* would be null for mainWidget, otherwise
            // we would need to deal with fucked casting and this way we know that the widget
            // either hasnt been appended anywhere or its the mainWidget
            // virtual void destroy();

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
    };
#endif