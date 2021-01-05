#include "lineedit.hpp"

LineEdit::LineEdit(std::string text) : Widget() {
    Widget::m_bg = Color(1, 1, 1);
    this->setText(text);
    this->onMouseDown = [&](MouseEvent event) {
        if (!this->text().length()) {
            this->m_cursor_position = this->padding() + (this->borderWidth() / 2);
        } else {
            this->m_last_mouse_x = event.x;
            this->m_process_mouse_event = true;
        }
    };
}

LineEdit::~LineEdit() {

}

const char* LineEdit::name() {
    return "LineEdit";
}

void LineEdit::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;

    dc->fillRect(
        rect, 
        m_fg
    );
    // resize rectangle to account for border
    rect = Rect(
        rect.x + this->m_border_width, 
        rect.y + this->m_border_width, 
        rect.w - this->m_border_width * 2, 
        rect.h - this->m_border_width * 2
    );

    dc->fillRect(rect, this->background());
    dc->fillTextAligned(
        this->font() ? this->font() : dc->default_font, 
        this->text(), 
        HorizontalAlignment::Left, 
        VerticalAlignment::Center, 
        rect,
        this->m_padding,
        m_fg
    );

    if (this->m_process_mouse_event) {
        float x = this->padding() + this->borderWidth() / 2;
        unsigned int index = 0;
        for (char c : this->text()) {
            float w = dc->measureText(this->font() ? this->font() : dc->default_font, c).w;
            if (x + w > (rect.x * -1) + this->m_last_mouse_x) {
                break;
            }
            x += w;
            index++;
        }
        this->m_cursor_position = x;
        this->m_process_mouse_event = false;
        this->m_cursor_index = index;
    }
    if (this->isFocused()) {
        float text_height = (float)(this->font() ? this->font()->max_height : dc->default_font->max_height);
        text_height += m_padding;
        dc->fillRect(
            Rect(
                rect.x + this->m_cursor_position, 
                rect.y + (rect.h / 2) - (text_height / 2), 
                1, 
                text_height
            ), 
            m_fg
        );
    }
}

Size LineEdit::sizeHint(DrawingContext *dc) {
    if (this->m_size_changed) {
        Size size = dc->measureText(this->font() ? this->font() : dc->default_font, text());
        // Note: full padding is applied on both sides but
        // border width is divided in half for each side
        size.w += this->m_padding * 2 + this->m_border_width;
        size.h += this->m_padding * 2 + this->m_border_width;

        this->m_size = size;
        this->m_size_changed = false;

        return size;
    } else {
        return this->m_size;
    }
}

std::string LineEdit::text() {
    return this->m_text;
}

LineEdit* LineEdit::setText(std::string text) {
    this->m_text = text;
    this->m_size_changed = true;
    this->update();
    this->layout();

    return this;
}

uint LineEdit::borderWidth() {
    return this->m_border_width;
}

LineEdit* LineEdit::setBorderWidth(uint border_width) {
    if (m_border_width != border_width) {
        m_border_width = border_width;
        this->m_size_changed = true;
        this->update();
        this->layout();
    }

    return this;
}

uint LineEdit::padding() {
    return this->m_padding;
}

LineEdit* LineEdit::setPadding(uint padding) {
    if (m_padding != padding) {
        m_padding = padding;
        this->m_size_changed = true;
        this->update();
        this->layout();
    }
    
    return this;
}