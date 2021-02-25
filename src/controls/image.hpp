#ifndef IMAGE_HPP
    #define IMAGE_HPP

    #include <string>
    #include <memory>

    #include "widget.hpp"
    #include "../renderer/texture.hpp"

    struct TextureCoordinates {
        Point top_left = Point(0.0, 1.0);
        Point bottom_left = Point(0.0, 0.0);
        Point bottom_right = Point(1.0, 0.0);
        Point top_right = Point(1.0, 1.0);

        TextureCoordinates() {

        }

        void flipHorizontally() {
            // if flipped reset to default
            if (top_left.x) {
                top_left.x = 0.0;
                bottom_left.x = 0.0;
                bottom_right.x = 1.0;
                top_right.x = 1.0;
            // flip x to the opposite side
            } else {
                top_left.x = 1.0;
                bottom_left.x = 1.0;
                bottom_right.x = 0.0;
                top_right.x = 0.0;
            }
        }

        void flipVertically() {
            // if flipped reset to default
            if (!top_left.y) {
                top_left.y = 1.0;
                bottom_left.y = 0.0;
                bottom_right.y = 0.0;
                top_right.y = 1.0;
            // flip y to the opposite side
            } else {
                top_left.y = 0.0;
                bottom_left.y = 1.0;
                bottom_right.y = 1.0;
                top_right.y = 0.0;
            }
        }

        void flipBoth() {
            this->flipHorizontally();
            this->flipVertically();
        }
    };

    class Image : public Widget {
        public:
            Image(std::string file_path);
            // TODO actually check if we need the from_memory bool
            // its there to disambiguate ctor overload
            Image(bool from_memory, const unsigned char *image_data, int length);
            Image(std::shared_ptr<Texture> texture);
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
            bool maintainAspectRation();
            Image* setMaintainAspectRatio(bool aspect_ratio);
            Image* flipHorizontally();
            Image* flipVertically();
            Image* flipBoth();
            void resetOrientation();
            void setMinSize(Size min_size);
            Size originalSize();
            void setTexture(std::shared_ptr<Texture> texture);
            TextureCoordinates* coords();
            std::shared_ptr<Texture> texture();
            Texture* _texture();

        protected:
            std::shared_ptr<Texture> m_texture;
            bool m_expand = false;
            bool m_maintain_aspect_ratio = true;
            HorizontalAlignment m_horizontal_align = HorizontalAlignment::Center;
            VerticalAlignment m_vertical_align = VerticalAlignment::Center;
            Size m_min_size;
            TextureCoordinates m_coords;     
    };
#endif
