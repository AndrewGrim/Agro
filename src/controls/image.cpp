#include "image.hpp"
#include "../common/point.hpp"

Image::Image(std::string file_path) : Texture(file_path) {
    Widget::m_fg = Color(1, 1, 1, 1);
    Widget::m_bg = Color(0, 0, 0, 0);
}

Image::~Image() {
}

const char* Image::name() {
    return "Image";
}

void Image::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    dc->fillRect(rect, Widget::m_bg);
    if (this->m_expand) {
        // TODO add option to maintan aspect ratio
        // basically check whats bigger rect.w rect.h, and use the smaller one as size for both of them
        dc->drawImageAtSize(
            rect.x,
            rect.y, 
            Size(rect.w, rect.h),
            this,
            Widget::m_fg
        );
    } else {
        Point pos = Point();
        switch (this->m_horizontal_align) {
            case HorizontalAlignment::Left:
                pos.x = rect.x;
                break;
            case HorizontalAlignment::Right:
                pos.x = rect.x + rect.w - Texture::width;
                break;
            case HorizontalAlignment::Center:
                pos.x = rect.x + (rect.w / 2) - Texture::width / 2;
                break;
        }
        switch (this->m_vertical_align) {
            case VerticalAlignment::Top:
                pos.y = rect.y;
                break;
            case VerticalAlignment::Bottom:
                pos.y = rect.y + rect.h - Texture::height;
                break;
            case VerticalAlignment::Center:
                pos.y = rect.y + (rect.h / 2) - Texture::height / 2;
                break;
        }
        dc->drawImage(
            pos.x,
            pos.y, 
            this,
            Widget::m_fg
        );
    }
}

Size Image::sizeHint(DrawingContext *dc) {
    return Size(Texture::width, Texture::height);
}

Image* Image::setBackground(Color background) {
    if (Widget::m_bg != background) {
        Widget::m_bg = background;
        this->update();
    }

    return this;
}

Image* Image::setForeground(Color foreground) {
    if (Widget::m_fg != foreground) {
        Widget::m_fg = foreground;
        this->update();
    }

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

