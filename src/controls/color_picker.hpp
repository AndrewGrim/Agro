#pragma once

#include "widget.hpp"
#include "label.hpp"
#include "text_edit.hpp"

#define COLOR_PICKER_WIDTH 300
#define COLOR_PICKER_HEIGHT 200

class ColorPicker : public Widget {
    public:
        EventListener<Widget*, Color> onColorChanged = EventListener<Widget*, Color>();

        ColorPicker();
        ~ColorPicker();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
        virtual Size sizeHint(DrawingContext &dc) override;
        bool isLayout() override;
        i32 isFocusable() override;
        Widget* handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) override;
        Color color();

        i32 m_cursor_width = 10;
        Color m_color = COLOR_NONE;
        Point m_position = Point(0, 0);
        TextureCoordinates m_coords;
        TextEdit *m_color_edit = nullptr;
        Label *m_color_label = nullptr;
        f32 *m_texture_data = new f32[COLOR_PICKER_HEIGHT * COLOR_PICKER_WIDTH * 4];
};
