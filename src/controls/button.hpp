#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include <string>

    #include "widget.hpp"
    #include "label.hpp"
    #include "image.hpp"

    class Button : public Widget {
        public:
            Button(std::string text);
            Button(Image *image);
            ~Button();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual Button* setBackground(Color background) override;
            virtual Button* setForeground(Color foreground) override;
            std::string text();
            Button* setText(std::string text);
            Image* image(); 
            Button* setImage(Image *image);
            HorizontalAlignment horizontalAlignment();
            Button* setHorizontalAlignment(HorizontalAlignment text_align);
            VerticalAlignment verticalAlignment();
            Button* setVerticalAlignment(VerticalAlignment text_align);
            unsigned int padding();
            Button* setPadding(unsigned int padding);
            unsigned int borderWidth();
            Button* setBorderWidth(unsigned int border_width);

        protected:
            std::string m_text = "";
            HorizontalAlignment m_horizontal_align = HorizontalAlignment::Center;
            VerticalAlignment m_vertical_align = VerticalAlignment::Center;
            unsigned int m_padding = 5;
            unsigned int m_border_width = 4;
            Image *m_image = nullptr;
    };
#endif
