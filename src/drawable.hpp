#pragma once

#include "renderer/drawing_context.hpp"

class Drawable {
	public:
		Drawable() {}
		virtual ~Drawable() {}
		virtual void draw(DrawingContext &dc, Rect rect) = 0;
		virtual Size sizeHint(DrawingContext &dc) = 0;
		virtual bool isWidget() = 0;
};
