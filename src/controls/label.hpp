#ifndef LABEL_HPP
    #define LABEL_HPP

    #include <string>

    #include "widget.hpp"
    #include "../common/align.hpp"
    #include "../renderer/font.hpp"

    class Label : public Widget {
        public:
            Label(std::string text, TextAlignment text_align = TextAlignment::Left);
            ~Label();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual Label* setBackground(Color background) override;
            virtual Label* setForeground(Color foreground) override;
            std::string text();
            virtual Label* setText(std::string text);
            TextAlignment textAlignment();
            Label* setTextAlignment(TextAlignment text_align);

        protected:
            std::string m_text;
            TextAlignment m_text_align;
    };
#endif
