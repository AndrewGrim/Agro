#include "image.hpp"

Image::Image(std::string file_path) : Widget() {
    Widget::m_fg = COLOR_WHITE;
    Widget::m_bg = COLOR_NONE;
    m_texture = std::make_shared<Texture>(file_path);
    m_size = Size(m_texture->width, m_texture->height);
}

Image::Image(bool from_memory, const unsigned char *image_data, int length) : Widget() {
    Widget::m_fg = COLOR_WHITE;
    Widget::m_bg = COLOR_NONE;
    m_texture = std::make_shared<Texture>(from_memory, image_data, length);
    m_size = Size(m_texture->width, m_texture->height);
}

// TODO not to sure about the order and defaultness
// of owning here, think about it
Image::Image(std::shared_ptr<Texture> texture) {
    Widget::m_fg = COLOR_WHITE;
    Widget::m_bg = COLOR_NONE;
    m_texture = texture;
    m_size = Size(m_texture->width, m_texture->height);
}

Image::~Image() {

}

const char* Image::name() {
    return "Image";
}

void Image::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    // We pretend that the widget always gets enough space.
    // This allows us to draw the widget exactly how its meant to look.
    // Not explicitly clipping should not be a problem because
    // as long as there is a scrollable somewhere in the tree everything will get its
    // requested size and otherwise if its boxes all the way up
    // you only wont get enough space if the widget is going to go outside the window.
    Rect dr = Rect(
        rect.x, 
        rect.y, 
        sizeHint(dc).w > rect.w ? sizeHint(dc).w : rect.w, 
        sizeHint(dc).h > rect.h ? sizeHint(dc).h : rect.h
    );
    dc->fillRect(dr, Widget::m_bg);
    if (m_expand) {
        if (m_maintain_aspect_ratio) {
            Size size = Size();
            if (dr.w > dr.h) {
                size.w = 1;
            } else if (dr.h > dr.w) {
                size.h = 1;
            }
            if (size.w) {
                size.w = dr.h;
                size.h = size.w / (size.w / dr.h);
            } else if (size.h) {
                size.h = dr.w;
                size.w = size.h / (size.h / dr.w);
            } else {
                size = Size(dr.w < dr.h ? dr.w : dr.h, dr.h < dr.w ? dr.h : dr.w);
            }
            dc->drawTextureAligned(
                dr,
                size,
                this->_texture(),
                this->coords(),
                m_horizontal_align,
                m_vertical_align,
                Widget::m_fg
            );
        } else {
            dc->drawTextureAligned(
                dr,
                Size(dr.w, dr.h),
                this->_texture(),
                this->coords(),
                m_horizontal_align,
                m_vertical_align,
                Widget::m_fg
            );
        }
    } else {
        dc->drawTextureAligned(
            dr,
            m_size,
            this->_texture(),
            this->coords(),
            m_horizontal_align,
            m_vertical_align,
            Widget::m_fg
        );
    }
}

Size Image::sizeHint(DrawingContext *dc) {
    return m_size;
}

void Image::setMinSize(Size min_size) {
    m_size = min_size;
    layout();
}

Size Image::originalSize() {
    return Size(m_texture->width, m_texture->height);
}

Image* Image::setBackground(Color background) {
    Widget::m_bg = background;
    this->update();

    return this;
}

Image* Image::setForeground(Color foreground) {
    Widget::m_fg = foreground;
    this->update();

    return this;
}

bool Image::expand() {
    return this->m_expand;
}

Image* Image::setExpand(bool expand) {
    if (this->m_expand != expand) {
        this->m_expand = expand;
        this->update();
    }

    return this;
}

HorizontalAlignment Image::horizontalAlignment() {
    return this->m_horizontal_align;
}

Image* Image::setHorizontalAlignment(HorizontalAlignment image_align) {
    if (m_horizontal_align != image_align) {
        m_horizontal_align = image_align;
        this->update();
    }

    return this;
}

VerticalAlignment Image::verticalAlignment() {
    return this->m_vertical_align;
}

Image* Image::setVerticalAlignment(VerticalAlignment image_align) {
    if (m_vertical_align != image_align) {
        m_vertical_align = image_align;
        this->update();
    }

    return this;
}

bool Image::maintainAspectRation() {
    return this->m_maintain_aspect_ratio;
}

Image* Image::setMaintainAspectRatio(bool aspect_ratio) {
    if (m_maintain_aspect_ratio != aspect_ratio) {
        m_maintain_aspect_ratio = aspect_ratio;
        this->update();
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