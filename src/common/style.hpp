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
        int border = STYLE_ALL;
        int border_size = 2;
        int padding = STYLE_ALL;
        int padding_size = 5;
        int margin = STYLE_NONE;
        int margin_size = 0;

        Color border_color = COLOR_BLACK;
        Color text_color = COLOR_BLACK;
        Color background_color = Color(0.9, 0.9, 0.9);
        Color selection_color = Color(0.2, 0.5, 1.0);
        Color hover_color = Color(0.5, 0.5, 0.5, 0.1);

        Style();
        Style(StyleOptions default_value, int default_size);
        ~Style();
    };
#endif