#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include "../core/string.hpp"

    #include "widget.hpp"
    #include "label.hpp"
    #include "image.hpp"

    class Button : public Widget {
        public:
            Button(String text);
            Button(Image *image);
            ~Button();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            String text();
            Button* setText(String text);
            Image* image();
            Button* setImage(Image *image);
            HorizontalAlignment horizontalAlignment();
            Button* setHorizontalAlignment(HorizontalAlignment text_align);
            VerticalAlignment verticalAlignment();
            Button* setVerticalAlignment(VerticalAlignment text_align);
            i32 isFocusable() override;

            String m_text = "";
            HorizontalAlignment m_horizontal_align = HorizontalAlignment::Center;
            VerticalAlignment m_vertical_align = VerticalAlignment::Center;
            Image *m_image = nullptr;
    };
#endif
