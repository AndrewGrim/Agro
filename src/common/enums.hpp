#ifndef ENUMS_HPP
    #define ENUMS_HPP

    enum class Gradient {
        TopToBottom,
        LeftToRight,
    };

    enum class Fill {
        None,
        Horizontal,
        Vertical,
        Both,
    };

    // TODO direction? layout?
    enum class Align {
        Horizontal,
        Vertical,
    };

    enum class HorizontalAlignment {
        Left,
        Right,
        Center,
    };

    enum class VerticalAlignment {
        Top,
        Bottom,
        Center,
    };
#endif