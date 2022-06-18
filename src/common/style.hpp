#ifndef STYLE_HPP
    #define STYLE_HPP

    #include <memory>

    #include "color.hpp"
    #include "../renderer/font.hpp"

    enum StyleOptions : i8 {
        STYLE_DEFAULT = -1,
        STYLE_NONE    =  0,
        STYLE_TOP     =  1,
        STYLE_BOTTOM  =  2,
        STYLE_LEFT    =  4,
        STYLE_RIGHT   =  8,
        STYLE_ALL     =  15,
    };

    struct Style {
        struct BorderColor {
            Color type   = COLOR_DEFAULT;
            Color top    = COLOR_DEFAULT;
            Color bottom = COLOR_DEFAULT;
            Color left   = COLOR_DEFAULT;
            Color right  = COLOR_DEFAULT;
        };

        struct Border {
            i8 type   = STYLE_DEFAULT;
            i16 top    = STYLE_DEFAULT;
            i16 bottom = STYLE_DEFAULT;
            i16 left   = STYLE_DEFAULT;
            i16 right  = STYLE_DEFAULT;
        };

        struct Margin {
            i8 type   = STYLE_DEFAULT;
            i16 top    = STYLE_DEFAULT;
            i16 bottom = STYLE_DEFAULT;
            i16 left   = STYLE_DEFAULT;
            i16 right  = STYLE_DEFAULT;
        };

        struct Padding {
            i8 type   = STYLE_DEFAULT;
            i16 top    = STYLE_DEFAULT;
            i16 bottom = STYLE_DEFAULT;
            i16 left   = STYLE_DEFAULT;
            i16 right  = STYLE_DEFAULT;
        };

        BorderColor border_color;
        Border border;
        Margin margin;
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
