#ifndef LABEL_HPP
    #define LABEL_HPP

    #include <string>

    #include "widget.hpp"

    class Label : public Widget {
        public:
            Label(std::string text);
            ~Label();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            std::string text();
            virtual Label* setText(std::string text);
            HorizontalAlignment horizontalAlignment();
            Label* setHorizontalAlignment(HorizontalAlignment text_align);
            VerticalAlignment verticalAlignment();
            Label* setVerticalAlignment(VerticalAlignment text_align);

            std::string m_text;
            HorizontalAlignment m_horizontal_align = HorizontalAlignment::Left;
            VerticalAlignment m_vertical_align = VerticalAlignment::Center;
            i32 m_line_spacing = 5;
    };
#endif
