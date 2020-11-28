#ifndef NEW_WIDGET_HPP
    #define NEW_WIDGET_HPP

    #include <vector>
    #include <functional>

    #include "../state.hpp"
    #include "../event.hpp"
    #include "../common/rect.hpp"
    #include "../common/fill.hpp"
    #include "../common/align.hpp"
    #include "../common/point.hpp"
    #include "../common/size.hpp"
    #include "../common/color.hpp"

    #include "../renderer/drawing_context.hpp"

    class NewWidget {
        public:
            /// The following functions pointers are responsible
            /// for callbacks that the Widget can execute as 
            /// certain events happen. If not in use these should
            /// always be `nullptr` or `NULL`.
            /// std::function allows the assigned function pointer
            /// to be a standalone function, a member method or
            /// a lambda.
            ///
            /// Note: When a user clicks on a Widget the events
            /// will always fire in this order: Down, Up, Click
            /// furthermore the Click event will **ONLY** fire if
            /// both the target of Down and Up are the same.
            ///
            /// `onMouseDown` gets called when the user presses
            /// down **ANY** mouse button over the target Widget. 
            std::function<void(MouseEvent)> onMouseDown = nullptr;
            std::function<void(MouseEvent)> onMouseUp = nullptr;
            std::function<void(MouseEvent)> onMouseClick = nullptr;
            std::function<void(MouseEvent)> onMouseLeft = nullptr;
            std::function<void(MouseEvent)> onMouseEntered = nullptr;
            std::function<void(MouseEvent)> onMouseMotion = nullptr; // TODO name it move instead?

            NewWidget() {};
            virtual ~NewWidget() {};

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
            NewWidget* append(NewWidget* widget, Fill fill_policy);

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
            virtual Color background();
            virtual NewWidget* setBackground(Color background);
            virtual Color hoverBackground();
            virtual NewWidget* setHoverBackground(Color background);
            virtual Color pressedBackground();
            virtual NewWidget* setPressedBackground(Color background);

            /// This method sets the Fill policy of the Widget.
            /// Fill dicates how the Widget will expand.
            /// Options are: None, Horizontal, Vertical and Both.
            NewWidget* setFillPolicy(Fill fill_policy);

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
            virtual bool isLayout(); // TODO i'm not sure if this is the best way
            // an alternative would be to use dynamic casts at runtime, but
            // i think in this case the overriding would be better
            virtual bool isScrollable(); // TODO i'm not sure if this is the best way
            // an alternative would be to use dynamic casts at runtime, but
            // i think in this case the overriding would be better
            // TODO is enabled, this would ignore events when disabled, and have a different colour scheme
            bool isHovered();
            void setHovered(bool hover);
            bool isPressed();
            void setPressed(bool pressed);
            bool isFocused();
            void setFocused(bool focused);
            void update();
            void* propagateMouseEvent(State *state, MouseEvent event);
            virtual void mouseEvent(State *state, MouseEvent event);
            void attachApp(void *app);
    };
#endif