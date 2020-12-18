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

        protected:
            bool m_expand = false;
    };
#endif
