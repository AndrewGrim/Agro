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
            LineEdit* setText(std::string text);
            std::string text();
            uint borderWidth();
            LineEdit* setBorderWidth(uint border_width);
            uint padding();
            LineEdit* setPadding(uint padding);

        protected:
            float m_cursor_position = this->padding() + (this->borderWidth() / 2);
            uint m_last_mouse_x = 0;
            uint m_cursor_index = 0;
            bool m_process_mouse_event = false;
            std::string m_text;
            uint m_border_width = 2;
            uint m_padding = 10;
    };
#endif