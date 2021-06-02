#pragma once

#include "renderer/drawing_context.hpp"

class Drawable {
	public:
		enum DrawableState {
            STATE_DEFAULT = 0x0000,
            STATE_HOVERED = 0x0010,
            STATE_PRESSED = 0x0100,
            STATE_FOCUSED = 0x1000,
        };
		Drawable() {}
		virtual ~Drawable() {}
		virtual void draw(DrawingContext &dc, Rect rect, int state) = 0;
		virtual Size sizeHint(DrawingContext &dc) = 0;
		virtual bool isWidget() = 0;
};
