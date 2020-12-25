#include "image.hpp"
#include "../common/point.hpp"

Image::Image(std::string file_path, bool expand) : Widget(), Texture(file_path) {
    Widget::m_fg = Color(1, 1, 1, 1);
    Widget::m_bg = Color(0, 0, 0, 0);
    this->setExpand(expand);
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
        if (m_maintain_aspect_ratio) {
            Size size = Size();
            if (rect.w > rect.h) {
                size.w = 1;
            } else if (rect.h > rect.w) {
                size.h = 1;
            }
            if (size.w) {
                size.w = rect.h;
                size.h = size.w / (size.w / rect.h);
            } else if (size.h) {
                size.h = rect.w;
                size.w = size.h / (size.h / rect.w);
            } else {
                size = Size(rect.w < rect.h ? rect.w : rect.h, rect.h < rect.w ? rect.h : rect.w);
            }
            dc->drawImageAlignedAtSize(
                rect,
                this->m_horizontal_align,
                this->m_vertical_align,
                size,
                this,
                Widget::m_fg
            );
        } else {
            dc->drawImageAlignedAtSize(
                rect,
                this->m_horizontal_align,
                this->m_vertical_align,
                Size(rect.w, rect.h),
                this,
                Widget::m_fg
            );
        }
    } else {
        dc->drawImageAligned(
            rect,
            this->m_horizontal_align,
            this->m_vertical_align,
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
