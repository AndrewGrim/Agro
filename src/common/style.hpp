#ifndef STYLE_HPP
    #define STYLE_HPP

    #include "color.hpp"

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
        int margin = STYLE_ALL;
        int margin_top = 0;
        int margin_bottom = 0;
        int margin_left = 0;
        int margin_right = 0;

        int border = STYLE_ALL;
        int border_top = 2;
        Color border_top_color = COLOR_BLACK;
        int border_bottom = 2;
        Color border_bottom_color = COLOR_BLACK;
        int border_left = 2;
        Color border_left_color = COLOR_BLACK;
        int border_right = 2;
        Color border_right_color = COLOR_BLACK;

        int padding = STYLE_ALL;
        int padding_top = 5;
        int padding_bottom = 5;
        int padding_left = 5;
        int padding_right = 5;

        Color text_color = COLOR_BLACK;
        Color background_color = Color(0.9, 0.9, 0.9);
        Color window_color = Color(0.5, 0.5, 0.5);
        Color selection_color = Color(0.2, 0.5, 1.0);
        Color hover_color = Color(0.5, 0.5, 0.5, 0.1);

        Style();
        Style(StyleOptions default_value, int default_size);
        ~Style();
    };
#endif