#ifndef STYLE_HPP
    #define STYLE_HPP

    #include "color.hpp"
    #include "../renderer/font.hpp"

    enum StyleOptions {
        STYLE_DEFAULT =     -1,
        STYLE_NONE    = 0x0000,
        STYLE_TOP     = 0x0001,
        STYLE_BOTTOM  = 0x0010,
        STYLE_LEFT    = 0x0100,
        STYLE_RIGHT   = 0x1000,
        STYLE_ALL     = 0x1111,
    };

    struct Style {
        struct Margin {
            i32 type   = STYLE_DEFAULT;
            i32 top    = STYLE_DEFAULT;
            i32 bottom = STYLE_DEFAULT;
            i32 left   = STYLE_DEFAULT;
            i32 right  = STYLE_DEFAULT;
        };

        struct Border {
            i32 type   = STYLE_DEFAULT;
            i32 top    = STYLE_DEFAULT;
            i32 bottom = STYLE_DEFAULT;
            i32 left   = STYLE_DEFAULT;
            i32 right  = STYLE_DEFAULT;
        };

        struct BorderColor {
            Color type   = COLOR_DEFAULT;
            Color top    = COLOR_DEFAULT;
            Color bottom = COLOR_DEFAULT;
            Color left   = COLOR_DEFAULT;
            Color right  = COLOR_DEFAULT;
        };

        struct Padding {
            i32 type   = STYLE_DEFAULT;
            i32 top    = STYLE_DEFAULT;
            i32 bottom = STYLE_DEFAULT;
            i32 left   = STYLE_DEFAULT;
            i32 right  = STYLE_DEFAULT;
        };

        Margin margin;
        Border border;
        BorderColor border_color;
        Padding padding;
        std::shared_ptr<Font> font = nullptr;

        Color window_background_color = COLOR_DEFAULT;
        Color widget_background_color = COLOR_DEFAULT;
        Color accent_widget_background_color = COLOR_DEFAULT;

        Color text_foreground_color = COLOR_DEFAULT;
        Color text_background_color = COLOR_DEFAULT;
        Color text_selected_color = COLOR_DEFAULT;
        Color text_disabled_color = COLOR_DEFAULT;

        Color hovered_background_color = COLOR_DEFAULT;
        Color pressed_background_color = COLOR_DEFAULT;
        Color accent_hovered_background_color = COLOR_DEFAULT;
        Color accent_pressed_background_color = COLOR_DEFAULT;

        Color icon_foreground_color = COLOR_DEFAULT;
        Color border_background_color = COLOR_DEFAULT;
    };
#endif
