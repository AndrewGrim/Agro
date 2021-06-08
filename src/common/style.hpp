#ifndef STYLE_HPP
    #define STYLE_HPP

    #include "color.hpp"

    #define TOP_PADDING(widget) (widget->style.padding.top == STYLE_DEFAULT ? Application::get()->dc->default_style.padding.top : widget->style.padding.top)
    #define BOTTOM_PADDING(widget) (widget->style.padding.bottom == STYLE_DEFAULT ? Application::get()->dc->default_style.padding.bottom : widget->style.padding.bottom)
    #define LEFT_PADDING(widget) (widget->style.padding.left == STYLE_DEFAULT ? Application::get()->dc->default_style.padding.left : widget->style.padding.left)
    #define RIGHT_PADDING(widget) (widget->style.padding.right == STYLE_DEFAULT ? Application::get()->dc->default_style.padding.right : widget->style.padding.right)

    #define TOP_BORDER(widget) (widget->style.border.top == STYLE_DEFAULT ? Application::get()->dc->default_style.border.top : widget->style.border.top)
    #define BOTTOM_BORDER(widget) (widget->style.border.bottom == STYLE_DEFAULT ? Application::get()->dc->default_style.border.bottom : widget->style.border.bottom)
    #define LEFT_BORDER(widget) (widget->style.border.left == STYLE_DEFAULT ? Application::get()->dc->default_style.border.left : widget->style.border.left)
    #define RIGHT_BORDER(widget) (widget->style.border.right == STYLE_DEFAULT ? Application::get()->dc->default_style.border.right : widget->style.border.right)


    enum StyleOptions {
        STYLE_DEFAULT = -1,
        STYLE_NONE = 0x0000,
        STYLE_TOP = 0x0001,
        STYLE_BOTTOM = 0x0010,
        STYLE_LEFT = 0x0100,
        STYLE_RIGHT = 0x1000,
        STYLE_ALL = 0x1111,
    };

    struct Style {
        struct Margin {
            int type = STYLE_DEFAULT;
            int top = STYLE_DEFAULT;
            int bottom = STYLE_DEFAULT;
            int left = STYLE_DEFAULT;
            int right = STYLE_DEFAULT;
        };

        struct Border {
            int type = STYLE_DEFAULT;
            int top = STYLE_DEFAULT;
            int bottom = STYLE_DEFAULT;
            int left = STYLE_DEFAULT;
            int right = STYLE_DEFAULT;

            // #adadad
            Color color_top = COLOR_DEFAULT;
            Color color_bottom = COLOR_DEFAULT;
            Color color_left = COLOR_DEFAULT;
            Color color_right = COLOR_DEFAULT;
        };

        struct Padding {
            int type = STYLE_DEFAULT;
            int top = STYLE_DEFAULT;
            int bottom = STYLE_DEFAULT;
            int left = STYLE_DEFAULT;
            int right = STYLE_DEFAULT;
        };

        Margin margin;
        Border border;
        Padding padding;

        Color window_background = COLOR_DEFAULT;
        Color widget_background = COLOR_DEFAULT;

        Color text_foreground = COLOR_DEFAULT;
        Color text_background = COLOR_DEFAULT;
        Color selected_text = COLOR_DEFAULT;
        Color text_disabled = COLOR_DEFAULT;

        Color selected_background = COLOR_DEFAULT;
        Color hovered_background = COLOR_DEFAULT;
        Color pressed_background = COLOR_DEFAULT;
    };
#endif
