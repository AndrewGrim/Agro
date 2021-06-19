#pragma once

#include "widget.hpp"
#include "label.hpp"
#include "line_edit.hpp"

#define COLOR_PICKER_LENGTH 260.0f

class ColorPicker : public Widget {
    public:
        EventListener<Widget*, Color> onColorChanged = EventListener<Widget*, Color>();

        ColorPicker();
        ~ColorPicker();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, int state) override;
        virtual Size sizeHint(DrawingContext &dc);
        bool isLayout() override;
        Color color();

        float m_cursor_width = 10;
        Color m_color = COLOR_NONE;
        Point m_position = Point(-m_cursor_width, -m_cursor_width);
        TextureCoordinates m_coords;
        LineEdit *m_color_edit = nullptr;
        Label *m_color_label = nullptr;
        float *m_texture_data = new float[(int)(COLOR_PICKER_LENGTH * COLOR_PICKER_LENGTH * 4)];
};
