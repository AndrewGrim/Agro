#pragma once

#include "widget.hpp"

class ColorPicker : public Widget {
    public:
        std::function<void(Widget *widget, Color color)> onColorChanged = nullptr;

        ColorPicker();
        ~ColorPicker();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, int state) override;
        virtual Size sizeHint(DrawingContext &dc);
        Color color();

        Color m_color = COLOR_NONE;
        Point m_position;
        float m_cursor_width = 10;
};
