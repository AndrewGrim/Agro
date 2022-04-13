#pragma once

#include "renderer/drawing_context.hpp"

/// A generic class meant to represent any graphical
/// element within the libarary.
/// The reason that Drawable is the base class
/// as opposed to Widget is that when it comes to TreeView
/// this approach allows us to have either a custom CellRenderer
/// or even a regular Widget within a cell.
/// Providing "free" inclusion of Widgets within the TreeView Widget.
struct Drawable {
    /// Describes the various UI states that can apply to graphical elements.
    /// This can be used to determine how to draw a Widget in accordance
    /// to how the user is currently interacting with it.
    enum DrawableState {
        STATE_DEFAULT      = 0x000000,
        STATE_HOVERED      = 0x000010,
        STATE_PRESSED      = 0x000100,
        STATE_SOFT_FOCUSED = 0x001000,
        STATE_HARD_FOCUSED = 0x010000,
    };

    Drawable() {}

    virtual ~Drawable() {}

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
    virtual bool isWidget() = 0;
};
