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
            unsigned int borderWidth();
            LineEdit* setBorderWidth(unsigned int border_width);
            unsigned int padding();
            LineEdit* setPadding(unsigned int padding);
            float minLength();
            LineEdit* setMinLength(float length);
            LineEdit* moveCursorLeft();
            LineEdit* moveCursorRight();
            LineEdit* moveCursorBegin();
            LineEdit* moveCursorEnd();
            LineEdit* deleteAt(int index);
            LineEdit* clear();
            LineEdit* setPlaceholderText(std::string text);
            std::string placeholderText();
            LineEdit* updateView();
            LineEdit* jumpWordLeft();
            LineEdit* jumpWordRight();

        protected:
            float m_cursor_position = this->padding() + (this->borderWidth() / 2);
            unsigned int m_last_mouse_x = 0;
            unsigned int m_cursor_index = 0;
            bool m_process_mouse_event = false;
            std::string m_text;
            std::string m_placeholder_text;
            unsigned int m_border_width = 2;
            unsigned int m_padding = 10;
            float m_min_length = 50;
            float m_min_view = 0.0;
            float m_max_view = 1.0;
            float m_current_view = m_min_view;
            Size m_virtual_size = Size();
            bool m_text_changed = false;
    };
#endif