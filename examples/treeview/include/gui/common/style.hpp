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

    // TODO we need to think about color somewhat
    // cause we want to be able to tell when a color
    // is not overriden
    // i though about using uint32_t for 8bits per color/alpha
    // but then we couldnt really set it to -1 unless we checked for
    // the max uint32_t value instead
    // and on the flip side int32_t would quite work for bitshifting?
    // i pressume anyway, the uint32_t method should also be easier
    // to work with rgba int and hex | string
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

            Color color_top = COLOR_BLACK;
            Color color_bottom = COLOR_BLACK;
            Color color_left = COLOR_BLACK;
            Color color_right = COLOR_BLACK;
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
        Color text_color = COLOR_BLACK;
        Color background_color = Color(0.9f, 0.9f, 0.9f);
        Color window_color = Color(0.5f, 0.5f, 0.5f);
        Color selection_color = Color(0.2f, 0.5f, 1.0f);
        Color hover_color = Color(0.5f, 0.5f, 0.5f, 0.1f);

        Style();
        ~Style();
    };
#endif