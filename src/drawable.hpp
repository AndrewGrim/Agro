#ifndef DRAWABLE_HPP
    #define DRAWABLE_HPP

    #include "renderer/drawing_context.hpp"

    class Widget;

    /// A generic class meant to represent any graphical
    /// element within the libarary.
    /// The reason that Drawable is the base class
    /// as opposed to Widget is that when it comes to TreeView
    /// this approach allows us to have either a custom CellRenderer
    /// or even a regular Widget within a cell.
    /// Providing "free" inclusion of Widgets within the TreeView Widget.
    struct Drawable {
        /// Describes the style according to which the Drawable should be
        /// drawn. Note that for most Drawables this will contain default
        /// values that specify that the drawee should use the default
        /// style from DrawingContext.
        Style _style;

        /// The parent* is mostly used by compound Widgets and event handling (especially layout).
        Widget *parent = nullptr;

        /// The precomputed sizeHint of the Drawable.
        /// Returned when nothing changed that would
        /// invalidate the sizeHint() calculation.
        Size m_size = Size();

        /// Stores whether the sizeHint() calculation
        /// needs to be recomputed.
        bool m_size_changed = true;

        Drawable();

        virtual ~Drawable();

        /// Method used to draw the element to the Window.
        /// Note that this library does not support incremental drawing
        /// at this moment, so whenever draw is called the Drawable should
        /// draw itself in its entirety every time unless it is a Scrollable or
        /// similar where you would only draw the visible portion.
        virtual void draw(DrawingContext &dc, Rect rect, i32 state) = 0;

        /// The sizeHint informs layout Widgets the extents
        /// a Drawable should have.
        /// A layout should guarantee that a Drawable gets its requested size.
        virtual Size sizeHint(DrawingContext &dc) = 0;

        /// Used to determine whether a Drawable is a Widget.
        /// This has a big effect on interaction since Widgets
        /// support events and this tells the TreeView to treat them differently.
        virtual bool isWidget();

        /// Tells the Application that it needs to redraw.
        void update();

        /// Recursively tells the parent that m_size_changed so only
        /// the affected Drawables recalculate their sizeHints.
        void layout(LayoutEvent event);
        virtual bool handleLayoutEvent(LayoutEvent event);

        void setStyle(Style style);
        Style& style();
        void setMarginType(i32 type);
        i32 marginType();
        void setMarginTop(i32 top);
        i32 marginTop();
        void setMarginBottom(i32 bottom);
        i32 marginBottom();
        void setMarginLeft(i32 left);
        i32 marginLeft();
        void setMarginRight(i32 right);
        i32 marginRight();
        void setBorderType(i32 type);
        i32 borderType();
        void setBorderTop(i32 top);
        i32 borderTop();
        void setBorderBottom(i32 bottom);
        i32 borderBottom();
        void setBorderLeft(i32 left);
        i32 borderLeft();
        void setBorderRight(i32 right);
        i32 borderRight();
        void setBorderColorTop(Color top);
        Color borderColorTop();
        void setBorderColorBottom(Color bottom);
        Color borderColorBottom();
        void setBorderColorLeft(Color left);
        Color borderColorLeft();
        void setBorderColorRight(Color right);
        Color borderColorRight();
        void setPaddingType(i32 type);
        i32 paddingType();
        void setPaddingTop(i32 top);
        i32 paddingTop();
        void setPaddingBottom(i32 bottom);
        i32 paddingBottom();
        void setPaddingLeft(i32 left);
        i32 paddingLeft();
        void setPaddingRight(i32 right);
        i32 paddingRight();
        void setMargin(Style::Margin margin);
        Style::Margin margin();
        void setBorder(Style::Border border);
        Style::Border border();
        void setPadding(Style::Padding padding);
        Style::Padding padding();
        void setFont(std::shared_ptr<Font> font);
        std::shared_ptr<Font> font();
        void setWindowBackgroundColor(Color window_background_color);
        Color windowBackgroundColor();
        void setWidgetBackgroundColor(Color widget_background_color);
        Color widgetBackgroundColor();
        void setAccentWidgetBackgroundColor(Color accent_widget_background_color);
        Color accentWidgetBackgroundColor();
        void setTextForegroundColor(Color text_foreground_color);
        Color textForegroundColor();
        void setTextBackgroundColor(Color text_background_color);
        Color textBackgroundColor();
        void setTextSelectedColor(Color text_selected_color);
        Color textSelectedColor();
        void setTextDisabledColor(Color text_disabled_color);
        Color textDisabledColor();
        void setHoveredBackgroundColor(Color hovered_background_color);
        Color hoveredBackgroundColor();
        void setPressedBackgroundColor(Color pressed_background_color);
        Color pressedBackgroundColor();
        void setAccentHoveredBackgroundColor(Color accent_hovered_background_color);
        Color accentHoveredBackgroundColor();
        void setAccentPressedBackgroundColor(Color accent_pressed_background_color);
        Color accentPressedBackgroundColor();
        void setIconForegroundColor(Color icon_foreground_color);
        Color iconForegroundColor();
        void setBorderBackgroundColor(Color border_background_color);
        Color borderBackgroundColor();
    };
#endif
