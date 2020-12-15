#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include <string>

    #include "widget.hpp"
    #include "../common/align.hpp"
    #include "../renderer/text_renderer.hpp"

    class Button : public Widget {
        public:
            Button(std::string text);
            ~Button();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual Button* setBackground(Color background) override;
            virtual Button* setForeground(Color background) override;
            std::string text();
            virtual Button* setText(std::string text);
            TextAlignment textAlignment();
            void setTextAlignment(TextAlignment text_align);

        protected:
            std::string m_text;
            TextAlignment m_text_align = TextAlignment::Center; // needs a setter with both update and layout
            unsigned int m_padding = 10;
            unsigned int m_border_width = 4;
    };
#endif
