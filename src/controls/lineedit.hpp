#ifndef LINEEDIT_HPP
    #define LINEEDIT_HPP

    #include <string>

    #include "widget.hpp"

    class LineEdit : public Widget {
        public:
            LineEdit(std::string text = "");
            ~LineEdit();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual void handleTextEvent(DrawingContext *dc, const char *text);
            LineEdit* setText(std::string text);
            std::string text();
            uint borderWidth();
            LineEdit* setBorderWidth(uint border_width);
            uint padding();
            LineEdit* setPadding(uint padding);
            float minLength();
            LineEdit* setMinLength(float length);
            LineEdit* moveCursorLeft(DrawingContext *dc);
            LineEdit* moveCursorRight(DrawingContext *dc);

        protected:
            float m_cursor_position = this->padding() + (this->borderWidth() / 2);
            uint m_last_mouse_x = 0;
            uint m_cursor_index = 0;
            bool m_process_mouse_event = false;
            std::string m_text;
            uint m_border_width = 2;
            uint m_padding = 10;
            float m_min_length = 50;
            float m_min = 0.0;
            float m_max = 1.0;
            float m_value = m_min;
            Size m_virtual_size = Size();
            bool m_text_changed = false;
    };
#endif