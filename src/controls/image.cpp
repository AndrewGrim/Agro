#include "image.hpp"

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
    dc->drawImage(
        rect.x + (rect.w / 2) - Texture::width / 2,
        rect.y + (rect.h / 2) - Texture::height / 2, this,
        Widget::m_fg
    );
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
