#include "image.hpp"

Image::Image(std::string file_path) : Widget() {
    m_texture = std::make_shared<Texture>(file_path);
    m_size = originalSize();
    style.widget_background = COLOR_NONE;
}

Image::Image(const unsigned char *image_data, i32 length) : Widget() {
    m_texture = std::make_shared<Texture>(image_data, length);
    m_size = originalSize();
    style.widget_background = COLOR_NONE;
}

Image::Image(std::shared_ptr<Texture> texture) : Widget() {
    m_texture = texture;
    m_size = originalSize();
    style.widget_background = COLOR_NONE;
}

Image::~Image() {

}

const char* Image::name() {
    return "Image";
}

void Image::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;

    dc.fillRect(rect, dc.widgetBackground(style));
    if (m_expand) {
        if (m_maintain_aspect_ratio) {
            Size size = Size();
            if (rect.w > rect.h) {
                size.w = 1; // Expand based on width.
            } else if (rect.h > rect.w) {
                size.h = 1; // Expand based on height.
            }
            if (size.w) { // More space horizontally.
                size.w = rect.h;
                size.h = size.w / (size.w / rect.h);
            } else if (size.h) { // More space vertically.
                size.h = rect.w;
                size.w = size.h / (size.h / rect.w);
            } else {
                size = Size(rect.w < rect.h ? rect.w : rect.h, rect.h < rect.w ? rect.h : rect.w);
            }
            dc.drawTextureAligned(
                rect,
                size,
                _texture(),
                coords(),
                m_horizontal_align,
                m_vertical_align,
                foreground()
            );
        } else {
            dc.drawTextureAligned(
                rect,
                Size(rect.w, rect.h),
                _texture(),
                coords(),
                m_horizontal_align,
                m_vertical_align,
                foreground()
            );
        }
    } else {
        dc.drawTextureAligned(
            rect,
            size(),
            _texture(),
            coords(),
            m_horizontal_align,
            m_vertical_align,
            foreground()
        );
    }
}

Size Image::sizeHint(DrawingContext &dc) {
    return m_size;
}

Image* Image::setMinSize(Size min_size) {
    m_size = min_size;
    layout();
    return this;
}

Size Image::originalSize() {
    return Size(m_texture->width, m_texture->height);
}

Color Image::foreground() {
    return m_foreground;
}

Image* Image::setForeground(Color foreground) {
    m_foreground = foreground;
    update();

    return this;
}

bool Image::expand() {
    return m_expand;
}

Image* Image::setExpand(bool expand) {
    if (m_expand != expand) {
        m_expand = expand;
        update();
    }

    return this;
}

HorizontalAlignment Image::horizontalAlignment() {
    return m_horizontal_align;
}

Image* Image::setHorizontalAlignment(HorizontalAlignment image_align) {
    if (m_horizontal_align != image_align) {
        m_horizontal_align = image_align;
        update();
    }

    return this;
}

VerticalAlignment Image::verticalAlignment() {
    return m_vertical_align;
}

Image* Image::setVerticalAlignment(VerticalAlignment image_align) {
    if (m_vertical_align != image_align) {
        m_vertical_align = image_align;
        update();
    }

    return this;
}

bool Image::maintainAspectRation() {
    return m_maintain_aspect_ratio;
}

Image* Image::setMaintainAspectRatio(bool aspect_ratio) {
    if (m_maintain_aspect_ratio != aspect_ratio) {
        m_maintain_aspect_ratio = aspect_ratio;
        update();
    }

    return this;
}

Image* Image::flipHorizontally() {
    m_coords.flipHorizontally();

    return this;
}

Image* Image::flipVertically() {
    m_coords.flipVertically();

    return this;
}

Image* Image::flipBoth() {
    m_coords.flipBoth();

    return this;
}

Image* Image::clockwise90() {
    m_coords.clockwise90();

    return this;
}

Image* Image::counterClockwise90() {
    m_coords.counterClockwise90();

    return this;
}

void Image::setTexture(std::shared_ptr<Texture> texture) {
    m_texture = texture;
    update();
}

void Image::resetOrientation() {
    m_coords = TextureCoordinates();
}

TextureCoordinates* Image::coords() {
    return &m_coords;
}

std::shared_ptr<Texture> Image::texture() {
    return m_texture;
}

Texture* Image::_texture() {
    return m_texture.get();
}
