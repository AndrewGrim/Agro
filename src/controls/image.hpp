#ifndef IMAGE_HPP
    #define IMAGE_HPP

    #include <string>
    #include <memory>

    #include "widget.hpp"
    #include "../renderer/texture.hpp"

    class Image : public Widget {
        public:
            Image(std::string file_path);
            Image(const unsigned char *image_data, int length);
            Image(std::shared_ptr<Texture> texture);
            ~Image();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            Color foreground();
            Image* setForeground(Color foreground);
            bool expand();
            Image* setExpand(bool expand);
            HorizontalAlignment horizontalAlignment();
            Image* setHorizontalAlignment(HorizontalAlignment image_align);
            VerticalAlignment verticalAlignment();
            Image* setVerticalAlignment(VerticalAlignment image_align);
            bool maintainAspectRation();
            Image* setMaintainAspectRatio(bool aspect_ratio);
            Image* flipHorizontally();
            Image* flipVertically();
            Image* flipBoth();
            Image* clockwise90();
            Image* counterClockwise90();
            void resetOrientation();
            Image* setMinSize(Size min_size);
            Size originalSize();
            void setTexture(std::shared_ptr<Texture> texture);
            TextureCoordinates* coords();
            std::shared_ptr<Texture> texture();
            Texture* _texture();

            Color m_foreground = COLOR_WHITE;
            std::shared_ptr<Texture> m_texture;
            bool m_expand = false;
            bool m_maintain_aspect_ratio = true;
            HorizontalAlignment m_horizontal_align = HorizontalAlignment::Center;
            VerticalAlignment m_vertical_align = VerticalAlignment::Center;
            TextureCoordinates m_coords;
    };
#endif
