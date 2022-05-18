#ifndef LABEL_HPP
    #define LABEL_HPP

    #include "widget.hpp"
    #include "../core/string.hpp"

    class Label : public Widget {
        public:
            Label(String text);
            ~Label();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            String text();
            virtual Label* setText(String text);
            HorizontalAlignment horizontalAlignment();
            Label* setHorizontalAlignment(HorizontalAlignment text_align);
            VerticalAlignment verticalAlignment();
            Label* setVerticalAlignment(VerticalAlignment text_align);

            String m_text;
            HorizontalAlignment m_horizontal_align = HorizontalAlignment::Left;
            VerticalAlignment m_vertical_align = VerticalAlignment::Center;
            i32 m_line_spacing = 5;
    };
#endif
