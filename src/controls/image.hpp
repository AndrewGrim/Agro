#ifndef IMAGE_HPP
    #define IMAGE_HPP

    #include <string>

    #include "widget.hpp"
    #include "../renderer/texture.hpp"

    class Image : public Widget, public Texture {
        public:
            Image(std::string file_path);
            ~Image();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual Image* setBackground(Color background) override;
            virtual Image* setForeground(Color background) override;
            bool expand();
            Image* setExpand(bool expand);
            HorizontalAlignment horizontalAlignment();
            Image* setHorizontalAlignment(HorizontalAlignment image_align);
            VerticalAlignment verticalAlignment();
            Image* setVerticalAlignment(VerticalAlignment image_align);

        protected:
            bool m_expand = false;
            HorizontalAlignment m_horizontal_align = HorizontalAlignment::Center;
            VerticalAlignment m_vertical_align = VerticalAlignment::Center;
    };
#endif
