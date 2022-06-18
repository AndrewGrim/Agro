#ifndef ENUMS_HPP
    #define ENUMS_HPP

    #include "number_types.h"

    enum class Gradient : u8 {
        TopToBottom,
        LeftToRight,
    };

    enum class Fill : u8 {
        None,
        Horizontal,
        Vertical,
        Both,
    };

    // TODO direction? layout?
    enum class Align : u8 {
        Horizontal,
        Vertical,
    };

    enum class HorizontalAlignment : u8 {
        Left,
        Right,
        Center,
    };

    enum class VerticalAlignment : u8 {
        Top,
        Bottom,
        Center,
    };

    /// Describes the various UI states that can apply to graphical elements.
    /// This can be used to determine how to draw a Drawable in accordance
    /// to how the user is currently interacting with it.
    enum DrawableState : u8 {
        STATE_DEFAULT      = 0b000000,
        STATE_HOVERED      = 0b000010,
        STATE_PRESSED      = 0b000100,
        STATE_SOFT_FOCUSED = 0b001000,
        STATE_HARD_FOCUSED = 0b010000,
    };

    /// Describes the layout changes that occured so that a Drawable
    /// can make the minimal recalculations necessary to updates its size.
    enum LayoutEvent : u8 {
        LAYOUT_NONE    = 0b000000,
        LAYOUT_MARGIN  = 0b000001,
        LAYOUT_BORDER  = 0b000010,
        LAYOUT_PADDING = 0b000100,
        LAYOUT_FONT    = 0b001000,
        LAYOUT_STYLE   = 0b001111,
        LAYOUT_CHILD   = 0b010000,
        LAYOUT_SCALE   = 0b100000,
    };
#endif
