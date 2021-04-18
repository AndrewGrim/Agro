#include "label.hpp"

Label::Label(std::string text) {
    this->setText(text);
    Widget::m_bg = COLOR_NONE;
}

Label::~Label() {
}

const char* Label::name() {
    return "Label";
}

void Label::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;

    Rect old_clip = dc->clip();
    dc->setClip(rect.clipTo(old_clip));
    // We pretend that the label always gets enough space
    // but we still clip to the space that we were given.
    Rect drawing_rect = Rect(
        rect.x, 
        rect.y, 
        sizeHint(dc).w > rect.w ? sizeHint(dc).w : rect.w, 
        sizeHint(dc).h > rect.h ? sizeHint(dc).h : rect.h
    );
    dc->fillRect(drawing_rect, Widget::m_bg);
    dc->fillTextAligned(
        this->font() ? this->font() : dc->default_font,
        this->m_text,
        this->m_horizontal_align,
        this->m_vertical_align,
        drawing_rect,
        0,
        this->m_fg
    );
    dc->setClip(old_clip);
}

Size Label::sizeHint(DrawingContext *dc) {
    if (this->m_size_changed) {
        Size size = dc->measureText(this->font() ? this->font() : dc->default_font, this->text());
        this->m_size = size;
        this->m_size_changed = false;

        return size;
    } else {
        return this->m_size;
    }
}

Label* Label::setForeground(Color foreground) {
    Widget::m_fg = foreground;
    this->update();

    return this;
}

Label* Label::setBackground(Color background) {
    Widget::m_bg = background;
    this->update();

    return this;
}

std::string Label::text() {
    return this->m_text;
}

Label* Label::setText(std::string text) {
    // Unlike everywhere else I don't think we
    // should compare the m_text to text since
    // that can be pretty expensive and we can
    // assume that if someone is setting the text
    // its going to be different.
    this->m_text = text;
    this->layout();

    return this;
}

HorizontalAlignment Label::horizontalAlignment() {
    return this->m_horizontal_align;
}

Label* Label::setHorizontalAlignment(HorizontalAlignment text_align) {
    if (m_horizontal_align != text_align) {
        m_horizontal_align = text_align;
        this->update();
    }

    return this;
}

VerticalAlignment Label::verticalAlignment() {
    return this->m_vertical_align;
}

Label* Label::setVerticalAlignment(VerticalAlignment text_align) {
    if (m_vertical_align != text_align) {
        m_vertical_align = text_align;
        this->update();
    }

    return this;
}
